// Test: GNU asm statements - basic, extended, qualifiers, operands, clobbers, goto labels, file-scope asm.
// AI: Generated.
/* --- file-scope asm --------------------------------------------------- */
asm(".globl file_scope_symbol");
__asm__(".text");
__asm("nop" "\n\t" "nop");

/* --- basic asm -------------------------------------------------------- */
void test_basic_asm(void) {
    asm("nop");
    __asm__("nop");
    __asm("nop");
    asm volatile("nop");
    asm inline("nop");
    asm volatile inline("nop");
    asm inline volatile("nop");
    asm("nop" "\n\t" "nop");
}

/* --- extended asm ----------------------------------------------------- */
int test_extended_asm(int x, int *p) {
    int y = 0;
    asm volatile("mfence" :);
    asm volatile("mfence" : :);
    asm volatile("" ::: "memory");
    asm("mov %1, %0" : "=r" (y) : "r" (x));
    asm("add %1, %0" : "+r" (y) : "ri" (x + 1) : "cc");
    asm("mov %[src], %[dst]" : [dst] "=r" (y) : [src] "r" (x));
    asm("" : "=r" (y), "=m" (*p) : "r" (x), "m" (p[1]) : "cc", "memory");
    asm("" : : "i" (sizeof(int)));
    asm("" : : : );
    if (x) asm volatile("nop");
    return y;
}

/* --- asm goto --------------------------------------------------------- */
int test_asm_goto(int x) {
    asm goto("jmp %l0" : : : : out);
    asm volatile goto("test %0, %0; jz %l1" : : "r" (x) : "cc" : zero, out);
    asm goto("jmp %l[out]" : : : "memory" : out);
    asm goto("" : "=r" (x) : : : out);
    return 1;
zero:
    return 0;
out:
    return x;
}
