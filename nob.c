#include "cnotes.h"
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-g", "-Wall", "-Wextra", "-std=c99")

#define NOB_IMPLEMENTATION
#include "nob.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Drop in arena implementation.
typedef struct arena {
    uint64_t capacity;
    void *allocation;
    void *ptr;
} Arena;

Arena arena_make(uint64_t capacity) {
    void *mem = malloc(capacity);

    if (mem == NULL) {
        fprintf(stderr, "Couldn't malloc %lu bytes of memory for the arena.", capacity);
        return (Arena) {0};
    }

    return (Arena) {
        .capacity = capacity,
        .allocation = mem,
        .ptr = mem,
    };
}

void *arena_alloc(Arena *arena, uint64_t size) {
    arena->ptr += size;

    if (arena->ptr > arena->allocation + arena->capacity) {
        fprintf(stderr, "Couldn't allocate %lu bytes of memory from the arena, this allocation exceeded arena's capacity.", size);
        return NULL;
    }

    return arena->ptr - size;
}

/**
 * Copies sv into arena, so it outlives the buffer it points to. Empty view if arena is out of memory.
 */
Nob_String_View arena_sv_dup(Arena *arena, Nob_String_View sv) {
    if (sv.count == 0) return (Nob_String_View) {0};

    char *data = arena_alloc(arena, sv.count);
    if (data == NULL) return (Nob_String_View) {0};

    memcpy(data, sv.data, sv.count);
    return nob_sv_from_parts(data, sv.count);
}

uint64_t arena_size(Arena *arena) {
    return arena->ptr - arena->allocation;
}

void arena_clear(Arena *arena) {
    arena->ptr = arena->allocation;
}

void arena_free(Arena *arena) {
    free(arena->allocation);

    *arena = (Arena) {0};
}

//
// Async proc test setup to detect timeout and report.
//
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <errno.h>
#  include <signal.h>
#  include <string.h>
#  include <sys/wait.h>
#  include <time.h>
#  include <unistd.h>
#  include <signal.h>
#endif // _WIN32

#define TEST_TIMEOUT_MS 2000

typedef struct {
#ifdef _WIN32
    HANDLE proc;
    HANDLE job;
#else
    pid_t pid;   // doubles as the process group id
#endif // _WIN32
} Build_Proc;

typedef enum { 
    PROC_OK, 
    PROC_FAIL, 
    PROC_TIMEOUT 
} Proc_Result;

bool        build_proc_spawn(Build_Proc *p, Nob_Cmd *cmd, const char *stdout_path);
Proc_Result build_proc_wait(Build_Proc *p, unsigned timeout_ms);

#ifdef _WIN32
bool build_proc_spawn(Build_Proc *p, Nob_Cmd *cmd, const char *stdout_path) {
    if (cmd->count < 1) {
        nob_log(NOB_ERROR, "build proc: could not run empty command");
        return false;
    }

    Nob_Fd fdout = NOB_INVALID_FD;
    if (stdout_path != NULL) {
        fdout = nob_fd_open_for_write(stdout_path);
        if (fdout == NOB_INVALID_FD) return false;
    }

    HANDLE job = CreateJobObjectA(NULL, NULL);
    if (job == NULL) {
        nob_log(NOB_ERROR, "build proc: could not create job object: %lu", GetLastError());
        if (fdout != NOB_INVALID_FD) nob_fd_close(fdout);
        return false;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {0};
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
        nob_log(NOB_ERROR, "build proc: could not configure job object: %lu", GetLastError());
        CloseHandle(job);
        if (fdout != NOB_INVALID_FD) nob_fd_close(fdout);
        return false;
    }

    STARTUPINFOA si = {0};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = (fdout != NOB_INVALID_FD) ? fdout : GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);

    Nob_String_Builder sb = {0};
    nob_cmd_render(*cmd, &sb);
    nob_sb_append_null(&sb);
    nob_log(NOB_INFO, "CMD: %s", sb.items);

    PROCESS_INFORMATION pi = {0};
    BOOL ok = CreateProcessA(NULL, sb.items, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    nob_sb_free(sb);
    if (fdout != NOB_INVALID_FD) nob_fd_close(fdout);
    cmd->count = 0;

    if (!ok) {
        nob_log(NOB_ERROR, "build proc: could not create child process: %lu", GetLastError());
        CloseHandle(job);
        return false;
    }

    if (!AssignProcessToJobObject(job, pi.hProcess)) {
        nob_log(NOB_ERROR, "build proc: could not assign process to job: %lu", GetLastError());
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        CloseHandle(job);
        return false;
    }

    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);

    p->proc = pi.hProcess;
    p->job  = job;
    return true;
}

Proc_Result build_proc_wait(Build_Proc *p, unsigned timeout_ms) {
    Proc_Result result;

    DWORD w = WaitForSingleObject(p->proc, (DWORD)timeout_ms);
    if (w == WAIT_TIMEOUT) {
        TerminateJobObject(p->job, 1);
        WaitForSingleObject(p->proc, INFINITE);
        result = PROC_TIMEOUT;
    } else if (w == WAIT_OBJECT_0) {
        DWORD code = 0;
        if (!GetExitCodeProcess(p->proc, &code)) {
            nob_log(NOB_ERROR, "build proc: could not get process exit code: %lu", GetLastError());
            result = PROC_FAIL;
        } else if (code != 0) {
            nob_log(NOB_ERROR, "build proc: exited with code %lu", code);
            result = PROC_FAIL;
        } else {
            result = PROC_OK;
        }
    } else {
        nob_log(NOB_ERROR, "build proc: could not wait on child process: %lu", GetLastError());
        result = PROC_FAIL;
    }

    CloseHandle(p->proc);
    CloseHandle(p->job);   // KILL_ON_JOB_CLOSE reaps any stragglers
    return result;
}
#else
bool build_proc_spawn(Build_Proc *p, Nob_Cmd *cmd, const char *stdout_path) {
    if (cmd->count < 1) {
        nob_log(NOB_ERROR, "build proc: could not run empty command");
        return false;
    }

    Nob_String_Builder sb = {0};
    nob_cmd_render(*cmd, &sb);
    nob_sb_append_null(&sb);
    nob_log(NOB_INFO, "CMD: %s", sb.items);
    nob_sb_free(sb);

    Nob_Fd fdout = NOB_INVALID_FD;
    if (stdout_path != NULL) {
        fdout = nob_fd_open_for_write(stdout_path);
        if (fdout == NOB_INVALID_FD) return false;
    }

    nob_da_append(cmd, NULL);

    pid_t pid = fork();
    if (pid < 0) {
        nob_log(NOB_ERROR, "build proc: could not fork: %s", strerror(errno));
        if (fdout != NOB_INVALID_FD) nob_fd_close(fdout);
        cmd->count = 0;
        return false;
    }

    if (pid == 0) {
        setpgid(0, 0);
        if (fdout != NOB_INVALID_FD) {
            if (dup2(fdout, STDOUT_FILENO) < 0) _exit(127);
            close(fdout);
        }
        execvp(cmd->items[0], (char * const *) cmd->items);
        _exit(127);
    }

    setpgid(pid, pid);
    if (fdout != NOB_INVALID_FD) nob_fd_close(fdout);
    cmd->count = 0;
    p->pid = pid;
    return true;
}

