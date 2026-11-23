// Test: Sizeof expressions
struct Point { int x; int y; };

int test_sizeof(void) {
    int x;
    int arr[10];
    struct Point p;

    x = sizeof(int);
    x = sizeof(char);
    x = sizeof(float);
    x = sizeof(double);
    x = sizeof(void *);

    x = sizeof(x);
    x = sizeof(arr);
    x = sizeof(p);
    x = sizeof(struct Point);

    x = sizeof(int *);
    x = sizeof(int **);

    return x;
}
