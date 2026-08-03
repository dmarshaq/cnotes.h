// Test: Type stringification (cn_type_stringify)
#include "../cnotes.h"

// NOTE: Cn_Type is a union, so a designated initializer that touches two different
// members clobbers the base. Everything below zero initializes first, then assigns
// through a single member.
static Cn_Type make_array(const Cn_Type *element, int64_t length) {
    Cn_Type t = {0};
    t.array.kind         = CN_ARRAY;
    t.array.element_type = (Cn_Type *)element;
    t.array.length       = length;
    t.array.size         = length >= 0 ? length * element->size : 0;
    t.array.align        = element->align;
    return t;
}

static Cn_Type make_function(const Cn_Type *return_type, Cn_Type_Function_Param *params, int64_t params_length) {
    Cn_Type t = {0};
    t.function.kind          = CN_FUNCTION;
    t.function.return_type   = (Cn_Type *)return_type;
    t.function.params        = params;
    t.function.params_length = params_length;
    return t;
}

#define CHECK(type_ptr, identifier, expected)                                                   \
    do {                                                                                        \
        char buffer_[256];                                                                      \
        Cn_String got_ = cn_type_stringify(CN_STR(sizeof(buffer_), buffer_),                     \
                                           (type_ptr), (identifier));                           \
        CN_ASSERT(got_.data != NULL);                                                            \
        CN_ASSERT(got_.length == (int64_t)(sizeof(expected) - 1));                               \
        CN_ASSERT(memcmp(got_.data, (expected), sizeof(expected) - 1) == 0);                      \
    } while (0)

#define NO_IDENTIFIER ((Cn_String) {0})

int main(void) {
    Cn_Type p_int    = cn_type_make_pointer(&CN_TYPE_INT);
    Cn_Type p_char   = cn_type_make_pointer(&CN_TYPE_CHAR);
    Cn_Type pp_char  = cn_type_make_pointer(&p_char);
    Cn_Type const_int = cn_type_make_qualified(CN_TYPE_QUALIFIED_CONSTANT, &CN_TYPE_INT);
    Cn_Type p_const_int = cn_type_make_pointer(&const_int);

    // Primitives, pointers and qualifiers: identifier lands at the end.
    {
        CHECK(&CN_TYPE_INT,  CN_STR_LIT("x"),   "int x");
        CHECK(&p_int,        CN_STR_LIT("p"),   "int *p");
        CHECK(&pp_char,      CN_STR_LIT("s"),   "char **s");
        CHECK(&const_int,    CN_STR_LIT("ci"),  "const int ci");
        CHECK(&p_const_int,  CN_STR_LIT("pci"), "const int *pci");
    }

    // Arrays: identifier goes between the base type and the bracket suffix.
    {
        Cn_Type a10_int   = make_array(&CN_TYPE_INT, 10);
        Cn_Type a_unsized = make_array(&CN_TYPE_INT, -1);
        Cn_Type a5_p_int  = make_array(&p_int, 5);
        Cn_Type p_a10_int = cn_type_make_pointer(&a10_int);

        CHECK(&a10_int,   CN_STR_LIT("a"),  "int a[10]");
        CHECK(&a_unsized, CN_STR_LIT("a"),  "int a[]");
        CHECK(&a5_p_int,  CN_STR_LIT("a"),  "int *a[5]");
        CHECK(&p_a10_int, CN_STR_LIT("pa"), "int (*pa)[10]");
    }

    // Multi dimensional arrays: outer dimension prints before inner ones.
    {
        Cn_Type a4_int    = make_array(&CN_TYPE_INT, 4);
        Cn_Type a3_a4_int = make_array(&a4_int, 3);
        Cn_Type a2_a3_a4  = make_array(&a3_a4_int, 2);

        CHECK(&a3_a4_int, CN_STR_LIT("m"),  "int m[3][4]");
        CHECK(&a2_a3_a4,  CN_STR_LIT("m3"), "int m3[2][3][4]");
    }

    // Functions. NOTE: no space between return type and '(*' for a function pointer,
    // which is valid C, just tight.
    {
        Cn_Type_Function_Param params[2] = {
            { .type = (Cn_Type *)&CN_TYPE_INT },
            { .type = &p_char },
        };
        Cn_Type fn      = make_function(&CN_TYPE_INT, params, 2);
        Cn_Type fn_void = make_function(&CN_TYPE_VOID, NULL, 0);
        Cn_Type p_fn    = cn_type_make_pointer(&fn);

        CHECK(&fn,      CN_STR_LIT("f"),  "int f(int, char *)");
        CHECK(&fn_void, CN_STR_LIT("g"),  "void g(void)");
        CHECK(&p_fn,    CN_STR_LIT("fp"), "int(*fp)(int, char *)");
    }

    // The cases the suffix ordering exists for: the array brackets and the parameter
    // list have to stay on the correct side of the enclosing parentheses.
    {
        Cn_Type_Function_Param params[1] = { { .type = (Cn_Type *)&CN_TYPE_INT } };
        Cn_Type fn       = make_function(&CN_TYPE_INT, params, 1);
        Cn_Type p_fn     = cn_type_make_pointer(&fn);
        Cn_Type a3_p_fn  = make_array(&p_fn, 3);

        Cn_Type a4_int    = make_array(&CN_TYPE_INT, 4);
        Cn_Type p_a4_int  = cn_type_make_pointer(&a4_int);
        Cn_Type a3_p_a4   = make_array(&p_a4_int, 3);

        Cn_Type a10_int   = make_array(&CN_TYPE_INT, 10);
        Cn_Type p_a10_int = cn_type_make_pointer(&a10_int);
        Cn_Type fn_ret_p_a10 = make_function(&p_a10_int, NULL, 0);

        // Array of 3 pointers to function, NOT a function returning an array.
        CHECK(&a3_p_fn,      CN_STR_LIT("fps"), "int(*fps[3])(int)");
        // Array of 3 pointers to int[4].
        CHECK(&a3_p_a4,      CN_STR_LIT("x"),   "int (*x[3])[4]");
        // Function returning a pointer to int[10].
        CHECK(&fn_ret_p_a10, CN_STR_LIT("f2"),  "int (*f2(void))[10]");
    }

    // Empty identifier keeps the plain type spelling used by diagnostics.
    {
        Cn_Type a10_int   = make_array(&CN_TYPE_INT, 10);
        Cn_Type p_a10_int = cn_type_make_pointer(&a10_int);

        Cn_Type_Function_Param params[1] = { { .type = (Cn_Type *)&CN_TYPE_INT } };
        Cn_Type fn      = make_function(&CN_TYPE_INT, params, 1);
        Cn_Type p_fn    = cn_type_make_pointer(&fn);
        Cn_Type a3_p_fn = make_array(&p_fn, 3);

        CHECK(&CN_TYPE_INT, NO_IDENTIFIER, "int");
        CHECK(&p_int,       NO_IDENTIFIER, "int *");
        CHECK(&a10_int,     NO_IDENTIFIER, "int[10]");
        CHECK(&p_a10_int,   NO_IDENTIFIER, "int (*)[10]");
        CHECK(&fn,          NO_IDENTIFIER, "int(int)");
        CHECK(&a3_p_fn,     NO_IDENTIFIER, "int(*[3])(int)");
    }

    return 0;
}
