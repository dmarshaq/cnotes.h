#define DEV

// Defining flags.
#ifdef DEV
#   define nob_cc_flags(cmd)    nob_cmd_append(cmd, "-g", "-O0", "-Wno-override-init-side-effects", "-Wno-format")
#else
#   define nob_cc_flags(cmd)    nob_cmd_append(cmd, "-O2")
#endif // DEV

#define nob_cc(cmd) nob_cmd_append(cmd, "gcc")

// Defining libs.
#define nob_cc_libs(cmd)    nob_cmd_append(cmd, "-lm")


// Defining project paths.
#define BIN_DIR     "bin"
#define BUILD_DIR   "build"
#define SRC_DIR     "src"





// Defining includes path.
#define nob_cc_includes(cmd)    nob_cmd_append(cmd, "-I"SRC_DIR)




#define NOB_IMPLEMENTATION
#include "nob.h"




// Making a buffer to save important strings.
#define STRINGS_BUFFER_CAPACITY 2048
static char strings_buffer[STRINGS_BUFFER_CAPACITY];
static char *strings_buffer_write = strings_buffer;

char *save_string(char *str) {
    if (strings_buffer_write + strlen(str) - strings_buffer > STRINGS_BUFFER_CAPACITY) return NULL;

    char *result = memcpy(strings_buffer_write, str, strlen(str) + 1);
    strings_buffer_write = result + strlen(str) + 1;
    return result;
}

void reset_saved_strings() {
    strings_buffer_write = strings_buffer;
}






bool nob_cmd_append_all_in_dir(Nob_Cmd *cmd, const char *directory, const char *file_format) {
    // Finding specific paths variables.
    Nob_File_Paths paths = {0};
    char file_path_buffer[strlen(directory) + 1 + 128];

    strcpy(file_path_buffer, directory);
    strcat(file_path_buffer, "/");

    if (!nob_read_entire_dir(directory, &paths)) return false;

    for (size_t i = 0; i < paths.count; i++) {
        // Everytime I am doing null terminated strings I want to jump off a bridge... into the ocean water...
        strncat(file_path_buffer, paths.items[i], 128);

        // Taking only .c files.
        if (nob_get_file_type(file_path_buffer) == NOB_FILE_REGULAR && strcmp(strrchr(file_path_buffer, '.'), file_format) == 0) {
            
            char *saved = save_string(file_path_buffer);
            if (saved == NULL) {
                nob_log(NOB_ERROR, "Couldn't save string, not enough space.");
                return false;
            }

            nob_cmd_append(cmd, saved);
        }

        file_path_buffer[strlen(directory) + 1] = '\0';
    }

    return true;
}


/**
 * Compiles all .c files inside directory into .o files.
 */
bool nob_cc_compile_objs(Nob_Cmd *cmd, const char *source, const char *output) {

    Nob_String_Builder src_dir_builder = {0};

    Nob_String_Builder obj_dir_builder = {0};

    Nob_File_Paths paths = {0};
    if (!nob_read_entire_dir(source, &paths)) return false;

    for (size_t i = 0; i < paths.count; i++) {
        if (strcmp(paths.items[i], ".") == 0) continue;
        if (strcmp(paths.items[i], "..") == 0) continue;

        // Building path for source.
        nob_sb_append_cstr(&src_dir_builder, source);
        nob_sb_append_cstr(&src_dir_builder, "/");
        nob_sb_append_cstr(&src_dir_builder, paths.items[i]);
        nob_sb_append_null(&src_dir_builder);

        // Taking only .c source files.
        if (nob_get_file_type(src_dir_builder.items) == NOB_FILE_REGULAR && strcmp(strrchr(src_dir_builder.items, '.'), ".c") == 0) {

            // Building path for output.
            nob_sb_append_cstr(&obj_dir_builder, output);
            nob_sb_append_cstr(&obj_dir_builder, "/");
            nob_sb_append_cstr(&obj_dir_builder, paths.items[i]);
            obj_dir_builder.items[obj_dir_builder.count - 1] = 'o'; // Replacing 'c' with 'o'.
            nob_sb_append_null(&obj_dir_builder);



            nob_cc(cmd);
            nob_cc_flags(cmd);
            nob_cmd_append(cmd, "-c", src_dir_builder.items, "-o", obj_dir_builder.items);
            nob_cc_includes(cmd);

            if (!nob_cmd_run_sync_and_reset(cmd)) return 1;



            obj_dir_builder.count = 0;
        }

        src_dir_builder.count = 0;
    }


    nob_sb_free(src_dir_builder);
    nob_sb_free(obj_dir_builder);

    return true;
}




