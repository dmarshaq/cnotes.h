#include "../testing_utility.h"

int main(int argc, char **argv) {
    fprintf(stderr, "==================== GNU RUNNER ===================\n");

    cn_diagnostic_handler = &cn_test_diagnostic_handler;

    argc--;
    argv++;

    if (argc != 1) {
        fprintf(stderr, "runner: expected parsing test input.");
        return -1;
    }


    Cn_Translation_Unit tu = {0};
    cn_tu_init(&tu, argv[0], .flags = CN_TU_NO_CODE_OUTPUT);
    cn_tu_process(&tu);
    return 0;
}

