/**
 * TEST: GNU `typeof` type specifier with type name and expression operands.
 *
 * `typeof`, `__typeof` and `__typeof__` spellings on globals, struct members, parameters and locals;
 * type name operands (`int *`, `char`, `struct S`), expression operands of variables, parameters and
 * arithmetic (`g + d` becoming `double`); qualified `const typeof(y) *`, casts to `typeof`, `sizeof`
 * of `typeof`, and constant expressions that only hold if the resolved type is right.
 *
 * --gnu
 */

int g;
double d;
typeof(g) g2;
__typeof(int *) gp;
__typeof__(g + d) gd;
struct S { typeof(g) a; typeof(char) b[4]; };
int check_double[sizeof(typeof(gd)) == sizeof(double) ? 1 : 0];
int check_pointer[sizeof(typeof(gp)) == sizeof(void *) ? 1 : 0];
int check_struct[sizeof(typeof(struct S)) == 8 ? 1 : 0];

int f(typeof(g) x) {
    typeof(x) y = x;
    const typeof(y) *p = &y;
    typeof(*p) z = *p;
    return (typeof(y))sizeof(typeof(struct S)) + z;
}