Proc_Result build_proc_wait(Build_Proc *p, unsigned timeout_ms) {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (;;) {
        int wstatus = 0;
        pid_t w = waitpid(p->pid, &wstatus, WNOHANG);
        if (w < 0) {
            nob_log(NOB_ERROR, "build proc: could not wait on pid %d: %s", p->pid, strerror(errno));
            return PROC_FAIL;
        }
        if (w > 0) {
            kill(-p->pid, SIGKILL);   // sweep up grandchildren the test left running
            if (WIFEXITED(wstatus)) {
                int code = WEXITSTATUS(wstatus);
                if (code != 0) {
                    nob_log(NOB_ERROR, "build proc: exited with code %d", code);
                    return PROC_FAIL;
                }
                return PROC_OK;
            }
            nob_log(NOB_ERROR, "build proc: killed by signal %d", WTERMSIG(wstatus));
            return PROC_FAIL;
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = (now.tv_sec - start.tv_sec)*1000.0
                       + (now.tv_nsec - start.tv_nsec)/1e6;
        if (elapsed >= (double)timeout_ms) break;

        nanosleep(&(struct timespec){ .tv_nsec = 5*1000*1000 }, NULL);
    }

    kill(-p->pid, SIGKILL);
    waitpid(p->pid, NULL, 0);
    return PROC_TIMEOUT;
}
#endif // _WIN32

//
// Testing tools.
//

bool        is_c_file(const char *path);
const char *get_file_name(const char *path);
const char *cut_file_extension(const char *path);
bool        compare_files(const char *expected, const char *actual);

#define CSTR_BUFFER(name, len)          char name[len + 1] = {0}
#define CSTR_BUFFER_APPEND(buf, str)    NOB_ASSERT((sizeof(buf) - 1 - strlen(buf)) >= strlen(str)); strcat((buf), (str))


bool is_c_file(const char *path) {
    return strcmp(path + strlen(path) - 2, ".c") == 0;
}

const char *get_file_name(const char *path) {
    const char *name = strrchr(path, '/');
    if (name == NULL) name = path;
    else name++;
    return name;
}

const char *cut_file_extension(const char *path) {
    char *dot = strrchr(path, '.');
    if (dot == NULL) return path;

    *dot = '\0';
    return path;
}

bool compare_files(const char *expected, const char *actual) {
    Nob_String_Builder src = {0}, dst = {0};

    if (!read_entire_file(actual, &src)) {
        nob_log(NOB_ERROR, "compare: actual file not found.");
        return false;
    }

    if (!read_entire_file(expected, &dst)) {
        nob_log(NOB_ERROR, "compare: expected file not found.");
        NOB_FREE(src.items);
        return false;
    }

    Nob_String_View src_sv = sb_to_sv(src);
    Nob_String_View dst_sv = sb_to_sv(dst);

    if (!sv_eq(src_sv, dst_sv)) {
        nob_log(NOB_ERROR, "compare: expected:");
        fprintf(stderr, SV_Fmt, SV_Arg(dst_sv));
        nob_log(NOB_ERROR, "compare: actual:");
        fprintf(stderr, SV_Fmt, SV_Arg(src_sv));

        NOB_FREE(dst.items);
        NOB_FREE(src.items);
        return false;
    }

    NOB_FREE(dst.items);
    NOB_FREE(src.items);
    return true;
}


bool confirm(const char *prompt) {
    char input[10];

    while (true) {
        fprintf(stderr, "%s (y/n): ", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return false; // Treat input failure as "no".
        }

        char c = tolower(input[0]);

        if (c == 'y')
            return true;
        if (c == 'n')
            return false;

        fprintf(stderr, "Please enter 'y' or 'n'.\n");
    }
}



#define STDOUT_TXT_FILE_EXTENSION   ".stdout.txt"

#define BUILD_DIR       "build"
#define OBJ_DIR         "obj"
#define BIN_DIR         "bin"
#define SRC_DIR         "src"
#define DOC_DIR         "doc"
#define TESTS_DIR       "tests"
#define PARSE_DIR       "parse"
#define EXEC_DIR        "exec"
#define HOW_TO_DIR      "how_to"

/**
 * Executes compiler commands to produce .a file.
 * Assumes needed directories exist.
 */
void compile_static_lib(Nob_Cmd *cmd) {
#if defined(__GNUC__) || defined(__clang__)
    // GCC, Clang

    nob_write_entire_file(BUILD_DIR"/temp.c", "", 0);

    nob_cc(cmd);
    nob_cc_flags(cmd);
    // Flags to tell to compile only, define CN_IMPLEMENTATION, and include cnotes.h.
    nob_cmd_append(cmd, "-c", "-DCN_IMPLEMENTATION", "-include", "cnotes.h");
    nob_cc_inputs(cmd, BUILD_DIR"/temp.c");
    nob_cc_output(cmd, BUILD_DIR"/"OBJ_DIR"/cnotes.o");

    if (!nob_cmd_run(cmd)) {
        nob_log(NOB_ERROR, "couldn't compile cnotes object file.");
    }

    nob_cmd_append(cmd, "ar", "rcs", BIN_DIR"/libcnotes.a", BUILD_DIR"/"OBJ_DIR"/cnotes.o");
    
    if (!nob_cmd_run(cmd)) {
        nob_log(NOB_ERROR, "couldn't produce static lib.");
    }

#else
    // MSVC
    NOB_TODO("Write implementation of static library compilation for MSVC.");
#endif
}

void clean(Nob_Cmd *cmd) {
    nob_cmd_append(cmd, "rm", "-r", BUILD_DIR, BIN_DIR);
    if (!nob_cmd_run(cmd)) {
        nob_log(NOB_ERROR, "couldn't clean build dir.");
    }
}


static Arena arena_strings;





//
// Doc interface.
//

#define DOC_TAG_GEN_LIST(X)                 \
    X(RETURNS,      Returns,    returns)    \
    X(PARAM,        Param,      param)      \
    X(MEMBER,       Member,     member)     \
    X(OUTPUTS,      Outputs,    outputs)    \
    X(SINCE,        Since,      since)      \
    X(NOTE,         Note,       note)       \
    X(IMPORTANT,    Important,  important)  \
    X(AI,           AI,         ai)         \
    X(TODO,         Todo,       todo)       \
    X(LEAK,         Leak,       leak)       \
    X(TEMPORARY,    Temporary,  temporary)  \
    X(FIXME,        Fixme,      fixme)      \
    X(TEST,         Test,       test)       \

typedef enum {
#define X(K, T, m) K,
    DOC_TAG_GEN_LIST(X)
#undef X
    UNKNOWN,
} Doc_Tag_Kind;

typedef struct {
    Doc_Tag_Kind    kind;
    Nob_String_View text;
} Doc_Tag;

typedef struct {
    Doc_Tag *items;
    size_t count;
    size_t capacity;
} Doc_Tags;

#define DOC_FLAG_GEN_LIST(X)                \
    X(GNU,          0x1,        "--gnu")    \

typedef enum {
    DOC_NO_FLAGS = 0,
#define X(K, v, lit) DOC_FLAG_##K = v,
    DOC_FLAG_GEN_LIST(X)
#undef X
} Doc_Flags;

/**
 * Kinds of declarations doc scanner recognizes, see `doc_classify_declaration`.
 */
#define DOC_KIND_GEN_LIST(X)                \
    X(FUNCTION,     "function")             \
    X(MACRO,        "macro")                \
    X(STRUCT,       "struct")               \
    X(UNION,        "union")                \
    X(ENUM,         "enum")                 \
    X(TYPEDEF,      "typedef")              \
    X(VARIABLE,     "variable")             \

typedef enum {
    DOC_KIND_NONE = 0,
#define X(K, lit) DOC_KIND_##K,
    DOC_KIND_GEN_LIST(X)
#undef X
} Doc_Kind;

typedef struct {
    Nob_String_View symbol;
    Doc_Kind        kind;
    Nob_String_View module;     // Name of the "SECTION:" the entry is declared in.
    Nob_String_View decl;
    Nob_String_View summary;
    Nob_String_View description;
    Doc_Tags        tags;
    Doc_Flags       flags;
} Doc_Entry;

typedef enum {
    DOC_TARGET_NONE,
    DOC_TARGET_SUMMARY,
    DOC_TARGET_TAG,
    DOC_TARGET_DESCRIPTION,
} Doc_Target;

#define DOC_SV_LIT(literal)             ((Nob_String_View) {sizeof(literal) - 1, literal})
#define DOC_SV_UNPACK(str)              (int)((str).count), (str).data

Nob_String_View doc_trim_sv_till(Nob_String_View sv, Nob_String_View target);
Nob_String_View doc_get_sv_till(Nob_String_View sv, Nob_String_View target);
void            doc_print_json_string(FILE *f, Nob_String_View sv);
Doc_Tag_Kind    doc_sv_to_tag(Nob_String_View sv);
const char     *doc_tag_to_lit(Doc_Tag_Kind tag);
const char     *doc_kind_to_lit(Doc_Kind kind);
Doc_Flags       doc_sv_to_flag(Nob_String_View sv);
bool            doc_parse_flags(Nob_String_View symbol, Nob_String_View line, Doc_Flags *flags);
bool            doc_is_identifier_char(char c);
bool            doc_sv_is_identifier(Nob_String_View sv);
bool            doc_sv_starts_with_word(Nob_String_View sv, Nob_String_View word);
Nob_String_View doc_sv_first_identifier(Nob_String_View sv);
Nob_String_View doc_sv_last_identifier(Nob_String_View sv);
bool            doc_sv_has_library_prefix(Nob_String_View symbol);
void            doc_append_line(Nob_String_Builder *sb, size_t *count, Nob_String_View line);
void            doc_append_to_target(Doc_Entry *entry, Nob_String_Builder *text_sb, Nob_String_Builder *description_sb, Doc_Target target, Nob_String_View line);
void            doc_print_json_entry(FILE *f, Doc_Entry *entry);
Nob_String_View doc_get_next_comment(Nob_String_View *content);
Nob_String_View doc_get_section(Nob_String_View doc);
size_t          doc_skip_literal_or_comment(Nob_String_View sv, size_t i);
Nob_String_View doc_get_declaration(Nob_String_View *content);
bool            doc_classify_declaration(Nob_String_View decl, Doc_Kind *kind, Nob_String_View *symbol);
Nob_String_View doc_make_signature(Nob_String_View decl, Doc_Kind kind, Nob_String_Builder *sb);
Nob_String_View doc_get_block_comment(Nob_String_View *content);
bool            doc_write_library_json(Nob_String_View file, const char *path);
bool            doc_parse_entry(Nob_String_View symbol, Nob_String_View signature, Nob_String_View doc, Doc_Entry *entry);

Nob_String_View doc_trim_sv_till(Nob_String_View sv, Nob_String_View target) {
    while (sv.count >= target.count) {
        if (nob_sv_starts_with(sv, target)) return sv;

        sv.count--;
        sv.data++;
    }

    return (Nob_String_View) { 0, NULL };
}

Nob_String_View doc_get_sv_till(Nob_String_View sv, Nob_String_View target) {
    return (Nob_String_View) { sv.count - doc_trim_sv_till(sv, target).count, sv.data };
}

void doc_print_json_string(FILE *f, Nob_String_View sv) {
    fputc('"', f);
    for (size_t i = 0; i < sv.count; i++) {
        unsigned char c = (unsigned char)sv.data[i];
        switch (c) {
        case '"':  fputs("\\\"", f); break;
        case '\\': fputs("\\\\", f); break;
        case '\n': fputs("\\n",  f); break;
        case '\r': fputs("\\r",  f); break;
        case '\t': fputs("\\t",  f); break;
        default:
            if (c < 0x20) fprintf(f, "\\u%04x", c);
            else          fputc(c, f);
        }
    }
    fputc('"', f);
}

Doc_Tag_Kind doc_sv_to_tag(Nob_String_View sv) {
#define X(K, T, m) if (nob_sv_eq(sv, DOC_SV_LIT(#K))) return K;
    DOC_TAG_GEN_LIST(X)
#undef X
    return UNKNOWN;
}

const char *doc_tag_to_lit(Doc_Tag_Kind tag) {
    switch (tag) {
#define X(K, T, m) case K: return #K; break;
    DOC_TAG_GEN_LIST(X)
#undef X
        default:
            return "<UNKNOWN>";
    }
}

const char *doc_kind_to_lit(Doc_Kind kind) {
    switch (kind) {
#define X(K, lit) case DOC_KIND_##K: return lit;
    DOC_KIND_GEN_LIST(X)
#undef X
        default:
            return "<UNKNOWN>";
    }
}

/**
 * Maps flag literal, like "--gnu", to its flag value, DOC_NO_FLAGS if flag is unknown.
 */
Doc_Flags doc_sv_to_flag(Nob_String_View sv) {
#define X(K, v, lit) if (nob_sv_eq(sv, DOC_SV_LIT(lit))) return DOC_FLAG_##K;
    DOC_FLAG_GEN_LIST(X)
#undef X
    return DOC_NO_FLAGS;
}

/**
 * Parses whitespace separated flags of a flags line into flags.
 * Every word on the line must be a known flag, otherwise error is logged and false is returned.
 */
bool doc_parse_flags(Nob_String_View symbol, Nob_String_View line, Doc_Flags *flags) {
    bool ok = true;

    while (true) {
        line = nob_sv_trim_left(line);
        if (line.count == 0) break;

        // Chopping one word, words are separated by any whitespace.
        size_t length = 0;
        while (length < line.count && !isspace((unsigned char)line.data[length])) length++;
        Nob_String_View word = nob_sv_from_parts(line.data, length);
        nob_sv_chop_left(&line, length);

        if (!nob_sv_starts_with(word, DOC_SV_LIT("--"))) {
            nob_log(NOB_ERROR, "Non flag text '%.*s' on flags line in documentation of '%.*s', flags must be on their own line.", DOC_SV_UNPACK(word), DOC_SV_UNPACK(symbol));
            ok = false;
            continue;
        }

        Doc_Flags flag = doc_sv_to_flag(word);
        if (flag == DOC_NO_FLAGS) {
            nob_log(NOB_ERROR, "Unknown flag '%.*s' in documentation of '%.*s'.", DOC_SV_UNPACK(word), DOC_SV_UNPACK(symbol));
            ok = false;
            continue;
        }

        *flags |= flag;
    }

    return ok;
}

bool doc_is_identifier_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

/**
 * Checks whether sv is a C identifier: letter or '_', followed by letters, digits or '_'.
 */
bool doc_sv_is_identifier(Nob_String_View sv) {
    if (sv.count == 0 || isdigit((unsigned char)sv.data[0])) return false;
    for (size_t i = 0; i < sv.count; i++) {
        if (!doc_is_identifier_char(sv.data[i])) return false;
    }
    return true;
}

/**
 * Checks whether sv starts with the whole word, so "struct" matches "struct {" but not "struct_x".
 */
bool doc_sv_starts_with_word(Nob_String_View sv, Nob_String_View word) {
    return nob_sv_starts_with(sv, word) && (sv.count == word.count || !doc_is_identifier_char(sv.data[word.count]));
}

/**
 * Identifier sv starts with, empty if it doesn't start with one.
 */
Nob_String_View doc_sv_first_identifier(Nob_String_View sv) {
    size_t length = 0;
    while (length < sv.count && doc_is_identifier_char(sv.data[length])) length++;

    Nob_String_View identifier = nob_sv_from_parts(sv.data, length);
    return doc_sv_is_identifier(identifier) ? identifier : (Nob_String_View) {0};
}

/**
 * Last identifier in sv, skipping any trailing `[...]` array sizes, so "int names[16];" gives "names".
 * Empty if there is none.
 */
Nob_String_View doc_sv_last_identifier(Nob_String_View sv) {
    size_t end = sv.count;
    int brackets = 0;
    while (end > 0) {
        char c = sv.data[end - 1];
        if      (c == ']') brackets++;
        else if (c == '[') brackets--;
        else if (brackets == 0 && doc_is_identifier_char(c)) break;
        end--;
    }

    size_t start = end;
    while (start > 0 && doc_is_identifier_char(sv.data[start - 1])) start--;

    Nob_String_View identifier = nob_sv_from_parts(sv.data + start, end - start);
    return doc_sv_is_identifier(identifier) ? identifier : (Nob_String_View) {0};
}

/**
 * Checks whether symbol has one of the library prefixes: `cn_`, `Cn_` or `CN_`.
 * Internal `cn__` symbols have it too, they are told apart by the 4th character being '_'.
 */
bool doc_sv_has_library_prefix(Nob_String_View symbol) {
    return nob_sv_starts_with(symbol, DOC_SV_LIT("cn_"))
        || nob_sv_starts_with(symbol, DOC_SV_LIT("Cn_"))
        || nob_sv_starts_with(symbol, DOC_SV_LIT("CN_"));
}

/**
 * Appends line to sb, separated by newline if text of the given count is not empty.
 * Count of the text the line belongs to is advanced accordingly.
 */
void doc_append_line(Nob_String_Builder *sb, size_t *count, Nob_String_View line) {
    if (*count > 0) {
        nob_da_append(sb, '\n');
        (*count)++;
    }
    nob_sb_append_buf(sb, line.data, line.count);
    *count += line.count;
}

/**
 * Appends line to whatever text target points to, target must be open.
 */
void doc_append_to_target(Doc_Entry *entry, Nob_String_Builder *text_sb, Nob_String_Builder *description_sb, Doc_Target target, Nob_String_View line) {
    switch (target) {
    case DOC_TARGET_SUMMARY:
        doc_append_line(text_sb, &entry->summary.count, line);
        break;

    case DOC_TARGET_TAG:
        doc_append_line(text_sb, &nob_da_last(&entry->tags).text.count, line);
        break;

    case DOC_TARGET_DESCRIPTION:
        if (description_sb->count != 0) nob_da_append(description_sb, '\n');
        nob_sb_append_buf(description_sb, line.data, line.count);
        break;

    case DOC_TARGET_NONE:
        NOB_UNREACHABLE("doc_append_to_target");
    }
}


void doc_print_json_entry(FILE *f, Doc_Entry *entry) {
    fputs("  {\n", f);

    fputs("    \"symbol\": ", f);
    doc_print_json_string(f, entry->symbol);
    fputs(",\n", f);

    fprintf(f, "    \"kind\": \"%s\",\n", doc_kind_to_lit(entry->kind));

    fputs("    \"module\": ", f);
    doc_print_json_string(f, entry->module);
    fputs(",\n", f);

    fputs("    \"decl\": ", f);
    doc_print_json_string(f, entry->decl);
    fputs(",\n", f);

    fputs("    \"summary\": ", f);
    doc_print_json_string(f, entry->summary);
    fputs(",\n", f);

    fputs("    \"description\": ", f);
    doc_print_json_string(f, entry->description);
    fputs(",\n", f);

    fputs("    \"tags\": [", f);
    for (size_t i = 0; i < entry->tags.count; i++) {
        Doc_Tag *tag = &entry->tags.items[i];
        fputs(i == 0 ? "\n" : ",\n", f);
        fprintf(f, "      { \"kind\": \"%s\", \"text\": ", doc_tag_to_lit(tag->kind));
        doc_print_json_string(f, tag->text);
        fputs(" }", f);
    }
    fputs(entry->tags.count > 0 ? "\n    ],\n" : "],\n", f);

    fputs("    \"flags\": [", f);
    bool first_flag = true;
#define X(K, v, lit)                                    \
    if (entry->flags & DOC_FLAG_##K) {                  \
        fputs(first_flag ? "\"" lit "\"" : ", \"" lit "\"", f); \
        first_flag = false;                             \
    }
    DOC_FLAG_GEN_LIST(X)
#undef X
    fputs("]\n", f);

    fputs("  }", f);
}

Nob_String_View doc_get_next_comment(Nob_String_View *content) {
    // Getting doc comment.
    *content = doc_trim_sv_till(*content, DOC_SV_LIT("/**"));
    if (content->data == NULL) return (Nob_String_View) {0};

    // Chopping /** away.
    nob_sv_chop_left(content, 3);

    // Getting doc text.
    Nob_String_View doc = doc_get_sv_till(*content, DOC_SV_LIT("*/"));
    nob_sv_chop_left(content, doc.count + 2); // +2 for "*/".

    return doc;
}

/**
 * If doc is a section banner, returns the section name, otherwise returns empty view.
 * Banner is a doc comment that has a line starting with "SECTION:", like:
 * ```
 * ==========================
 * SECTION: String Builder
 * ==========================
 * ```
 */
Nob_String_View doc_get_section(Nob_String_View doc) {
    while (doc.count > 0) {
        Nob_String_View line = nob_sv_trim(nob_sv_chop_by_delim(&doc, '\n'));
        if (nob_sv_starts_with(line, DOC_SV_LIT("*"))) line = nob_sv_trim_left(nob_sv_from_parts(line.data + 1, line.count - 1));

        if (nob_sv_starts_with(line, DOC_SV_LIT("SECTION:"))) {
            return nob_sv_trim(nob_sv_from_parts(line.data + 8, line.count - 8));
        }
    }

    return (Nob_String_View) {0};
}

/**
 * If sv has string or char literal, or a comment, starting at index i, returns index right after it.
 * Otherwise returns i itself. Used so that braces, parens and ';' inside them are not counted.
 */
size_t doc_skip_literal_or_comment(Nob_String_View sv, size_t i) {
    char c    = sv.data[i];
    char next = i + 1 < sv.count ? sv.data[i + 1] : '\0';

    if (c == '"' || c == '\'') {
        for (i++; i < sv.count && sv.data[i] != c; i++) {
            if (sv.data[i] == '\\') i++; // Escaped character, like \" or \\.
        }
        return i < sv.count ? i + 1 : sv.count;
    }

    if (c == '/' && next == '*') {
        Nob_String_View end = doc_trim_sv_till(nob_sv_from_parts(sv.data + i + 2, sv.count - i - 2), DOC_SV_LIT("*/"));
        return end.data == NULL ? sv.count : (size_t)(end.data - sv.data) + 2;
    }

    if (c == '/' && next == '/') {
        while (i < sv.count && sv.data[i] != '\n') i++;
        return i;
    }

    return i;
}

/**
 * Gets full text of the declaration that content starts with, content is advanced past it.
 * That way doc comments nested in the declaration, like on struct members, are skipped too.
 *
 * Preprocessor line continues to the next line as long as it ends with '\', so multiline macros are complete.
 * Anything else ends with the first ';' outside of braces, parens and brackets, so whole struct bodies are included,
 * or right before the next doc comment outside of them, if there is no such ';'.
 *
 * RETURNS: Declaration including its ';', empty if content starts with another comment instead.
 */
Nob_String_View doc_get_declaration(Nob_String_View *content) {
    *content = nob_sv_trim_left(*content);
    Nob_String_View decl = nob_sv_from_parts(content->data, 0);

    // Doc comment that is not followed by declaration, next comment must stay for doc_get_next_comment.
    if (nob_sv_starts_with(*content, DOC_SV_LIT("/*")) || nob_sv_starts_with(*content, DOC_SV_LIT("//"))) {
        return decl;
    }

    if (nob_sv_starts_with(*content, DOC_SV_LIT("#"))) {
        while (content->count > 0) {
            Nob_String_View line = nob_sv_chop_by_delim(content, '\n');
            decl.count = (size_t)(line.data + line.count - decl.data);
            if (!nob_sv_ends_with(nob_sv_trim_right(line), DOC_SV_LIT("\\"))) break;
        }
        return decl;
    }

    int depth = 0;
    size_t i = 0;
    while (i < content->count) {
        // Doc comment outside of any body starts the next entry, for example after a macro call without ';'.
        if (depth == 0 && nob_sv_starts_with(nob_sv_from_parts(content->data + i, content->count - i), DOC_SV_LIT("/**"))) break;

        size_t skipped = doc_skip_literal_or_comment(*content, i);
        if (skipped != i) {
            i = skipped;
            continue;
        }

        char c = content->data[i++];
        if      (c == '{' || c == '(' || c == '[') depth++;
        else if (c == '}' || c == ')' || c == ']') depth--;

        // ';' ends declaration, unmatched closing brace means doc comment was the last thing in some body.
        if ((c == ';' && depth == 0) || depth < 0) break;
    }

    decl.count = i;
    nob_sv_chop_left(content, i);
    return decl;
}

/**
 * Determines kind and symbol name of the declaration, by how it starts:
 * ```
 * #define NAME ...                         macro, name right after define
 * CNDEF ... name(...);                     function, name right before first '('
 * CNDEF Function_Type name;                function, declared through function typedef, last name
 * extern ... name;                         variable, last name, or function if it has '('
 * typedef struct|union|enum {...} Name;    struct, union or enum, last name
 * typedef R (Name)(...);                   typedef, name inside first parens, (*Name) works too
 * typedef ... Name;                        typedef, last name
 * struct|union|enum Name {...};            struct, union or enum, name right after keyword
 * ```
 * RETURNS: `true` if declaration is one of the above and has a name, `false` otherwise.
 */
bool doc_classify_declaration(Nob_String_View decl, Doc_Kind *kind, Nob_String_View *symbol) {
    static const struct { Nob_String_View keyword; Doc_Kind kind; } tags[] = {
        { DOC_SV_LIT("struct"), DOC_KIND_STRUCT },
        { DOC_SV_LIT("union"),  DOC_KIND_UNION  },
        { DOC_SV_LIT("enum"),   DOC_KIND_ENUM   },
    };

    // Part of the declaration before first '(', whole declaration if there is no '('.
    Nob_String_View before_paren = doc_get_sv_till(decl, DOC_SV_LIT("("));
    bool has_paren = before_paren.count < decl.count;

    *kind   = DOC_KIND_NONE;
    *symbol = (Nob_String_View) {0};

    if (nob_sv_starts_with(decl, DOC_SV_LIT("#"))) {
        Nob_String_View directive = nob_sv_trim_left(nob_sv_from_parts(decl.data + 1, decl.count - 1));
        if (!doc_sv_starts_with_word(directive, DOC_SV_LIT("define"))) return false;
        nob_sv_chop_left(&directive, 6);

        *kind   = DOC_KIND_MACRO;
        *symbol = doc_sv_first_identifier(nob_sv_trim_left(directive));

    } else if (doc_sv_starts_with_word(decl, DOC_SV_LIT("CNDEF"))) {
        *kind   = DOC_KIND_FUNCTION;
        *symbol = doc_sv_last_identifier(before_paren);

    } else if (doc_sv_starts_with_word(decl, DOC_SV_LIT("extern"))) {
        *kind   = has_paren ? DOC_KIND_FUNCTION : DOC_KIND_VARIABLE;
        *symbol = doc_sv_last_identifier(before_paren);

    } else if (doc_sv_starts_with_word(decl, DOC_SV_LIT("typedef"))) {
        Nob_String_View type = nob_sv_trim_left(nob_sv_from_parts(decl.data + 7, decl.count - 7));
        bool has_body = doc_trim_sv_till(decl, DOC_SV_LIT("{")).data != NULL;

        *kind   = DOC_KIND_TYPEDEF;
        *symbol = doc_sv_last_identifier(decl);

        for (size_t i = 0; i < NOB_ARRAY_LEN(tags); i++) {
            if (has_body && doc_sv_starts_with_word(type, tags[i].keyword)) *kind = tags[i].kind;
        }

        // Function typedef, name is inside first parens: "typedef R (Name)(...)".
        if (*kind == DOC_KIND_TYPEDEF && has_paren) {
            Nob_String_View in_parens = nob_sv_from_parts(decl.data + before_paren.count + 1, decl.count - before_paren.count - 1);
            *symbol = doc_sv_last_identifier(doc_get_sv_till(in_parens, DOC_SV_LIT(")")));
        }

    } else {
        for (size_t i = 0; i < NOB_ARRAY_LEN(tags); i++) {
            if (!doc_sv_starts_with_word(decl, tags[i].keyword)) continue;

            *kind   = tags[i].kind;
            *symbol = doc_sv_first_identifier(nob_sv_trim_left(nob_sv_from_parts(decl.data + tags[i].keyword.count, decl.count - tags[i].keyword.count)));
        }
    }

    return *kind != DOC_KIND_NONE && symbol->count > 0;
}

/**
 * Makes signature that is displayed from the declaration, written into sb, which is cleared first.
 * Leading "CNDEF" and trailing ';' are removed.
 * Functions, variables and typedefs are joined into a single line, whitespace collapsed.
 * Macros, structs, unions and enums keep their lines, with trailing whitespace of each line trimmed.
 *
 * RETURNS: View into sb, valid until sb is changed.
 */
Nob_String_View doc_make_signature(Nob_String_View decl, Doc_Kind kind, Nob_String_Builder *sb) {
    sb->count = 0;

    decl = nob_sv_trim(decl);
    if (nob_sv_ends_with(decl, DOC_SV_LIT(";"))) decl.count--;
    if (doc_sv_starts_with_word(decl, DOC_SV_LIT("CNDEF"))) decl = nob_sv_from_parts(decl.data + 5, decl.count - 5);
    decl = nob_sv_trim(decl);

    bool single_line = kind == DOC_KIND_FUNCTION || kind == DOC_KIND_VARIABLE || kind == DOC_KIND_TYPEDEF;

    if (single_line) {
        for (size_t i = 0; i < decl.count; i++) {
            if (!isspace((unsigned char)decl.data[i])) {
                nob_da_append(sb, decl.data[i]);
            } else if (sb->count > 0 && sb->items[sb->count - 1] != ' ') {
                nob_da_append(sb, ' ');
            }
        }
    } else {
        while (decl.count > 0) {
            Nob_String_View line = nob_sv_trim_right(nob_sv_chop_by_delim(&decl, '\n'));
            if (sb->count > 0) nob_da_append(sb, '\n');
            nob_sb_append_buf(sb, line.data, line.count);
        }
    }

    return nob_sb_to_sv(*sb);
}

/**
 * Gets text inside of the block comment that content starts with, whitespace before it is skipped.
 * Content is advanced past the comment.
 *
 * RETURNS: Text between comment opening and closing, view with NULL data if content doesn't start with a block comment.
 */
Nob_String_View doc_get_block_comment(Nob_String_View *content) {
    *content = nob_sv_trim_left(*content);
    if (!nob_sv_starts_with(*content, DOC_SV_LIT("/*"))) return (Nob_String_View) {0};
    nob_sv_chop_left(content, 2);

    Nob_String_View text = doc_get_sv_till(*content, DOC_SV_LIT("*/"));
    nob_sv_chop_left(content, text.count < content->count ? text.count + 2 : text.count); // +2 for "*/".

    return text;
}

/**
 * Writes json with the comments describing library as a whole, they are not documentation entries:
 * ```
 * {
 *   "header":  "comment cnotes.h starts with: name, version, license, macro interface, conventions...",
 *   "trailer": ["comments after the last #endif: revision history, version conventions, license..."]
 * }
 * ```
 * Comment text is kept as is, frontend formats it.
 */
bool doc_write_library_json(Nob_String_View file, const char *path) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        nob_log(NOB_ERROR, "Could not open '%s' for writing: %s", path, strerror(errno));
        return false;
    }

    Nob_String_View content = file;
    Nob_String_View header  = doc_get_block_comment(&content);

    // Trailer starts on the line after the last #endif, empty if there is none.
    Nob_String_View trailer = {0};
    for (Nob_String_View rest = doc_trim_sv_till(file, DOC_SV_LIT("#endif")); rest.data != NULL; rest = doc_trim_sv_till(rest, DOC_SV_LIT("#endif"))) {
        nob_sv_chop_by_delim(&rest, '\n');
        trailer = rest;
    }

    fputs("{\n  \"header\": ", f);
    doc_print_json_string(f, header);
    fputs(",\n  \"trailer\": [", f);

    size_t count = 0;
    while (true) {
        Nob_String_View comment = doc_get_block_comment(&trailer);
        if (comment.data == NULL) break;

        fputs(count == 0 ? "\n    " : ",\n    ", f);
        doc_print_json_string(f, comment);
        count++;
    }

    fputs(count > 0 ? "\n  ]\n}\n" : "]\n}\n", f);
    fclose(f);

    nob_log(NOB_INFO, "Wrote library header and %zu trailing comments to '%s'.", count, path);
    return true;
}

