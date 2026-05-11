#ifndef CN_H_
#define CN_H_

#ifndef CNDEF
#   define CNDEF
#endif // CNDEF

#ifndef CNOTES_ASSERT
#   include <assert.h>
#   define CN_ASSERT assert
#endif // CN_ASSERT

#ifndef CN_REALLOC
#   include <stdlib.h>
#   define CN_REALLOC realloc
#endif // CN_REALLOC

#ifndef CN_FREE
#   include <stdlib.h>
#   define CN_FREE free
#endif // CN_FREE

#ifdef _WIN32
#    define CN_LINE_END "\r\n"
#else
#    define CN_LINE_END "\n"
#endif // _WIN32

#ifdef CN_ANSI_NO_COLOR
#   define CN_ANSI_BLACK
#   define CN_ANSI_RED
#   define CN_ANSI_GREEN
#   define CN_ANSI_YELLOW
#   define CN_ANSI_BLUE
#   define CN_ANSI_MAGENTA
#   define CN_ANSI_CYAN
#   define CN_ANSI_WHITE
#   define CN_ANSI_RESET
#   define CN_ANSI_BRIGHT_BLACK
#   define CN_ANSI_BRIGHT_RED
#   define CN_ANSI_BRIGHT_GREEN
#   define CN_ANSI_BRIGHT_YELLOW
#   define CN_ANSI_BRIGHT_BLUE
#   define CN_ANSI_BRIGHT_MAGENTA
#   define CN_ANSI_BRIGHT_CYAN
#   define CN_ANSI_BRIGHT_WHITE
#else
#   define CN_ANSI_BLACK          "\x1b[30m"
#   define CN_ANSI_RED            "\x1b[31m"
#   define CN_ANSI_GREEN          "\x1b[32m"
#   define CN_ANSI_YELLOW         "\x1b[33m"
#   define CN_ANSI_BLUE           "\x1b[34m"
#   define CN_ANSI_MAGENTA        "\x1b[35m"
#   define CN_ANSI_CYAN           "\x1b[36m"
#   define CN_ANSI_WHITE          "\x1b[37m"
#   define CN_ANSI_RESET          "\x1b[0m"
#   define CN_ANSI_BRIGHT_BLACK   "\x1b[90m"
#   define CN_ANSI_BRIGHT_RED     "\x1b[91m"
#   define CN_ANSI_BRIGHT_GREEN   "\x1b[92m"
#   define CN_ANSI_BRIGHT_YELLOW  "\x1b[93m"
#   define CN_ANSI_BRIGHT_BLUE    "\x1b[94m"
#   define CN_ANSI_BRIGHT_MAGENTA "\x1b[95m"
#   define CN_ANSI_BRIGHT_CYAN    "\x1b[96m"
#   define CN_ANSI_BRIGHT_WHITE   "\x1b[97m"
#endif // CN_ANSI_NO_COLOR

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#   include <intrin.h>
    // MSVC uses _BitScanReverse64.
    static inline uint32_t cn__builtin_clzll_msvc(uint64_t mask) {
        unsigned long where;
        if (_BitScanReverse64(&where, mask)) {
            return (uint32_t)(63 - where);
        }
        return 64; // Handle 0 case.
    }
#   define CN_COUNT_LEADING_ZEROS(x) (cn__builtin_clzll_msvc(x))
#else
    // GCC / Clang.
#   define CN_COUNT_LEADING_ZEROS(x) ((x) == 0 ? 64 : __builtin_clzll(x))
#endif // _MSC_VER

#define CN_UNUSED(value) (void)(value)
#define CN_TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define CN_UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define CN_ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))
#define CN_ARRAY_GET(array, index) \
    (CN_ASSERT((size_t)index < CN_ARRAY_LENGTH(array)), array[(size_t)index])

// LOG SECTION
typedef enum {
    CN_INFO,
    CN_WARNING,
    CN_ERROR,
    CN_NO_LOGS,
} Cn_Log_Level;

/**
 * Any messages with the level below cn_min_log_level are going to be suppressed. 
 * Idea stolen from nob, just like the most of the library structure. 
 * Thank you nob and stb.
 */
extern Cn_Log_Level cn_min_log_level;

typedef void (Cn_Log_Handler)(Cn_Log_Level level, const char *format, va_list args);

/**
 * Variable that stores current log handler, by default assigned to cn_default_log_handler.
 */
extern Cn_Log_Handler *cn_log_handler;

/**
 * Logs to stderr formatted message prepended with corresponding log level.
 */
CNDEF Cn_Log_Handler cn_default_log_handler;

/**
 * Completely ignores all incoming logs, doesn't output anything to anywhere.
 */
CNDEF Cn_Log_Handler cn_null_log_handler;

/**
 * Interface function to communicate logs to the current log handler.
 * Inside cn_log_handler is called to print log.
 */
CNDEF void cn_log(Cn_Log_Level level, const char *format, ...);

// STRING SECTION
typedef struct {
    int64_t length;
    char *data;
} Cn_String;

// TODO: Write description for following string macros.
#define CN_STR(length, data)            ((Cn_String) { (int64_t)length, (char *)data } )
#define CN_CSTR(cstring)                CN_STR(strlen(cstring), cstring)

#define CN_STR_BUFFER(literal)          CN_STR(sizeof(literal) - 1, (char[]){literal})
#define CN_STR_BUFFER_EMPTY(size)       CN_STR(size, (char[size]){})

// Cast to int is required, because "%.*s" expects maximum length of 32 bit INT_MAX.
// Yes, it will truncate the string if it's length exceeds INT_MAX.
#define CN_UNPACK(str)                  (int)((str).length), (str).data
#define CN_UNPACK_LITERAL(literal)      (int)(sizeof(literal) - 1), (literal)

/**
 * RETURNS: Cn_String that points to the memory of original "str" at index "start" with length up until index "end".
 * NOTE: Character at index "end" is not included in the returned Cn_String, domain for resulting substring is always [ start, end ).
 * IMPORTANT: DOESN'T COPY MEMORY. If "str" memory is freed later, returned string will not point to valid address anymore.
 */
CNDEF Cn_String cn_str_substring(Cn_String str, int64_t start, int64_t end);

/**
 * Compares "str1" and "str2", checks for lengths equality first and then compares symbol by symbol.
 * RETURNS: True if strings are identical.
 */
CNDEF bool cn_str_equals(Cn_String str1, Cn_String str2);

/**
 * Linearly searches for the first occurnse of "search" in "str" from the LEFT, by comparing them through "cn_str_equals()" function.
 * RETURNS: Index of first character of the occurns, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find(Cn_String str, Cn_String search);

/**
 * Linearly searches for the first occurnse of char "symbol" in "str" from the LEFT, by comparing each char in "str".
 * RETURNS: Index of first character of the occurns, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find_char_left(Cn_String str, char symbol);

/**
 * Linearly searches for the first occurnse of char "symbol" in "str" from the RIGHT, by comparing each char in "str".
 * RETURNS: Index of first character of the occurns, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find_char_right(Cn_String str, char symbol);

/**
 * Finds index of the first non whitespace occurns from the left.
 */
CNDEF int64_t cn_str_find_non_whitespace_left(Cn_String str);

/**
 * Finds index of the first non whitespace occurns from the right.
 */
CNDEF int64_t cn_str_find_non_whitespace_right(Cn_String str);

/**
 * Finds index of the first whitespace occurns from the left.
 */
CNDEF int64_t cn_str_find_whitespace_left(Cn_String str);

/**
 * Finds index of the first whitespace occurns from the right.
 */
CNDEF int64_t cn_str_find_whitespace_right(Cn_String str);

/**
 * Shortens the string by excluding specified count of chars from the left.
 */
CNDEF Cn_String cn_str_eat_chars(Cn_String str, int64_t count);

/**
 * Shortens the string from the left by excluding all of the whitespace chars.
 */
CNDEF Cn_String cn_str_eat_spaces(Cn_String str);

/**
 * Shortens the string from the left by excluding the chars until a it hits a whitespace, doesn't exclude a whitespace.
 */
CNDEF Cn_String cn_str_eat_until_space(Cn_String str);

/**
 * RETURNS: String that contains everything until a whitespace, deosn't include a whitespace char.
 */
CNDEF Cn_String cn_str_get_until_space(Cn_String str);

/**
 * RETURNS: True is str contians only symbol chars.
 */
CNDEF bool cn_str_is_symbol(Cn_String str);

/**
 * RETURNS: True is str is an integer.
 */
CNDEF bool cn_str_is_int(Cn_String str);

/**
 * RETURNS: True is str is a float.
 */
CNDEF bool cn_str_is_float(Cn_String str);

/**
 * RETURNS: Integer parsed from the string.
 */
CNDEF int64_t cn_str_parse_int(Cn_String str);

/**
 * RETURNS: Float parsed from the string.
 */
CNDEF float cn_str_parse_float(Cn_String str);

/**
 * Counts the number of specific characters in the string.
 */
CNDEF int64_t cn_str_count_chars(Cn_String str, char c);

/**
 * Copies contents of the src string into buffer.
 * IMPORTANT: Buffer should have enough space to hold src data.
 */
CNDEF void *cn_str_copy_to(Cn_String str, void *buffer);

/**
 * Formats string and outputs it into a buffer memory, buffer should be big enough to hold the string.
 * RETURNS: String that corresponds to the formatted string.
 */
CNDEF Cn_String cn_str_format(Cn_String buffer, char *format, ...);

/**
 * Checks whether the string is empty by checking whether the length of the string is 0.
 */
CNDEF bool cn_str_empty(Cn_String str);


// ARRAY LIST SECTION
typedef struct {
    int64_t capacity;
    int64_t length;
    int64_t item_size;
} Cn_Array_List_Header;

#define cn_array_list_make(type, capacity)\
    (type *)cn__array_list_make(sizeof(type), capacity)

#define cn_array_list_length(ptr_list)\
    cn__array_list_length((void *)*ptr_list)

#define cn_array_list_capacity(ptr_list)\
    cn__array_list_capacity((void *)*ptr_list)

#define cn_array_list_item_size(ptr_list)\
    cn__array_list_item_size((void *)*ptr_list)

#define cn_array_list_append(ptr_list, item)\
    do {\
        cn__array_list_resize_to_fit((void **)(ptr_list), cn_array_list_length(ptr_list) + 1);\
        Cn_Array_List_Header *cn__h_ = (Cn_Array_List_Header *)((uint8_t *)(*ptr_list) - sizeof(Cn_Array_List_Header)); \
        (*ptr_list)[cn__h_->length++] = item;\
    } while(0)

#define cn_array_list_append_multiple(ptr_list, item_arr, count)\
    cn__array_list_append_multiple((void **)ptr_list, (void *)item_arr, count)

#define cn_array_list_pop(ptr_list)\
    cn__array_list_pop((void *)*ptr_list, 1)

#define cn_array_list_pop_multiple(ptr_list, count)\
    cn__array_list_pop((void *)*ptr_list, count)

#define cn_array_list_clear(ptr_list)\
    cn__array_list_clear((void *)*ptr_list)

#define cn_array_list_unordered_remove(ptr_list, index)\
    cn__array_list_unordered_remove((void *)*ptr_list, index)

#define cn_array_list_free(ptr_list)\
    cn__array_list_free((void **)ptr_list)

CNDEF void *cn__array_list_make(int64_t item_size, int64_t initial_capacity);

CNDEF int64_t cn__array_list_length(void *list);

CNDEF int64_t cn__array_list_capacity(void *list);

CNDEF int64_t cn__array_list_item_size(void *list);

CNDEF void cn__array_list_resize_to_fit(void **list, int64_t requiered_length);

CNDEF int64_t cn__array_list_append_multiple(void **list, void *items, int64_t count);

CNDEF void cn__array_list_pop(void *list, int64_t count);

CNDEF void cn__array_list_clear(void *list);

CNDEF void cn__array_list_unordered_remove(void *list, int64_t index);

CNDEF void cn__array_list_free(void **list);

// HASH TABLE SECTION
typedef uint32_t (Cn_Hashfunc)(int64_t, uint8_t *);

typedef struct {
    int64_t capacity;
    int64_t count;
    int64_t item_size;
    Cn_Hashfunc *hash_func; 
    uint8_t *keys;
} Cn_Hash_Table_Header;

typedef enum : uint8_t {
    CN_SLOT_EMPTY      = 0x00,
    CN_SLOT_OCCUPIED   = 0x01,
    CN_SLOT_DEPRICATED = 0x02,
} Cn_Hash_Table_Slot_State;

typedef struct {
    Cn_Hash_Table_Slot_State state;
    int64_t key_size;
    int64_t key_idx;
} Cn_Hash_Table_Slot;

#define cn_hash_table_make(type, capacity)\
    (type *)cn__hash_table_make(sizeof(type), capacity) 

#define cn_hash_table_count(ptr_table)\
    cn__hash_table_count((void *)*ptr_table)

#define cn_hash_table_capacity(ptr_table)\
    cn__hash_table_capacity((void *)*ptr_table)

#define cn_hash_table_item_size(ptr_table)\
    cn__hash_table_item_size((void *)*ptr_table)

#define cn_hash_table_put(ptr_table, item, ...)\
    do {\
        cn__hash_table_resize_to_fit((void **)(ptr_table), cn_hash_table_count(ptr_table) + 1);\
        (*ptr_table)[cn__hash_table_push_key((void **)(ptr_table), __VA_ARGS__)] = item;\
    } while(0)

#define cn_hash_table_get(ptr_table, ...)\
    cn__hash_table_get((void **)(ptr_table), __VA_ARGS__)

#define cn_hash_table_remove(ptr_table, ...)\
    cn__hash_table_remove((void **)(ptr_table), __VA_ARGS__) 

#define cn_hash_table_free(ptr_table)\
    cn__hash_table_free((void **)(ptr_table))

#define cn_hash_table_header(table_ptr)\
    ((Cn_Hash_Table_Header *)((uint8_t *)*table_ptr - sizeof(Cn_Hash_Table_Header)))

CNDEF uint32_t cn_hashf(int64_t key_size, uint8_t *key);

CNDEF void cn_hash_table_print(void **table);

CNDEF void *cn__hash_table_make(int64_t item_size, int64_t initial_capacity);

CNDEF int64_t cn__hash_table_count(void *table);

CNDEF int64_t cn__hash_table_capacity(void *table);

CNDEF int64_t cn__hash_table_item_size(void *table);

CNDEF void cn__hash_table_resize_to_fit(void **table, int64_t requiered_length);

CNDEF int64_t cn__hash_table_push_key(void **table, int64_t key_size, uint8_t *key);

