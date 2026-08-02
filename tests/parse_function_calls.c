// Test: Function call parsing
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int add(int a, int b) { return a + b; }\n"
    "int negate(int x) { return -x; }\n"
    "void noop(void) { }\n"
    "\n"
    "int test_calls(void) {\n"
    "    int x;\n"
    "    x = add(1, 2);\n"
    "    x = negate(x);\n"
    "    noop();\n"
    "    x = add(add(1, 2), add(3, 4));\n"
    "    return x;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = {0};
    cn_tu_init(&tu, "input.i", .source = src, .flags = CN_TU_NO_CODE_OUTPUT);
    cn_tu_process(&tu);
    return 0;
}
