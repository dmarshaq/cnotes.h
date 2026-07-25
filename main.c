#include <stdio.h>

#define CUSTOM_MACRO " what? "

int bar(int x) {
    return x;
}

int foo() {
    return bar(8);
}

int test(int x);

int main() {
    int arr[10] = {0};

    printf("Hello world!\n");
    
    foo();
    
    return test(10);
} 

int test(int x) {
    // block statement
    {
        int y = 1;
    }

    // if statement
    if (x > 0) {
        x = x - 1;
    }

    // while statement
    while (x > 0) {
        x = x - 1;
    }

    // do-while statement
    do {
        x = x + 1;
    } while (x < 5);

    // for statement
    for (int i = 0; i < 3; i = i + 1) {
        x = x + i;
    }

    // continue / break (inside a loop for validity)
    for ([[checkin]] int i = 0; i < 3; i = i + 1) {
        if (i == 1) continue;
        if (i == 2) break;
    }

    goto early_return;

    [[my_cool_note]] early_return:
    // return statement
    return x;
}

int test2(int x) {
    // plain expression statement
    x = x * 2;
    return x;
} 

