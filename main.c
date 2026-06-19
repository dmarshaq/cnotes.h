// ============================================================================
// cnotes typecheck — FULL expression test
//
// Every expression-typecheck case is now implemented; this exercises all of
// them, including the three that were just finished (postfix ++/--, sizeof,
// ternary ?:), plus deep nested combinations and an extensive error section.
//
// Build:  gcc -E -o typecheck_full_test.i typecheck_full_test.c
//         then feed the .i to the tool.
//
// ACTIVE statements should all typecheck cleanly. Two trailing blocks are
// commented out:
//   * "DEPENDS ON OPTIONAL PATCHES" — pass only if the matching patch is in.
//   * "EXPECTED ERRORS"             — uncomment one at a time to see diagnostics.
//
// Type-printing note: your stringifier has no typedef names, so it prints
// `long`/`long long`/`ptrdiff_t` as "long long" and `size_t` as
// "unsigned long long". Annotations below use the conceptual C name.
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
    int       i = 0, j = 0;
    unsigned  u = 0x10;
    char      c = 0;
    short     sh = 0;
    long      lng = 0;
    double    d = 2.0;
    float     f = 1.5f;
    _Bool     bl = 0;

    int      *p  = 0, *q = 0;
    int     **pp = 0;
    char     *cp = 0;
    void     *vp = 0;

    struct Vec     v;
    struct Entity  ent, ent2;
    struct Entity *ep = 0;

    // ------------------------------------------------------------------
    // Primary expressions
    // ------------------------------------------------------------------
    i;                     // int
    0x2A;                  // int
    3.5f;                  // float

    // ------------------------------------------------------------------
    // Arithmetic + usual arithmetic conversions / promotion
    // ------------------------------------------------------------------
    i + j;                 // int
    i + u;                 // unsigned int
    i + sh;                // int     (short -> int by rank)
    i + lng;               // long
    u + lng;               // long    (long wider than unsigned int)
    c * c;                 // int     (char operands promote to int)
    sh * sh;               // int     (short operands promote to int)
    f + i;                 // float
    f + d;                 // double
    d / 2;                 // double

    // ------------------------------------------------------------------
    // Bitwise / shift
    // ------------------------------------------------------------------
    u & 0xF;               // unsigned int
    i | j;                 // int
    sh ^ sh;               // int     (promotes to int)
    i << 2;                // int
    u >> 1;                // unsigned int
    c << 4;                // int     (left promotes to int)
    u % 3;                 // unsigned int

    // ------------------------------------------------------------------
    // Relational / equality / logical
    // ------------------------------------------------------------------
    i < j;                 // int
    u <= 5;                // int
    f > 0;                 // int
    p == q;                // int
    p != 0;                // int
    i && j;                // int
    p || i;                // int
    !p;                    // int
    !bl;                   // int     (_Bool is scalar)

    // ------------------------------------------------------------------
    // Comma (type of right operand)
    // ------------------------------------------------------------------
    i, f;                  // float
    (i, j, d);             // double

    // ------------------------------------------------------------------
    // Pointer arithmetic / subscript / deref / address-of
    // ------------------------------------------------------------------
    p + 5;                 // int *
    5 + p;                 // int *
    p - 2;                 // int *
    p - q;                 // ptrdiff_t
    *p;                    // int
    *(p + i);              // int
    p[i];                  // int
    pp[0];                 // int *
    pp[0][1];              // int
    &i;                    // int *
    &*p;                   // int *
    &p;                    // int **
    **pp;                  // int
    cp + 1;                // char *
    &ent;                  // struct Entity *
    &ent.id;               // int *   (address of a member)

    // ------------------------------------------------------------------
    // Struct member access (nested + through pointers)
    // ------------------------------------------------------------------
    ent.id;                // int
    ent.health;            // float
    ent.pos;               // struct Vec
    ent.pos.x;             // int
    ent.vel->y;            // int
    *ent.tag;              // int
    ep->id;                // int
    ep->pos.y;             // int
    (*ep).health;          // float
    ep->vel->x;            // int
    v.x + v.y;             // int

    // ------------------------------------------------------------------
    // Function calls (fixed, variadic, nested, casts-as-args, void)
    // ------------------------------------------------------------------
    add(i, j);             // int
    scale(f, i);           // float
    sum(2, i, j);          // int     (variadic: trailing args unchecked)
    sum(0);                // int     (variadic: just the fixed arg)
    make_vec(1, 2);        // struct Vec *
    make_vec(1, 2)->x;     // int
    noop(i);               // void
    add(add(1, 2), 3);     // int     (nested)
    add((int)f, (short)i); // int     (casts as arguments)

    // ------------------------------------------------------------------
    // Casts
    // ------------------------------------------------------------------
    (float)i;              // float
    (int)f;                // int
    (double)i;             // double
    (char)u;               // char
    (unsigned)i;           // unsigned int
    (int)p;                // int          (pointer -> integer)
    (int *)i;              // int *        (integer -> pointer)
    (void *)p;             // void *
    (struct Vec *)vp;      // struct Vec * (pointer -> pointer)
    (void)i;               // void

    // ------------------------------------------------------------------
    // Unary
    // ------------------------------------------------------------------
    -i;                    // int
    +f;                    // float
    ~i;                    // int
    ~c;                    // int     (promotes)
    !i;                    // int
    ++i;                   // int
    --p;                   // int *

    // ------------------------------------------------------------------
    // Postfix
    // ------------------------------------------------------------------
    i++;                   // int
    i--;                   // int
    p++;                   // int *
    f--;                   // float
    ent.id++;              // int
    p[0]++;                // int
    (*p)++;                // int
    ep->id--;              // int

    // ------------------------------------------------------------------
    // sizeof (type-name and expression forms)
    // ------------------------------------------------------------------
    sizeof(int);           // size_t
    sizeof i;              // size_t
    sizeof(struct Entity); // size_t
    sizeof(int *);         // size_t
    sizeof ent;            // size_t
    sizeof ep;             // size_t
    sizeof(i + j);         // size_t
    sizeof p[0];           // size_t
    sizeof(i ? f : d);     // size_t  (sizeof of a ternary)

    // ------------------------------------------------------------------
    // Ternary ?:
    // ------------------------------------------------------------------
    i ? j : 0;             // int
    i ? f : j;             // float   (usual arithmetic conversions)
    i ? d : f;             // double
    i ? p : q;             // int *
    i ? p : 0;             // int *   (pointer / null constant)
    i ? 0 : p;             // int *
    i ? p : vp;            // void *  (one side is void*)
    i ? vp : p;            // void *
    p ? i : j;             // int     (condition may be a pointer)
    bl ? i : j;            // int     (condition may be _Bool)
    i ? ent : ent2;        // struct Entity
    i ? (j + 1) : (f * 2); // float

    // ------------------------------------------------------------------
    // Assignment (simple + compound)
    // ------------------------------------------------------------------
    i = j;                 // int
    i = u + 2;             // int
    u = i;                 // unsigned int
    f += i;                // float
    i -= 1;                // int
    i *= 2;                // int
    f /= 2;                // float
    i %= 3;                // int
    i <<= 2;               // int
    u >>= 1;               // unsigned int
    i &= 0xF;              // int
    i |= 1;                // int
    i ^= 1;                // int
    p = &i;                // int *
    p = q;                 // int *
    p += 2;                // int *
    p -= 1;                // int *
    *p = i;                // int
    ent.id = 5;            // int
    ep->health = f;        // float
    pp = &p;               // int **

    // ------------------------------------------------------------------
    // Big nested combinations
    // ------------------------------------------------------------------
    (ent.health + (float)ep->id) * scale(f, i);            // float
    *(p + (i & 3)) + ~c;                                   // int
    add(i, ep->pos.x) + p[0x1] - (*ep).id;               // int
    (i < u) && (p != 0) && (ep->health > 0.0f);            // int
    i ? add(p[0], *q) : (int)(f + d);                      // int
    ep ? ep->vel : (struct Vec *)0;                        // struct Vec *
    sizeof(i ? p : q) + (unsigned)(p - q);                 // size_t
    (i++ , p[0] , ep->health) > 0.0f;                      // int
    !(i < j) == (u >= 1);                                  // int

    // ============================================================
    // DEPENDS ON OPTIONAL PATCHES — uncomment if the patch is in.
    //   If these error, it's the patch missing, not the typechecker.
    // ============================================================
    p  = 0;          // null-pointer-constant in assignment path
    vp = p;          // void* interop in cn_type_is_assignable
    p  = vp;         // void* interop in cn_type_is_assignable

    // ============================================================
    // EXPECTED ERRORS — uncomment individually to verify diagnostics.
    // ============================================================
    // *vp;             // dereference of void*
    // i.x;             // member access on non-struct
    // ep.id;           // '.' on a pointer (should be '->')
    // ent->id;         // '->' on a non-pointer (should be '.')
    // add(i);          // too few arguments
    // add(i, j, 1);    // too many arguments (non-variadic)
    // ~f;              // '~' requires integer
    // f % 2;           // '%' requires integer
    // i << f;          // shift requires integer operands
    // p + f;           // pointer + non-integer
    // p * 2;           // '*' on a pointer
    // p + q;           // pointer + pointer
    // (struct Vec)i;   // cast to non-scalar type
    // (float)p;        // cast between float and pointer
    // 5 = i;           // assignment to non-lvalue
    // (i + 1) = j;     // assignment to non-lvalue (binary result)
    // &5;              // address-of non-lvalue
    // ++5;             // prefix ++ on non-lvalue
    // i = ep;          // struct* not assignable to int
    // i ? j : p;       // int (non-constant) vs pointer in ?:
    // ent ? i : j;     // ?: condition not scalar
    // sizeof(void);    // sizeof of incomplete type
    // sizeof add;      // sizeof of function type
}