CNDEF void *cn__hash_table_get(void **table, int64_t key_size, uint8_t *key);

CNDEF void cn__hash_table_remove(void **table, int64_t key_size, uint8_t *key);

CNDEF void cn__hash_table_free(void **table);

CNDEF Cn_Hash_Table_Slot *cn__hash_table_get_slot(void **table, int64_t index);

// LEXER SECTION
typedef enum {
    CN_TOKEN_ZERO,
    CN_TOKEN_UNKNOWN,
    CN_TOKEN_SYMBOL,
    CN_TOKEN_STRING,
    CN_TOKEN_COMMENT,
    CN_TOKEN_PREPROC,
    CN_TOKEN_INTEGER,
    CN_TOKEN_FLOAT,
    // Literal tokens
    CN_TOKEN_COLON,
    CN_TOKEN_SEMICOLON,
    CN_TOKEN_PARAN_OPEN,
    CN_TOKEN_PARAN_CLOSE,
    CN_TOKEN_CURLY_OPEN,
    CN_TOKEN_CURLY_CLOSE,
    CN_TOKEN_SQR_BRACES_OPEN,
    CN_TOKEN_SQR_BRACES_CLOSE,
    CN_TOKEN_COMMA,
    CN_TOKEN_DOT,
    CN_TOKEN_ARROW,
    CN_TOKEN_ASSIGN,
    CN_TOKEN_ASTERISK,
    CN_TOKEN_METANOTE,
} Cn_Token_Type;

typedef struct {
    Cn_Token_Type type;
    Cn_String str;
    int64_t line_num;
} Cn_Token;

typedef struct {
    Cn_Token_Type type;
    Cn_String literal;
} Cn_Literal_Token;

typedef struct {
    int64_t cursor;
    int64_t bol;            // BOL -> Beginning Of Line
    int64_t line_num;
    Cn_String content;
    Cn_Token token; // Current token, changes whenever cn_lexer_next_token is called.
} Cn_Lexer;

// TODO: Write comments for following functions.
CNDEF void cn_lexer_init(Cn_Lexer *lexer, Cn_String content);

CNDEF void cn_lexer_next_token(Cn_Lexer *lexer);

/**
 * Checks if current token matches expected type.
 * If it does returns true, otherwise false.
 */
CNDEF bool cn_lexer_expect(Cn_Lexer *lexer, Cn_Token_Type type);

/**
 * Peeks one token ahead and returns it without altering the state of the lexer.
 */
CNDEF Cn_Token cn_lexer_peek(Cn_Lexer lexer);

/**
 * Prints code snippet message.
 *      
 *  46 | typedef struct {
 *     |         ^~~~~~
 *     |          
 */
CNDEF void cn_lexer_print_snippet(Cn_Lexer *lexer, uint64_t index, int64_t length);

CNDEF void cn_lexer_print_snippet_token(Cn_Lexer *lexer);

// AST SECTION

// TODO: REFACTOR THIS MACRO.
#define FLAG_ORDINAL(flag) (__builtin_ctz(flag))


typedef uint32_t Cn_Ast_Idx;

#define CN_AST_NIL_IDX 0


// TODO: Move these into implementation only section of the library.

/**
 * This list of strings keeps tracks of actual string saves in arena_strings.
 * It acts like set, saving needed strings only once and then reusing them as needed.
 * IMPORTANT: Use array_list_length(&string_list) to get the length of the list.
 */
extern Cn_String *cn__string_list;

/**
 * Linearly checks is string already exists, 
 * if so reuses existing string rather then saving new.
 */
CNDEF Cn_String cn__string_list_save(Cn_String str);

/**
 * Saves string to the string list regardless of whether it is a duplicate or not.
 */
CNDEF Cn_String cn__string_list_force_save(Cn_String str);


typedef enum cn_qualifier_flags : uint8_t {
    CN_TYPE_QUALIFIER_CONST           = 0x01,
    CN_TYPE_QUALIFIER_RESTRICT        = 0x02,
    CN_TYPE_QUALIFIER_VOLATILE        = 0x04,
    CN_TYPE_QUALIFIER_ATOMIC          = 0x08,
} Cn_Qualifier_Flags;

static const Cn_String CN_CONST_STR    = CN_STR_BUFFER("const");
static const Cn_String CN_RESTRICT_STR = CN_STR_BUFFER("restrict");
static const Cn_String CN_VOLATILE_STR = CN_STR_BUFFER("volatile");
static const Cn_String CN_ATOMIC_STR   = CN_STR_BUFFER("_Atomic");


typedef enum cn_storage_specifier_flags : uint8_t {
    CN_STORAGE_SPECIFIER_STATIC       = 0x01,
    CN_STORAGE_SPECIFIER_EXTERN       = 0x02,
    CN_STORAGE_SPECIFIER_REGISTER     = 0x04,
    CN_STORAGE_SPECIFIER_AUTO         = 0x08,
    CN_STORAGE_SPECIFIER_TYPEDEF      = 0x10,
} Cn_Storage_Specifier_Flags;

static const Cn_String CN_STATIC_STR   = CN_STR_BUFFER("static");
static const Cn_String CN_EXTERN_STR   = CN_STR_BUFFER("extern");
static const Cn_String CN_REGISTER_STR = CN_STR_BUFFER("register");
static const Cn_String CN_AUTO_STR     = CN_STR_BUFFER("auto");
static const Cn_String CN_TYPEDEF_STR  = CN_STR_BUFFER("typedef");


typedef enum : uint8_t {
    CN_TYPE_SIGN_NONE = 0,
    CN_TYPE_SIGN_SIGNED,
    CN_TYPE_SIGN_UNSIGNED,
} Cn_Type_Sign;

static const Cn_String CN_SIGNED_STR   = CN_STR_BUFFER("signed");
static const Cn_String CN_UNSIGNED_STR = CN_STR_BUFFER("unsigned");

typedef enum : uint8_t {
    CN_TYPE_WIDTH_NONE = 0,
    CN_TYPE_WIDTH_SHORT,
    CN_TYPE_WIDTH_LONG,
    CN_TYPE_WIDTH_LONG_LONG,
} Cn_Type_Width;

static const Cn_String CN_SHORT_STR     = CN_STR_BUFFER("short");
static const Cn_String CN_LONG_STR      = CN_STR_BUFFER("long");

typedef enum : uint8_t {
    // IMPORTANT: These primitive "base" types are mapped to constant array declared below.
    // CN_TYPE_NONE at index 0 is NULL string.
    CN_TYPE_NONE    = 0,
    CN_TYPE_INT     = 1,
    CN_TYPE_CHAR    = 2,
    CN_TYPE_FLOAT   = 3,
    CN_TYPE_DOUBLE  = 4,
    CN_TYPE_BOOL    = 5,
    CN_TYPE_VOID    = 6,

    CN_TYPE_TYPEDEF,
    CN_TYPE_STRUCT,
    CN_TYPE_ENUM,
    CN_TYPE_UNION,
} Cn_Type_Kind;

static const Cn_String CN_TYPE_KINDS[] = {
    {0},
    CN_STR_BUFFER("int"),
    CN_STR_BUFFER("char"),
    CN_STR_BUFFER("float"),
    CN_STR_BUFFER("double"),
    CN_STR_BUFFER("_Bool"),
    CN_STR_BUFFER("void"),
};

static const Cn_String CN_STRUCT_STR        = CN_STR_BUFFER("struct");
static const Cn_String CN_ENUM_STR          = CN_STR_BUFFER("enum");
static const Cn_String CN_UNION_STR         = CN_STR_BUFFER("union");

static const Cn_String CN_IF_STR            = CN_STR_BUFFER("if");
static const Cn_String CN_WHILE_STR         = CN_STR_BUFFER("while");
static const Cn_String CN_DO_STR            = CN_STR_BUFFER("do");
static const Cn_String CN_FOR_STR           = CN_STR_BUFFER("for");
static const Cn_String CN_GOTO_STR          = CN_STR_BUFFER("goto");
static const Cn_String CN_CONTINUE_STR      = CN_STR_BUFFER("continue");
static const Cn_String CN_BREAK_STR         = CN_STR_BUFFER("break");
static const Cn_String CN_RETURN_STR        = CN_STR_BUFFER("return");

/**
 * Grabs next token in the lexer the same way as lexer next token except,
 * it ignores blacklistted tokens, for example comments.
 */
CNDEF void cn_ast_next_token(Cn_Lexer *lexer);

/**
 *  Abstract Syntax Tree generated by cnotes, 
 *  simply is an intrusive tree that lives 
 *  inside the array list composed of ast nodes.
 *
 *  Therefore tree structure is embedded into nodes themselves.
 *
 *  For example tree like this:
 *
 *          A
 *         / \
 *        B   C
 *       /
 *      D
 *
 *  Would be represented as series of the following references:
 *
 *          A
 *         /
 *        B -- C
 *       /
 *      D
 *
 *  Where:
 *       A.child -> B
 *       A.next  -> NIL
 *
 *       B.child -> D
 *       B.next  -> C
 *
 *       C.child -> NIL
 *       C.next  -> NIL
 *
 *       D.next  -> NIL
 *       D.next  -> NIL
 *
 *  Ast nodes in this implementation will adhere to the same logic.
 *  So that next always means a sibling of a current node.
 *  Or a child of current's node parent.
 */
typedef struct cn_ast_node Cn_Ast_Node;

typedef enum cn_ast_node_kind : uint8_t {
    CN_AST_NODE_UNKNOWN                        = 0,
    CN_AST_NODE_TRANSLATION_UNIT,
    CN_AST_NODE_EXTERNAL_DECLARATION,
    CN_AST_NODE_DECLARATION,
    CN_AST_NODE_FUNCTION_DEFINITION,
    CN_AST_NODE_LABELED_STATEMENT,
    CN_AST_NODE_COMPOUND_STATEMENT,
    CN_AST_NODE_EXPRESSION_STATEMENT,
    CN_AST_NODE_SELECTION_STATEMENT,
    CN_AST_NODE_ITERATION_STATEMENT,
    CN_AST_NODE_JUMP_STATEMENT,
    CN_AST_NODE_INIT_DECLARATOR_LIST,
    CN_AST_NODE_INIT_DECLARATOR,
    CN_AST_NODE_INITIALIZER,
    CN_AST_NODE_DECLARATOR,
    CN_AST_NODE_POINTER,
    CN_AST_NODE_DIRECT_DECLARATOR,
    CN_AST_NODE_IDENTIFIER,
    CN_AST_NODE_ASSIGNMENT_EXPRESSION,
    CN_AST_NODE_PARAMETER_TYPE_LIST,
    CN_AST_NODE_DECLARATION_SPECIFIERS,
    CN_AST_NODE_ASM_DEFINITION,
    CN_AST_NODE_TYPE_SPECIFIER,
} Cn_Ast_Node_Kind;

typedef struct {
    bool extension;
} Cn_Ast_Node_External_Declaration;

typedef struct {
    Cn_Storage_Specifier_Flags storage_specifiers;
    Cn_Qualifier_Flags         qualifiers;
} Cn_Ast_Node_Declaration_Specifiers;

typedef struct {
    Cn_Type_Kind  kind;
    Cn_Type_Width width;
    Cn_Type_Sign  sign;
} Cn_Ast_Node_Type_Specifier;

typedef struct {
    Cn_Qualifier_Flags qualifiers;
} Cn_Ast_Node_Pointer;

typedef enum {
    CN_DD_IDENTIFIER,
    CN_DD_GROUPED,
    CN_DD_ARRAY,
    CN_DD_FUNCTION,
} Cn_Direct_Declarator_Kind;

typedef struct {
    Cn_Direct_Declarator_Kind kind;
} Cn_Ast_Node_Direct_Declarator;

typedef struct {
    Cn_String name;
} Cn_Ast_Node_Identifier;


typedef struct cn_ast_node {
    Cn_Ast_Node_Kind kind;

    Cn_Ast_Idx  last_idx;
    Cn_Ast_Idx  child_idx;
    Cn_Ast_Idx  next_idx;

    union {
        Cn_Ast_Node_External_Declaration    external_declaration;
        Cn_Ast_Node_Declaration_Specifiers  declaration_specifiers;
        Cn_Ast_Node_Type_Specifier          type_specifier;
        Cn_Ast_Node_Pointer                 pointer;
        Cn_Ast_Node_Direct_Declarator       direct_declarator;
        Cn_Ast_Node_Identifier              identifier;
    };
} Cn_Ast_Node;

#ifndef CN_AST_NODE_LIST_INITIAL_CAP
#   define CN_AST_NODE_LIST_INITIAL_CAP 32
#endif // CN_AST_NODE_LIST_INITIAL_CAP

/**
 * Stores all nodes in growing array list.
 * IMPORTANT: Access elements by indicies, so there are no unsafe situations occuring.
 * First element is considered NIL element, it is reserved to identify illegal references.
 */
extern Cn_Ast_Node *cn_ast_node_list;

#ifndef CN_AST_TAG_DEFINITION_TABLE_INITIAL_CAP
#   define CN_AST_TAG_DEFINITION_TABLE_INITIAL_CAP 16
#endif // CN_AST_TAG_DEFINITION_TABLE_INITIAL_CAP

/**
 * Tag table uses tag names of struct, enum, union to store index of such definition.
 * IMPORTANT: If tag is declared but not defined, and it is not in the table yet it will be stored without definition, meaning index will be equal to CN_AST_NIL_IDX.
 */
extern Cn_Ast_Idx *cn_ast_tag_definition_table;

#ifndef CN_AST_TYPEDEF_DEFINITION_TABLE_INITIAL_CAP
#   define CN_AST_TYPEDEF_DEFINITION_TABLE_INITIAL_CAP 16
#endif // CN_AST_TYPEDEF_DEFINITION_TABLE_INITIAL_CAP

/**
 * Typedef table acts similar to the tag table except. 
 * If typedef is declared but not yet defined it will error rather than storing typedef. 
 * Indicies stored should always be valid.
 * Only typedefs are included here.
 */
extern Cn_Ast_Idx *cn_ast_typedef_definition_table;

/**
 * Inits ast functionality, called once before parsing begins.
 */
CNDEF int cn_ast_init();

/**
 * Simple macro to get ast node based on its idx.
 * It is abstracted away same way as append, if for some reason 
 * underlying implementaion will change in the future.
 */
#define cn_ast_node_get(idx)    (cn_ast_node_list + (idx))

/**
 * It is a simple macro that unwrawps into for loop, where
 * it is each child of a node parent.
 */
