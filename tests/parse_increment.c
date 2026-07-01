// Test: Increment/decrement operators
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int test_increment(void) {\n"
    "    int x = 0;\n"
    "    int y;\n"
    "\n"
    "    x++;\n"
    "    ++x;\n"
    "    x--;\n"
    "    --x;\n"
    "\n"
    "    y = x++;\n"
    "    y = ++x;\n"
    "    y = x--;\n"
    "    y = --x;\n"
    "\n"
    "    return x + y;\n"
    "}\n"
    "\n"
    "int test_pointer_increment(int *p) {\n"
    "    p++;\n"
    "    ++p;\n"
    "    p--;\n"
    "    --p;\n"
    "    return *p;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    return 0;
}
