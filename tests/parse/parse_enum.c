/**
 * TEST: Enum specifiers, enumerators and enum constants.
 *
 * `enum` definitions with implicit, explicit and mixed enumerator values; 
 * values from constant expressions referencing earlier enumerators, `sizeof` and negatives; 
 * trailing comma; single enumerator; anonymous enums; named and anonymous `typedef enum`; 
 * fixed underlying type (`enum E : unsigned int`) in both definition and use; enums as globals, locals, 
 * parameters, return types and struct members; block scoped enum definitions; 
 * enum constants in initializers, array sizes, conditions and expressions; `sizeof` of enum types 
 * and constants.
 */
enum Color {
    RED,
    GREEN,
    BLUE,
};

enum Explicit {
    E_ONE = 1,
    E_TWO = 2,
    E_TEN = 10,
};

enum Mixed {
    M_ZERO,
    M_FIVE = 5,
    M_SIX,
    M_SEVEN,
};

enum Computed {
    C_A = 1,
    C_B = C_A + 1,
    C_C = C_A | C_B,
    C_D = 1 << 4,
    C_E = sizeof(int),
    C_F = -1,
};

enum Trailing {
    T_A,
    T_B,
};

enum Single { S_ONLY, };

enum {
    ANON_A = 10,
    ANON_B,
};

typedef enum Season {
    WINTER,
    SPRING,
    SUMMER,
    AUTUMN,
} Season;

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
} State;

enum Fixed : unsigned int {
    FX_A,
    FX_B,
};

enum Color global_color;
enum Color global_color_init = GREEN;
Season global_season = SPRING;
State global_state = STATE_IDLE;
enum Fixed : unsigned int global_fixed;         // TODO: Should not be passed.

int color_table[BLUE + 1];
int anon_table[ANON_B];
static const int const_from_enum = E_TEN * 2;

enum Color test_enum_return(void) {
    return RED;
}

int test_enum_parameter(enum Color c, Season s) {
    return c + s;
}

int test_enum_locals(void) {
    enum Color local = BLUE;
    enum Mixed m = M_SEVEN;
    Season s = AUTUMN;
    State st = STATE_RUNNING;

    enum Local {
        L_A,
        L_B = 3
    };
    enum Local l = L_B;

    return local + m + s + st + l;
}

int test_enum_constants_in_expressions(int x) {
    int r;
    r = RED + GREEN + BLUE;
    r = x > 0 ? E_ONE : E_TWO;
    r = C_C & C_D;
    r = (int)ANON_A;
    if (x == BLUE) r = 1;
    return r;
}

int test_sizeof_enum(void) {
    int x;
    x = sizeof(enum Color);
    x = sizeof(Season);
    x = sizeof(RED);
    return x;
}

struct WithEnum {
    enum Color color;
    Season season;
    enum Inner {
        IN_A,
        IN_B,
    } inner;
};

int test_enum_member_access(struct WithEnum *w) {
    w->color = RED;
    w->inner = IN_B;
    return w->color + w->inner;
}

int test_enum_array_initializer(void) {
    int values[3] = { RED, GREEN, BLUE, };
    return values[0];
}