#define cn_ast_node_foreach_child(it, node) for (Cn_Ast_Node *it = cn_ast_node_get((node)->child_idx); it != cn_ast_node_get(CN_AST_NIL_IDX); it = cn_ast_node_get(it->next_idx))

/**
 * Appends specified node to the cn_ast_node_list. It doesn't add or change nodes parent.
 * RETURNS: Cn_Ast_Idx of where the node was inserted.
 */
CNDEF Cn_Ast_Idx cn_ast_node_list_append(Cn_Ast_Node node);

/**
 * Adds child to the specified parent node. 
 * If its not the only child the newer one is appended as last child.
 * NOTE: All added children must already have their idx's.
 */
CNDEF void cn_ast_node_add_child(Cn_Ast_Node *node, Cn_Ast_Idx child_idx);

/**
 * Recursivly prints ast tree to stdout.
 * IMPORTANT: Use starting depth as 0.
 */
CNDEF void cn_ast_print(Cn_Ast_Node *node, int depth);

/**
 * Parses code starting of with lexer current token as translation unit.
 *
 *  translation_unit
 *          : external_declaration* EOF
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_translation_unit(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as external declaration.
 *
 *  external_declaration
 *          : '__extension__'? (
 *              ';'                         // Stray ';'
 *              | asm_definition            // GCC
 *              | function_definition
 *              | declaration
 *              )
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_external_declaration(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as function definition 
 * or declaration. Since we can't really know for certain which one until 
 * parsed far enough ast structure to determine. Ultimetly existance of function 
 * body at the end of the declaration signifies that function is definition is being parsed.
 * 
 * NOTE: function_definition doesn't include classic declaration list that are K&R styled function definitons.
 *
 * The deciding logic of whether declaration is a function definiton, 
 * is solely determined by first trying to parse init declarator list 
 * and if it is only a single declarator without init, simplified down to just one declarator ast node 
 * and expecting function body right after.
 *
 *  declaration
 *          : (
 *          	declaration_specifiers init_declarator_list? ';'
 *          	| static_assert_declaration
 *              )
 *          ;
 *
 *  function_definition
 *          : declaration_specifiers declarator compound_statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_function_definition_or_declaration(Cn_Lexer *lexer);

/**
 * Determines whether tokens indicate start of declaration, 
 * used to distinguish between declaration and statement before parsing them.
 * 
 * NOTE: Doesn't alter state of lexer.
 * 
 * RETURNS: true if tokens indicate start of declaration, false otherwise.
 */
