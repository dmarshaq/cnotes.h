#include <stdio.h>

// Simple defer.
int main() {
    int cn__defer_rvalue;
    [[defer]] printf("Bye!!\n");

    printf("Hello world!\n");

    return 0;
} 

