/**
 * TEST: Attribute specifiers, C style `[[...]]` and GNU style `__attribute__((...))`.
 *
 * `[[...]]` specifiers on external declarations, function definitions and declarations, block
 * scoped declarations, and every statement kind (expression, empty, block, `if`, `switch`,
 * `while`, `do while`, `for` including its init declaration, `goto`, `break`, `continue`,
 * `return`, labels); empty `[[]]`, multiple specifiers, multiple attributes per specifier,
 * attribute arguments, string arguments, and `vendor::name` namespaced attributes; on `enum`
 * specifiers and enumerators. GNU `__attribute__((...))` leading and trailing on declarations, on
 * function definitions, `struct`/`union` specifiers and members, `enum` specifiers and
 * enumerators, locals, with arguments and string arguments; both styles combined on one
 * declaration.
 *
 * --gnu
 */
/* --- [[...]] on external declarations and definitions ---------------- */
[[note]] int attributed_global;
[[note]] static int attributed_static_global = 1;
[[]] int empty_attribute_global;
[[first]] [[second]] int two_specifiers_global;
[[first, second]] int two_attributes_global;
[[note(1)]] int attribute_with_argument;
[[note(1, 2, 3)]] int attribute_with_arguments;
[[note("text")]] int attribute_with_string_argument;
[[vendor::note]] int namespaced_attribute_global;
[[vendor::note(42)]] int namespaced_attribute_with_argument;

[[note]] int attributed_declaration(int x);

[[note]] int attributed_function(int x) {
    return x;
}

[[note]] struct AttributedStructUser {
    int x;
} attributed_struct_global;

/* --- [[...]] on statements ------------------------------------------- */
int test_attributes_on_statements(int x) {
    [[note]] int local = 0;
    [[note]] local = x;
    [[note]] ;

    [[note]] {
        local = local + 1;
    }

    [[note]] if (x > 0) {
        local = 1;
    } else {
        local = 2;
    }

    [[note]] switch (x) {
        default:
            local = 3;
            break;
    }

    [[note]] while (local > 0) {
        [[note]] break;
    }

    [[note]] do {
        [[note]] continue;
    } while (0);

    [[note]] for ([[note]] int i = 0; i < x; i = i + 1) {
        local = local + i;
    }

    [[note]] goto finish;

    [[note]] finish:
    [[note]] return local;
}

int test_multiple_attributes_on_statement(int x) {
    [[first]] [[second]] x = x + 1;
    [[first, second]] x = x + 1;
    [[vendor::note(1)]] x = x + 1;
    return x;
}

/* --- [[...]] on enums ------------------------------------------------ */
enum [[note]] AttributedEnum {
    AE_A [[note]],
    AE_B [[note]] = 5,
    AE_C [[vendor::note(1)]],
    AE_D [[first, second]]
};

/* --- GNU __attribute__((...)) ---------------------------------------- */
__attribute__((unused)) int gnu_leading_global;
int gnu_trailing_global __attribute__((unused));
int gnu_two_specifiers __attribute__((unused)) __attribute__((deprecated));
int gnu_two_attributes __attribute__((unused, deprecated));
int gnu_attribute_with_argument __attribute__((aligned(16)));
int gnu_attribute_with_string __attribute__((section("mysection")));
__attribute__((unused)) static int gnu_static_global;

__attribute__((unused)) int gnu_attributed_declaration(int x);

__attribute__((unused)) int gnu_attributed_function(int x) {
    return x;
}

struct __attribute__((packed)) GnuPackedStruct {
    int x;
    int y __attribute__((aligned(8)));
};

union __attribute__((packed)) GnuPackedUnion {
    int i;
    float f;
};

enum __attribute__((unused)) GnuAttributedEnum {
    GAE_A __attribute__((unused)),
    GAE_B __attribute__((deprecated)) = 2
};

int test_gnu_attributes_on_locals(int x) {
    __attribute__((unused)) int a = 0;
    int b __attribute__((unused)) = 0;
    return x + a + b;
}

/* --- both styles combined -------------------------------------------- */
[[note]] __attribute__((unused)) int mixed_leading_global;
[[note]] int mixed_trailing_global __attribute__((unused));

[[note]] __attribute__((unused)) int test_mixed_attributes(int x) {
    [[note]] __attribute__((unused)) int local = x;
    return local;
}