bool doc_parse_entry(Nob_String_View symbol, Nob_String_View signature, Nob_String_View doc, Doc_Entry *entry_out) {
    static Nob_String_Builder doc__text_sb         = {0};  // LEAK.
    static Nob_String_Builder doc__description_sb  = {0};  // LEAK.

    Doc_Entry entry = {0};

    entry.symbol = symbol;
    entry.decl   = signature;

    doc__text_sb.count          = 0;
    doc__description_sb.count   = 0;


    // Examining doc line by line.
    // Consecutive lines are joined with '\n' into whatever is currently open: summary, tag or description.
    // A blank line closes it. Text after a blank line goes to description, a tag line always opens a new tag.
    // Inside ``` fences lines are kept verbatim: indentation and blank lines stay, tags are not detected.
    // A line starting with "--" is a flags line, it is parsed into flags and not added to any text.
    Doc_Target target = DOC_TARGET_NONE;
    bool has_content = false;
    bool in_fence = false;
    bool flags_ok = true;

    while (doc.count > 0) {
        Nob_String_View raw = nob_sv_trim(nob_sv_chop_by_delim(&doc, '\n'));

        // Removing leading "*" and only one space after it, so indentation inside fences survives.
        if (nob_sv_starts_with(raw, DOC_SV_LIT("*"))) {
            nob_sv_chop_left(&raw, 1);
            if (nob_sv_starts_with(raw, DOC_SV_LIT(" "))) nob_sv_chop_left(&raw, 1);
        }

        Nob_String_View line = nob_sv_trim_left(raw);

        // Fence line toggles fence, unless fence is also closed on the same line, like ```code```.
        bool is_fence = nob_sv_starts_with(line, DOC_SV_LIT("```"));
        bool toggles_fence = is_fence && doc_trim_sv_till(nob_sv_from_parts(line.data + 3, line.count - 3), DOC_SV_LIT("```")).data == NULL;

        if (in_fence) {
            // Target is always open here, opening fence line opened it.
            if (toggles_fence) in_fence = false;
            doc_append_to_target(&entry, &doc__text_sb, &doc__description_sb, target, toggles_fence ? line : raw);
            continue;
        }

        // Blank line closes current summary, tag or description paragraph.
        if (line.count == 0) {
            target = DOC_TARGET_NONE;
            continue;
        }

        // Flags line closes current text same as blank line.
        if (nob_sv_starts_with(line, DOC_SV_LIT("--"))) {
            if (!doc_parse_flags(symbol, line, &entry.flags)) flags_ok = false;
            target = DOC_TARGET_NONE;
            continue;
        }

        // Tag lines have priority, they are checked first.
        Nob_String_View tag_sv = doc_get_sv_till(line, DOC_SV_LIT(":"));
        size_t tag_length = tag_sv.count + 1; // +1 for ':'.

        // "PARAM name: text" and "MEMBER name: text", the name is kept in the text as "name: text".
        Nob_String_View tag_word = doc_get_sv_till(tag_sv, DOC_SV_LIT(" "));
        bool is_named_tag = tag_word.count < tag_sv.count &&
            (nob_sv_eq(tag_word, DOC_SV_LIT("PARAM")) || nob_sv_eq(tag_word, DOC_SV_LIT("MEMBER")));
        if (is_named_tag && doc_sv_is_identifier(nob_sv_from_parts(tag_sv.data + tag_word.count + 1, tag_sv.count - tag_word.count - 1))) {
            tag_sv     = tag_word;
            tag_length = tag_word.count + 1; // +1 for ' ', name itself stays in the line.
        }

        Doc_Tag_Kind tag_kind = is_fence ? UNKNOWN : doc_sv_to_tag(tag_sv);
        if (tag_kind != UNKNOWN) {
            Doc_Tag tag = { .kind = tag_kind };
            nob_da_append(&entry.tags, tag);

            // Removing tag from the line to just add text.
            nob_sv_chop_left(&line, tag_length);
            line = nob_sv_trim_left(line);
            doc_append_line(&doc__text_sb, &nob_da_last(&entry.tags).text.count, line);

            target = DOC_TARGET_TAG;
            has_content = true;
            continue;
        }

        if (target == DOC_TARGET_NONE) {
            if (!has_content) {
                // Summary only starts from the very first line, if it is tagged summary is skipped.
                target = DOC_TARGET_SUMMARY;
            } else {
                // New description paragraph. Together with '\n' added on append,
                // paragraphs end up separated by blank line, same as in markdown.
                if (doc__description_sb.count != 0) nob_da_append(&doc__description_sb, '\n');
                target = DOC_TARGET_DESCRIPTION;
            }
        }

        if (toggles_fence) in_fence = true;
        doc_append_to_target(&entry, &doc__text_sb, &doc__description_sb, target, line);
        has_content = true;
    }

    if (in_fence) {
        nob_log(NOB_WARNING, "Unclosed ``` code block in documentation of '%.*s'.", DOC_SV_UNPACK(symbol));
    }

    // Summary and tag texts are stored back to back in doc_text_sb, only counts were recorded,
    // since sb might have been reallocated while appending. Now pointing views into final buffer.
    const char *text_cursor = doc__text_sb.items;
    entry.summary.data = text_cursor;
    text_cursor += entry.summary.count;
    nob_da_foreach(Doc_Tag, tag, &entry.tags) {
        tag->text.data = text_cursor;
        text_cursor += tag->text.count;
    }

    entry.description = nob_sb_to_sv(doc__description_sb);

    *entry_out = entry;

    return flags_ok;
}






