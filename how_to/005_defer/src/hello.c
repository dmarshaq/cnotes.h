#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// This file demonstrates the scoped `[[defer]]` metaprogram implemented in
// `defer.c`, one construct at a time.
//
// A deferred statement is not executed where it is written. Instead the
// metaprogram moves it to the end of the block it appears in, and reroutes
// every exit out of that block through it. So a defer runs when its enclosing
// scope ends, no matter how control leaves that scope. (The only exception is arbitrary exit with `goto`)
//
// Each example below prints as it goes, so the ordering the metaprogram
// produces is visible in the output.

// ---------------------------------------------------------------------------
// The basics
// ---------------------------------------------------------------------------

// A deferred statement runs after the rest of the block, even though it is
// written first. The returned expression is still evaluated before it runs.
int doubled(int x) {
    [[defer]] printf("    ...and the defer runs last\n");

    printf("    body runs first\n");
    return x * 2;
}

// Defers unwind in reverse order of registration, the same way a stack does.
// Written first, second, third; executed third, second, first.
void countdown(void) {
    [[defer]] printf("    first\n");
    [[defer]] printf("    second\n");
    [[defer]] printf("    third\n");

    printf("    registering three defers\n");
}

// `[[defer]]` can take a whole block, which is how you defer more than one
// statement without writing three separate defers.
void deferred_block(void) {
    [[defer]] {
        printf("    closing down\n");
        printf("    done\n");
    }

    printf("    working\n");
}

// ---------------------------------------------------------------------------
// Why defer is useful: one cleanup, many exits
// ---------------------------------------------------------------------------

// Without defer this function would need the free() repeated on all three
// return paths, or a goto-to-cleanup ladder at the bottom. With defer the
// release sits directly under the acquisition and covers every exit.
int load_name(const char *input, char *out, int out_size) {
    char *buf = malloc(64);
    if (buf == NULL) return -1;

    [[defer]] {
        printf("    freeing buffer\n");
        free(buf);
    }

    if (input == NULL) {
        printf("    rejected: no input\n");
        return -2;
    }

    if ((int)strlen(input) >= out_size) {
        printf("    rejected: too long\n");
        return -3;
    }

    strcpy(buf, input);
    strcpy(out, buf);
    printf("    accepted: %s\n", out);
    return (int)strlen(out);
}

// The rerouting applies to every `return` in the function, so branchy code
// keeps its cleanup guarantee without any extra bookkeeping.
int classify(int value) {
    [[defer]] printf("    classify(%d) finished\n", value);

    if (value < 0)   return -1;
    if (value == 0)  return 0;
    if (value < 100) return 1;

    return 2;
}

// ---------------------------------------------------------------------------
// Defer inside loops
// ---------------------------------------------------------------------------

// A loop body is its own scope, so a defer written inside it belongs to a
// single iteration and fires at the bottom of each one.
void per_iteration(int n) {
    for (int i = 0; i < n; i++) {
        [[defer]] printf("    iteration %d closed\n", i);

        printf("    iteration %d open\n", i);
    }
}

// `break` leaves the loop body early, so the metaprogram runs the iteration's
// defer first and only then actually breaks out of the loop.
void stop_early(int n) {
    for (int i = 0; i < n; i++) {
        [[defer]] printf("    iteration %d closed\n", i);

        if (i == 2) {
            printf("    iteration %d: stopping\n", i);
            break;
        }

        printf("    iteration %d open\n", i);
    }

    printf("    loop is over\n");
}

// `continue` is handled the same way: the defer runs, then the loop moves on
// to the next iteration.
void skip_odd(int n) {
    for (int i = 0; i < n; i++) {
        [[defer]] printf("    iteration %d closed\n", i);

        if (i % 2 != 0) {
            printf("    iteration %d: skipping\n", i);
            continue;
        }

        printf("    iteration %d open\n", i);
    }
}

// Returning from inside a loop unwinds both scopes: the loop body's defer runs
// first, then the function's, before the value goes back to the caller.
int find_first(const int *values, int count, int wanted) {
    [[defer]] printf("    search finished\n");

    for (int i = 0; i < count; i++) {
        [[defer]] printf("    checked slot %d\n", i);

        if (values[i] == wanted) {
            printf("    found %d at slot %d\n", wanted, i);
            return i;
        }
    }

    return -1;
}

// Nested loops each keep their own defers, so the inner one fires on every
// inner iteration and the outer one at the end of each outer iteration.
void grid(int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        [[defer]] printf("    row %d closed\n", r);

        for (int c = 0; c < cols; c++) {
            [[defer]] printf("      cell %d,%d closed\n", r, c);

            printf("      cell %d,%d open\n", r, c);
        }
    }
}

// ---------------------------------------------------------------------------
// Defer in other scopes
// ---------------------------------------------------------------------------

// A bare block is a scope like any other. Its defer fires at the closing brace,
// and the statements after the block still run normally.
void inner_scope(void) {
    printf("    before the block\n");
    {
        [[defer]] printf("    leaving the block\n");

        printf("    inside the block\n");
    }
    printf("    after the block\n");
}

// A switch case body is a block too, so its defer runs before `break` passes
// control out of the switch.
void describe(int code) {
    switch (code) {
        case 1: {
            [[defer]] printf("    case 1 closed\n");

            printf("    handling code 1\n");
            break;
        }
        default: {
            printf("    unhandled code %d\n", code);
            break;
        }
    }

    printf("    past the switch\n");
}

// ---------------------------------------------------------------------------
// Building on defer: a scope timer
// ---------------------------------------------------------------------------

// Because a defer is tied to the scope it is written in, a macro can open a
// timer and register the reporting half in one line. The caller never has to
// remember to stop it.
#define TIME(name)\
    clock_t cn_t_start = clock();\
    [[defer]] {\
        clock_t cn_t_end = clock();\
        printf("    %s took %.3f ms\n", name, (double)(cn_t_end - cn_t_start) * 1000.0 / CLOCKS_PER_SEC);\
    }

int sum_to(int n) {
    TIME("sum_to");

    int total = 0;
    for (int i = 0; i < n; i++) total += i;

    return total;
}

// ---------------------------------------------------------------------------

int main(void) {
    char name[32];
    int values[5] = { 4, 8, 15, 16, 23 };
    int result;

    // Note the ordering here: the defer inside `doubled` runs before this
    // printf gets its value, because the defer is part of leaving `doubled`.
    printf("doubled(21):\n");
    result = doubled(21);
    printf("    = %d\n", result);

    printf("\ncountdown():\n");
    countdown();

    printf("\ndeferred_block():\n");
    deferred_block();

    printf("\nload_name(\"cnotes\"):\n");
    load_name("cnotes", name, sizeof(name));

    printf("\nload_name(NULL):\n");
    load_name(NULL, name, sizeof(name));

    printf("\nclassify(42):\n");
    result = classify(42);
    printf("    = %d\n", result);

    printf("\nper_iteration(3):\n");
    per_iteration(3);

    printf("\nstop_early(5):\n");
    stop_early(5);

    printf("\nskip_odd(4):\n");
    skip_odd(4);

    printf("\nfind_first(values, 5, 15):\n");
    result = find_first(values, 5, 15);
    printf("    = %d\n", result);

    printf("\ngrid(2, 2):\n");
    grid(2, 2);

    printf("\ninner_scope():\n");
    inner_scope();

    printf("\ndescribe(1):\n");
    describe(1);

    printf("\ndescribe(7):\n");
    describe(7);

    printf("\nsum_to(100000):\n");
    result = sum_to(100000);
    printf("    = %d\n", result);

    return 0;
}
