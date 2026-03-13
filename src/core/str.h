#ifndef STR_H
#define STR_H

/**
 * String.
 */

#include "core/core.h"
#include "core/type.h"
#include <string.h>
#include <stdbool.h>


typedef struct string {
    s64 length;
    char *data;
} String;

#define STR(length, data)   ((String) { length, data } )
#define CSTR(cstring)       STR(strlen(cstring), cstring)

#define STR_BUFFER(literal) STR(sizeof(literal) - 1, (char[]){literal})
#define STR_BUFFER_EMPTY(size) STR(size, (char[size]){})

#define UNPACK(str)         (str).length, (str).data

#define UNPACK_LITERAL(literal)     (sizeof(literal) - 1), (literal)



/**
 * Returns String that points to the memory of original "str" at index "start" with length up until index "end".
 * Note: Character at index "end" is not included in the returned String, domain for resulting substring is always [ start, end ).
 * @Important: DOESN'T COPY MEMORY. If "str" memory is freed later, returned string will not point to valid adress anymore.
 */
String str_substring(String str, s64 start, s64 end);

/**
 * Compares "str1" and "str2", checks for lengths equality first and then compares symbol by symbol.
 * Returns true if strings are identical.
 */
bool str_equals(String str1, String str2);

/**
 * Linearly searches for the first occurnse of "search" in "str" from the LEFT, by comparing them through "str_equals()" function.
 * Returns the index of first character of the occurns, otherwise, returns -1.
 */
s64 str_find(String str, String search);

/**
 * Linearly searches for the first occurnse of char "symbol" in "str" from the LEFT, by comparing each char in "str".
 * Returns the index of first character of the occurns, otherwise, returns -1.
 */
s64 str_find_char_left(String str, char symbol);

/**
 * Linearly searches for the first occurnse of char "symbol" in "str" from the RIGHT, by comparing each char in "str".
 * Returns the index of first character of the occurns, otherwise, returns -1.
 */
s64 str_find_char_right(String str, char symbol);

/**
 * Finds index of the first non whitespace occurns from the left.
 */
s64 str_find_non_whitespace_left(String str);

/**
 * Finds index of the first non whitespace occurns from the right.
 */
s64 str_find_non_whitespace_right(String str);

/**
 * Finds index of the first whitespace occurns from the left.
 */
s64 str_find_whitespace_left(String str);

/**
 * Finds index of the first whitespace occurns from the right.
 */
s64 str_find_whitespace_right(String str);

/**
 * Shortens the string by excluding specified count of chars from the left.
 */
String str_eat_chars(String str, s64 count);

/**
 * Shortens the string from the left by excluding all of the whitespace chars.
 */
String str_eat_spaces(String str);

/**
 * Shortens the string from the left by excluding the chars until a it hits a whitespace, doesn't exclude a whitespace.
 */
String str_eat_until_space(String str);

/**
 * Returns string that contains everything until a whitespace, deosn't include a whitespace char.
 */
String str_get_until_space(String str);

/**
 * Retursn true is str contians only symbol chars.
 */
bool str_is_symbol(String str);

/**
 * Retursn true is str is an integer.
 */
bool str_is_int(String str);

/**
 * Retursn true is str is a float.
 */
bool str_is_float(String str);

/**
 * Returns integer parsed from the string.
 */
s64 str_parse_int(String str);

/**
 * Returns float parsed from the string.
 */
float str_parse_float(String str);

/**
 * Counts the number of specific characters in the string.
 */
s64 str_count_chars(String str, char c);

/**
 * Copies contents of the src string into buffer.
 * @Important: buffer should have enough space to hold src data.
 */
void *str_copy_to(String str, void *buffer);

/**
 * Formats string and outputs it into a buffer memory, buffer should be big enough to hold the string.
 * Returns string that corresponds to the formatted string.
 */
String str_format(String buffer, char *format, ...);

/**
 * Checks whether the string is empty by checking whether the length of the string is 0.
 */
static inline bool str_empty(String str) {
    return str.length == 0;
}


// typedef struct string_builder_node {
// 
// } String_Builder_Node;
// 
// 
// typedef struct string_builder {
//     s64 count;
//     String *elements;
// } String_Builder;
// 
// 
// String_Builder str_builder_make(s64 count, String *elements);
// 
// 
// String str_builder_cut
// 
// /**
//  * Returns total length of all string elements inside a builder.
//  */
// s64 str_builder_length(String_Builder *builder);
// 
// /**
//  * Builds the string outputting all contents into buffer memory.
//  * Returns string that corresponds to the outputted string.
//  * @Important: buffer should be big enough to hold the contents.
//  */
// String str_builder_build(String_Builder *builder, String buffer);



#endif
