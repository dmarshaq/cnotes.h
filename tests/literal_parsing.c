// Test: Literal parsing (cn_parse_int_literal, cn_parse_float_literal)
#include "../cnotes.h"

int main(void) {
    uint64_t int_result;
    double float_result;
    bool ok;

    // Test cn_parse_int_literal - decimal
    {
        ok = cn_parse_int_literal(CN_STR_LIT("42"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);

        ok = cn_parse_int_literal(CN_STR_LIT("0"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 0);

        ok = cn_parse_int_literal(CN_STR_LIT("123456789"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 123456789);
    }

    // Test cn_parse_int_literal - hexadecimal
    {
        ok = cn_parse_int_literal(CN_STR_LIT("0x10"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 16);

        ok = cn_parse_int_literal(CN_STR_LIT("0xFF"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 255);

        ok = cn_parse_int_literal(CN_STR_LIT("0xABCD"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 0xABCD);

        ok = cn_parse_int_literal(CN_STR_LIT("0X1a2B"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 0x1A2B);
    }

    // Test cn_parse_int_literal - octal
    {
        ok = cn_parse_int_literal(CN_STR_LIT("010"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 8);

        ok = cn_parse_int_literal(CN_STR_LIT("0777"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 511);
    }

    // Test cn_parse_int_literal - binary (GCC extension)
    // TODO: Enable when binary literals are supported
    // {
    //     ok = cn_parse_int_literal(CN_STR_LIT("0b1010"), &int_result);
    //     CN_ASSERT(ok == true);
    //     CN_ASSERT(int_result == 10);

    //     ok = cn_parse_int_literal(CN_STR_LIT("0B11111111"), &int_result);
    //     CN_ASSERT(ok == true);
    //     CN_ASSERT(int_result == 255);
    // }

    // Test cn_parse_int_literal - with suffixes
    {
        ok = cn_parse_int_literal(CN_STR_LIT("42u"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);

        ok = cn_parse_int_literal(CN_STR_LIT("42U"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);

        ok = cn_parse_int_literal(CN_STR_LIT("42l"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);

        ok = cn_parse_int_literal(CN_STR_LIT("42L"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);

        ok = cn_parse_int_literal(CN_STR_LIT("42ul"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);

        ok = cn_parse_int_literal(CN_STR_LIT("42ULL"), &int_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(int_result == 42);
    }

    // Test cn_parse_float_literal - basic
    {
        ok = cn_parse_float_literal(CN_STR_LIT("3.14"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 3.13 && float_result < 3.15);

        ok = cn_parse_float_literal(CN_STR_LIT("0.5"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 0.49 && float_result < 0.51);

        ok = cn_parse_float_literal(CN_STR_LIT("100.0"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 99.9 && float_result < 100.1);
    }

    // Test cn_parse_float_literal - scientific notation
    {
        ok = cn_parse_float_literal(CN_STR_LIT("1e10"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 9e9 && float_result < 1.1e10);

        ok = cn_parse_float_literal(CN_STR_LIT("1.5e2"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 149 && float_result < 151);

        ok = cn_parse_float_literal(CN_STR_LIT("1E-3"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 0.0009 && float_result < 0.0011);
    }

    // Test cn_parse_float_literal - with suffixes
    {
        ok = cn_parse_float_literal(CN_STR_LIT("3.14f"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 3.13 && float_result < 3.15);

        ok = cn_parse_float_literal(CN_STR_LIT("3.14F"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 3.13 && float_result < 3.15);

        ok = cn_parse_float_literal(CN_STR_LIT("3.14l"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 3.13 && float_result < 3.15);

        ok = cn_parse_float_literal(CN_STR_LIT("3.14L"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 3.13 && float_result < 3.15);
    }

    // Test cn_parse_float_literal - edge cases
    {
        ok = cn_parse_float_literal(CN_STR_LIT(".5"), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 0.49 && float_result < 0.51);

        ok = cn_parse_float_literal(CN_STR_LIT("5."), &float_result);
        CN_ASSERT(ok == true);
        CN_ASSERT(float_result > 4.9 && float_result < 5.1);
    }

    return 0;
}