CNDEF bool cn_ast_starts_declaration(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as statement.
 *
 * IMPORTANT: CN_AST_NODE_STATEMENT doesn't exist by itself instead there are nodes variation, 
 * when calling this function to parse statement it returns on of the following types of nodes.
 *
 *  statement
 *          : compound_statement
 *          | selection_statement
 *          | iteration_statement
 *          | jump_statement
 *          | labeled_statement
 *          | expression_statement
 *          | TODO: asm_statement
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as compound statement.
 *
 *  compound_statement
 *          : '{' (statement | declaration)* '}'
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_compound_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as selection_statement
 *
 *  selection_statement
 *          : TODO: Selection statement
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_selection_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as iteration_statement
 *
 *  iteration_statement
 *          : TODO: Iteration statement
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_iteration_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as jump_statement
 *
 *  jump_statement
 *          : TODO: Jump statement
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_jump_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as labeled statement.
 *
 *  labeled_statement
 *          : TODO: Labeled statement
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_labeled_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as expression statement.
 *
 *  expression_statement
 *          : expression? ';'
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as full expression.
 *
 *  full_expression
 *          : expression (',' expression)*
 *          ;
 *  
 */
CNDEF Cn_Ast_Idx cn_ast_parse_full_expression(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as expression.
 * Utilizing following precedence table as guide to properly structure AST nodes in the right order.
 *
 *  C OPERATOR PRECEDENCE TABLE highest (14) to lowest (1)
 *  ================================================
 *  
 *  14                              Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    ++  --        Postfix increment/decrement
 *    ()            Function call
 *    []            Array subscript
 *    .             Member access
 *    ->            Member access through pointer
 *    (type){...}   Compound literal
 *  
 *  13                              Associativity: Right-to-left
 *  ---------------------------------------------------------------
 *    ++  --        Prefix increment/decrement
 *    +   -         Unary plus/minus
 *    !   ~         Logical NOT, bitwise NOT
 *    (type)        Cast
 *    *             Dereference
 *    &             Address-of
 *    sizeof        Size-of
 *  
 *  12                              Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    *   /   %     Multiplication, division, remainder
 *  
 *  11                              Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    +   -         Addition, subtraction
 *  
 *  10                              Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    <<  >>        Bitwise left/right shift
 *  
 *  9                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    <   <=        Less than, less-or-equal
 *    >   >=        Greater than, greater-or-equal
 *  
 *  8                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    ==  !=        Equal, not equal
 *  
 *  7                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    &             Bitwise AND
 *  
 *  6                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    ^             Bitwise XOR
 *  
 *  5                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    |             Bitwise OR
 *  
 *  4                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    &&            Logical AND
 *  
 *  3                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    ||            Logical OR
 *  
 *  2                               Associativity: Right-to-left
 *  ---------------------------------------------------------------
 *    ? :           Ternary conditional
 *  
 *  1                               Associativity: Right-to-left
 *  ---------------------------------------------------------------
 *    =             Simple assignment
 *    +=  -=        Compound assignment (additive)
 *    *=  /=  %=    Compound assignment (multiplicative)
 *    <<=  >>=      Compound assignment (shift)
 *    &=  ^=  |=    Compound assignment (bitwise)
 *
 * Following are grammar rules to group expression into certain types, but in reality all parsing is based on precedence table.
 * Grammars are just what is general logic, and what specific tokens parser might expect in certain expressions.
 *
 *  expression
 *          : binary_expression
 *          | unary_expression
 *          | ternary_expression
 *          | assignment_expression
 *          | postfix_expression
 *          | leaf_expression
 *          ;
 *
 *  binary_expression
 *          : expression binary_operator expression
 *          ;
 *
 *  binary_operator
 *          : '*'
 *          | '/'
 *          | '%'
 *          | '+'
 *          | '-'
 *          | '<<'
 *          | '>>'
 *          | '<'
 *          | '<='
 *          | '>'
 *          | '>='
 *          | '=='
 *          | '!='
 *          | '&'
 *          | '^'
 *          | '|'
 *          | '&&'
 *          | '||'
 *          ;
 *  
 *  unary_expression
 *          : unary_operator expression
 *          | '(' TODO: type ')' expression
 *          | 'sizeof' expression
 *          ;
 *
 *  unary_operator
 *          : '++'
 *          | '--'
 *          | '+'
 *          | '-'
 *          | '!'
 *          | '~'
 *          | '*'
 *          | '&'
 *          ;
 * 
 *  ternary_expression
 *          : expression '?' expression ':' expression
 *          ;
 * 
 * NOTE: Assignment expression also checks if expression the left is a modifiable lvalue.
 *
 *  assignment_expression
 *          : expression assignment_operator expression
 *          ;
 *
 *  assignment_operator
 *          : '=' 
 *          | '*=' 
 *          | '/=' 
 *          | '%=' 
 *          | '+=' 
 *          | '-=' 
 *          | '<<=' 
 *          | '>>=' 
 *          | '&=' 
 *          | '^=' 
 *          | '|='
 *          ;
 *
 * NOTE: Some of postfix expressions have right side arguments, example: a[b], f(args), s.m, p->m.
 * 
 *  postfix_expression
 *          : expression '[' expression ']'
 *          | expression '(' argument_list ')'
 *          | expression '.' identifier
 *          | expression '->' identifier
 *          | expression '++'
 *          | expression '--'
 *          | '(' TODO: type ')' '{' TODO: ... '}'
 *          ;
 *
 *  primary_expression
 *          : identifier
 *          | integer
 *          | float
 *          ;
 *
 * IMPORTANT: Naturally this function constructs left leaning tree on binary expressions, but based of precedence it will 
 * invoke function that constructs right leaning tree if precedence strictly increasing.
 *
 * Example: a + b * c (Strictly increasing precedence -> right leaning tree)
 *  
 *    +
 *   /
 *  a   
 *    
 *    +
 *   / \
 *  a   *
 *     /
 *    b
 *
 *    +
 *   / \
 *  a   *
 *     / \
 *    b   c
 *
 * Example: a + b - c (Same precedence -> left leaning tree)
 *
 *    +
 *   /
 *  a   
 *
 *      -
 *     / 
 *    +
 *   / \
 *  a   b
 *
 *      -
 *     / \
 *    +   c
 *   / \
 *  a   b
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_increasing_precedence(Cn_Lexer *lexer, Cn_Ast_Idx left_idx, int min_precedence);

CNDEF Cn_Ast_Idx cn_ast_parse_expression(Cn_Lexer *lexer, int min_precedence);

CNDEF Cn_Ast_Idx cn_ast_parse_expression_leaf(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as init declarator list.
 *
 *  init_declarator_list
 *          : init_declarator (',' init_declarator)*
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator_list(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as init declarator.
 *
 *  init_declarator
 *          : declarator ('=' initializer)?
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as initializer.
 *
 *  initializer
 *          : assignment_expression
 *          | TODO: Initializer list.
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as declarator.
 *
 *  declarator
 *          : pointer? direct_declarator
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as pointer.
 *
 *  pointer
 *          : '*' qualifier* pointer?
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_pointer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as direct declarator.
 *
 *  direct_declarator
 *          : identifier
 *          | '(' declarator ')'
 *          | direct_declarator '[' assignment_expression? ']'
 *          | direct_declarator '(' parameter_type_list? ')'
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_direct_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as identifier.
 *
 * IMPORTANT: Identifier is simply any CN_TOKEN_SYMBOL.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_identifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as declaration specifiers.
 *
 *  declaration_specifiers
 *          : (
 *              storage_specifier
 *              | qualifier
 *              | type_specifier
 *              )+
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_parse_declaration_specifiers(Cn_Lexer *lexer);

/**
 * Tries to parse code starting of with lexer current token as storage specifier.
 *
 * OUTPUTS: It into supplied output destination.
 *
 * RETURNS: 0 on success, 1 if token wasn't accepted, 2 on error.
 *
 *  storage_specifier
 *          : 'static'
 *          | 'extern'
 *          | 'register'
 *          | 'auto'
 *          | 'typedef'
 *          ;
 */
CNDEF int cn_ast_try_parse_storage_specifier(Cn_Lexer *lexer, Cn_Storage_Specifier_Flags *output);

/**
 * Tries to parse code starting of with lexer current token as qualifier.
 *
 * OUTPUTS: It into supplied output destination.
 *
 * RETURNS: 0 on success, 1 if token wasn't accepted, 2 on error.
 *
 *  qualifier
 *          : 'const'
 *          | 'restrict'
 *          | 'volatile'
 *          | '_Atomic'
 *          ;
 */
CNDEF int cn_ast_try_parse_qualifier(Cn_Lexer *lexer, Cn_Qualifier_Flags *output);

/**
 * Tries to parse code starting of with lexer current token as type specifier.
 *
 * OUTPUTS: It into supplied output ast node.
 * Passing NIL as output_idx is an error.
 * 
 * IMPORTANT: This function can return 1, which is NOT an error.
 * It will occure in a case where current token is not a valid token type specifier can accept.
 * And that no errors occured. It is done so type specifier can check everything it needs 
 * and if it has no work there to be done, 
 * signifying outer function that current token is definitly not a type specifier.
 *
 * RETURNS: 0 on success, 1 if token wasn't accepted, 2 on error.
 *
 *  type_specifier
 *          : 'void'
 *          | 'char'
 *          | 'short'
 *          | 'int'
 *          | 'long'
 *          | 'float'
 *          | 'double'
 *          | 'signed'
 *          | 'unsigned'
 *          | '_Bool'
 *          | '_Complex'
 *          | atomic_type_specifier
 *          | struct_or_union_specifier
 *          | enum_specifier
 *          | typedef
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_try_parse_type_specifier(Cn_Lexer *lexer, Cn_Ast_Idx output_idx);

/**
 * IMPORTANT: Attributes of C23 in theory have similar ideas to what this library is trying to implement.
 * And this library could in theory use [[]] syntax for attributes. The problem is that this syntax might not scale really well.
 * And regardless of whether this library adapts it, various code analysis tools will still view attributes as just transparent sequence of tokens.
 * Yelling at anything more complex, even if it bases of recognized C syntax.
 * This happens because this library is planned to be more complex than plain attachment of attributes.
 *
 * For example potential generic implementation using this library that adapts C attribute syntax might look like this:
 *
 * typedef int T;
 * [[cn::generic(T: int, char, void *)]] T my_function(T my_var) {
 *      T var = my_var;
 *      return var;
 * }
 *
 * Other example:
 *
 * typedef void * T;
 * T my_function([[cn::generic]] T my_var) {
 *      T var = my_var;
 *      return var;
 * }
 *
 * The problem is that this might not be intuitive, and contains garbage syntax to satisfy code analysis tools.
 * So it doesn't really make sense to try to look like "C standard like" extension, if other tools won't understand it anyway.
 * Alternative is just modern, cleaner syntax, for example:
 * 
 * @Generic(T: int, char, void *) T my_function(T my_var) {
 *      T var = my_var;
 *      return var;
 * }
 *
 * Or even this, depending on generic implementation:
 *
 * T my_function(@Generic T my_var) {
 *      T var = my_var;
 *      return var;
 * }
 *
 * Such intuitive syntax also means that power over [[...]] is fully left to the C compilers,
 * so none of the future changes are likely to cause collisions or problems with the library.
 * 
 *
 * Parses code starting of with lexer current token as attribute specifier sequence.
 *
 * attribute_specifier_sequence
 *          : attribute_specifier+
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_attribute_specifier_sequence(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as asm defintion.
 *
 *  asm_definition
 *          : simple_asm_expression
 *          | asm '(' top_level_asm_argument ')'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_asm_definition(Cn_Lexer *lexer);


// PRE-PROCESSING SECTION
typedef enum {
    CN_INSERT,
    CN_REMOVE,
} Cn_Modification_Kind;

typedef struct {
    Cn_String str;
} Cn_Modification_Insert;

typedef struct {
    int64_t length;
} Cn_Modification_Remove;

typedef int64_t Cn_Modification_Idx;

#define CN_MUTATION_IDX_NIL 0

typedef struct {
    int64_t              offset;
    Cn_Modification_Kind kind;

    union {
        Cn_Modification_Insert insert;
        Cn_Modification_Remove remove;
    };
} Cn_Modification;



#ifndef CN_TU_MODIFICATION_LIST_INITIAL_CAP
#   define CN_TU_MODIFICATION_LIST_INITIAL_CAP 32
#endif // CN_TU_MODIFICATION_LIST_INITIAL_CAP

typedef struct {
    int64_t          version;
    char *           path;
    Cn_String        content;
    Cn_Modification *modification_list;
} Cn_Translation_Unit;


/**
 * RETURNS: Cn_Translation_Unit struct that represent classic C translation unit in which all lexing, parsing, infering, modification occurs.
 * IMPORTANT: Takes in path to the .i file, that is intermidiate pre-processed representation of a translation unit.
 * Simply it is a .c file that has all macros expanded and ready to be sent to the compiler.
 * Supplying .c files with macros that are not expanded will result in error, ensure to pass .c file through compiler's pre-processor first.
 * In gcc it would be executing the following:
 *      
 *      $ gcc -E -o file.i file.c
 *
 * This will generate .i file, path to which can be safely specified here.
 */
CNDEF Cn_Translation_Unit cn_tu_make(char *intermidiate_path);

/**
 * This function free's all memory used by the translation unit, including closing previously opened file.
 */
CNDEF void cn_tu_free(Cn_Translation_Unit *tu);

/**
 * Processes the translation unit from top to bottom.
 * Translation Unit passes through Infer -> Size stages.
 * Meaning AST is built, type and symbol table is constructed, 
 * and size's of the types are calculated too.
 * Through the processing the messages are enqueued, 
 * and by the end of the process they are triggered,
 * if cn_message_handler is not NULL.
 * RETURNS: 0 if processing is successful. -1 if error occured.
 */
CNDEF int cn_tu_process(Cn_Translation_Unit *tu); 


typedef struct {
    Cn_Translation_Unit *tu;

    Cn_String file_path;
    int64_t   line_number;

    // TODO: Attach declaration info later.
} Cn_Message;

typedef void (Cn_Message_Handler)(Cn_Message *message);

extern Cn_Message_Handler *cn_message_handler;


#endif // CN_H_
       

#ifdef CN_IMPLEMENTATION

// LOG SECTION
Cn_Log_Level cn_min_log_level = CN_INFO;

Cn_Log_Handler *cn_log_handler = &cn_default_log_handler;

CNDEF void cn_default_log_handler(Cn_Log_Level level, const char *format, va_list args) {
    
    if (level < cn_min_log_level)
        return;

    switch (level) {
        case CN_INFO:
            fprintf(stderr, CN_ANSI_BRIGHT_BLUE"[INFO]" CN_ANSI_RESET " ");
            break;
        case CN_WARNING:
            fprintf(stderr, CN_ANSI_YELLOW"[WARNING]" CN_ANSI_RESET " ");
            break;
        case CN_ERROR:
            fprintf(stderr, CN_ANSI_BRIGHT_RED"[ERROR]" CN_ANSI_RESET " ");
            break;
        case CN_NO_LOGS: 
            return;
        default:
            CN_UNREACHABLE("Cn_Log_Level");
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}

CNDEF void cn_null_log_handler(Cn_Log_Level level, const char *format, va_list args) {
    CN_UNUSED(level);
    CN_UNUSED(format);
    CN_UNUSED(args);
}

CNDEF void cn_log(Cn_Log_Level level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    cn_log_handler(level, format, args);
    va_end(args);
}


// STRING SECTION
CNDEF Cn_String cn_str_substring(Cn_String str, int64_t start, int64_t end) {
    return CN_STR(end - start, str.data + start);
}

CNDEF bool cn_str_equals(Cn_String str1, Cn_String str2) {
    if (str1.length != str2.length) {
        return false;
    }
    return !memcmp(str1.data, str2.data, str1.length);
}

CNDEF int64_t cn_str_find(Cn_String str, Cn_String search) {
    Cn_String substr;
    for (int64_t i = 0; i + search.length <= str.length; i++) {
        if (str.data[i] == search.data[0]) {
            substr = cn_str_substring(str, i, i + search.length);
            if (cn_str_equals(substr, search)) {
                return i;
            }
        }
    }
    
    return -1;
}

CNDEF int64_t cn_str_find_char_left(Cn_String str, char symbol) {
    for (int64_t i = 0; i < str.length; i++) {
        if (str.data[i] == symbol) {
            return i;
        }
    }

    return -1;
}

CNDEF int64_t cn_str_find_char_right(Cn_String str, char symbol) {
    for (int64_t i = str.length - 1; i > -1; i--) {
        if (str.data[i] == symbol) {
            return i;
        }
    }

    return -1;
}

CNDEF int64_t cn_str_find_non_whitespace_left(Cn_String str) {
    for (int64_t i = 0; i < str.length; i++) {
        if (!isspace(str.data[i])) {
            return i;
        }
    }

    return -1;
}

CNDEF int64_t cn_str_find_non_whitespace_right(Cn_String str) {
    for (int64_t i = str.length - 1; i > -1; i--) {
        if (!isspace(str.data[i])) {
            return i;
        }
    }

    return -1;
}

CNDEF int64_t cn_str_find_whitespace_left(Cn_String str) {
    for (int64_t i = 0; i < str.length; i++) {
        if (isspace(str.data[i])) {
            return i;
        }
    }

    return -1;
}

CNDEF int64_t cn_str_find_whitespace_right(Cn_String str) {
    for (int64_t i = str.length - 1; i > -1; i--) {
        if (isspace(str.data[i])) {
            return i;
        }
    }

    return -1;
}

CNDEF Cn_String cn_str_eat_chars(Cn_String str, int64_t count) {
    return cn_str_substring(str, count, str.length);
}

CNDEF Cn_String cn_str_eat_spaces(Cn_String str) {
    int64_t i = 0;

    while(i < str.length && isspace(str.data[i])) {
        i++;
    }

    return CN_STR(str.length - i, str.data + i);
}

CNDEF Cn_String cn_str_eat_until_space(Cn_String str) {
    int64_t i = 0;

    while(i < str.length && !isspace(str.data[i])) {
        i++;
    }

    return CN_STR(str.length - i, str.data + i);
}

CNDEF Cn_String cn_str_get_until_space(Cn_String str) {
    int64_t i = 0;

    while(i < str.length && !isspace(str.data[i])) {
        i++;
    }

    return CN_STR(i, str.data);
}

CNDEF bool cn_str_is_symbol(Cn_String str) {
    if (str.length <= 0) {
        return false;
    }

    int64_t i = 0;

    if (!isalpha(str.data[i]) && str.data[i] != '_') {
        return false;
    }
    
    while(i < str.length) {
        if (!isalnum(str.data[i]) && str.data[i] != '_') {
            return false;
        }
        i++;
    }

    return true;
}

CNDEF bool cn_str_is_int(Cn_String str) {
    if (str.length <= 0) {
        return false;
    }

    int64_t i = 0;

    if (str.data[i] == '+' || str.data[i] == '-' ) {
        if (str.length < 2)
            return false;
        i++;
    }
    
    while(i < str.length) {
        if (!isdigit(str.data[i])) {
            return false;
        }
        i++;
    }

    return true;
}

CNDEF bool cn_str_is_float(Cn_String str) {
    if (str.length <= 0) {
        return false;
    }

    bool only_one_dot = false;
    int64_t i = 0;

    if (str.data[i] == '+' || str.data[i] == '-') {
        // Edge case: "-"
        if (str.length < 2)
            return false;

        // Edge case: "-."
        if (str.data[i + 1] == '.' && str.length < 3) {
            return false;
        }
        i++;
    }
    
    while(i < str.length) {
        if (!isdigit(str.data[i])) {
            if (!only_one_dot && str.data[i] == '.') {
                only_one_dot = true;
            } else {
                return false;
            }
        }
        i++;
    }

    return true;
}

CNDEF int64_t cn_str_parse_int(Cn_String str) {
    int8_t sign = 1;
    int64_t i = 0;

    if (str.data[0] == '-') {
        sign = -1;
        i = 1;
    } else if (str.data[0] == '+') {
        i = 1;
    }

    int result = 0;
    for (; i < str.length; i++) {
        result = result * 10 + (str.data[i] - '0');
    }

    return sign * result;
}

CNDEF float cn_str_parse_float(Cn_String str) {
    int8_t sign = 1;
    int64_t i = 0;

    if (str.data[0] == '-') {
        sign = -1;
        i = 1;
    } else if (str.data[0] == '+') {
        i = 1;
    }

    double result = 0.0;

    // Parse integer part
    while (i < str.length && str.data[i] != '.') {
        result = result * 10.0 + (str.data[i] - '0');
        i++;
    }

    // Parse fractional part
    if (i < str.length && str.data[i] == '.') {
        i++;  // skip dot
        double frac = 0.0;
        double base = 0.1;
        while (i < str.length) {
            frac += (str.data[i] - '0') * base;
            base *= 0.1;
            i++;
        }
        result += frac;
    }

    return sign * result;
}

CNDEF int64_t cn_str_count_chars(Cn_String str, char c) {
    int64_t count = 0;

    for (int64_t i = 0; i < str.length; i++) {
        if (str.data[i] == c) {
            count++;
        }
    }

    return count;
}

CNDEF void *cn_str_copy_to(Cn_String str, void *buffer) {
    return memcpy(buffer, str.data, str.length);
}

CNDEF Cn_String cn_str_format(Cn_String buffer, char *format, ...) {
    va_list args;
    va_start(args, format);

    // vsnprintf returns the number of chars that *would* have been written.
    int written = vsnprintf(buffer.data, buffer.length, format, args);

    va_end(args);

    if (written < 0) {
        return CN_STR(0, NULL);
    }

    buffer.length = written;

    return buffer;
}

CNDEF bool cn_str_empty(Cn_String str) {
    return str.length == 0;
}

// ARRAY LIST SECTION
CNDEF void *cn__array_list_make(int64_t item_size, int64_t capacity) {
    CN_ASSERT(item_size > 0);
    CN_ASSERT(capacity > 0);

    Cn_Array_List_Header *header = (Cn_Array_List_Header *)CN_REALLOC(NULL, sizeof(Cn_Array_List_Header) + item_size * capacity);

    if (header == NULL) {
        cn_log(CN_ERROR, "Couldn't allocate more memory of size: %lld bytes, for the array list.", sizeof(Cn_Array_List_Header) + item_size * capacity);
        return NULL;
    }

    // Setting all array list header members.
    header->capacity   = capacity;
    header->item_size  = item_size;
    header->length     = 0;

    // IMPORTANT: Because header is of type "Cn_Array_List_Header *", compiler will automatically translate "header + 1" to "(void*)(header) + sizeof(Cn_Array_List_Header)".
    return header + 1;
}

CNDEF int64_t cn__array_list_length(void *list) {
    return ((Cn_Array_List_Header *)((uint8_t *)list - sizeof(Cn_Array_List_Header)))->length;
}

CNDEF int64_t cn__array_list_capacity(void *list) {
    return ((Cn_Array_List_Header *)((uint8_t *)list - sizeof(Cn_Array_List_Header)))->capacity;
}

CNDEF int64_t cn__array_list_item_size(void *list) {
    return ((Cn_Array_List_Header *)((uint8_t *)list - sizeof(Cn_Array_List_Header)))->item_size;
}

CNDEF void cn__array_list_free(void **list) {
    CN_FREE((uint8_t *)*list - sizeof(Cn_Array_List_Header));
    *list = NULL;
}

CNDEF void cn__array_list_resize_to_fit(void **list, int64_t requiered_length) {
    Cn_Array_List_Header *header = (Cn_Array_List_Header *)((uint8_t *)*list - sizeof(Cn_Array_List_Header));

    if (requiered_length > header->capacity) {
        // IMPORTANT: This is an original equation for calculating capacity_multiplier.
        //
        // int64_t capacity_multiplier = (int64_t)pow(2.0, (double)((int64_t)(log2((double)requiered_length / (double)header->capacity)) + 1));
        //
        // Following code is a more cheap alternative that uses bit shifting to achieve the same result.
        int64_t ratio = requiered_length / header->capacity;
        int highest_bit_pos = 63 - CN_COUNT_LEADING_ZEROS(ratio);
        CN_ASSERT(highest_bit_pos >= 0);
        int64_t capacity_multiplier = (int64_t)(1 << (highest_bit_pos + 1));

        header = (Cn_Array_List_Header *)CN_REALLOC(header, sizeof(Cn_Array_List_Header) + header->capacity * capacity_multiplier * header->item_size);
        
        if (header == NULL) {
            cn_log(CN_ERROR, "Couldn't reallocate more memory to fit new size of: %lld bytes, for the array list.", sizeof(Cn_Array_List_Header) + header->capacity * capacity_multiplier * header->item_size);
            return;
        }

        *list = header + 1; // IMPORTANT: Updating pointer to the list data after resizing.
        header->capacity *= capacity_multiplier; // IMPORTANT: Updating capacity after, because if resize fails capacity should not change.
    }
}

CNDEF int64_t cn__array_list_append_multiple(void **list, void *items, int64_t count) {
    cn__array_list_resize_to_fit(list, cn__array_list_length(*list) + count);

    Cn_Array_List_Header *header = (Cn_Array_List_Header *)((uint8_t *)*list - sizeof(Cn_Array_List_Header));

    // Copying items to the list.
    memcpy((uint8_t *)*list + header->length * header->item_size, items, header->item_size * count);
    header->length += count;

    return header->length - count;
}

CNDEF void cn__array_list_pop(void *list, int64_t count) {
    ((Cn_Array_List_Header *)((uint8_t *)list - sizeof(Cn_Array_List_Header)))->length -= count;
}

CNDEF void cn__array_list_clear(void *list) {
    ((Cn_Array_List_Header *)((uint8_t *)list - sizeof(Cn_Array_List_Header)))->length = 0;
}

CNDEF void cn__array_list_unordered_remove(void *list, int64_t index) {
    Cn_Array_List_Header *header = (Cn_Array_List_Header *)((uint8_t *)list - sizeof(Cn_Array_List_Header));
    // Copy last element in the list to the index position, and then pop element at the end.
    memcpy((uint8_t *)list + index * header->item_size, (uint8_t *)list + (header->length - 1) * header->item_size, header->item_size);
    cn__array_list_pop(list, 1);
}

// HASH TABLE SECTION

/**
 * Internal function.
 * RETURNS: Index of the corresponding key by calculating hash.
 * IMPORTANT: Doesn't perfom any slot checks.
 */
CNDEF uint32_t cn__hash_table_hash_index_of(void **table, int64_t key_size, uint8_t *key) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);
    return header->hash_func(key_size, key) % header->capacity;
}

/**
 * Internal function.
 * Sets all occupied slots to depricated.
 */
CNDEF void cn__hash_table_depricate_slots(void **table) {
    int64_t cap = cn_hash_table_capacity(table);
    Cn_Hash_Table_Slot *slot = NULL;
    for (int64_t i = 0; i < cap; i++) {
        slot = cn__hash_table_get_slot(table, i);
        if (slot->state == CN_SLOT_OCCUPIED) {
            slot->state = CN_SLOT_DEPRICATED;
        }
    }
}

/**
 * Internal function.
 */
CNDEF void cn__hash_table_print_slot(void *item, int64_t item_size, uint8_t *keys, Cn_Hash_Table_Slot *slot) {
    

    // Print the state and key_size as hex.

    if (slot->state == CN_SLOT_OCCUPIED) {
        // Print the item in hex based on item_size.
        printf("Item: 0x");
        for (int64_t i = 0; i < item_size; i++) {
            printf("%02x", *((uint8_t *)item + i));  // Print each byte of the item.
        }

        printf(" | State: 0x%02x | Key Size: %ld | Key -> %.*s\n", slot->state, slot->key_size, (int)slot->key_size, (char *)keys + slot->key_idx);
    } else {
        printf("EMPTY   %*s | State: 0x%02x\n", (int)item_size * 2, "",  slot->state);
    }
}

/**
 * Internal function.
 * RECURSION: Recursivly readdresses slots if they are depricated.
 * RETURNS: True, if it succesfully readdressed a slot.
 */
CNDEF bool cn__hash_table_readdress(void **table, int64_t index) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);
    Cn_Hash_Table_Slot *target_slot = cn__hash_table_get_slot(table, index);

    int64_t new_index = cn__hash_table_hash_index_of(table, target_slot->key_size, header->keys + target_slot->key_idx);
    target_slot->state = CN_SLOT_EMPTY;

    Cn_Hash_Table_Slot *slot = NULL;
    for (int64_t i = 0; i < header->capacity; i++) {
        slot = cn__hash_table_get_slot(table, (new_index + i) % header->capacity);
        if (slot->state == CN_SLOT_EMPTY) {
            // Copy data to a new slot.
            slot->state = CN_SLOT_OCCUPIED;

            slot->key_size = target_slot->key_size;
            slot->key_idx = target_slot->key_idx;

            memmove((uint8_t *)*table + ((new_index + i) % header->capacity) * header->item_size, (uint8_t *)*table + index * header->item_size, header->item_size);

            return true;
        }
        else if (slot->state == CN_SLOT_DEPRICATED && cn__hash_table_readdress(table, (new_index + i) % header->capacity)) {

            // IMPORTANT: There is an additional is CN_SLOT_OCCUPIED check, because cn__hash_table_readdress can possbily readdress slot to the same index as it was before, therefore additional check is needed.
            if (slot->state != CN_SLOT_OCCUPIED) {
                // Copy data to a new slot.
                slot->state = CN_SLOT_OCCUPIED;

                slot->key_size = target_slot->key_size;
                slot->key_idx = target_slot->key_idx;

                memmove((uint8_t *)*table + ((new_index + i) % header->capacity) * header->item_size, (uint8_t *)*table + index * header->item_size, header->item_size);

                return true;
            }
        }
    }

    cn_log(CN_ERROR, "Couldn't find new free hash slot when readdressing.\n");
    return false;

}

CNDEF uint32_t cn_hashf(int64_t key_size, uint8_t *key) {
    CN_ASSERT(key != NULL);         // Do we need to assert this?
    CN_ASSERT(key_size > 0);

    if (key_size < 2) {
        return *(uint8_t *)(key);
    }
    
    uint32_t hash = 0;
    uint8_t *hash_ptr = (uint8_t *)&(hash);
    hash_ptr[0] = *((uint8_t *)(key) + 0);
    hash_ptr[1] = *((uint8_t *)(key) + 1);
    hash_ptr[2] = *((uint8_t *)(key) + key_size - 1);
    hash_ptr[3] = *((uint8_t *)(key) + key_size - 2);

    return hash;
}

CNDEF void cn_hash_table_print(void **table) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    printf("\n--------\tHash Table\t--------\n");
    for (int64_t i = 0; i < header->capacity; i++) {
        cn__hash_table_print_slot((uint8_t *)*table + i * header->item_size, header->item_size, header->keys, cn__hash_table_get_slot(table, i));
    }
}

