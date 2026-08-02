// Test: Cast expression parsing
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int test_casts(void) {\n"
    "    int i = 42;\n"
    "    float f = 3.14;\n"
    "    char c = 65;\n" // NOTE: Char literals are not supported yet.
    "    void *ptr;\n"
    "\n"
    "    f = (float)i;\n"
    "    i = (int)f;\n"
    "    c = (char)i;\n"
    "    i = (int)c;\n"
    "\n"
    "    ptr = (void *)&i;\n"
    "    i = *(int *)ptr;\n"
    "\n"
    "    return i;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = {0};
    cn_tu_init(&tu, "input.i", .source = src, .flags = CN_TU_NO_CODE_OUTPUT);
    cn_tu_process(&tu);
    return 0;
}
