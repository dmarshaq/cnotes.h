// Test: Operator precedence parsing
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int test_precedence(int a, int b, int c) {\n"
    "    int x;\n"
    "\n"
    "    x = a + b * c;\n"
    "    x = (a + b) * c;\n"
    "    x = a * b + c;\n"
    "\n"
    "    x = a + b - c;\n"
    "    x = a - b + c;\n"
    "\n"
    "    x = a << 2 + 1;\n"
    "    x = (a << 2) + 1;\n"
    "\n"
    "    x = a & b | c;\n"
    "    x = a | b & c;\n"
    "\n"
    "    x = a && b || c;\n"
    "    x = a || b && c;\n"
    "\n"
    "    x = a > 0 ? b : c;\n"
    "    x = a > 0 ? b > 0 ? 1 : 2 : 3;\n"
    "\n"
    "    return x;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    return 0;
}
