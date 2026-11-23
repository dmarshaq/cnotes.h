/**
 * TEST: Basic function definition parsing.
 *
 * Function definition with parameters, `void` parameter list, empty body, `static inline`
 * specifiers.
 */
int add(int a, int b) {
    return a + b;
}

void empty_function(void) {
}

static inline int static_func(void) {
    return 42;
}
