#include "../cnotes.h"

int main(void) {
    Cn_Lexer lexer;

    // Test cn_lexer_peek - look ahead without consuming
    {
        cn_lexer_init(&lexer, CN_STR_LIT("int x = 42;"), (Cn_Lexer_Blacklist){0});

        // Current token is "int"
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);

        // Peek ahead
        CN_ASSERT(cn_lexer_peek(&lexer, 1).type == CN_TOKEN_IDENTIFIER);  // x
        CN_ASSERT(cn_lexer_peek(&lexer, 2).type == CN_TOKEN_ASSIGN);      // =
        CN_ASSERT(cn_lexer_peek(&lexer, 3).type == CN_TOKEN_INTEGER_VALUE); // 42

        // Current should still be "int"
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);
    }

    // Test cn_lexer_expect
    {
        cn_lexer_init(&lexer, CN_STR_LIT("int x"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_expect(&lexer, CN_TOKEN_INT) == true);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_expect(&lexer, CN_TOKEN_IDENTIFIER) == true);
        CN_ASSERT(cn_lexer_expect(&lexer, CN_TOKEN_INT) == false);  // Wrong type
    }

    // Test navigation to EOF
    {
        cn_lexer_init(&lexer, CN_STR_LIT("a b c"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EOF);

        // Multiple next_token calls at EOF should stay at EOF
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EOF);
    }

    // Test empty input
    {
        cn_lexer_init(&lexer, CN_STR_LIT(""), (Cn_Lexer_Blacklist){0});
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EOF);
    }

    // Test whitespace handling
    {
        cn_lexer_init(&lexer, CN_STR_LIT("   int   \n\t  x   "), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_EOF);
    }

    // Test peek beyond EOF returns EOF
    {
        cn_lexer_init(&lexer, CN_STR_LIT("x"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        CN_ASSERT(cn_lexer_peek(&lexer, 1).type == CN_TOKEN_EOF);
        CN_ASSERT(cn_lexer_peek(&lexer, 2).type == CN_TOKEN_EOF);
        CN_ASSERT(cn_lexer_peek(&lexer, 3).type == CN_TOKEN_EOF);
    }

    // Test complex expression tokenization
    {
        cn_lexer_init(&lexer, CN_STR_LIT("a+b*c-d/e%f"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);  // a
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PLUS);        // +
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);  // b
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_ASTERISK);    // *
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);  // c
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_MINUS);       // -
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);  // d
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_SLASH);       // /
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);  // e
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PERCENT);     // %
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);  // f
    }

    // Test function declaration tokenization
    {
        cn_lexer_init(&lexer, CN_STR_LIT("int foo(int x, int y)"), (Cn_Lexer_Blacklist){0});

        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PARAN_OPEN);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_COMMA);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_INT);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_IDENTIFIER);
        cn_lexer_next_token(&lexer);
        CN_ASSERT(cn_lexer_token(&lexer).type == CN_TOKEN_PARAN_CLOSE);
    }

    return 0;
}
