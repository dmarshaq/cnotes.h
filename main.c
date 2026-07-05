#include <stdio.h>

int bar(int x) {
    return x;
}

int foo(int x) {
    return bar(x);
}

int main() {
    printf("Hello world!\n");
    foo(8);
    return 0;
}

