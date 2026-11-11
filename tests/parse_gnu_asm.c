// Test: GNU asm statements - basic, extended, qualifiers, operands, clobbers, goto labels, file-scope asm.
// AI: Generated.
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    /* --- file-scope asm --------------------------------------------------- */
    "asm(\".globl file_scope_symbol\");\n"
    "__asm__(\".text\");\n"
    "__asm(\"nop\" \"\\n\\t\" \"nop\");\n"
    "\n"
    /* --- basic asm -------------------------------------------------------- */
    "void test_basic_asm(void) {\n"
    "    asm(\"nop\");\n"
    "    __asm__(\"nop\");\n"
    "    __asm(\"nop\");\n"
    "    asm volatile(\"nop\");\n"
    "    asm inline(\"nop\");\n"
    "    asm volatile inline(\"nop\");\n"
    "    asm inline volatile(\"nop\");\n"
    "    asm(\"nop\" \"\\n\\t\" \"nop\");\n"
    "}\n"
    "\n"
    /* --- extended asm ----------------------------------------------------- */
    "int test_extended_asm(int x, int *p) {\n"
    "    int y = 0;\n"
    "    asm volatile(\"mfence\" :);\n"
    "    asm volatile(\"mfence\" : :);\n"
    "    asm volatile(\"\" ::: \"memory\");\n"
    "    asm(\"mov %1, %0\" : \"=r\" (y) : \"r\" (x));\n"
    "    asm(\"add %1, %0\" : \"+r\" (y) : \"ri\" (x + 1) : \"cc\");\n"
    "    asm(\"mov %[src], %[dst]\" : [dst] \"=r\" (y) : [src] \"r\" (x));\n"
    "    asm(\"\" : \"=r\" (y), \"=m\" (*p) : \"r\" (x), \"m\" (p[1]) : \"cc\", \"memory\");\n"
    "    asm(\"\" : : \"i\" (sizeof(int)));\n"
    "    asm(\"\" : : : );\n"
    "    if (x) asm volatile(\"nop\");\n"
    "    return y;\n"
    "}\n"
    "\n"
    /* --- asm goto --------------------------------------------------------- */
    "int test_asm_goto(int x) {\n"
    "    asm goto(\"jmp %l0\" : : : : out);\n"
    "    asm volatile goto(\"test %0, %0; jz %l1\" : : \"r\" (x) : \"cc\" : zero, out);\n"
    "    asm goto(\"jmp %l[out]\" : : : \"memory\" : out);\n"
    "    asm goto(\"\" : \"=r\" (x) : : : out);\n"
    "    return 1;\n"
    "zero:\n"
    "    return 0;\n"
    "out:\n"
    "    return x;\n"
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = {0};
    cn_tu_init(&tu, "input.i", .source = src, .flags = CN_TU_NO_CODE_OUTPUT);
    cn_tu_process(&tu);
    cn_tu_free(&tu);
    return 0;
}
