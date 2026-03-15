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
#endif /* CN_REALLOC */

#ifndef CN_FREE
#   include <stdlib.h>
#   define CN_FREE free
#endif /* CN_FREE */

#ifdef _WIN32
#    define CN_LINE_END "\r\n"
#else
#    define CN_LINE_END "\n"
#endif // _WIN32

#include <stdio.h>
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
#endif

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
 * Thank you nob and stb <3.
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
#define CN_STR(length, data)            ((Cn_String) { length, data } )
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
 * IMPORTANT: DOESN'T COPY MEMORY. If "str" memory is freed later, returned string will not point to valid adress anymore.
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
    uint64_t line_num;
} Cn_Token;

typedef struct {
    Cn_Token_Type type;
    Cn_String literal;
} Cn_Literal_Token;

typedef struct {
    uint64_t cursor;
    uint64_t bol;            // BOL -> Beginning Of Line
    uint64_t line_num;
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
            fprintf(stderr, "[INFO] ");
            break;
        case CN_WARNING:
            fprintf(stderr, "[WARN] ");
            break;
        case CN_ERROR:
            fprintf(stderr, "[ERRO] ");
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
    CN_ASSERT(capacity > 0);

    Cn_Array_List_Header *ptr = CN_REALLOC(NULL, sizeof(Cn_Array_List_Header) + item_size * capacity);

    if (ptr == NULL) {
        cn_log(CN_ERROR, "Couldn't allocate more memory of size: %lld bytes, for the array list.", sizeof(Cn_Array_List_Header) + item_size * capacity);
        return NULL;
    }

    ptr->capacity = capacity;
    ptr->item_size = item_size;
    ptr->length = 0;

    // IMPORTANT: Because ptr is of type "Cn_Array_List_Header *", compiler will automatically translate "ptr + 1" to "(void*)(ptr) + sizeof(Cn_Array_List_Header)".
    return ptr + 1;
}

CNDEF int64_t cn__array_list_length(void *list) {
    return ((Cn_Array_List_Header *)(list - sizeof(Cn_Array_List_Header)))->length;
}

CNDEF int64_t cn__array_list_capacity(void *list) {
    return ((Cn_Array_List_Header *)(list - sizeof(Cn_Array_List_Header)))->capacity;
}

CNDEF int64_t cn__array_list_item_size(void *list) {
    return ((Cn_Array_List_Header *)(list - sizeof(Cn_Array_List_Header)))->item_size;
}

CNDEF void cn__array_list_free(void **list) {
    CN_FREE(*list - sizeof(Cn_Array_List_Header));
    *list = NULL;
}

CNDEF void cn__array_list_resize_to_fit(void **list, int64_t requiered_length) {
    Cn_Array_List_Header *header = *list - sizeof(Cn_Array_List_Header);

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

        header = CN_REALLOC(header, sizeof(Cn_Array_List_Header) + header->capacity * capacity_multiplier * header->item_size);
        
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

    Cn_Array_List_Header *header = *list - sizeof(Cn_Array_List_Header);

    // Copying items to the list.
    memcpy(*list + header->length * header->item_size, items, header->item_size * count);
    header->length += count;

    return header->length - count;
}

CNDEF void cn__array_list_pop(void *list, int64_t count) {
    ((Cn_Array_List_Header *)(list - sizeof(Cn_Array_List_Header)))->length -= count;
}

CNDEF void cn__array_list_clear(void *list) {
    ((Cn_Array_List_Header *)(list - sizeof(Cn_Array_List_Header)))->length = 0;
}

CNDEF void cn__array_list_unordered_remove(void *list, int64_t index) {
    Cn_Array_List_Header *header = list - sizeof(Cn_Array_List_Header);
    // Copy last element in the list to the index position, and then pop element at the end.
    memcpy(list + index * header->item_size, list + (header->length - 1) * header->item_size, header->item_size);
    cn__array_list_pop(list, 1);
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
        .str.data = l->content.data + l->cursor, 
        .str.length = 0,
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

    for (int64_t i = 0; i < CN_ARRAY_LENGTH(CN_LITERAL_TOKENS); i++) {
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



#endif // CN_IMPLEMENTATION



