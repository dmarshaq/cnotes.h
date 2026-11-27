/**
 * TEST: Pointer operations and arithmetic.
 *
 * Address-of and dereference, double pointers and double dereference as assignment target, pointer
 * arithmetic (`p + 1`, `p - 1`, `*(arr + 2)`), array subscript.
 */
int test_pointers(void) {
    int x = 10;
    int *p = &x;
    int **pp = &p;

    *p = 20;
    **pp = 30;

    return *p + **pp;
}

int test_pointer_arithmetic(int *arr) {
    int *p = arr;
    int x;

    x = *p;
    p = p + 1;
    x = *p;
    p = p - 1;

    x = *(arr + 2);
    x = arr[3];

    return x;
}

int test_address_of(void) {
    int x = 5;
    int *p;
    p = &x;
    return *p;
}
