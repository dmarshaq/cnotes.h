// Test: Compound literal expression parsing
#include "../testing_utility.h"
const Cn_String src = CN_STR_BUFFER(
    "struct point { int x; int y; };\n"
    "\n"
    "int test_compound_literals(void) {\n"
    "    int i;\n"
    "    int *pi;\n"
    "    float f;\n"
    "    struct point p;\n"
    "    struct point *pp;\n"
    "\n"
    "    i = (int){42};\n"
    "    f = (float){3.14};\n"
    "\n"
    "    pi = (int[3]){1, 2, 3};\n"
    "    i = pi[1];\n"
    "\n"
    "    p = (struct point){.x = 1, .y = 2};\n"
    "    i = p.x + p.y;\n"
    "\n"
    "    pp = &(struct point){10, 20};\n"
    "    i = pp->x + pp->y;\n"
    "\n"
    "    return i;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    return 0;
}
