/**
 * TEST: Typechecking trusts opaque types, builtins and `__builtin_va_list`, in every typecheck.
 *
 * Typechecking trusting opaque types, from `__builtin_*` identifiers and `__builtin_va_list`.
 * `sizeof` of opaque not being a constant, so it sizes a block scope array. Binary: pointer
 * `+`/`-` opaque on either side, arithmetic with opaque, opaque and pointer subscripts, chained
 * subscripts, integer-only operators (`%`, `&`, `|`, `^`, shifts), relational and equality against
 * pointers and arithmetic, logical, comma. `.` and `->` access on opaque, chained access,
 * assigning through opaque `->`, access through dereferenced opaque. Calls of opaque functions
 * with any arguments, nested calls, `(*opaque)()`, access and subscript on call results. Unary
 * `-`/`+`/`~`/`!`, single and double dereference, assigning through dereference, address of opaque
 * variable, prefix and postfix `++`/`--`. Casts to and from opaque. Ternary with opaque against
 * pointer, struct, arithmetic and `void` operands, opaque condition. Simple and every compound
 * assignment with opaque on either side. Initializers: opaque initializer values, positional,
 * member, array, range and nested designators on opaque type, opaque compound literal, opaque
 * values in struct and array initializers.
 */
// AI: Generated.
struct Opaque_Test { int x; };
struct Opaque_Test opaque_test_struct;
int *opaque_test_ptr;
int opaque_test_arr[4];

/* --- sizeof of opaque isn't a constant, array becomes VLA-like ------- */
void test_opaque_sizeof(void) {
    char buffer[sizeof(__builtin_va_list)];
    unsigned long size = sizeof(__builtin_va_list) + sizeof __builtin_opaque_value;
    (void)buffer;
    (void)size;
}

/* --- binary ---------------------------------------------------------- */
void test_opaque_binary(int i, double d) {
    int *p = opaque_test_ptr;
    long diff;
    p = p + __builtin_opaque_value;
    p = __builtin_opaque_value + p;
    p = p - __builtin_opaque_value;
    diff = __builtin_opaque_value - p;
    p = i + __builtin_opaque_value;
    d = d * __builtin_opaque_value;
    d = __builtin_opaque_value / d;
    i = __builtin_opaque_value[0];
    i = __builtin_opaque_value[i][i];
    i = p[__builtin_opaque_value];
    i = opaque_test_arr[__builtin_opaque_value];
    i = i % __builtin_opaque_value;
    i = __builtin_opaque_value & i;
    i = i | __builtin_opaque_value;
    i = __builtin_opaque_value ^ __builtin_opaque_value;
    i = i << __builtin_opaque_value;
    i = __builtin_opaque_value >> i;
    i = p < __builtin_opaque_value;
    i = __builtin_opaque_value >= p;
    i = d > __builtin_opaque_value;
    i = p == __builtin_opaque_value;
    i = __builtin_opaque_value != p;
    i = __builtin_opaque_value && p;
    i = (p, __builtin_opaque_value);
    (void)diff;
}

/* --- access ---------------------------------------------------------- */
void test_opaque_access(void) {
    int i;
    i = __builtin_opaque_value.member;
    i = __builtin_opaque_value->member;
    i = __builtin_opaque_value->member.nested->deep;
    __builtin_opaque_value->member = 1;
    i = (*__builtin_opaque_value).member;
    __builtin_va_list v = __builtin_opaque_value;
    opaque_test_struct.x = v;
    (void)i;
}

/* --- call ------------------------------------------------------------ */
int test_opaque_call(int i) {
    __builtin_va_list v;
    i = __builtin_opaque_function();
    i = __builtin_opaque_function(i, &i, opaque_test_ptr, 1.5, "text");
    i = __builtin_opaque_function(__builtin_opaque_function(i));
    i = (*__builtin_opaque_function)(i);
    i = __builtin_opaque_function(v)->member;
    __builtin_opaque_function(i)[0] = i;
    return __builtin_opaque_function(i) + 1;
}

/* --- unary and postfix ----------------------------------------------- */
void test_opaque_unary(void) {
    __builtin_va_list v;
    int i;
    i = -__builtin_opaque_value;
    i = +__builtin_opaque_value;
    i = ~__builtin_opaque_value;
    i = !__builtin_opaque_value;
    i = *__builtin_opaque_value;
    i = **__builtin_opaque_value;
    *__builtin_opaque_value = i;
    void *p = &v;
    v++;
    v--;
    ++v;
    --v;
    (void)p;
}

/* --- cast ------------------------------------------------------------ */
void test_opaque_cast(void) {
    __builtin_va_list v;
    int i = (int)__builtin_opaque_value;
    int *p = (int *)__builtin_opaque_value;
    double d = (double)v;
    v = (__builtin_va_list)i;
    (void)__builtin_opaque_value;
    (void)p;
    (void)d;
}

/* --- ternary --------------------------------------------------------- */
void test_opaque_ternary(int c) {
    int *p = opaque_test_ptr;
    struct Opaque_Test s = opaque_test_struct;
    int i;
    p = c ? p : __builtin_opaque_value;
    p = c ? __builtin_opaque_value : p;
    s = c ? s : __builtin_opaque_value;
    i = c ? 1 : __builtin_opaque_value;
    i = __builtin_opaque_value ? 1 : 2;
    c ? (void)0 : __builtin_opaque_value;
    (void)i;
}

/* --- assignment ------------------------------------------------------ */
void test_opaque_assignment(int i, double d) {
    __builtin_va_list v;
    int *p = opaque_test_ptr;
    struct Opaque_Test s;
    v = __builtin_opaque_value;
    s = __builtin_opaque_value;
    p = __builtin_opaque_value;
    i = v;
    p += __builtin_opaque_value;
    p -= __builtin_opaque_value;
    d *= __builtin_opaque_value;
    v /= d;
    i %= __builtin_opaque_value;
    i <<= __builtin_opaque_value;
    v >>= i;
    v &= i;
    i ^= v;
    v |= v;
    (void)s;
}

/* --- initializers and designators ------------------------------------ */
struct Opaque_Test opaque_test_designated = { .x = __builtin_opaque_value };
void test_opaque_initializers(int i) {
    __builtin_va_list a = __builtin_opaque_value;
    __builtin_va_list b = { 1, 2 };
    __builtin_va_list c = { .member = 1 };
    __builtin_va_list d = { [0] = 1, [1 ... 2] = i };
    __builtin_va_list e = { .member[0].nested = 1, [i] = 2 };
    __builtin_va_list f = (__builtin_va_list){ .member = 1 };
    struct Opaque_Test s = { .x = __builtin_opaque_value };
    int arr[2] = { __builtin_opaque_value, [1] = __builtin_opaque_value };
    int *p = __builtin_opaque_value;
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)s; (void)arr; (void)p;
}