//
// Testing.
//
typedef enum : uint8_t {
    PENDING             = 0,
    BUILD_FAIL,
    RUNTIME_FAIL,
    UNEXPECTED_OUTPUT,
    TIMEOUT,
    SUCCESS,
} Test_Status;

typedef enum : uint8_t {
    PARSE   = 0,
    EXEC,
} Test_Kind;

typedef struct {
    Test_Kind       kind;
    const char     *path;
    Test_Status     status;
    uint64_t        duration_ms;    // Time it took to build and run the test.

    // From "/** TEST: title ... */" doc comment the test file starts with, empty if there is none.
    Nob_String_View title;
    Nob_String_View description;
    bool            gnu;            // "--gnu" flag, parse test is run by gnu runner.
} Test_Record;

typedef struct {
    Test_Record *items;
    size_t count;
    size_t capacity;
} Test_Records;

bool test_prepare_parse_entry(Nob_Walk_Entry entry);
bool test_prepare_exec_entry(Nob_Walk_Entry entry);
void test_execute_parse(Test_Record *r, Nob_Cmd *cmd, const char *runner);
void test_execute_exec(Test_Record *r, Nob_Cmd *cmd);
void test_print_results(Test_Records *record);
bool test_scan_record(Test_Record *r);
const char *test_status_to_lit(Test_Status status);
const char *test_kind_to_lit(Test_Kind kind);
void test_print_json_records(FILE *f, Test_Records *records, bool *first);
bool test_write_json(Test_Records *parse_records, Test_Records *exec_records, const char *path);

