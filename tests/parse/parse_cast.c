/**
 * TEST: Cast expression parsing.
 *
 * Casts between arithmetic types, casts to and from pointer types (`(void *)&i`), dereferencing a
 * cast pointer (`*(int *)ptr`). Char values written as integers since char literals are not
 * supported yet.
 */
int test_casts(void) {
    int i = 42;
    float f = 3.14;
    char c = 'A';
    void *ptr;

    f = (float)i;
    i = (int)f;
    c = (char)i;
    i = (int)c;

    ptr = (void *)&i;
    i = *(int *)ptr;

    return i;
}
