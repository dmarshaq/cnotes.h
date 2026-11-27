/**
 * TEST: Increment and decrement operators.
 *
 * Prefix and postfix `++`/`--` as statements and as sub-expressions of an assignment, on both
 * integers and pointers.
 */
int test_increment(void) {
    int x = 0;
    int y;

    x++;
    ++x;
    x--;
    --x;

    y = x++;
    y = ++x;
    y = x--;
    y = --x;

    return x + y;
}

int test_pointer_increment(int *p) {
    p++;
    ++p;
    p--;
    --p;
    return *p;
}
