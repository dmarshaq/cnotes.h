#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-g", "-Wall", "-Wextra", "-std=c99")

#define NOB_IMPLEMENTATION
#include "nob.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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

// Async proc test setup to detect timeout and report.
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

typedef enum : uint8_t {
    PENDING = 0,
    BUILD_FAIL,
    RUNTIME_FAIL,
    UNEXPECTED_OUTPUT,
    TIMEOUT,
    SUCCESS,
} Test_Status;

typedef struct {
    const char *path;
    Test_Status status;
} Test_Record;

typedef struct {
    Test_Record *items;
    size_t count;
    size_t capacity;
} Test_Records;

bool test_prepare_entry(Nob_Walk_Entry entry) {
    Test_Records *records = (Test_Records *)entry.data;
    if (entry.type == FILE_REGULAR && strcmp(entry.path + strlen(entry.path) - 2, ".c") == 0) {
        char *saved_path = arena_alloc(&arena_strings, strlen(entry.path) + 1);
        strcpy(saved_path, entry.path);
        nob_da_append(records, ((Test_Record) { .path = saved_path, .status = PENDING }));
        nob_log(NOB_INFO, "test: added '%s' test.", entry.path);
    }

    return true;
}

void test_record(const char *source, Nob_Cmd *cmd);

bool test_record_entry(Nob_Walk_Entry entry) {
    Nob_Cmd *cmd = (Nob_Cmd *)entry.data;
    if (entry.type == FILE_REGULAR && strcmp(entry.path + strlen(entry.path) - 2, ".c") == 0) {
        test_record(entry.path, cmd);
    }

    return true;
}

/**
 * This function does following things:
 * First it copies the source file .c into tests directory, compiles and runs it there. 
 * Generating .stdout.txt which contains information about expected output.
 * After that it cleans up generated executable leaving only expected .stdout.txt.
 * TODO: Sanitize test files.
 */
void test_record(const char *source, Nob_Cmd *cmd) {
    Nob_String_Builder sb = {0};

    if (strcmp(source + strlen(source) - 2, ".c") != 0) {
        nob_log(NOB_ERROR, "record: '%s' is not a C file.", source);
        return;
    }

    if (!nob_read_entire_file(source, &sb)) {
        return;
    }

    const char *name = strrchr(source, '/');
    if (name == NULL) name = source;
    else name++;
    
    char test_path[strlen(TESTS_DIR"/") + strlen(name) + 1];
    test_path[0] = '\0';
    strcat(test_path, TESTS_DIR"/");
    strcat(test_path, name);

    if (!nob_write_entire_file(test_path, sb.items, sb.count)) {
        nob_log(NOB_ERROR, "record: couldn't write '%s' file.", test_path);
        NOB_FREE(sb.items);
        return;
    }


    // Copypasta from test_execute.
    char output_path_buffer[64];
    int output_path_length;

    output_path_length = snprintf(output_path_buffer, sizeof(output_path_buffer), "%s", test_path);
    if (output_path_length < 0 && output_path_length >= sizeof(output_path_buffer)) {
        nob_log(NOB_ERROR, "record: test path is too long exceeds %lu buffer size.", sizeof(output_path_buffer));
        NOB_FREE(sb.items);
        return;
    }
    output_path_buffer[output_path_length - 2] = '\0';
    
    nob_cc(cmd);
    nob_cc_flags(cmd);

    nob_cc_output(cmd, output_path_buffer);
    nob_cc_inputs(cmd, test_path);
    nob_cmd_append(cmd, BIN_DIR"/libcnotes.a");

    if (!nob_cmd_run(cmd)) {
        NOB_FREE(sb.items);
        return;
    }

    nob_cmd_append(cmd, output_path_buffer);

    char stdout_path[strlen(output_path_buffer) + strlen(STDOUT_TXT_FILE_EXTENSION) + 1];
    stdout_path[0] = '\0';
    strcat(stdout_path, output_path_buffer);
    strcat(stdout_path, STDOUT_TXT_FILE_EXTENSION);

    if (!nob_cmd_run(cmd, .stdout_path = stdout_path)) {
        nob_delete_file(output_path_buffer);
        NOB_FREE(sb.items);
        return;
    }

    nob_delete_file(output_path_buffer);
    NOB_FREE(sb.items);
    nob_log(NOB_INFO, "record: successfuly recorded '%s' file.", test_path);
}

