/**
 * TEST: `switch` statement with `case` and `default` labels.
 *
 * `switch` with `case` and `default` labels, stacked labels, fallthrough, `default` before `case`,
 * empty body, non-block body, declaration inside the switch body, nested switches, expression
 * conditions, and constant expression case labels (`sizeof`, `1 << 4`, negative).
 */
int test_switch(int x) {
    int r = 0;
    switch (x) {
        case 0:
            r = 1;
            break;
        case 1:
        case 2:
            r = 2;
            break;
        case 3 + 4:
            r = 3;
            break;
        default:
            r = -1;
            break;
    }
    return r;
}

int test_switch_fallthrough(int x) {
    int r = 0;
    switch (x) {
        case 0:
            r = r + 1;
        case 1:
            r = r + 2;
        default:
            r = r + 3;
    }
    return r;
}

int test_switch_default_first(int x) {
    int r;
    switch (x) {
        default:
            r = 0;
            break;
        case 5:
            r = 5;
            break;
    }
    return r;
}

int test_switch_empty_body(int x) {
    switch (x) {
    }
    return x;
}

int test_switch_no_block(int x) {
    int r = 0;
    switch (x)
        case 0:
            r = 1;
    return r;
}

int test_switch_declaration_in_body(int x) {
    int r = 0;
    switch (x) {
        int local;
        case 0:
            local = 7;
            r = local;
            break;
    }
    return r;
}

int test_nested_switch(int x, int y) {
    int r = 0;
    switch (x) {
        case 0:
            switch (y) {
                case 0:
                    r = 1;
                    break;
                default:
                    r = 2;
                    break;
            }
            break;
        default:
            r = 3;
            break;
    }
    return r;
}

int test_switch_expression_condition(int a, int b) {
    int r = 0;
    switch (a + b * 2) {
        case 0:
            r = 1;
            break;
    }
    switch ((a)) {
        case 0:
            r = 2;
            break;
    }
    return r;
}

int test_switch_constant_case_labels(int x) {
    int r = 0;
    switch (x) {
        case sizeof(int):
            r = 1;
            break;
        case 1 << 4:
            r = 2;
            break;
        case -1:
            r = 3;
            break;
    }
    return r;
}
