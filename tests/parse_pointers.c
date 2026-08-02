// Test: Pointer operations and arithmetic
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int test_pointers(void) {\n"
    "    int x = 10;\n"
    "    int *p = &x;\n"
    "    int **pp = &p;\n"
    "\n"
    "    *p = 20;\n"
    "    **pp = 30;\n"
    "\n"
    "    return *p + **pp;\n"
    "}\n"
    "\n"
    "int test_pointer_arithmetic(int *arr) {\n"
    "    int *p = arr;\n"
    "    int x;\n"
    "\n"
    "    x = *p;\n"
    "    p = p + 1;\n"
    "    x = *p;\n"
    "    p = p - 1;\n"
    "\n"
    "    x = *(arr + 2);\n"
    "    x = arr[3];\n"
    "\n"
    "    return x;\n"
    "}\n"
    "\n"
    "int test_address_of(void) {\n"
    "    int x = 5;\n"
    "    int *p;\n"
    "    p = &x;\n"
    "    return *p;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = {0};
    cn_tu_init(&tu, "input.i", .source = src, .flags = CN_TU_NO_CODE_OUTPUT);
    cn_tu_process(&tu);
    return 0;
}
