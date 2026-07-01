// Test: String operations (cn_str_*)
#include "../cnotes.h"

int main(void) {
    // Test cn_str_equals
    {
        Cn_String a = CN_CSTR("hello");
        Cn_String b = CN_CSTR("hello");
        Cn_String c = CN_CSTR("world");
        Cn_String empty1 = CN_CSTR("");
        Cn_String empty2 = {0};

        CN_ASSERT(cn_str_equals(&a, &b) == true);
        CN_ASSERT(cn_str_equals(&a, &c) == false);
        CN_ASSERT(cn_str_equals(&empty1, &empty2) == true);
    }

    // Test cn_str_is_empty
    {
        Cn_String empty1 = CN_CSTR("");
        Cn_String empty2 = {0};
        Cn_String nonempty = CN_CSTR("x");

        CN_ASSERT(cn_str_is_empty(empty1) == true);
        CN_ASSERT(cn_str_is_empty(empty2) == true);
        CN_ASSERT(cn_str_is_empty(nonempty) == false);
    }

    // Test cn_str_substring
    {
        Cn_String str = CN_CSTR("hello world");
        Cn_String sub = cn_str_substring(str, 0, 5);
        Cn_String expected = CN_CSTR("hello");
        CN_ASSERT(cn_str_equals(&sub, &expected));

        sub = cn_str_substring(str, 6, 11);
        expected = CN_CSTR("world");
        CN_ASSERT(cn_str_equals(&sub, &expected));
    }

    // Test cn_str_find_left
    {
        Cn_String haystack = CN_CSTR("hello world");
        Cn_String needle = CN_CSTR("world");
        CN_ASSERT(cn_str_find_left(haystack, needle) == 6);

        needle = CN_CSTR("hello");
        CN_ASSERT(cn_str_find_left(haystack, needle) == 0);

        needle = CN_CSTR("xyz");
        CN_ASSERT(cn_str_find_left(haystack, needle) == -1);
    }

    // Test cn_str_find_right
    {
        Cn_String haystack = CN_CSTR("abcabc");
        Cn_String needle = CN_CSTR("bc");
        CN_ASSERT(cn_str_find_right(haystack, needle) == 4);
    }

    // Test cn_str_find_char_left
    {
        Cn_String str = CN_CSTR("hello");
        CN_ASSERT(cn_str_find_char_left(str, 'h') == 0);
        CN_ASSERT(cn_str_find_char_left(str, 'l') == 2);
        CN_ASSERT(cn_str_find_char_left(str, 'o') == 4);
        CN_ASSERT(cn_str_find_char_left(str, 'x') == -1);
    }

    // Test cn_str_find_char_right
    {
        Cn_String str = CN_CSTR("hello");
        CN_ASSERT(cn_str_find_char_right(str, 'l') == 3);
        CN_ASSERT(cn_str_find_char_right(str, 'h') == 0);
    }

    // Test cn_str_is_symbol
    {
        CN_ASSERT(cn_str_is_symbol(CN_CSTR("foo")) == true);
        CN_ASSERT(cn_str_is_symbol(CN_CSTR("_bar")) == true);
        CN_ASSERT(cn_str_is_symbol(CN_CSTR("foo123")) == true);
        CN_ASSERT(cn_str_is_symbol(CN_CSTR("123foo")) == false);
        CN_ASSERT(cn_str_is_symbol(CN_CSTR("")) == false);
    }

    // Test cn_str_is_int
    {
        CN_ASSERT(cn_str_is_int(CN_CSTR("123")) == true);
        CN_ASSERT(cn_str_is_int(CN_CSTR("-456")) == true);
        CN_ASSERT(cn_str_is_int(CN_CSTR("abc")) == false);
        CN_ASSERT(cn_str_is_int(CN_CSTR("12.3")) == false);
    }

    // Test cn_str_parse_int
    {
        CN_ASSERT(cn_str_parse_int(CN_CSTR("42")) == 42);
        CN_ASSERT(cn_str_parse_int(CN_CSTR("-10")) == -10);
        CN_ASSERT(cn_str_parse_int(CN_CSTR("0")) == 0);
    }

    // Test cn_str_count_chars
    {
        Cn_String str = CN_CSTR("hello");
        CN_ASSERT(cn_str_count_chars(str, 'l') == 2);
        CN_ASSERT(cn_str_count_chars(str, 'h') == 1);
        CN_ASSERT(cn_str_count_chars(str, 'x') == 0);
    }

    // Test cn_str_hash (consistency)
    {
        Cn_String a = CN_CSTR("test");
        Cn_String b = CN_CSTR("test");
        Cn_String c = CN_CSTR("different");

        CN_ASSERT(cn_str_hash(&a) == cn_str_hash(&b));
        CN_ASSERT(cn_str_hash(&a) != cn_str_hash(&c));
    }

    // Test cn_str_eat_chars
    {
        Cn_String str = CN_CSTR("hello");
        Cn_String result = cn_str_eat_chars(str, 2);
        Cn_String expected = CN_CSTR("llo");
        CN_ASSERT(cn_str_equals(&result, &expected));
    }

    // Test cn_str_get_chars
    {
        Cn_String str = CN_CSTR("hello");
        Cn_String result = cn_str_get_chars(str, 2);
        Cn_String expected = CN_CSTR("he");
        CN_ASSERT(cn_str_equals(&result, &expected));
    }

    // Test cn_str_find_whitespace_left
    {
        Cn_String str = CN_CSTR("hello world");
        CN_ASSERT(cn_str_find_whitespace_left(str) == 5);

        str = CN_CSTR("nospace");
        CN_ASSERT(cn_str_find_whitespace_left(str) == -1);
    }

    // Test cn_str_eat_spaces
    {
        Cn_String str = CN_CSTR("   hello");
        Cn_String result = cn_str_eat_spaces(str);
        Cn_String expected = CN_CSTR("hello");
        CN_ASSERT(cn_str_equals(&result, &expected));
    }

    // Test cn_str_format
    {
        char buffer[64];
        Cn_String buf = { .data = buffer, .length = sizeof(buffer) };
        Cn_String result = cn_str_format(buf, "value: %d", 42);
        Cn_String expected = CN_CSTR("value: 42");
        CN_ASSERT(cn_str_equals(&result, &expected));
    }

    return 0;
}