bool test_prepare_parse_entry(Nob_Walk_Entry entry) {
    Test_Records *records = (Test_Records *)entry.data;
    if (entry.type == FILE_REGULAR && strcmp(entry.path + strlen(entry.path) - 2, ".c") == 0) {
        char *saved_path = arena_alloc(&arena_strings, strlen(entry.path) + 1);
        strcpy(saved_path, entry.path);
        nob_da_append(records, ((Test_Record) { .kind = PARSE, .path = saved_path, .status = PENDING }));
        nob_log(NOB_INFO, "test: added '%s' parse test.", entry.path);
    }

    return true;
}

bool test_prepare_exec_entry(Nob_Walk_Entry entry) {
    Test_Records *records = (Test_Records *)entry.data;
    if (entry.type == FILE_REGULAR && strcmp(entry.path + strlen(entry.path) - 2, ".c") == 0) {
        char *saved_path = arena_alloc(&arena_strings, strlen(entry.path) + 1);
        strcpy(saved_path, entry.path);
        nob_da_append(records, ((Test_Record) { .kind = EXEC, .path = saved_path, .status = PENDING }));
        nob_log(NOB_INFO, "test: added '%s' exec test.", entry.path);
    }

    return true;
}

void test_execute_parse(Test_Record *r, Nob_Cmd *cmd, const char *runner) {
    NOB_ASSERT(r->kind == PARSE);

    nob_log(NOB_INFO, "test: '%s':", r->path);

    

    // Running test.
    nob_log(NOB_INFO, "    : parsing '%s'.", r->path);

    CSTR_BUFFER(stdout_path, 64);
    CSTR_BUFFER_APPEND(stdout_path, BUILD_DIR"/");
    CSTR_BUFFER_APPEND(stdout_path, r->path);
    cut_file_extension(stdout_path);
    CSTR_BUFFER_APPEND(stdout_path, STDOUT_TXT_FILE_EXTENSION);

    nob_cmd_append(cmd, runner);
    nob_cmd_append(cmd, r->path);
    Build_Proc proc = {0};
    if (!build_proc_spawn(&proc, cmd, stdout_path)) {
        r->status = RUNTIME_FAIL;
        return;
    }

    switch (build_proc_wait(&proc, TEST_TIMEOUT_MS)) {
    case PROC_TIMEOUT:
        nob_log(NOB_ERROR, "test: '%s' timed out after %d ms", r->path, TEST_TIMEOUT_MS);
        r->status = TIMEOUT;
        return;
    case PROC_FAIL:
        nob_log(NOB_ERROR, "test: '%s' fail.", r->path);
        r->status = RUNTIME_FAIL;
        return;
    case PROC_OK:
        nob_log(NOB_INFO, "test: '%s' ok.", r->path);
        break;
    }

    // Comparing output against golden.
    CSTR_BUFFER(expected_stdout_path, 64);
    CSTR_BUFFER_APPEND(expected_stdout_path, r->path);
    cut_file_extension(expected_stdout_path);
    CSTR_BUFFER_APPEND(expected_stdout_path, STDOUT_TXT_FILE_EXTENSION);
    
    if (!compare_files(expected_stdout_path, stdout_path)) {
        nob_log(NOB_ERROR, "test: unexpected output.");
        r->status = UNEXPECTED_OUTPUT;
        return;
    }

    r->status = SUCCESS;
}

