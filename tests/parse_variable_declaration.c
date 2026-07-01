// Test: Variable declaration parsing with various types
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int global_int;\n"
    "static int static_int;\n"
    "extern int extern_int;\n"
    "const int const_int = 100;\n"
    "\n"
    "char c;\n"
    "short s;\n"
    "long l;\n"
    "long long ll;\n"
    "unsigned int ui;\n"
    "signed char sc;\n"
    "\n"
    "float f;\n"
    "double d;\n"
    "\n"
    "int *ptr;\n"
    "int **ptr_ptr;\n"
    "const int *ptr_to_const;\n"
    "int *const const_ptr = 0;\n"
    "int arr[10];\n"
    "int arr_2d[10][5];\n"
    "\n"
    "void func(void) {\n"
    "    int local_var = 5;\n"
    "    register int reg_var = 10;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    cn_tu_free(&tu);

    return 0;
}