/**
 * @Important: Build nob one time and run it.
 *
 *      $ cc -o nob nob.c
 *      $ ./nob
 *
 * Then just run it like usually, thanks to NOB_GO_REBUILD_URSELF.
 *
 *      $ ./nob
 *
 */
int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    // Create basic dirs if they don't exist.
    if (!nob_mkdir_if_not_exists(BUILD_DIR))        return 1;
    if (!nob_mkdir_if_not_exists(BIN_DIR))          return 1;
    if (!nob_mkdir_if_not_exists(SRC_DIR))          return 1;






    // Cleaning.
    // @Important: Each build fully clean rebuilds binaries and build files.
    nob_cmd_append(&cmd, "rm", "-rf", BIN_DIR"/*");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "rm", "-rf", BUILD_DIR"/*");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;


    // Making obj dir for .o files.
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/obj"))  return 1;

    // Main building will be done in build/src, cause meta files are generated there.
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"SRC_DIR)) return 1;


    // Compile core c files into core.o.
    if (!nob_mkdir_if_not_exists(SRC_DIR"/core"))       return 1;
    if (!nob_mkdir_if_not_exists(BUILD_DIR"/obj/core")) return 1;

    nob_cc_compile_objs(&cmd, SRC_DIR"/core", BUILD_DIR"/obj/core");


    // Link all obj's into static lib.
    nob_cmd_append(&cmd, "ar", "rcs", BIN_DIR"/libcore.a");
    nob_cmd_append_all_in_dir(&cmd, BUILD_DIR"/obj/core", ".o");
    nob_cc_libs(&cmd);

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    reset_saved_strings();





    // Building meta.exe
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_output(&cmd, BIN_DIR"/meta.exe");
    nob_cc_includes(&cmd);
    nob_cmd_append_all_in_dir(&cmd, SRC_DIR"/meta", ".c");

    nob_cmd_append(&cmd, "-L"BIN_DIR, "-lcore");
    nob_cc_libs(&cmd);

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    reset_saved_strings();


    // Running meta.exe
    nob_cmd_append(&cmd, BIN_DIR"/meta.exe");
    nob_cmd_append(&cmd, "-in");
    nob_cmd_append_all_in_dir(&cmd, SRC_DIR"/main", ".c");
    nob_cmd_append_all_in_dir(&cmd, SRC_DIR"/main", ".h");
    nob_cmd_append(&cmd, "-out", BUILD_DIR);

    if (!nob_mkdir_if_not_exists(BUILD_DIR"/"SRC_DIR"/main")) return 1;

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    reset_saved_strings();






    // The following is needed because includes will be specified relative to build/src directory so core headers, need to be copied over to the build.
    // This solutions is cursed, but oh well...
    if (!nob_copy_directory_recursively(SRC_DIR"/core", BUILD_DIR"/"SRC_DIR"/core")) return 1;


    // // Building main
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_output(&cmd, BIN_DIR"/main");
    nob_cmd_append(&cmd, "-I"BUILD_DIR"/"SRC_DIR);

    // nob_cmd_append(&cmd, BUILD_DIR"/"SRC_DIR"/meta_generated.c");
    nob_cmd_append_all_in_dir(&cmd, BUILD_DIR"/"SRC_DIR"/main", ".c");

    nob_cmd_append(&cmd, "-L"BIN_DIR, "-lcore");
    nob_cc_libs(&cmd);

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    reset_saved_strings();

    return 0;
}

