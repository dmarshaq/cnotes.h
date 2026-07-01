// Test: Sizeof expressions
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "struct Point { int x; int y; };\n"
    "\n"
    "int test_sizeof(void) {\n"
    "    int x;\n"
    "    int arr[10];\n"
    "    struct Point p;\n"
    "\n"
    "    x = sizeof(int);\n"
    "    x = sizeof(char);\n"
    "    x = sizeof(float);\n"
    "    x = sizeof(double);\n"
    "    x = sizeof(void *);\n"
    "\n"
    "    x = sizeof(x);\n"
    "    x = sizeof(arr);\n"
    "    x = sizeof(p);\n"
    "    x = sizeof(struct Point);\n"
    "\n"
    "    x = sizeof(int *);\n"
    "    x = sizeof(int **);\n"
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
