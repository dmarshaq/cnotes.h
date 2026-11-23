// Test: Increment/decrement operators
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
