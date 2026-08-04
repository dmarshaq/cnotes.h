#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// This file demonstrates the function scoped `[[defer]]` metaprogram implemented in `defer.c`,
// one construct at a time.
//
// A deferred statement is not executed where it is written. Instead the metaprogram moves it to
// the end of the function, and reroutes every `return` through it. So a defer runs when the
// function is done, no matter which `return` ends it.
//
// Two things follow from "function scope", and both are on purpose:
//  - `[[defer]]` may only be written in the function's own block. Writing one inside a loop or
//    an inner block is an error, `020_block_defer` is the version that handles those.
//  - `break` and `continue` are left alone. They stay inside the function, so they have no
//    reason to run the function's defers.
//
// Each example below prints as it goes, so the ordering the metaprogram produces is visible in
// the output.

// ---------------------------------------------------------------------------
// The basics
// ---------------------------------------------------------------------------

// A deferred statement runs after the rest of the function, even though it is written first.
// The returned expression is still evaluated before it runs.
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

// `[[defer]]` can take a whole block, which is how you defer more than one statement without
// writing three separate defers.
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

// Without defer this function would need the free() repeated on all three return paths, or a
// goto-to-cleanup ladder at the bottom. With defer the release sits directly under the
// acquisition and covers every exit.
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

// Note that the `return -1` above is written before the defer, and the metaprogram leaves it
// alone: at that point in the function there is nothing registered to run yet.

// The rerouting applies to every `return` below the defer, so branchy code keeps its cleanup
// guarantee without any extra bookkeeping.
int classify(int value) {
    [[defer]] printf("    classify(%d) finished\n", value);

    if (value < 0)   return -1;
    if (value == 0)  return 0;
    if (value < 100) return 1;

    return 2;
}

// A `void` function works the same way, a bare `return;` is rerouted just like one with a value.
void greet(const char *name) {
    [[defer]] printf("    greeting done\n");

    if (name == NULL) {
        printf("    nobody to greet\n");
        return;
    }

    printf("    hello, %s\n", name);
}

// ---------------------------------------------------------------------------
// Returns in nested scopes
// ---------------------------------------------------------------------------

// The defer belongs to the function, but the `return` it has to cover can sit anywhere. Here it
// is inside a loop, two scopes down, and it still leaves through the deferred statement.
int find_first(const int *values, int count, int wanted) {
    [[defer]] printf("    search finished\n");

    for (int i = 0; i < count; i++) {
        if (values[i] == wanted) {
            printf("    found %d at slot %d\n", wanted, i);
            return i;
        }
    }

    printf("    no match\n");
    return -1;
}

// Same story inside a switch: the `return` in the case body is an exit out of the function, so
// it goes through the defer.
int scale(int code) {
    [[defer]] printf("    scale(%d) finished\n", code);

    switch (code) {
        case 0: return 0;
        case 1: return 10;
        case 2: return 20;
        default: return -1;
    }
}

// `break` and `continue` are a different matter. They only move control around inside the
// function, so the metaprogram leaves them untouched and the defer still runs once, at the end.
void stop_early(int n) {
    [[defer]] printf("    stop_early finished\n");

    for (int i = 0; i < n; i++) {
        if (i % 2 != 0) continue;

        if (i == 4) {
            printf("    stopping at %d\n", i);
            break;
        }

        printf("    step %d\n", i);
    }

    printf("    loop is over\n");
}

// ---------------------------------------------------------------------------
// Building on defer: a function timer
// ---------------------------------------------------------------------------

// Because a defer is tied to the function it is written in, a macro can start a timer and
// register the reporting half in one line. The caller never has to remember to stop it, and
// every `return` reports before leaving.
//
// `010_profiling` takes this one step further and generates the same code from an attribute on
// the function, so the body does not have to mention the timer at all.
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

    // Note the ordering here: the defer inside `doubled` runs before this printf gets its value,
    // because the defer is part of leaving `doubled`.
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

    printf("\ngreet(\"world\"):\n");
    greet("world");

    printf("\ngreet(NULL):\n");
    greet(NULL);

    printf("\nfind_first(values, 5, 15):\n");
    result = find_first(values, 5, 15);
    printf("    = %d\n", result);

    printf("\nfind_first(values, 5, 99):\n");
    result = find_first(values, 5, 99);
    printf("    = %d\n", result);

    printf("\nscale(2):\n");
    result = scale(2);
    printf("    = %d\n", result);

    printf("\nstop_early(6):\n");
    stop_early(6);

    printf("\nsum_to(100000):\n");
    result = sum_to(100000);
    printf("    = %d\n", result);

    return 0;
}
