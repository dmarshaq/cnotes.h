#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// This file demonstrates two metaprograms working together:
//
//  - `[[time]]`, implemented in `timer.c`, measures how long a function takes and prints it.
//  - `[[defer]]`, implemented in `defer.c`, the exact same file `005_defer` uses.
//
// `[[time]]` does not write timing code, it writes a `[[defer]]`. Everything about getting that
// clean up to run on every exit out of the function is already `defer.c`'s job, so the two of
// them together handle early returns, returns from inside loops and user written defers without
// `timer.c` knowing that any of those exist.
//
// NOTE: The generated code calls `clock()` and `printf()`, so a file using `[[time]]` has to
// include <time.h> and <stdio.h>. A metaprogram runs after the pre-processor, it cannot add an
// include to a file that has already been pre-processed.

// The attribute is the whole change. The body below says nothing about timing, and there is no
// timer to start, stop or forget to stop.
[[time]] int sum_to(int n) {
    int total = 0;
    for (int i = 0; i < n; i++) total += i;

    return total;
}

// A `void` function is timed the same way, there is nothing here about how the function ends.
[[time]] void warm_up(int n) {
    int sink = 0;
    for (int i = 0; i < n; i++) sink += i % 7;

    if (sink < 0) printf("    unreachable\n");
}

// Four exits, and all four are measured. `timer.c` never looked at a single one of them, it wrote
// one `[[defer]]` at the top of the function and `defer.c` rerouted every `return` through it.
[[time]] int classify(int value) {
    if (value < 0)   return -1;
    if (value == 0)  return 0;
    if (value < 100) return 1;

    return 2;
}

// The `return` here is two scopes down, inside a loop, and it is still covered.
[[time]] int find_first(const int *values, int count, int wanted) {
    for (int i = 0; i < count; i++) {
        if (values[i] == wanted) return i;
    }

    return -1;
}

// A timed function can use `[[defer]]` itself, they end up on the same stack. The timer is
// registered first, before the body, so it unwinds last: the measurement covers the free() below
// instead of stopping just before it.
[[time]] int load_name(const char *input, char *out, int out_size) {
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

// Neither attribute needs the other. This function is timed but defers nothing.
[[time]] int fib(int n) {
    int a = 0, b = 1;

    for (int i = 0; i < n; i++) {
        int next = a + b;
        a = b;
        b = next;
    }

    return a;
}

// And this one defers but is not timed, `[[defer]]` works exactly as it did in `005_defer`.
int measured_elsewhere(int x) {
    [[defer]] printf("    measured_elsewhere finished\n");

    return x + 1;
}

// Timed functions calling timed functions just nest, each one reports as it finishes, so the
// callee's line comes out before the caller's.
[[time]] int pipeline(int n) {
    int total = sum_to(n);
    total += fib(30);

    return total;
}

int main(void) {
    char name[32];
    int values[5] = { 4, 8, 15, 16, 23 };
    int result;

    printf("sum_to(100000):\n");
    result = sum_to(100000);
    printf("    = %d\n", result);

    printf("\nwarm_up(200000):\n");
    warm_up(200000);

    printf("\nclassify(42):\n");
    result = classify(42);
    printf("    = %d\n", result);

    printf("\nclassify(-1):\n");
    result = classify(-1);
    printf("    = %d\n", result);

    printf("\nfind_first(values, 5, 15):\n");
    result = find_first(values, 5, 15);
    printf("    = %d\n", result);

    printf("\nload_name(\"cnotes\"):\n");
    load_name("cnotes", name, sizeof(name));

    printf("\nload_name(NULL):\n");
    load_name(NULL, name, sizeof(name));

    printf("\nmeasured_elsewhere(41):\n");
    result = measured_elsewhere(41);
    printf("    = %d\n", result);

    printf("\npipeline(100000):\n");
    result = pipeline(100000);
    printf("    = %d\n", result);

    return 0;
}
