// Test: Basic function definition parsing
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int add(int a, int b) {\n"
    "    return a + b;\n"
    "}\n"
    "\n"
    "void empty_function(void) {\n"
    "}\n"
    "\n"
    "static inline int static_func(void) {\n"
    "    return 42;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;

    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    cn_tu_free(&tu);

    return 0;
}
