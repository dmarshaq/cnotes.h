#include <stdio.h>

#define CUSTOM_MACRO " what? "

int bar(int x) {
    return x;
}

int foo() {
    return bar(8);
}

struct vec2 {
    int ;
};

int main() {
    int arr[10] = {0};

    struct vec2 a = {0};

    printf("Hello world!\n");

    foo();
    return 0;
} 