CNDEF Cn_Hash_Table_Slot *cn__hash_table_get_slot(void **table, int64_t index) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);
    return (Cn_Hash_Table_Slot *)((uint8_t *)*table + header->capacity * header->item_size + index * sizeof(Cn_Hash_Table_Slot));
}

CNDEF void *cn__hash_table_make(int64_t item_size, int64_t capacity) {
    CN_ASSERT(item_size > 0);
    CN_ASSERT(capacity > 0);

    Cn_Hash_Table_Header *header = (Cn_Hash_Table_Header *)CN_REALLOC(NULL, sizeof(Cn_Hash_Table_Header) + (sizeof(Cn_Hash_Table_Slot) + item_size) * capacity);

    if (header == NULL) {
        cn_log(CN_ERROR, "Couldn't allocate more memory of size: %ld bytes, for the hash table.", sizeof(Cn_Hash_Table_Header) + (sizeof(Cn_Hash_Table_Slot) + item_size) * capacity);
        return NULL;
    }
    
    // IMPORTANT: Using separately allocated array list for keys, because keys are variable size, 
    // and we do NOT want to reallocate whole hash table every time a long key is added. 
    // But it is also useful to allow hash table to store keys, 
    // so user doesn't have to worry about saving keys manually. 
    // And it also allows table to check whether certain key value pair is legal or not.
    uint8_t *keys = cn_array_list_make(uint8_t, capacity * 8);    

    if (keys == NULL) {
        cn_log(CN_ERROR, "Couldn't allocate more memory of size: %ld bytes, for the keys needed for the hash table.\n", sizeof(uint8_t) * capacity * 8);
        CN_FREE(header);
        return NULL;
    }

    // Setting all hash table header members.
    header->capacity   = capacity;
    header->item_size  = item_size;
    header->count      = 0;
    header->hash_func  = cn_hashf;
    header->keys       = keys;

    // IMPORTANT: Because header is of type "Cn_Hash_Table_Header *", compiler will automatically translate "header + 1" to "(void*)(header) + sizeof(Cn_Hash_Table_Header)".
    void *data = header + 1;
    
    // Set all slots to CN_SLOT_EMPTY.
    for (int64_t i = 0; i < header->capacity; i++) {
        cn__hash_table_get_slot(&data, i)->state = CN_SLOT_EMPTY;
    }
    
    return data;
}

CNDEF int64_t cn__hash_table_count(void *table) {
    return ((Cn_Hash_Table_Header *)((uint8_t *)table - sizeof(Cn_Hash_Table_Header)))->count;
}

CNDEF int64_t cn__hash_table_capacity(void *table) {
    return ((Cn_Hash_Table_Header *)((uint8_t *)table - sizeof(Cn_Hash_Table_Header)))->capacity;
}

CNDEF int64_t cn__hash_table_item_size(void *table) {
    return ((Cn_Hash_Table_Header *)((uint8_t *)table - sizeof(Cn_Hash_Table_Header)))->item_size;
}

CNDEF void cn__hash_table_free(void **table) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);
    cn_array_list_free(&(header->keys));
    CN_FREE(header);
    *table = NULL;
}

CNDEF void cn__hash_table_resize_to_fit(void **table, int64_t requiered_length) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    if (requiered_length > header->capacity) {
        // Before resizing, depricate slots.
        cn__hash_table_depricate_slots(table);

        // IMPORTANT: To understand where this calculation comes from check cn__array_list_resize_to_fit implementation.
        // It uses same calculation that simplifies pow and log of base 2 caluclation to just using bit manipulation.
        int64_t ratio = requiered_length / header->capacity;
        int highest_bit_pos = 63 - CN_COUNT_LEADING_ZEROS(ratio);
        CN_ASSERT(highest_bit_pos >= 0);
        int64_t capacity_multiplier = (int64_t)(1 << (highest_bit_pos + 1));

        header = (Cn_Hash_Table_Header *)CN_REALLOC(header, sizeof(Cn_Hash_Table_Header) + header->capacity * capacity_multiplier * (header->item_size + sizeof(Cn_Hash_Table_Slot)));

        if (header == NULL) {
            cn_log(CN_ERROR, "Couldn't reallocate more memory to fit new size of: %ld bytes, for the hash table.", sizeof(Cn_Hash_Table_Header) + header->capacity * capacity_multiplier * (header->item_size + sizeof(Cn_Hash_Table_Slot)));
            return;
        }

        *table = header + 1; // IMPORTANT: Updating pointer to the table data after resizing.
        header->capacity *= capacity_multiplier; // IMPORTANT: Updating capacity after, because if resize fails capacity should not change.

        /**
         * After resize, all data is copied and buffer is expanded to the right.
         * But due to the nature of hash table structure the slot area of the buffer would not be properly shift in the resulting array.
         * For example diagrams (NOT TO SCALE):
         *
         *      BEFORE:
         *                  
         *                  |-----------------------------Buffer-Capacity-4-------------------------------|
         *                  |                                                                             |
         *                  |------Array-of-Data-------|-----------------Array-of-Slots-------------------|
         *      Indicies:   | 0     1     2     3      | 0           1           2           3            |
         *      Data:       | [1111][1111][1111][____] | [10][0x2342][01][0x2344][11][0x2348][__][______] |
         *                  ^
         *                  |
         *                  *table
         *
         *
         *      AFTER RESIZE (BAD):
         *                  
         *                  |-------------------------------------------------------------------Buffer-Capacity-8-----------------------------------------------------------------------|
         *                  |                                                                                                                                                           |
         *                  |------Array-of-Data-------|-----------------Array-of-Slots-------------------|--------------------------|--------------------------------------------------|
         *      Indicies:   | 0     1     2     3      | 0           1           2           3            |                          |                                                  |
         *      Data:       | [1111][1111][1111][____] | [10][0x2342][01][0x2344][11][0x2348][__][______] | [____][____][____][____] | [__][______][__][______][__][______][__][______] |
         *                  ^
         *                  |
         *                  *table
         *
         *
         *      WHAT IS EXPECTED / NEEDED (GOOD):
         *                  
         *                  |----------------------------------------------------------------Buffer-Capacity-8--------------------------------------------------------------------|
         *                  |                                                                                                                                                     |
         *                  |------------------Array-of-Data-------------------|------------------------------------------Array-of-Slots------------------------------------------|
         *      Indicies:   | 0     1     2     3     4     5     6     7      | 0           1           2           3           4           5           6           7            |
         *      Data:       | [1111][1111][1111][____][____][____][____][____] | [10][0x2342][01][0x2344][11][0x2348][__][______][__][______][__][______][__][______][__][______] |
         *                  ^
         *                  |
         *                  *table
         *
         * To achieve this good layout, it is needed to shift array of slots to the right.
         * And it must be done with memmove cause, destination and source may overlap.
         */

        memmove((uint8_t *)*table + header->capacity * header->item_size, (uint8_t *)*table + (header->capacity / capacity_multiplier) * header->item_size, (header->capacity / capacity_multiplier) * sizeof(Cn_Hash_Table_Slot));

        // Set new slots to CN_SLOT_EMPTY.
        for (int64_t i = header->capacity / capacity_multiplier; i < header->capacity; i++) {
            cn__hash_table_get_slot(table, i)->state = CN_SLOT_EMPTY;
        }

        // Readdress slots after resizing.
        for (int64_t i = 0; i < header->capacity; i++) {
            if (cn__hash_table_get_slot(table, i)->state == CN_SLOT_DEPRICATED) {
                cn__hash_table_readdress(table, i);
            }
        }
    }
}

CNDEF int64_t cn__hash_table_push_key(void **table, int64_t key_size, uint8_t *key) {
    int64_t index = cn__hash_table_hash_index_of(table, key_size, key);
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    Cn_Hash_Table_Slot *slot = NULL;
    for (int64_t i = 0; i < header->capacity; i++) {
        slot = cn__hash_table_get_slot(table, (index + i) % header->capacity);
        if (slot->state == CN_SLOT_EMPTY) {
            // Write all data to hash table by corresponding index.
            slot->state = CN_SLOT_OCCUPIED;

            slot->key_size = key_size;
            
            /**
             * Is this still an issue?
             *
             * IMPORTANT: "cn_array_list_append_multiple" is not inlined because it might change the "header->keys" value when resizing array list.
             * So it is neccessary to call this macro like function in a separate line, otherwise undefined behaviour will occure.
             */
            int64_t key_index = cn_array_list_append_multiple(&(header->keys), key, key_size);
            slot->key_idx = key_index;

            header->count++;
           
            return (index + i) % header->capacity;

        } else if (slot->key_size == key_size && !memcmp(header->keys + slot->key_idx, key, key_size)) {
            // Key already exists, return index of that slot.
            return (index + i) % header->capacity;
        }
    }

    cn_log(CN_ERROR, "Couldn't find free hash table slot for the new key.\n");
    return -1;
}

CNDEF void *cn__hash_table_get(void **table, int64_t key_size, uint8_t *key) {
    int64_t index = cn__hash_table_hash_index_of(table, key_size, key);
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    Cn_Hash_Table_Slot *slot = NULL;
    for (int64_t i = 0; i < header->capacity; i++) {
        slot = cn__hash_table_get_slot(table, (index + i) % header->capacity);

        if (slot->state == CN_SLOT_EMPTY)
            return NULL;
        
        if (slot->key_size == key_size && !memcmp(header->keys + slot->key_idx, key, key_size)) {
            // Right key is found, return.
            return (uint8_t *)*table + ((index + i) % header->capacity) * header->item_size;
        }
    }

    return NULL;
}

CNDEF void cn__hash_table_remove(void **table, int64_t key_size, uint8_t *key) {
    int64_t index = cn__hash_table_hash_index_of(table, key_size, key);
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    Cn_Hash_Table_Slot *slot = NULL;
    for (int64_t i = 0; i < header->capacity; i++) {
        slot = cn__hash_table_get_slot(table, (index + i) % header->capacity);

        if (slot->state == CN_SLOT_EMPTY)
            return;

        if (slot->key_size == key_size && !memcmp(header->keys + slot->key_idx, key, key_size)) {
            // Right key is found.
            // LEAK: INCOMPLETE: Doesn't delete key.
            header->count--;
            slot->state = CN_SLOT_EMPTY;
            return;
        }
    }
}


