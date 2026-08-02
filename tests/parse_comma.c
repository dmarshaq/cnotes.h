// Test: Comma operator
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int test_comma(void) {\n"
    "    int a, b, c;\n"
    "    a = 1, b = 2, c = 3;\n"
    "    return (a, b, c);\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = {0};
    cn_tu_init(&tu, "input.i", .source = src, .flags = CN_TU_NO_CODE_OUTPUT);
    cn_tu_process(&tu);
    return 0;
}
