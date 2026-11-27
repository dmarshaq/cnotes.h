/**
 * TEST: Variable declaration parsing with various types.
 *
 * Global, `static`, `extern` and `const` declarations; `char`/`short`/`long`/`long
 * long`/`unsigned`/`signed`, float and double; pointers, pointer-to-pointer, pointer-to-const,
 * const-pointer; 1D and 2D arrays; block scope locals including `register`.
 */
int global_int;
static int static_int;
extern int extern_int;
const int const_int = 100;

char c;
short s;
long l;
long long ll;
unsigned int ui;
signed char sc;

float f;
double d;

int *ptr;
int **ptr_ptr;
const int *ptr_to_const;
int *const const_ptr = 0;
int arr[10];
int arr_2d[10][5];

void func(void) {
    int local_var = 5;
    register int reg_var = 10;
}
