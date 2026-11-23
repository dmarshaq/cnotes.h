// Test: Expression parsing - binary, unary, sizeof, etc.
int test_binary_ops(int a, int b) {
    int x;
    x = a + b;
    x = a - b;
    x = a * b;
    x = a / b;
    x = a % b;
    x = a & b;
    x = a | b;
    x = a ^ b;
    x = a << 2;
    x = a >> 2;
    return x;
}

int test_comparison(int a, int b) {
    if (a == b) return 1;
    if (a != b) return 2;
    if (a < b) return 3;
    if (a > b) return 4;
    if (a <= b) return 5;
    if (a >= b) return 6;
    return 0;
}

int test_logical(int a, int b) {
    if (a && b) return 1;
    if (a || b) return 2;
    if (!a) return 3;
    return 0;
}

int test_unary(int a) {
    int x;
    x = -a;
    x = +a;
    x = ~a;
    x = !a;
    return x;
}

int test_sizeof(void) {
    int x;
    x = sizeof(int);
    x = sizeof(x);
    x = sizeof(int *);
    return x;
}

int test_ternary(int a) {
    return a > 0 ? 1 : -1;
}

int test_assignment(void) {
    int x = 10;
    x += 5;
    x -= 3;
    x *= 2;
    x /= 2;
    x %= 3;
    x &= 0xFF;
    x |= 0x10;
    x ^= 0x01;
    x <<= 1;
    x >>= 1;
    return x;
}
