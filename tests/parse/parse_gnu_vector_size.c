/**
 * TEST: GNU `__attribute__((vector_size(...)))` attribute.
 *
 * This attribute can appear in `init_declarator` and impacts the base type
 * conversion from AST. This test makes sure its type modification 
 * is properly applied.
 *
 * --gnu
 */

int value __attribute__((vector_size(16))) = {1, 2, 3, 4};

// Flagged in Clang, but is allowed in GCC.
// In GCC is viewed as pointer to vector of 4 int.
int *vec __attribute__((vector_size(16))) = (void *)0;

int test_member_access(void) {
    int a = value[0];
    int b = value[1];
    value = value * a;
    value += *vec;
}
