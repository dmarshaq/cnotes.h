// Test: Cast expression parsing
int test_casts(void) {
    int i = 42;
    float f = 3.14;
    char c = 65; // NOTE: Char literals are not supported yet.
    void *ptr;

    f = (float)i;
    i = (int)f;
    c = (char)i;
    i = (int)c;

    ptr = (void *)&i;
    i = *(int *)ptr;

    return i;
}
