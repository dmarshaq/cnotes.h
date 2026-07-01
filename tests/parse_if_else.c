// Test: If/else statement parsing
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int test_if(int x) {\n"
    "    if (x > 0) {\n"
    "        return 1;\n"
    "    }\n"
    "    return 0;\n"
    "}\n"
    "\n"
    "int test_if_else(int x) {\n"
    "    if (x > 0) {\n"
    "        return 1;\n"
    "    } else {\n"
    "        return -1;\n"
    "    }\n"
    "}\n"
    "\n"
    "int test_if_else_if(int x) {\n"
    "    if (x > 0) {\n"
    "        return 1;\n"
    "    } else if (x < 0) {\n"
    "        return -1;\n"
    "    } else {\n"
    "        return 0;\n"
    "    }\n"
    "}\n"
    "\n"
    "int test_nested_if(int x, int y) {\n"
    "    if (x > 0) {\n"
    "        if (y > 0) {\n"
    "            return 1;\n"
    "        }\n"
    "    }\n"
    "    return 0;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;

    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    cn_tu_free(&tu);

    return 0;
}
