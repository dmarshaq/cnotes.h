#ifndef LEXER_H
#define LEXER_H

#include "core/str.h"
#include <stdbool.h>



typedef enum token_type {
    TOKEN_ZERO,
    TOKEN_UNKNOWN,
    TOKEN_SYMBOL,
    TOKEN_STRING,
    TOKEN_COMMENT,
    TOKEN_PREPROC,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    

    // Literal tokens.
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_PARAN_OPEN,
    TOKEN_PARAN_CLOSE,
    TOKEN_CURLY_OPEN,
    TOKEN_CURLY_CLOSE,
    TOKEN_SQR_BRACES_OPEN,
    TOKEN_SQR_BRACES_CLOSE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_ARROW,
    TOKEN_ASSIGN,
    TOKEN_ASTERISK,
    TOKEN_METANOTE,

} Token_Type;

typedef struct token {
    Token_Type type;
    String str;
    u64 line_num;
} Token;

typedef struct literal_token {
    Token_Type type;
    String literal;
} Literal_Token;

typedef struct lexer {
    u64 cursor;
    u64 bol;            // BOL -> Beginning Of Line
    u64 line_num;
    String content;
    Token token; // Current token, changes whenever lexer_next_token is called.
} Lexer;

void token_print_block(Token *token, bool show_token_type);

void token_print(Token *token);

void token_type_print(Token_Type token_type);

void lexer_init(Lexer *lexer, String content);

void lexer_next_token(Lexer *lexer);

/**
 * Checks if current token matches expected type.
 * If it does returns true, otherwise false.
 */
bool lexer_expect(Lexer *lexer, Token_Type type);

/**
 * Peeks one token ahead and returns it without altering the state of the lexer.
 */
Token lexer_peek(Lexer lexer);

/**
 * Will print code snippet message.
 *      
 *  46 | typedef struct {
 *     |         ^~~~~~
 *     |          
 */
void lexer_print_snippet(Lexer *lexer, u64 index, s64 length);

void lexer_print_snippet_token(Lexer *lexer);

#endif

