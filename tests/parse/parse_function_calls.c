// Test: Function call parsing
int add(int a, int b) { return a + b; }
int negate(int x) { return -x; }
void noop(void) { }

int test_calls(void) {
    int x;
    x = add(1, 2);
    x = negate(x);
    noop();
    x = add(add(1, 2), add(3, 4));
    return x;
}
