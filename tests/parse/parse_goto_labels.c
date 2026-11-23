// Test: goto statement and identifier labels
int test_goto_forward(int x) {
    if (x > 0) goto done;
    x = -x;
done:
    return x;
}

int test_goto_backward(int x) {
again:
    x = x - 1;
    if (x > 0) goto again;
    return x;
}

int test_multiple_labels(int x) {
    if (x == 0) goto zero;
    if (x == 1) goto one;
    goto other;
zero:
    return 0;
one:
    return 1;
other:
    return x;
}

int test_label_on_empty_statement(int x) {
    goto end;
end:
    ;
    return x;
}

int test_label_on_block(int x) {
    goto body;
body:
    {
        x = x + 1;
    }
    return x;
}

int test_chained_labels(int x) {
    goto second;
first:
second:
third:
    return x;
}

int test_label_inside_nested_block(int x) {
    {
        {
            if (x > 0) goto inner;
inner:
            x = x + 1;
        }
    }
    return x;
}

int test_goto_out_of_loop(int n) {
    int sum = 0;
    for (int i = 0; i < n; i = i + 1) {
        for (int j = 0; j < n; j = j + 1) {
            if (i * j > 10) goto finished;
            sum = sum + 1;
        }
    }
finished:
    return sum;
}

int test_goto_into_same_name_label(int x) {
    goto again;
again:
    return x;
}

int test_label_on_loop(int x) {
loop_head:
    while (x > 0) {
        x = x - 1;
        if (x == 3) goto loop_head;
    }
    return x;
}
