// Test: Initializer, designators and designations.
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "typedef struct vec3 {\n"
    "    int x, y;\n"
    "    int z;\n"
    "} Vec3;\n"
    "\n"
    "int test_simple_initializer(void) {\n"
    "    int x = 5;\n"
    "    int y = 5;\n"
    "    int arr[2] = { 1, [0] = 2, 3 };\n"
    "    Vec3 v = { .y = 3, 4, .x = 0 };\n"
    "    return v.x + v.y + v.z;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    return 0;
}