void test_execute_exec(Test_Record *r, Nob_Cmd *cmd) {
    NOB_ASSERT(r->kind == EXEC);

    nob_log(NOB_INFO, "test: '%s':", r->path);

    // Making executable output path for the test.
    CSTR_BUFFER(output_path_buffer, 64);
    CSTR_BUFFER_APPEND(output_path_buffer, BUILD_DIR"/");
    CSTR_BUFFER_APPEND(output_path_buffer, r->path);
    cut_file_extension(output_path_buffer);

    // Building test executable.
    nob_log(NOB_INFO, "    : building '%s'.", output_path_buffer);

    nob_cc(cmd);
    nob_cc_flags(cmd);

    nob_cc_output(cmd, output_path_buffer);
    nob_cc_inputs(cmd, r->path);
    nob_cmd_append(cmd, BIN_DIR"/libcnotes.a");

    if (!nob_cmd_run(cmd)) {
        r->status = BUILD_FAIL;
        return;
    }

    // Running test.
    nob_log(NOB_INFO, "    : running '%s'.", output_path_buffer);

    CSTR_BUFFER(stdout_path, 64);
    CSTR_BUFFER_APPEND(stdout_path, output_path_buffer);
    CSTR_BUFFER_APPEND(stdout_path, STDOUT_TXT_FILE_EXTENSION);

    nob_cmd_append(cmd, output_path_buffer);
    Build_Proc proc = {0};
    if (!build_proc_spawn(&proc, cmd, stdout_path)) {
        r->status = RUNTIME_FAIL;
        return;
    }

    switch (build_proc_wait(&proc, TEST_TIMEOUT_MS)) {
    case PROC_TIMEOUT:
        nob_log(NOB_ERROR, "test: '%s' timed out after %d ms", output_path_buffer, TEST_TIMEOUT_MS);
        r->status = TIMEOUT;
        return;
    case PROC_FAIL:
        nob_log(NOB_ERROR, "test: '%s' fail.", output_path_buffer);
        r->status = RUNTIME_FAIL;
        return;
    case PROC_OK:
        nob_log(NOB_INFO, "test: '%s' ok.", output_path_buffer);
        break;
    }


    // Comparing output against golden.
    CSTR_BUFFER(expected_stdout_path, 64);
    CSTR_BUFFER_APPEND(expected_stdout_path, r->path);
    cut_file_extension(expected_stdout_path);
    CSTR_BUFFER_APPEND(expected_stdout_path, STDOUT_TXT_FILE_EXTENSION);
    
    if (!compare_files(expected_stdout_path, stdout_path)) {
        nob_log(NOB_ERROR, "test: unexpected output.");
        r->status = UNEXPECTED_OUTPUT;
        return;
    }

    r->status = SUCCESS;
}

void test_print_results(Test_Records *records) {
    nob_da_foreach(Test_Record, r, records) {
        fprintf(stderr, "%64s : ", r->path);
    
        switch (r->status) {
            case PENDING:
                fprintf(stderr, "\033[1;33mIGNORED\033[0m       ");
                break;
        
            case BUILD_FAIL:
                fprintf(stderr, "\033[1;31mBUILD FAIL\033[0m    ");
                break;
        
            case RUNTIME_FAIL:
                fprintf(stderr, "\033[31mRUNTIME FAIL\033[0m    ");
                break;

            case UNEXPECTED_OUTPUT:
                fprintf(stderr, "\033[31mUNEXPECTED OUTPUT\033[0m    ");
                break;

            case TIMEOUT:
                fprintf(stderr, "\033[31mTIMEOUT\033[0m    ");
                break;
        
            case SUCCESS:
                fprintf(stderr, "\033[1;32mSUCCESS\033[0m       ");
                break;
        }
    
        fprintf(stderr, "\n");
    }
}

/**
 * Reads doc comment the test file starts with into title, description and flags of the record.
 * Comment must have "TEST:" tag, its text is the title, paragraphs after it are the description:
 * ```
 * TEST: Array declarators in parameters adjusted to pointers.
 *
 * Longer description of what is tested.
 * ```
 * Files without such comment are fine, their title and description stay empty.
 *
 * RETURNS: `false` if the file couldn't be read or the comment has invalid flags.
 */
bool test_scan_record(Test_Record *r) {
    static Nob_String_Builder sb = {0}; // LEAK.
    sb.count = 0; // nob_read_entire_file appends, previous test content must be dropped.

    if (!nob_read_entire_file(r->path, &sb)) {
        nob_log(NOB_ERROR, "test: '%s' couldn't open for doc scan.", r->path);
        return false;
    }

    Nob_String_View content = nob_sb_to_sv(sb);
    Nob_String_View doc     = doc_get_next_comment(&content);
    if (doc.data == NULL) return true;

    Doc_Entry entry = {0};
    bool ok = doc_parse_entry(nob_sv_from_cstr(r->path), (Nob_String_View) {0}, doc, &entry);

    nob_da_foreach(Doc_Tag, t, &entry.tags) {
        if (t->kind != TEST) continue;

        // Texts of the entry are reused by the next doc_parse_entry call, so they are copied.
        r->title       = arena_sv_dup(&arena_strings, t->text);
        r->description = arena_sv_dup(&arena_strings, entry.description);
        r->gnu         = (entry.flags & DOC_FLAG_GNU) != 0;
        break;
    }

    nob_da_free(entry.tags);
    return ok;
}

const char *test_status_to_lit(Test_Status status) {
    switch (status) {
    case PENDING:           return "ignored";
    case BUILD_FAIL:        return "build_fail";
    case RUNTIME_FAIL:      return "runtime_fail";
    case UNEXPECTED_OUTPUT: return "unexpected_output";
    case TIMEOUT:           return "timeout";
    case SUCCESS:           return "success";
    }
    return "<UNKNOWN>";
}

const char *test_kind_to_lit(Test_Kind kind) {
    switch (kind) {
    case PARSE: return "parse";
    case EXEC:  return "exec";
    }
    return "<UNKNOWN>";
}

