#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
        "int main(void) {\n"
        "   return 0\n"             // Missing semicolon.
        "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;

    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);

    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);

    return 0;
}
