#include <stdio.h>

typedef struct {
    float x, y;
} Vec2f;

int bar(int x) {
    return x;
}

int foo(int x) {
    return bar(x);
}


int main() {
    return foo(8);
}




