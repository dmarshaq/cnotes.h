#include "core/core.h"
#include "core/str.h"

#include "meta/meta.h"


/**
 *  How to use:
 *
 *      $ meta.exe -in ... -out ...
 *
 */
int main(int argc, char **argv) {

    int input_files_count = 0;
    char **input_files = NULL;

    char *output_path = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-in") == 0) {
            i++;
            input_files = argv + i;
            for (; i < argc; i++) {
                if (argv[i][0] == '-') {
                    i--;
                    break;
                }
                input_files_count++;
            }

        } else if (strcmp(argv[i], "-out") == 0) {
    
            if (i + 1 < argc) {
                i++;
                output_path = argv[i];
            }

        } else {
            META_LOG_ERROR("Unknown command line option: '%s'.", argv[i]);
            return 1;
        }
    }

    if (input_files_count == 0) {
        META_LOG_ERROR("Input files are not specified.");
        return 1;
    }


    if (output_path == NULL) {
        META_LOG_ERROR("Output path is not specified.");
        return 1;
    }

    if (meta_process(input_files_count, input_files, output_path) != 0) return 1;
    
    return 0;
}