// LEXER SECTION
const Cn_Literal_Token CN_LITERAL_TOKENS[] = {
    { CN_TOKEN_COLON,              CN_STR_BUFFER(":") },
    { CN_TOKEN_SEMICOLON,          CN_STR_BUFFER(";") },
    { CN_TOKEN_PARAN_OPEN,         CN_STR_BUFFER("(") },
    { CN_TOKEN_PARAN_CLOSE,        CN_STR_BUFFER(")") },
    { CN_TOKEN_CURLY_OPEN,         CN_STR_BUFFER("{") },
    { CN_TOKEN_CURLY_CLOSE,        CN_STR_BUFFER("}") },
    { CN_TOKEN_SQR_BRACES_OPEN,    CN_STR_BUFFER("[") },
    { CN_TOKEN_SQR_BRACES_CLOSE,   CN_STR_BUFFER("]") },
    { CN_TOKEN_COMMA,              CN_STR_BUFFER(",") },
    { CN_TOKEN_DOT,                CN_STR_BUFFER(".") },
    { CN_TOKEN_ARROW,              CN_STR_BUFFER("->") },
    { CN_TOKEN_ASSIGN,             CN_STR_BUFFER("=") },
    { CN_TOKEN_ASTERISK,           CN_STR_BUFFER("*") },
};

CNDEF bool cn__valid_symbol_start(char c) {
    return isalpha(c) || c == '_';
}

CNDEF bool cn__valid_symbol(char c) {
    return isalnum(c) || c == '_';
}

CNDEF bool cn__valid_number_start(char c) {
    return isdigit(c) || c == '.';
}

CNDEF bool cn__valid_number(char c) {
    return isalnum(c) || c == '.';
}

/**
 * RETURNS: True if newline character was eaten.
 */
CNDEF bool cn__lexer_eat_char(Cn_Lexer *l) {
    l->cursor++;
    if (l->content.data[l->cursor - 1] == '\n') {
        l->bol = l->cursor;
        l->line_num++;
        return true;
    }

    return false;
}

/**
 * RETURNS: True if stopped right after after newline character was eaten.
 */
CNDEF void cn__lexer_consume_spaces(Cn_Lexer *l) {

    while (l->cursor < l->content.length && isspace(l->content.data[l->cursor])) {
        cn__lexer_eat_char(l);
    }

}

/**
 * RETURNS: True if reached end of file.
 */
CNDEF bool cn__lexer_eof(Cn_Lexer *l) {
    return l->cursor >= l->content.length;
}


/**
 * Gets and consumes, appends to the current token all of the digits [0-9], stops when non-digit char is reached.
 * RETURNS: Count of added digits.
 */
CNDEF int cn__lexer_get_digits(Cn_Lexer *l) {
    int count = 0;
    while (!cn__lexer_eof(l) && isdigit(l->content.data[l->cursor])) {
        l->token.str.length++;
        l->cursor++;
        count++;
    }
    return count;
}

/**
 * Gets and consumes, appends to the current token all of the hex digits [0-F] stops when non-hex digit char is reached, not case sensative.
 * RETURNS: Count of added digits.
 */
CNDEF int cn__lexer_get_hex_digits(Cn_Lexer *l) {
    int count = 0;
    while (!cn__lexer_eof(l) && isxdigit(l->content.data[l->cursor])) {
        l->token.str.length++;
        l->cursor++;
        count++;
    }
    return count;
}

/**
 * Gets and consumes, appends to the current token any integer suffix [l, L, u, U].
 */
CNDEF void cn__lexer_get_integer_suffix(Cn_Lexer *l) {
    while (!cn__lexer_eof(l) && (l->content.data[l->cursor] == 'l' || l->content.data[l->cursor] == 'L' || l->content.data[l->cursor] == 'u' || l->content.data[l->cursor] == 'U')) {
        l->token.str.length++;
        l->cursor++;
    }
}

/**
 * Gets and consumes, appends to the current token any float suffix [f, F, l, L].
 */
CNDEF void cn__lexer_get_float_suffix(Cn_Lexer *l) {
    while (!cn__lexer_eof(l) && (l->content.data[l->cursor] == 'f' || l->content.data[l->cursor] == 'F' || l->content.data[l->cursor] == 'l' || l->content.data[l->cursor] == 'L')) {
        l->token.str.length++;
        l->cursor++;
    }
}

CNDEF void cn__lexer_backtrack_token(Cn_Lexer *l) {
    l->cursor -= l->token.str.length;
    l->token.str.length = 0;
}

CNDEF void cn_lexer_init(Cn_Lexer *lexer, Cn_String content) {
    *lexer = (Cn_Lexer) {0};

    lexer->line_num = 1;
    lexer->content  = content;
}

CNDEF void cn_lexer_next_token(Cn_Lexer *l) {
    cn__lexer_consume_spaces(l);

    l->token = (Cn_Token) {
        .type = CN_TOKEN_UNKNOWN,
        .str = CN_STR(0, l->content.data + l->cursor), 
        .line_num = l->line_num,
    };

    if (l->cursor >= l->content.length) {
        l->token.type = CN_TOKEN_ZERO;
        return;
    }

    if (cn__valid_number_start(l->content.data[l->cursor])) {
        int digit_count = 0;
        bool is_float = false;
        
        digit_count += cn__lexer_get_digits(l);
        if (cn__lexer_eof(l)) return;

        if (l->content.data[l->cursor] == '.') {
            is_float = true;
            l->token.str.length++;
            l->cursor++;
        }

        digit_count += cn__lexer_get_digits(l);
        if (cn__lexer_eof(l)) return;
        
        if (l->content.data[l->cursor] == 'e' || l->content.data[l->cursor] == 'E') {
            is_float = true;
            l->token.str.length++;
            l->cursor++;

            if (cn__lexer_eof(l)) return;
            
            if (l->content.data[l->cursor] == '+' || l->content.data[l->cursor] == '-') {
                l->token.str.length++;
                l->cursor++;
            }
        }

        digit_count += cn__lexer_get_digits(l);
        if (cn__lexer_eof(l)) return;

        if (is_float) {
            if (digit_count == 0) {
                cn__lexer_backtrack_token(l);
                goto number_lexer_fail;
            }
            cn__lexer_get_float_suffix(l);
            l->token.type = CN_TOKEN_FLOAT;
            return;
        }

        if (l->content.data[l->cursor - 1] == '0' && digit_count == 1 && (l->content.data[l->cursor] == 'x' || l->content.data[l->cursor] == 'X')) {
            l->token.str.length++;
            l->cursor++;
            if (cn__lexer_get_hex_digits(l) == 0) {
                return;
            }
        }

        cn__lexer_get_integer_suffix(l);
        l->token.type = CN_TOKEN_INTEGER;
        return;
    }
number_lexer_fail:

    for (uint64_t i = 0; i < CN_ARRAY_LENGTH(CN_LITERAL_TOKENS); i++) {
        if (l->content.length - l->cursor > CN_LITERAL_TOKENS[i].literal.length && cn_str_equals(CN_LITERAL_TOKENS[i].literal, CN_STR(CN_LITERAL_TOKENS[i].literal.length, l->content.data + l->cursor))) {
            l->token.type = CN_LITERAL_TOKENS[i].type;
            l->token.str = CN_STR(CN_LITERAL_TOKENS[i].literal.length, l->content.data + l->cursor);
            l->cursor += l->token.str.length;
            return;
        }
    }


    if (l->content.data[l->cursor] == '#') {
        l->token.type = CN_TOKEN_PREPROC;
        l->token.str.length++;
        l->cursor++;


        while (l->cursor < l->content.length) {

            if (cn__lexer_eat_char(l)) { 
                // If we got '\n'.
                int64_t index_of_last_char = cn_str_find_non_whitespace_right(l->token.str);
                if (l->token.str.data[index_of_last_char] != '\\') { 
                    // If not multiline line macro.
                    return;
                } 
            }
            l->token.str.length++;
        }

        return;
    }


    if (l->content.data[l->cursor] == '"') {
        l->token.type = CN_TOKEN_STRING;
        l->token.str.length++;
        l->cursor++;

        while (l->cursor < l->content.length) {
            if (l->content.data[l->cursor] == '"' && l->content.data[l->cursor - 1] != '\\') {
                // If a valid string end.
                break;
            }
            l->token.str.length++;
            l->cursor++;
        }

        l->token.str.length++;
        l->cursor++;

        return;
    }


    if (l->content.data[l->cursor] == '/') {
        l->token.str.length++;
        l->cursor++;
        
        // One line comment.
        if (l->cursor < l->content.length && l->content.data[l->cursor] == '/') {
            l->token.type = CN_TOKEN_COMMENT;
            l->token.str.length++;
            l->cursor++;

            while (l->cursor < l->content.length) {

                if (cn__lexer_eat_char(l)) {
                    break;
                }
                l->token.str.length++;
            }

            return;
        }

        // Multiline comment.
        if (l->cursor < l->content.length && l->content.data[l->cursor] == '*') {
            l->token.type = CN_TOKEN_COMMENT;
            l->token.str.length++;
            l->cursor++;

            while (l->cursor < l->content.length && (l->content.data[l->cursor - 1] != '*' || l->content.data[l->cursor] != '/')) {

                cn__lexer_eat_char(l);
                l->token.str.length++;
            }


            l->token.str.length++;
            l->cursor++;

            return;
        }
        

        return;
    }


    if (l->content.data[l->cursor] == '@') {
        l->token.type = CN_TOKEN_METANOTE;
        l->token.str.length++;
        l->cursor++;

        while (l->cursor < l->content.length && cn__valid_symbol(l->content.data[l->cursor])) {
            l->token.str.length++;
            l->cursor++;
        }
        
        return;
    }

    if (cn__valid_symbol_start(l->content.data[l->cursor])) {
        l->token.type = CN_TOKEN_SYMBOL;
        while (l->cursor < l->content.length && cn__valid_symbol(l->content.data[l->cursor])) {
            l->token.str.length++;
            l->cursor++;
        }
        
        return;
    }
    
    l->token.str.length++;
    l->cursor++;
    return;
}

CNDEF bool cn_lexer_expect(Cn_Lexer *lexer, Cn_Token_Type type) {
    return lexer->token.type == type;
}

CNDEF Cn_Token cn_lexer_peek(Cn_Lexer lexer) {
    cn_lexer_next_token(&lexer);
    return lexer.token;
}

CNDEF void cn_lexer_print_snippet_token(Cn_Lexer *lexer) {
    cn_lexer_print_snippet(lexer, lexer->token.str.data - lexer->content.data, lexer->token.str.length);
}

CNDEF void cn_lexer_print_snippet(Cn_Lexer *lexer, uint64_t index, int64_t length) {
    Cn_String line = cn_str_eat_chars(lexer->content, lexer->bol);
    int64_t end = cn_str_find_char_left(line, '\n');
    if (end == -1) {
        end = lexer->content.length - lexer->bol;
    }
    line = cn_str_substring(line, 0, end);
    uint64_t underline_offset = index - lexer->bol;

    fprintf(stderr, "\n%4lu | %.*s", lexer->line_num, CN_UNPACK(cn_str_substring(line, 0, underline_offset)));
    fprintf(stderr, "\033[31m%.*s\033[0m", CN_UNPACK(cn_str_substring(line, underline_offset, underline_offset + length)));
    fprintf(stderr, "%.*s\n", CN_UNPACK(cn_str_substring(line, underline_offset + length, line.length)));
    fprintf(stderr, "     | %*s\033[31m^", (int)underline_offset, "");

    for(int64_t i = 1; i < length; i++)
        fputc('~', stderr);

    fputs("\033[0m\n\n", stderr);
}


// AST SECTION
const Cn_Token_Type CN_TOKEN_BLACKLIST[] = { CN_TOKEN_COMMENT, CN_TOKEN_PREPROC };

CNDEF void cn_ast_next_token(Cn_Lexer *lexer) {
next_token:
    cn_lexer_next_token(lexer);

    for (uint64_t i = 0; i < CN_ARRAY_LENGTH(CN_TOKEN_BLACKLIST); i++) {
        if (lexer->token.type == CN_TOKEN_BLACKLIST[i]) {
            goto next_token; // Skips token if current is in the blacklist.
        }
    }
}

Cn_Ast_Node *cn_ast_node_list = NULL;
Cn_Ast_Idx *cn_ast_tag_definition_table = NULL;
Cn_Ast_Idx *cn_ast_typedef_definition_table = NULL;

CNDEF int cn_ast_init() {
    cn_ast_node_list = cn_array_list_make(Cn_Ast_Node, CN_AST_NODE_LIST_INITIAL_CAP);
    cn_ast_tag_definition_table = cn_hash_table_make(Cn_Ast_Idx, CN_AST_TAG_DEFINITION_TABLE_INITIAL_CAP);
    cn_ast_typedef_definition_table = cn_hash_table_make(Cn_Ast_Idx, CN_AST_TYPEDEF_DEFINITION_TABLE_INITIAL_CAP);

    // Inserting first element as NIL.
    Cn_Ast_Node nil = {0};
    cn_array_list_append(&cn_ast_node_list, nil);
    if (cn_ast_node_list == NULL)
        return -1;

    return 0;
}

CNDEF Cn_Ast_Idx cn_ast_node_list_append(Cn_Ast_Node node) {
    cn_array_list_append(&cn_ast_node_list, node);
    return cn_array_list_length(&cn_ast_node_list) - 1;
}

CNDEF void cn_ast_node_add_child(Cn_Ast_Node *node, Cn_Ast_Idx child_idx) {
    if (node->child_idx == CN_AST_NIL_IDX) {
        node->child_idx = child_idx;
        node->last_idx = child_idx;
    } else {
        cn_ast_node_get(node->last_idx)->next_idx = child_idx;
        node->last_idx                            = child_idx;
    }
}

#define CN__ENUM_PRINT_CASE(enum_name)\
    case enum_name: {\
        printf("%s", #enum_name);\
        break;\
        }\

CNDEF void cn__ast_print_kind(Cn_Ast_Node_Kind kind) {
    switch(kind) {
        CN__ENUM_PRINT_CASE(CN_AST_NODE_TRANSLATION_UNIT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_EXTERNAL_DECLARATION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DECLARATION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_FUNCTION_DEFINITION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_LABELED_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_COMPOUND_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_EXPRESSION_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_SELECTION_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ITERATION_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_JUMP_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INIT_DECLARATOR_LIST);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INIT_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INITIALIZER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_POINTER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DIRECT_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_IDENTIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ASSIGNMENT_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_PARAMETER_TYPE_LIST);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DECLARATION_SPECIFIERS);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ASM_DEFINITION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_TYPE_SPECIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_UNKNOWN);
        default: 
            printf("??");
            break;
    }
}

