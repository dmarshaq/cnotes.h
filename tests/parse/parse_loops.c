/**
 * TEST: Iteration statements, `while`, `do while` and `for`.
 *
 * `while`, `do while` and `for` in block, non-block and empty body forms; complex and bare
 * conditions; `for` with expression init, declaration init, multiple declarators, comma clauses,
 * each clause omitted and `for (;;)`; nested and mixed loops; declarations scoped to loop bodies.
 */
int test_while(int x) {
    while (x > 0) {
        x = x - 1;
    }
    return x;
}

int test_while_no_block(int x) {
    while (x > 0)
        x = x - 1;
    return x;
}

int test_while_empty_body(int x) {
    while (x > 0)
        ;
    while (x > 0) {
    }
    return x;
}

int test_while_complex_condition(int a, int b) {
    while (a > 0 && b > 0) {
        a = a - 1;
        b = b - 1;
    }
    while (a) {
        a = 0;
    }
    return a + b;
}

int test_nested_while(int x, int y) {
    int r = 0;
    while (x > 0) {
        while (y > 0) {
            r = r + 1;
            y = y - 1;
        }
        x = x - 1;
    }
    return r;
}

int test_do_while(int x) {
    do {
        x = x + 1;
    } while (x < 5);
    return x;
}

int test_do_while_no_block(int x) {
    do
        x = x + 1;
    while (x < 5);
    return x;
}

int test_do_while_empty_body(int x) {
    do {
    } while (0);
    do ; while (0);
    return x;
}

int test_nested_do_while(int x) {
    int y = 0;
    do {
        do {
            y = y + 1;
        } while (y < 3);
        x = x - 1;
    } while (x > 0);
    return y;
}

int test_for(int n) {
    int sum = 0;
    int i;
    for (i = 0; i < n; i = i + 1) {
        sum = sum + i;
    }
    return sum;
}

int test_for_declaration_init(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        sum = sum + i;
    }
    for (int i = 0, j = n; i < j; i = i + 1, j = j - 1) {
        sum = sum + 1;
    }
    return sum;
}

int test_for_omitted_clauses(int n) {
    int i = 0;
    int sum = 0;
    for (; i < n; i = i + 1) {
        sum = sum + 1;
    }
    for (i = 0; ; i = i + 1) {
        break;
    }
    for (i = 0; i < n;) {
        i = i + 1;
    }
    for (;;) {
        break;
    }
    for (;;)
        break;
    return sum;
}

int test_for_no_block(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1)
        sum = sum + i;
    for (int i = 0; i < n; i = i + 1)
        ;
    return sum;
}

int test_for_comma_clauses(int n) {
    int i, j, sum = 0;
    for (i = 0, j = n; i < j; i = i + 1, j = j - 1) {
        sum = sum + 1;
    }
    return sum;
}

int test_nested_for(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        for (int j = 0; j < n; j = j + 1) {
            sum = sum + i * j;
        }
    }
    return sum;
}

int test_mixed_loops(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        int j = i;
        while (j > 0) {
            do {
                j = j - 1;
            } while (0);
        }
        sum = sum + j;
    }
    return sum;
}

int test_loop_scoped_declarations(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        int local = i * 2;
        sum = sum + local;
    }
    while (sum > 0) {
        int local = sum;
        sum = local - 1;
    }
    return sum;
}
