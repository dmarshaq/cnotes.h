// Test: Operator precedence parsing
int test_precedence(int a, int b, int c) {
    int x;

    x = a + b * c;
    x = (a + b) * c;
    x = a * b + c;

    x = a + b - c;
    x = a - b + c;

    x = a << 2 + 1;
    x = (a << 2) + 1;

    x = a & b | c;
    x = a | b & c;

    x = a && b || c;
    x = a || b && c;

    x = a > 0 ? b : c;
    x = a > 0 ? b > 0 ? 1 : 2 : 3;

    return x;
}