const char *cn_ast_print_prefixes[32] = {0};

const char *CN_AST_PRINT_EMPTY_TAB = "    ";
const char *CN_AST_PRINT_FLAT_TAB  = "│   ";
const char *CN_AST_PRINT_SPLIT_TAB = "├── ";
const char *CN_AST_PRINT_LAST_TAB  = "└── ";

#define CN__AST_PRINT_TABS(depth, prefix) for (int i = 0; i < depth; i++) { printf("%s", cn_ast_print_prefixes[i]); } printf("%s", prefix);

CNDEF void cn_ast_print(Cn_Ast_Node *node, int depth) {
    CN_ASSERT(depth < (int)CN_ARRAY_LENGTH(cn_ast_print_prefixes));

    cn__ast_print_kind(node->kind);
    
    // Printing info about specific nodes.
    switch(node->kind) {
       case CN_AST_NODE_EXTERNAL_DECLARATION:
            if (node->external_declaration.extension) {
                printf(" '__extension__'");
            }
            if (node->child_idx == CN_AST_NIL_IDX) {
                printf(" stray ';'");
            }
            break;
        case CN_AST_NODE_DIRECT_DECLARATOR:
            printf(" kind: ");
            switch(node->direct_declarator.kind) {
                case CN_DD_IDENTIFIER:
                    printf("identifier");
                    break;
                case CN_DD_GROUPED:
                    printf("grouped");
                    break;
                case CN_DD_ARRAY:
                    printf("array");
                    break;
                case CN_DD_FUNCTION:
                    printf("function");
                    break;
            }
            
            break;
        case CN_AST_NODE_IDENTIFIER:
            printf(" '%.*s'", CN_UNPACK(node->identifier.name));
            break;
        case CN_AST_NODE_DECLARATION_SPECIFIERS:
            for (int i = 0; i < 5; i++) {
                switch (node->declaration_specifiers.storage_specifiers & (1 << i)) {
                    case CN_STORAGE_SPECIFIER_STATIC:
                        printf(" '%.*s'", CN_UNPACK(CN_STATIC_STR));
                        break;
                    case CN_STORAGE_SPECIFIER_EXTERN:
                        printf(" '%.*s'", CN_UNPACK(CN_EXTERN_STR));
                        break;
                    case CN_STORAGE_SPECIFIER_REGISTER:
                        printf(" '%.*s'", CN_UNPACK(CN_REGISTER_STR));
                        break;
                    case CN_STORAGE_SPECIFIER_AUTO:
                        printf(" '%.*s'", CN_UNPACK(CN_AUTO_STR));
                        break;
                    case CN_STORAGE_SPECIFIER_TYPEDEF:
                        printf(" '%.*s'", CN_UNPACK(CN_TYPEDEF_STR));
                        break;
                }
            }
            for (int i = 0; i < 4; i++) {
                switch (node->declaration_specifiers.qualifiers & (1 << i)) {
                    case CN_TYPE_QUALIFIER_CONST:
                        printf(" '%.*s'", CN_UNPACK(CN_CONST_STR));
                        break;
                    case CN_TYPE_QUALIFIER_RESTRICT:
                        printf(" '%.*s'", CN_UNPACK(CN_RESTRICT_STR));
                        break;
                    case CN_TYPE_QUALIFIER_VOLATILE:
                        printf(" '%.*s'", CN_UNPACK(CN_VOLATILE_STR));
                        break;
                    case CN_TYPE_QUALIFIER_ATOMIC:
                        printf(" '%.*s'", CN_UNPACK(CN_ATOMIC_STR));
                        break;
                }
            }
            break;
        case CN_AST_NODE_TYPE_SPECIFIER:
            switch(node->type_specifier.sign) {
                case CN_TYPE_SIGN_SIGNED:
                    printf(" '%.*s'", CN_UNPACK(CN_SIGNED_STR));
                    break;
                case CN_TYPE_SIGN_UNSIGNED:
                    printf(" '%.*s'", CN_UNPACK(CN_UNSIGNED_STR));
                    break;
                default:
                    break;
            }

            switch(node->type_specifier.width) {
                case CN_TYPE_WIDTH_SHORT:
                    printf(" '%.*s'", CN_UNPACK(CN_SHORT_STR));
                    break;
                case CN_TYPE_WIDTH_LONG:
                    printf(" '%.*s'", CN_UNPACK(CN_LONG_STR));
                    break;
                case CN_TYPE_WIDTH_LONG_LONG:
                    printf(" '%.*s %.*s'", CN_UNPACK(CN_LONG_STR), CN_UNPACK(CN_LONG_STR));
                    break;
                default:
                    break;
            }

            if (node->type_specifier.kind < CN_ARRAY_LENGTH(CN_TYPE_KINDS)) {
                printf(" '%.*s'", CN_UNPACK(CN_TYPE_KINDS[node->type_specifier.kind]));
            }
            break;
        default:
            break;
    }
    printf("\n");

    cn_ast_print_prefixes[depth] = CN_AST_PRINT_FLAT_TAB;
    cn_ast_node_foreach_child(c, node) {
        // If last child.
        if (c->next_idx == CN_AST_NIL_IDX) {
            cn_ast_print_prefixes[depth] = CN_AST_PRINT_EMPTY_TAB;
            CN__AST_PRINT_TABS(depth, CN_AST_PRINT_LAST_TAB);
        } else {
            CN__AST_PRINT_TABS(depth, CN_AST_PRINT_SPLIT_TAB);
        }

        cn_ast_print(c, depth + 1);
    }

    cn_ast_print_prefixes[depth] = NULL;
}


CNDEF Cn_Ast_Idx cn_ast_parse_translation_unit(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_TRANSLATION_UNIT };

    Cn_Ast_Idx next_idx;

    while (lexer->token.type != CN_TOKEN_ZERO) {
        next_idx = cn_ast_parse_external_declaration(lexer);

        if (next_idx == CN_AST_NIL_IDX)
            goto error;
        
        cn_ast_node_add_child(&node, next_idx);
    }

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_external_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_EXTERNAL_DECLARATION };

    // Optional '__extension__' symbol.
    if (lexer->token.type == CN_TOKEN_SYMBOL) {
        if (cn_str_equals(lexer->token.str, CN_CSTR("__extension__"))) {
            node.external_declaration.extension = true;
            cn_ast_next_token(lexer);
        }
    }
    
    // If stray ';' case.
    if (lexer->token.type == CN_TOKEN_SEMICOLON) {
        cn_ast_next_token(lexer);
        return cn_ast_node_list_append(node);
    }


    Cn_Ast_Idx child_idx;

    // TODO: Parse asm definition here.
    // ...

    // Last possible case function definition or declaration.
    child_idx = cn_ast_parse_function_definition_or_declaration(lexer);

    if (child_idx == CN_AST_NIL_IDX)
        goto error;

    cn_ast_node_add_child(&node, child_idx);

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_function_definition_or_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = {0};

    Cn_Ast_Idx child_idx;

    // Getting declration specifiers.
    child_idx = cn_ast_parse_declaration_specifiers(lexer);
    if (child_idx == CN_AST_NIL_IDX) goto error;
    cn_ast_node_add_child(&node, child_idx);
    
    // Deciding whether node is function definiton or declaration.
    // Simple case, empty declaration.
    if (cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        cn_lexer_next_token(lexer);
        node.kind = CN_AST_NODE_DECLARATION;
        return cn_ast_node_list_append(node);
    }
    
    // Then there MUST be declarator, which will be either part of init_declarator_list or just be by itself.
    Cn_Lexer before_declarator = *lexer;
    child_idx = cn_ast_parse_declarator(lexer);
    if (child_idx == CN_AST_NIL_IDX) goto error;

    // Now finally we can decide if next token is '{' it is definitely a function definition because, it is part of function definition body, anything else could be part of init_declarator_list or ';'.
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        node.kind = CN_AST_NODE_FUNCTION_DEFINITION;

        cn_ast_node_add_child(&node, child_idx);

        child_idx = cn_ast_parse_compound_statement(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);

    } else {
        // IMPORTANT: It is a wasteful way of doing things since we reparse the same declarator twice, there might be better options in the future.
        node.kind = CN_AST_NODE_DECLARATION;

        cn_array_list_pop(&cn_ast_node_list); // pop last parsed ast node (declarator).
        *lexer = before_declarator;

        child_idx = cn_ast_parse_init_declarator_list(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
            cn_log(CN_ERROR, "Expected ';' at the end of declaration.");
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }
        cn_lexer_next_token(lexer);
    }

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF bool cn_ast_starts_declaration(Cn_Lexer *lexer) {
    return 
            cn_str_equals(lexer->token.str, CN_CONST_STR) ||
            cn_str_equals(lexer->token.str, CN_RESTRICT_STR) ||
            cn_str_equals(lexer->token.str, CN_VOLATILE_STR) ||
            cn_str_equals(lexer->token.str, CN_ATOMIC_STR) ||

            cn_str_equals(lexer->token.str, CN_STATIC_STR) ||
            cn_str_equals(lexer->token.str, CN_EXTERN_STR) ||
            cn_str_equals(lexer->token.str, CN_REGISTER_STR) ||
            cn_str_equals(lexer->token.str, CN_AUTO_STR) ||
            cn_str_equals(lexer->token.str, CN_TYPEDEF_STR) ||

            cn_str_equals(lexer->token.str, CN_SIGNED_STR) ||
            cn_str_equals(lexer->token.str, CN_UNSIGNED_STR) ||

            cn_str_equals(lexer->token.str, CN_SHORT_STR) ||
            cn_str_equals(lexer->token.str, CN_LONG_STR) ||

            cn_str_equals(lexer->token.str, CN_TYPE_KINDS[CN_TYPE_INT]) ||
            cn_str_equals(lexer->token.str, CN_TYPE_KINDS[CN_TYPE_CHAR]) ||
            cn_str_equals(lexer->token.str, CN_TYPE_KINDS[CN_TYPE_FLOAT]) ||
            cn_str_equals(lexer->token.str, CN_TYPE_KINDS[CN_TYPE_DOUBLE]) ||
            cn_str_equals(lexer->token.str, CN_TYPE_KINDS[CN_TYPE_BOOL]) ||
            cn_str_equals(lexer->token.str, CN_TYPE_KINDS[CN_TYPE_VOID]) ||

            cn_str_equals(lexer->token.str, CN_STRUCT_STR) ||
            cn_str_equals(lexer->token.str, CN_ENUM_STR) ||
            cn_str_equals(lexer->token.str, CN_UNION_STR) ||

            cn_hash_table_get(&cn_ast_typedef_definition_table, lexer->token.str.length, (uint8_t *)lexer->token.str.data) != NULL
            ;
}

CNDEF Cn_Ast_Idx cn_ast_parse_statement(Cn_Lexer *lexer) {
    if (lexer->token.type == CN_TOKEN_CURLY_OPEN) {
        return cn_ast_parse_compound_statement(lexer);
    }

    if (cn_str_equals(lexer->token.str, CN_IF_STR)) {
        return cn_ast_parse_selection_statement(lexer);
    }

    if (cn_str_equals(lexer->token.str, CN_WHILE_STR) || cn_str_equals(lexer->token.str, CN_DO_STR) || cn_str_equals(lexer->token.str, CN_FOR_STR)) {
        return cn_ast_parse_iteration_statement(lexer);
    }

    if (cn_str_equals(lexer->token.str, CN_GOTO_STR) || cn_str_equals(lexer->token.str, CN_CONTINUE_STR) || cn_str_equals(lexer->token.str, CN_BREAK_STR) || cn_str_equals(lexer->token.str, CN_RETURN_STR)) {
        return cn_ast_parse_jump_statement(lexer);
    }

    if (cn_lexer_peek(*lexer).type == CN_TOKEN_COLON) {
        return cn_ast_parse_labeled_statement(lexer);
    }
    
    return cn_ast_parse_expression_statement(lexer);
}

CNDEF Cn_Ast_Idx cn_ast_parse_compound_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_COMPOUND_STATEMENT };

    Cn_Ast_Idx child_idx;

    if (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        cn_log(CN_ERROR, "Expected '{' in the beginning of compound statement.");
        cn_lexer_print_snippet_token(lexer);
        goto error;
    }
    cn_lexer_next_token(lexer);

    while (true) {
        if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            cn_lexer_next_token(lexer);
            return cn_ast_node_list_append(node);
        }

        if (cn_ast_starts_declaration(lexer)) {
            child_idx = cn_ast_parse_function_definition_or_declaration(lexer);
        } else {
            child_idx = cn_ast_parse_statement(lexer);
        }

        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);
    }

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_selection_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_SELECTION_STATEMENT };

    Cn_Ast_Idx child_idx;

    
    
    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_iteration_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_ITERATION_STATEMENT };

    Cn_Ast_Idx child_idx;

    
    
    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_jump_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_JUMP_STATEMENT };

    Cn_Ast_Idx child_idx;

    
    
    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_labeled_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_LABELED_STATEMENT };

    Cn_Ast_Idx child_idx;

    
    
    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_EXPRESSION_STATEMENT };

    Cn_Ast_Idx child_idx;

    
    
    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}


