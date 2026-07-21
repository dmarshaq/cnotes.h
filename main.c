#include <stdio.h>

#define CUSTOM_MACRO " what? "

int bar(int x) {
    return x;
}

int foo() {
    return bar(8);
}

struct vec2 {
    int x, y;
};

int main() {
    int arr[10] = { [0] = 2 };

    printf("Hello world!\n");

    foo();
    return 0;
} 

