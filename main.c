#include <stdio.h>

#define CUSTOM_MACRO " what? "

[[cn::hello("info!!!"CUSTOM_MACRO"other_str")]]
int bar(int x) {
    return x;
}

int foo() {
    return bar(8);
}

int f __attribute__(());

[[hello("meta_main")]]
int main() {
    printf("Hello world!\n");

    foo();
    return 0;
} 

