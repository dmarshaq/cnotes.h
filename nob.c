#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-g", "-Wall", "-Wextra", "-std=c99")

#define NOB_IMPLEMENTATION
#include "nob.h"

#define CN_IMPLEMENTATION
#include "cnotes.h"

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

#define BUILD_DIR   "build"
#define OBJ_DIR     "obj"
#define BIN_DIR     "bin"
#define SRC_DIR     "src"
#define TESTS_DIR   "tests"

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
    if (output_path_length < 0 && output_path_length >= sizeof(output_path_buffer)) {
        nob_log(NOB_ERROR, "test: test path is too long exceeds %lu buffer size.", sizeof(output_path_buffer));
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


    if (!nob_cmd_run(cmd, .stdout_path = stdout_path)) {
        r->status = RUNTIME_FAIL;
        return;
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
int cn_command(int *argc, char ***argv);

const static Command commands[] = {
    { "help",       "",             "List all available commands.", help_command },
    { "test",       "[FILE...]",    "Run the tests and check their output.", test_command },
    { "record",     "[FILE...]",    "Record file as a test and generate it's expected output.", record_command },
    { "clean",      "",             "Recursivly deletes "BUILD_DIR"/ and "BIN_DIR"/ directories.", clean_command },
    { "lib",        "",             "Will compile whole library into .o file and then produce static library.", lib_command },
    { "cn",         "",             "Cleans, compiles library, runs tests, and uses library with compiler to pre-process and compile main.c file.", cn_command },
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

    nob_da_foreach(Test_Record, r, &records) {
        test_execute(r, &cmd);
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
        
            case SUCCESS:
                fprintf(stderr, "\033[1;32mSUCCESS\033[0m       ");
                break;
        }
    
        fprintf(stderr, "\n");
    }

    NOB_FREE(cmd.items);
    arena_free(&arena_strings);

    return 0;
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

int msg_handler(Cn_Message_Kind kind, void *message) {
    switch (kind) {
        case CN_MESSAGE_PARSED_FUNCTION_DEFINITION:
            {   
                Cn_Message_Parsed_Function_Definition *m = message;


                //  Cn_Ast_Node *node = cn_ast_node_get(m->node_idx);

                //  node = cn_ast_node_get(node->function_definition.declarator_idx);
                //  node = cn_ast_node_get(node->declarator.direct_declarator_idx);
                //  node = cn_ast_node_get(node->direct_declarator.function.direct_declarator_idx);

                //  Cn_String foo = CN_CSTR("foo");
                //  if (cn_str_equals(&node->direct_declarator.identifier, &foo)) {
                //      node->direct_declarator.identifier = CN_CSTR("bar");
                //      return 1;
                //  }
                            
                return 0;
            }
        default: 
            return 0;
    }

    return 0;
}

int cn_command(int *argc, char ***argv) {
    int    argc_ = 0;
    char **argv_ = NULL;

    clean_command(&argc_, &argv_);
    if (lib_command(&argc_, &argv_) != 0) return 1;
    if (test_command(&argc_, &argv_) != 0) return 1;

    Nob_Cmd cmd = {0};

    // Compiling main.i
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cmd_append(&cmd, "-E");
    nob_cc_inputs(&cmd, "main.c");
    nob_cc_output(&cmd, "main.i");

    if (!nob_cmd_run(&cmd)) {
        nob_log(NOB_ERROR, "couldn't compile main intermediate file.");
    }

    // Library pre-processing.
    cn_message_handler = msg_handler;
    Cn_Translation_Unit tu = cn_tu_make("main.i");
    
    // if (cn_tu_process(&tu, CN_PRINT_BINDINGS | CN_PRINT_TYPES | CN_PRINT_AST) == -1) {
    if (cn_tu_process(&tu, CN_PRINT_TOKENS) == -1) {
        cn_tu_free(&tu);
        return 1;
    }

    cn_tu_free(&tu);
    
    // Compiling main executable.
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_inputs(&cmd, "main.i");
    nob_cc_output(&cmd, "main");

    if (!nob_cmd_run(&cmd)) {
        nob_log(NOB_ERROR, "couldn't produce main executable.");
    }

    NOB_FREE(cmd.items);

    return 0;
}



int main(int argc, char **argv) {
    
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "cnotes.h");
    
    const char *program_name = shift(argv, argc);
    
    if (argc == 0) {
        return cn_command(&argc, &argv);
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
    }   

    return 0;
}


