/**
 * TEST: `break` and `continue` jump statements.
 *
 * `break` and `continue` in `while`, `do while` and `for` loops, `break` in `switch`, both inside
 * non-block loop bodies and `if`/`else` branches, nested loops, and nested plain blocks inside a
 * loop.
 */
int test_break_in_while(int x) {
    while (x > 0) {
        if (x == 5) break;
        x = x - 1;
    }
    return x;
}

int test_continue_in_while(int x) {
    int sum = 0;
    while (x > 0) {
        x = x - 1;
        if (x == 5) continue;
        sum = sum + x;
    }
    return sum;
}

int test_break_continue_in_do_while(int x) {
    int sum = 0;
    do {
        x = x - 1;
        if (x == 3) continue;
        if (x == 0) break;
        sum = sum + x;
    } while (x > 0);
    return sum;
}

int test_break_continue_in_for(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        if (i == 1) continue;
        if (i == 2) break;
        sum = sum + i;
    }
    return sum;
}

int test_break_in_switch(int x) {
    int r = 0;
    switch (x) {
        default:
            r = 1;
            break;
    }
    return r;
}

int test_break_continue_without_block(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1)
        if (i == 2) break;
    while (n > 0) {
        n = n - 1;
        if (n) continue;
        else break;
    }
    return sum;
}

int test_break_continue_in_nested_loops(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        for (int j = 0; j < n; j = j + 1) {
            if (j == i) continue;
            if (j > 3) break;
            sum = sum + 1;
        }
        if (sum > 100) break;
    }
    while (n > 0) {
        do {
            n = n - 1;
            if (n == 1) break;
        } while (n > 0);
        if (n == 0) break;
        continue;
    }
    return sum;
}

int test_break_continue_in_nested_blocks(int n) {
    while (n > 0) {
        {
            {
                n = n - 1;
                if (n == 3) continue;
                if (n == 1) break;
            }
        }
    }
    return n;
}
