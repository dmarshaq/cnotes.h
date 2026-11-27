/**
 * TEST: Variable length array parsing.
 *
 * Variable length arrays: block scope VLAs with variable and expression lengths, 2D VLAs with both
 * dimensions variable and mixed with constant dimensions, `const` element VLAs, pointers to VLAs
 * (`int (*p)[n]`), block scope VLA `typedef`s, run time `sizeof` on VLA objects, `int[n]` and VLA
 * typedefs, VLAs scoped to loop bodies. VLA parameters `int arr[n]`, `int m[n][n]`, `int
 * m[rows][cols]` referencing earlier parameters, `[*]` and `[*][*]` in prototypes, and passing
 * VLAs as arguments.
 */
int sum(int n, int arr[n]);
// Uncomment once unspecified array lengths are allowed.
// int sum_unspecified(int n, int arr[*]);
int trace(int n, int m[n][n]);
// int trace_unspecified(int n, int m[*][*]);

int sum(int n, int arr[n]) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += arr[i];
    }
    return total;
}

int trace(int n, int m[n][n]) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += m[i][i];
    }
    return total;
}

int rows_sum(int rows, int cols, int m[rows][cols]) {
    int total = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            total += m[r][c];
        }
    }
    return total;
}

int test_vla(int n) {
    int a[n];
    int b[n * 2 + 1];
    int m[n][n];
    int mixed[n][4];
    int mixed_rev[4][n];
    const int c[n];
    int (*p)[n] = &a;
    int (*rows)[n] = m;
    typedef int Row[n];
    Row r;

    int size = sizeof(a);
    size = sizeof(m) / sizeof(m[0]);
    size = sizeof(int[n]);
    size = sizeof(Row);

    for (int i = 0; i < n; i++) {
        a[i] = i;
        b[i] = (*p)[i];
        r[i] = rows[i][0];
        m[i][i] = mixed[i][0] + mixed_rev[0][i];
    }

    for (int i = 1; i <= n; i++) {
        int scoped[i];
        scoped[0] = i;
    }

    return sum(n, a) + trace(n, m) + rows_sum(n, 4, mixed) + c[0];
}
