#include <stdio.h>

typedef struct {
    float x, y;
} Vec2f;

int foo(int x) {
    printf("Hello from modified foo -> bar!\n");
    return x;
}

int main() {
    return foo(8);
}




