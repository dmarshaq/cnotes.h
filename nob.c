#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-Wall", "-Wextra", "-std=c99")

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






#define STDOUT_TXT_FILE_EXTENSION   ".stdout.txt"

#define BUILD_DIR   "build"
#define SRC_DIR     "src"
#define TESTS_DIR   "tests"


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
        nob_delete_file(test_path);
        NOB_FREE(sb.items);
        return;
    }
    output_path_buffer[output_path_length - 2] = '\0';
    
    nob_cc(cmd);
    nob_cc_flags(cmd);

    nob_cc_output(cmd, output_path_buffer);
    nob_cc_inputs(cmd, test_path);

    if (!nob_cmd_run(cmd)) {
        nob_delete_file(test_path);
        NOB_FREE(sb.items);
        return;
    }

    nob_cmd_append(cmd, output_path_buffer);

    char stdout_path[strlen(output_path_buffer) + strlen(STDOUT_TXT_FILE_EXTENSION) + 1];
    stdout_path[0] = '\0';
    strcat(stdout_path, output_path_buffer);
    strcat(stdout_path, STDOUT_TXT_FILE_EXTENSION);

    if (!nob_cmd_run(cmd, .stdout_path = stdout_path)) {
        nob_delete_file(test_path);
        nob_delete_file(output_path_buffer);
        NOB_FREE(sb.items);
        return;
    }

    nob_delete_file(output_path_buffer);
    NOB_FREE(sb.items);
    nob_log(NOB_INFO, "record: successfuly recorded '%s' file.", test_path);
}

/**
 * TODO: Compare .stdout.txt outputs to verify test. Write description.
 */
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

const static Command commands[] = {
    { "help",   "",         "List all available commands.", help_command },
    { "test",   "file...",  "Run the tests and check their output.", test_command },
    { "record", "file...",  "Record file as a test and generate it's expected output.", record_command },
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

    if (!nob_mkdir_if_not_exists(SRC_DIR)) return 1;
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
    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"TESTS_DIR)) return 1;

    Nob_Cmd cmd = {0};

    if (*argc == 0) {
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



int main(int argc, char **argv) {
    
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "cnotes.h");
    
    const char *program_name = shift(argv, argc);
    
    if (argc == 0) {
        return test_command(&argc, &argv);
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