CNDEF Cn_Ast_Idx cn_ast_parse_declaration_specifiers(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_DECLARATION_SPECIFIERS };
    node.child_idx = cn_ast_node_list_append((Cn_Ast_Node) { .kind = CN_AST_NODE_TYPE_SPECIFIER });



    bool at_least_one = false;
    int ok;
    while (true) {
        ok = cn_ast_try_parse_storage_specifier(lexer, &node.declaration_specifiers.storage_specifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        ok = cn_ast_try_parse_qualifier(lexer, &node.declaration_specifiers.qualifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;
    
        ok = cn_ast_try_parse_type_specifier(lexer, node.child_idx);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        break;
    }

    // If at least one other declaration specifier present continue, if not error.
    if (!at_least_one) {
        cn_log(CN_ERROR, "At least one declaration specifier should be present.");
        cn_lexer_print_snippet_token(lexer);
        goto error;
    }

    // Validation of type specifier.
    Cn_Ast_Node *ts = cn_ast_node_get(node.child_idx);

    // Implicit int case.
    if (ts->type_specifier.kind == CN_TYPE_NONE) {
        ts->type_specifier.kind = CN_TYPE_INT;
    }

    if (ts->type_specifier.width != CN_TYPE_WIDTH_NONE && ts->type_specifier.kind != CN_TYPE_INT) {
        cn_log(CN_ERROR, "Specified type width on non 'int' type.");
        goto error;
    }

    if (ts->type_specifier.sign != CN_TYPE_SIGN_NONE && ts->type_specifier.kind != CN_TYPE_INT && ts->type_specifier.kind != CN_TYPE_CHAR) {
        cn_log(CN_ERROR, "Specified type sign on non 'int' or 'char' type.");
        goto error;
    }

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF int cn_ast_try_parse_storage_specifier(Cn_Lexer *lexer, Cn_Storage_Specifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

#define KEYWORD_CHECK(name)\
    if (cn_str_equals(lexer->token.str, CN_##name##_STR)) {\
        if (*output != 0) goto error_multiple_storage_specifier;\
        cn_lexer_next_token(lexer);\
        *output = CN_STORAGE_SPECIFIER_##name;\
        return 0;\
    }

    KEYWORD_CHECK(STATIC);
    KEYWORD_CHECK(EXTERN);
    KEYWORD_CHECK(REGISTER);
    KEYWORD_CHECK(AUTO);
    KEYWORD_CHECK(TYPEDEF);

#undef KEYWORD_CHECK

    return 1;

error_multiple_storage_specifier:
    cn_log(CN_ERROR, "Multiple storage specifiers are not allowed.");
    cn_lexer_print_snippet_token(lexer);
    goto error;

error:
    *lexer = original_state;
    return 2;
}

CNDEF int cn_ast_try_parse_qualifier(Cn_Lexer *lexer, Cn_Qualifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

#define KEYWORD_CHECK(name)\
    if (cn_str_equals(lexer->token.str, CN_##name##_STR)) {\
        cn_lexer_next_token(lexer);\
        *output = CN_TYPE_QUALIFIER_##name;\
        return 0;\
    }

    KEYWORD_CHECK(CONST);
    KEYWORD_CHECK(RESTRICT);
    KEYWORD_CHECK(VOLATILE);

#undef KEYWORD_CHECK

    if (cn_str_equals(lexer->token.str, CN_ATOMIC_STR)) {
        // Case where it is atomic specifier is not handled here, if such case appears just skip.
        if (cn_lexer_peek(*lexer).type == CN_TOKEN_PARAN_OPEN) return 1;
        cn_lexer_next_token(lexer);
        *output = CN_TYPE_QUALIFIER_ATOMIC;
        return 0;
    }

    return 1;
error:
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Idx cn_ast_try_parse_type_specifier(Cn_Lexer *lexer, Cn_Ast_Idx output_idx) {
    Cn_Lexer original_state = *lexer;

    CN_ASSERT(output_idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_node_get(output_idx);
    
    // Checking if token is primitive.
    for (Cn_Type_Kind kind = 1; kind < CN_ARRAY_LENGTH(CN_TYPE_KINDS); kind++) {
        if (cn_str_equals(lexer->token.str, CN_TYPE_KINDS[kind])) {

            if (node->type_specifier.kind != CN_TYPE_NONE) {
                cn_log(CN_ERROR, "Only single type specifier kind is allowed.");
                cn_lexer_print_snippet_token(lexer);
                goto error;
            }
            
            cn_lexer_next_token(lexer);
            node->type_specifier.kind = kind;
            return 0;
        }
    }

    // INCOMPLETE:
    // Checking if token is typedef.
    
    // INCOMPLETE:
    // Checking if token(s) is struct or union.

    // INCOMPLETE:
    // Checking if token(s) is enum specifier.

    // Checking if token is type sign.
    if (cn_str_equals(lexer->token.str, CN_SIGNED_STR)) {
        if (node->type_specifier.sign != CN_TYPE_SIGN_NONE) {
            cn_log(CN_ERROR, "Duplicate type sign '%.*s' is not allowed in type specifier.", CN_UNPACK(CN_SIGNED_STR));
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.sign = CN_TYPE_SIGN_SIGNED;
        return 0;
    }

    if (cn_str_equals(lexer->token.str, CN_UNSIGNED_STR)) {
        if (node->type_specifier.sign != CN_TYPE_SIGN_NONE) {
            cn_log(CN_ERROR, "Duplicate type sign '%.*s' is not allowed in type specifier.", CN_UNPACK(CN_UNSIGNED_STR));
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.sign = CN_TYPE_SIGN_UNSIGNED;
        return 0;
    }

    // Checking if token is type width.
    if (cn_str_equals(lexer->token.str, CN_SHORT_STR)) {
        if (node->type_specifier.width != CN_TYPE_WIDTH_NONE) {
            cn_log(CN_ERROR, "Duplicate type width '%.*s' is not allowed in type specifier.", CN_UNPACK(CN_SHORT_STR));
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.width = CN_TYPE_WIDTH_SHORT;
        return 0;
    }

    if (cn_str_equals(lexer->token.str, CN_LONG_STR)) {
        if (node->type_specifier.width != CN_TYPE_WIDTH_NONE) {
            if (node->type_specifier.width == CN_TYPE_WIDTH_LONG) {
                cn_lexer_next_token(lexer);
                node->type_specifier.width = CN_TYPE_WIDTH_LONG_LONG;
                return 0;
            }

            cn_log(CN_ERROR, "Duplicate type width '%.*s' is not allowed in type specifier.", CN_UNPACK(CN_LONG_STR));
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.width = CN_TYPE_WIDTH_LONG;
        return 0;
    }


    // INCOMPLETE:
    // Checking if token(s) atomic_type_specifier.
    

    return 1;

error:
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator_list(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INIT_DECLARATOR_LIST };
    
    Cn_Ast_Idx child_idx;

    while(true) {
        child_idx = cn_ast_parse_init_declarator(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;

        cn_lexer_next_token(lexer);
    }

    return cn_ast_node_list_append(node);
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INIT_DECLARATOR };
    
    Cn_Ast_Idx child_idx;

    child_idx = cn_ast_parse_declarator(lexer);
    if (child_idx == CN_AST_NIL_IDX) goto error;
    cn_ast_node_add_child(&node, child_idx);


    if (cn_lexer_expect(lexer, CN_TOKEN_ASSIGN)) {
        cn_lexer_next_token(lexer);

        child_idx = cn_ast_parse_initializer(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);
    }

    return cn_ast_node_list_append(node);
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INITIALIZER };

    // TODO: Initializer implementation.

    return cn_ast_node_list_append(node);
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_DECLARATOR };

    Cn_Ast_Idx child_idx;
    
    // Optional pointer in declarator.
    if (cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        child_idx = cn_ast_parse_pointer(lexer);

        if (child_idx == CN_AST_NIL_IDX) goto error;

        cn_ast_node_add_child(&node, child_idx);
    }
    
    // Direct declarator parsing.
    child_idx = cn_ast_parse_direct_declarator(lexer);

    if (child_idx == CN_AST_NIL_IDX) goto error;

    cn_ast_node_add_child(&node, child_idx);

    
    return cn_ast_node_list_append(node);
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_pointer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_POINTER };
    
    if (!cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        cn_log(CN_ERROR, "Expected '*' here when parsing pointer.");
        cn_lexer_print_snippet_token(lexer);
        goto error;
    }

    cn_lexer_next_token(lexer);
    
    int ok;
    while (true) {
        ok = cn_ast_try_parse_qualifier(lexer, &node.pointer.qualifiers);
        if (ok == 0) continue;
        if (ok == 2) goto error;

        break;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        Cn_Ast_Idx child_idx;

        child_idx = cn_ast_parse_pointer(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);
    }
    
    return cn_ast_node_list_append(node);
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_direct_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_DIRECT_DECLARATOR };

    Cn_Ast_Idx child_idx;
    
    // '(' declarator ')' case.
    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        node.direct_declarator.kind = CN_DD_GROUPED;

        cn_lexer_next_token(lexer);

        child_idx = cn_ast_parse_declarator(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
            cn_log(CN_ERROR, "Expected ')' when parsing direct declarator.");
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);
    }
    // identifier case.
    else {
        node.direct_declarator.kind = CN_DD_IDENTIFIER;

        child_idx = cn_ast_parse_identifier(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_add_child(&node, child_idx);
    }
    
    Cn_Ast_Idx direct_declarator_idx = cn_ast_node_list_append(node);

    // Postfix cases.
    // direct_declarator '[' assignment_expression? ']' case.
    if (cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
        cn_lexer_next_token(lexer);

        node = (Cn_Ast_Node) { .kind = CN_AST_NODE_DIRECT_DECLARATOR };
        node.direct_declarator.kind = CN_DD_ARRAY;

        // TODO: Parse assignment_expression.
        
        if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
            cn_log(CN_ERROR, "Expected ']' when parsing direct declarator.");
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);

        cn_ast_node_add_child(&node, direct_declarator_idx);
        return cn_ast_node_list_append(node);
    }
    // direct_declarator '(' parameter_type_list? ')' case.
    else if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_lexer_next_token(lexer);

        node = (Cn_Ast_Node) { .kind = CN_AST_NODE_DIRECT_DECLARATOR };
        node.direct_declarator.kind = CN_DD_FUNCTION;

        // TODO: Parse parameter_type_list.

        if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
            cn_log(CN_ERROR, "Expected ')' when parsing direct declarator.");
            cn_lexer_print_snippet_token(lexer);
            goto error;
        }

        cn_lexer_next_token(lexer);

        cn_ast_node_add_child(&node, direct_declarator_idx);
        return cn_ast_node_list_append(node);
    }

    // No postfix case.
    return direct_declarator_idx;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_identifier(Cn_Lexer *lexer) {
    Cn_Ast_Node node = { .kind = CN_AST_NODE_IDENTIFIER };
    
    if (!cn_lexer_expect(lexer, CN_TOKEN_SYMBOL)) {
        cn_log(CN_ERROR, "Expected symbol when parsing identifier.");
        cn_lexer_print_snippet_token(lexer);
        goto error;
    }

    // IMPORTANT: String preserving area.
    node.identifier.name = lexer->token.str;

    cn_lexer_next_token(lexer);
    
    return cn_ast_node_list_append(node);

error:
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_attribute_specifier_sequence(Cn_Lexer *lexer) {
    CN_TODO("AST attribute specifier sequence.");
}

CNDEF Cn_Ast_Idx cn_ast_parse_asm_definition(Cn_Lexer *lexer) {
    CN_TODO("AST asm definition.");
}


// PRE-PROCESSING SECTION
Cn_Message_Handler *cn_message_handler = NULL;

CNDEF Cn_Translation_Unit cn_tu_make(char *intermidiate_path) {
    // Reading the whole .i file into memory.
    FILE *file = fopen(intermidiate_path, "rb");
    if (file == NULL) {
        cn_log(CN_ERROR, "Couldn't open the file '%s'.\n", intermidiate_path);
        return (Cn_Translation_Unit) {0};
    }

    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    void *buffer = CN_REALLOC(NULL, size); // LEAK.
    if (buffer == NULL) {
        cn_log(CN_ERROR, "Memory allocation for string buffer failed while reading the file '%s'.\n", intermidiate_path);
        fclose(file);
        return (Cn_Translation_Unit) {0};
    }

    if (fread(buffer, 1, size, file) != size) {
        cn_log(CN_ERROR, "Failure reading the file '%s'.\n", intermidiate_path);
        fclose(file);
        free(buffer);
        return (Cn_Translation_Unit) {0};
    }

    fclose(file);

    // Saving added file as single insert modification, so AST can of it can be constructed once cn_tu_process(...) is called.
    Cn_Translation_Unit tu = {
        .version = 0,
        .path = intermidiate_path,
        .content = {0},
        .modification_list = cn_array_list_make(Cn_Modification, CN_TU_MODIFICATION_LIST_INITIAL_CAP),
    };

    cn_array_list_append(&tu.modification_list, ((Cn_Modification) { .offset = 0, .kind = CN_INSERT, .insert = ((Cn_Modification_Insert){ CN_STR(size, buffer) }) }) );

    return tu;
}


CNDEF int cn_tu_process(Cn_Translation_Unit *tu) {
    // Check if there any modifications.
    // Process them if there are.
    // Start size with previous content length.
    // Expected to be zero by default.
    int64_t size = tu->content.length;
    for (int64_t i = 0; i < cn_array_list_length(&tu->modification_list); i++) {
        // TODO: Account for overlaps, and resolve them.
        switch (tu->modification_list[i].kind) {
            case CN_INSERT:
                size += tu->modification_list[i].insert.str.length;
                break;
            case CN_REMOVE:
                CN_TODO("Implement remove modification.");
                // size -= tu->modification_list[i].remove.length;
                break;
        }
    }

    char *buffer = (char *)CN_REALLOC(NULL, size);
    
    int64_t buffer_offset = 0;
    int64_t content_offset = 0;
    for (int64_t i = 0; i < cn_array_list_length(&tu->modification_list); i++) {
        memcpy(buffer + buffer_offset, tu->content.data + content_offset, tu->modification_list[i].offset - content_offset);
        buffer_offset += tu->modification_list[i].offset - content_offset;
        content_offset = tu->modification_list[i].offset;

        switch (tu->modification_list[i].kind) {
            case CN_INSERT:
                cn_str_copy_to(tu->modification_list[i].insert.str, buffer);
                break;
            case CN_REMOVE:
                CN_TODO("Implement remove modification.");
                break;
            default:
                CN_UNREACHABLE("Unexpected modification kind encountered.");
                break;
        }
    }

    CN_FREE(tu->content.data);

    tu->content.length = size;
    tu->content.data = buffer;

    cn_log(CN_INFO, "Received main.i:\n" CN_ANSI_BRIGHT_BLACK "%.*s" CN_ANSI_RESET, CN_UNPACK(tu->content));

    // Setting up lexer.
    Cn_Lexer lexer = {0};

    // Logging tokens.
    cn_lexer_init(&lexer, tu->content);

    cn_log(CN_INFO, "Tokenized main.i:" CN_ANSI_CYAN);
    do {
        cn_lexer_next_token(&lexer);
        fprintf(stderr, "TOKEN:     %.*s\n", CN_UNPACK(lexer.token.str));
    } while (lexer.token.type != CN_TOKEN_ZERO);
    fprintf(stderr, CN_ANSI_RESET"\n");
    
    // Building AST.
    cn_lexer_init(&lexer, tu->content);
    cn_ast_next_token(&lexer);



    Cn_Ast_Idx idx = cn_ast_parse_translation_unit(&lexer);

    if (idx == CN_AST_NIL_IDX) {
        return -1;
    }

    cn_ast_print(cn_ast_node_get(idx), 0);

    return 0;
}

CNDEF void cn_tu_free(Cn_Translation_Unit *tu) {
    CN_FREE(tu->content.data);
    cn_array_list_free(&tu->modification_list);
}



#endif // CN_IMPLEMENTATION



