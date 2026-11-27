/**
 * TEST: Comma operator.
 *
 * Comma operator in an expression statement and inside parentheses as a return value, multi
 * declarator declaration.
 */
int test_comma(void) {
    int a, b, c;
    a = 1, b = 2, c = 3;
    return (a, b, c);
}
