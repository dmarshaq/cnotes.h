// ============================================================================
// cnotes typecheck checkpoint test
//
// Exercises every expression-typecheck case implemented so far:
//   - binary: + - * / % << >> < <= > >= == != & ^ | && || , and []
//   - member access: . and ->   (incl. nested and through pointers)
//   - function calls: fixed-arity, variadic, nested, casts-as-args, void return
//   - casts: arithmetic, pointer<->integer, cast-to-void
//   - unary: + - ! ~ * &  and prefix ++ --
//   - assignment: simple = and compound (+= -= <<= ...), null pointer constant
//
// NOT yet implemented (deliberately left out of the active section):
//   sizeof, ternary ?:, postfix ++/--
//
// Build:  gcc -E -o checkpoint_test.i checkpoint_test.c
//         then feed checkpoint_test.i to the tool.
//
// Every ACTIVE statement should typecheck cleanly (the typechecker prints the
// resulting type as an info diagnostic). The trailing block lists statements
// that SHOULD error — uncomment them one at a time to verify diagnostics.
// ============================================================================

struct Vec { int x; int y; };

struct Entity {
    int         id;
    float       health;
    struct Vec  pos;
    struct Vec *vel;
    int        *tag;
};

int          add(int a, int b);
float        scale(float v, int factor);
int          sum(int count, ...);          // variadic
struct Vec  *make_vec(int x, int y);
void         noop(int x);

int main(void) {
    int       i  = 0;
    unsigned  u  = 0x10;
    char      c  = 0;
    float     f  = 1.5f;
    double    d  = 2.0;
    int      *p  = 0;
    int     **pp = 0;
    void     *vp = 0;

    struct Vec     v;
    struct Entity  ent;
    struct Entity *ep = 0;

    // --- arithmetic + usual arithmetic conversions / promotion ---
    i + u * 2;             // unsigned int
    c * c + i;             // int     (char operands promote to int)
    f + i * 2;             // float
    d * f - i;             // double
    (char)u + 1;           // int     (char + int -> int by rank)

    // --- bitwise / shift ---
    u & 0xF | i << 2;      // unsigned int
    ~c & 0xFF;             // int
    u >> c;                // unsigned int

    // --- relational / equality / logical ---
    i < u && f > 0;        // int
    p == 0 || i;           // int     (pointer vs null constant)
    !p;                    // int
    ep != 0;               // int

    // --- comma (type of the right operand) ---
    i, f;                  // float
    f, i + 1;              // int

    // --- pointer arithmetic + subscript + deref ---
    p + 5;                 // int *
    (p + 5) - p;           // ptrdiff_t
    *(p + 3);              // int
    p[2] + 1;              // int
    pp[0][1];              // int
    p += 2;                // int *   (compound assign: pointer += int)

    // --- address-of / deref / multi-level ---
    &i;                    // int *
    *&i;                   // int
    **pp;                  // int
    *p + i;                // int

    // --- struct member access (nested + through pointers) ---
    ent.id + 1;            // int
    ent.health * 2;        // float
    ent.pos.x + ent.pos.y; // int
    ep->id;                // int
    ep->pos.x;             // int
    (*ep).health;          // float
    ep->vel->y;            // int
    *ep->tag;              // int     (-> binds tighter than unary *)
    v.x + v.y;             // int

    // --- function calls ---
    add(i, 2);             // int
    add(i, 2) + add(3, 4); // int
    scale(f, i) + 1.0f;    // float
    sum(3, i, u, c);       // int     (variadic: trailing args unchecked)
    make_vec(1, 2);        // struct Vec *
    make_vec(1, 2)->x;     // int
    add((int)f, (int)d);   // int
    noop(i);               // void

    // --- casts ---
    (double)i + f;         // double
    (int)f * 2;            // int
    (int)p + 1;            // int     (pointer -> integer)
    (void)u;               // void

    // --- assignment (simple + compound) ---
    i = u + 2;             // int
    f += i;                // float
    p = &i;                // int *
    p = 0;                 // int *   (null pointer constant)
    *p = i + 1;            // int
    ent.id = 5;            // int
    ep->health = f;        // float
    u <<= 2;               // unsigned int
    ep->tag = &i;          // int *

    // --- big nested combinations ---
    (ent.health + (float)ep->id) * scale(f, i);     // float
    *(p + (i & 3)) + ~c;                             // int
    add(i, ep->pos.x) + p[0x2] - (*ep).id;           // int
    (i < u) && (p != 0) && (ep->health > 0.0f);      // int

    // ========================================================================
    // Expected ERRORS — uncomment individually to verify diagnostics
    // ========================================================================
    // *vp;                // dereference of void*
    // i.x;                // member access on non-struct
    // add(i);             // too few arguments (non-variadic)
    // ~f;                 // '~' requires integer
    // p + f;              // pointer + non-integer
    // (struct Vec)i;      // cast to non-scalar type
    // 5 = i;              // assignment to non-lvalue
    // &5;                 // address-of non-lvalue
    // i = ep;             // struct* not assignable to int
}
