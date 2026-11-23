/**
 * TEST: Compound literal expression parsing.
 *
 * Compound literals: scalar (`(int){42}`, `(float){3.14}`), array (`(int[3]){1, 2, 3}`) with
 * subscripting, struct with designated initializers, address of a compound literal followed by
 * `->` access.
 */
struct point { int x; int y; };

int test_compound_literals(void) {
    int i;
    int *pi;
    float f;
    struct point p;
    struct point *pp;

    i = (int){42};
    f = (float){3.14};

    pi = (int[3]){1, 2, 3};
    i = pi[1];

    p = (struct point){.x = 1, .y = 2};
    i = p.x + p.y;

    pp = &(struct point){10, 20};
    i = pp->x + pp->y;

    return i;
}