void test_print_json_records(FILE *f, Test_Records *records, bool *first) {
    nob_da_foreach(Test_Record, r, records) {
        fputs(*first ? "\n" : ",\n", f);
        *first = false;

        fputs("    { \"path\": ", f);
        doc_print_json_string(f, nob_sv_from_cstr(r->path));
        fprintf(f, ", \"kind\": \"%s\", \"status\": \"%s\", \"duration_ms\": %llu, \"gnu\": %s,\n",
                test_kind_to_lit(r->kind), test_status_to_lit(r->status),
                (unsigned long long)r->duration_ms, r->gnu ? "true" : "false");
        fputs("      \"title\": ", f);
        doc_print_json_string(f, r->title);
        fputs(",\n      \"description\": ", f);
        doc_print_json_string(f, r->description);
        fputs(" }", f);
    }
}

/**
 * Writes results of the test run as json, frontend shows them on its tests page:
 * ```
 * {
 *   "generated": "2026-11-25T10:00:00Z",
 *   "commit":    "sha from GITHUB_SHA environment variable, empty when run locally",
 *   "tests": [
 *     { "path", "kind": "parse|exec", "status", "duration_ms", "gnu", "title", "description" }
 *   ]
 * }
 * ```
 */
bool test_write_json(Test_Records *parse_records, Test_Records *exec_records, const char *path) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        nob_log(NOB_ERROR, "Could not open '%s' for writing: %s", path, strerror(errno));
        return false;
    }

    char generated[32] = {0};
    time_t now = time(NULL);
    strftime(generated, sizeof(generated), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    const char *commit = getenv("GITHUB_SHA");

    fprintf(f, "{\n  \"generated\": \"%s\",\n  \"commit\": ", generated);
    doc_print_json_string(f, nob_sv_from_cstr(commit != NULL ? commit : ""));
    fputs(",\n  \"tests\": [", f);

    bool first = true;
    test_print_json_records(f, parse_records, &first);
    test_print_json_records(f, exec_records, &first);

    fputs(first ? "]\n}\n" : "\n  ]\n}\n", f);
    fclose(f);

    nob_log(NOB_INFO, "test: wrote results to '%s'.", path);
    return true;
}

// void test_record(const char *source, Nob_Cmd *cmd);
// 
// bool test_record_entry(Nob_Walk_Entry entry) {
//     Nob_Cmd *cmd = (Nob_Cmd *)entry.data;
//     if (entry.type == FILE_REGULAR && strcmp(entry.path + strlen(entry.path) - 2, ".c") == 0) {
//         test_record(entry.path, cmd);
//     }
// 
//     return true;
// }
// 
// /**
//  * This function does following things:
//  * First it copies the source file .c into tests directory, compiles and runs it there. 
//  * Generating .stdout.txt which contains information about expected output.
//  * After that it cleans up generated executable leaving only expected .stdout.txt.
//  * TODO: Sanitize test files.
//  */
// void test_record(const char *source, Nob_Cmd *cmd) {
//     Nob_String_Builder sb = {0};
// 
//     if (strcmp(source + strlen(source) - 2, ".c") != 0) {
//         nob_log(NOB_ERROR, "record: '%s' is not a C file.", source);
//         return;
//     }
// 
//     if (!nob_read_entire_file(source, &sb)) {
//         return;
//     }
// 
//     const char *name = strrchr(source, '/');
//     if (name == NULL) name = source;
//     else name++;
//     
//     char test_path[strlen(TESTS_DIR"/") + strlen(name) + 1];
//     test_path[0] = '\0';
//     strcat(test_path, TESTS_DIR"/");
//     strcat(test_path, name);
// 
//     if (!nob_write_entire_file(test_path, sb.items, sb.count)) {
//         nob_log(NOB_ERROR, "record: couldn't write '%s' file.", test_path);
//         NOB_FREE(sb.items);
//         return;
//     }
// 
// 
//     // Copypasta from test_execute.
//     char output_path_buffer[64];
//     int output_path_length;
// 
//     output_path_length = snprintf(output_path_buffer, sizeof(output_path_buffer), "%s", test_path);
//     if (output_path_length < 0 && output_path_length >= sizeof(output_path_buffer)) {
//         nob_log(NOB_ERROR, "record: test path is too long exceeds %lu buffer size.", sizeof(output_path_buffer));
//         NOB_FREE(sb.items);
//         return;
//     }
//     output_path_buffer[output_path_length - 2] = '\0';
//     
//     nob_cc(cmd);
//     nob_cc_flags(cmd);
// 
//     nob_cc_output(cmd, output_path_buffer);
//     nob_cc_inputs(cmd, test_path);
//     nob_cmd_append(cmd, BIN_DIR"/libcnotes.a");
// 
//     if (!nob_cmd_run(cmd)) {
//         NOB_FREE(sb.items);
//         return;
//     }
// 
//     nob_cmd_append(cmd, output_path_buffer);
// 
//     char stdout_path[strlen(output_path_buffer) + strlen(STDOUT_TXT_FILE_EXTENSION) + 1];
//     stdout_path[0] = '\0';
//     strcat(stdout_path, output_path_buffer);
//     strcat(stdout_path, STDOUT_TXT_FILE_EXTENSION);
// 
//     if (!nob_cmd_run(cmd, .stdout_path = stdout_path)) {
//         nob_delete_file(output_path_buffer);
//         NOB_FREE(sb.items);
//         return;
//     }
// 
//     nob_delete_file(output_path_buffer);
//     NOB_FREE(sb.items);
//     nob_log(NOB_INFO, "record: successfuly recorded '%s' file.", test_path);
// }




Proc_Result how_to_execute(char *path, Nob_Cmd *cmd) {
    nob_log(NOB_INFO, "how_to: running '%s' how_to.", path);
    nob_cmd_append(cmd, "make", "-C", path);

    Build_Proc proc = {0};
    if (!build_proc_spawn(&proc, cmd, NULL))
        return PROC_FAIL;

    switch (build_proc_wait(&proc, TEST_TIMEOUT_MS)) {
        case PROC_TIMEOUT:
            nob_log(NOB_ERROR, "how_to: '%s' timed out after %d ms", path, TEST_TIMEOUT_MS);
            return PROC_TIMEOUT;
        case PROC_FAIL: return PROC_FAIL;
        case PROC_OK:   return PROC_OK;
    }
}

bool how_to_execute_entry(Nob_Walk_Entry entry) {
    // Skip walk examlples dir, is always the first dir entry.
    if (strcmp(entry.path, HOW_TO_DIR) == 0) return true;

    Cmd *cmd = (Cmd *)entry.data;
    *entry.action = NOB_WALK_SKIP;
    if (entry.type == FILE_DIRECTORY) {
        Proc_Result result = how_to_execute((char *)entry.path, cmd);
        if (result != PROC_OK) return false;
    }

    return true;
}













typedef int (Command_Func)(int *argc, char ***argv);

typedef struct {
    const char *name;
    const char *options;
    const char *description;
    Command_Func *func;
} Command;

int help_command(int *argc, char ***argv);
int test_command(int *argc, char ***argv);
// int record_command(int *argc, char ***argv);
int clean_command(int *argc, char ***argv);
int lib_command(int *argc, char ***argv);
int doc_command(int *argc, char ***argv);
int all_command(int *argc, char ***argv);

const static Command commands[] = {
    { "help",       "",             "List all available commands.", help_command },
    { "test",       "",             "Run the tests and check their output.", test_command },
    // { "record",     "[FILE...]",    "Record file as a test and generate it's expected output.", record_command },
    { "clean",      "",             "Recursivly deletes "BUILD_DIR"/ and "BIN_DIR"/ directories.", clean_command },
    { "lib",        "",             "Will compile whole library into .o file and then produce static library.", lib_command },
    { "doc",        "",             "Will run documentation renderer on whole library API.", doc_command },
    { "all",        "",             "Cleans, compiles library, runs tests, and uses library with compiler to run all examples.", all_command },
};

void commands_list(void) {
    size_t count = sizeof(commands) / sizeof(commands[0]);

    size_t max_name = 0;
    size_t max_opts = 0;

    /* Find longest name and options for alignment */
    for (size_t i = 0; i < count; i++) {
        size_t name_len = strlen(commands[i].name);
        size_t opts_len = strlen(commands[i].options);

        if (name_len > max_name) max_name = name_len;
        if (opts_len > max_opts) max_opts = opts_len;
    }

    printf("Available commands:\n\n");

    for (size_t i = 0; i < count; i++) {
        const Command *cmd = &commands[i];

        printf("  \033[1;36m%-*s\033[0m %-*s  %s\n",
               (int)max_name, cmd->name,
               (int)max_opts, cmd->options,
               cmd->description);
    }

    printf("\n");
}

int help_command(int *argc, char ***argv) {
    commands_list();
    return 0;
}

