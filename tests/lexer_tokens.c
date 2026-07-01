// Test: Lexer token recognition (keywords, identifiers, literals, operators)
#include "../cnotes.h"
#include <stdio.h>

int main(void) {
    Cn_Lexer lexer;

    // Test keywords
    {
        cn_lexer_init(&lexer, CN_CSTR("int char float double void"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_CHAR);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_FLOAT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_DOUBLE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_VOID);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EOF);
    }

    // Test storage class and qualifier keywords
    {
        cn_lexer_init(&lexer, CN_CSTR("static extern const volatile register auto typedef"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_STATIC);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EXTERN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_CONST);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_VOLATILE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_REGISTER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_AUTO);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_TYPEDEF);
    }

    // Test control flow keywords
    {
        cn_lexer_init(&lexer, CN_CSTR("if else switch while do for goto continue break return"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IF);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ELSE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SWITCH);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_WHILE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_DO);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_FOR);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_GOTO);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_CONTINUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_BREAK);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_RETURN);
    }

    // Test type keywords
    {
        cn_lexer_init(&lexer, CN_CSTR("struct union enum sizeof"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_STRUCT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_UNION);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ENUM);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SIZEOF);
    }

    // Test identifiers
    {
        cn_lexer_init(&lexer, CN_CSTR("foo bar_baz _underscore myVar123"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
    }

    // Test integer literals
    {
        cn_lexer_init(&lexer, CN_CSTR("42 0 123 0x1F"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INTEGER_VALUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INTEGER_VALUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INTEGER_VALUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INTEGER_VALUE);
    }

    // TODO: Test octal literals (0777) - may not be supported
    // {
    //     cn_lexer_init(&lexer, CN_CSTR("0777"), (Cn_Lexer_Blacklist){0});
    //     CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INTEGER_VALUE);
    // }

    // Test float literals
    {
        cn_lexer_init(&lexer, CN_CSTR("3.14 0.5 1e10 2.5e-3"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_FLOAT_VALUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_FLOAT_VALUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_FLOAT_VALUE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_FLOAT_VALUE);
    }

    // Test string literals
    {
        cn_lexer_init(&lexer, CN_CSTR("\"hello\" \"world\""), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_STRING);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_STRING);
    }

    // Test single-char operators and punctuation
    {
        cn_lexer_init(&lexer, CN_CSTR("( ) { } [ ] ; : , ."), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PARAN_OPEN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PARAN_CLOSE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_CURLY_OPEN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_CURLY_CLOSE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SQR_BRACES_OPEN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SQR_BRACES_CLOSE);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SEMICOLON);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_COLON);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_COMMA);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_DOT);
    }

    // Test arithmetic operators
    {
        cn_lexer_init(&lexer, CN_CSTR("+ - * / %"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PLUS);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_MINUS);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ASTERISK);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SLASH);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PERCENT);
    }

    // Test comparison operators
    {
        cn_lexer_init(&lexer, CN_CSTR("< > <= >= == !="), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_LESS);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_GREATER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_LESS_EQ);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_GREATER_EQ);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EQ);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_NOT_EQ);
    }

    // Test logical and bitwise operators
    {
        cn_lexer_init(&lexer, CN_CSTR("&& || ! & | ^ ~"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_AND);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_OR);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EXCLAMATION);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_AMPERSAND);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_BAR);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_HAT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_TILDE);
    }

    // Test shift operators
    {
        cn_lexer_init(&lexer, CN_CSTR("<< >>"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_LSHIFT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_RSHIFT);
    }

    // Test assignment operators
    {
        cn_lexer_init(&lexer, CN_CSTR("= += -= *= /= %= &= |= ^= <<= >>="), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PLUS_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_MINUS_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_MULTIPLY_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_DIVIDE_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_MODULO_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_BIT_AND_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_BIT_OR_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_BIT_XOR_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_LSHIFT_ASSIGN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_RSHIFT_ASSIGN);
    }

    // Test increment/decrement and other operators
    {
        cn_lexer_init(&lexer, CN_CSTR("++ -- -> ? ..."), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INCREMENT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_DECREMENT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ARROW);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_QUESTION);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ELLIPSIS);
    }

    return 0;
}
