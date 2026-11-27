/**
 * TEST: GNU `__extension__` keyword in every position it can appear.
 *
 * GNU `__extension__` in every position GCC accepts it. Before external declarations: globals with
 * and without initializers, multiple declarators, `static`/`extern`/`const`, `typedef`s,
 * `struct`/`union`/`enum` definitions, prototypes, function definitions including `static inline`,
 * followed by `[[...]]` and GNU attributes, stray `;` and file-scope `asm`. Before member
 * declarations of structs and unions, including bit-fields, nested struct and union members and
 * `[[...]]` attributes. Before block scope declarations: `static`/`register`/`const` locals, block
 * `typedef`s and their use, local struct definitions, nested blocks. Before `for` initialization
 * declarations, including multiple declarators, and as part of `for` initialization, condition and
 * update expressions. As a unary operator on expressions: expression statements, assignment
 * targets, after `[[...]]` on an expression statement, binary operands, compound assignment,
 * parenthesized expressions, after unary `-`/`!`/`~`/`*`/`&`/`++`, cast and `sizeof` operands,
 * subscripts, `.`/`->` access, call arguments and callee, ternary, comma, compound literal and
 * initializer elements, array sizes, enumerator values, GNU attribute arguments, asm operands,
 * `if`/`while`/`do while`/`switch` conditions, `case` labels and `return`. Repeated `__extension__
 * __extension__` in every declaration position.
 *
 * AI: Generated.
 *
 * --gnu
 */

/* --- external declarations ------------------------------------------- */
__extension__ int extension_global;
__extension__ int extension_initialized_global = 1;
__extension__ int extension_first_global, extension_second_global = 2;
__extension__ static int extension_static_global;
__extension__ extern int extension_extern_global;
__extension__ const long long extension_const_global = 1LL;
__extension__ typedef long long extension_long_long;
__extension__ typedef struct { int x; } Extension_Typedef_Struct;
__extension__ struct Extension_Struct { int x; };
__extension__ union Extension_Union { int x; float y; };
__extension__ enum Extension_Enum { EXTENSION_A, EXTENSION_B };
__extension__ int extension_prototype(int x);
__extension__ int extension_function(int x) {
    return x;
}
__extension__ static inline int extension_static_inline_function(void) {
    return 0;
}
__extension__ __extension__ int extension_repeated_global;
__extension__ __extension__ __extension__ int extension_repeated_function(void) {
    return 0;
}
__extension__ [[note]] int extension_attributed_global;
__extension__ __attribute__((unused)) static int extension_gnu_attributed_global;
__extension__ int extension_trailing_gnu_attributed_global __attribute__((unused));
__extension__ ;
__extension__ __asm__("nop");

/* --- member declarations --------------------------------------------- */
struct Extension_Members {
    __extension__ int a;
    __extension__ long long b, c;
    __extension__ unsigned int bits : 3;
    __extension__ __extension__ int repeated;
    __extension__ [[note]] int attributed;
    __extension__ struct Extension_Struct nested;
    __extension__ union { int i; float f; } nested_union;
    int plain;
};
union Extension_Union_Members {
    __extension__ int a;
    __extension__ long long b;
};

/* --- expressions at file scope --------------------------------------- */
int extension_global_expression = __extension__ 42;
int extension_global_binary = __extension__ 1 + __extension__ 2;
int extension_global_array[__extension__ 4];
int extension_global_array_initializer[2] = { __extension__ 1, [1] = __extension__ 2 };
struct Extension_Struct extension_global_designated = { .x = __extension__ 3 };
enum Extension_Values { EXTENSION_VALUE = __extension__ 5, EXTENSION_NEXT = __extension__ EXTENSION_VALUE + 1 };
int extension_global_aligned __attribute__((aligned(__extension__ 8)));

/* --- block item declarations ----------------------------------------- */
void test_extension_block_declarations(void) {
    __extension__ int local;
    __extension__ int local_initialized = 1;
    __extension__ int local_first, local_second = 2;
    __extension__ long long local_long_long = 1LL;
    __extension__ static int local_static;
    __extension__ register int local_register;
    __extension__ const int local_const = 3;
    __extension__ typedef long long local_long_long_type;
    __extension__ local_long_long_type local_typedef_use;
    __extension__ struct Local_Extension_Struct { int x; } local_struct;
    __extension__ __extension__ int local_repeated;
    __extension__ [[note]] int local_attributed;
    __extension__ __attribute__((unused)) int local_gnu_attributed;
    {
        __extension__ int nested_local;
    }
}

/* --- for initialization ---------------------------------------------- */
int test_extension_for(int n) {
    int sum = 0;
    int j;
    for (__extension__ int i = 0; i < n; i++) sum += i;
    for (__extension__ __extension__ int i = 0, k = 1; i < n; i++) sum += k;
    for (__extension__ [[note]] int i = 0; i < n; i++) sum += i;
    for (__extension__ j = 0; __extension__ j < n; __extension__ j++) sum += j;
    return sum;
}

/* --- expression statements and expressions --------------------------- */
int extension_helper(int a, int b) {
    return a + b;
}
int test_extension_expressions(int x, int *p) {
    int y = __extension__ x;
    long long ll = __extension__ 1LL;
    struct Extension_Struct s = { .x = 1 };
    struct Extension_Struct *sp = &s;
    int arr[__extension__ 3] = { __extension__ 1, __extension__ 2, [2] = __extension__ 3 };

    __extension__ y = 1;
    __extension__ __extension__ y = 2;
    __extension__ (void)y;
    __extension__ y++;
    __extension__ extension_helper(1, 2);
    [[note]] __extension__ y = 3;
    y = __extension__ x + __extension__ 1;
    y = __extension__ x * 2 + 3;
    y += __extension__ x;
    y = (__extension__ x);
    y = -__extension__ x;
    y = !__extension__ x;
    y = ~__extension__ x;
    y = *__extension__ p;
    p = &__extension__ y;
    y = ++__extension__ y;
    y = (int)__extension__ ll;
    y = (int)sizeof __extension__ x;
    y = (int)sizeof(__extension__ x);
    y = __extension__ arr[1];
    y = arr[__extension__ 1];
    y = __extension__ s.x;
    y = __extension__ sp->x;
    y = extension_helper(__extension__ x, __extension__ 2);
    y = (__extension__ extension_helper)(1, 2);
    y = __extension__ x ? __extension__ 1 : __extension__ 2;
    y = (__extension__ x, __extension__ y);
    y = (struct Extension_Struct){ .x = __extension__ 4 }.x;
    asm("" : "=r" (y) : "r" (__extension__ x));

    if (__extension__ x) y = 0;
    if (x) __extension__ y = 0;
    while (__extension__ y) y--;
    do __extension__ y++; while (__extension__ y < 2);
    switch (__extension__ x) {
        case __extension__ 1: break;
        case __extension__ EXTENSION_VALUE: break;
        default: break;
    }
    return __extension__ y;
}