void test_execute(Test_Record *r, Nob_Cmd *cmd) {
    char output_path_buffer[64];
    int output_path_length;

    output_path_length = snprintf(output_path_buffer, sizeof(output_path_buffer), BUILD_DIR"/%s", r->path);
    if (output_path_length < 0 || output_path_length >= sizeof(output_path_buffer)) {
        nob_log(NOB_ERROR, "test: test path is too long exceeds %lu buffer size.", sizeof(output_path_buffer));
        r->status = BUILD_FAIL;
        return;
    }
    output_path_buffer[output_path_length - 2] = '\0';


    nob_cc(cmd);
    nob_cc_flags(cmd);

    nob_cc_output(cmd, output_path_buffer);
    nob_cc_inputs(cmd, r->path);
    nob_cmd_append(cmd, BIN_DIR"/libcnotes.a");

    if (!nob_cmd_run(cmd)) {
        r->status = BUILD_FAIL;
        return;
    }

    nob_log(NOB_INFO, "test: running '%s' test.", output_path_buffer);

    nob_cmd_append(cmd, output_path_buffer);

    char stdout_path[strlen(output_path_buffer) + strlen(STDOUT_TXT_FILE_EXTENSION) + 1];
    stdout_path[0] = '\0';
    strcat(stdout_path, output_path_buffer);
    strcat(stdout_path, STDOUT_TXT_FILE_EXTENSION);


    Build_Proc proc = {0};
    if (!build_proc_spawn(&proc, cmd, stdout_path)) {
        r->status = RUNTIME_FAIL;
        return;
    }

    switch (build_proc_wait(&proc, TEST_TIMEOUT_MS)) {
    case PROC_TIMEOUT:
        nob_log(NOB_ERROR, "test: '%s' timed out after %d ms",
                output_path_buffer, TEST_TIMEOUT_MS);
        r->status = TIMEOUT;
        return;
    case PROC_FAIL:
        r->status = RUNTIME_FAIL;
        return;
    case PROC_OK:
        break;
    }

    char expected_stdout_path[strlen(r->path) - 2 + strlen(STDOUT_TXT_FILE_EXTENSION) + 1];
    expected_stdout_path[0] = '\0';
    strncat(expected_stdout_path, r->path, strlen(r->path) - 2);
    strcat(expected_stdout_path, STDOUT_TXT_FILE_EXTENSION);
    
    Nob_String_Builder src = {0}, dst = {0};

    if (!read_entire_file(stdout_path, &src)) {
        r->status = UNEXPECTED_OUTPUT;
        return;
    }

    if (!read_entire_file(expected_stdout_path, &dst)) {
        r->status = UNEXPECTED_OUTPUT;
        return;
    }

    Nob_String_View src_sv = sb_to_sv(src);
    Nob_String_View dst_sv = sb_to_sv(dst);

    if (!sv_eq(src_sv, dst_sv)) {
        nob_log(NOB_ERROR, "test: unexpected output!");
        nob_log(NOB_ERROR, "test: expected:");
        fprintf(stderr, SV_Fmt, SV_Arg(dst_sv));
        nob_log(NOB_ERROR, "test: actual:");
        fprintf(stderr, SV_Fmt, SV_Arg(src_sv));
        r->status = UNEXPECTED_OUTPUT;
        return;
    }

    NOB_FREE(dst.items);
    NOB_FREE(src.items);

    r->status = SUCCESS;
}

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
int record_command(int *argc, char ***argv);
int clean_command(int *argc, char ***argv);
int lib_command(int *argc, char ***argv);
int doc_command(int *argc, char ***argv);
int all_command(int *argc, char ***argv);

