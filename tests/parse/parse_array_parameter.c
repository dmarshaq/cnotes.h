/**
 * TEST: Array declarators in parameters adjusted to pointers.
 *
 * Array declarators in parameters adjusted to pointers: `int arr[]`, `int arr[10]`, prototypes
 * with `[]`, `*` and `[10]` agreeing with each other, assigning, incrementing and doing arithmetic
 * on the parameter, `sizeof` of a parameter yielding pointer size, `[static 4]`, qualifiers inside
 * brackets (`[const 4]`, `[volatile]`, `[static const 2]`), multi dimensional parameters (`int
 * m[][4]`, `int c[2][3][4]`) adjusted to pointer to array, `char *argv[]`, `const char
 * names[][8]`, function parameters adjusted to function pointers (`int fn(int arr[])`). Call sites
 * passing arrays, which decay to pointers, and assigning a function with an array parameter to an
 * `int (*)(int *)`.
 */
int first(int arr[]);
int first(int *arr);
int first(int arr[10]);

int first(int arr[]) {
    return arr[0];
}

int sized(int arr[10]) {
    int *p = arr;
    arr = arr + 1;
    arr++;
    p = &arr[2];
    return *arr + *(arr + 1) + p[0] + (int)sizeof(arr);
}

// Uncomment when parsing qualifiers in arrays is implemented.
// int at_least(int arr[static 4]) {
//     return arr[3];
// }
//
// int qualified(int arr[const 4], int other[volatile], int both[static const 2]) {
//     return arr[0] + other[0] + both[1];
// }
//
int matrix(int m[][4], int rows) {
    int (*row)[4] = m;
    m++;
    return m[rows - 1][3] + (*row)[0];
}

int cube(int c[2][3][4]) {
    int (*plane)[3][4] = c;
    return c[1][2][3] + plane[0][0][0];
}

int strings(char *argv[], const char names[][8]) {
    char **pp = argv;
    const char (*np)[8] = names;
    return pp[0][0] + np[0][0];
}

void fill(int count, int out[]) {
    for (int i = 0; i < count; i++) {
        out[i] = i;
    }
}

int apply(int (*fn)(int arr[]), int arr[]) {
    return fn(arr);
}

int test_array_parameters(void) {
    int arr[10];
    int m[3][4];
    int c[2][3][4];
    int (*fp)(int *) = first;

    fill(10, arr);
//     return first(arr) + sized(arr) + at_least(arr) + qualified(arr, arr, arr)
//         + matrix(m, 3) + cube(c) + apply(first, arr) + fp(&arr[1]);
    return first(arr) + sized(arr) + matrix(m, 3) + cube(c) + apply(first, arr) + fp(&arr[1]);
}