int test_command(int *argc, char ***argv) {
    if (!nob_mkdir_if_not_exists(TESTS_DIR))                            return 1;
    if (!nob_mkdir_if_not_exists(TESTS_DIR"/"PARSE_DIR))                return 1;
    if (!nob_mkdir_if_not_exists(TESTS_DIR"/"EXEC_DIR))                 return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR))                            return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"TESTS_DIR))                return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"TESTS_DIR"/"PARSE_DIR))    return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"TESTS_DIR"/"EXEC_DIR))     return 1;

    arena_strings = arena_make(1024*1024); // Paths, titles and descriptions of the tests.

    Test_Records parse_records  = {0};
    Test_Records exec_records   = {0};

    if (*argc == 0) {
        // Find all available tests.
        if (!nob_walk_dir(TESTS_DIR"/"PARSE_DIR, test_prepare_parse_entry, .data = &parse_records)) return 1;
        if (!nob_walk_dir(TESTS_DIR"/"EXEC_DIR, test_prepare_exec_entry, .data = &exec_records)) return 1;

    } else {
        // Use test files provided by the arguments.
        while (*argc > 0) {
            const char *file = nob_shift(*argv, *argc);
            NOB_TODO("Implement separate test execution for test command.");
        }
    }


    Nob_Cmd cmd = {0};

    // Building runner.
    nob_log(NOB_INFO, "test: building parse runner.");

    nob_cc(&cmd);
    nob_cc_flags(&cmd);

    nob_cc_output(&cmd, BUILD_DIR"/"TESTS_DIR"/runner");
    nob_cc_inputs(&cmd, TESTS_DIR"/runner.c");
    // nob_cmd_append(&cmd, BIN_DIR"/libcnotes.a");

    if (!nob_cmd_run(&cmd)) {
        return -1;
    }

    // Building gnu runner.
    nob_log(NOB_INFO, "test: building parse gnu runner.");

    nob_cc(&cmd);
    nob_cc_flags(&cmd);

    nob_cc_output(&cmd, BUILD_DIR"/"TESTS_DIR"/gnu_runner");
    nob_cc_inputs(&cmd, TESTS_DIR"/gnu_runner.c");
    // nob_cmd_append(&cmd, BIN_DIR"/libcnotes.a");

    if (!nob_cmd_run(&cmd)) {
        return -1;
    }

    // Executing tests, test with unreadable doc comment is not run and stays ignored.
    int failed = 0;

    nob_da_foreach(Test_Record, r, &parse_records) {
        uint64_t start = nob_nanos_since_unspecified_epoch();

        if (test_scan_record(r)) {
            test_execute_parse(r, &cmd, r->gnu ? BUILD_DIR"/"TESTS_DIR"/gnu_runner" : BUILD_DIR"/"TESTS_DIR"/runner");
        }

        r->duration_ms = (nob_nanos_since_unspecified_epoch() - start) / (NOB_NANOS_PER_SEC / 1000);
        if (r->status != SUCCESS) failed += 1;
    }

    nob_da_foreach(Test_Record, r, &exec_records) {
        uint64_t start = nob_nanos_since_unspecified_epoch();

        if (test_scan_record(r)) {
            test_execute_exec(r, &cmd);
        }

        r->duration_ms = (nob_nanos_since_unspecified_epoch() - start) / (NOB_NANOS_PER_SEC / 1000);
        if (r->status != SUCCESS) failed += 1;
    }

    // Showing test results
    fprintf(stderr, "PARSE:\n");
    test_print_results(&parse_records);
    fprintf(stderr, "EXEC:\n");
    test_print_results(&exec_records);
    
    int64_t total = parse_records.count + exec_records.count;
    if (failed > 0) {
        nob_log(NOB_ERROR, "test: total [%lu/%lu] passed.", total - failed, total);
    } else {
        nob_log(NOB_INFO, "test: total [%lu/%lu] passed.", total - failed, total);
    }

    // Results for the tests page of the documentation site.
    if (!nob_mkdir_if_not_exists(DOC_DIR)) return 1;
    test_write_json(&parse_records, &exec_records, DOC_DIR"/tests.json");


    NOB_FREE(cmd.items);
    arena_free(&arena_strings);

    return failed > 0;
}

// int record_command(int *argc, char ***argv) {
//     if (!nob_mkdir_if_not_exists(SRC_DIR)) return 1;
//     if (!nob_mkdir_if_not_exists(TESTS_DIR)) return 1;
//     if (!nob_mkdir_if_not_exists(BIN_DIR)) return 1;
// 
//     Nob_Cmd cmd = {0};
// 
//     if (*argc == 0) {
// 
//         if (!confirm("Are you sure you want to re-record ALL tests again?")) return 1;
//         
//         // Rerecord all available tests.
//         if (!nob_walk_dir(TESTS_DIR, test_record_entry, .data = &cmd)) 
//             return 1;
//     } else {
//         // Use test files provided by the arguments.
//         while (*argc > 0) {
//             const char *file = nob_shift(*argv, *argc);
// 
//             test_record(file, &cmd);
//         }
//     }   
//     
//     NOB_FREE(cmd.items);
// 
//     return 0;
// }

int how_to_command(int *argc, char ***argv) {
    arena_strings = arena_make(1024);
    Test_Records records = {0};

    int failed = 0;

    // Executing tests
    Nob_Cmd cmd = {0};

    // Find all available how to and execute the,.
    if (!nob_walk_dir(HOW_TO_DIR, how_to_execute_entry, .data = &cmd)) return 1;
    
    return 0;
}

int clean_command(int *argc, char ***argv) {
    Nob_Cmd cmd = {0};

    clean(&cmd);
    
    NOB_FREE(cmd.items);

    return 0;
}

int lib_command(int *argc, char ***argv) {
    Nob_Cmd cmd = {0};

    clean(&cmd);

    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"OBJ_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(BIN_DIR)) return 1;

    compile_static_lib(&cmd);
    
    NOB_FREE(cmd.items);

    return 0;
}

int doc_command(int *argc, char ***argv) {

    if (!nob_mkdir_if_not_exists(DOC_DIR)) return 1;

    const char *json_path = DOC_DIR"/doc.json";
    FILE *json = fopen(json_path, "wb");
    if (json == NULL) {
        nob_log(NOB_ERROR, "Could not open '%s' for writing: %s", json_path, strerror(errno));
        return 1;
    }
    fputs("[\n", json);
    size_t entry_count = 0;


    // Making string builder and opening file.
    Nob_String_Builder sb = {0};
    if (!nob_read_entire_file("cnotes.h", &sb)) return 1;

    Nob_String_View content = nob_sb_to_sv(sb);

    // Comments about the library as a whole go into their own json.
    if (!doc_write_library_json(content, DOC_DIR"/library.json")) return 1;

    // Cutting content down to the needed size.
    content = doc_get_sv_till(content, DOC_SV_LIT("#ifdef CN_IMPLEMENTATION"));

    // Every doc comment is followed by declaration it documents, unless it is a section banner,
    // which switches the module of all following entries.
    Nob_String_View    module       = DOC_SV_LIT("misc");
    Nob_String_Builder signature_sb = {0};  // Cleared for every entry, it is printed right away.
    Doc_Entry          doc_entry    = {0};

    while (true) {
        Nob_String_View doc = doc_get_next_comment(&content);
        if (doc.data == NULL) break;

        Nob_String_View section = doc_get_section(doc);
        if (section.count > 0) {
            module = section;
            continue;
        }

        Nob_String_View decl = doc_get_declaration(&content);

        Doc_Kind        kind;
        Nob_String_View symbol;
        if (!doc_classify_declaration(decl, &kind, &symbol)) continue;

        if (!doc_sv_has_library_prefix(symbol)) {
            nob_log(NOB_WARNING, "Documented symbol '%.*s' is not prefixed with 'cn_', 'Cn_' or 'CN_', skipping it.", DOC_SV_UNPACK(symbol));
            continue;
        }

        // Internal `cn__` declarations are not documented.
        if (symbol.data[3] == '_') continue;

        Nob_String_View signature = doc_make_signature(decl, kind, &signature_sb);
        if (!doc_parse_entry(symbol, signature, doc, &doc_entry)) return -1;
        doc_entry.kind   = kind;
        doc_entry.module = module;

        if (entry_count > 0) fputs(",\n", json);
        doc_print_json_entry(json, &doc_entry);
        entry_count++;

        // Tags are appended by doc_parse_entry into a fresh list every time.
        nob_da_free(doc_entry.tags);
    }

    fputs(entry_count > 0 ? "\n]\n" : "]\n", json);
    fclose(json);
    nob_log(NOB_INFO, "Wrote %zu documentation entries to '%s'.", entry_count, json_path);

    nob_sb_free(signature_sb);

    nob_sb_free(sb);

    return 0;
}

int all_command(int *argc, char ***argv) {
    int    argc_ = 0;
    char **argv_ = NULL;

    clean_command(&argc_, &argv_);
    if (lib_command(&argc_, &argv_) != 0) return 1;
    if (test_command(&argc_, &argv_) != 0) return 1;
    if (how_to_command(&argc_, &argv_) != 0) return 1;

    return 0;
}



int main(int argc, char **argv) {
    nob_set_log_handler(nob_cancer_log_handler);

    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "cnotes.h");

    const char *program_name = shift(argv, argc);

    if (argc == 0) {
        return all_command(&argc, &argv);
    }

    const char *command_name;
    while (argc > 0) {
        command_name = nob_shift(argv, argc);

        for (int i = 0; i < NOB_ARRAY_LEN(commands); i++) {
            if (strcmp(command_name, commands[i].name) == 0) {
                if (commands[i].func(&argc, &argv) != 0) 
                    return 1;
                goto continue_outer;
            }
        }

        nob_log(NOB_ERROR, "unknown command: '%s'.", command_name);
        commands_list();
continue_outer:
        ;
    }   

    return 0;
}


