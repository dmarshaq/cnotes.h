#include <stdio.h>

int bar(int x) {
    return x;
}

int foo() {
    return bar(8);
}

int main() {
    printf("Hello world!\n");
    foo();
    return 0;
}