const static Command commands[] = {
    { "help",       "",             "List all available commands.", help_command },
    { "test",       "[FILE...]",    "Run the tests and check their output.", test_command },
    { "record",     "[FILE...]",    "Record file as a test and generate it's expected output.", record_command },
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
    if (!nob_mkdir_if_not_exists(TESTS_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"TESTS_DIR)) return 1;

    arena_strings = arena_make(1024);
    Test_Records records = {0};

    if (*argc == 0) {
        // Find all available tests.
        if (!nob_walk_dir(TESTS_DIR, test_prepare_entry, .data = &records)) 
            return 1;

    } else {
        // Use test files provided by the arguments.
        while (*argc > 0) {
            const char *file = nob_shift(*argv, *argc);

            // Validating file.
            if (strncmp(TESTS_DIR"/", file, strlen(TESTS_DIR"/")) != 0 && (strrchr(file, '/') - file) != strlen(TESTS_DIR)) {
                nob_log(NOB_ERROR, "test: invalid input file '%s'.", file);
                return 1;
            }

            // Reusing test_prepare_entry because why not.
            if (!test_prepare_entry((Nob_Walk_Entry) { .path = file, .type = FILE_REGULAR, .data = &records })) {
                return 1;
            }
        }
    }

    // Executing tests
    Nob_Cmd cmd = {0};

    int failed = 0;
    
    nob_da_foreach(Test_Record, r, &records) {
        test_execute(r, &cmd);
        if (r->status != SUCCESS) failed += 1;
    }
    
    // Showing test results
    nob_da_foreach(Test_Record, r, &records) {
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
    
    if (failed > 0) {
        nob_log(NOB_ERROR, "test: [%lu/%lu] passed.", records.count - failed, records.count);
    } else {
        nob_log(NOB_INFO, "test: [%lu/%lu] passed.", records.count - failed, records.count);
    }


    NOB_FREE(cmd.items);
    arena_free(&arena_strings);

    return failed > 0;
}

int record_command(int *argc, char ***argv) {
    if (!nob_mkdir_if_not_exists(SRC_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(TESTS_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(BIN_DIR)) return 1;

    Nob_Cmd cmd = {0};

    if (*argc == 0) {

        if (!confirm("Are you sure you want to re-record ALL tests again?")) return 1;
        
        // Rerecord all available tests.
        if (!nob_walk_dir(TESTS_DIR, test_record_entry, .data = &cmd)) 
            return 1;
    } else {
        // Use test files provided by the arguments.
        while (*argc > 0) {
            const char *file = nob_shift(*argv, *argc);

            test_record(file, &cmd);
        }
    }   
    
    NOB_FREE(cmd.items);

    return 0;
}

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

#define DOC_SV_LIT(literal)             ((Nob_String_View) {sizeof(literal) - 1, literal})
#define DOC_SV_UNPACK(str)              (int)((str).count), (str).data

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



#define DOC_TAG_GEN_LIST(X)\
    X(RETURNS,      Returns,    returns)\
    X(PARAM,        Param,      param)\
    X(OUTPUTS,      Outputs,    outputs)\
    X(SINCE,        Since,      since)\
    X(NOTE,         Note,       note)\
    X(IMPORTANT,    Important,  important)\
    X(AI,           AI,         ai)\
    X(TODO,         Todo,       todo)\
    X(LEAK,         Leak,       leak)\
    X(TEMPORARY,    Temporary,  temporary)\
    X(FIXME,        Fixme,      fixme)\

typedef enum {
#define X(K, T, m) K,
    DOC_TAG_GEN_LIST(X)
#undef X
    UNKNOWN,
} Doc_Tag_Kind;

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

typedef struct {
    Doc_Tag_Kind    kind;
    Nob_String_View text;
} Doc_Tag;

typedef struct {
    Doc_Tag *items;
    size_t count;
    size_t capacity;
} Doc_Tags;

typedef struct {
    Nob_String_View symbol;
    Nob_String_View decl;
    Nob_String_View summary;
    Nob_String_View description;
    Doc_Tags        tags;
} Doc_Entry;

/**
 * Checks whether sv is a C identifier: letter or '_', followed by letters, digits or '_'.
 */
bool doc_sv_is_identifier(Nob_String_View sv) {
    if (sv.count == 0 || !(isalpha((unsigned char)sv.data[0]) || sv.data[0] == '_')) return false;
    for (size_t i = 1; i < sv.count; i++) {
        if (!(isalnum((unsigned char)sv.data[i]) || sv.data[i] == '_')) return false;
    }
    return true;
}

typedef enum {
    DOC_TARGET_NONE,
    DOC_TARGET_SUMMARY,
    DOC_TARGET_TAG,
    DOC_TARGET_DESCRIPTION,
} Doc_Target;

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
    fputs(entry->tags.count > 0 ? "\n    ]\n" : "]\n", f);

    fputs("  }", f);
}


int doc_command(int *argc, char ***argv) {

    Nob_String_Builder sb = {0};
    if (!nob_read_entire_file("cnotes.h", &sb)) return 1;

    if (!nob_mkdir_if_not_exists(DOC_DIR)) {
        nob_sb_free(sb);
        return 1;
    }

    const char *json_path = DOC_DIR"/doc.json";
    FILE *json = fopen(json_path, "wb");
    if (json == NULL) {
        nob_log(NOB_ERROR, "Could not open '%s' for writing: %s", json_path, strerror(errno));
        nob_sb_free(sb);
        return 1;
    }
    fputs("[\n", json);
    size_t entry_count = 0;

    Nob_String_View content = nob_sb_to_sv(sb);


    // Cutting content down to the needed size.
    content = doc_get_sv_till(content, DOC_SV_LIT("#ifdef CN_IMPLEMENTATION"));

    // Will be used on every doc. Cleared on every new doc.
    Nob_String_Builder doc_text_sb = {0};           // Summary and tag texts, back to back.
    Nob_String_Builder doc_description_sb = {0};
    Doc_Entry doc_entry = {0};

    while (true) {
        // Getting doc comment.
        content = doc_trim_sv_till(content, DOC_SV_LIT("/**"));
        if (content.data == NULL) break;

        // Chopping /** away.
        nob_sv_chop_left(&content, 3);

        // Getting doc text.
        Nob_String_View doc = doc_get_sv_till(content, DOC_SV_LIT("*/"));
        nob_sv_chop_left(&content, doc.count + 2); // +2 for "*/".

        // Getting declaration.
        Nob_String_View decl = content;
        decl = nob_sv_trim_left(decl);  // Removing any white space (newline, etc...).

        decl = doc_get_sv_till(decl, DOC_SV_LIT("\n"));

        // Skipping declarations without CNDEF.
        if (!nob_sv_starts_with(decl, DOC_SV_LIT("CNDEF"))) {
            continue;
        }

        // Geting name based on `cn_` and detecting if it is a function, if not skipping it too.
        // Internal `cn__` names are omitted too.
        Nob_String_View symbol = decl;
        symbol = doc_trim_sv_till(symbol, DOC_SV_LIT("cn_"));

        // Gettting symbol based on prefix.
        if (symbol.data == NULL) {
            nob_log(NOB_WARNING, "Non-prefixed symbol name, is not detected in declaration: '%.*s'.", DOC_SV_UNPACK(decl));
            continue;
        }

        // Internal declaration skipped.
        if (symbol.count < 4 || symbol.data[3] == '_') {
            continue;
        }

        // Getting end of the symbol.
        Nob_String_View symbol_end = doc_trim_sv_till(symbol, DOC_SV_LIT("("));
        if (symbol_end.data == NULL) {
            symbol_end = doc_trim_sv_till(symbol, DOC_SV_LIT(";"));
            if (symbol_end.data == NULL) {
                nob_log(NOB_WARNING, "Unidentified symbol name, '(' or ';' is not detected in declaration: '%.*s'.", DOC_SV_UNPACK(decl));
                continue;
            }
        }
        symbol.count -= symbol_end.count;

        
        // Assembling doc entry, one at a time.
        doc_entry.symbol = symbol;

        // Signature for display: without CNDEF prefix and trailing ';'.
        Nob_String_View signature = decl;
        nob_sv_chop_left(&signature, DOC_SV_LIT("CNDEF").count);
        signature = nob_sv_trim(signature);
        if (signature.count > 0 && signature.data[signature.count - 1] == ';') signature.count--;
        doc_entry.decl = nob_sv_trim_right(signature);
        doc_entry.tags.count = 0;
        doc_entry.summary = (Nob_String_View) {0};
        doc_entry.description = (Nob_String_View) {0};

        doc_text_sb.count = 0;
        doc_description_sb.count = 0;

        // Examining doc line by line.
        // Consecutive lines are joined with '\n' into whatever is currently open: summary, tag or description.
        // A blank line closes it. Text after a blank line goes to description, a tag line always opens a new tag.
        // Inside ``` fences lines are kept verbatim: indentation and blank lines stay, tags are not detected.
        Doc_Target target = DOC_TARGET_NONE;
        bool has_content = false;
        bool in_fence = false;

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
                doc_append_to_target(&doc_entry, &doc_text_sb, &doc_description_sb, target, toggles_fence ? line : raw);
                continue;
            }

            // Blank line closes current summary, tag or description paragraph.
            if (line.count == 0) {
                target = DOC_TARGET_NONE;
                continue;
            }

            // Tag lines have priority, they are checked first.
            Nob_String_View tag_sv = doc_get_sv_till(line, DOC_SV_LIT(":"));
            size_t tag_length = tag_sv.count + 1; // +1 for ':'.

            // "PARAM name: text", name of the parameter is kept in the text as "name: text".
            if (nob_sv_starts_with(tag_sv, DOC_SV_LIT("PARAM ")) && doc_sv_is_identifier(nob_sv_from_parts(tag_sv.data + 6, tag_sv.count - 6))) {
                tag_sv.count = 5;
                tag_length = 6;
            }

            Doc_Tag_Kind tag_kind = is_fence ? UNKNOWN : doc_sv_to_tag(tag_sv);
            if (tag_kind != UNKNOWN) {
                Doc_Tag tag = { .kind = tag_kind };
                nob_da_append(&doc_entry.tags, tag);

                // Removing tag from the line to just add text.
                nob_sv_chop_left(&line, tag_length);
                line = nob_sv_trim_left(line);
                doc_append_line(&doc_text_sb, &nob_da_last(&doc_entry.tags).text.count, line);

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
                    if (doc_description_sb.count != 0) nob_da_append(&doc_description_sb, '\n');
                    target = DOC_TARGET_DESCRIPTION;
                }
            }

            if (toggles_fence) in_fence = true;
            doc_append_to_target(&doc_entry, &doc_text_sb, &doc_description_sb, target, line);
            has_content = true;
        }

        if (in_fence) {
            nob_log(NOB_WARNING, "Unclosed ``` code block in documentation of '%.*s'.", DOC_SV_UNPACK(symbol));
        }

        // Summary and tag texts are stored back to back in doc_text_sb, only counts were recorded,
        // since sb might have been reallocated while appending. Now pointing views into final buffer.
        const char *text_cursor = doc_text_sb.items;
        doc_entry.summary.data = text_cursor;
        text_cursor += doc_entry.summary.count;
        nob_da_foreach(Doc_Tag, tag, &doc_entry.tags) {
            tag->text.data = text_cursor;
            text_cursor += tag->text.count;
        }

        doc_entry.description = nob_sb_to_sv(doc_description_sb);


        if (entry_count > 0) fputs(",\n", json);
        doc_print_json_entry(json, &doc_entry);
        entry_count++;
    }

    fputs(entry_count > 0 ? "\n]\n" : "]\n", json);
    fclose(json);
    nob_log(NOB_INFO, "Wrote %zu documentation entries to '%s'.", entry_count, json_path);

    nob_da_free(doc_entry.tags);
    nob_sb_free(doc_text_sb);
    nob_sb_free(doc_description_sb);

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


