/**
 * TEST: `if`/`else` statement parsing.
 *
 * `if`, `if`/`else`, `else if` chains, nested `if` inside `if`.
 */
int test_if(int x) {
    if (x > 0) {
        return 1;
    }
    return 0;
}

int test_if_else(int x) {
    if (x > 0) {
        return 1;
    } else {
        return -1;
    }
}

int test_if_else_if(int x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

int test_nested_if(int x, int y) {
    if (x > 0) {
        if (y > 0) {
            return 1;
        }
    }
    return 0;
}
