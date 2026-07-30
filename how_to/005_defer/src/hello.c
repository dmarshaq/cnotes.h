#include <stdio.h>
#include <time.h>

#define TIME(func)\
    clock_t start = clock();\
    [[defer]] {\
        clock_t end = clock();\
        fprintf(stderr, "%s(): %.3f ms\n", func, (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);\
    }

int bar(int value) {
    TIME("bar");

    for (int i = 0; i < 40000; i++) {
        value *= 2;
    }

    return value;
}

int foo(int a, int b) {
    TIME("foo");

    a = b + 1;
    b = a - 1;
    return bar(b) + bar(a);
}

int main() {
    TIME("main");
    
    printf("Hello world!\n");

    return foo(6, 9);
} 
