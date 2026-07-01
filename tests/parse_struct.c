// Test: Struct and union parsing
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "struct Point {\n"
    "    int x;\n"
    "    int y;\n"
    "};\n"
    "\n"
    "struct Rectangle {\n"
    "    struct Point top_left;\n"
    "    struct Point bottom_right;\n"
    "};\n"
    "\n"
    "union Data {\n"
    "    int i;\n"
    "    float f;\n"
    "    char c;\n"
    "};\n"
    "\n"
    "struct WithPointer {\n"
    "    int *ptr;\n"
    "    struct WithPointer *next;\n"
    "};\n"
    "\n"
    "struct Point global_point;\n"
    "\n"
    "int test_struct_access(void) {\n"
    "    struct Point p;\n"
    "    p.x = 10;\n"
    "    p.y = 20;\n"
    "    return p.x + p.y;\n"
    "}\n"
    "\n"
    "int test_pointer_access(struct Point *p) {\n"
    "    p->x = 5;\n"
    "    p->y = 10;\n"
    "    return p->x + p->y;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;

    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    cn_tu_free(&tu);

    return 0;
}
