/*  cnotes - v0.1.0 - MIT License - https://github.com/dmarshaq/cnotes.h

    A header-only library for building C meta programs, static analysis tools, 
    and source transformation pipelines - operating directly on pre-processed .i files.

    # Macro Interface

        All these macros are `#define`d by the user before including cnotes.h,
        in a similar fashion nob.h and other stb signle header libraries operate.

    ## Flags

        Enable or disable certain aspects of cnotes.h

      - CN_IMPLEMENTATION                   Enable definitions of the functions. By default only declarations are included.
                                            See https://github.com/nothings/stb/blob/f58f558c120e9b32c217290b80bad1a0729fbb2c/docs/stb_howto.txt
                                            for more information.
      - CN_ANSI_NO_COLOR                    Sets all CN_ANSI_* macros to empty string, disabling any colored output.

    ## Redefinable Macros

        Redefine default behaviors of nob.h.

      - CNDEF                               Appends additional things to function declarations.
      - CN_ASSERT(condition)                Redefine which assert() cnotes.h shall use.
      - CN_REALLOC(oldptr, size)            Redefine which realloc() cnotes.h shall use.
      - CN_FREE(ptr)                        Redefine which free() cnotes.h shall use.
      - CN_AST_NODE_LIST_INITIAL_CAP        Redefine initial capacity of array list that holds ast nodes.
      - CN_BUILD_OPT_DEFAULT_ALLOC          Redefine default value of alloc optional in cn_build_* functios.
                                            See Cn_Build_Opt for more information.

    # API Conventions & Navigation

        The rules below ensure a stable, well documented API across all public releases. 
        Since this library grew larger than expected, it's split into multiple sections, 
        each following its own convention and grouping related functions into manageable, 
        easy to understand units.

    ## Conventions
        
      - All user facing interface is prefixed with `cn_`, `Cn_`, or `CN_` respectivly depending on the use case.
      - Internal functionality is prefixed with `cn__`, `Cn__`, or `CN__` - double underscore, depending on the use case.
        Such functionality is not recommended for the user to work with as it can backward incompatible in any releases,
        and is considered unstable, therefor is labeled as internal.
      - Functions ending with `_opt` denote ability to accept optional parameters through respective `_Opt` ending struct.
        Usually are internal and are coupled with user facing macros.

    ## Navigation
        
        Below are prefixes that appear after initial `cn_` or `cn__` prefix that denote which section the function is coming from.
        Entries marked with CAUTION:, indicate that specific user interface is activily being changed and developed, 
        and is to be used with caution.
        
        `log_`                          Responsible for libraries logging interface, with separate log handler idea from nob.h.
        `hash_`                         Used for various hashing operations by the library.
        `str_`                          Interface for Cn_String operations.
        `sb_`                           Interface for Cn_String_Builder operations.
        `chained_arena_`                Interface for Cn_Chained_Arena operations.
        `array_list_`                   Interface to operate on dynamically allocated array list.
        `hash_table_`                   Interface to operate on dynamically allocated hash table.
        `hash_set_`                     Interface to operate on dynamically allocated hash set.
        `source_`                       Small interface for Cn_Source basic operations
        `lexer_`                        Interface for Cn_Lexer operations, is coupled with `token_` functionality.
        `type_`                         Interface for type operations and introspection, inspired by Jai Programming Language Type_Info.
        `any_`                          Interface to operate on coupled type and data, idea also inspired by Jai.
        `ast_`                 CAUTION: Interface for majority of Abstract Syntax Tree analysis and parsing functionality.
        `ast_parse_`           CAUTION: Interface for parsing tokens into Abstract Syntax Tree.
        `ast_reparse_`         CAUTION: Interface for validating and reanalyzing already existing Abstract Syntax Tree.
        `ast_binding_`         CAUTION: Interface for operating on various C bindings that come from analyzed Abstract Syntax Tree.
        `emit_`                         Interface for C code emittion from Abstract Syntax Tree.
        `tu_`                           Interface to manage Translation Unit parsing, analysis, code modification, and emittion from start to finish.
        `build_`                        Interface for user friendly construction of Abstract Syntax Tree, to be used in code transformations.

*/
 

#ifndef CN_H_
#define CN_H_

#ifndef CNDEF
#   define CNDEF
#endif // CNDEF

#ifndef CN_ASSERT
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
#   define CN_LINE_END "\r\n"
#else
#   define CN_LINE_END "\n"
#endif // _WIN32

#define CN_INDENT "    "

#ifdef CN_ANSI_NO_COLOR
#   define CN_ANSI_BLACK            ""
#   define CN_ANSI_RED              ""
#   define CN_ANSI_GREEN            ""
#   define CN_ANSI_YELLOW           ""
#   define CN_ANSI_BLUE             ""
#   define CN_ANSI_MAGENTA          ""
#   define CN_ANSI_CYAN             ""
#   define CN_ANSI_WHITE            ""
#   define CN_ANSI_RESET            ""
#   define CN_ANSI_BRIGHT_BLACK     ""
#   define CN_ANSI_BRIGHT_RED       ""
#   define CN_ANSI_BRIGHT_GREEN     ""
#   define CN_ANSI_BRIGHT_YELLOW    ""
#   define CN_ANSI_BRIGHT_BLUE      ""
#   define CN_ANSI_BRIGHT_MAGENTA   ""
#   define CN_ANSI_BRIGHT_CYAN      ""
#   define CN_ANSI_BRIGHT_WHITE     ""
#   define CN_ANSI_BOLD             ""
#else
#   define CN_ANSI_BLACK            "\x1b[30m"
#   define CN_ANSI_RED              "\x1b[31m"
#   define CN_ANSI_GREEN            "\x1b[32m"
#   define CN_ANSI_YELLOW           "\x1b[33m"
#   define CN_ANSI_BLUE             "\x1b[34m"
#   define CN_ANSI_MAGENTA          "\x1b[35m"
#   define CN_ANSI_CYAN             "\x1b[36m"
#   define CN_ANSI_WHITE            "\x1b[37m"
#   define CN_ANSI_RESET            "\x1b[0m"
#   define CN_ANSI_BRIGHT_BLACK     "\x1b[90m"
#   define CN_ANSI_BRIGHT_RED       "\x1b[91m"
#   define CN_ANSI_BRIGHT_GREEN     "\x1b[92m"
#   define CN_ANSI_BRIGHT_YELLOW    "\x1b[93m"
#   define CN_ANSI_BRIGHT_BLUE      "\x1b[94m"
#   define CN_ANSI_BRIGHT_MAGENTA   "\x1b[95m"
#   define CN_ANSI_BRIGHT_CYAN      "\x1b[96m"
#   define CN_ANSI_BRIGHT_WHITE     "\x1b[97m"
#   define CN_ANSI_BOLD             "\x1b[1m"
#endif // CN_ANSI_NO_COLOR

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>
#include <stddef.h>

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
#define CN_TODO(message) do { fprintf(stderr, "%s:%d: "CN_ANSI_BRIGHT_RED"TODO:"CN_ANSI_RESET" %s\n", __FILE__, __LINE__, message); abort(); } while(0)
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

// HASHING SECTION
typedef uint64_t (Cn_Hash_Function)(const void *data);

typedef bool (Cn_Equals_Function)(const void *a, const void *b);

/**
 * Unsigned 64 integer hashing using splitmix64 algorithm.
 */
CNDEF uint64_t cn_hash_u64(uint64_t value);

/**
 * Hashing of specified address.
 *
 * IMPORTANT: Wraps around cn_hash_u64 function.
 */
CNDEF uint64_t cn_hash_ptr(void *ptr);

/**
 * Hashing sequence of bytes using FNV-1a implementation.
 */
CNDEF uint64_t cn_hash_bytes(void *data, int64_t length);

/**
 * Mixes two hashes a and b. Order matters, since it 
 * uses bit shifts to ensure mix(a, b) != mix(b, a).
 */
CNDEF uint64_t cn_hash_mix(uint64_t a, uint64_t b);

// STRING SECTION

/**
 * Here length based strings behave like string views,
 * they contain simple structure: length + data.
 * And an interface to variously modify and use them.
 */
typedef struct {
    int64_t length;
    char *data;
} Cn_String;

/**
 * Directly construct string view taking in it's length and data.
 */
#define CN_STR(length, data)            ((Cn_String) { (int64_t)length, (char *)data } )

/**
 * Construct string view of supplied NUL terminated string.
 */
#define CN_CSTR(cstring)                CN_STR(strlen(cstring), cstring)

/**
 * Construct string view directly from literal.
 */
#define CN_STR_LIT(literal)             CN_STR(sizeof(literal) - 1, literal)

/**
 * Taking in string literal this macro allocates string character on the stack,
 * and uses that buffer to construct string view.
 */
#define CN_STR_BUFFER(literal)          CN_STR(sizeof(literal) - 1, (char[]){literal})

/**
 * Works exactly like CN_STR_BUFFER allocating char array on the stack, 
 * but doesn't take in literal or fill the buffer with any characters.
 */
#define CN_STR_BUFFER_EMPTY(size)       CN_STR(size, (char[size]){})

/**
 * This macro "unpacks" the string into length and data components.
 * Is used in formating of the string. For example given string "str":
 *      
 *      printf("value of str: %.*s", CN_UNPACK(str));
 *
 * NOTE: Cast to int is required, because %.*s expects maximum length of 32 bit INT_MAX.
 * Yes, it will truncate the string if it's length exceeds INT_MAX.
 */
#define CN_UNPACK(str)                  (int)((str).length), (str).data

/**
 * This is just a shortcut macro for unpackng any string literal 
 * into the same length and data components.
 */
#define CN_UNPACK_LITERAL(literal)      (int)(sizeof(literal) - 1), (literal)

/**
 * RETURNS: Cn_String that points to the memory of original "str" at index "start" with length up until index "end".
 *
 * NOTE: Character at index "end" is not included in the returned Cn_String, domain for resulting substring is always [ start, end ).
 *
 * IMPORTANT: DOESN'T COPY MEMORY. If "str" memory is freed later, returned string will not point to valid address anymore.
 */
CNDEF Cn_String cn_str_substring(Cn_String str, int64_t start, int64_t end);

/**
 * Linearly searches for the first occurrence of "search" in "str" from the LEFT, by comparing them through "cn_str_equals()" function.
 *
 * RETURNS: Index of first character of the occurrence, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find_left(Cn_String str, Cn_String search);

/**
 * Linearly searches for the first occurrence of "search" in "str" from the RIGHT, by comparing them through "cn_str_equals()" function.
 * RETURNS: Index of first character of the occurrence, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find_right(Cn_String str, Cn_String search);

/**
 * Linearly searches for the first occurrence of char "symbol" in "str" from the LEFT, by comparing each char in "str".
 *
 * RETURNS: Index of first character of the occurrence, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find_char_left(Cn_String str, char symbol);

/**
 * Linearly searches for the first occurrence of char "symbol" in "str" from the RIGHT, by comparing each char in "str".
 * 
 * RETURNS: Index of first character of the occurrence, otherwise, returns -1.
 */
CNDEF int64_t cn_str_find_char_right(Cn_String str, char symbol);

/**
 * RETURNS: Index of the first non-whitespace occurrence from the left.
 */
CNDEF int64_t cn_str_find_non_whitespace_left(Cn_String str);

/**
 * RETURNS: Index of the first non-whitespace occurrence from the right.
 */
CNDEF int64_t cn_str_find_non_whitespace_right(Cn_String str);

/**
 * RETURNS: Index of the first whitespace occurrence from the left.
 */
CNDEF int64_t cn_str_find_whitespace_left(Cn_String str);

/**
 * RETURNS: Index of the first whitespace occurrence from the right.
 */
CNDEF int64_t cn_str_find_whitespace_right(Cn_String str);

/**
 * Shortens the string by excluding specified count of chars from the left.
 *
 * RETURNS: Resulting string.
 */
CNDEF Cn_String cn_str_eat_chars(Cn_String str, int64_t count);

/**
 * Shortens the string from the left by excluding all of the whitespace chars.
 *
 * RETURNS: Resulting string.
 */
CNDEF Cn_String cn_str_eat_spaces(Cn_String str);

/**
 * Shortens the string from the left by excluding the chars until a it hits a whitespace, doesn't exclude a whitespace.
 *
 * RETURNS: Resulting string.
 */
CNDEF Cn_String cn_str_eat_until_space(Cn_String str);

/**
 * RETURNS: String that contains specified count of chars from the left.
 */
CNDEF Cn_String cn_str_get_chars(Cn_String str, int64_t count);

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
 * RETURNS: Count of the number of specific characters in the string.
 */
CNDEF int64_t cn_str_count_chars(Cn_String str, char c);

/**
 * Copies contents of the src string into buffer.
 *
 * IMPORTANT: Buffer should have enough space to hold src data.
 *
 * RETURNS: Pointer to the destination buffer.
 */
CNDEF void *cn_str_copy_to(Cn_String str, void *buffer);

/**
 * Formats string and outputs it into a buffer memory, buffer should be big enough to hold the string.
 *
 * RETURNS: String that corresponds to the formatted string.
 */
CNDEF Cn_String cn_str_format(Cn_String buffer, char *format, ...);

/**
 * Checks whether the string is empty by checking whether the length of the string is 0.
 *
 * RETURNS: True if string is empty, false otherwise.
 */
CNDEF bool cn_str_is_empty(Cn_String str);

/**
 * Compares "str1" and "str2", checks for lengths equality first and then compares symbol by symbol.
 *
 * RETURNS: True if strings are identical.
 *
 * IMPORTANT: It uses pointers to strings to be compatible with Cn_Equals_Function,
 * which allows it to be used in hash table and hash set.
 */
CNDEF bool cn_str_equals(const Cn_String *str1, const Cn_String *str2);

/**
 * RETURNS: Hash of specified string.
 *
 * IMPORTANT: It uses pointers to strings to be compatible with Cn_Hash_Function,
 * which allows it to be used in hash table and hash set.
 */
CNDEF uint64_t cn_str_hash(const Cn_String *str);

// STRING BUILDER SECTION
typedef struct {
    int64_t capacity;
    int64_t length;
    char *data;
} Cn_String_Builder;

#define CN_SB_STACK_STORAGE_CAP 32

/**
 * This macro allocates new Cn_String_Builder on the stack if capacity is small enough,
 * otherwise allocates it on the heap.
 *
 * IMPORTANT: String builder lifetime thus depends on the life time of stack allocated string.
 * If greater life time needed make sure to heap allocate string builder,
 * or make sure it is resized to be heap allocated.
 */
#define cn_sb_make(cap) ((cap) <= CN_SB_STACK_STORAGE_CAP ? ((Cn_String_Builder) { CN_SB_STACK_STORAGE_CAP, 0, (char *)(char[CN_SB_STACK_STORAGE_CAP]){}}) : cn__sb_make(cap))

/**
 * Constructs new string builder with specified initial capacity.
 * Heap allocates the string builder.
 *
 * IMPORTANT: Expects initial_capacity > CN_SB_STACK_STORAGE_CAP.
 */
CNDEF Cn_String_Builder cn__sb_make(int64_t initial_capacity);

/**
 * Free's heap memory occupied by string builder, 
 * only if capacity > CN_SB_STACK_STORAGE_CAP.
 */
CNDEF void cn_sb_free(Cn_String_Builder *sb);

/**
 * Clears all contents of string builder, by setting it's length to 0.
 * Doesn't allocate or free any memory.
 */
CNDEF void cn_sb_clear(Cn_String_Builder *sb);

/**
 * Appends char to the specified string builder.
 */
CNDEF void cn_sb_append_char(Cn_String_Builder *sb, char c);

/**
 * Appends string to the specified string builder.
 */
CNDEF void cn_sb_append_str(Cn_String_Builder *sb, Cn_String str);

/**
 * Formats string and appends output to the specified string builder.
 */
CNDEF void cn_sb_append_format(Cn_String_Builder *sb, char *format, ...);

/**
 * Reverses contents of the specified string builder.
 */
CNDEF void cn_sb_reverse(Cn_String_Builder *sb);

/**
 * Converts string builder back to str, that points to the memory used by string builder.
 * 
 * IMPORTANT: If string builder is freed, memory occupied by the string might be invalid.
 *
 * NOTE: After getting string it's contents can be copied elsewhere to prevent such invalidation.
 */
CNDEF Cn_String cn_sb_to_str(Cn_String_Builder *sb);

// CHAINED ARENA SECTION

/**
 * Chained arena is an allocator that linearly gives memory and when needed allocates next block.
 * It pointers returned guranteed to point to valid memory at all times, till freed, or popped.
 * Instead of making forward linked list of blocks.
 * It makes reverse linked list of blocks and always points to the most recent block.
 * 
 *  block_capacity
 *  <--------->         
 *
 *  -----------     -----------     -----------
 *  | Block 1 | <-- | Block 2 | <-- | Block 3 |
 *  -----------     -----------     -----------
 *                                  ^      ^
 *                                  *block *ptr
 *  <------------- allocated ------------->
 *
 *
 *  These setup allows quick append allocation and pop deallocation.
 */
typedef struct {
    uint64_t block_capacity;
    void *block;
} Cn_Chained_Arena;

#define CN_CHAINED_ARENA_BLOCK_HEADER(block) ((Cn_Chained_Arena_Block_Header *)((uint8_t *)(block) - sizeof(Cn_Chained_Arena_Block_Header)))

typedef struct {
    void *prev;
    uint64_t allocated;
} Cn_Chained_Arena_Block_Header;

/**
 * Initializes arena by allocating first block and setting all pointers.
 */
CNDEF Cn_Chained_Arena cn_chained_arena_make(uint64_t block_capacity);

/**
 * Allocates specified memory size from the arena.
 *
 * RETURNS: Pointer to the memory segment.
 *
 * IMPORTANT: It will be invalid once arena is freed.
 */
CNDEF void *cn_chained_arena_alloc(Cn_Chained_Arena *arena, uint64_t size);

/**
 * Dellocates specified memory size from the front of the arena.
 * If specified size completely deallocates whole block, it is freed.
 */
CNDEF void cn_chained_arena_dealloc(Cn_Chained_Arena *arena, uint64_t size);

/**
 * Given chained arena and pointer allocated by it, it will compute index 
 * of the allocation block it was allocated in, and byte offset from the beginning 
 * of the allocation block to the allocated memory.
 *
 * RETURNS: Index of the allocation block, -1 if pointer supplied doesn't point to the allocated memory.
 *
 * OUTPUTS: Byte offset to the allocated memory from beginning of the allocation block.
 *
 * NOTE: This function will properly return if supplied pointer points to the memory held by the arena.
 * Even if address was not supplied correctly, but it happens to point to the allocated memory, 
 * this function will properly return.
 */
CNDEF int64_t cn_chained_arena_allocation_info(Cn_Chained_Arena *arena, void *allocation_ptr, uint64_t *offset);

/**
 * Given chained arena computes total allocated size by the arena.
 *
 * RETURNS: Total allocated size.
 */
CNDEF uint64_t cn_chained_arena_allocated(Cn_Chained_Arena *arena);

#define cn_ast_chained_arena_foreach_in_block(type, it, block) for (type *it = (type *)(block); (uint8_t *)it <= (uint8_t *)(block) + CN_CHAINED_ARENA_BLOCK_HEADER(block)->allocated - sizeof(type); it++)

/**
 * Completely frees all memory occupied by the arena.
 */
CNDEF void cn_chained_arena_free(Cn_Chained_Arena *arena);

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

typedef struct {
    int64_t capacity;
    int64_t count;
    int64_t item_size;
    int64_t key_size;
    Cn_Hash_Function *hash_func; 
    Cn_Equals_Function *equals_func; 
    float load_factor;
} Cn_Hash_Table_Header;

typedef enum {
    CN_HASH_TABLE_SLOT_EMPTY,
    CN_HASH_TABLE_SLOT_OCCUPIED,
    CN_HASH_TABLE_SLOT_DELETED,
} Cn_Hash_Table_Slot_State;

typedef struct {
    Cn_Hash_Table_Slot_State state;
    uint64_t hash;
} Cn_Hash_Table_Slot;

#define cn_hash_table_make(key_type, value_type, capacity, hash_func, equals_func)\
    (value_type *)cn__hash_table_make(sizeof(key_type), sizeof(value_type), capacity, hash_func, equals_func) 

#define cn_hash_table_count(ptr_table)\
    cn__hash_table_count((void *)*ptr_table)

#define cn_hash_table_capacity(ptr_table)\
    cn__hash_table_capacity((void *)*ptr_table)

#define cn_hash_table_item_size(ptr_table)\
    cn__hash_table_item_size((void *)*ptr_table)

#define cn_hash_table_put(ptr_table, item, ptr_key)\
    do {\
        cn__hash_table_resize_to_fit((void **)(ptr_table), cn_hash_table_count(ptr_table) + 1);\
        (*ptr_table)[cn__hash_table_push_key((void **)(ptr_table), ptr_key)] = item;\
    } while(0)

#define cn_hash_table_get(ptr_table, ptr_key)\
    cn__hash_table_get((void **)(ptr_table), ptr_key)

#define cn_hash_table_remove(ptr_table, ptr_key)\
    cn__hash_table_remove((void **)(ptr_table), ptr_key) 

#define cn_hash_table_free(ptr_table)\
    cn__hash_table_free((void **)(ptr_table))

#define cn_hash_table_header(table_ptr)\
    ((Cn_Hash_Table_Header *)((uint8_t *)*table_ptr - sizeof(Cn_Hash_Table_Header)))

CNDEF void cn_hash_table_print(FILE *f, void **table);

CNDEF void *cn__hash_table_make(int64_t key_size, int64_t item_size, int64_t capacity, Cn_Hash_Function *hash_func, Cn_Equals_Function *equals_func);

CNDEF int64_t cn__hash_table_count(void *table);

CNDEF int64_t cn__hash_table_capacity(void *table);

CNDEF int64_t cn__hash_table_item_size(void *table);

CNDEF void cn__hash_table_resize_to_fit(void **table, int64_t requiered_length);

CNDEF int64_t cn__hash_table_push_key(void **table, void *key);

CNDEF void *cn__hash_table_get(void **table, void *key);

CNDEF void cn__hash_table_remove(void **table, void *key);

CNDEF void cn__hash_table_free(void **table);

// HASH SET SECTION

typedef struct {
    int64_t capacity;
    int64_t count;
    int64_t item_size;
    Cn_Hash_Function *hash_func; 
    Cn_Equals_Function *equals_func;
    float load_factor;
} Cn_Hash_Set_Header;

typedef enum {
    CN_HASH_SET_SLOT_EMPTY,
    CN_HASH_SET_SLOT_OCCUPIED,
    CN_HASH_SET_SLOT_DELETED,
} Cn_Hash_Set_Slot_State;

typedef struct {
    Cn_Hash_Set_Slot_State state;
    uint64_t hash;
} Cn_Hash_Set_Slot;

#define cn_hash_set_make(type, capacity, hash_func, equals_func)\
    (type *)cn__hash_set_make(sizeof(type), capacity, hash_func, equals_func) 

#define cn_hash_set_header(ptr_set)\
    ((Cn_Hash_Set_Header *)((uint8_t *)((*ptr_set) - 1) - sizeof(Cn_Hash_Set_Header)))

#define cn_hash_set_put(ptr_set, item)\
    (*((*ptr_set) - 1) = item, cn__hash_set_put(cn_hash_set_header(ptr_set), (void **)ptr_set), *((*ptr_set) - 1))

#define cn_hash_set_contains(ptr_set, item)\
    (*((*ptr_set) - 1) = item, cn__hash_set_contains(cn_hash_set_header(ptr_set)))

#define cn_hash_set_remove(ptr_set, item)\
    (*((*ptr_set) - 1) = item, cn__hash_set_remove(cn_hash_set_header(ptr_set)))

#define cn_hash_set_free(ptr_set)\
    cn__hash_set_free(cn_hash_set_header(ptr_set))

CNDEF void *cn__hash_set_make(int64_t item_size, int64_t capacity, Cn_Hash_Function *hash_func, Cn_Equals_Function *equals_func);

CNDEF void cn__hash_set_put(Cn_Hash_Set_Header *header, void **set);

CNDEF bool cn__hash_set_contains(Cn_Hash_Set_Header *header);

CNDEF void cn__hash_set_remove(Cn_Hash_Set_Header *header);

CNDEF void cn__hash_set_free(Cn_Hash_Set_Header *header);

// SOURCE SECTION

typedef struct {
    Cn_String file;
    int64_t line;
    int64_t column;
} Cn_Location;

typedef struct {
    Cn_String source;
    int64_t offset;
    int64_t length;
} Cn_Source;

/**
 * RETURNS: String based on the source supplied.
 * 
 * IMPORTANT: If location length is 0, empty string is returned, 
 * but it's data will be pointed to the valid memory specified by source.
 */
CNDEF Cn_String cn_source_to_str(Cn_Source *src);

/**
 * RETURNS: Signed distance from source a to source b.
 */
CNDEF int64_t cn_source_dist(Cn_Source *a, Cn_Source *b);

/**
 * RETURNS: True if source is empty.
 */
CNDEF int64_t cn_source_is_empty(Cn_Source *src);

// LEXER SECTION

typedef enum {
    CN_TOKEN_EOF,
    CN_TOKEN_UNKNOWN,
    CN_TOKEN_STRING,
    CN_TOKEN_COMMENT,
    CN_TOKEN_LINE_MARKER,
    CN_TOKEN_INTEGER_VALUE,
    CN_TOKEN_FLOAT_VALUE,
    CN_TOKEN_IDENTIFIER,
    // Keyword tokens
    CN_TOKEN_GNU_EXTENSION,
    CN_TOKEN_GNU_ATTRIBUTE,
    CN_TOKEN_CONST,
    CN_TOKEN_RESTRICT,
    CN_TOKEN_VOLATILE,
    CN_TOKEN_ATOMIC,
    CN_TOKEN_STATIC,
    CN_TOKEN_EXTERN,
    CN_TOKEN_REGISTER,
    CN_TOKEN_AUTO,
    CN_TOKEN_TYPEDEF,
    CN_TOKEN_SIGNED,
    CN_TOKEN_UNSIGNED,
    CN_TOKEN_SHORT,
    CN_TOKEN_LONG,
    CN_TOKEN_INT,
    CN_TOKEN_CHAR,
    CN_TOKEN_FLOAT,
    CN_TOKEN_DOUBLE,
    CN_TOKEN_BOOL,
    CN_TOKEN_VOID,
    CN_TOKEN_STRUCT,
    CN_TOKEN_ENUM,
    CN_TOKEN_UNION,
    CN_TOKEN_GNU_TYPEOF,
    CN_TOKEN_IF,
    CN_TOKEN_ELSE,
    CN_TOKEN_SWITCH,
    CN_TOKEN_WHILE,
    CN_TOKEN_DO,
    CN_TOKEN_FOR,
    CN_TOKEN_GOTO,
    CN_TOKEN_CONTINUE,
    CN_TOKEN_BREAK,
    CN_TOKEN_RETURN,
    CN_TOKEN_SIZEOF,
    CN_TOKEN_INLINE,
    CN_TOKEN_NORETURN,
    CN_TOKEN_ASM,
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
    CN_TOKEN_SLASH,
    CN_TOKEN_PLUS,
    CN_TOKEN_MINUS,
    CN_TOKEN_PERCENT,
    CN_TOKEN_INCREMENT,
    CN_TOKEN_DECREMENT,
    CN_TOKEN_LSHIFT,
    CN_TOKEN_RSHIFT,
    CN_TOKEN_LESS,
    CN_TOKEN_GREATER,
    CN_TOKEN_LESS_EQ,
    CN_TOKEN_GREATER_EQ,
    CN_TOKEN_EQ,
    CN_TOKEN_NOT_EQ,
    CN_TOKEN_AMPERSAND,
    CN_TOKEN_HAT,
    CN_TOKEN_BAR,
    CN_TOKEN_AND,
    CN_TOKEN_OR,
    CN_TOKEN_EXCLAMATION,
    CN_TOKEN_TILDE,
    CN_TOKEN_LSHIFT_ASSIGN,
    CN_TOKEN_RSHIFT_ASSIGN,
    CN_TOKEN_MULTIPLY_ASSIGN,
    CN_TOKEN_DIVIDE_ASSIGN,
    CN_TOKEN_MODULO_ASSIGN,
    CN_TOKEN_PLUS_ASSIGN,
    CN_TOKEN_MINUS_ASSIGN,
    CN_TOKEN_BIT_AND_ASSIGN,
    CN_TOKEN_BIT_XOR_ASSIGN,
    CN_TOKEN_BIT_OR_ASSIGN,
    CN_TOKEN_QUESTION,
    CN_TOKEN_ELLIPSIS,
} Cn_Token_Type;

typedef struct {
    Cn_Token_Type   type; // TODO: Rename to kind.
    Cn_Location     loc;
    Cn_Source       src;
} Cn_Token;

typedef struct {
    Cn_Token_Type type;
    Cn_String     literal;
} Cn_Token_Literal;

/**
 * Count of tokens lexer can lookup to, meaning tokens that are ahead.
 */
#define CN_LEXER_LOOKUP_COUNT       8

/**
 * Count of tokens lexer can lookdown to, meaning tokens that already passed.
 */
#define CN_LEXER_LOOKDOWN_COUNT     8

#define CN_LEXER_QUEUE_COUNT        (CN_LEXER_LOOKDOWN_COUNT + 1 + CN_LEXER_LOOKUP_COUNT)

typedef struct {
    Cn_Token_Type *ttypes;
    int64_t length;
} Cn_Lexer_Blacklist;

typedef struct {
    int64_t cursor;
    int64_t bol;            // BOL -> Beginning Of Line
    int64_t line;
    Cn_String file;
    Cn_String content;
    int64_t head_idx;
    int64_t tail_idx;
    int64_t current_idx;
    Cn_Token queue[CN_LEXER_LOOKDOWN_COUNT + 1 + CN_LEXER_LOOKUP_COUNT];
    Cn_Lexer_Blacklist blacklist;
} Cn_Lexer;

/**
 * Unwraps to the current token lexer is at, 
 * changes whenever cn_lexer_next_token is called.
 */
#define cn_lexer_token(lexer)   (lexer)->queue[(lexer)->current_idx]

#define cn_lexer_head(lexer)    (lexer)->queue[(lexer)->head_idx]

#define cn_lexer_tail(lexer)    (lexer)->queue[(lexer)->tail_idx]

/**
 * Initializes lexer with speicific content.
 * Setting up queue and getting ready to output tokens.
 *
 * Setting blacklist is optional, but if set, tokens from that list won't appear in the lexed output.
 */
CNDEF void cn_lexer_init(Cn_Lexer *lexer, Cn_String content, Cn_Lexer_Blacklist blacklist);

/**
 * Advances lexer, grabs next token.
 */
CNDEF void cn_lexer_next_token(Cn_Lexer *lexer);

/**
 * Checks if current token matches expected type.
 * If it does returns true, otherwise false.
 */
CNDEF bool cn_lexer_expect(Cn_Lexer *lexer, Cn_Token_Type type);

/**
 * Peeks token based on its offset from current token.
 * offset == 0 will return current token.
 * offset < 0 will result peeking past tokens.
 * 
 * IMPORTANT: Expects -CN_LEXER_LOOKDOWN_COUNT < offset < CN_LEXER_LOOKUP_COUNT.
 *
 * RETURNS: CN_TOKEN_UNKNOWN if requested is out of queue bounds. 
 * CN_TOKEN_EOF if requested token comes after already found CN_TOKEN_EOF.
 */
CNDEF Cn_Token cn_lexer_peek(Cn_Lexer *lexer, int64_t offset);

/**
 * RETURNS: Static string of the token type. Used in messages.
 */
CNDEF const char *cn_token_kind_name(Cn_Token_Type type);

// TYPE SECTION

typedef enum : uint8_t {
    CN_TYPE_COMPLETE = 0x1,

    CN_TYPE_QUALIFIED_CONSTANT = 0x2,
    CN_TYPE_QUALIFIED_RESTRICT = 0x4,
    CN_TYPE_QUALIFIED_VOLATILE = 0x8,
    CN_TYPE_QUALIFIED_ATOMIC   = 0x10,
} Cn_Type_Flags;

typedef enum : uint8_t {
    CN_INTEGER,
    CN_FLOAT,
    CN_BOOL,
    CN_POINTER,
    CN_FUNCTION,
    CN_ARRAY,
    CN_VOID,
    CN_STRUCT,
    CN_UNION,
    CN_ENUM,
    CN_QUALIFIED,
    CN_OPAQUE,
    CN_UNKNOWN,
} Cn_Type_Kind;

#define CN_TYPE_BASE_MEMBERS     \
    Cn_Type_Kind kind;      \
    Cn_Type_Flags flags;    \
    int64_t size;           \
    int64_t align;

typedef struct {
    CN_TYPE_BASE_MEMBERS
} Cn_Type_Base;

#define CN_TYPE_BASE                        \
    union {                                 \
        Cn_Type_Base base;                  \
        struct { CN_TYPE_BASE_MEMBERS };    \
    }

typedef union cn_type Cn_Type;

typedef struct { CN_TYPE_BASE;
    bool is_signed;
} Cn_Type_Integer;

typedef struct { CN_TYPE_BASE;
    Cn_Type *ptr_to;
} Cn_Type_Pointer;

typedef struct {
    Cn_Type *type;
} Cn_Type_Function_Param;

typedef struct { CN_TYPE_BASE;
    Cn_Type *return_type;

    int64_t params_length;
    Cn_Type_Function_Param *params;

    bool is_variadic;
} Cn_Type_Function;

typedef struct {
    Cn_Type *type;
    Cn_String name;
    int64_t offset;
} Cn_Type_Struct_Member;

typedef struct { CN_TYPE_BASE;
    Cn_String tag;
    
    int64_t members_length;
    Cn_Type_Struct_Member *members;
} Cn_Type_Struct;

typedef struct {
    Cn_Type *type;
    Cn_String name;
} Cn_Type_Union_Member;

typedef struct { CN_TYPE_BASE;
    Cn_String tag;
    
    int64_t members_length;
    Cn_Type_Union_Member *members;
} Cn_Type_Union;

typedef struct { CN_TYPE_BASE;
    Cn_Type *element_type;
    int64_t length;
} Cn_Type_Array;

typedef struct {
    Cn_String name;
    int64_t value;
} Cn_Type_Enum_Member;

typedef struct { CN_TYPE_BASE;
    Cn_String tag;

    Cn_Type *member_type;

    int64_t members_length;
    Cn_Type_Enum_Member *members;
} Cn_Type_Enum;

typedef struct { CN_TYPE_BASE;
    Cn_Type *base_type;
} Cn_Type_Qualified;

union cn_type {
    CN_TYPE_BASE;
    Cn_Type_Integer   integer;
    Cn_Type_Pointer   pointer;
    Cn_Type_Function  function;
    Cn_Type_Struct    struct_t;
    Cn_Type_Union     union_t;
    Cn_Type_Array     array;
    Cn_Type_Enum      enum_t;
    Cn_Type_Qualified qualified;
};

extern const Cn_Type CN_TYPE_INT;
extern const Cn_Type CN_TYPE_FLOAT;
extern const Cn_Type CN_TYPE_CHAR;
extern const Cn_Type CN_TYPE_VOID;
extern const Cn_Type CN_TYPE_PTRDIFF;
extern const Cn_Type CN_TYPE_SIZE;
extern const Cn_Type CN_TYPE_OPAQUE;

/**
 * RETURNS: Pointer type struct that points to the base type of specified type.
 */
CNDEF Cn_Type cn_type_make_pointer(const Cn_Type *type);

/**
 * RETURNS: Qualified type struct that points to the base type of specified type.
 */
CNDEF Cn_Type cn_type_make_qualified(Cn_Type_Flags flags, const Cn_Type *type);

/**
 * Checks if "type1" equals "type2".
 *
 * RETURNS: True if types are identical.
 */
CNDEF bool cn_type_equals(const Cn_Type *type1, const Cn_Type *type2);

/**
 * RETURNS: Hash of specified type.
 */
CNDEF uint64_t cn_type_hash(const Cn_Type *type);

/**
 * Converts type to string and copies contents to the buffer.
 * 
 * IMPORTANT: Buffer should be big enough to hold string, 
 * otherwise it will be cutoff short.
 *
 * RETURNS: Resulting string.
 */
CNDEF Cn_String cn_type_stringify(Cn_String buffer, const Cn_Type *type);

/**
 * Prints type to stderr, using cn_type_stringify.
 */
CNDEF void cn_type_print(const Cn_Type *type);

/**
 * RETURNS: True if type is constant.
 */
CNDEF bool cn_type_is_constant(const Cn_Type *type);

/**
 * RETURNS: Unqualified version of the type.
 */
CNDEF Cn_Type *cn_type_unqualified(const Cn_Type *type);

/**
 * RETURNS: True if integer or float including complex.
 */
CNDEF bool cn_type_is_arithmetic(const Cn_Type *type);

/**
 * RETURNS: True if thing with type from can be assigned 
 * to the thing with type to.
 */
CNDEF bool cn_type_is_assignable(const Cn_Type *to, const Cn_Type *from);

/**
 * RETURNS: True if types are stucturally compatible.
 */
CNDEF bool cn_type_is_compatible(const Cn_Type *a, const Cn_Type *b);

/**
 * RETURNS: True if types are stucturally compatible without qualifiers.
 */
CNDEF bool cn_type_is_compatible_no_qualifiers(const Cn_Type *a, const Cn_Type *b);

/**
 * RETURNS: Type that has a greater arithmetic rank between two types a and b.
 *
 * EXPECTS: a and b to be arithmetic types.
 */
CNDEF Cn_Type *cn_type_greatest_arithmetic_rank(const Cn_Type *a, const Cn_Type *b);

/**
 * RETURNS: True if type is arithmetic, pointer or bool.
 */
CNDEF bool cn_type_is_scalar(Cn_Type *type);

typedef struct {
    Cn_Type *type;
    void *data;
} Cn_Any;

#define CN_TYPE_SCALAR_MAX_SIZE 16

/**
 * Reads contents of supplied any, interprets it as integer.
 *
 * RETURNS: Integer value of any.
 */
CNDEF int64_t cn_any_read_int(Cn_Any any);

/**
 * Reads contents of supplied any, interprets it as floating point.
 *
 * RETURNS: Floating point value of any.
 */
CNDEF double cn_any_read_float(Cn_Any any);

/**
 * Writes value to contents of any, uses any's type to 
 * properly size the integer.
 *
 * RETURNS: Resulting any.
 */
CNDEF Cn_Any cn_any_write_int(Cn_Any any, int64_t value);

/**
 * Writes value to contents of any, uses any's type to 
 * properly size the floating point.
 *
 * RETURNS: Resulting any.
 */
CNDEF Cn_Any cn_any_write_float(Cn_Any any, double value);

/**
 * RETURNS: True if any is 0.
 */
CNDEF bool cn_any_is_zero(Cn_Any any);

/**
 * Converts any to double, including any scalar,
 * interpreting any other data as raw integer.
 *
 * NOTE: If supplied any is not a scalar type, 
 * it will still try to read it as raw int and convert to a double.
 *
 * RETURNS: Resulting double value.
 */
CNDEF double cn_any_as_double(Cn_Any any);

/**
 * Safe double to int64 used by casts. 
 * Rejects NaN and out of range values whose
 * conversion would be undefined.
 *
 * RETURNS: True on success, false on failure.
 */
CNDEF bool cn_double_to_int64(double d, int64_t *out);

/**
 * Converts an already evaluated scalar value to target type, 
 * can be used by casts. NIL when target or source isn't a
 * foldable scalar, or when a float to int conversion would be out of range.
 *
 * NOTE: Uses supplied buffer to store output.
 *
 * RETURNS: Any of the new converted value on success, NIL on failure.
 */
CNDEF Cn_Any cn_any_convert(Cn_Any src, Cn_Type *target, void *buffer);

/**
 * Checks whether the any value is empty,
 * meaning it's data and type are both NULL.
 *
 * RETURNS: True if any is empty and false otherwise.
 */
CNDEF bool cn_any_is_empty(Cn_Any any);

// AST SECTION

typedef uint32_t Cn_Ast_Idx;

#define CN_AST_NIL_IDX      0
#define CN_AST_ERROR_IDX    1

typedef enum : uint8_t {
    CN_AST_TYPE_QUALIFIER_CONST         = 0x01,
    CN_AST_TYPE_QUALIFIER_RESTRICT      = 0x02,
    CN_AST_TYPE_QUALIFIER_VOLATILE      = 0x04,
    CN_AST_TYPE_QUALIFIER_ATOMIC        = 0x08,
} Cn_Qualifier_Flags;

CNDEF Cn_Type_Flags cn_ast_qualifier_flags_to_type(Cn_Qualifier_Flags flags);

typedef enum : uint8_t {
    CN_AST_FUNCTION_SPECIFIER_INLINE    = 0x01,
    CN_AST_FUNCTION_SPECIFIER_NORETURN  = 0x02,
} Cn_Function_Specifier_Flags;

typedef enum : uint8_t {
    CN_STORAGE_SPECIFIER_STATIC         = 0x01,
    CN_STORAGE_SPECIFIER_EXTERN         = 0x02,
    CN_STORAGE_SPECIFIER_REGISTER       = 0x04,
    CN_STORAGE_SPECIFIER_AUTO           = 0x08,
    CN_STORAGE_SPECIFIER_TYPEDEF        = 0x10,
} Cn_Storage_Specifier_Flags;

typedef enum : uint8_t {
    CN_AST_TYPE_SIGN_NONE = 0,
    CN_AST_TYPE_SIGN_SIGNED,
    CN_AST_TYPE_SIGN_UNSIGNED,
} Cn_Ast_Type_Sign;

typedef enum : uint8_t {
    CN_AST_TYPE_WIDTH_NONE = 0,
    CN_AST_TYPE_WIDTH_SHORT,
    CN_AST_TYPE_WIDTH_LONG,
    CN_AST_TYPE_WIDTH_LONG_LONG,
} Cn_Ast_Type_Width;

typedef enum : uint8_t {
    CN_AST_TYPE_NONE    = 0,
    CN_AST_TYPE_INT     = 1,
    CN_AST_TYPE_CHAR    = 2,
    CN_AST_TYPE_FLOAT   = 3,
    CN_AST_TYPE_DOUBLE  = 4,
    CN_AST_TYPE_BOOL    = 5,
    CN_AST_TYPE_VOID    = 6,
} Cn_Ast_Type_Kind;

static const Cn_Token_Type CN_AST_TYPE_KINDS[] = {
    CN_TOKEN_UNKNOWN,
    CN_TOKEN_INT,
    CN_TOKEN_CHAR,
    CN_TOKEN_FLOAT,
    CN_TOKEN_DOUBLE,
    CN_TOKEN_BOOL,
    CN_TOKEN_VOID,
};

/**
 *  binary_operator
 *          : '['
 *          | '('
 *          | '.'
 *          | '->'
 *          | '*'
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
 *          | ','
 *          ;
 */
typedef enum : int8_t {
    CN_BINARY_OP_NONE           = -1,
    CN_BINARY_OP_ARRAY_SUB      = 0,
    CN_BINARY_OP_FUNCTION       = 1,
    CN_BINARY_OP_DOT            = 2,
    CN_BINARY_OP_ARROW          = 3,
    CN_BINARY_OP_MULTIPLICATION = 4,
    CN_BINARY_OP_DIVISION       = 5,
    CN_BINARY_OP_MODULO         = 6,
    CN_BINARY_OP_ADDITION       = 7,
    CN_BINARY_OP_SUBTRACTION    = 8,
    CN_BINARY_OP_LSHIFT         = 9,
    CN_BINARY_OP_RSHIFT         = 10,
    CN_BINARY_OP_LESS           = 11,
    CN_BINARY_OP_LESS_EQ        = 12,
    CN_BINARY_OP_GREATER        = 13,
    CN_BINARY_OP_GREATER_EQ     = 14,
    CN_BINARY_OP_EQ             = 15,
    CN_BINARY_OP_NOT_EQ         = 16,
    CN_BINARY_OP_BIT_AND        = 17,
    CN_BINARY_OP_BIT_XOR        = 18,
    CN_BINARY_OP_BIT_OR         = 19,
    CN_BINARY_OP_AND            = 20,
    CN_BINARY_OP_OR             = 21,
    CN_BINARY_OP_COMMA          = 22,
} Cn_Binary_Operator_Kind;

typedef struct {
    Cn_Binary_Operator_Kind kind;   // Kind of operator.
    Cn_Token_Type token_identifier; // Token which identifies binary operator.
    int precedence;                 // Precedence.
} Cn_Binary_Operator;

/**
 * Checks table if current token is binary operator, 
 * returns it's kind, if not CN_BINARY_OP_NONE.
 */
CNDEF Cn_Binary_Operator_Kind cn_ast_is_binary_operator(Cn_Lexer *lexer);

/**
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
 */
#define CN_UNARY_OPERATOR_PRECEDENCE 13

typedef enum : int8_t {
    CN_UNARY_OP_NONE           = -1,
    CN_UNARY_OP_INCREMENT      = 0,
    CN_UNARY_OP_DECREMENT      = 1,
    CN_UNARY_OP_POSITIVE       = 2,
    CN_UNARY_OP_NEGATIVE       = 3,
    CN_UNARY_OP_NOT            = 4,
    CN_UNARY_OP_BIT_NOT        = 5,
    CN_UNARY_OP_DEREF          = 6,
    CN_UNARY_OP_ADDROF         = 7,
} Cn_Unary_Operator_Kind;

typedef struct {
    Cn_Unary_Operator_Kind kind;         // Kind of operator.
    Cn_Token_Type token_identifier;      // Token which identifies unary operator, if such present.
} Cn_Unary_Operator;

/**
 * Checks table if current token is unary operator, 
 * returns it's kind, if not CN_UNARY_OP_NONE.
 */
CNDEF Cn_Unary_Operator_Kind cn_ast_is_unary_operator(Cn_Lexer *lexer);

/**
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
 */
#define CN_ASSIGNMENT_OPERATOR_PRECEDENCE 1

typedef enum : int8_t {
    CN_ASSIGNMENT_OP_NONE           = -1,
    CN_ASSIGNMENT_OP_ASSIGN         = 0,
    CN_ASSIGNMENT_OP_MULTIPLY       = 1,
    CN_ASSIGNMENT_OP_DIVIDE         = 2,
    CN_ASSIGNMENT_OP_MODULO         = 3,
    CN_ASSIGNMENT_OP_PLUS           = 4,
    CN_ASSIGNMENT_OP_MINUS          = 5,
    CN_ASSIGNMENT_OP_LSHIFT         = 6,
    CN_ASSIGNMENT_OP_RSHIFT         = 7,
    CN_ASSIGNMENT_OP_BIT_AND        = 8,
    CN_ASSIGNMENT_OP_BIT_XOR        = 9,
    CN_ASSIGNMENT_OP_BIT_OR         = 10,
} Cn_Assignment_Operator_Kind;

typedef struct {
    Cn_Assignment_Operator_Kind kind;    // Kind of operator.
    Cn_Token_Type token_identifier;      // Token which identifies assignment operator, if such present.
} Cn_Assignment_Operator;

/**
 * Checks table if current token is assignment operator, 
 * returns it's kind, if not CN_ASSIGNMENT_OP_NONE.
 */
CNDEF Cn_Assignment_Operator_Kind cn_ast_is_assignment_operator(Cn_Lexer *lexer);

/**
 *  postfix_operator
 *          : '++'
 *          | '--'
 *          ;
 */
#define CN_POSTFIX_OPERATOR_PRECEDENCE 14

typedef enum : int8_t {
    CN_POSTFIX_OP_NONE              = -1,
    CN_POSTFIX_OP_INCREMENT         = 0,
    CN_POSTFIX_OP_DECREMENT         = 1,
} Cn_Postfix_Operator_Kind;

typedef struct {
    Cn_Postfix_Operator_Kind kind;       // Kind of operator.
    Cn_Token_Type token_identifier;      // Token which identifies postfix operator, if such present.
} Cn_Postfix_Operator;

/**
 * Checks table if current token is postfix operator, 
 * returns it's kind, if not CN_POSTFIX_OP_NONE.
 */
CNDEF Cn_Postfix_Operator_Kind cn_ast_is_postfix_operator(Cn_Lexer *lexer);

#define CN_AST_GEN_LIST(X)\
    /* --- misc ----------------------------------------------------------- */                              \
    X(CODE,                             Code,                               code)                           \
    X(TRANSLATION_UNIT,                 Translation_Unit,                   translation_unit)               \
    X(EXTERNAL_DECLARATION,             External_Declaration,               external_declaration)           \
    X(DECLARATION,                      Declaration,                        declaration)                    \
    X(FUNCTION,                         Function,                           function)                       \
    /* --- statements ----------------------------------------------------- */                              \
    X(BLOCK,                            Block,                              block)                          \
    X(IF,                               If,                                 if_statement)                   \
    X(SWITCH,                           Switch,                             switch_statement)               \
    X(WHILE,                            While,                              while_statement)                \
    X(DO_WHILE,                         Do_While,                           do_while)                       \
    X(FOR,                              For,                                for_statement)                  \
    /* X(LABEL,                         Label,                              label)           TODO */        \
    X(GOTO,                             Goto,                               goto_statement)                 \
    X(RETURN,                           Return,                             return_statement)               \
    X(EXPRESSION_STATEMENT,             Expression_Statement,               expression_statement)           \
    /* --- expressions ---------------------------------------------------- */                              \
    X(BINARY,                           Binary,                             binary)                         \
    X(ACCESS,                           Access,                             access)                         \
    X(CALL,                             Call,                               call)                           \
    X(UNARY,                            Unary,                              unary)                          \
    X(CAST,                             Cast,                               cast)                           \
    X(COMPOUND,                         Compound,                           compound)                       \
    X(SIZEOF,                           Sizeof,                             sizeof_expression)              \
    X(TERNARY,                          Ternary,                            ternary)                        \
    X(ASSIGN,                           Assign,                             assign)                         \
    X(POSTFIX,                          Postfix,                            postfix)                        \
    X(PRIMARY,                          Primary,                            primary)                        \
    /* --- literals ------------------------------------------------------- */                              \
    X(IDENTIFIER,                       Identifier,                         identifier)                     \
    X(INTEGER,                          Integer,                            integer)                        \
    X(FLOAT,                            Float,                              flt)                            \
    X(STRING,                           String,                             string)                         \
    /* --- declarator machinery ------------------------------------------- */                              \
    X(INIT_DECLARATOR,                  Init_Declarator,                    init_declarator)                \
    X(INITIALIZER,                      Initializer,                        initializer)                    \
    X(DESIGNATION,                      Designation,                        designation)                    \
    X(DESIGNATOR,                       Designator,                         designator)                     \
    X(DECLARATOR,                       Declarator,                         declarator)                     \
    X(POINTER,                          Pointer,                            pointer)                        \
    X(DIRECT_DECLARATOR_GROUPED,        Direct_Declarator_Grouped,          direct_declarator_grouped)      \
    X(DIRECT_DECLARATOR_ARRAY,          Direct_Declarator_Array,            direct_declarator_array)        \
    X(DIRECT_DECLARATOR_FUNCTION,       Direct_Declarator_Function,         direct_declarator_function)     \
    X(DECLARATION_SPECIFIERS,           Declaration_Specifiers,             declaration_specifiers)         \
    X(GNU_TYPEOF,                       Gnu_Typeof,                         gnu_typeof)                     \
    X(TYPE_SPECIFIER_PRIMITIVE,         Type_Specifier_Primitive,           type_specifier_primitive)       \
    X(TYPE_SPECIFIER_TYPEDEF,           Type_Specifier_Typedef,             type_specifier_typedef)         \
    X(TYPE_NAME,                        Type_Name,                          type_name)                      \
    X(SPECIFIER_QUALIFIER,              Specifier_Qualifier,                specifier_qualifier)            \
    X(PARAMETER_DECLARATION,            Parameter_Declaration,              parameter_declaration)          \
    /* --- tag types ------------------------------------------------------ */                              \
    X(STRUCT_SPECIFIER,                 Struct_Specifier,                   struct_specifier)               \
    X(UNION_SPECIFIER,                  Union_Specifier,                    union_specifier)                \
    X(MEMBER_DECLARATION,               Member_Declaration,                 member_declaration)             \
    X(MEMBER_DECLARATOR,                Member_Declarator,                  member_declarator)              \
    X(ENUM_SPECIFIER,                   Enum_Specifier,                     enum_specifier)                 \
    X(ENUMERATOR,                       Enumerator,                         enumerator)                     \
    /* --- attributes ----------------------------------------------------- */                              \
    X(ATTRIBUTE_SPECIFIER,              Attribute_Specifier,                attribute_specifier)            \
    X(ATTRIBUTE,                        Attribute,                          attribute)                      \
    X(GNU_ATTRIBUTE_SPECIFIER,          Gnu_Attribute_Specifier,            gnu_attribute_specifier)        \
    X(GNU_ATTRIBUTE,                    Gnu_Attribute,                      gnu_attribute)                  \
    X(GNU_ASM_LABEL,                    Gnu_Asm_Label,                      gnu_asm_label)
 
/**
 * Kinds that carry no payload beyond the base node. No struct, no cast —
 * just an enum value and a debug name.
 */
#define CN_AST_GEN_TAG_LIST(X) \
    X(BREAK)                   \
    X(CONTINUE)

typedef enum : uint8_t {
    CN_AST_UNKNOWN = 0,
    CN_AST_ERROR   = 1,
#define X(K, T, m) CN_AST_##K,
    CN_AST_GEN_LIST(X)
#undef X
#define X(K) CN_AST_##K,
    CN_AST_GEN_TAG_LIST(X)
#undef X
} Cn_Ast_Kind;

typedef enum {
    CN_AST_IS_REPLACED                  = 0x1,
    CN_AST_USE_REPLACED                 = 0x2,
    CN_AST_SYNTHETIC                    = 0x4,

    CN_AST_EXTERNAL_DECLARATION_HAS_EXTENSION     = 0x100,
    CN_AST_DECLARATOR_IS_ABSTRACT                 = 0x100,
    CN_AST_ACCESS_IS_POINTER                      = 0x100,
    CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC = 0x100,
    CN_AST_ENUM_HAS_DEFINITION                    = 0x100,
    CN_AST_STRUCT_OR_UNION_HAS_DEFINITION         = 0x100,
    CN_AST_DECLARATION_IS_EMPTY                   = 0x100,
} Cn_Ast_Flags;

#define CN_AST_BASE_MEMBERS         \
    Cn_Ast_Kind     kind;           \
    Cn_Ast_Flags    flags;          \
    Cn_Location     loc;            \
    Cn_Source       src;            \
    Cn_Ast_Idx      replaced_idx;   \
    Cn_Ast_Idx      parent_idx;

typedef struct {
    CN_AST_BASE_MEMBERS
} Cn_Ast_Base;

#define CN_AST_BASE                         \
    union {                                 \
        Cn_Ast_Base base;                   \
        struct { CN_AST_BASE_MEMBERS };     \
    }

typedef struct {
    Cn_Ast_Idx *idxs;
    int64_t     length;
} Cn_Ast_List;

typedef struct { CN_AST_BASE;
    Cn_String text;
} Cn_Ast_Code;

typedef struct { CN_AST_BASE;
    Cn_Ast_List external_declarations;
} Cn_Ast_Translation_Unit;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx child_idx;
} Cn_Ast_External_Declaration;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx  declaration_specifiers_idx;
    Cn_Ast_List init_declarators;
    Cn_Ast_List gnu_attribute_specifiers;
} Cn_Ast_Declaration;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx  declaration_specifiers_idx;
    Cn_Ast_Idx  declarator_idx;
    Cn_Ast_Idx  block_idx;
} Cn_Ast_Function;

typedef struct { CN_AST_BASE;
    Cn_Ast_List statements; // Statements or declarations in order.
} Cn_Ast_Block;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx then_idx;
    Cn_Ast_Idx else_idx; // Can be NIL.
} Cn_Ast_If;
 
typedef struct { CN_AST_BASE;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx body_idx;
} Cn_Ast_Switch;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx body_idx;
} Cn_Ast_While;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx body_idx;
} Cn_Ast_Do_While;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx body_idx;
} Cn_Ast_For;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx identifier_idx; // Label.
} Cn_Ast_Goto;
 
typedef struct { CN_AST_BASE;
    Cn_Ast_Idx expression_idx; // Can be NIL.
} Cn_Ast_Return;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx expression_idx;
} Cn_Ast_Expression_Statement;

typedef struct { CN_AST_BASE;
    Cn_Type *               type;
    Cn_Ast_Idx              left_idx;
    Cn_Binary_Operator_Kind operator;
    Cn_Ast_Idx              right_idx;
} Cn_Ast_Binary;

typedef struct { CN_AST_BASE;
    Cn_Type *  type;
    Cn_Ast_Idx expression_idx;
    Cn_Ast_Idx member_idx;
    bool pointer;
} Cn_Ast_Access;

typedef struct { CN_AST_BASE;
    Cn_Type *   type;
    Cn_Ast_Idx  expression_idx;
    Cn_Ast_List arguments;
} Cn_Ast_Call;

typedef struct { CN_AST_BASE;
    Cn_Type *              type;
    Cn_Unary_Operator_Kind operator;
    Cn_Ast_Idx             expression_idx;
} Cn_Ast_Unary;

typedef struct { CN_AST_BASE;
    Cn_Type *  type;
    Cn_Ast_Idx type_name_idx;
    Cn_Ast_Idx expression_idx;
} Cn_Ast_Cast;

typedef struct { CN_AST_BASE;
    Cn_Type *   type;
    Cn_Ast_Idx  type_name_idx;
    Cn_Ast_List designations;
} Cn_Ast_Compound;

typedef struct { CN_AST_BASE;
    Cn_Type *  type;
    Cn_Ast_Idx target_idx; // Could be expression or type name.
} Cn_Ast_Sizeof;

typedef struct { CN_AST_BASE;
    Cn_Type *  type;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx true_idx;
    Cn_Ast_Idx false_idx;
} Cn_Ast_Ternary;

typedef struct { CN_AST_BASE;
    Cn_Type *                   type;
    Cn_Ast_Idx                  left_idx;
    Cn_Assignment_Operator_Kind operator;
    Cn_Ast_Idx                  right_idx;
} Cn_Ast_Assign;

typedef struct { CN_AST_BASE;
    Cn_Type *                type;
    Cn_Ast_Idx               expression_idx;
    Cn_Postfix_Operator_Kind operator;
} Cn_Ast_Postfix;

typedef struct { CN_AST_BASE;
    Cn_Type *  type;
    Cn_Ast_Idx literal_idx; // Identifier, integer, float or string.
} Cn_Ast_Primary;

typedef struct { CN_AST_BASE;
    Cn_String name;
} Cn_Ast_Identifier;

typedef struct { CN_AST_BASE;
    Cn_String value;
} Cn_Ast_Integer;

typedef struct { CN_AST_BASE;
    Cn_String value;
} Cn_Ast_Float;

typedef struct { CN_AST_BASE;
    Cn_String str;
} Cn_Ast_String;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx declarator_idx;
    Cn_Ast_Idx initializer_idx;
    Cn_Ast_Idx gnu_asm_label_idx;
} Cn_Ast_Init_Declarator;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx expression_idx;  // Can be NIL if not single expression initializer.
    Cn_Ast_List designations;   // Can be empty if not compound intializer '{' '}'.
} Cn_Ast_Initializer;

typedef struct { CN_AST_BASE;
    Cn_Ast_List designators;    // Can be empty.
    Cn_Ast_Idx initializer_idx;
} Cn_Ast_Designation;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx identifier_idx;              // Can be NIL if not member designator.
    Cn_Ast_Idx expression_idx;              // Can be NIL if not array designator.
    Cn_Ast_Idx expression_range_end_idx;    // Can be NIL if not ragne.
} Cn_Ast_Designator;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx pointer_idx;             // Can be NIL if declarator is not wrapped with pointer(s).
    Cn_Ast_Idx direct_declarator_idx;   // Cab be NIL if declarator is abstract.
} Cn_Ast_Declarator;

typedef struct { CN_AST_BASE;
    Cn_Qualifier_Flags qualifiers;
    Cn_Ast_Idx         pointer_idx;     // Can be NIL.
} Cn_Ast_Pointer;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx declarator_idx;
} Cn_Ast_Direct_Declarator_Grouped;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx direct_declarator_idx;   // Can be NIL, if abstract.
    Cn_Ast_Idx expression_idx;
} Cn_Ast_Direct_Declarator_Array;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  direct_declarator_idx;
    Cn_Ast_List parameter_declarations;
} Cn_Ast_Direct_Declarator_Function;

typedef struct { CN_AST_BASE;
    Cn_Storage_Specifier_Flags  storage_specifiers;
    Cn_Qualifier_Flags          qualifiers;
    Cn_Function_Specifier_Flags function_specifiers;
    Cn_Ast_List                 gnu_attribute_specifiers;
    Cn_Ast_Idx                  type_specifier_idx;
} Cn_Ast_Declaration_Specifiers;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx target_idx; // Could be expression or type name.
} Cn_Ast_Gnu_Typeof;

typedef struct { CN_AST_BASE;
    Cn_Ast_Type_Kind  primitive_kind;
    Cn_Ast_Type_Width width;
    Cn_Ast_Type_Sign  sign;
} Cn_Ast_Type_Specifier_Primitive;

typedef struct { CN_AST_BASE;
    Cn_String typedef_name;
} Cn_Ast_Type_Specifier_Typedef;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx specifier_qualifier_idx;
    Cn_Ast_Idx abstract_declarator_idx;
} Cn_Ast_Type_Name;

typedef struct { CN_AST_BASE;
    Cn_Qualifier_Flags qualifiers;
    Cn_Ast_Idx         type_specifier_idx;
} Cn_Ast_Specifier_Qualifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx declaration_specifiers_idx;
    Cn_Ast_Idx declarator_idx;
} Cn_Ast_Parameter_Declaration;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List member_declarations;
} Cn_Ast_Struct_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List member_declarations;
} Cn_Ast_Union_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  specifier_qualifier_idx;
    Cn_Ast_List member_declarators;
} Cn_Ast_Member_Declaration;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx declarator_idx;
    Cn_Ast_Idx bitfield_idx;
} Cn_Ast_Member_Declarator;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_List gnu_attribute_specifiers;
    Cn_Ast_Idx  specifier_qualifier_idx;
    Cn_Ast_List enumerators;
} Cn_Ast_Enum_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_List gnu_attribute_specifiers;
    Cn_Ast_Idx  expression_idx;
} Cn_Ast_Enumerator;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attributes;
} Cn_Ast_Attribute_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  vendor_identifier_idx;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List arguments;
} Cn_Ast_Attribute;

typedef struct { CN_AST_BASE;
    Cn_Ast_List gnu_attributes;
} Cn_Ast_Gnu_Attribute_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List arguments;
} Cn_Ast_Gnu_Attribute;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx string_idx;
} Cn_Ast_Gnu_Asm_Label;

typedef union {
    CN_AST_BASE;
#define X(K, T, m) Cn_Ast_##T m;
    CN_AST_GEN_LIST(X)
#undef X
} Cn_Ast_Node;

#ifndef CN_AST_NODE_LIST_INITIAL_CAP
#   define CN_AST_NODE_LIST_INITIAL_CAP 64
#endif // CN_AST_NODE_LIST_INITIAL_CAP

#define CN_AST_TYPE_ARENA_BLOCK_CAP             (sizeof(Cn_Type) * 64)
#define CN_AST_TYPE_PTR_SET_INITIAL_CAP         32
#define CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP    4096

typedef uint32_t Cn_Ast_Binding_Idx;

#define CN_AST_NIL_BINDING_IDX 0

typedef enum : uint8_t {
    CN_BINDING_TAG,
    CN_BINDING_VARIABLE,
    CN_BINDING_FUCNTION,
    CN_BINDING_TYPEDEF,
    CN_BINDING_ENUM_CONSTANT,
} Cn_Ast_Binding_Kind;

typedef struct {
    Cn_Storage_Specifier_Flags storage_flags;
    bool is_definition;
} Cn_Ast_Binding_Variable;

typedef struct {
    Cn_Storage_Specifier_Flags storage_flags;
    Cn_Function_Specifier_Flags function_flags;
    Cn_String *parameter_names;
    bool is_definition;
} Cn_Ast_Binding_Function;

typedef struct {
    int64_t value; 
} Cn_Ast_Binding_Enum_Constant;

typedef struct {
    /**
     * Kind of the binding, different kinds can be stored and modified differently.
     */
    Cn_Ast_Binding_Kind kind;           
    /**
     * Name of the binding is a single keyword that identifies it,
     * could be a variable name, typedef name or tag name.
     * Depends on the kind of the binding.
     */
    Cn_String name;
    /**
     * Which ast node binding is coming from, usually identifier.
     */
    Cn_Ast_Idx src;
    /**
     * Resolved type of the binding.
     */
    Cn_Type *type;
    /**
     * Each binding can "shadow" previous binding,
     * therefore if such case appears most recent binding
     * will be at the head of linked list.
     */
    Cn_Ast_Binding_Idx next_idx;        
    /**
     * Idx of a scope in which this binding was added.
     * IMPORTANT: Binding should not be used if it's scope doesn't exist.
     */
    int64_t scope_idx;                  

    /**
     * Additional per binding kind data, like storage flags, parameter names,
     * whether it is definiton or not and so on.
     */
    union {
        Cn_Ast_Binding_Variable         b_variable;
        Cn_Ast_Binding_Function         b_function;
        Cn_Ast_Binding_Enum_Constant    b_enum_constant;
    };

} Cn_Ast_Binding;

#define CN_AST_BINDING_LIST_INITIAL_CAP             64
#define CN_AST_TAG_BINDING_TABLE_INITIAL_CAP        32
#define CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP     32

#define CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP       4096
#define CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP    4096

/**
 * This struct simply contains information that each scope will have.
 */
typedef struct {
    int64_t saved_strings_length;
    int64_t binding_idx;

    /**
     * If set to true, means that there is checkpoint set in this scope.
     * Therefore it cannot be removed, because doing so would invalidate 
     * checkpoint saved state.
     *
     * IMPORTANT: This is a workaround to not store massive deep copies of 
     * scoped data structures like bindings hash tables, or scoped string arenas.
     * It assumes that ast functionality won't require processor to 
     * jump into a completely different scope that no longer exists.
     * In other words all jump can only occure in from current to previous 
     * active scopes.
     *
     * If locked scope is getting removed error will occur.
     */
    bool    is_checkpoint_locked;
} Cn_Ast_Scope;

#define CN_AST_SCOPE_STACK_INITIAL_CAP 32
#define CN_AST_IDX_STACK_INITIAL_CAP   128

typedef struct {
    /**
     * Source string of the code being parsed.
     */
    Cn_String source;
    /**
     * Stores all nodes in growing array list.
     *
     * IMPORTANT: Access elements by indicies, 
     * so there are no unsafe situations occuring.
     * First element is considered NIL element, 
     * it is reserved to identify illegal references.
     */
    Cn_Ast_Node *node_list;
    /**
     * Type arena used, to store type structs.
     * So that pointers to types never change.
     */
    Cn_Chained_Arena type_arena;
    /**
     * Set of pointers to the types is used, 
     * to deduplicate structural types, whenever such type is found
     * it is inserted into this set if it is not already there.
     */
    Cn_Type **type_ptr_set;
    /**
     * This arena is used to store various type children.
     * For example: struct members, function parameters, etc.
     * Since each type has its own array of members, 
     * which doesn't overlap, all of them can be stored in the single arena.
     */
    Cn_Chained_Arena type_children_arena;
    /**
     * Stores all bindings in growing array list.
     *
     * IMPORTANT: Access elements by indicies, 
     * so there are no unsafe situations occuring.
     * First element is considered NIL element, 
     * it is reserved to identify illegal references.
     */
    Cn_Ast_Binding *binding_list;
    /**
     * Tag table use tag names of struct, 
     * enum, union as keys and bindings as values.
     */
    Cn_Ast_Binding_Idx *tag_binding_table;
    /**
     * Symbol table acts similar to the tag table except. 
     * It contains: variables, functions, typedefs, enum constants.
     */
    Cn_Ast_Binding_Idx *symbol_binding_table;
    /**
     * Scoped strings arena is used to store every scoped string data.
     * Pointers to the data remain the same throughout the execution.
     * since chained arena gurantees that memory once allocated is not moved.
     * Deallocation mechanism is used in the arena to "pop" string data once
     * it is out of scope.
     *
     * Used for binding names, such as:
     * Functions, Variables, Typedefs, Enums.
     */
    Cn_Chained_Arena scoped_strings_arena;
    /**
     * Permanent strings arena is used to store strings data that lives 
     * globally outside of source. Pointers to the data remain the same 
     * throughout the execution. since chained arena gurantees that memory 
     * once allocated is not moved.
     *
     * Used for type names, persistent names:
     * Struct members, Tags:
     */
    Cn_Chained_Arena permanent_strings_arena;
    /**
     * Regular array list used as a stack to properly handle scoping,
     * throughout parsing.
     */
    Cn_Ast_Scope *scope_stack;
    /**
     * Stack of Cn_Ast_Idx used during parsing to build Cn_Ast_List arrays.
     * Parsing functions push indices onto this stack, then finalize ranges
     * into Cn_Ast_List when the list is complete.
     */
    Cn_Ast_Idx *idx_stack;
    /**
     * Error and warning count, is incremented by cn_diagnostic,
     * every time error or warning is reported.
     */
    int64_t error_count, warning_count;
    uint64_t counter;
} Cn_Ast_Data;

extern Cn_Ast_Data *cn__ast_data;

typedef enum {
    CN_AST_CHECKPOINT_IGNORE_AST_NODES = 0x1,
} Cn_Ast_Checkpoint_Flags;

typedef struct {
    Cn_Ast_Checkpoint_Flags flags;
    jmp_buf                 jmpbuf;
    int64_t                 saved_node_length;
    uint64_t                saved_type_length;
    uint64_t                saved_type_children_length;
    int64_t                 saved_binding_length;
    uint64_t                saved_scoped_strings_length;
    uint64_t                saved_permanent_strings_length;
    int64_t                 saved_scope_length;
    int64_t                 saved_idx_stack_length;
    uint64_t                saved_counter;
    Cn_Ast_Data *           data;
} Cn_Ast_Checkpoint;

/**
 * Uses current data from cn__ast_data to store current ast state 
 * in checkpoint. Wrapping around setjump to also store stack state.
 *
 * IMPORTANT: Locks scope checkpoint is set in.
 *
 * RETURNS: True if program jumped to this code point. 
 * False if checkpoint set and no jumping or loading occured.
 */
#define cn_ast_checkpoint_set(checkpoint_ptr, data_ptr, flags) (cn__ast_checkpoint_save(checkpoint_ptr, data_ptr, flags), setjmp((checkpoint_ptr)->jmpbuf) != 0)

CNDEF void cn__ast_checkpoint_save(Cn_Ast_Checkpoint *checkpoint, Cn_Ast_Data *data, Cn_Ast_Checkpoint_Flags flags);

/**
 * Loads state saved in checkpoint and long jumps to the 
 * set checkpoint. Modifies data stored in cn__ast_data.
 */
CNDEF void cn_ast_checkpoint_load(Cn_Ast_Checkpoint *checkpoint);

/**
 * Removes and nullifies saved state in checkpoint.
 *
 * IMPORTANT: Unlocks scope checkpoint was set in.
 */
CNDEF void cn_ast_checkpoint_remove(Cn_Ast_Checkpoint *checkpoint);

/**
 * RETURNS: Next fresh counter value, and increments counter.
 */
#define cn_ast_counter_next() (cn__ast_data->counter++)

/**
 * Inits ast functionality, called once before parsing begins.
 */
CNDEF int cn_ast_init(Cn_Ast_Data *data);

/**
 * Frees all memory occupied by ast data, 
 * this will invalidate everything that was parsed, 
 * analyzed and stored while working with the ast tree, 
 * including every string and name.
 */
CNDEF void cn_ast_free(Cn_Ast_Data *data);

/**
 * Function to get ast node based on its idx.
 * IMPORTANT: If flags USE_REPLACED set, it will return replaced node.
 */
CNDEF void *cn_ast_get(Cn_Ast_Idx idx);

#define cn_ast_get_as_node(idx) ((Cn_Ast_Node *)cn_ast_get(idx))

/**
 * Simple macro to get ast idx based on pointer to the node.
 */
#define cn_ast_idx_get(node)    ((Cn_Ast_Node *)(node) - cn__ast_data->node_list)

/**
 * Returns current idx_stack length, used to mark start of a list.
 */
#define cn_ast_idx_stack_mark() cn_array_list_length(&cn__ast_data->idx_stack)

/**
 * Pushes an idx onto the idx_stack.
 */
#define cn_ast_idx_stack_push(idx) cn_array_list_append(&cn__ast_data->idx_stack, (idx))

/**
 * Finalizes a range of the idx_stack (from mark to end) into a Cn_Ast_List.
 * Pops the finalized indices from the stack.
 */
CNDEF Cn_Ast_List cn_ast_idx_stack_finalize(int64_t mark);

/**
 * Sets parent of all idx's in the list to parent_idx.
 */
CNDEF void cn_ast_list_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_List *list);

/**
 * Sets parent of all idx's supplied to parent_idx.
 */
#define cn_ast_node_set_parent(parent_idx, ...) cn__ast_node_set_parent(parent_idx, (Cn_Ast_Idx []) { __VA_ARGS__ }, sizeof((Cn_Ast_Idx []) { __VA_ARGS__ }) / sizeof(Cn_Ast_Idx))

CNDEF void cn__ast_node_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_Idx idxs[], size_t length);

/**
 * Recursivly prints ast tree to stdout.
 * IMPORTANT: Use starting depth as 0.
 */
CNDEF void cn_ast_print(Cn_Ast_Idx idx, int depth);

/**
 * RETURNS: Static string with the enum name of the node kind. Used in messages.
 */
CNDEF const char *cn_ast_node_kind_name(Cn_Ast_Kind kind);

typedef struct {
    /**
     * User data or context that user wants to preserve, share with every 
     * write call made by emitter.
     */
    void *ctx;
    /**
     * Maximum number of lines allowed to be printed before interupted.
     * If set to 0, will be ignored and continue 
     * printing till whole supplied AST tree is emitted.
     */
    int64_t max_lines;
    /**
     * Node that needs to be highlighted by emitter.
     * If set to NIL, is ignored, otherwise emitter
     * will output highlight info in highlight_offset 
     * and highlight_length in emitted text.
     * 
     * NOTE: If highlight_offset or highlight_length set to NULL those are 
     * ignored and not set even if highlight_idx is not NIL.
     */
    Cn_Ast_Idx highlight_idx;
    int64_t *highlight_offset;
    int64_t *highlight_length;
    /**
     * Total written length from the whole emit.
     * If specified is NULL, written_length is not outputted.
     */
    int64_t *written_length;
    /**
     * Indentation level spacing to be added.
     */
    int indent;
} Cn_Emit_Opt;

/**
 * Write function used by emitter.
 * Takes in string which is text sent to write.
 * And context which is any user data supplied.
 * ctx is directly copied from optional ctx, so when calling emitter user 
 * can specify their own ctx and use it however they want in the write call site.
 */
typedef void (Cn_Emit_Write)(Cn_String str, void *ctx);

CNDEF void cn_emit_write_file(Cn_String str, void *ctx);

CNDEF void cn_emit_write_sb(Cn_String str, void *ctx);

/**
 * Emits C source code from AST node to output.
 * Reconstructs valid C source from AST representation.
 *
 * RETURNS:
 *  -1      If error occured during printing.
 *  0       Success on printing whole tree.
 *  1       Interrupted on new line, (can be set via optional).
 */
#define cn_emit(node_idx, func, ...) cn_emit_opt(node_idx, func, (Cn_Emit_Opt) { __VA_ARGS__ })

CNDEF int cn_emit_opt(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt opt);

/**
 * Wrapper around cn_type_equals, to be used in a set that holds pointers to the types.
 * That is made so actual type data stays in the same memory addressees.
 */
CNDEF bool cn_ast_type_ptr_equals(const Cn_Type **type1_ptr, const Cn_Type **type2_ptr);

/**
 * Wrapper around cn_type_hash, to be used in a set that holds pointers to the types.
 * That is made so actual type data stays in the same memory addressees.
 */
CNDEF uint64_t cn_ast_type_ptr_hash(const Cn_Type **type_ptr);

/**
 * Scope macro that simply returns idx of the currently active scope.
 */
#define CN_AST_SCOPE_STACK_CURRENT_IDX (cn_array_list_length(&cn__ast_data->scope_stack) - 1)

/**
 * Defines idx of the file scope.
 */
#define CN_AST_FILE_SCOPE_IDX 0

/**
 * RETURNS: scope that sits at the top of the scope stack.
 */
CNDEF Cn_Ast_Scope *cn_ast_scope_stack_peek();

/**
 * Pushes new scope to the top of the stack.
 */
CNDEF void cn_ast_scope_stack_push();

/**
 * Pops scope from the top of the stack.
 */
CNDEF void cn_ast_scope_stack_pop();

/**
 * Simple macro to get ast binding based on its idx.
 */
#define cn_ast_binding_get(idx)    (cn__ast_data->binding_list + (idx))

/**
 * TODO: Write documentation.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_binding_table_put(Cn_Ast_Binding binding, Cn_Ast_Binding_Idx **binding_table);
    
/**
 * TODO: Write documentation.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_binding_table_get(Cn_String name, Cn_Ast_Binding_Idx **binding_table);

/**
 * Adds new function binding to the current scope.
 * If same binding already exists handles redeclaration, redefinition of the binding.
 *
 * RETURNS: NIL if error occured, binding idx on success.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_function_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Storage_Specifier_Flags storage_flags, Cn_Function_Specifier_Flags function_flags, Cn_Type *type, Cn_Ast_List *parameter_declarations, bool is_definition);

/**
 * Adds new variable binding to the current scope.
 * If same binding already exists handles redeclaration, redefinition of the binding.
 *
 * RETURNS: NIL if error occured, binding idx on success.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_variable_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Storage_Specifier_Flags storage_flags, Cn_Type *type, bool is_definition);

/**
 * Adds new typedef binding to the current scope.
 * If same binding already exists handles redeclaration, redefinition of the binding.
 *
 * RETURNS: NIL if error occured, binding idx on success.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_typedef_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Type *type);

/**
 * Adds new enum constant binding to the current scope.
 * If same binding already exists handles redeclaration, redefinition of the binding.
 *
 * RETURNS: NIL if error occured, binding idx on success.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_enum_constant_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Type *type, int64_t value);

/**
 * Declares new tag binding to the current scope. 
 *
 * If same binding already exists handles redeclaration.
 *  -   If binding exists and complete from any scope, including current scope, 
 *      it will return that binding from that scope.
 *  -   If binding exists but not completed in different scope, 
 *      it will declare new binding in current scope.
 *  -   If binding exists but not completed in current scope, 
 *      it will reference that declaration.
 *  -   If binding doesn't exist it will declare new binding in current scope.
 *
 * RETURNS: NIL if error occured, binding idx on success.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_tag_binding_declare(Cn_String tag, Cn_Type_Kind kind);

/**
 * Parses signle token type, if token is not of the expected type, emits error diagnostic.
 * 
 * RETURNS: True if successfully parsed token, false otherwise.
 */
CNDEF bool cn_parse_expect(Cn_Lexer *lexer, Cn_Token_Type type);

/**
 * Parses signle token type, if token is not of the expected type, skips.
 * 
 * RETURNS: True if successfully parsed token, false otherwise.
 */
CNDEF bool cn_parse_optional(Cn_Lexer *lexer, Cn_Token_Type type);

/**
 * Parses code starting of with lexer current token as translation unit.
 *
 *  translation_unit
 *          : external_declaration* EOF
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_translation_unit(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as external declaration.
 *
 *  external_declaration
 *          : '__extension__'? (
 *              ';'                         // Stray ';'
 *              | function
 *              | declaration ';'
 *              )
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_external_declaration(Cn_Lexer *lexer);

/**
 * Following function traverses declarator nodes to get declarator identifier.
 *
 * TODO: Make always output NIL as identifier on any abstract declarator.
 *
 * OUTPUTS: Last declarator idx in chain.
 */
CNDEF void cn_get_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *last_declarator_idx);

/**
 * Following function traverses declarator nodes to get function declarator identifier.
 * And other information.
 *
 * TODO: Make always output NIL as identifier on any abstract declarator.
 *
 * OUTPUTS: Parameter type list idx of the function, if supplied value is not NULL. 
 * OUTPUTS: Last declarator idx in chain.
 *
 * RETURNS: True on success, false if declarator is not a proper function.
 */
CNDEF bool cn_get_function_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *function_direct_declarator_idx, Cn_Ast_Idx *last_declarator_idx);

/**
 * Parses code starting of with lexer current token as function or declaration. 
 * Since we can't really know for certain which one until parsed far enough ast structure to determine. 
 * Ultimetly existance of a function body at the end of the declaration signifies 
 * that the declaration is a function definition.
 * 
 * NOTE: function doesn't include classic declaration list that are K&R styled function definitons.
 *
 * The deciding logic of whether declaration is a function definiton, 
 * is solely determined by first trying to parse single declarator
 * and if it is followed by function body, then it is function definition.
 *
 *  declaration ';'
 *          : attribute_specifiers declaration_specifiers init_declarators? gnu_attribute_specifiers ';'
 *          ;
 *
 *  function
 *          : attribute_specifiers declaration_specifiers declarator block
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_function_or_declaration(Cn_Lexer *lexer);

/**
 * Continues parsing code starting of with lexer current token as declaration, 
 * taking supplied attribute specifiers.
 *
 *  declaration
 *          : attribute_specifiers declaration_specifiers init_declarators? gnu_attribute_specifiers
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_continue_declaration(Cn_Lexer *lexer, Cn_Ast_List attribute_specifiers);

/**
 * Analyzes already constructed ast nodes as declaration.
 *
 * RETURNS: True on success, false if any errors occured.
 */
CNDEF bool cn_ast_analyze_declaration(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_List init_declarators);

/**
 * Analyzes already constructed ast nodes as function definition.
 *
 * OUTPUTS: Function binding on success.
 *
 * RETURNS: True on success, false if any errors occured.
 */
CNDEF bool cn_ast_analyze_function(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_Idx declarator_idx, Cn_Ast_Binding_Idx *function_binding_idx);

/**
 * Binds function definition params to the current scope.
 *
 * RETURNS: True on success, false if any errors occured.
 */
CNDEF bool cn_ast_bind_function_params(Cn_Ast_Binding_Idx function_binding_idx);

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
 * Determines whether tokens indicate start of a type, 
 * used to distinguish type cast during parsing of an expression.
 * 
 * NOTE: Doesn't alter state of lexer.
 * 
 * RETURNS: true if tokens indicate start of a type, false otherwise.
 */
CNDEF bool cn_ast_starts_type(Cn_Lexer *lexer);

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
 */
CNDEF Cn_Ast_Idx cn_ast_parse_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as block.
 *
 * IMPORTANT: Block introduces new scope,
 * but this can be controlled by setting use current scope to true.
 * By doing so, block won't introduce new scope
 * and everything will be defined and declared in current scope instead of a new one.
 *
 *  block
 *          : '{' (statement | declaration)* '}'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_block(Cn_Lexer *lexer, bool use_current_scope);

/**
 * Parses code starting of with lexer current token as if statement.
 *
 *  if_statement
 *          : 'if' '(' expression ')' statement ('else' statement)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_if(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as switch statement.
 *
 *  switch_statement
 *          : 'switch' '(' expression ')' statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_switch(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as while statement.
 *
 *  while_statement
 *          : 'while' '(' expression ')' statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_while_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as do while
 *
 *  do_while
 *          : 'do' statement 'while' '(' expression ')' ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_do_while(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as for statement.
 *
 *  for_statement
 *          : 'for' '(' (for_declaration | expression)? ';' ';' ')' statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_while_statement(Cn_Lexer *lexer);


/**
 * Parses 'goto' identifier ';'
 */
CNDEF Cn_Ast_Idx cn_ast_parse_goto(Cn_Lexer *lexer);

/**
 * Parses 'return' expression? ';'
 */
CNDEF Cn_Ast_Idx cn_ast_parse_return(Cn_Lexer *lexer);

/**
 * Parses 'break' ';'
 */
CNDEF Cn_Ast_Idx cn_ast_parse_break(Cn_Lexer *lexer);

/**
 * Parses 'continue' ';'
 */
CNDEF Cn_Ast_Idx cn_ast_parse_continue(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as labeled statement.
 *
 *  label
 *          : TODO: Labeled statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_label(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as expression statement.
 *
 *  expression_statement
 *          : expression? ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as argument list.
 *
 *  argument_list
 *          : expression (',' expression)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_arguments(Cn_Lexer *lexer, bool *ok);

typedef enum : uint8_t {
    /**
     * If set, particular parse expression function call will ignore comma operator, 
     * not it doesn't mean that all recrusive function calls will ignore comma operators too, 
     * for example, this flag is set to 0 when paranthesized expression is encountered. 
     * This flag is used when top level comma token acts as a separator, 
     * as example in declartions with multiple declarators or function arguments.
     */
    CN_NO_COMMA_OPERATOR = 0x1, 
    
} Cn_Expression_Parsing_Flags;

/**
 * Parses code starting of with lexer current token as expression.
 * Function first parses left leaf, and then calls increasing precedence parsing, until it returns left leaf.
 * Utilizing following precedence table as guide to properly structure AST nodes in order.
 *
 *  C OPERATOR PRECEDENCE TABLE highest (14) to lowest (0)
 *  ===============================================================
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
 *  0                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    ,             Comma
 *
 * Following are grammar rules to group expression into certain kinds, but in reality all parsing is based on precedence table.
 * Grammars are just what is general logic, and what specific tokens parser might expect in certain expressions.
 *
 *  expression
 *          : binary_expression
 *          | function_expression
 *          | access_expression
 *          | unary_expression
 *          | cast_expression
 *          | compound_literal
 *          | sizeof_expression
 *          | primary_expression
 *          | ternary_expression
 *          | assignment_expression
 *          | postfix_expression
 *          ;
 *
 *  binary_expression
 *          : expression binary_operator expression
 *          | expression '[' expression ']'
 *          | function_expression
 *          | access_expression
 *          ;
 *
 *  function_expression
 *          : expression '(' argument_list? ')'
 *          ;
 *
 *  access_expression
 *          : expression '.' identifier
 *          | expression '->' identifier
 *          ;
 *
 *  unary_expression
 *          : cast_expression
 *          | '(' expression ')'
 *          | sizeof_expression
 *          | unary_operator expression
 *          ;
 *
 *  cast_expression
 *          : '(' type_name ')' expression
 *          | compound
 *          ;
 *
 *  compound
 *          : '(' type_name ')' '{' designations? '}'
 *          ;
 *  
 *  sizeof_expression
 *          : 'sizeof' expression
 *          | 'sizeof' '(' type_name ')'
 *          ;
 *
 *  primary_expression
 *          : identifier
 *          | integer
 *          | float
 *          | string
 *          ;
 *
 *  ternary_expression
 *          : expression '?' expression ':' expression
 *          ;
 *
 *  assignment_expression
 *          : expression assignment_operator expression
 *          ;
 *
 *  postfix_expression
 *          : expression '++'
 *          | expression '--'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression(Cn_Lexer *lexer, int min_precedence, Cn_Expression_Parsing_Flags flags);

/**
 * This function constructs right leaning tree if next binary operator is increaseing precedence, 
 * if not it just returns left leaf idx.
 * It recursivly calls parse expression to get right leaf idx.
 *
 * IMPORTANT: Naturally this function constructs right leaning tree on binary expressions.
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
 * But also there is situation where left leaning tree must be constructed.
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
 * A more sophisticated example
 * Another example:
 *
 * a + b * c + d
 *
 * Is parsed like:
 * ((a + (b * c)) + d)
 *
 * parse0 min_prec -1:
 *
 *   +
 *  / \
 * a   parse1
 *
 * parse1 min_prec 11:
 *   
 *   *
 *  / \
 * b   parse2
 *
 * parse2 min_prec 12:
 *
 * c
 *
 * RETURNING
 *
 * parse1 min_prec 11:
 *
 *   *
 *  / \
 * b   c
 *
 * parse0 min_prec -1:
 *
 *   +
 *  / \
 * a   *
 *    / \
 *   b   c
 *
 * parse0 min_prec -1:
 *
 *     +
 *    / \
 *   +   parse3
 *  / \
 * a   *
 *    / \
 *   b   c
 *
 * parse3 min_prec 11:
 *
 * d
 *
 * RETURNING
 *
 * parse0 min_prec -1:
 *
 *     +
 *    / \
 *   +   d
 *  / \
 * a   *
 *    / \
 *   b   c
 *
 * NO MORE INPUT, STOP.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_increasing_precedence(Cn_Lexer *lexer, Cn_Ast_Idx left_idx, int min_precedence, Cn_Expression_Parsing_Flags flags);

/**
 * Parses non-binary leaf expression, it will detect and parse unary operators, 
 * but it doesn't parse binary precedent expressions or postfix expressions.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_leaf(Cn_Lexer *lexer, Cn_Expression_Parsing_Flags flags);

/**
 * Continues parsing code starting of with lexer current token as init declarator list, 
 * taking supplied declarator and finishing it as first init declarator.
 *
 *  init_declarators
 *          : init_declarator (',' init_declarator)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_continue_init_declarators(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx, bool *ok);

/**
 * Parses code starting of with lexer current token as init declarator list.
 *
 *  init_declarators
 *          : init_declarator (',' init_declarator)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_init_declarators(Cn_Lexer *lexer, bool *ok);

/**
 * Finishes parsing code starting of with lexer current token 
 * and given already parsed declarator as init declarator.
 *
 *  init_declarator
 *          : declarator gnu_asm_label? ('=' initializer)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_finish_init_declarator(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx);

/**
 * Parses code starting of with lexer current token as init declarator.
 *
 *  init_declarator
 *          : declarator gnu_asm_label? ('=' initializer)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as initializer.
 *
 *  initializer
 *          : expression
 *          | '{' designations? '}'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as initializers.
 *
 *  designations
 *          : designation (',' designation)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_designations(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as designation.
 *
 *  designation
 *          : (designators '=')? initializer 
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_designation(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as designators.
 *
 *  designators
 *          : designator+
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_designators(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as designators.
 *
 *  designator
 *          : '.' identifier
 *          | '[' expression ']'
 *          | '[' expression '...' expression ']'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_designator(Cn_Lexer *lexer);

/**
 * Determines whether tokens indicate start of designator, 
 * used to distinguish between designator and initializer before parsing them.
 * 
 * NOTE: Doesn't alter state of lexer.
 * 
 * RETURNS: true if tokens indicate start of designator, false otherwise.
 */
CNDEF bool cn_ast_starts_designator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as declarator.
 *
 *  declarator
 *          : pointer? direct_declarator
 *          ;
 *
 * NOTE: This function can return NIL successfully. Only occures in the case where there is 
 * completely empty abstract declarator, meaning abstract declarator with no direct declarator, 
 * in that case no tokens are consumed either since nothing is parsed.
 *
 * OUTPUTS: ok true on success, ok false if error occured.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_declarator(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as pointer.
 *
 *  pointer
 *          : '*' qualifier* pointer?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_pointer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as direct declarator.
 *
 *  direct_declarator
 *          : identifier
 *          | '(' declarator ')'
 *          | direct_declarator '[' expression? ']'
 *          | direct_declarator '(' ('void' | parameter_type_list)? ')'
 *          ;
 * 
 * OUTPUTS: is_abstract true, if no identifier was found at the end of direct declarator parsing, 
 * false otherwise.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_direct_declarator(Cn_Lexer *lexer, bool *is_abstract);

/**
 * Parses code starting of with lexer current token as identifier.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_identifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as integer.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_integer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as float.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_float(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as string literal.
 * 
 * IMPORTANT: It will concat any naighboring strings till 
 * non string token is reached.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_string(Cn_Lexer *lexer);

/**
 * RETURNS: True if ast node is identifier, integer, float or string.
 */
CNDEF bool cn_ast_is_primary(Cn_Ast_Idx node);

/**
 * Parses code starting of with lexer current token as declaration specifiers.
 *
 *  declaration_specifiers
 *          : (
 *              storage_specifier
 *              | qualifier
 *              | function_specifier
 *              | gnu_attribute_specifier
 *              | type_specifier
 *              )+
 *          ;
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
 * Tries to parse code starting of with lexer current token as functions specifier.
 *
 * OUTPUTS: It into supplied output destination.
 *
 * RETURNS: 0 on success, 1 if token wasn't accepted, 2 on error.
 *
 *  function_specifier
 *          : 'inline'
 *          | '_Noreturn'
 *          ;
 */
CNDEF int cn_ast_try_parse_function_specifier(Cn_Lexer *lexer, Cn_Function_Specifier_Flags *output);

/**
 * Parses code starting of with lexer current token as GNU typeof specifier.
 *
 *  gnu_typeof_specifier
 *          | 'typeof' '(' type_name ')'
 *          : 'typeof' '(' expression ')'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_gnu_typeof(Cn_Lexer *lexer);

/**
 * Helper struct for accumulating primitive type specifier info during parsing
 */
typedef struct {
    Cn_Ast_Type_Kind  kind;
    Cn_Ast_Type_Width width;
    Cn_Ast_Type_Sign  sign;
} Cn_Primitive_Type_Info;

/**
 * TODO: Refactor implementation of this functions and its interface.
 * It can be simpler, without need of helper primitive type info struct.
 * And without passing pointer to idx? Thats should be unnecessary.
 *
 * Tries to parse code starting of with lexer current token as type specifier.
 *
 * OUTPUTS: For primitive types (int, char, signed, unsigned, long, short, etc.), updates primitive_info.
 * OUTPUTS: For typedef/struct/union/enum/typeof, sets type_specifier_idx to the node.
 * 
 * IMPORTANT: This function can return 1, which is NOT an error.
 * It will occure in a case where current token is not a valid token type specifier can accept.
 * And that no errors occured. It is done so type specifier can check everything it needs 
 * and if it has no work there to be done, 
 * signifying outer function that current token is definitly not a type specifier.
 *
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
 *          | struct_specifier
 *          | union_specifier
 *          | enum_specifier
 *          | typedef
 *          | gnu_typeof
 *          ;
 */
CNDEF int cn_ast_try_parse_type_specifier(Cn_Lexer *lexer, Cn_Primitive_Type_Info *primitive_info, Cn_Ast_Idx *type_specifier_idx);

/**
 * Parses code starting of with lexer current token as type name.
 *
 *  type_name
 *          : specifier_qualifier abstract_declarator
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_type_name(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as specifier qualifier.
 *
 *  specifier_qualifier
 *          : gnu_attribute_specifiers (qualifier | type_specifier)+
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_specifier_qualifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as parameter type list.
 *
 *  parameter_type_list
 *          : '...'
 *          | parameter_declaration_list (',' '...')?
 *          ;
 *
 *  parameter_declaration_list
 *          : parameter_declaration (',' parameter_declaration)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_parameter_declarations(Cn_Lexer *lexer, bool *variadic, bool *ok);

/**
 * Parses code starting of with lexer current token as parameter declaration.
 *
 *  parameter_declaration
 *          : declaration_specifiers declarator
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_parameter_declaration(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as struct or union specifier.
 *
 *  struct_specifier
 *          : 'struct' gnu_attribute_specifiers
 *          ( identifier? '{' member_declaration* '}' ) 
 *          | identifier
 *          ;
 *
 *  union_specifier
 *          : 'union' gnu_attribute_specifiers
 *          ( identifier? '{' member_declaration* '}' ) 
 *          | identifier
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_struct_or_union_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as member declaration.
 *
 *  member_declaration
 *          : specifier_qualifier member_declarators ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_member_declaration(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as member declarator list.
 *
 *  member_declarators
 *          : member_declarator (',' member_declarator)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_member_declarators(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as member declarator.
 *
 *  member_declarator
 *          : declarator gnu_attribute_specifiers
 *          | declarator? ':' expression gnu_attribute_specifiers
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_member_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as enum specifier.
 *
 *  enum_specifier
 *          : 'enum' attribute_specifiers gnu_attribute_specifiers
 *          ( identifier? (':' specifier_qualifier)? '{' enumerator* '}' ) 
 *          | identifier  (':' specifier_qualifier)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_enum_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as enumerator.
 *
 *  enumerator
 *          : identifier attribute_specifiers gnu_attribute_specifiers ('=' expression)? ','
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_enumerator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as attribute specifier sequence.
 *
 * attribute_specifiers
 *          : attribute_specifier*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_attribute_specifiers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as attribute specifier.
 *
 * attribute_specifier
 *          : '[' '[' attribute_list? ']' ']'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_attribute_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as attribute list.
 *
 * attributes
 *          : attribute (',' attribute)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_attributes(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as attribute.
 *
 * attribute
 *          : ( identifier ':' ':' )? identifier ( '(' arguments ')' )?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_attribute(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as GNU attribute specifier sequence.
 *
 * gnu_attribute_specifiers
 *          : gnu_attribute_specifier*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_gnu_attribute_specifiers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as GNU attribute specifier.
 *
 * gnu_attribute_specifier
 *          : '__attribute__' '(' '(' gnu_attribute_list? ')' ')'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as GNU attribute list.
 *
 * gnu_attributes
 *          : gnu_attribute (',' gnu_attribute)*
 *          ;
 */
CNDEF Cn_Ast_List cn_ast_parse_gnu_attributes(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as GNU attribute.
 *
 * gnu_attribute
 *          : identifier ( '(' arguments ')' )?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as GNU asm label.
 *
 * gnu_asm_label
 *          : '__asm__' '(' string* ')'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_gnu_asm_label(Cn_Lexer *lexer);

/**
 * TODO: Expose to the user and document.
 */
CNDEF Cn_Type *cn__ast_add_type_if_not(Cn_Type *type);

/**
 * Converts ast nodes to type.
 * If type already exists returns pointer to it, 
 * if not creates new type(s) and returns pointer to it.
 *
 * RETURNS: NULL if error occured, type of an ast nodes on success.
 */
CNDEF Cn_Type *cn_ast_to_type(Cn_Qualifier_Flags flags, Cn_Ast_Idx type_specifier_idx, Cn_Ast_Idx declarator_idx);

/**
 * Recursive function that type checks expression 
 * by traversing ast nodes from bottom down,
 * and assigning appropriate type to every node.
 *
 * EXPECTS: Expression idx != 0.
 *
 * RETURNS: NULL if error occured, resulting type of an expression on success.
 */
CNDEF Cn_Type *cn_ast_expression_typecheck(Cn_Ast_Idx expression_idx);

/**
 * Gets resulting type of an expression.
 *
 * EXPECTS: Expression idx != 0.
 *
 * RETURNS: NULL if expression wasn't typechecked, valid type on success.
 */
CNDEF Cn_Type *cn_ast_expression_get_type(Cn_Ast_Idx expression_idx);

/**
 * Parses a integer literal: decimal, octal, 0x-hex, optional u/U/l/L suffix. 
 * into its magnitude. No leading sign: unary minus is its own AST node.
 *
 * RETURNS: True on success, false on error.
 */
CNDEF bool cn_parse_int_literal(Cn_String literal, uint64_t *out);

/**
 * Parses a float literal: mantissa + optional exponent + optional f/F/l/L suffix. 
 * Rounding is not bit exact. 
 *
 * NOTE: Hex floats are not handled will output NIL.
 *
 * RETURNS: True on success, false on error.
 */
CNDEF bool cn_parse_float_literal(Cn_String literal, double *out);

/**
 * Evaluates an expression, uses supplied buffer to store data.
 *
 * NOTE: Size of the buffer can be determined 
 * from the resulting type of an expression.
 *
 * EXPECTS: Expression idx != 0 and expression to be typechecked.
 *
 * RETURNS: NIL if expression wasn't typechecked or if expression 
 * cannot be evaluated at compile time, valid any struct on success.
 */
CNDEF Cn_Any cn_ast_expression_evaluate(Cn_Ast_Idx expression_idx, void *buffer);

/**
 * Clears expression ast nodes from their types undoing typechecking.
 * Used to reanalyze modified ast nodes.
 */
CNDEF void cn_ast_expression_clear_types(Cn_Ast_Idx expression_idx);

/**
 * Recursive function that type checks designations against known type. 
 *
 * RETURNS: False if error occured, true on success.
 */
CNDEF bool cn_ast_designations_typecheck(Cn_Ast_List designations, Cn_Type *type);

/**
 * Recursive function that type checks initializer against known type. 
 *
 * EXPECTS: Initializer idx != 0.
 *
 * RETURNS: False if error occured, true on success.
 */
CNDEF bool cn_ast_initializer_typecheck(Cn_Ast_Idx initializer_idx, Cn_Type *type);

/**
 * Reparse given ast node as translation unit.
 *
 * IMPORTANT: Reparse funtions perfom:
 *  Structural AST validation.
 *  Type resolution via cn_ast_to_type.
 *  Binding creation for variables, functions, typedefs, tags.
 *  Expression typechecking.
 *  Scope management for compound statements and functions.
 *  Constant evaluation if needed.
 *
 * Expecting clear ast tree.
 */
CNDEF bool cn_ast_reparse_translation_unit(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as external declaration.
 */
CNDEF bool cn_ast_reparse_external_declaration(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as declaration.
 */
CNDEF bool cn_ast_reparse_declaration(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as function definition.
 */
CNDEF bool cn_ast_reparse_function_definition(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as asm definition.
 */
CNDEF bool cn_ast_reparse_asm_definition(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as statement.
 */
CNDEF bool cn_ast_reparse_statement(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as compound statement.
 */
CNDEF bool cn_ast_reparse_block(Cn_Ast_Idx node_idx, bool use_current_scope);

/**
 * Reparse given ast node as selection statement.
 */
CNDEF bool cn_ast_reparse_if(Cn_Ast_Idx node_idx);
CNDEF bool cn_ast_reparse_switch(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as iteration statement.
 */
CNDEF bool cn_ast_reparse_iteration_statement(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as jump statement.
 */
CNDEF bool cn_ast_reparse_goto(Cn_Ast_Idx node_idx);
CNDEF bool cn_ast_reparse_return(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as labeled statement.
 */
CNDEF bool cn_ast_reparse_labeled_statement(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as expression statement.
 */
CNDEF bool cn_ast_reparse_expression_statement(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as argument list.
 */
CNDEF bool cn_ast_reparse_argument_list(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as expression.
 */
CNDEF bool cn_ast_reparse_expression(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as init declarators.
 */
CNDEF bool cn_ast_reparse_init_declarators(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as init declarator.
 */
CNDEF bool cn_ast_reparse_init_declarator(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as initializer.
 */
CNDEF bool cn_ast_reparse_initializer(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as declarator.
 */
CNDEF bool cn_ast_reparse_abstract_declarator(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as declarator.
 */
CNDEF bool cn_ast_reparse_declarator(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as pointer.
 */
CNDEF bool cn_ast_reparse_pointer(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as direct declarator.
 */
CNDEF bool cn_ast_reparse_direct_declarator(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as declaration specifiers.
 */
CNDEF bool cn_ast_reparse_declaration_specifiers(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as type specifier.
 */
CNDEF bool cn_ast_reparse_type_specifier(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as GNU typeof specifier.
 */
CNDEF bool cn_ast_reparse_gnu_typeof_specifier(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as type name.
 */
CNDEF bool cn_ast_reparse_type_name(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as specifier qualifier.
 */
CNDEF bool cn_ast_reparse_specifier_qualifier(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as parameter type list.
 */

/**
 * Reparse given ast node as parameter declaration.
 */
CNDEF bool cn_ast_reparse_parameter_declaration(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as struct or union specifier.
 */
CNDEF bool cn_ast_reparse_struct_or_union_specifier(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as member declaration.
 */
CNDEF bool cn_ast_reparse_member_declaration(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as member declarator.
 */
CNDEF bool cn_ast_reparse_member_declarator(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as gnu attribute specifier.
 */
CNDEF bool cn_ast_reparse_gnu_attribute_specifier(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as gnu attribute.
 */
CNDEF bool cn_ast_reparse_gnu_attribute(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as gnu asm label.
 */
CNDEF bool cn_ast_reparse_gnu_asm_label(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as enum specifier.
 */
CNDEF bool cn_ast_reparse_enum_specifier(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as enumerator.
 */
CNDEF bool cn_ast_reparse_enumerator(Cn_Ast_Idx node_idx);

// DIAGNOSTIC SECTION

typedef enum {
    CN_DC_ZERO,
    CN_DC_DUPLICATE_TYPE_SPECIFIER,
    CN_DC_EXPECTED_TOKEN,
    CN_DC_EXPECTED_EXPRESSION,
    CN_DC_UNEXPECTED_TOKEN,
    CN_DC_INVALID_DECLARATOR,
    CN_DC_INVALID_TYPE_NAME,
    CN_DC_MULTIPLE_STORAGE_SPECIFIERS,
    CN_DC_MISSING_DECLARATION_SPECIFIER,
    CN_DC_INVALID_TYPE_SPECIFIER,
    CN_DC_REDEFINITION,
    CN_DC_INCOMPLETE_TYPE,
    CN_DC_INVALID_SYMBOL,
    CN_DC_ILLEGAL_TYPE,
    CN_DC_INVALID_CONSTANT_EXPRESSION,
    CN_DC_ILLEGAL_BINDING,
    CN_DC_FROM,
    CN_DC_EXPECTED_AST_NODE,
    CN_DC_UNEXPECTED_FUNCTION,
    CN__DC_COUNT,
} Cn_Diagnostic_Code;

extern const Cn_String CN_DIAGNOSTIC_CODES[CN__DC_COUNT];

typedef enum {
    CN_DIAGNOSTIC_INFO,
    CN_DIAGNOSTIC_WARNING,
    CN_DIAGNOSTIC_ERROR,
} Cn_Diagnostic_Level;

extern Cn_Diagnostic_Level cn_min_diagnostic_level;

typedef struct {
    int64_t offset;
    int64_t length;
} Cn_Diagnostic_Annotation;

typedef void (Cn_Diagnostic_Handler)(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, Cn_Diagnostic_Annotation annotations[], size_t annotations_length, const char *format, va_list args);

extern Cn_Diagnostic_Handler *cn_diagnostic_handler;

CNDEF Cn_Diagnostic_Handler cn_default_diagnostic_handler;

CNDEF Cn_Diagnostic_Handler cn_null_diagnostic_handler;

CNDEF void cn_diagnostic_src(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Source *src, Cn_Diagnostic_Code code, const char *format, ...);

/**
 * Diagnostic variant to report error per node, if node was modified or replaced,
 * multiple diagnostics eventually pointing to the source location will be emitted.
 */
CNDEF void cn_diagnostic_node(Cn_Diagnostic_Level level, Cn_Ast_Idx idx, Cn_Diagnostic_Code code, const char *format, ...);

// PRE-PROCESSING SECTION

typedef enum {
    CN_MESSAGE_PARSED_FUNCTION_DEFINITION
} Cn_Message_Kind;

typedef struct {
    Cn_Ast_Idx node_idx;
} Cn_Message_Parsed_Function_Definition;

typedef union {
    Cn_Message_Parsed_Function_Definition parsed_function_definition;
} Cn_Message;

typedef int (Cn_Message_Handler)(Cn_Message_Kind kind, void *message);

extern Cn_Message_Handler *cn_message_handler;

/**
 * Simple wrapper that checks if message handler not NULL, 
 * if so it sends specified message to the user.
 *
 * RETURNS: True if reload to last checkpoit required, 
 * false if no modifications occured.
 */
CNDEF bool cn_send_message(Cn_Message_Kind kind, Cn_Message message);

CNDEF void cn_log_types();

CNDEF void cn_log_bindings();

typedef struct {
    Cn_String source;
} Cn_Tu_Make_Opt;

typedef struct {
    char *path;
    bool no_malloc;
    Cn_String content;
    Cn_Ast_Data ast_data;
} Cn_Translation_Unit;

typedef enum : uint8_t {
    CN_PRINT_SOURCE   = 0x01,
    CN_PRINT_TOKENS   = 0x02,
    CN_PRINT_AST      = 0x04,
    CN_PRINT_TYPES    = 0x08,
    CN_PRINT_BINDINGS = 0x10,
    CN_NO_CODE_OUTPUT = 0x20,
} Cn_Flags;

/**
 * RETURNS: Cn_Translation_Unit struct that represent 
 * classic C translation unit in which all lexing, parsing, 
 * infering, modification occurs.
 *
 * IMPORTANT: Takes in path to the .i file, 
 * that is intermidiate pre-processed representation of a translation unit.
 * Simply it is a .c file that has all macros expanded 
 * and ready to be sent to the compiler.
 * Supplying .c files with macros that are not expanded will result in error, 
 * ensure to pass .c file through compiler's pre-processor first.
 * In gcc it would be executing the following:
 *      
 *      $ gcc -E -o file.i file.c
 *
 * This will generate .i file, path to which can be safely specified here.
 */
#define cn_tu_make(intermidiate_path, ...) cn_tu_make_opt(intermidiate_path, (Cn_Tu_Make_Opt) { __VA_ARGS__ })

CNDEF Cn_Translation_Unit cn_tu_make_opt(char *intermidiate_path, Cn_Tu_Make_Opt opt);

/**
 * Processes the translation unit from top to bottom.
 * Translation Unit passes through Infer -> Size stages.
 * Meaning AST is built, type and symbol table is constructed, 
 * and size's of the types are calculated too.
 * Through the processing the messages are enqueued, 
 * and by the end of the process they are triggered,
 * if cn_message_handler is not NULL.
 *
 * RETURNS: 0 if processing is successful. -1 if error occured.
 */
CNDEF int cn_tu_process(Cn_Translation_Unit *tu, Cn_Flags flags); 

/**
 * This function free's all memory used by the translation unit, 
 * including closing previously opened file.
 */
CNDEF void cn_tu_free(Cn_Translation_Unit *tu);

/**
 * Replaces old ast node with a new, this function is 
 * a recommended way of modifying ast tree.
 * Because it automatically tracks node changes 
 * and guarantees proper diagnostics.
 */
CNDEF void cn_replace(Cn_Ast_Idx *original, Cn_Ast_Idx new, Cn_Ast_Idx parent_idx);

/**
 * Optional values that can be set for any cn_build_* function.
 */
typedef struct {
    /**
     * Defines whether call to build function should allocate 
     * memory internally in permanent arena for any supplied data.
     *
     * IMPORTANT: If set to false, the function will trust it's user to 
     * manage passed memory, for example, any names, snippets of code, etc.
     *
     * NOTE: Setting it to false, is helpful if certain piece of code is 
     * inserted a lot. As example it prevents build function from allocating 
     * memory over and over again for the same piece of string data.
     */
    bool alloc;

    /**
     * File and line are information that will indicate from where specific 
     * build function was called, used to trace meta program code insertions.
     */
    const char *file;
    int64_t line;
} Cn_Build_Opt;

#ifndef CN_BUILD_OPT_DEFAULT_ALLOC
#   define CN_BUILD_OPT_DEFAULT_ALLOC false
#endif // CN_BUILD_OPT_DEFAULT_ALLOC

/**
 * Builds list out of supplied nodes.
 * Accepts variadic array of nodes in order.
 *
 * RETURNS: List composed of supplied nodes.
 */
#define cn_build_list(...) ((Cn_Ast_List){ (Cn_Ast_Idx[]) { __VA_ARGS__ }, sizeof((Cn_Ast_Idx[]) { __VA_ARGS__ }) / sizeof(Cn_Ast_Idx) })

/**
 * Builds identifier with supplied name.
 * 
 * RETURNS: Built identifier.
 */
#define cn_build_identifier(name, ...) cn__build_identifier(name, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_identifier(Cn_String name, Cn_Build_Opt opt);

/**
 * Builds integer with supplied value.
 *
 * RETURNS: Built integer.
 */
#define cn_build_integer(value, ...) cn__build_integer(value, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_integer(Cn_String value, Cn_Build_Opt opt);

/**
 * Builds float with supplied value.
 * 
 * RETURNS: Built float.
 */
#define cn_build_float(value, ...) cn__build_float(value, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_float(Cn_String value, Cn_Build_Opt opt);

/**
 * Builds string with supplied str.
 * 
 * RETURNS: Built string.
 */
#define cn_build_string(str, ...) cn__build_string(str, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_string(Cn_String str, Cn_Build_Opt opt);

/**
 * Builds binary expression, left and right params must be 
 * valid built node idx's.
 * 
 * RETURNS: Built binary expression.
 */
#define cn_build_binary(op, left, right, ...) cn__build_binary(op, left, right, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_binary(Cn_Binary_Operator_Kind op, Cn_Ast_Idx left, Cn_Ast_Idx right, Cn_Build_Opt opt);

/**
 * Builds unary expression, expression must be 
 * valid built node idx.
 * 
 * RETURNS: Built unary expression.
 */
#define cn_build_unary(op, expression, ...) cn__build_unary(op, expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_unary(Cn_Unary_Operator_Kind op, Cn_Ast_Idx expression, Cn_Build_Opt opt);

/**
 * Builds function call expression, callee must be 
 * valid built node idx. And arg_list should be constructed beforehand 
 * with appropriate expression children.
 * 
 * RETURNS: Built function call expression.
 */
#define cn_build_func_call(callee, arguments, ...) cn__build_func_call(callee, arguments, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_func_call(Cn_Ast_Idx callee, Cn_Ast_List arguments, Cn_Build_Opt opt);

/**
 * Builds expression statement, expression must be 
 * valid built node idx.
 * 
 * RETURNS: Built expression statement.
 */
#define cn_build_expr_statement(expression, ...) cn__build_expr_statement(expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_expr_statement(Cn_Ast_Idx expression, Cn_Build_Opt opt);



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

CNDEF int64_t cn_str_find_left(Cn_String str, Cn_String search) {
    Cn_String substr;
    for (int64_t i = 0; i + search.length <= str.length; i++) {
        if (str.data[i] == search.data[0]) {
            substr = cn_str_substring(str, i, i + search.length);
            if (cn_str_equals(&substr, &search)) {
                return i;
            }
        }
    }
    
    return -1;
}

CNDEF int64_t cn_str_find_right(Cn_String str, Cn_String search) {
    Cn_String substr;
    for (int64_t i = str.length - 1; i - search.length + 1 > -1; i--) {
        if (str.data[i] == search.data[search.length - 1]) {
            substr = cn_str_substring(str, i - search.length + 1, i + 1);
            if (cn_str_equals(&substr, &search)) {
                return i - search.length + 1;
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

CNDEF Cn_String cn_str_get_chars(Cn_String str, int64_t count) {
    return cn_str_substring(str, 0, count);
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

CNDEF bool cn_str_is_empty(Cn_String str) {
    return str.length == 0;
}

CNDEF bool cn_str_equals(const Cn_String *str1, const Cn_String *str2) {
    if (str1 == str2) return true;

    if (str1->length != str2->length) return false;

    return !memcmp(str1->data, str2->data, str1->length);
}

CNDEF uint64_t cn_str_hash(const Cn_String *str) {
    return cn_hash_bytes(str->data, str->length);
}

// STRING BUILDER SECTION
CNDEF Cn_String_Builder cn__sb_make(int64_t initial_capacity) {
    CN_ASSERT(initial_capacity > CN_SB_STACK_STORAGE_CAP);
    
    char *data = CN_REALLOC(NULL, initial_capacity);

    if (data == NULL){
        cn_log(CN_ERROR, "Couldn't allocate memory of size: %lld bytes, for the string builder.", initial_capacity);
        return (Cn_String_Builder) {0};
    }

    return (Cn_String_Builder) {
        .capacity = initial_capacity,
        .length = 0,
        .data = data,
    };
}

CNDEF void cn_sb_free(Cn_String_Builder *sb) {
    if (sb->capacity > CN_SB_STACK_STORAGE_CAP) {
        CN_FREE(sb->data);
    }

    sb->length = 0;
    sb->capacity = 0;
    sb->data = NULL;
}

CNDEF void cn_sb_clear(Cn_String_Builder *sb) {
    sb->length = 0;
}

CNDEF void cn_sb_append_char(Cn_String_Builder *sb, char c) {
    if (sb->length + 1 > sb->capacity) {
        if (sb->capacity > CN_SB_STACK_STORAGE_CAP) {
            sb->data = CN_REALLOC(sb->data, sb->capacity * 2);
        } else {
            char *old = sb->data;
            sb->data = CN_REALLOC(NULL, sb->capacity * 2);
            memcpy(sb->data, old, sb->length);
        }

        sb->capacity *= 2;
    }

    sb->data[sb->length] = c;
    sb->length++;
}

CNDEF void cn_sb_append_str(Cn_String_Builder *sb, Cn_String str) {
    if (sb->length + str.length > sb->capacity) {
        // IMPORTANT: To understand where this calculation comes from check cn__array_list_resize_to_fit implementation.
        // It uses same calculation that simplifies pow and log of base 2 caluclation to just using bit manipulation.
        int64_t ratio = (sb->length + str.length) / sb->capacity;
        if (ratio < 1) ratio = 1;
        int highest_bit_pos = 63 - CN_COUNT_LEADING_ZEROS(ratio);
        CN_ASSERT(highest_bit_pos >= 0);
        int64_t capacity_multiplier = (int64_t)(1 << (highest_bit_pos + 1));

        if (sb->capacity > CN_SB_STACK_STORAGE_CAP) {
            sb->data = CN_REALLOC(sb->data, sb->capacity * capacity_multiplier);
        } else {
            char *old = sb->data;
            sb->data = CN_REALLOC(NULL, sb->capacity * capacity_multiplier);
            memcpy(sb->data, old, sb->length);
        }

        sb->capacity *= capacity_multiplier;
    }

    memcpy(sb->data + sb->length, str.data, str.length);
    sb->length += str.length;
}

CNDEF void cn_sb_append_format(Cn_String_Builder *sb, char *format, ...) {
    va_list args;
    va_start(args, format);
    int length = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (length < 0) return;

    if (sb->length + length + 1 > sb->capacity) {
        // IMPORTANT: To understand where this calculation comes from check cn__array_list_resize_to_fit implementation.
        // It uses same calculation that simplifies pow and log of base 2 caluclation to just using bit manipulation.
        int64_t ratio = (sb->length + length + 1) / sb->capacity;
        if (ratio < 1) ratio = 1;
        int highest_bit_pos = 63 - CN_COUNT_LEADING_ZEROS(ratio);
        CN_ASSERT(highest_bit_pos >= 0);
        int64_t capacity_multiplier = (int64_t)(1 << (highest_bit_pos + 1));

        if (sb->capacity > CN_SB_STACK_STORAGE_CAP) {
            sb->data = CN_REALLOC(sb->data, sb->capacity * capacity_multiplier);
        } else {
            char *old = sb->data;
            sb->data = CN_REALLOC(NULL, sb->capacity * capacity_multiplier);
            memcpy(sb->data, old, sb->length);
        }

        sb->capacity *= capacity_multiplier;
    }

    va_start(args, format);
    vsnprintf(sb->data + sb->length, length + 1, format, args);
    va_end(args);

    sb->length += length;
}

CNDEF void cn_sb_reverse(Cn_String_Builder *sb) {
    char temp;
    for (int64_t i = 0; i < sb->length / 2; i++) {
        temp = sb->data[i];
        sb->data[i] = sb->data[sb->length - 1 - i];
        sb->data[sb->length - 1 - i] = temp;
    }
}

CNDEF Cn_String cn_sb_to_str(Cn_String_Builder *sb) {
    return CN_STR(sb->length, sb->data);
}


// CHAINED ARENA SECTION
CNDEF Cn_Chained_Arena cn_chained_arena_make(uint64_t block_capacity) {
    CN_ASSERT(block_capacity > 0);

    Cn_Chained_Arena_Block_Header *header = (Cn_Chained_Arena_Block_Header *)CN_REALLOC(NULL, sizeof(Cn_Chained_Arena_Block_Header) + block_capacity);

    header->prev = NULL;
    header->allocated = 0;
    
    return (Cn_Chained_Arena) {
        .block_capacity = block_capacity,
        .block = header + 1,
    };
}

CNDEF void *cn_chained_arena_alloc(Cn_Chained_Arena *arena, uint64_t size) {
    CN_ASSERT(size > 0);
    CN_ASSERT(size <= arena->block_capacity);

    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

    if ((header->allocated + size) > arena->block_capacity) {
        header = (Cn_Chained_Arena_Block_Header *)CN_REALLOC(NULL, sizeof(Cn_Chained_Arena_Block_Header) + arena->block_capacity);

        header->prev = arena->block;
        header->allocated = 0;
        
        arena->block = header + 1;
    }

    header->allocated += size;
    return (uint8_t *)arena->block + header->allocated - size;
}

CNDEF void cn_chained_arena_dealloc(Cn_Chained_Arena *arena, uint64_t size) {
    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
    uint64_t decrease;

    while (size > 0) {
        if (header->allocated == 0) {
            // No more blocks to deallocate simply return.
            if (header->prev == NULL) return;
            
            // Freeing current block, going back to the previous.
            arena->block = header->prev;
            CN_FREE(header);
        }

        header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

        decrease = size > header->allocated ? header->allocated : size;
        header->allocated -= decrease;
        size -= decrease;
    }
}

CNDEF int64_t cn_chained_arena_allocation_info(Cn_Chained_Arena *arena, void *allocation_ptr, uint64_t *offset) {
    Cn_Chained_Arena_Block_Header *header;
    uint8_t *ptr = allocation_ptr;
    Cn_Chained_Arena a = *arena;

    while (true) {
        header = CN_CHAINED_ARENA_BLOCK_HEADER(a.block);
        
        if (ptr >= (uint8_t *)a.block && ptr < ((uint8_t *)a.block + header->allocated)) {
            *offset = ptr - (uint8_t *)a.block;
            break;
        }

        if (header->prev == NULL) {
            // No block was found that contains address supplied by the user.
            return -1;
        }
        
        a.block = header->prev;
    }

    // Count number of blocks before found allocation block (it's index from the start).
    int64_t idx = 0;
    while (header->prev != NULL) {
        idx++;
        a.block = header->prev;
        header = CN_CHAINED_ARENA_BLOCK_HEADER(a.block);
    }

    return idx;
}

CNDEF uint64_t cn_chained_arena_allocated(Cn_Chained_Arena *arena) {
    uint64_t allocated = 0;
    Cn_Chained_Arena a = *arena;
    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(a.block);
    
    while (header->prev != NULL) {
        a.block = header->prev;
        allocated += header->allocated;
        header = CN_CHAINED_ARENA_BLOCK_HEADER(a.block);
    }

    allocated += header->allocated;

    return allocated;
}

CNDEF void cn_chained_arena_free(Cn_Chained_Arena *arena) {
    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

    while (header->prev != NULL) {
        arena->block = header->prev;
        CN_FREE(header);
        header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
    }

    CN_FREE(header);

    arena->block = NULL;
    arena->block_capacity = 0;
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

// HASHING SECTION

CNDEF uint64_t cn_hash_u64(uint64_t value) {
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    value = value ^ (value >> 31);
    return value;
}

CNDEF uint64_t cn_hash_ptr(void *ptr) {
    return cn_hash_u64((uint64_t)ptr);
}

CNDEF uint64_t cn_hash_bytes(void *data, int64_t length) {
    uint8_t *p = (uint8_t *)data;
    uint64_t h = 0xcbf29ce484222325ULL;
    for (int64_t i = 0; i < length; i++) {
        h ^= p[i];
        h *= 0x100000001b3ULL;
    }
    return h;
}

CNDEF uint64_t cn_hash_mix(uint64_t a, uint64_t b) {
    return cn_hash_u64(a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2)));
}

// HASH TABLE SECTION

CNDEF Cn_Hash_Table_Slot *cn__hash_table_get_slot(Cn_Hash_Table_Header *header, int64_t idx) {
    return (Cn_Hash_Table_Slot *)( ((uint8_t *)(header + 1)) + (header->capacity * header->item_size) + (idx * (sizeof(Cn_Hash_Table_Slot) + header->key_size)));
}

CNDEF int64_t cn__hash_table_probe_insert(void *data, Cn_Hash_Table_Header *header, void *item, int64_t idx) {
    Cn_Hash_Table_Slot *slot;
    int64_t insertion_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        insertion_idx = (idx + i) % header->capacity;
        slot = cn__hash_table_get_slot(header, insertion_idx);
        if (slot->state != CN_HASH_TABLE_SLOT_OCCUPIED) {
            memcpy((uint8_t *)data + insertion_idx * header->item_size, item, header->item_size);
            return insertion_idx;
        }
    }
    
    cn_log(CN_ERROR, "No valid entry in the hash table.");
    return -1;
}

CNDEF void cn_hash_table_print(FILE *f, void **table) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    fprintf(f, "\n--------Hash Table--------\n");
    Cn_Hash_Table_Slot *slot;
    uint8_t *item;

    for (int64_t i = 0; i < header->capacity; i++) {
        slot = cn__hash_table_get_slot(header, i);
        item = (uint8_t *)*table + i * header->item_size;

        if (slot->state == CN_HASH_TABLE_SLOT_OCCUPIED) {
            // Print the item in hex based on item_size.
            fprintf(f, "item: 0x");
            for (int64_t i = 0; i < header->item_size; i++) {
                fprintf(f, "%02x", *(item + i)); // Print each byte of the item.
            }

            fprintf(f, " | state: 0x%02x, ", slot->state);
            fprintf(f, "hash: 0x");
            for (int64_t i = 0; i < 8; i++) {
                fprintf(f, "%02x", *((uint8_t *)(&slot->hash) + i)); // Print each byte of the hash.
            }

            fprintf(f, "\n");
        } else {
            fprintf(f, "EMPTY   %*s |\n", (int)header->item_size * 2, "");
        }
    }
}

CNDEF void *cn__hash_table_make(int64_t key_size, int64_t item_size, int64_t capacity, Cn_Hash_Function *hash_func, Cn_Equals_Function *equals_func) {
    CN_ASSERT(item_size > 0);
    CN_ASSERT(capacity > 0);

    Cn_Hash_Table_Header *header = (Cn_Hash_Table_Header *)CN_REALLOC(NULL, sizeof(Cn_Hash_Table_Header) + (item_size + sizeof(Cn_Hash_Table_Slot) + key_size) * capacity);

    if (header == NULL) {
        cn_log(CN_ERROR, "Couldn't allocate more memory of size: %ld bytes, for the hash table.", sizeof(Cn_Hash_Table_Header) + (item_size + sizeof(Cn_Hash_Table_Slot) + key_size) * capacity);
        return NULL;
    }
    
    // Setting all hash table header members.
    header->capacity    = capacity;
    header->item_size   = item_size;
    header->key_size    = key_size;
    header->count       = 0;
    header->hash_func   = hash_func;
    header->equals_func = equals_func;
    header->load_factor = 0.9f;

    uint8_t *data = (uint8_t *)(header + 1);
    Cn_Hash_Table_Slot *slot;
    
    // Make all slots free.
    for (int64_t i = 0; i < header->capacity; i++) {
        slot = (Cn_Hash_Table_Slot *)(data + (header->capacity * header->item_size) + (i * (sizeof(Cn_Hash_Table_Slot) + header->key_size)));
        slot->state = CN_HASH_TABLE_SLOT_EMPTY;
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

CNDEF void cn__hash_table_resize_to_fit(void **table, int64_t requiered_length) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    if (requiered_length > header->capacity * header->load_factor) {
        // IMPORTANT: To understand where this calculation comes from check cn__array_list_resize_to_fit implementation.
        // It uses same calculation that simplifies pow and log of base 2 caluclation to just using bit manipulation.
        int64_t ratio = requiered_length / header->capacity;
        if (ratio < 1) ratio = 1;
        int highest_bit_pos = 63 - CN_COUNT_LEADING_ZEROS(ratio);
        CN_ASSERT(highest_bit_pos >= 0);
        int64_t capacity_multiplier = (int64_t)(1 << (highest_bit_pos + 1));

        Cn_Hash_Table_Header *h = (Cn_Hash_Table_Header *)CN_REALLOC(NULL, sizeof(Cn_Hash_Table_Header) + (header->capacity * capacity_multiplier + 1) * (header->item_size + sizeof(Cn_Hash_Table_Slot) + header->key_size)); // Adding 1 to make next capacity odd.

        if (h == NULL) {
            cn_log(CN_ERROR, "Couldn't allocate more memory to fit new size of: %ld bytes, for the hash table.", sizeof(Cn_Hash_Table_Header) + (header->capacity * capacity_multiplier + 1) * (header->item_size + sizeof(Cn_Hash_Table_Slot)));
            return;
        }

        h->capacity = header->capacity * capacity_multiplier + 1;
        h->count = header->count;
        h->item_size = header->item_size;
        h->key_size = header->key_size;
        h->hash_func = header->hash_func;
        h->equals_func = header->equals_func;
        h->load_factor = header->load_factor;


        // Free new slots to.
        uint8_t *new_data = (uint8_t *)(h + 1);

        {
            Cn_Hash_Table_Slot *slot;
            for (int64_t i = 0; i < h->capacity; i++) {
                slot = (Cn_Hash_Table_Slot *)(new_data + (h->capacity * h->item_size) + (i * (sizeof(Cn_Hash_Table_Slot) + h->key_size)));
                slot->state = CN_HASH_TABLE_SLOT_EMPTY;
            }
        }

        // Readdressing into new hash table.
        uint8_t *old_data = (uint8_t *)(header + 1);

        {
            Cn_Hash_Table_Slot *old_slot, *new_slot;
            int64_t idx;
            for (int64_t i = 0; i < header->capacity; i++) {
                old_slot = cn__hash_table_get_slot(header, i);

                if (old_slot->state == CN_HASH_TABLE_SLOT_OCCUPIED) {
                    idx = old_slot->hash % h->capacity;
                    
                    idx = cn__hash_table_probe_insert(new_data, h, old_data + i * header->item_size, idx);
                    new_slot = cn__hash_table_get_slot(h, idx);
                    new_slot->state = CN_HASH_TABLE_SLOT_OCCUPIED;
                    new_slot->hash = old_slot->hash;
                    memcpy(new_slot + 1, old_slot + 1, header->key_size);

                    header->count--;
                }

                if (header->count == 0) break;
            }
        }


        CN_FREE(header);
        
        *table = new_data;
    }
}

CNDEF int64_t cn__hash_table_push_key(void **table, void *key) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    uint64_t hash = header->hash_func(key);
    int64_t idx = hash % header->capacity;

    Cn_Hash_Table_Slot *slot;
    int64_t insertion_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        insertion_idx = (idx + i) % header->capacity;
        slot = cn__hash_table_get_slot(header, insertion_idx);

        if (slot->state != CN_HASH_TABLE_SLOT_OCCUPIED) {
            slot->state = CN_HASH_TABLE_SLOT_OCCUPIED;
            slot->hash = hash;
            memcpy(slot + 1, key, header->key_size);

            header->count++;

            return insertion_idx;
        } else if (slot->hash == hash && header->equals_func(key, slot + 1)) {
            return insertion_idx;
        }
    }

    cn_log(CN_ERROR, "No free hash table slot for the new key in the hash table.");
    return -1;
}

CNDEF void *cn__hash_table_get(void **table, void *key) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    uint64_t hash = header->hash_func(key);
    int64_t idx = hash % header->capacity;

    Cn_Hash_Table_Slot *slot;
    int64_t probe_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        probe_idx = (idx + i) % header->capacity;
        slot = cn__hash_table_get_slot(header, probe_idx);

        if (slot->state == CN_HASH_TABLE_SLOT_EMPTY) return NULL;

        if (slot->state == CN_HASH_TABLE_SLOT_OCCUPIED && slot->hash == hash && header->equals_func(key, slot + 1)) {
            return (uint8_t *)*table + probe_idx * header->item_size;
        }
    }

    return NULL;
}

CNDEF void cn__hash_table_remove(void **table, void *key) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);

    uint64_t hash = header->hash_func(key);
    int64_t idx = hash % header->capacity;

    Cn_Hash_Table_Slot *slot;
    int64_t probe_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        probe_idx = (idx + i) % header->capacity;
        slot = cn__hash_table_get_slot(header, probe_idx);

        if (slot->state == CN_HASH_TABLE_SLOT_EMPTY) return;

        if (slot->state == CN_HASH_TABLE_SLOT_OCCUPIED && slot->hash == hash && header->equals_func(key, slot + 1)) {
            slot->state = CN_HASH_TABLE_SLOT_DELETED;
            return;
        }
    }

}

CNDEF void cn__hash_table_free(void **table) {
    Cn_Hash_Table_Header *header = cn_hash_table_header(table);
    CN_FREE(header);
    *table = NULL;
}

// HASH SET SECTION

CNDEF Cn_Hash_Set_Slot *cn__hash_set_get_slot(Cn_Hash_Set_Header *header, int64_t idx) {
    return (Cn_Hash_Set_Slot *)( ((uint8_t *)(header + 1) + header->item_size) + (header->capacity * header->item_size) + (idx * (sizeof(Cn_Hash_Set_Slot))));
}

CNDEF int64_t cn__hash_set_probe_insert(void *data, Cn_Hash_Set_Header *header, void *item, int64_t idx) {
    Cn_Hash_Set_Slot *slot;
    int64_t insertion_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        insertion_idx = (idx + i) % header->capacity;
        slot = cn__hash_set_get_slot(header, insertion_idx);
        if (slot->state != CN_HASH_SET_SLOT_OCCUPIED) {
            memcpy((uint8_t *)data + insertion_idx * header->item_size, item, header->item_size);
            return insertion_idx;
        }
    }
    
    cn_log(CN_ERROR, "No valid entry in the hash set.");
    return -1;
}

CNDEF Cn_Hash_Set_Header *cn__hash_set_resize_to_fit(Cn_Hash_Set_Header *header, int64_t requiered_length) {

    if (requiered_length > header->capacity * header->load_factor) {
        // IMPORTANT: To understand where this calculation comes from check cn__array_list_resize_to_fit implementation.
        // It uses same calculation that simplifies pow and log of base 2 caluclation to just using bit manipulation.
        int64_t ratio = requiered_length / header->capacity;
        if (ratio < 1) ratio = 1;
        int highest_bit_pos = 63 - CN_COUNT_LEADING_ZEROS(ratio);
        CN_ASSERT(highest_bit_pos >= 0);
        int64_t capacity_multiplier = (int64_t)(1 << (highest_bit_pos + 1));

         // Adding 1 to make next capacity odd.
        Cn_Hash_Set_Header *h = (Cn_Hash_Set_Header *)CN_REALLOC(NULL, sizeof(Cn_Hash_Set_Header) + header->item_size + (header->item_size + sizeof(Cn_Hash_Set_Slot)) * (header->capacity * capacity_multiplier + 1));

        if (h == NULL) {
            cn_log(CN_ERROR, "Couldn't allocate more memory to fit new size of: %ld bytes, for the hash set.", sizeof(Cn_Hash_Set_Header) + header->item_size + (header->item_size + sizeof(Cn_Hash_Set_Slot)) * (header->capacity * capacity_multiplier + 1));
            return NULL;
        }

        // Copying temp item.
        memcpy(h + 1, header + 1, header->item_size);

        h->capacity = header->capacity * capacity_multiplier + 1;
        h->count = header->count;
        h->item_size = header->item_size;
        h->hash_func = header->hash_func;
        h->equals_func = header->equals_func;
        h->load_factor = header->load_factor;


        // Free new slots to.
        uint8_t *new_data = (uint8_t *)(h + 1) + header->item_size;

        {
            Cn_Hash_Set_Slot *slot;
            for (int64_t i = 0; i < h->capacity; i++) {
                slot = cn__hash_set_get_slot(h, i);
                slot->state = CN_HASH_SET_SLOT_EMPTY;
            }
        }

        // Readdressing into new hash set.
        uint8_t *old_data = (uint8_t *)(header + 1) + header->item_size;

        {
            Cn_Hash_Set_Slot *old_slot, *new_slot;
            int64_t idx;
            for (int64_t i = 0; i < header->capacity; i++) {
                old_slot = cn__hash_set_get_slot(header, i);

                if (old_slot->state == CN_HASH_SET_SLOT_OCCUPIED) {
                    idx = old_slot->hash % h->capacity;
                    
                    idx = cn__hash_set_probe_insert(new_data, h, old_data + i * header->item_size, idx);
                    new_slot = cn__hash_set_get_slot(h, idx);
                    new_slot->state = CN_HASH_SET_SLOT_OCCUPIED;
                    new_slot->hash = old_slot->hash;

                    header->count--;
                }

                if (header->count == 0) break;
            }
        }


        CN_FREE(header);
        
        return h;
    }

    return header;
}

CNDEF void *cn__hash_set_make(int64_t item_size, int64_t capacity, Cn_Hash_Function *hash_func, Cn_Equals_Function *equals_func) {
    CN_ASSERT(item_size > 0);
    CN_ASSERT(capacity > 0);

    Cn_Hash_Set_Header *header = (Cn_Hash_Set_Header *)CN_REALLOC(NULL, sizeof(Cn_Hash_Set_Header) + item_size + (item_size + sizeof(Cn_Hash_Set_Slot)) * (capacity + 1));

    if (header == NULL) {
        cn_log(CN_ERROR, "Couldn't allocate more memory of size: %lld bytes, for the hash set.", sizeof(Cn_Hash_Set_Header) + item_size + (item_size + sizeof(Cn_Hash_Set_Slot)) * (capacity + 1));
        return NULL;
    }

    header->capacity    = capacity;
    header->item_size   = item_size;
    header->count       = 0;
    header->hash_func   = hash_func;
    header->equals_func = equals_func;
    header->load_factor = 0.9f;

    Cn_Hash_Set_Slot *slot;
    for (int64_t i = 0; i < header->capacity; i++) {
        slot = cn__hash_set_get_slot(header, i);
        slot->state = CN_HASH_SET_SLOT_EMPTY;
    }

    return (uint8_t *)(header + 1) + item_size;
}


CNDEF void cn__hash_set_put(Cn_Hash_Set_Header *header, void **set) {
    header = cn__hash_set_resize_to_fit(header, header->count + 1);
    *set = (uint8_t *)(header + 1) + header->item_size;

    uint8_t *item = (uint8_t *)(header + 1);
    uint8_t *data = item + header->item_size;

    uint64_t hash = header->hash_func(item);
    int64_t idx = hash % header->capacity;

    Cn_Hash_Set_Slot *slot;
    int64_t probe_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        probe_idx = (idx + i) % header->capacity;
        slot = cn__hash_set_get_slot(header, probe_idx);
        
        if (slot->state != CN_HASH_SET_SLOT_OCCUPIED) {
            memcpy(data + probe_idx * header->item_size, item, header->item_size);
            slot->state = CN_HASH_SET_SLOT_OCCUPIED;
            slot->hash = hash;
            header->count++;
            return;
        } else if (slot->hash == hash && header->equals_func(item, data + probe_idx * header->item_size)) {
            // Memcpy here to the item, slot so put can use it as return value if item already exists.
            memcpy(item, data + probe_idx * header->item_size, header->item_size);
            return;
        }
    }

    cn_log(CN_ERROR, "Couldn't find free slot for the item in hash set.");
}

CNDEF bool cn__hash_set_contains(Cn_Hash_Set_Header *header) {
    uint8_t *item = (uint8_t *)(header + 1);
    uint8_t *data = item + header->item_size;

    uint64_t hash = header->hash_func(item);
    int64_t idx = hash % header->capacity;

    Cn_Hash_Set_Slot *slot;
    int64_t probe_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        probe_idx = (idx + i) % header->capacity;
        slot = cn__hash_set_get_slot(header, probe_idx);
        
        if (slot->state == CN_HASH_SET_SLOT_EMPTY) return false;

        if (slot->state == CN_HASH_SET_SLOT_OCCUPIED && slot->hash == hash && header->equals_func(item, data + probe_idx * header->item_size)) {
            return true;
        }
    }

    return false;
}

CNDEF void cn__hash_set_remove(Cn_Hash_Set_Header *header) {
    uint8_t *item = (uint8_t *)(header + 1);
    uint8_t *data = item + header->item_size;

    uint64_t hash = header->hash_func(item);
    int64_t idx = hash % header->capacity;

    Cn_Hash_Set_Slot *slot;
    int64_t probe_idx;
    for (int64_t i = 0; i < header->capacity; i++) {
        probe_idx = (idx + i) % header->capacity;
        slot = cn__hash_set_get_slot(header, probe_idx);
        
        if (slot->state == CN_HASH_SET_SLOT_EMPTY) return;

        if (slot->state == CN_HASH_SET_SLOT_OCCUPIED && slot->hash == hash && header->equals_func(item, data + probe_idx * header->item_size)) {
            slot->state = CN_HASH_SET_SLOT_DELETED;
            header->count--;
            return;
        }
    }
}

CNDEF void cn__hash_set_free(Cn_Hash_Set_Header *header) {
    CN_FREE(header);
}

// SOURCE SECTION

CNDEF Cn_String cn_source_to_str(Cn_Source *src) {
    return (Cn_String) {
        .data = src->source.data + src->offset,
        .length = src->length,
    };
}

CNDEF int64_t cn_source_dist(Cn_Source *a, Cn_Source *b) {
    return b->offset - a->offset;
}

CNDEF int64_t cn_source_is_empty(Cn_Source *src) {
    return src->length == 0;
}

// LEXER SECTION

const Cn_Token_Literal CN_KEYWORD_TOKENS[] = {
    { CN_TOKEN_GNU_EXTENSION,   CN_STR_BUFFER("__extension__") }, 
    { CN_TOKEN_GNU_ATTRIBUTE,   CN_STR_BUFFER("__attribute__") }, 
    { CN_TOKEN_CONST,           CN_STR_BUFFER("const") },
    { CN_TOKEN_RESTRICT,        CN_STR_BUFFER("restrict") },
    { CN_TOKEN_RESTRICT,        CN_STR_BUFFER("__restrict") },
    { CN_TOKEN_VOLATILE,        CN_STR_BUFFER("volatile") },
    { CN_TOKEN_ATOMIC,          CN_STR_BUFFER("_Atomic") },
    { CN_TOKEN_STATIC,          CN_STR_BUFFER("static") },
    { CN_TOKEN_EXTERN,          CN_STR_BUFFER("extern") },
    { CN_TOKEN_REGISTER,        CN_STR_BUFFER("register") },
    { CN_TOKEN_AUTO,            CN_STR_BUFFER("auto") },
    { CN_TOKEN_TYPEDEF,         CN_STR_BUFFER("typedef") },
    { CN_TOKEN_SIGNED,          CN_STR_BUFFER("signed") },
    { CN_TOKEN_UNSIGNED,        CN_STR_BUFFER("unsigned") },
    { CN_TOKEN_SHORT,           CN_STR_BUFFER("short") },
    { CN_TOKEN_LONG,            CN_STR_BUFFER("long") },
    { CN_TOKEN_INT,             CN_STR_BUFFER("int") },
    { CN_TOKEN_CHAR,            CN_STR_BUFFER("char") },
    { CN_TOKEN_FLOAT,           CN_STR_BUFFER("float") },
    { CN_TOKEN_DOUBLE,          CN_STR_BUFFER("double") },
    { CN_TOKEN_BOOL,            CN_STR_BUFFER("_Bool") },
    { CN_TOKEN_VOID,            CN_STR_BUFFER("void") },
    { CN_TOKEN_STRUCT,          CN_STR_BUFFER("struct") },
    { CN_TOKEN_ENUM,            CN_STR_BUFFER("enum") },
    { CN_TOKEN_UNION,           CN_STR_BUFFER("union") },
    { CN_TOKEN_GNU_TYPEOF,      CN_STR_BUFFER("typeof") },
    { CN_TOKEN_GNU_TYPEOF,      CN_STR_BUFFER("__typeof") },
    { CN_TOKEN_IF,              CN_STR_BUFFER("if") },
    { CN_TOKEN_ELSE,            CN_STR_BUFFER("else") },
    { CN_TOKEN_SWITCH,          CN_STR_BUFFER("switch") },
    { CN_TOKEN_WHILE,           CN_STR_BUFFER("while") },
    { CN_TOKEN_DO,              CN_STR_BUFFER("do") },
    { CN_TOKEN_FOR,             CN_STR_BUFFER("for") },
    { CN_TOKEN_GOTO,            CN_STR_BUFFER("goto") },
    { CN_TOKEN_CONTINUE,        CN_STR_BUFFER("continue") },
    { CN_TOKEN_BREAK,           CN_STR_BUFFER("break") },
    { CN_TOKEN_RETURN,          CN_STR_BUFFER("return") },
    { CN_TOKEN_SIZEOF,          CN_STR_BUFFER("sizeof") },
    { CN_TOKEN_INLINE,          CN_STR_BUFFER("inline") },
    { CN_TOKEN_INLINE,          CN_STR_BUFFER("__inline") },
    { CN_TOKEN_NORETURN,        CN_STR_BUFFER("_Noreturn") },
    { CN_TOKEN_ASM,             CN_STR_BUFFER("asm") },
    { CN_TOKEN_ASM,             CN_STR_BUFFER("__asm__") },
};
const Cn_Token_Literal CN_LITERAL_TOKENS[] = {
    { CN_TOKEN_ELLIPSIS,            CN_STR_BUFFER("...") },
    { CN_TOKEN_LSHIFT_ASSIGN,       CN_STR_BUFFER("<<=") },
    { CN_TOKEN_RSHIFT_ASSIGN,       CN_STR_BUFFER(">>=") },
    { CN_TOKEN_MULTIPLY_ASSIGN,     CN_STR_BUFFER("*=") },
    { CN_TOKEN_DIVIDE_ASSIGN,       CN_STR_BUFFER("/=") },
    { CN_TOKEN_MODULO_ASSIGN,       CN_STR_BUFFER("%=") },
    { CN_TOKEN_PLUS_ASSIGN,         CN_STR_BUFFER("+=") },
    { CN_TOKEN_MINUS_ASSIGN,        CN_STR_BUFFER("-=") },
    { CN_TOKEN_BIT_AND_ASSIGN,      CN_STR_BUFFER("&=") },
    { CN_TOKEN_BIT_XOR_ASSIGN,      CN_STR_BUFFER("^=") },
    { CN_TOKEN_BIT_OR_ASSIGN,       CN_STR_BUFFER("|=") },
    { CN_TOKEN_ARROW,               CN_STR_BUFFER("->") },
    { CN_TOKEN_INCREMENT,           CN_STR_BUFFER("++") },
    { CN_TOKEN_DECREMENT,           CN_STR_BUFFER("--") },
    { CN_TOKEN_LESS_EQ,             CN_STR_BUFFER("<=") },
    { CN_TOKEN_GREATER_EQ,          CN_STR_BUFFER(">=") },
    { CN_TOKEN_EQ,                  CN_STR_BUFFER("==") },
    { CN_TOKEN_NOT_EQ,              CN_STR_BUFFER("!=") },
    { CN_TOKEN_AND,                 CN_STR_BUFFER("&&") },
    { CN_TOKEN_OR,                  CN_STR_BUFFER("||") },
    { CN_TOKEN_LSHIFT,              CN_STR_BUFFER("<<") },
    { CN_TOKEN_RSHIFT,              CN_STR_BUFFER(">>") },
    { CN_TOKEN_COLON,               CN_STR_BUFFER(":") },
    { CN_TOKEN_SEMICOLON,           CN_STR_BUFFER(";") },
    { CN_TOKEN_PARAN_OPEN,          CN_STR_BUFFER("(") },
    { CN_TOKEN_PARAN_CLOSE,         CN_STR_BUFFER(")") },
    { CN_TOKEN_CURLY_OPEN,          CN_STR_BUFFER("{") },
    { CN_TOKEN_CURLY_CLOSE,         CN_STR_BUFFER("}") },
    { CN_TOKEN_SQR_BRACES_OPEN,     CN_STR_BUFFER("[") },
    { CN_TOKEN_SQR_BRACES_CLOSE,    CN_STR_BUFFER("]") },
    { CN_TOKEN_COMMA,               CN_STR_BUFFER(",") },
    { CN_TOKEN_DOT,                 CN_STR_BUFFER(".") },
    { CN_TOKEN_ASSIGN,              CN_STR_BUFFER("=") },
    { CN_TOKEN_ASTERISK,            CN_STR_BUFFER("*") },
    { CN_TOKEN_SLASH,               CN_STR_BUFFER("/") },
    { CN_TOKEN_PLUS,                CN_STR_BUFFER("+") },
    { CN_TOKEN_MINUS,               CN_STR_BUFFER("-") },
    { CN_TOKEN_PERCENT,             CN_STR_BUFFER("%") },
    { CN_TOKEN_LESS,                CN_STR_BUFFER("<") },
    { CN_TOKEN_GREATER,             CN_STR_BUFFER(">") },
    { CN_TOKEN_AMPERSAND,           CN_STR_BUFFER("&") },
    { CN_TOKEN_HAT,                 CN_STR_BUFFER("^") },
    { CN_TOKEN_BAR,                 CN_STR_BUFFER("|") },
    { CN_TOKEN_EXCLAMATION,         CN_STR_BUFFER("!") },
    { CN_TOKEN_TILDE,               CN_STR_BUFFER("~") },
    { CN_TOKEN_QUESTION,            CN_STR_BUFFER("?") },
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
        l->line++;
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
CNDEF int cn__lexer_get_digits(Cn_Lexer *l, Cn_Token *token) {
    int count = 0;
    while (!cn__lexer_eof(l) && isdigit(l->content.data[l->cursor])) {
        token->src.length++;
        l->cursor++;
        count++;
    }
    return count;
}

/**
 * Gets and consumes, appends to the current token all of the hex digits [0-F] stops when non-hex digit char is reached, not case sensative.
 * RETURNS: Count of added digits.
 */
CNDEF int cn__lexer_get_hex_digits(Cn_Lexer *l, Cn_Token *token) {
    int count = 0;
    while (!cn__lexer_eof(l) && isxdigit(l->content.data[l->cursor])) {
        token->src.length++;
        l->cursor++;
        count++;
    }
    return count;
}

/**
 * Gets and consumes, appends to the current token any integer suffix [l, L, u, U].
 */
CNDEF void cn__lexer_get_integer_suffix(Cn_Lexer *l, Cn_Token *token) {
    while (!cn__lexer_eof(l) && (l->content.data[l->cursor] == 'l' || l->content.data[l->cursor] == 'L' || l->content.data[l->cursor] == 'u' || l->content.data[l->cursor] == 'U')) {
        token->src.length++;
        l->cursor++;
    }
}

/**
 * Gets and consumes, appends to the current token any float suffix [f, F, l, L].
 */
CNDEF void cn__lexer_get_float_suffix(Cn_Lexer *l, Cn_Token *token) {
    while (!cn__lexer_eof(l) && (l->content.data[l->cursor] == 'f' || l->content.data[l->cursor] == 'F' || l->content.data[l->cursor] == 'l' || l->content.data[l->cursor] == 'L')) {
        token->src.length++;
        l->cursor++;
    }
}

CNDEF void cn__lexer_backtrack_token(Cn_Lexer *l, Cn_Token *token) {
    l->cursor -= token->src.length;
    token->src.length = 0;
    token->type = CN_TOKEN_UNKNOWN;
}

/**
 * RETURNS: True if token is blacklisted.
 */
CNDEF bool cn__lexer_blacklisted(Cn_Token *token, Cn_Lexer_Blacklist *blacklist) {
    for (int64_t i = 0; i < blacklist->length; i++) {
        if (token->type == blacklist->ttypes[i]) return true;
    }

    return false;
}

/**
 * Advances lexer state without writing to lexer token queue.
 *
 * RETURNS: Lexed token that can be inserted in the queue.
 */
CNDEF Cn_Token cn__lexer_lex(Cn_Lexer *l) {
    cn__lexer_consume_spaces(l);

    Cn_Token token = (Cn_Token) {
        .type = CN_TOKEN_UNKNOWN,
        .loc = (Cn_Location) { 
            .file = l->file, 
            .line = l->line, 
            .column = l->cursor - l->bol + 1, 
        },
        .src = (Cn_Source) {
            .source = l->content,
            .offset = l->cursor,
            .length = 0, 
        }
    };

    if (cn__lexer_eof(l)) {
        token.type = CN_TOKEN_EOF;
        return token;
    }

    if (cn__valid_number_start(l->content.data[l->cursor])) {
        int digit_count = 0;
        token.type = CN_TOKEN_INTEGER_VALUE;
        
        digit_count += cn__lexer_get_digits(l, &token);
        if (cn__lexer_eof(l)) return token;

        if (l->content.data[l->cursor] == '.') {
            token.type = CN_TOKEN_FLOAT_VALUE;
            token.src.length++;
            l->cursor++;
        }
       
        if (token.type == CN_TOKEN_FLOAT_VALUE && digit_count == 0) {
            cn__lexer_backtrack_token(l, &token);
            goto number_lexer_fail;
        }

        digit_count += cn__lexer_get_digits(l, &token);
        if (cn__lexer_eof(l)) return token;
        
        if (l->content.data[l->cursor] == 'e' || l->content.data[l->cursor] == 'E') {
            token.type = CN_TOKEN_FLOAT_VALUE;
            token.src.length++;
            l->cursor++;

            if (cn__lexer_eof(l)) return token;
            
            if (l->content.data[l->cursor] == '+' || l->content.data[l->cursor] == '-') {
                token.src.length++;
                l->cursor++;
            }
        }

        digit_count += cn__lexer_get_digits(l, &token);
        if (cn__lexer_eof(l)) return token;

        if (token.type == CN_TOKEN_FLOAT_VALUE) {
            if (digit_count == 0) {
                cn__lexer_backtrack_token(l, &token);
                goto number_lexer_fail;
            }
            cn__lexer_get_float_suffix(l, &token);

            return token;
        }

        if (l->content.data[l->cursor - 1] == '0' && digit_count == 1 && (l->content.data[l->cursor] == 'x' || l->content.data[l->cursor] == 'X')) {
            token.src.length++;
            l->cursor++;
            if (cn__lexer_get_hex_digits(l, &token) == 0) {
                return token;
            }
        }

        cn__lexer_get_integer_suffix(l, &token);
        return token;
    }
number_lexer_fail:


    if (l->content.data[l->cursor] == '#') {
        token.type = CN_TOKEN_LINE_MARKER;
        token.src.length++;
        l->cursor++;

        while (l->cursor < l->content.length) {

            // If we reached end of line '\n' cn__lexer_eat_char will return true.
            if (cn__lexer_eat_char(l)) { 
                Cn_String marker = cn_source_to_str(&token.src);
                // Parsing line marker here.
                marker = cn_str_eat_chars(marker, 1);
                marker = cn_str_eat_spaces(marker);

                int64_t line = cn_str_parse_int(cn_str_get_until_space(marker));
                marker = cn_str_eat_until_space(marker);
                marker = cn_str_eat_spaces(marker);
                
                Cn_String file = {0};
                if (marker.data[0] == '"') {
                    marker = cn_str_eat_chars(marker, 1);
                    file.data = marker.data;
                    int64_t end = cn_str_find_char_left(marker, '"');
                    if (end != -1) {
                        file.length = end;
                    }
                }

                // Preserving state.
                l->file = file;
                l->line = line;
                
                return token;
            }
            token.src.length++;
        }

        return token;
    }


    if (l->content.data[l->cursor] == '"') {
        token.type = CN_TOKEN_STRING;
        token.src.length++;
        l->cursor++;

        while (l->cursor < l->content.length) {
            if (l->content.data[l->cursor] == '"' && l->content.data[l->cursor - 1] != '\\') {
                // If a valid string end.
                break;
            }
            token.src.length++;
            l->cursor++;
        }

        token.src.length++;
        l->cursor++;

        return token;
    }


    if (l->content.data[l->cursor] == '/') {
        token.src.length++;
        l->cursor++;
        
        // One line comment.
        if (l->cursor < l->content.length && l->content.data[l->cursor] == '/') {
            token.type = CN_TOKEN_COMMENT;
            token.src.length++;
            l->cursor++;

            while (l->cursor < l->content.length) {

                if (cn__lexer_eat_char(l)) {
                    break;
                }
                token.src.length++;
            }

            return token;
        }

        // Multiline comment.
        if (l->cursor < l->content.length && l->content.data[l->cursor] == '*') {
            token.type = CN_TOKEN_COMMENT;
            token.src.length++;
            l->cursor++;

            while (l->cursor < l->content.length && (l->content.data[l->cursor - 1] != '*' || l->content.data[l->cursor] != '/')) {

                cn__lexer_eat_char(l);
                token.src.length++;
            }


            token.src.length++;
            l->cursor++;

            return token;
        }

        // No comment, backtrack.
        cn__lexer_backtrack_token(l, &token);
    }

    Cn_String comp;
    for (uint64_t i = 0; i < CN_ARRAY_LENGTH(CN_LITERAL_TOKENS); i++) {
        if (l->content.length - l->cursor >= CN_LITERAL_TOKENS[i].literal.length) {
            comp = CN_STR(CN_LITERAL_TOKENS[i].literal.length, l->content.data + l->cursor);
            if (cn_str_equals((Cn_String *)&CN_LITERAL_TOKENS[i].literal, &comp)) {
                token.type = CN_LITERAL_TOKENS[i].type;
                token.src.length = CN_LITERAL_TOKENS[i].literal.length;
                l->cursor += token.src.length;
                return token;
            }
        }
    }


    if (cn__valid_symbol_start(l->content.data[l->cursor])) {
        token.type = CN_TOKEN_IDENTIFIER;
        while (l->cursor < l->content.length && cn__valid_symbol(l->content.data[l->cursor])) {
            token.src.length++;
            l->cursor++;
        }
       
        Cn_String symbol = cn_source_to_str(&token.src);
        for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_KEYWORD_TOKENS); i++) {
            if (cn_str_equals(&CN_KEYWORD_TOKENS[i].literal, &symbol)) {
                token.type = CN_KEYWORD_TOKENS[i].type;
                break;
            }
        }
        
        return token;
    }
    
    token.src.length++;
    l->cursor++;

    return token;
}

CNDEF void cn_lexer_init(Cn_Lexer *lexer, Cn_String content, Cn_Lexer_Blacklist blacklist) {
    *lexer = (Cn_Lexer) {0};
    
    lexer->line = 1;
    lexer->content = content;
    lexer->blacklist = blacklist;

    // Fill queue.
    lexer->tail_idx = 0;
    lexer->current_idx = 0;
    Cn_Token token;
    for (int64_t i = 0; i < CN_LEXER_QUEUE_COUNT; i++) {
        do {
            token = cn__lexer_lex(lexer);
        } while(cn__lexer_blacklisted(&token, &lexer->blacklist));

        lexer->queue[i] = token;

        if (lexer->queue[i].type == CN_TOKEN_EOF) {
            lexer->head_idx = i;
            return;
        }
    }

    lexer->head_idx = CN_LEXER_QUEUE_COUNT - 1;
}

#define cn__lexer_tail_dist(l) (l->current_idx >= l->tail_idx ? l->current_idx - l->tail_idx : CN_LEXER_QUEUE_COUNT - (l->tail_idx - l->current_idx))

#define cn__lexer_head_dist(l) (l->current_idx <=  l->head_idx ? l->head_idx - l->current_idx : CN_LEXER_QUEUE_COUNT - (l->current_idx - l->head_idx))

CNDEF void cn_lexer_next_token(Cn_Lexer *l) {
    // No more tokens left case.
    if (cn_lexer_head(l).type == CN_TOKEN_EOF) {
        if (l->current_idx == l->head_idx) return;

        if (cn__lexer_tail_dist(l) >= CN_LEXER_LOOKDOWN_COUNT) {
            l->tail_idx = (l->tail_idx + 1) % CN_LEXER_QUEUE_COUNT;
        }

        l->current_idx = (l->current_idx + 1) % CN_LEXER_QUEUE_COUNT;

        return;
    }

    Cn_Token next;
    if (cn__lexer_tail_dist(l) >= CN_LEXER_LOOKDOWN_COUNT) {
        l->tail_idx = (l->tail_idx + 1) % CN_LEXER_QUEUE_COUNT;

        do {
            next = cn__lexer_lex(l);
        } while(cn__lexer_blacklisted(&next, &l->blacklist));

        l->head_idx = (l->head_idx + 1) % CN_LEXER_QUEUE_COUNT;
        cn_lexer_head(l) = next;
    }

    l->current_idx = (l->current_idx + 1) % CN_LEXER_QUEUE_COUNT;
}

CNDEF bool cn_lexer_expect(Cn_Lexer *lexer, Cn_Token_Type type) {
    return cn_lexer_token(lexer).type == type;
}

CNDEF Cn_Token cn_lexer_peek(Cn_Lexer *lexer, int64_t offset) {
    CN_ASSERT(-CN_LEXER_LOOKDOWN_COUNT < offset && offset < CN_LEXER_LOOKUP_COUNT);
    
    // Peeking past tokens.
    if (offset < 0) {
        if (-offset > cn__lexer_tail_dist(lexer)) {
            return (Cn_Token) { .type = CN_TOKEN_UNKNOWN };
        }

        return lexer->queue[(lexer->current_idx + offset) % CN_LEXER_QUEUE_COUNT];
    }

    if (offset > cn__lexer_head_dist(lexer)) {
        if (cn_lexer_head(lexer).type == CN_TOKEN_EOF) return cn_lexer_head(lexer);

        return (Cn_Token) { .type = CN_TOKEN_UNKNOWN };
    }
    
    return lexer->queue[(lexer->current_idx + offset) % CN_LEXER_QUEUE_COUNT];
}

CNDEF const char *cn_token_kind_name(Cn_Token_Type type) {
    switch(type) {
        case CN_TOKEN_EOF:              return "<eof>";
        case CN_TOKEN_UNKNOWN:          return "<unknown>";
        case CN_TOKEN_STRING:           return "<string literal>";
        case CN_TOKEN_COMMENT:          return "<comment>";
        case CN_TOKEN_LINE_MARKER:      return "<line marker>";
        case CN_TOKEN_INTEGER_VALUE:    return "<integer literal>";
        case CN_TOKEN_FLOAT_VALUE:      return "<float literal>";
        case CN_TOKEN_IDENTIFIER:       return "<identifier>";
        // Keyword tokens
        case CN_TOKEN_GNU_EXTENSION:    return "__extension__";
        case CN_TOKEN_GNU_ATTRIBUTE:    return "__attribute__";
        case CN_TOKEN_CONST:            return "const";
        case CN_TOKEN_RESTRICT:         return "restrict";
        case CN_TOKEN_VOLATILE:         return "volatile";
        case CN_TOKEN_ATOMIC:           return "_Atomic";
        case CN_TOKEN_STATIC:           return "static";
        case CN_TOKEN_EXTERN:           return "extern";
        case CN_TOKEN_REGISTER:         return "register";
        case CN_TOKEN_AUTO:             return "auto";
        case CN_TOKEN_TYPEDEF:          return "typedef";
        case CN_TOKEN_SIGNED:           return "signed";
        case CN_TOKEN_UNSIGNED:         return "unsigned";
        case CN_TOKEN_SHORT:            return "short";
        case CN_TOKEN_LONG:             return "long";
        case CN_TOKEN_INT:              return "int";
        case CN_TOKEN_CHAR:             return "char";
        case CN_TOKEN_FLOAT:            return "float";
        case CN_TOKEN_DOUBLE:           return "double";
        case CN_TOKEN_BOOL:             return "_Bool";
        case CN_TOKEN_VOID:             return "void";
        case CN_TOKEN_STRUCT:           return "struct";
        case CN_TOKEN_ENUM:             return "enum";
        case CN_TOKEN_UNION:            return "union";
        case CN_TOKEN_GNU_TYPEOF:       return "typeof";
        case CN_TOKEN_IF:               return "if";
        case CN_TOKEN_ELSE:             return "else";
        case CN_TOKEN_SWITCH:           return "switch";
        case CN_TOKEN_WHILE:            return "while";
        case CN_TOKEN_DO:               return "do";
        case CN_TOKEN_FOR:              return "for";
        case CN_TOKEN_GOTO:             return "goto";
        case CN_TOKEN_CONTINUE:         return "continue";
        case CN_TOKEN_BREAK:            return "break";
        case CN_TOKEN_RETURN:           return "return";
        case CN_TOKEN_SIZEOF:           return "sizeof";
        case CN_TOKEN_INLINE:           return "inline";
        case CN_TOKEN_NORETURN:         return "_Noreturn";
        case CN_TOKEN_ASM:              return "asm";
        // Literal tokens
        case CN_TOKEN_COLON:            return ":";
        case CN_TOKEN_SEMICOLON:        return ";";
        case CN_TOKEN_PARAN_OPEN:       return "(";
        case CN_TOKEN_PARAN_CLOSE:      return ")";
        case CN_TOKEN_CURLY_OPEN:       return "{";
        case CN_TOKEN_CURLY_CLOSE:      return "}";
        case CN_TOKEN_SQR_BRACES_OPEN:  return "[";
        case CN_TOKEN_SQR_BRACES_CLOSE: return "]";
        case CN_TOKEN_COMMA:            return ",";
        case CN_TOKEN_DOT:              return ".";
        case CN_TOKEN_ARROW:            return "->";
        case CN_TOKEN_ASSIGN:           return "=";
        case CN_TOKEN_ASTERISK:         return "*";
        case CN_TOKEN_SLASH:            return "/";
        case CN_TOKEN_PLUS:             return "+";
        case CN_TOKEN_MINUS:            return "-";
        case CN_TOKEN_PERCENT:          return "%";
        case CN_TOKEN_INCREMENT:        return "++";
        case CN_TOKEN_DECREMENT:        return "--";
        case CN_TOKEN_LSHIFT:           return "<<";
        case CN_TOKEN_RSHIFT:           return ">>";
        case CN_TOKEN_LESS:             return "<";
        case CN_TOKEN_GREATER:          return ">";
        case CN_TOKEN_LESS_EQ:          return "<=";
        case CN_TOKEN_GREATER_EQ:       return ">=";
        case CN_TOKEN_EQ:               return "==";
        case CN_TOKEN_NOT_EQ:           return "!=";
        case CN_TOKEN_AMPERSAND:        return "&";
        case CN_TOKEN_HAT:              return "^";
        case CN_TOKEN_BAR:              return "|";
        case CN_TOKEN_AND:              return "&&";
        case CN_TOKEN_OR:               return "||";
        case CN_TOKEN_EXCLAMATION:      return "!";
        case CN_TOKEN_TILDE:            return "~";
        case CN_TOKEN_LSHIFT_ASSIGN:    return "<<=";
        case CN_TOKEN_RSHIFT_ASSIGN:    return ">>=";
        case CN_TOKEN_MULTIPLY_ASSIGN:  return "*=";
        case CN_TOKEN_DIVIDE_ASSIGN:    return "/=";
        case CN_TOKEN_MODULO_ASSIGN:    return "%=";
        case CN_TOKEN_PLUS_ASSIGN:      return "+=";
        case CN_TOKEN_MINUS_ASSIGN:     return "-=";
        case CN_TOKEN_BIT_AND_ASSIGN:   return "&=";
        case CN_TOKEN_BIT_XOR_ASSIGN:   return "^=";
        case CN_TOKEN_BIT_OR_ASSIGN:    return "|=";
        case CN_TOKEN_QUESTION:         return "?";
        case CN_TOKEN_ELLIPSIS:         return "...";
    }

    return "<invalid token>";
}

CNDEF void cn_lexer_print_snippet(Cn_Lexer *lexer, uint64_t index, int64_t length) {
    Cn_String line = cn_str_eat_chars(lexer->content, lexer->bol);
    int64_t end = cn_str_find_char_left(line, '\n');
    if (end == -1) {
        end = lexer->content.length - lexer->bol;
    }
    line = cn_str_substring(line, 0, end);
    uint64_t underline_offset = index - lexer->bol;

    fprintf(stderr, "\n%4lu | %.*s", lexer->line, CN_UNPACK(cn_str_substring(line, 0, underline_offset)));
    fprintf(stderr, "\033[31m%.*s\033[0m", CN_UNPACK(cn_str_substring(line, underline_offset, underline_offset + length)));
    fprintf(stderr, "%.*s\n", CN_UNPACK(cn_str_substring(line, underline_offset + length, line.length)));
    fprintf(stderr, "     | %*s\033[31m^", (int)underline_offset, "");

    for(int64_t i = 1; i < length; i++)
        fputc('~', stderr);

    fputs("\033[0m\n\n", stderr);
}


// TYPE SECTION
const Cn_Type CN_TYPE_INT = { 
    .integer.flags = CN_TYPE_COMPLETE,
    .integer.size = sizeof(int), 
    .integer.align = sizeof(int), 
    .integer.kind = CN_INTEGER,
    .integer.is_signed = true,
};

const Cn_Type CN_TYPE_FLOAT = { 
    .flags = CN_TYPE_COMPLETE,
    .size = sizeof(float), 
    .align = sizeof(float), 
    .kind = CN_FLOAT,
};

const Cn_Type CN_TYPE_CHAR = { 
    .integer.flags = CN_TYPE_COMPLETE,
    .integer.size = sizeof(char), 
    .integer.align = sizeof(char), 
    .integer.kind = CN_INTEGER,
    .integer.is_signed = true,
};

const Cn_Type CN_TYPE_VOID = { 
    .size = 0, 
    .align = 0, 
    .kind = CN_VOID,
};

const Cn_Type CN_TYPE_PTRDIFF = { 
    .integer.flags = CN_TYPE_COMPLETE,
    .integer.size = sizeof(ptrdiff_t), 
    .integer.align = sizeof(ptrdiff_t), 
    .integer.kind = CN_INTEGER,
    .integer.is_signed = true,
};

const Cn_Type CN_TYPE_SIZE = {
    .integer.flags = CN_TYPE_COMPLETE,
    .integer.size  = sizeof(size_t),
    .integer.align = sizeof(size_t),
    .integer.kind  = CN_INTEGER,
    .integer.is_signed = false,   // size_t is unsigned.
};

const Cn_Type CN_TYPE_OPAQUE = {
    .flags = CN_TYPE_COMPLETE,
    .kind = CN_OPAQUE,
};

CNDEF Cn_Type cn_type_make_pointer(const Cn_Type *type) {
    return (Cn_Type) {
        .pointer.flags = CN_TYPE_COMPLETE,
        .pointer.size  = sizeof(void *),
        .pointer.align = sizeof(void *),
        .pointer.kind = CN_POINTER,
        .pointer.ptr_to = (Cn_Type *)type,
    };
}

CNDEF Cn_Type cn_type_make_qualified(Cn_Type_Flags flags, const Cn_Type *type) {
    return (Cn_Type) {
        .qualified.kind = CN_QUALIFIED,
        .qualified.flags = flags | CN_TYPE_COMPLETE,
        .qualified.base_type = (Cn_Type *)type,
    };
}

CNDEF bool cn_type_equals(const Cn_Type *a, const Cn_Type *b) {
    if (a == b) return true;

    if (a->kind != b->kind) return false;
    if (a->flags != b->flags) return false;
    if (a->size != b->size && a->align != b->align) return false;

    switch (a->kind) {
        case CN_INTEGER: {
            const Cn_Type_Integer *ia = (const Cn_Type_Integer *)a;
            const Cn_Type_Integer *ib = (const Cn_Type_Integer *)b;
            if (ia->is_signed != ib->is_signed) return false;
        } break;
        case CN_POINTER: {
            const Cn_Type_Pointer *pa = (const Cn_Type_Pointer *)a;
            const Cn_Type_Pointer *pb = (const Cn_Type_Pointer *)b;
            if (pa->ptr_to != pb->ptr_to) return false;
        } break;
        case CN_FUNCTION: {
            const Cn_Type_Function *fa = (const Cn_Type_Function *)a;
            const Cn_Type_Function *fb = (const Cn_Type_Function *)b;
            if (fa->return_type != fb->return_type) return false;
            if (fa->params_length != fb->params_length) return false;
            for (int64_t i = 0; i < fa->params_length; i++) {
                if (fa->params[i].type != fb->params[i].type) return false;
            }
        } break;
        case CN_ARRAY: {
            const Cn_Type_Array *aa = (const Cn_Type_Array *)a;
            const Cn_Type_Array *ab = (const Cn_Type_Array *)b;
            if (aa->element_type != ab->element_type) return false;
            if (aa->length != ab->length) return false;
        } break;
        case CN_STRUCT: {
            const Cn_Type_Struct *sa = (const Cn_Type_Struct *)a;
            const Cn_Type_Struct *sb = (const Cn_Type_Struct *)b;
            if (!cn_str_equals(&sa->tag, &sb->tag)) return false;
            if (sa->members_length != sb->members_length) return false;
            for (int64_t i = 0; i < sa->members_length; i++) {
                if (sa->members[i].type != sb->members[i].type) return false;
                if (!cn_str_equals(&sa->members[i].name, &sb->members[i].name)) return false;
            }
        } break;
        case CN_ENUM:
            break;
        case CN_UNION:
            break;
        case CN_QUALIFIED: {
            const Cn_Type_Qualified *qa = (const Cn_Type_Qualified *)a;
            const Cn_Type_Qualified *qb = (const Cn_Type_Qualified *)b;
            if (qa->base_type != qb->base_type) return false;
        } break;
        default:
            break;
    }

    return true;
}

CNDEF uint64_t cn_type_hash(const Cn_Type *type) {
    uint64_t hash = cn_hash_u64(type->kind);
    hash = cn_hash_mix(hash, cn_hash_u64(type->flags));
    hash = cn_hash_mix(hash, cn_hash_u64(type->size));
    hash = cn_hash_mix(hash, cn_hash_u64(type->align));

    switch (type->kind) {
        case CN_INTEGER: {
            const Cn_Type_Integer *ti = (const Cn_Type_Integer *)type;
            hash = cn_hash_mix(hash, cn_hash_u64(ti->is_signed));
        } break;
        case CN_POINTER: {
            const Cn_Type_Pointer *tp = (const Cn_Type_Pointer *)type;
            hash = cn_hash_mix(hash, cn_hash_ptr(tp->ptr_to));
        } break;
        case CN_FUNCTION: {
            const Cn_Type_Function *tf = (const Cn_Type_Function *)type;
            hash = cn_hash_mix(hash, cn_hash_ptr(tf->return_type));
            hash = cn_hash_mix(hash, cn_hash_u64(tf->params_length));
            for (int64_t i = 0; i < tf->params_length; i++) {
                hash = cn_hash_mix(hash, cn_hash_ptr(tf->params[i].type));
            }
        } break;
        case CN_ARRAY: {
            const Cn_Type_Array *ta = (const Cn_Type_Array *)type;
            hash = cn_hash_mix(hash, cn_hash_ptr(ta->element_type));
            hash = cn_hash_mix(hash, cn_hash_u64(ta->length));
        } break;
        case CN_STRUCT: {
            const Cn_Type_Struct *ts = (const Cn_Type_Struct *)type;
            hash = cn_hash_mix(hash, cn_str_hash(&ts->tag));
            hash = cn_hash_mix(hash, cn_hash_u64(ts->members_length));
            for (int64_t i = 0; i < ts->members_length; i++) {
                hash = cn_hash_mix(hash, cn_hash_ptr(ts->members[i].type));
            }
        } break;
        case CN_ENUM:
            break;
        case CN_UNION:
            break;
        case CN_QUALIFIED: {
            const Cn_Type_Qualified *tq = (const Cn_Type_Qualified *)type;
            hash = cn_hash_mix(hash, cn_hash_ptr(tq->base_type));
        } break;
        default:
            break;
    }

    return hash;
}

CNDEF void cn__type_stringify(const Cn_Type *type, Cn_String_Builder *left, Cn_String_Builder *right) {

    switch (type->kind) {
        case CN_OPAQUE:
            cn_sb_append_str(left, CN_CSTR("<opaque>"));  return;
        case CN_VOID:
            cn_sb_append_str(left, CN_CSTR("void"));      return;
        case CN_BOOL:
            cn_sb_append_str(left, CN_CSTR("_Bool"));     return;
        case CN_UNKNOWN:
            cn_sb_append_str(left, CN_CSTR("<unknown>")); return;

        case CN_INTEGER: {
            const Cn_Type_Integer *ti = (const Cn_Type_Integer *)type;
            if (!ti->is_signed) cn_sb_append_str(left, CN_CSTR("unsigned "));
            switch (type->size) {
                case 1:  cn_sb_append_str(left, CN_CSTR("char"));      return;
                case 2:  cn_sb_append_str(left, CN_CSTR("short"));     return;
                case 4:  cn_sb_append_str(left, CN_CSTR("int"));       return;
                case 8:  cn_sb_append_str(left, CN_CSTR("long long")); return;
                default: cn_sb_append_format(left, "int%lld_t", type->size * 8); return;
            }
        }

        case CN_FLOAT: {
            switch (type->size) {
                case 4:  cn_sb_append_str(left, CN_CSTR("float"));       return;
                case 8:  cn_sb_append_str(left, CN_CSTR("double"));      return;
                case 16: cn_sb_append_str(left, CN_CSTR("long double")); return;
                default: cn_sb_append_format(left, "float%lld", type->size * 8); return;
            }
        }

        case CN_STRUCT: {
            const Cn_Type_Struct *ts = (const Cn_Type_Struct *)type;
            cn_sb_append_str(left, CN_CSTR("struct "));
            cn_sb_append_str(left, ts->tag);
            return;
        }

        case CN_UNION: {
            const Cn_Type_Union *tu = (const Cn_Type_Union *)type;
            cn_sb_append_str(left, CN_CSTR("union "));
            cn_sb_append_str(left, tu->tag);
            return;
        }

        case CN_ENUM: {
            const Cn_Type_Enum *te = (const Cn_Type_Enum *)type;
            cn_sb_append_str(left, CN_CSTR("enum "));
            cn_sb_append_str(left, te->tag);
            return;
        }

        case CN_POINTER: {
            const Cn_Type_Pointer *tp = (const Cn_Type_Pointer *)type;
            Cn_Type *inner = tp->ptr_to;
            bool needs_parens = inner->kind == CN_ARRAY || inner->kind == CN_FUNCTION;
            bool needs_space = cn_type_unqualified(inner)->kind != CN_POINTER && inner->kind != CN_FUNCTION;

            if (needs_parens) {
                Cn_String_Builder inner_right = cn_sb_make(32);

                cn__type_stringify(inner, left, &inner_right);

                if (needs_space) cn_sb_append_char(left, ' ');

                cn_sb_append_str(left, CN_CSTR("(*"));

                cn_sb_append_char(right, ')');
                cn_sb_append_str(right, cn_sb_to_str(&inner_right));

                cn_sb_free(&inner_right);
            } else {
                cn__type_stringify(inner, left, right);

                if (needs_space) cn_sb_append_char(left, ' ');

                cn_sb_append_str(left, CN_CSTR("*"));
            }

            return;
        }

        case CN_ARRAY: {
            const Cn_Type_Array *ta = (const Cn_Type_Array *)type;
            cn__type_stringify(ta->element_type, left, right);
            if (ta->length >= 0)
                cn_sb_append_format(right, "[%lld]", ta->length);
            else
                cn_sb_append_str(right, CN_CSTR("[]"));
            return;
        }

        case CN_FUNCTION: {
            const Cn_Type_Function *tf = (const Cn_Type_Function *)type;
            cn__type_stringify(tf->return_type, left, right);

            cn_sb_append_char(right, '(');
            if (tf->params_length == 0) {
                cn_sb_append_str(right, CN_CSTR("void"));
            } else {
                for (int64_t i = 0; i < tf->params_length; i++) {
                    if (i > 0) cn_sb_append_str(right, CN_CSTR(", "));

                    Cn_String_Builder param_left  = cn_sb_make(32);
                    Cn_String_Builder param_right = cn_sb_make(32);

                    cn__type_stringify(tf->params[i].type, &param_left, &param_right);

                    cn_sb_append_str(right, cn_sb_to_str(&param_left));
                    cn_sb_append_str(right, cn_sb_to_str(&param_right));

                    cn_sb_free(&param_left);
                    cn_sb_free(&param_right);
                }
            }
            cn_sb_append_char(right, ')');
            return;
        }

        case CN_QUALIFIED: {
            const Cn_Type_Qualified *tq = (const Cn_Type_Qualified *)type;
            if (tq->base_type->kind == CN_POINTER) {
                cn__type_stringify(tq->base_type, left, right);
                cn_sb_append_char(left, ' ');
            }

            if (type->flags & CN_TYPE_QUALIFIED_CONSTANT)
                cn_sb_append_str(left, CN_CSTR("const "));
            if (type->flags & CN_TYPE_QUALIFIED_VOLATILE)
                cn_sb_append_str(left, CN_CSTR("volatile "));
            if (type->flags & CN_TYPE_QUALIFIED_RESTRICT)
                cn_sb_append_str(left, CN_CSTR("restrict "));
            if (type->flags & CN_TYPE_QUALIFIED_ATOMIC)
                cn_sb_append_str(left, CN_CSTR("_Atomic "));

            if (tq->base_type->kind != CN_POINTER) {
                cn__type_stringify(tq->base_type, left, right);
            }

            return;
        }
    }
}

CNDEF Cn_String cn_type_stringify(Cn_String buffer, const Cn_Type *type) {
    Cn_String_Builder left  = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    Cn_String_Builder right = cn_sb_make(CN_SB_STACK_STORAGE_CAP);

    cn__type_stringify(type, &left, &right);

    int64_t total = left.length + right.length;
    if (total > buffer.length) {
        cn_log(CN_ERROR, "cn_type_stringify: buffer too small, needed %lld got %lld", total, buffer.length);
        cn_sb_free(&left);
        cn_sb_free(&right);
        return CN_STR(NULL, 0);
    }

    memcpy(buffer.data, left.data, left.length);
    memcpy(buffer.data + left.length,  right.data, right.length);

    cn_sb_free(&left);
    cn_sb_free(&right);

    return CN_STR(total, buffer.data);
}


CNDEF void cn_type_print(const Cn_Type *t) {
    if (t == NULL) { fputs("<null>", stderr); return; }

    char buf[256];
    Cn_String str = cn_type_stringify(CN_STR(sizeof(buf), buf), t);
    if (str.data == NULL) {
        fputs("<stringify failed>", stderr);
        return;
    }

    fprintf(stderr, "%.*s", CN_UNPACK(str));
}

CNDEF bool cn_type_is_constant(const Cn_Type *type) {
    return type->kind == CN_QUALIFIED && (type->flags & CN_TYPE_QUALIFIED_CONSTANT);
}

CNDEF Cn_Type *cn_type_unqualified(const Cn_Type *type) {
    Cn_Type *t = (Cn_Type *)type;
    while (t->kind == CN_QUALIFIED) {
        t = ((Cn_Type_Qualified *)t)->base_type;
    }
    return t;
}

CNDEF bool cn_type_is_arithmetic(const Cn_Type *type) {
    return type->kind == CN_INTEGER || type->kind == CN_FLOAT;
}

CNDEF bool cn_type_is_assignable(const Cn_Type *to, const Cn_Type *from) {
    to = cn_type_unqualified(to);
    from = cn_type_unqualified(from);

    if (to == from) return true;

    // If either type is opaque, then trust those are assignable.
    if (to->kind == CN_OPAQUE || from->kind == CN_OPAQUE)
        return true;

    if (cn_type_is_arithmetic(to) && cn_type_is_arithmetic(from)) return true;

    // If to is pointer.
    if (to->kind == CN_POINTER && from->kind == CN_POINTER) {
        const Cn_Type_Pointer *to_ptr = (const Cn_Type_Pointer *)to;
        const Cn_Type_Pointer *from_ptr = (const Cn_Type_Pointer *)from;
        // Void pointers can be assigned in either direction without explicit casting.
        if (to_ptr->ptr_to->kind == CN_VOID || from_ptr->ptr_to->kind == CN_VOID) return true;

        return cn_type_is_compatible_no_qualifiers(to_ptr->ptr_to, from_ptr->ptr_to);
    }

    // If to is pointer and from is array of same base type. 
    if (to->kind == CN_POINTER && from->kind == CN_ARRAY) {
        const Cn_Type_Pointer *to_ptr = (const Cn_Type_Pointer *)to;
        const Cn_Type_Array *from_arr = (const Cn_Type_Array *)from;

        // Any array to void pointer can be assigned without explicit casting.
        if (to_ptr->ptr_to->kind == CN_VOID) return true;
        
        return cn_type_is_compatible_no_qualifiers(to_ptr->ptr_to, from_arr->element_type);
    }

    return false;
}

CNDEF bool cn_type_is_compatible(const Cn_Type *a, const Cn_Type *b) {
    return a == b;
}

CNDEF bool cn_type_is_compatible_no_qualifiers(const Cn_Type *a, const Cn_Type *b) {
    a = cn_type_unqualified(a);
    b = cn_type_unqualified(b);
    return a == b;
}

CNDEF Cn_Type *cn_type_greatest_arithmetic_rank(const Cn_Type *a, const Cn_Type *b) {
    // Float cases:
    if (a->kind == CN_FLOAT && (b->kind == CN_INTEGER || a->size >= b->size)) {
        return (Cn_Type *)a;
    }
    if (b->kind == CN_FLOAT && (a->kind == CN_INTEGER || b->size >= a->size)) {
        return (Cn_Type *)b;
    }

    // Int uneven ranks:
    if (a->size != b->size) {
        return (Cn_Type *)(a->size > b->size ? a : b);
    }

    // Int even rank: prefer unsigned
    return (Cn_Type *)(!((const Cn_Type_Integer *)a)->is_signed ? a : b);
}

CNDEF bool cn_type_is_scalar(Cn_Type *type) {
    return cn_type_is_arithmetic(type) || type->kind == CN_POINTER || type->kind == CN_BOOL;
}

CNDEF int64_t cn_any_read_int(Cn_Any any) {
    Cn_Type *type = cn_type_unqualified(any.type);
    int64_t size = type->size;
    bool is_signed = (type->kind == CN_INTEGER) && ((Cn_Type_Integer *)type)->is_signed;
 
    switch (size) {
        case 1:
            if (is_signed) { 
                int8_t x; 
                memcpy(&x, any.data, 1); 
                return x; 
            } else { 
                uint8_t x; 
                memcpy(&x, any.data, 1); 
                return x; 
            }
        case 2:
            if (is_signed) { 
                int16_t x; 
                memcpy(&x, any.data, 2); 
                return x; 
            } else { 
                uint16_t x; 
                memcpy(&x, any.data, 2); 
                return x; 
            }
        case 4:
            if (is_signed) { 
                int32_t x; 
                memcpy(&x, any.data, 4); 
                return x; 
            } else { 
                uint32_t x; 
                memcpy(&x, any.data, 4); 
                return x; 
            }
        case 8:
            if (is_signed) { 
                int64_t x; 
                memcpy(&x, any.data, 8); 
                return x; 
            } else { 
                uint64_t x; 
                memcpy(&x, any.data, 8); 
                return x; 
            }
        default:
            { 
                int64_t x = 0; 
                memcpy(&x, any.data, (size_t)size); 
                return x; 
            }
    }
}

CNDEF double cn_any_read_float(Cn_Any any) {
    Cn_Type *type = cn_type_unqualified(any.type);

    if (type->size == (int64_t)sizeof(float)) {
        float x; 
        memcpy(&x, any.data, sizeof(float)); 
        return (double)x;
    }

    double x; 
    memcpy(&x, any.data, sizeof(double)); 
    return x;
}

CNDEF Cn_Any cn_any_write_int(Cn_Any any, int64_t value) {
    Cn_Type *type = cn_type_unqualified(any.type);

    int64_t size = type->size;
    switch (size) {
        case 1: 
            { 
                int8_t x = (int8_t)value;  
                memcpy(any.data, &x, 1); 
                break; 
            }
        case 2: 
            { 
                int16_t x = (int16_t)value; 
                memcpy(any.data, &x, 2); 
                break; 
            }
        case 4: 
            { 
                int32_t x = (int32_t)value; 
                memcpy(any.data, &x, 4); 
                break; 
            }
        case 8: 
            { 
                memcpy(any.data, &value, 8); 
                break; 
            }
        default: 
            { 
                memcpy(any.data, &value, (size_t)size); 
                break; 
            }
    }
    
    return any;
}

CNDEF Cn_Any cn_any_write_float(Cn_Any any, double value) {
    Cn_Type *type = cn_type_unqualified(any.type);

    if (type->size == (int64_t)sizeof(float)) {
        float x = (float)value; 
        memcpy(any.data, &x, sizeof(float));
    } else {
        memcpy(any.data, &value, sizeof(double));
    }

    return any;
}

CNDEF bool cn_any_is_zero(Cn_Any any) {
    if (cn_any_is_empty(any)) return false;

    Cn_Type *type = cn_type_unqualified(any.type);
    
    if (type->kind == CN_FLOAT) return cn_any_read_float(any) == 0.0;

    return cn_any_read_int(any) == 0;
}

CNDEF double cn_any_as_double(Cn_Any any) {
    Cn_Type *type = cn_type_unqualified(any.type);
    if (type->kind == CN_FLOAT) return cn_any_read_float(any);
 
    int64_t raw = cn_any_read_int(any);
    if (type->kind == CN_INTEGER && !((Cn_Type_Integer *)type)->is_signed) {
        return (double)(uint64_t)raw;
    }
    return (double)raw;
}

CNDEF bool cn_double_to_int64(double d, int64_t *out) {
    if (d != d) return false;                       // NaN.
    if (d >=  9223372036854775808.0) return false;  // >= 2^63.
    if (d <  -9223372036854775808.0) return false;  // <  -2^63.
    *out = (int64_t)d;
    return true;
}

CNDEF Cn_Any cn_any_convert(Cn_Any src, Cn_Type *target, void *buffer) {
    if (src.type == NULL || target == NULL) return (Cn_Any) {0};
 
    Cn_Type *target_t = cn_type_unqualified(target);
    Cn_Type *src_t    = cn_type_unqualified(src.type);
 
    if (!cn_type_is_scalar(target_t)) return (Cn_Any) {0};
    if (target_t->kind == CN_POINTER) return (Cn_Any) {0};

    if (!cn_type_is_scalar(src_t)) return (Cn_Any) {0};
    if (src_t->kind == CN_POINTER) return (Cn_Any) {0};
 
    Cn_Any result = { 
        .type = target, 
        .data = buffer 
    };
 
    // Bool case: 0 is 0, non zero is 1.
    if (target_t->kind == CN_BOOL) {
        cn_any_write_int(result, cn_any_is_zero(src) ? 0 : 1);
    } 
    // Float case: any value converted to double, and written to result.
    else if (target_t->kind == CN_FLOAT) {
        cn_any_write_float(result, cn_any_as_double(src));
    } 
    // Integer case:
    else {
        if (src_t->kind == CN_FLOAT) {
            int64_t v;
            // Tries to convert double to int64, 
            // if out of range conversion return NIL.
            if (!cn_double_to_int64(cn_any_read_float(src), &v)) return (Cn_Any) {0};
            cn_any_write_int(result, v);
        } else {
            // Simple int to int, reading and writing will 
            // automatically convert width and signess based on types.
            cn_any_write_int(result, cn_any_read_int(src));
        }
    }
 
    return result;
}

CNDEF bool cn_any_is_empty(Cn_Any any) {
    return any.type == NULL && any.data == NULL;
}

// AST SECTION

Cn_Ast_Checkpoint cn_ast_checkpoint_message = {0};
Cn_Ast_Idx        cn_ast_idx_from_message   = CN_AST_NIL_IDX;

CNDEF Cn_Type_Flags cn_ast_qualifier_flags_to_type(Cn_Qualifier_Flags flags) {
    Cn_Type_Flags f = 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_CONST) ? CN_TYPE_QUALIFIED_CONSTANT : 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_ATOMIC) ? CN_TYPE_QUALIFIED_ATOMIC : 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_RESTRICT) ? CN_TYPE_QUALIFIED_RESTRICT : 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_VOLATILE) ? CN_TYPE_QUALIFIED_VOLATILE : 0;
    return f;
}

const Cn_Binary_Operator CN_BINARY_OPERATORS[] = {
    { CN_BINARY_OP_ARRAY_SUB,       CN_TOKEN_SQR_BRACES_OPEN,   14  },
    { CN_BINARY_OP_FUNCTION,        CN_TOKEN_PARAN_OPEN,        14  },
    { CN_BINARY_OP_DOT,             CN_TOKEN_DOT,               14  },
    { CN_BINARY_OP_ARROW,           CN_TOKEN_ARROW,             14  },
    { CN_BINARY_OP_MULTIPLICATION,  CN_TOKEN_ASTERISK,          12  },
    { CN_BINARY_OP_DIVISION,        CN_TOKEN_SLASH,             12  },
    { CN_BINARY_OP_MODULO,          CN_TOKEN_PERCENT,           12  },
    { CN_BINARY_OP_ADDITION,        CN_TOKEN_PLUS,              11  },
    { CN_BINARY_OP_SUBTRACTION,     CN_TOKEN_MINUS,             11  },
    { CN_BINARY_OP_LSHIFT,          CN_TOKEN_LSHIFT,            10  },
    { CN_BINARY_OP_RSHIFT,          CN_TOKEN_RSHIFT,            10  },
    { CN_BINARY_OP_LESS,            CN_TOKEN_LESS,              9   },
    { CN_BINARY_OP_LESS_EQ,         CN_TOKEN_LESS_EQ,           9   },
    { CN_BINARY_OP_GREATER,         CN_TOKEN_GREATER,           9   },
    { CN_BINARY_OP_GREATER_EQ,      CN_TOKEN_GREATER_EQ,        9   },
    { CN_BINARY_OP_EQ,              CN_TOKEN_EQ,                8   },
    { CN_BINARY_OP_NOT_EQ,          CN_TOKEN_NOT_EQ,            8   },
    { CN_BINARY_OP_BIT_AND,         CN_TOKEN_AMPERSAND,         7   },
    { CN_BINARY_OP_BIT_XOR,         CN_TOKEN_HAT,               6   },
    { CN_BINARY_OP_BIT_OR,          CN_TOKEN_BAR,               5   },
    { CN_BINARY_OP_AND,             CN_TOKEN_AND,               4   },
    { CN_BINARY_OP_OR,              CN_TOKEN_OR,                3   },
    { CN_BINARY_OP_COMMA,           CN_TOKEN_COMMA,             0   },
};

CNDEF Cn_Binary_Operator_Kind cn_ast_is_binary_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_BINARY_OPERATORS); i++) {
        if (cn_lexer_token(lexer).type == CN_BINARY_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_BINARY_OP_NONE;
}

const Cn_Unary_Operator CN_UNARY_OPERATORS[] = {
    { CN_UNARY_OP_INCREMENT,    CN_TOKEN_INCREMENT   },
    { CN_UNARY_OP_DECREMENT,    CN_TOKEN_DECREMENT   },
    { CN_UNARY_OP_POSITIVE,     CN_TOKEN_PLUS        },
    { CN_UNARY_OP_NEGATIVE,     CN_TOKEN_MINUS       },
    { CN_UNARY_OP_NOT,          CN_TOKEN_EXCLAMATION },
    { CN_UNARY_OP_BIT_NOT,      CN_TOKEN_TILDE       },
    { CN_UNARY_OP_DEREF,        CN_TOKEN_ASTERISK    },
    { CN_UNARY_OP_ADDROF,       CN_TOKEN_AMPERSAND   },
};

CNDEF Cn_Unary_Operator_Kind cn_ast_is_unary_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_UNARY_OPERATORS); i++) {
        if (cn_lexer_token(lexer).type == CN_UNARY_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_UNARY_OP_NONE;
}

const Cn_Assignment_Operator CN_ASSIGNMENT_OPERATORS[] = {
    { CN_ASSIGNMENT_OP_ASSIGN,      CN_TOKEN_ASSIGN          },
    { CN_ASSIGNMENT_OP_MULTIPLY,    CN_TOKEN_MULTIPLY_ASSIGN },
    { CN_ASSIGNMENT_OP_DIVIDE,      CN_TOKEN_DIVIDE_ASSIGN   },
    { CN_ASSIGNMENT_OP_MODULO,      CN_TOKEN_MODULO_ASSIGN   },
    { CN_ASSIGNMENT_OP_PLUS,        CN_TOKEN_PLUS_ASSIGN     },
    { CN_ASSIGNMENT_OP_MINUS,       CN_TOKEN_MINUS_ASSIGN    },
    { CN_ASSIGNMENT_OP_LSHIFT,      CN_TOKEN_LSHIFT_ASSIGN   },
    { CN_ASSIGNMENT_OP_RSHIFT,      CN_TOKEN_RSHIFT_ASSIGN   },
    { CN_ASSIGNMENT_OP_BIT_AND,     CN_TOKEN_BIT_AND_ASSIGN  },
    { CN_ASSIGNMENT_OP_BIT_XOR,     CN_TOKEN_BIT_XOR_ASSIGN  },
    { CN_ASSIGNMENT_OP_BIT_OR,      CN_TOKEN_BIT_OR_ASSIGN   },
};

CNDEF Cn_Assignment_Operator_Kind cn_ast_is_assignment_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_ASSIGNMENT_OPERATORS); i++) {
        if (cn_lexer_token(lexer).type == CN_ASSIGNMENT_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_ASSIGNMENT_OP_NONE;
}

const Cn_Postfix_Operator CN_POSTFIX_OPERATORS[] = {
    { CN_POSTFIX_OP_INCREMENT,    CN_TOKEN_INCREMENT },
    { CN_POSTFIX_OP_DECREMENT,    CN_TOKEN_DECREMENT },
};

CNDEF Cn_Postfix_Operator_Kind cn_ast_is_postfix_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_POSTFIX_OPERATORS); i++) {
        if (cn_lexer_token(lexer).type == CN_POSTFIX_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_POSTFIX_OP_NONE;
}

const Cn_Token_Type CN_TOKEN_BLACKLIST[] = { CN_TOKEN_COMMENT, CN_TOKEN_LINE_MARKER };

CNDEF void cn_ast_consume_till(Cn_Lexer *lexer, Cn_Token_Type type) {
    do {
        if (cn_lexer_token(lexer).type  == CN_TOKEN_EOF) break;

        cn_lexer_next_token(lexer);
    } while (cn_lexer_token(lexer).type != type) ;
}

Cn_Ast_Data *cn__ast_data = NULL;

CNDEF void cn__ast_checkpoint_save(Cn_Ast_Checkpoint *checkpoint, Cn_Ast_Data *data, Cn_Ast_Checkpoint_Flags flags) {
    CN_ASSERT(cn_array_list_length(&data->scope_stack) > 0);
    // Locking scope stack.
    data->scope_stack[cn_array_list_length(&data->scope_stack) - 1].is_checkpoint_locked = true;

    checkpoint->flags = flags;
    if (!(checkpoint->flags & CN_AST_CHECKPOINT_IGNORE_AST_NODES)) {
        checkpoint->saved_node_length = cn_array_list_length(&data->node_list);
    }
    checkpoint->saved_type_length = cn_chained_arena_allocated(&data->type_arena);
    checkpoint->saved_type_children_length = cn_chained_arena_allocated(&data->type_children_arena);
    checkpoint->saved_binding_length = cn_array_list_length(&data->binding_list);
    checkpoint->saved_scoped_strings_length = cn_chained_arena_allocated(&data->scoped_strings_arena);
    // checkpoint->saved_permanent_strings_length = cn_chained_arena_allocated(&data->permanent_strings_arena);
    checkpoint->saved_scope_length = cn_array_list_length(&data->scope_stack);
    checkpoint->saved_idx_stack_length = cn_array_list_length(&data->idx_stack);
    checkpoint->saved_counter = data->counter;
    checkpoint->data = data;
}

CNDEF void cn_ast_checkpoint_load(Cn_Ast_Checkpoint *checkpoint) {
    CN_ASSERT(checkpoint->data != NULL);

    Cn_Ast_Data *d = checkpoint->data;

    if (!(checkpoint->flags & CN_AST_CHECKPOINT_IGNORE_AST_NODES)) {
        cn_array_list_pop_multiple(&d->node_list, cn_array_list_length(&d->node_list) - checkpoint->saved_node_length);
    }

    // Removing types from hashset entries.
    int64_t types_count = (cn_chained_arena_allocated(&d->type_arena) - checkpoint->saved_type_length) / sizeof(Cn_Type);
    if (types_count > 0) {
        Cn_Chained_Arena *arena = &d->type_arena;
        Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
        Cn_Type *ptr = (Cn_Type *)((uint8_t *)arena->block + header->allocated) - 1;

        while (true) {
            cn_hash_set_remove(&d->type_ptr_set, ptr);
            types_count--;
            if (types_count <= 0) break;

            ptr--;
            if ((uint8_t *)ptr < (uint8_t *)arena->block) {
                if (header->prev == NULL) break;

                arena->block = header->prev;
                header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
                ptr = (Cn_Type *)((uint8_t *)arena->block + header->allocated) - 1;
            }
        }

        cn_chained_arena_dealloc(&d->type_arena, cn_chained_arena_allocated(&d->type_arena) - checkpoint->saved_type_length);
        cn_chained_arena_dealloc(&d->type_children_arena, cn_chained_arena_allocated(&d->type_children_arena) - checkpoint->saved_type_children_length);
    }


    // Removing bindings from hashtable entries.
    for (int i = checkpoint->saved_binding_length; i < cn_array_list_length(&cn__ast_data->binding_list); i++) {
        // Resolve each binding, properly dispose each binding.
        if (cn__ast_data->binding_list[i].kind == CN_BINDING_TAG) {
            if (cn__ast_data->binding_list[i].next_idx == CN_AST_NIL_IDX) {
                cn_hash_table_remove(&cn__ast_data->tag_binding_table, &cn__ast_data->binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__ast_data->tag_binding_table, cn__ast_data->binding_list[i].next_idx, &cn__ast_data->binding_list[i].name);
            }
        } else {
            if (cn__ast_data->binding_list[i].next_idx == CN_AST_NIL_IDX) {
                cn_hash_table_remove(&cn__ast_data->symbol_binding_table, &cn__ast_data->binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__ast_data->symbol_binding_table, cn__ast_data->binding_list[i].next_idx, &cn__ast_data->binding_list[i].name);
            }
        }
    }
    cn_array_list_pop_multiple(&d->binding_list, cn_array_list_length(&d->binding_list) - checkpoint->saved_binding_length);

    cn_chained_arena_dealloc(&d->scoped_strings_arena, cn_chained_arena_allocated(&d->scoped_strings_arena) - checkpoint->saved_scoped_strings_length);

    // cn_chained_arena_dealloc(&d->permanent_strings_arena, cn_chained_arena_allocated(&d->permanent_strings_arena) - checkpoint->saved_permanent_strings_length);

    cn_array_list_pop_multiple(&d->scope_stack, cn_array_list_length(&d->scope_stack) - checkpoint->saved_scope_length);

    cn_array_list_pop_multiple(&d->idx_stack, cn_array_list_length(&d->idx_stack) - checkpoint->saved_idx_stack_length);

    d->counter = checkpoint->saved_counter;

    longjmp(checkpoint->jmpbuf, 1);
}

CNDEF void cn_ast_checkpoint_remove(Cn_Ast_Checkpoint *checkpoint) {
    CN_ASSERT(checkpoint->data != NULL);

    Cn_Ast_Data *d = checkpoint->data;

    CN_ASSERT(cn_array_list_length(&d->scope_stack) > 0);
    // Unlocking scope stack.
    d->scope_stack[cn_array_list_length(&d->scope_stack) - 1].is_checkpoint_locked = false;
    *checkpoint = (Cn_Ast_Checkpoint) {0};
}

CNDEF int cn_ast_init(Cn_Ast_Data *data) {
    data->node_list = cn_array_list_make(Cn_Ast_Node, CN_AST_NODE_LIST_INITIAL_CAP);

    // Inserting first element as NIL.
    {
        Cn_Ast_Node nil = {0};
        cn_array_list_append(&data->node_list, nil);
        if (data->node_list == NULL) return -1;

        Cn_Ast_Node err = { .kind = CN_AST_ERROR };

        cn_array_list_append(&data->node_list, err);
        if (data->node_list == NULL) return -1;
    }

    data->type_arena = cn_chained_arena_make(CN_AST_TYPE_ARENA_BLOCK_CAP);

    data->type_ptr_set = cn_hash_set_make(Cn_Type *, CN_AST_TYPE_PTR_SET_INITIAL_CAP, (Cn_Hash_Function *)cn_ast_type_ptr_hash, (Cn_Equals_Function *)cn_ast_type_ptr_equals);

    data->type_children_arena = cn_chained_arena_make(CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP);

    data->binding_list = cn_array_list_make(Cn_Ast_Binding, CN_AST_BINDING_LIST_INITIAL_CAP);
    
    // Inserting first element as NIL.
    {
        Cn_Ast_Binding nil = {0};
        cn_array_list_append(&data->binding_list, nil);
        if (data->binding_list == NULL) return -1;
    }

    data->tag_binding_table = cn_hash_table_make(Cn_String, Cn_Ast_Idx, CN_AST_TAG_BINDING_TABLE_INITIAL_CAP, (Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);

    data->symbol_binding_table = cn_hash_table_make(Cn_String, Cn_Ast_Idx, CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP,(Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);

    data->scoped_strings_arena = cn_chained_arena_make(CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP);

    data->permanent_strings_arena = cn_chained_arena_make(CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP);

    data->scope_stack = cn_array_list_make(Cn_Ast_Scope, CN_AST_SCOPE_STACK_INITIAL_CAP);

    data->idx_stack = cn_array_list_make(Cn_Ast_Idx, CN_AST_IDX_STACK_INITIAL_CAP);

    data->warning_count = 0;
    data->error_count = 0;

    cn__ast_data = data;

    return 0;
}

CNDEF void cn_ast_free(Cn_Ast_Data *data) {
    cn_array_list_free(&data->idx_stack);

    cn_array_list_free(&data->scope_stack);

    cn_chained_arena_free(&data->permanent_strings_arena);

    cn_chained_arena_free(&data->scoped_strings_arena);

    cn_hash_table_free(&data->symbol_binding_table);

    cn_hash_table_free(&data->tag_binding_table);

    cn_array_list_free(&data->binding_list);

    cn_chained_arena_free(&data->type_children_arena);

    cn_hash_set_free(&data->type_ptr_set);

    cn_chained_arena_free(&data->type_arena);

    cn_array_list_free(&data->node_list);

    *data = (Cn_Ast_Data) {0};
}

CNDEF void *cn_ast_get(Cn_Ast_Idx idx) {
    CN_ASSERT(idx != CN_AST_NIL_IDX);

    Cn_Ast_Node *n = (cn__ast_data->node_list + (idx));
    if (n->flags & CN_AST_USE_REPLACED) return cn_ast_get(n->replaced_idx);
    return n;
}

CNDEF void cn__ast_node_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_Idx idxs[], size_t length) {
    // Ignores alternate routing that cn_ast_node_get gurantees on replace.
    for (size_t i = 0; i < length; i++)
        if (idxs[i] != CN_AST_NIL_IDX)
            (cn__ast_data->node_list + idxs[i])->base.parent_idx = parent_idx;
}

CNDEF Cn_Ast_List cn_ast_idx_stack_finalize(int64_t mark) {
    int64_t stack_len = cn_array_list_length(&cn__ast_data->idx_stack);
    int64_t count = stack_len - mark;

    if (count <= 0) {
        return (Cn_Ast_List) { .idxs = NULL, .length = 0 };
    }

    // Allocate from permanent_strings_arena
    Cn_Ast_Idx *idxs = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, count * sizeof(Cn_Ast_Idx));

    // Copy indices from stack
    memcpy(idxs, cn__ast_data->idx_stack + mark, count * sizeof(Cn_Ast_Idx));

    // Pop the finalized indices from stack
    cn_array_list_pop_multiple(&cn__ast_data->idx_stack, count);

    return (Cn_Ast_List) { .idxs = idxs, .length = count };
}

CNDEF void cn_ast_list_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_List *list) {
    for (int64_t i = 0; i < list->length; i++) {
        if (list->idxs[i] != CN_AST_NIL_IDX) {
            (cn__ast_data->node_list + list->idxs[i])->base.parent_idx = parent_idx;
        }
    }
}

CNDEF Cn_Ast_Idx cn_ast_node_list_append(Cn_Ast_Node node) {
    cn_array_list_append(&cn__ast_data->node_list, node);
    return cn_array_list_length(&cn__ast_data->node_list) - 1;
}

const char *cn_ast_print_prefixes[64] = {0};

const char *CN_AST_PRINT_EMPTY_TAB = "    ";
const char *CN_AST_PRINT_FLAT_TAB  = "│   ";
const char *CN_AST_PRINT_SPLIT_TAB = "├── ";
const char *CN_AST_PRINT_LAST_TAB  = "└── ";

#define CN__AST_PRINT_TABS(depth, prefix) for (int i = 0; i < depth; i++) { fprintf(stderr, "%s", cn_ast_print_prefixes[i]); } fprintf(stderr, "%s", prefix)
#define CN__AST_PRINT_LAST()  do { cn_ast_print_prefixes[depth] = CN_AST_PRINT_EMPTY_TAB; CN__AST_PRINT_TABS(depth, CN_AST_PRINT_LAST_TAB); } while(0)
#define CN__AST_PRINT_SPLIT() do { CN__AST_PRINT_TABS(depth, CN_AST_PRINT_SPLIT_TAB); } while(0)

CNDEF void cn__ast_print_list(Cn_Ast_List list, const char *list_name, int depth) {
    CN_ASSERT(depth < (int)CN_ARRAY_LENGTH(cn_ast_print_prefixes));

    cn_ast_print_prefixes[depth] = CN_AST_PRINT_FLAT_TAB;

    if (list.length > 0) {
        fprintf(stderr, "LIST %s:"CN_LINE_END, list_name);
        for (int64_t i = 0; i < list.length - 1; i++) {
            CN__AST_PRINT_SPLIT();
            cn_ast_print(list.idxs[i], depth + 1);
        }

        CN__AST_PRINT_LAST();
        cn_ast_print(list.idxs[list.length - 1], depth + 1);
    } else {
        fprintf(stderr, "LIST %s: <empty>"CN_LINE_END, list_name);
    }

    cn_ast_print_prefixes[depth] = NULL;
}

CNDEF void cn_ast_print(Cn_Ast_Idx idx, int depth) {
    CN_ASSERT(depth < (int)CN_ARRAY_LENGTH(cn_ast_print_prefixes));

    CN_ASSERT(idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_get(idx);

    fprintf(stderr, "%s", cn_ast_node_kind_name(node->kind));
    cn_ast_print_prefixes[depth] = CN_AST_PRINT_FLAT_TAB;
    
    int             next            = 0;
    void *          idxs[8]         = {0};
    const char *    list_names[8]   = {0};

#define ADD_IDX(idx_ptr) do { if (*(idx_ptr) != CN_AST_NIL_IDX) { CN_ASSERT(next < (int)CN_ARRAY_LENGTH(idxs)); idxs[next++] = (idx_ptr); } } while(0)
#define ADD_LIST(list_ptr, name) do { CN_ASSERT(next < (int)CN_ARRAY_LENGTH(idxs)); idxs[next] = (list_ptr); list_names[next++] = (name); } while(0)

    switch(node->kind) {
        case CN_AST_UNKNOWN:
            break;
        case CN_AST_ERROR:
            break;
        case CN_AST_CODE:
            break;
        case CN_AST_TRANSLATION_UNIT:
            ADD_LIST(&node->translation_unit.external_declarations, "external_declarations");
            break;
        case CN_AST_EXTERNAL_DECLARATION:
            ADD_IDX(&node->external_declaration.child_idx);
            break;
        case CN_AST_DECLARATION:
            ADD_LIST(&node->declaration.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->declaration.declaration_specifiers_idx);
            ADD_LIST(&node->declaration.init_declarators, "init_declarators");
            ADD_LIST(&node->declaration.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            break;
        case CN_AST_FUNCTION:
            ADD_LIST(&node->function.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->function.declaration_specifiers_idx);
            ADD_IDX(&node->function.declarator_idx);
            ADD_IDX(&node->function.block_idx);
            break;
        case CN_AST_BLOCK:
            ADD_LIST(&node->block.statements, "statements");
            break;
        case CN_AST_IF:
            ADD_IDX(&node->if_statement.condition_idx);
            ADD_IDX(&node->if_statement.then_idx);
            ADD_IDX(&node->if_statement.else_idx);
            break;
        case CN_AST_SWITCH:
            ADD_IDX(&node->switch_statement.condition_idx);
            ADD_IDX(&node->switch_statement.body_idx);
            break;
        case CN_AST_WHILE:
            break;
        case CN_AST_DO_WHILE:
            break;
        case CN_AST_FOR:
            break;
        case CN_AST_GOTO:
            ADD_IDX(&node->goto_statement.identifier_idx);
            break;
        case CN_AST_RETURN:
            ADD_IDX(&node->return_statement.expression_idx);
            break;
        case CN_AST_EXPRESSION_STATEMENT:
            ADD_IDX(&node->expression_statement.expression_idx);
            break;
        case CN_AST_BINARY:
            ADD_IDX(&node->binary.left_idx);
            ADD_IDX(&node->binary.right_idx);
            break;
        case CN_AST_ACCESS:
            ADD_IDX(&node->access.expression_idx);
            ADD_IDX(&node->access.member_idx);
            break;
        case CN_AST_CALL:
            ADD_IDX(&node->call.expression_idx);
            ADD_LIST(&node->call.arguments, "arguments");
            break;
        case CN_AST_UNARY:
            ADD_IDX(&node->unary.expression_idx);
            break;
        case CN_AST_CAST:
            ADD_IDX(&node->cast.type_name_idx);
            ADD_IDX(&node->cast.expression_idx);
            break;
        case CN_AST_COMPOUND:
            ADD_IDX(&node->compound.type_name_idx);
            ADD_LIST(&node->compound.designations, "designations");
            break;
        case CN_AST_SIZEOF:
            ADD_IDX(&node->sizeof_expression.target_idx);
            break;
        case CN_AST_TERNARY:
            ADD_IDX(&node->ternary.condition_idx);
            ADD_IDX(&node->ternary.true_idx);
            ADD_IDX(&node->ternary.false_idx);
            break;
        case CN_AST_ASSIGN:
            ADD_IDX(&node->assign.left_idx);
            ADD_IDX(&node->assign.right_idx);
            break;
        case CN_AST_POSTFIX:
            ADD_IDX(&node->postfix.expression_idx);
            break;
        case CN_AST_PRIMARY:
            ADD_IDX(&node->primary.literal_idx);
            break;
        case CN_AST_IDENTIFIER:
            fprintf(stderr, " %.*s", CN_UNPACK(node->identifier.name));
            break;
        case CN_AST_INTEGER:
            fprintf(stderr, " %.*s", CN_UNPACK(node->integer.value));
            break;
        case CN_AST_FLOAT:
            fprintf(stderr, " %.*s", CN_UNPACK(node->flt.value));
            break;
        case CN_AST_STRING:
            fprintf(stderr, " \"%.*s\"", CN_UNPACK(node->string.str));
            break;
        case CN_AST_INIT_DECLARATOR:
            ADD_IDX(&node->init_declarator.declarator_idx);
            ADD_IDX(&node->init_declarator.initializer_idx);
            ADD_IDX(&node->init_declarator.gnu_asm_label_idx);
            break;
        case CN_AST_INITIALIZER:
            ADD_IDX(&node->initializer.expression_idx);
            ADD_LIST(&node->initializer.designations, "designations");
            break;
        case CN_AST_DESIGNATION:
            ADD_LIST(&node->designation.designators, "designators");
            ADD_IDX(&node->designation.initializer_idx);
            break;
        case CN_AST_DESIGNATOR:
            ADD_IDX(&node->designator.identifier_idx);
            ADD_IDX(&node->designator.expression_idx);
            ADD_IDX(&node->designator.expression_range_end_idx);
            break;
        case CN_AST_DECLARATOR:
            if (node->flags & CN_AST_DECLARATOR_IS_ABSTRACT)
                fprintf(stderr, " <abstract>");
                
            ADD_IDX(&node->declarator.pointer_idx);
            ADD_IDX(&node->declarator.direct_declarator_idx);
            break;
        case CN_AST_POINTER:
            ADD_IDX(&node->pointer.pointer_idx);
            break;
        case CN_AST_DIRECT_DECLARATOR_GROUPED:
            ADD_IDX(&node->direct_declarator_grouped.declarator_idx);
            break;
        case CN_AST_DIRECT_DECLARATOR_ARRAY:
            ADD_IDX(&node->direct_declarator_array.direct_declarator_idx);
            ADD_IDX(&node->direct_declarator_array.expression_idx);
            break;
        case CN_AST_DIRECT_DECLARATOR_FUNCTION:
            ADD_IDX(&node->direct_declarator_function.direct_declarator_idx);
            ADD_LIST(&node->direct_declarator_function.parameter_declarations, "parameter_declarations");
            break;
        case CN_AST_DECLARATION_SPECIFIERS:
            ADD_LIST(&node->declaration_specifiers.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            ADD_IDX(&node->declaration_specifiers.type_specifier_idx);
            break;
        case CN_AST_GNU_TYPEOF:
            ADD_IDX(&node->gnu_typeof.target_idx);
            break;
        case CN_AST_TYPE_SPECIFIER_PRIMITIVE:
            switch (node->type_specifier_primitive.sign) {
                case CN_AST_TYPE_SIGN_NONE: 
                    break;
                case CN_AST_TYPE_SIGN_SIGNED:
                    fprintf(stderr, " signed");
                    break;
                case CN_AST_TYPE_SIGN_UNSIGNED:
                    fprintf(stderr, " unsigned");
                    break;
            }

            switch (node->type_specifier_primitive.width) {
                case CN_AST_TYPE_WIDTH_NONE:
                    break;
                case CN_AST_TYPE_WIDTH_SHORT:
                    fprintf(stderr, " short");
                    break;
                case CN_AST_TYPE_WIDTH_LONG:
                    fprintf(stderr, " long");
                    break;
                case CN_AST_TYPE_WIDTH_LONG_LONG:
                    fprintf(stderr, " long long");
                    break;
            }

            switch (node->type_specifier_primitive.primitive_kind) {
                case CN_AST_TYPE_NONE:
                    break;
                case CN_AST_TYPE_INT:
                    fprintf(stderr, " int");
                    break;
                case CN_AST_TYPE_CHAR:
                    fprintf(stderr, " char");
                    break;
                case CN_AST_TYPE_FLOAT:
                    fprintf(stderr, " float");
                    break;
                case CN_AST_TYPE_DOUBLE:
                    fprintf(stderr, " double");
                    break;
                case CN_AST_TYPE_BOOL:
                    fprintf(stderr, " _Bool");
                    break;
                case CN_AST_TYPE_VOID:
                    fprintf(stderr, " void");
                    break;
            }
            break;
        case CN_AST_TYPE_SPECIFIER_TYPEDEF:
            fprintf(stderr, " %.*s", CN_UNPACK(node->type_specifier_typedef.typedef_name));
            break;
        case CN_AST_TYPE_NAME:
            ADD_IDX(&node->type_name.specifier_qualifier_idx);
            ADD_IDX(&node->type_name.abstract_declarator_idx);
            break;
        case CN_AST_SPECIFIER_QUALIFIER:
            ADD_IDX(&node->specifier_qualifier.type_specifier_idx);
            break;
        case CN_AST_PARAMETER_DECLARATION:
            ADD_IDX(&node->parameter_declaration.declaration_specifiers_idx);
            ADD_IDX(&node->parameter_declaration.declarator_idx);
            break;
        case CN_AST_STRUCT_SPECIFIER:
            ADD_IDX(&node->struct_specifier.identifier_idx);
            ADD_LIST(&node->struct_specifier.member_declarations, "member_declarations");
            break;
        case CN_AST_UNION_SPECIFIER:
            ADD_IDX(&node->union_specifier.identifier_idx);
            ADD_LIST(&node->union_specifier.member_declarations, "member_declarations");
            break;
        case CN_AST_MEMBER_DECLARATION:
            ADD_IDX(&node->member_declaration.specifier_qualifier_idx);
            ADD_LIST(&node->member_declaration.member_declarators, "member_declarators");
            break;
        case CN_AST_MEMBER_DECLARATOR:
            ADD_IDX(&node->member_declarator.declarator_idx);
            ADD_IDX(&node->member_declarator.bitfield_idx);
            break;
        case CN_AST_ENUM_SPECIFIER:
            ADD_IDX(&node->enum_specifier.identifier_idx);
            ADD_LIST(&node->enum_specifier.attribute_specifiers, "attribute_specifiers");
            ADD_LIST(&node->enum_specifier.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            ADD_IDX(&node->enum_specifier.specifier_qualifier_idx);
            ADD_LIST(&node->enum_specifier.enumerators, "enumerators");
            break;
        case CN_AST_ENUMERATOR:
            ADD_IDX(&node->enumerator.identifier_idx);
            ADD_LIST(&node->enumerator.attribute_specifiers, "attribute_specifiers");
            ADD_LIST(&node->enumerator.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            ADD_IDX(&node->enumerator.expression_idx);
            break;
        case CN_AST_ATTRIBUTE_SPECIFIER:
            ADD_LIST(&node->attribute_specifier.attributes, "attributes");
            break;
        case CN_AST_ATTRIBUTE:
            ADD_IDX(&node->attribute.vendor_identifier_idx);
            ADD_IDX(&node->attribute.identifier_idx);
            ADD_LIST(&node->attribute.arguments, "arguments");
            break;
        case CN_AST_GNU_ATTRIBUTE_SPECIFIER:
            ADD_LIST(&node->gnu_attribute_specifier.gnu_attributes, "gnu_attributes");
            break;
        case CN_AST_GNU_ATTRIBUTE:
            ADD_IDX(&node->gnu_attribute.identifier_idx);
            ADD_LIST(&node->gnu_attribute.arguments, "arguments");
            break;
        case CN_AST_GNU_ASM_LABEL:
            ADD_IDX(&node->gnu_asm_label.string_idx);
            break;
        case CN_AST_BREAK:
        case CN_AST_CONTINUE:
            return;
    }

#undef ADD_IDX
#undef ADD_LIST

    fputs(CN_LINE_END, stderr);

    for (int i = 0; i < next; i++) {
        if (i == next - 1) {
            CN__AST_PRINT_LAST();
            if (list_names[i] != NULL) 
                cn__ast_print_list(*((Cn_Ast_List *)idxs[i]), list_names[i], depth + 1);
            else 
                cn_ast_print(*((Cn_Ast_Idx *)idxs[i]), depth + 1);
        } else {
            CN__AST_PRINT_SPLIT();
            if (list_names[i] != NULL) 
                cn__ast_print_list(*((Cn_Ast_List *)idxs[i]), list_names[i],  depth + 1);
            else 
                cn_ast_print(*((Cn_Ast_Idx *)idxs[i]), depth + 1);
        }
    }

    cn_ast_print_prefixes[depth] = NULL;
}

CNDEF const char *cn_ast_node_kind_name(Cn_Ast_Kind kind) {
    switch (kind) {
        case CN_AST_UNKNOWN:    return "UNKNOWN";
        case CN_AST_ERROR:      return "ERROR";
#define X(K, T, m) case CN_AST_##K: return #K;
    CN_AST_GEN_LIST(X)
#undef X
#define X(K) case CN_AST_##K: return #K;
    CN_AST_GEN_TAG_LIST(X)
#undef X
    }
    return "<invalid kind>";
}

CNDEF void cn_emit_write_file(Cn_String str, void *ctx) {
    fwrite(str.data, 1, (size_t)str.length, (FILE *)ctx);
}

CNDEF void cn_emit_write_sb(Cn_String str, void *ctx) {
    cn_sb_append_str((Cn_String_Builder *)ctx, str);
}
 
// Helper to emit string and track written_length.
CNDEF void cn__emit_str(Cn_String str, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    func(str, opt->ctx);
    *opt->written_length += str.length;
}

// Helper to emit indentation.
CNDEF void cn__emit_indent(Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    for (int i = 0; i < opt->indent; i++) cn__emit_str(CN_STR_LIT(CN_INDENT), func, opt);
}

// Helper to emit newline with max_lines tracking.
// Returns 1 if max_lines limit reached, 0 otherwise.
CNDEF int cn__emit_newline(Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    cn__emit_str(CN_STR_LIT(CN_LINE_END), func, opt);
    if (opt->max_lines > 0) {
        opt->max_lines--;
        if (opt->max_lines == 0) return 1;
    }
    return 0;
}

// Internal emit function that takes pointer to opt.
CNDEF int cn__emit_opt(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);

// Forward declarations for mutual recursion (ordered as ast_parse/reparse functions).
CNDEF void cn__emit_storage_specifiers(Cn_Storage_Specifier_Flags storage, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF void cn__emit_qualifiers(Cn_Qualifier_Flags qualifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF void cn__emit_function_specifiers(Cn_Function_Specifier_Flags func_spec, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_gnu_attribute_specifiers(Cn_Ast_List specifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_designations(Cn_Ast_List designations, Cn_Emit_Write *func, Cn_Emit_Opt *opt);


CNDEF int cn__emit_translation_unit(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Translation_Unit *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TRANSLATION_UNIT);

    int ok;
    for (int64_t i = 0; i < node->external_declarations.length; i++) {
        ok = cn__emit_opt(node->external_declarations.idxs[i], func, opt);
        if (ok != 0) return ok;
    }

    return 0;
}

CNDEF int cn__emit_external_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_External_Declaration *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_EXTERNAL_DECLARATION);

    int ok;
    if (node->flags & CN_AST_EXTERNAL_DECLARATION_HAS_EXTENSION) {
        cn__emit_str(CN_STR_LIT("__extension__ "), func, opt);
    }
    if (node->child_idx == CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(";"), func, opt);
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;
    } else {
        ok = cn__emit_opt(node->child_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Declaration *node = (Cn_Ast_Declaration *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DECLARATION);

    int ok;
    cn__emit_indent(func, opt);
    ok = cn__emit_opt(node->declaration_specifiers_idx, func, opt);
    if (ok != 0) return ok;

    for (int64_t i = 0; i < node->init_declarators.length; i++) {
        if (i == 0) cn__emit_str(CN_STR_LIT(" "), func, opt);
        else cn__emit_str(CN_STR_LIT(", "), func, opt);
        ok = cn__emit_opt(node->init_declarators.idxs[i], func, opt);
        if (ok != 0) return ok;
    }

    // Emit trailing GNU attribute specifier sequence.
    ok = cn__emit_gnu_attribute_specifiers(node->gnu_attribute_specifiers, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_function(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Function *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_FUNCTION);

    int ok;
    cn__emit_indent(func, opt);
    ok = cn__emit_opt(node->declaration_specifiers_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(" "), func, opt);
    ok = cn__emit_opt(node->declarator_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(" "), func, opt);
    return cn__emit_opt(node->block_idx, func, opt);
}

CNDEF int cn__emit_block(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Block *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BLOCK);

    int ok;
    cn__emit_str(CN_STR_LIT("{"), func, opt);
    ok = cn__emit_newline(func, opt);
    if (ok != 0) return ok;

    opt->indent++;

    for (int64_t i = 0; i < node->statements.length; i++) {
        ok = cn__emit_opt(node->statements.idxs[i], func, opt);
        if (ok != 0) { opt->indent--; return ok; }
    }


    opt->indent--;
    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("}"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_if_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_If *node = (Cn_Ast_If *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_IF);

    int ok;
    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("if ("), func, opt);
    ok = cn__emit_opt(node->condition_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(") "), func, opt);

    Cn_Ast_Node *then_stmt = cn_ast_get(node->then_idx);
    if (then_stmt->kind == CN_AST_BLOCK) {
        ok = cn__emit_opt(node->then_idx, func, opt);
        if (ok != 0) return ok;
    } else {
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;

        opt->indent++;
        ok = cn__emit_opt(node->then_idx, func, opt);
        opt->indent--;
        if (ok != 0) return ok;
    }

    if (node->else_idx != CN_AST_NIL_IDX) {
        cn__emit_indent(func, opt);
        cn__emit_str(CN_STR_LIT("else "), func, opt);
        Cn_Ast_Node *else_stmt = cn_ast_get(node->else_idx);
        if (else_stmt->kind == CN_AST_BLOCK || else_stmt->kind == CN_AST_IF) {
            ok = cn__emit_opt(node->else_idx, func, opt);
            if (ok != 0) return ok;
        } else {
            ok = cn__emit_newline(func, opt);
            if (ok != 0) return ok;

            opt->indent++;
            ok = cn__emit_opt(node->else_idx, func, opt);
            opt->indent--;
            if (ok != 0) return ok;
        }
    }
    return 0;
}

CNDEF int cn__emit_switch_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Switch *node = (Cn_Ast_Switch *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_SWITCH);

    int ok;
    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("switch ("), func, opt);
    ok = cn__emit_opt(node->condition_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(") "), func, opt);

    Cn_Ast_Node *body = cn_ast_get(node->body_idx);
    if (body->kind == CN_AST_BLOCK) {
        ok = cn__emit_opt(node->body_idx, func, opt);
        if (ok != 0) return ok;
    } else {
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;

        opt->indent++;
        ok = cn__emit_opt(node->body_idx, func, opt);
        opt->indent--;
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_while_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    return 0;
}

CNDEF int cn__emit_do_while(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    return 0;
}

CNDEF int cn__emit_for_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    return 0;
}

CNDEF int cn__emit_goto_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Goto *node = (Cn_Ast_Goto *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GOTO);

    int ok;
    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("goto "), func, opt);
    ok = cn__emit_opt(node->identifier_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_return_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Return *node = (Cn_Ast_Return *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_RETURN);

    int ok;
    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("return"), func, opt);
    if (node->expression_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(node->expression_idx, func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_break_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BREAK);

    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("break;"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_continue_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CONTINUE);

    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("continue;"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_expression_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Expression_Statement *node = (Cn_Ast_Expression_Statement *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_EXPRESSION_STATEMENT);

    int ok;
    cn__emit_indent(func, opt);
    if (node->expression_idx != CN_AST_NIL_IDX) {
        ok = cn__emit_opt(node->expression_idx, func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_unary(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Unary *node = (Cn_Ast_Unary *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_UNARY);

    Cn_String op = {0};
    switch (node->operator) {
        case CN_UNARY_OP_INCREMENT: op = CN_STR_LIT("++"); break;
        case CN_UNARY_OP_DECREMENT: op = CN_STR_LIT("--"); break;
        case CN_UNARY_OP_POSITIVE:  op = CN_STR_LIT("+"); break;
        case CN_UNARY_OP_NEGATIVE:  op = CN_STR_LIT("-"); break;
        case CN_UNARY_OP_NOT:       op = CN_STR_LIT("!"); break;
        case CN_UNARY_OP_BIT_NOT:   op = CN_STR_LIT("~"); break;
        case CN_UNARY_OP_DEREF:     op = CN_STR_LIT("*"); break;
        case CN_UNARY_OP_ADDROF:    op = CN_STR_LIT("&"); break;
        default: break;
    }
    cn__emit_str(op, func, opt);
    return cn__emit_opt(node->expression_idx, func, opt);
}

CNDEF int cn__emit_postfix(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Postfix *node = (Cn_Ast_Postfix *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_POSTFIX);

    int ok = cn__emit_opt(node->expression_idx, func, opt);
    if (ok != 0) return ok;

    switch (node->operator) {
        case CN_POSTFIX_OP_INCREMENT: cn__emit_str(CN_STR_LIT("++"), func, opt); break;
        case CN_POSTFIX_OP_DECREMENT: cn__emit_str(CN_STR_LIT("--"), func, opt); break;
        default: break;
    }
    return 0;
}

CNDEF int cn__emit_binary(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Binary *node = (Cn_Ast_Binary *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BINARY);

    int ok;

    // Special case for array subscript.
    if (node->operator == CN_BINARY_OP_ARRAY_SUB) {
        cn__emit_str(CN_STR_LIT("("), func, opt);
        ok = cn__emit_opt(node->left_idx, func, opt);
        if (ok != 0) return ok;
        cn__emit_str(CN_STR_LIT("["), func, opt);
        ok = cn__emit_opt(node->right_idx, func, opt);
        if (ok != 0) return ok;
        cn__emit_str(CN_STR_LIT("])"), func, opt);
        return 0;
    }

    Cn_String op = {0};
    switch (node->operator) {
        case CN_BINARY_OP_MULTIPLICATION: op = CN_STR_LIT(" * "); break;
        case CN_BINARY_OP_DIVISION:       op = CN_STR_LIT(" / "); break;
        case CN_BINARY_OP_MODULO:         op = CN_STR_LIT(" % "); break;
        case CN_BINARY_OP_ADDITION:       op = CN_STR_LIT(" + "); break;
        case CN_BINARY_OP_SUBTRACTION:    op = CN_STR_LIT(" - "); break;
        case CN_BINARY_OP_LSHIFT:         op = CN_STR_LIT(" << "); break;
        case CN_BINARY_OP_RSHIFT:         op = CN_STR_LIT(" >> "); break;
        case CN_BINARY_OP_LESS:           op = CN_STR_LIT(" < "); break;
        case CN_BINARY_OP_LESS_EQ:        op = CN_STR_LIT(" <= "); break;
        case CN_BINARY_OP_GREATER:        op = CN_STR_LIT(" > "); break;
        case CN_BINARY_OP_GREATER_EQ:     op = CN_STR_LIT(" >= "); break;
        case CN_BINARY_OP_EQ:             op = CN_STR_LIT(" == "); break;
        case CN_BINARY_OP_NOT_EQ:         op = CN_STR_LIT(" != "); break;
        case CN_BINARY_OP_BIT_AND:        op = CN_STR_LIT(" & "); break;
        case CN_BINARY_OP_BIT_XOR:        op = CN_STR_LIT(" ^ "); break;
        case CN_BINARY_OP_BIT_OR:         op = CN_STR_LIT(" | "); break;
        case CN_BINARY_OP_AND:            op = CN_STR_LIT(" && "); break;
        case CN_BINARY_OP_OR:             op = CN_STR_LIT(" || "); break;
        case CN_BINARY_OP_COMMA:          op = CN_STR_LIT(", "); break;
        default: break;
    }

    cn__emit_str(CN_STR_LIT("("), func, opt);
    ok = cn__emit_opt(node->left_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(op, func, opt);
    ok = cn__emit_opt(node->right_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_access(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Access *node = (Cn_Ast_Access *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ACCESS);

    int ok = cn__emit_opt(node->expression_idx, func, opt);
    if (ok != 0) return ok;

    if (node->pointer) {
        cn__emit_str(CN_STR_LIT("->"), func, opt);
    } else {
        cn__emit_str(CN_STR_LIT("."), func, opt);
    }

    return cn__emit_opt(node->member_idx, func, opt);
}

CNDEF int cn__emit_call(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Call *node = (Cn_Ast_Call *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CALL);

    int ok = cn__emit_opt(node->expression_idx, func, opt);
    if (ok != 0) return ok;

    cn__emit_str(CN_STR_LIT("("), func, opt);
    for (int64_t i = 0; i < node->arguments.length; i++) {
        if (i > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
        ok = cn__emit_opt(node->arguments.idxs[i], func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_cast(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Cast *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CAST);

    int ok;
    cn__emit_str(CN_STR_LIT("("), func, opt);
    ok = cn__emit_opt(node->type_name_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return cn__emit_opt(node->expression_idx, func, opt);
}

CNDEF int cn__emit_compound(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Compound *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CAST);

    int ok;
    cn__emit_str(CN_STR_LIT("("), func, opt);
    ok = cn__emit_opt(node->type_name_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(") "), func, opt);

    cn__emit_str(CN_STR_LIT("{ "), func, opt);
    ok = cn__emit_designations(node->designations, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT("}"), func, opt);

    return 0;
}

CNDEF int cn__emit_sizeof_expression(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Sizeof *node = (Cn_Ast_Sizeof *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_SIZEOF);

    int ok;
    cn__emit_str(CN_STR_LIT("sizeof("), func, opt);
    ok = cn__emit_opt(node->target_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_ternary(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Ternary *node = (Cn_Ast_Ternary *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TERNARY);

    int ok;
    cn__emit_str(CN_STR_LIT("("), func, opt);
    ok = cn__emit_opt(node->condition_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(" ? "), func, opt);
    ok = cn__emit_opt(node->true_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(" : "), func, opt);
    ok = cn__emit_opt(node->false_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_assign(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Assign *node = (Cn_Ast_Assign *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ASSIGN);

    Cn_String op = {0};
    switch (node->operator) {
        case CN_ASSIGNMENT_OP_ASSIGN:   op = CN_STR_LIT(" = "); break;
        case CN_ASSIGNMENT_OP_MULTIPLY: op = CN_STR_LIT(" *= "); break;
        case CN_ASSIGNMENT_OP_DIVIDE:   op = CN_STR_LIT(" /= "); break;
        case CN_ASSIGNMENT_OP_MODULO:   op = CN_STR_LIT(" %= "); break;
        case CN_ASSIGNMENT_OP_PLUS:     op = CN_STR_LIT(" += "); break;
        case CN_ASSIGNMENT_OP_MINUS:    op = CN_STR_LIT(" -= "); break;
        case CN_ASSIGNMENT_OP_LSHIFT:   op = CN_STR_LIT(" <<= "); break;
        case CN_ASSIGNMENT_OP_RSHIFT:   op = CN_STR_LIT(" >>= "); break;
        case CN_ASSIGNMENT_OP_BIT_AND:  op = CN_STR_LIT(" &= "); break;
        case CN_ASSIGNMENT_OP_BIT_XOR:  op = CN_STR_LIT(" ^= "); break;
        case CN_ASSIGNMENT_OP_BIT_OR:   op = CN_STR_LIT(" |= "); break;
        default: break;
    }

    int ok;
    cn__emit_str(CN_STR_LIT("("), func, opt);
    ok = cn__emit_opt(node->left_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(op, func, opt);
    ok = cn__emit_opt(node->right_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_primary(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Primary *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_PRIMARY);

    return cn__emit_opt(node->literal_idx, func, opt);
}

CNDEF int cn__emit_initializer(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Initializer *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_INITIALIZER);

    if (node->expression_idx != CN_AST_NIL_IDX) {
        return cn__emit_opt(node->expression_idx, func, opt);
    }
    
    cn__emit_str(CN_STR_LIT("{ "), func, opt);
    int ok = cn__emit_designations(node->designations, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT("}"), func, opt);

    return 0;
}

CNDEF int cn__emit_designations(Cn_Ast_List designations, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    int ok;
    for (int64_t i = 0; i < designations.length; i++) {
        ok = cn__emit_opt(designations.idxs[i], func, opt);
        if (ok != 0) return ok;

        if (i < designations.length - 1) {
            cn__emit_str(CN_STR_LIT(", "), func, opt);
        } else {
            cn__emit_str(CN_STR_LIT(" "), func, opt);
        }
    }

    return 0;
}

CNDEF int cn__emit_designation(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Designation *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DESIGNATION);
    
    int ok;
    for (int64_t i = 0; i < node->designators.length; i++) {
        ok = cn__emit_opt(node->designators.idxs[i], func, opt);
        if (ok != 0) return ok;
        
        if (i == node->designators.length - 1) {
            cn__emit_str(CN_STR_LIT(" = "), func, opt);
        }
    }

    return cn__emit_initializer(node->initializer_idx, func, opt);
}

CNDEF int cn__emit_designator(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Designator *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DESIGNATOR);

    if (node->identifier_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT("."), func, opt);
        return cn__emit_opt(node->identifier_idx, func, opt);
    }

    int ok;
    cn__emit_str(CN_STR_LIT("["), func, opt);

    ok = cn__emit_opt(node->expression_idx, func, opt);
    if (ok != 0) return ok;

    if (node->expression_range_end_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" ... "), func, opt);
        ok = cn__emit_opt(node->expression_range_end_idx, func, opt);
        if (ok != 0) return ok;
    }

    cn__emit_str(CN_STR_LIT("]"), func, opt);

    return 0;
}

CNDEF int cn__emit_declarator(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Declarator *node = (Cn_Ast_Declarator *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DECLARATOR);

    int ok = cn__emit_opt(node->pointer_idx, func, opt);
    if (ok != 0) return ok;
    return cn__emit_opt(node->direct_declarator_idx, func, opt);
}

CNDEF int cn__emit_pointer(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Pointer *node = (Cn_Ast_Pointer *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_POINTER);

    cn__emit_str(CN_STR_LIT("*"), func, opt);
    cn__emit_qualifiers(node->qualifiers, func, opt);
    return cn__emit_opt(node->pointer_idx, func, opt);
}

CNDEF int cn__emit_direct_declarator_grouped(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Direct_Declarator_Grouped *node = (Cn_Ast_Direct_Declarator_Grouped *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DIRECT_DECLARATOR_GROUPED);

    cn__emit_str(CN_STR_LIT("("), func, opt);
    int ok = cn__emit_opt(node->declarator_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_direct_declarator_array(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Direct_Declarator_Array *node = (Cn_Ast_Direct_Declarator_Array *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DIRECT_DECLARATOR_ARRAY);

    int ok = cn__emit_opt(node->direct_declarator_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT("["), func, opt);
    if (node->expression_idx != CN_AST_NIL_IDX) {
        ok = cn__emit_opt(node->expression_idx, func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT("]"), func, opt);
    return 0;
}

CNDEF int cn__emit_direct_declarator_function(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Direct_Declarator_Function *node = (Cn_Ast_Direct_Declarator_Function *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DIRECT_DECLARATOR_FUNCTION);

    int ok = cn__emit_opt(node->direct_declarator_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT("("), func, opt);
    for (int64_t i = 0; i < node->parameter_declarations.length; i++) {
        if (i > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
        ok = cn__emit_opt(node->parameter_declarations.idxs[i], func, opt);
        if (ok != 0) return ok;
    }
    if (node->flags & CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC) {
        if (node->parameter_declarations.length > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
        cn__emit_str(CN_STR_LIT("..."), func, opt);
    }
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_declaration_specifiers(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Declaration_Specifiers *node = (Cn_Ast_Declaration_Specifiers *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DECLARATION_SPECIFIERS);

    int ok;
    cn__emit_storage_specifiers(node->storage_specifiers, func, opt);
    cn__emit_function_specifiers(node->function_specifiers, func, opt);
    ok = cn__emit_gnu_attribute_specifiers(node->gnu_attribute_specifiers, func, opt);
    if (ok != 0) return ok;
    cn__emit_qualifiers(node->qualifiers, func, opt);
    return cn__emit_opt(node->type_specifier_idx, func, opt);
}

CNDEF void cn__emit_storage_specifiers(Cn_Storage_Specifier_Flags storage, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (storage & CN_STORAGE_SPECIFIER_TYPEDEF)  cn__emit_str(CN_STR_LIT("typedef "), func, opt);
    if (storage & CN_STORAGE_SPECIFIER_EXTERN)   cn__emit_str(CN_STR_LIT("extern "), func, opt);
    if (storage & CN_STORAGE_SPECIFIER_STATIC)   cn__emit_str(CN_STR_LIT("static "), func, opt);
    if (storage & CN_STORAGE_SPECIFIER_AUTO)     cn__emit_str(CN_STR_LIT("auto "), func, opt);
    if (storage & CN_STORAGE_SPECIFIER_REGISTER) cn__emit_str(CN_STR_LIT("register "), func, opt);
}

CNDEF void cn__emit_qualifiers(Cn_Qualifier_Flags qualifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (qualifiers & CN_AST_TYPE_QUALIFIER_CONST)    cn__emit_str(CN_STR_LIT("const "), func, opt);
    if (qualifiers & CN_AST_TYPE_QUALIFIER_RESTRICT) cn__emit_str(CN_STR_LIT("restrict "), func, opt);
    if (qualifiers & CN_AST_TYPE_QUALIFIER_VOLATILE) cn__emit_str(CN_STR_LIT("volatile "), func, opt);
    if (qualifiers & CN_AST_TYPE_QUALIFIER_ATOMIC)   cn__emit_str(CN_STR_LIT("_Atomic "), func, opt);
}

CNDEF void cn__emit_function_specifiers(Cn_Function_Specifier_Flags func_spec, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (func_spec & CN_AST_FUNCTION_SPECIFIER_INLINE)   cn__emit_str(CN_STR_LIT("inline "), func, opt);
    if (func_spec & CN_AST_FUNCTION_SPECIFIER_NORETURN) cn__emit_str(CN_STR_LIT("_Noreturn "), func, opt);
}

CNDEF int cn__emit_type_specifier_primitive(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Type_Specifier_Primitive *node = (Cn_Ast_Type_Specifier_Primitive *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TYPE_SPECIFIER_PRIMITIVE);

    // Sign.
    switch (node->sign) {
        case CN_AST_TYPE_SIGN_SIGNED:   cn__emit_str(CN_STR_LIT("signed "), func, opt); break;
        case CN_AST_TYPE_SIGN_UNSIGNED: cn__emit_str(CN_STR_LIT("unsigned "), func, opt); break;
        default: break;
    }

    // Width.
    switch (node->width) {
        case CN_AST_TYPE_WIDTH_SHORT:     cn__emit_str(CN_STR_LIT("short "), func, opt); break;
        case CN_AST_TYPE_WIDTH_LONG:      cn__emit_str(CN_STR_LIT("long "), func, opt); break;
        case CN_AST_TYPE_WIDTH_LONG_LONG: cn__emit_str(CN_STR_LIT("long long "), func, opt); break;
        default: break;
    }

    // Base type.
    switch (node->primitive_kind) {
        case CN_AST_TYPE_INT:    cn__emit_str(CN_STR_LIT("int"), func, opt); break;
        case CN_AST_TYPE_CHAR:   cn__emit_str(CN_STR_LIT("char"), func, opt); break;
        case CN_AST_TYPE_FLOAT:  cn__emit_str(CN_STR_LIT("float"), func, opt); break;
        case CN_AST_TYPE_DOUBLE: cn__emit_str(CN_STR_LIT("double"), func, opt); break;
        case CN_AST_TYPE_BOOL:   cn__emit_str(CN_STR_LIT("_Bool"), func, opt); break;
        case CN_AST_TYPE_VOID:   cn__emit_str(CN_STR_LIT("void"), func, opt); break;
        default: break;
    }
    return 0;
}

CNDEF int cn__emit_type_specifier_typedef(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Type_Specifier_Typedef *node = (Cn_Ast_Type_Specifier_Typedef *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TYPE_SPECIFIER_TYPEDEF);

    cn__emit_str(node->typedef_name, func, opt);
    return 0;
}

CNDEF int cn__emit_struct_specifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Struct_Specifier *node = (Cn_Ast_Struct_Specifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_STRUCT_SPECIFIER);

    int ok;
    cn__emit_str(CN_STR_LIT("struct "), func, opt);

    ok = cn__emit_opt(node->identifier_idx, func, opt);
    if (ok != 0) return ok;

    // Only emit body if there are members.
    if (node->member_declarations.length > 0) {
        cn__emit_str(CN_STR_LIT(" {"), func, opt);
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;

        opt->indent++;
        for (int64_t i = 0; i < node->member_declarations.length; i++) {
            ok = cn__emit_opt(node->member_declarations.idxs[i], func, opt);
            if (ok != 0) { opt->indent--; return ok; }
        }
        opt->indent--;
        cn__emit_str(CN_STR_LIT("}"), func, opt);
    }
    return 0;
}

CNDEF int cn__emit_union_specifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Union_Specifier *node = (Cn_Ast_Union_Specifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_UNION_SPECIFIER);

    int ok;
    cn__emit_str(CN_STR_LIT("union "), func, opt);

    ok = cn__emit_opt(node->identifier_idx, func, opt);
    if (ok != 0) return ok;

    if (node->member_declarations.length > 0) {
        cn__emit_str(CN_STR_LIT(" {"), func, opt);
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;

        opt->indent++;
        for (int64_t i = 0; i < node->member_declarations.length; i++) {
            ok = cn__emit_opt(node->member_declarations.idxs[i], func, opt);
            if (ok != 0) { opt->indent--; return ok; }
        }
        opt->indent--;
        cn__emit_str(CN_STR_LIT("}"), func, opt);
    }
    return 0;
}

CNDEF int cn__emit_enum_specifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Enum_Specifier *node = (Cn_Ast_Enum_Specifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ENUM_SPECIFIER);

    // TODO: Implement enum emission
    CN_UNUSED(node);
    CN_UNUSED(func);
    CN_UNUSED(opt);
    return 0;
}

CNDEF int cn__emit_gnu_typeof(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Gnu_Typeof *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_TYPEOF);

    int ok;
    cn__emit_str(CN_STR_LIT("__typeof__("), func, opt);
    ok = cn__emit_opt(node->target_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_type_name(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Type_Name *node = (Cn_Ast_Type_Name *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TYPE_NAME);

    int ok = cn__emit_opt(node->specifier_qualifier_idx, func, opt);
    if (ok != 0) return ok;

    if (node->abstract_declarator_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(node->abstract_declarator_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_specifier_qualifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Specifier_Qualifier *node = (Cn_Ast_Specifier_Qualifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_SPECIFIER_QUALIFIER);

    cn__emit_qualifiers(node->qualifiers, func, opt);
    return cn__emit_opt(node->type_specifier_idx, func, opt);
}

CNDEF int cn__emit_identifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Identifier *node = (Cn_Ast_Identifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_IDENTIFIER);

    cn__emit_str(node->name, func, opt);
    return 0;
}

CNDEF int cn__emit_integer(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Integer *node = (Cn_Ast_Integer *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_INTEGER);

    cn__emit_str(node->value, func, opt);
    return 0;
}

CNDEF int cn__emit_flt(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Float *node = (Cn_Ast_Float *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_FLOAT);

    cn__emit_str(node->value, func, opt);
    return 0;
}

CNDEF int cn__emit_string(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_String *node = (Cn_Ast_String *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_STRING);

    cn__emit_str(CN_STR_LIT("\""), func, opt);
    cn__emit_str(node->str, func, opt);
    cn__emit_str(CN_STR_LIT("\""), func, opt);
    return 0;
}

CNDEF int cn__emit_member_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Member_Declaration *node = (Cn_Ast_Member_Declaration *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_MEMBER_DECLARATION);

    int ok;
    cn__emit_indent(func, opt);
    ok = cn__emit_opt(node->specifier_qualifier_idx, func, opt);
    if (ok != 0) return ok;

    for (int64_t i = 0; i < node->member_declarators.length; i++) {
        if (i > 0) cn__emit_str(CN_STR_LIT(","), func, opt);
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(node->member_declarators.idxs[i], func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_member_declarator(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Member_Declarator *node = (Cn_Ast_Member_Declarator *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_MEMBER_DECLARATOR);

    int ok;
    if (node->declarator_idx != CN_AST_NIL_IDX) {
        ok = cn__emit_opt(node->declarator_idx, func, opt);
        if (ok != 0) return ok;
    }
    if (node->bitfield_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" : "), func, opt);
        ok = cn__emit_opt(node->bitfield_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_gnu_attribute_specifiers(Cn_Ast_List specifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    int ok;
    for (int64_t i = 0; i < specifiers.length; i++) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(specifiers.idxs[i], func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_gnu_attribute_specifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Gnu_Attribute_Specifier *node = (Cn_Ast_Gnu_Attribute_Specifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_ATTRIBUTE_SPECIFIER);

    int ok;
    cn__emit_str(CN_STR_LIT("__attribute__(("), func, opt);
    for (int64_t i = 0; i < node->gnu_attributes.length; i++) {
        if (i > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
        ok = cn__emit_opt(node->gnu_attributes.idxs[i], func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT("))"), func, opt);
    return 0;
}

CNDEF int cn__emit_gnu_attribute(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Gnu_Attribute *node = (Cn_Ast_Gnu_Attribute *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_ATTRIBUTE);

    int ok;
    // Emit attribute identifier.
    ok = cn__emit_opt(node->identifier_idx, func, opt);
    if (ok != 0) return ok;

    // Emit arguments if present.
    if (node->arguments.length > 0) {
        cn__emit_str(CN_STR_LIT("("), func, opt);
        for (int64_t i = 0; i < node->arguments.length; i++) {
            if (i > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
            ok = cn__emit_opt(node->arguments.idxs[i], func, opt);
            if (ok != 0) return ok;
        }
        cn__emit_str(CN_STR_LIT(")"), func, opt);
    }
    return 0;
}

CNDEF int cn__emit_init_declarator(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Init_Declarator *node = (Cn_Ast_Init_Declarator *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_INIT_DECLARATOR);

    int ok;
    ok = cn__emit_opt(node->declarator_idx, func, opt);
    if (ok != 0) return ok;

    if (node->gnu_asm_label_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(node->gnu_asm_label_idx, func, opt);
        if (ok != 0) return ok;
    }

    if (node->initializer_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" = "), func, opt);
        ok = cn__emit_opt(node->initializer_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_parameter_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Parameter_Declaration *node = (Cn_Ast_Parameter_Declaration *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_PARAMETER_DECLARATION);

    int ok;
    ok = cn__emit_opt(node->declaration_specifiers_idx, func, opt);
    if (ok != 0) return ok;

    if (node->declarator_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(node->declarator_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_enumerator(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Enumerator *node = (Cn_Ast_Enumerator *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ENUMERATOR);

    int ok;
    ok = cn__emit_opt(node->identifier_idx, func, opt);
    if (ok != 0) return ok;

    if (node->expression_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" = "), func, opt);
        ok = cn__emit_opt(node->expression_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_attribute_specifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Attribute_Specifier *node = (Cn_Ast_Attribute_Specifier *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ATTRIBUTE_SPECIFIER);

    int ok;
    cn__emit_str(CN_STR_LIT("[["), func, opt);
    for (int64_t i = 0; i < node->attributes.length; i++) {
        if (i > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
        ok = cn__emit_opt(node->attributes.idxs[i], func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT("]]"), func, opt);
    return 0;
}

CNDEF int cn__emit_attribute(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Attribute *node = (Cn_Ast_Attribute *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ATTRIBUTE);

    int ok;
    // Emit vendor prefix if present.
    if (node->vendor_identifier_idx != CN_AST_NIL_IDX) {
        ok = cn__emit_opt(node->vendor_identifier_idx, func, opt);
        if (ok != 0) return ok;
        cn__emit_str(CN_STR_LIT("::"), func, opt);
    }

    ok = cn__emit_opt(node->identifier_idx, func, opt);
    if (ok != 0) return ok;

    // Emit arguments if present.
    if (node->arguments.length > 0) {
        cn__emit_str(CN_STR_LIT("("), func, opt);
        for (int64_t i = 0; i < node->arguments.length; i++) {
            if (i > 0) cn__emit_str(CN_STR_LIT(", "), func, opt);
            ok = cn__emit_opt(node->arguments.idxs[i], func, opt);
            if (ok != 0) return ok;
        }
        cn__emit_str(CN_STR_LIT(")"), func, opt);
    }
    return 0;
}

CNDEF int cn__emit_gnu_asm_label(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Gnu_Asm_Label *node = (Cn_Ast_Gnu_Asm_Label *)cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_ASM_LABEL);

    cn__emit_str(CN_STR_LIT("__asm__("), func, opt);
    int ok = cn__emit_opt(node->string_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(")"), func, opt);
    return 0;
}

CNDEF int cn__emit_code(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    (void)node_idx; (void)func; (void)opt;
    // Code nodes are special and should be unwrapped during reparse, not emitted directly.
    return 0;
}

CNDEF int cn__emit_opt(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_get(node_idx);

    // Check if this is the highlighted node.
    bool is_highlight = (node_idx == opt->highlight_idx && opt->highlight_offset != NULL);
    int64_t highlight_start = 0;
    if (is_highlight) {
        highlight_start = (opt->written_length != NULL) ? *opt->written_length : 0;
        *opt->highlight_offset = highlight_start;
    }

    int ok = 0;
    switch (node->kind) {
#define X(K, T, m) case CN_AST_##K: ok = cn__emit_##m(node_idx, func, opt); break;
    CN_AST_GEN_LIST(X)
#undef X
        case CN_AST_BREAK:    ok = cn__emit_break_statement(node_idx, func, opt); break;
        case CN_AST_CONTINUE: ok = cn__emit_continue_statement(node_idx, func, opt); break;
        default:
            // For unhandled node types, try to emit source location if available.
            {
                Cn_String value = cn_source_to_str(&node->src);
                if (!cn_str_is_empty(value)) {
                    cn__emit_str(value, func, opt);
                }
            }
            break;
    }

    // Set highlight length (even if interrupted, extends to end of output).
    if (is_highlight && opt->highlight_length != NULL) {
        int64_t current_len = (opt->written_length != NULL) ? *opt->written_length : 0;
        *opt->highlight_length = current_len - highlight_start;
    }

    return ok;
}

CNDEF int cn_emit_opt(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt opt) {
    if (opt.written_length == NULL) {
        int64_t length = 0;
        opt.written_length = &length;
        return cn__emit_opt(node_idx, func, &opt);
    }
    // Initialize to 0 if not already set.
    *opt.written_length = 0;
    return cn__emit_opt(node_idx, func, &opt);
}

CNDEF bool cn_ast_type_ptr_equals(const Cn_Type **type1_ptr, const Cn_Type **type2_ptr) {
    return cn_type_equals(*type1_ptr, *type2_ptr);
}

CNDEF uint64_t cn_ast_type_ptr_hash(const Cn_Type **type_ptr) {
    return cn_type_hash(*type_ptr);
}

CNDEF Cn_Ast_Scope *cn_ast_scope_stack_peek() {
    if (cn_array_list_length(&cn__ast_data->scope_stack) == 0) return NULL;

    return cn__ast_data->scope_stack + (cn_array_list_length(&cn__ast_data->scope_stack) - 1);
}

CNDEF void cn_ast_scope_stack_push() {
    cn_array_list_append(&cn__ast_data->scope_stack, ((Cn_Ast_Scope) { 
                // .saved_strings_idx = cn_array_list_length(&cn_ast_saved_strings_list), 
                .saved_strings_length = 0,
                .binding_idx = cn_array_list_length(&cn__ast_data->binding_list),
                .is_checkpoint_locked = false,
                } ));
}

CNDEF void cn_ast_scope_stack_pop() {
    CN_ASSERT(cn_array_list_length(&cn__ast_data->scope_stack) > 0);

    Cn_Ast_Scope *scope = cn_ast_scope_stack_peek();

    if (scope->is_checkpoint_locked) {
        cn_log(CN_ERROR, "Scope with idx: %ld, is checkpoint locked and cannot be popped, make sure to remove checkpoint before popping the stack.", CN_AST_SCOPE_STACK_CURRENT_IDX);
        exit(1);
    }

    for (int i = scope->binding_idx; i < cn_array_list_length(&cn__ast_data->binding_list); i++) {
        // Resolve each binding, properly dispose each binding.
        if (cn__ast_data->binding_list[i].kind == CN_BINDING_TAG) {
            if (cn__ast_data->binding_list[i].next_idx == CN_AST_NIL_IDX) {
                cn_hash_table_remove(&cn__ast_data->tag_binding_table, &cn__ast_data->binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__ast_data->tag_binding_table, cn__ast_data->binding_list[i].next_idx, &cn__ast_data->binding_list[i].name);
            }
        } else {
            if (cn__ast_data->binding_list[i].next_idx == CN_AST_NIL_IDX) {
                cn_hash_table_remove(&cn__ast_data->symbol_binding_table, &cn__ast_data->binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__ast_data->symbol_binding_table, cn__ast_data->binding_list[i].next_idx, &cn__ast_data->binding_list[i].name);
            }
        }
    }
    cn_array_list_pop_multiple(&cn__ast_data->binding_list, cn_array_list_length(&cn__ast_data->binding_list) - scope->binding_idx);

    cn_chained_arena_dealloc(&cn__ast_data->scoped_strings_arena, scope->saved_strings_length);
    cn_array_list_pop(&cn__ast_data->scope_stack);
}

/**
 * Saves string respective to the current scope.
 * Once scope is popped the string data is lost.
 */
CNDEF Cn_String cn__ast_scope_stack_save_string(Cn_String str) {
    Cn_Ast_Scope *scope = cn_ast_scope_stack_peek();

    void *data = cn_chained_arena_alloc(&cn__ast_data->scoped_strings_arena, str.length);
    cn_str_copy_to(str, data);
    str.data = data;

    scope->saved_strings_length += str.length;

    return str;
}

/**
 * Permanently saves string.
 */
CNDEF Cn_String cn__ast_permanent_save_string(Cn_String str) {
    void *data = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, str.length);
    cn_str_copy_to(str, data);
    str.data = data;

    return str;
}

CNDEF Cn_Ast_Binding_Idx cn_ast_binding_table_put(Cn_Ast_Binding binding, Cn_Ast_Binding_Idx **binding_table) {
    cn_array_list_append(&cn__ast_data->binding_list, binding);
    cn_hash_table_put(binding_table, cn_array_list_length(&cn__ast_data->binding_list) - 1, &binding.name);
    return cn_array_list_length(&cn__ast_data->binding_list) - 1;
}

CNDEF Cn_Ast_Binding_Idx cn_ast_binding_table_get(Cn_String name, Cn_Ast_Binding_Idx **binding_table) {
    Cn_Ast_Binding_Idx *reference = cn_hash_table_get(binding_table, &name);
    return reference == NULL ? CN_AST_NIL_BINDING_IDX : *reference;
}

/**
 * Helper function, diagnoses error if binding a conflicts with binding b based 
 * on kind or type.
 * NOTE: Expects a and b to be in the same scope and contain the same name.
 *
 * RETURNS: True if there is conflict, false otherwise.
 */
CNDEF bool cn__ast_bindings_conflict(Cn_Ast_Binding *a, Cn_Ast_Binding *b) {
    CN_ASSERT(cn_str_equals(&a->name, &b->name));
    CN_ASSERT(a->scope_idx == b->scope_idx);

    // Different binding kind.
    if (a->kind != b->kind) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, b->src, CN_DC_ILLEGAL_BINDING, "'%.*s' redeclared as different kind of symbol.", CN_UNPACK(a->name));
        return true;
    }
    // Confliciting types.
    if (a->type != b->type) {
        Cn_String current_t = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), a->type);
        Cn_String binding_t = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), b->type);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, b->src, CN_DC_ILLEGAL_BINDING, "'%.*s' redeclared with a conflicting type, from %.*s to %.*s.", CN_UNPACK(b->name), CN_UNPACK(current_t), CN_UNPACK(binding_t));
        return true;
    }

    return false;
}

CNDEF Cn_Ast_Binding_Idx cn_ast_function_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Storage_Specifier_Flags storage_flags, Cn_Function_Specifier_Flags function_flags, Cn_Type *type, Cn_Ast_List *parameter_declarations, bool is_definition) {
    CN_ASSERT(cn_array_list_length(&cn__ast_data->scope_stack) > 0);

    Cn_Ast_Binding binding = {
        .kind = CN_BINDING_FUCNTION,
        .src = source_idx,
        .type = type,
        .scope_idx = CN_AST_SCOPE_STACK_CURRENT_IDX,
    };
    binding.b_function.storage_flags    = storage_flags;
    binding.b_function.function_flags   = function_flags;
    binding.b_function.is_definition    = is_definition;

    // Storing param names on the function binding.
    if (parameter_declarations != NULL && parameter_declarations->length > 0) {
        binding.b_function.parameter_names = cn_chained_arena_alloc(&cn__ast_data->scoped_strings_arena, sizeof(Cn_String) * parameter_declarations->length);
        cn_ast_scope_stack_peek()->saved_strings_length += sizeof(Cn_String) * parameter_declarations->length;

        for (int64_t i = 0; i < parameter_declarations->length; i++) {
            Cn_Ast_Parameter_Declaration *p = cn_ast_get(parameter_declarations->idxs[i]);
            Cn_Ast_Idx param_identifier_idx;
            cn_get_declarator_info(p->declarator_idx, &param_identifier_idx);

            if (param_identifier_idx != CN_AST_NIL_IDX && cn_ast_get_as_node(param_identifier_idx)->kind == CN_AST_IDENTIFIER) {
                binding.b_function.parameter_names[i] = cn__ast_scope_stack_save_string(((Cn_Ast_Identifier *)cn_ast_get(param_identifier_idx))->name);
            }
        }
    }
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Ast_Binding_Idx current_idx = cn_ast_binding_table_get(name, &cn__ast_data->symbol_binding_table);
    if (current_idx != CN_AST_NIL_BINDING_IDX) {
        Cn_Ast_Binding *current = cn_ast_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__ast_bindings_conflict(current, &binding)) return CN_AST_NIL_BINDING_IDX;

            if (current->b_function.is_definition && binding.b_function.is_definition) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' function redifinition.", CN_UNPACK(name));
                return CN_AST_NIL_BINDING_IDX;
            }

            // If newer binding is definition merge bindings, if not leave as it is.
            if (binding.b_function.is_definition) {
                current->src = binding.src;
                current->b_function = binding.b_function;
            }

            return current_idx;
        }
    } else {
        binding.name = cn__ast_scope_stack_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_ast_binding_table_put(binding, &cn__ast_data->symbol_binding_table);
}

CNDEF Cn_Ast_Binding_Idx cn_ast_variable_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Storage_Specifier_Flags storage_flags, Cn_Type *type, bool is_definition) {
    CN_ASSERT(cn_array_list_length(&cn__ast_data->scope_stack) > 0);

    // Checking if binding has valid type.
    if (is_definition && !(type->flags & CN_TYPE_COMPLETE)) {
        Cn_String type_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' variable defined with incomplete type %.*s.", CN_UNPACK(name), CN_UNPACK(type_str));
        return CN_AST_NIL_BINDING_IDX;
    }

    Cn_Ast_Binding binding = { 
        .kind = CN_BINDING_VARIABLE,
        .src = source_idx,
        .type = type,
        .scope_idx = CN_AST_SCOPE_STACK_CURRENT_IDX,
    };
    binding.b_variable.storage_flags = storage_flags;
    binding.b_variable.is_definition = is_definition;
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Ast_Binding_Idx current_idx = cn_ast_binding_table_get(name, &cn__ast_data->symbol_binding_table);
    if (current_idx != CN_AST_NIL_BINDING_IDX) {
        Cn_Ast_Binding *current = cn_ast_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__ast_bindings_conflict(current, &binding)) return CN_AST_NIL_BINDING_IDX;

            if (CN_AST_SCOPE_STACK_CURRENT_IDX != CN_AST_FILE_SCOPE_IDX) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' variable redeclared in non-file scope.", CN_UNPACK(name));
                return CN_AST_NIL_BINDING_IDX;
            }

            if (current->b_variable.is_definition && binding.b_variable.is_definition) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' variable redifinition.", CN_UNPACK(name));
                return CN_AST_NIL_BINDING_IDX;
            }

            // If newer binding is definition merge bindings, if not leave as it is.
            if (binding.b_variable.is_definition) {
                current->src = binding.src;
                current->b_variable = binding.b_variable;
            }

            return current_idx;
        }
    } else {
        binding.name = cn__ast_scope_stack_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_ast_binding_table_put(binding, &cn__ast_data->symbol_binding_table);
}

CNDEF Cn_Ast_Binding_Idx cn_ast_typedef_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Type *type) {
    CN_ASSERT(cn_array_list_length(&cn__ast_data->scope_stack) > 0);

    Cn_Ast_Binding binding = { 
        .kind = CN_BINDING_TYPEDEF,
        .src = source_idx,
        .type = type,
        .scope_idx = CN_AST_SCOPE_STACK_CURRENT_IDX,
    };
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Ast_Binding_Idx current_idx = cn_ast_binding_table_get(name, &cn__ast_data->symbol_binding_table);
    if (current_idx != CN_AST_NIL_BINDING_IDX) {
        Cn_Ast_Binding *current = cn_ast_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__ast_bindings_conflict(current, &binding)) return CN_AST_NIL_BINDING_IDX;

            // Typedef is not allowed to be redefined.
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' typedef redifinition.", CN_UNPACK(name));
            return CN_AST_NIL_BINDING_IDX;
        }
    } else {
        binding.name = cn__ast_scope_stack_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_ast_binding_table_put(binding, &cn__ast_data->symbol_binding_table);
}

CNDEF Cn_Ast_Binding_Idx cn_ast_enum_constant_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Type *type, int64_t value) {
    CN_ASSERT(cn_array_list_length(&cn__ast_data->scope_stack) > 0);

    Cn_Ast_Binding binding = { 
        .kind = CN_BINDING_ENUM_CONSTANT,
        .src = source_idx,
        .type = type,
        .scope_idx = CN_AST_SCOPE_STACK_CURRENT_IDX,
    };

    binding.b_enum_constant.value = value;
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Ast_Binding_Idx current_idx = cn_ast_binding_table_get(name, &cn__ast_data->symbol_binding_table);
    if (current_idx != CN_AST_NIL_BINDING_IDX) {
        Cn_Ast_Binding *current = cn_ast_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__ast_bindings_conflict(current, &binding)) return CN_AST_NIL_BINDING_IDX;

            // Enum constant is not allowed to be redefined.
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' enum constant redifinition.", CN_UNPACK(name));
            return CN_AST_NIL_BINDING_IDX;
        }
    } else {
        binding.name = cn__ast_scope_stack_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_ast_binding_table_put(binding, &cn__ast_data->symbol_binding_table);
}

CNDEF Cn_Ast_Binding_Idx cn_ast_tag_binding_declare(Cn_String tag, Cn_Type_Kind kind) {
    CN_ASSERT(cn_array_list_length(&cn__ast_data->scope_stack) > 0);
    CN_ASSERT(kind == CN_STRUCT || kind == CN_UNION || kind == CN_ENUM);

    Cn_Ast_Binding_Idx *ref = cn_hash_table_get(&cn__ast_data->tag_binding_table, &tag);

    // No previous declaration case.
    if (ref == NULL) {
        // Saving string.
        tag = cn__ast_permanent_save_string(tag);

        // Making type.
        Cn_Type *type = cn_chained_arena_alloc(&cn__ast_data->type_arena, sizeof(Cn_Type));
        type->kind = kind;
        type->flags = 0;
        switch (kind) {
            case CN_STRUCT:
                ((Cn_Type_Struct *)type)->tag = tag;
                break;
            case CN_UNION:
                ((Cn_Type_Union *)type)->tag = tag;
                break;
            case CN_ENUM:
                ((Cn_Type_Enum *)type)->tag = tag;
                break;
            default:
                break;
        }

        // Appending to the binding list.
        cn_array_list_append(&cn__ast_data->binding_list, ((Cn_Ast_Binding) { 
                    .name = tag, 
                    .kind = CN_BINDING_TAG, 
                    .type = type, 
                    .next_idx = CN_AST_NIL_BINDING_IDX, 
                    .scope_idx = cn_array_list_length(&cn__ast_data->scope_stack) - 1 
                    }));

        // Finally, putting it into the table, and returning.
        cn_hash_table_put(&cn__ast_data->tag_binding_table, cn_array_list_length(&cn__ast_data->binding_list) - 1, &tag);
        
        return cn_array_list_length(&cn__ast_data->binding_list) - 1;
    }


    Cn_Ast_Binding *binding = cn__ast_data->binding_list + *ref;
    if (binding->type->kind != kind) {
        cn_log(CN_ERROR, "Tag '%.*s' is already declared under different type. This tool deosn't support that in any form.", CN_UNPACK(tag));
        return CN_AST_NIL_BINDING_IDX;
    }

    if ((binding->type->flags & CN_TYPE_COMPLETE) || binding->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) return *ref;
    
    // Making type.
    Cn_Type *type = cn_chained_arena_alloc(&cn__ast_data->type_arena, sizeof(Cn_Type));
    type->kind = kind;
    type->flags = 0;
    switch (kind) {
        case CN_STRUCT:
            ((Cn_Type_Struct *)type)->tag = tag;
            break;
        case CN_UNION:
            ((Cn_Type_Union *)type)->tag = tag;
            break;
        case CN_ENUM:
            ((Cn_Type_Enum *)type)->tag = tag;
            break;
        default:
            break;
    }

    cn_array_list_append(&cn__ast_data->binding_list, ((Cn_Ast_Binding) {
                .name = binding->name, 
                .kind = CN_BINDING_TAG, 
                .type = type, 
                .next_idx = *ref, 
                .scope_idx = cn_array_list_length(&cn__ast_data->scope_stack) - 1 
                }));

    cn_hash_table_put(&cn__ast_data->tag_binding_table, cn_array_list_length(&cn__ast_data->binding_list) - 1, &tag);


    return cn_array_list_length(&cn__ast_data->binding_list) - 1;
}

CNDEF bool cn_parse_expect(Cn_Lexer *lexer, Cn_Token_Type type) {
    if (cn_lexer_expect(lexer, type)) {
        cn_lexer_next_token(lexer);
        return true;
    }

    Cn_Token token = cn_lexer_peek(lexer, 0);
    cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &token.loc, &token.src, CN_DC_EXPECTED_TOKEN, "Expected '%s' token, but got '%s'.", cn_token_kind_name(type), cn_token_kind_name(cn_lexer_token(lexer).type));
    return false;
}

CNDEF bool cn_parse_optional(Cn_Lexer *lexer, Cn_Token_Type type) {
    if (cn_lexer_expect(lexer, type)) {
        cn_lexer_next_token(lexer);
        return true;
    }
    return false;
}

#define CN__TRACE_ERROR
#ifndef CN__TRACE_ERROR
#   define CN__TRACE_ERROR   fprintf(stderr, "   error from '%s()'\n", __FUNCTION__);
#endif // CN__TRACE_ERROR

CNDEF Cn_Ast_Idx cn_ast_parse_translation_unit(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_TRANSLATION_UNIT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // Pushing first scope that will act as a global scope.
    cn_ast_scope_stack_push();
    // Inject compiler builtins that appear in preprocessed .i files but are
    // never declared by any source.
    {
        Cn_Type *opaque = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_OPAQUE);

        cn_ast_typedef_binding_declare(CN_CSTR("__builtin_va_list"),              CN_AST_NIL_IDX, opaque);
        cn_ast_function_binding_declare(CN_CSTR("__builtin___sprintf_chk"),       CN_AST_NIL_IDX, 0, 0, opaque, NULL, false);
        cn_ast_function_binding_declare(CN_CSTR("__builtin___snprintf_chk"),      CN_AST_NIL_IDX, 0, 0, opaque, NULL, false);
        cn_ast_function_binding_declare(CN_CSTR("__builtin___vsprintf_chk"),      CN_AST_NIL_IDX, 0, 0, opaque, NULL, false);
        cn_ast_function_binding_declare(CN_CSTR("__builtin___vsnprintf_chk"),     CN_AST_NIL_IDX, 0, 0, opaque, NULL, false);
        cn_ast_function_binding_declare(CN_CSTR("__builtin_va_arg_pack"),         CN_AST_NIL_IDX, 0, 0, opaque, NULL, false);
        cn_ast_function_binding_declare(CN_CSTR("__builtin_dynamic_object_size"), CN_AST_NIL_IDX, 0, 0, opaque, NULL, false);
    }

    int64_t mark = cn_ast_idx_stack_mark();

    while (cn_lexer_token(lexer).type != CN_TOKEN_EOF) {
        Cn_Ast_Idx next_idx = cn_ast_parse_external_declaration(lexer);

        if (next_idx == CN_AST_NIL_IDX)
            goto error;

        cn_ast_idx_stack_push(next_idx);
    }

    node.translation_unit.external_declarations = cn_ast_idx_stack_finalize(mark);

    Cn_Ast_Idx idx = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(idx, &node.translation_unit.external_declarations);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_external_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_EXTERNAL_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // Optional '__extension__' symbol.
    if (cn_lexer_token(lexer).type == CN_TOKEN_GNU_EXTENSION) {
        node.flags |= CN_AST_EXTERNAL_DECLARATION_HAS_EXTENSION;
        cn_lexer_next_token(lexer);
    }

    // If stray ';' case.
    if (cn_lexer_token(lexer).type == CN_TOKEN_SEMICOLON) {
        cn_lexer_next_token(lexer);
        return cn_ast_node_list_append(node);
    }

    // TODO: Parse asm definition here.
    // ...

    Cn_Ast_Idx idx = cn_ast_node_list_append(node);

    // Last possible case function definition or declaration.
    // Setting checkpoint.
    if (!cn_ast_checkpoint_set(&cn_ast_checkpoint_message, cn__ast_data, CN_AST_CHECKPOINT_IGNORE_AST_NODES)) {
        Cn_Ast_Idx child_idx = cn_ast_parse_function_or_declaration(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        ((Cn_Ast_External_Declaration *)cn_ast_get(idx))->child_idx = child_idx;
        cn_ast_node_set_parent(idx, child_idx);

        // Messaging function definition.
        cn_ast_idx_from_message = child_idx;

        if (((Cn_Ast_Function *)cn_ast_get(child_idx))->kind == CN_AST_FUNCTION) {
            bool modified = cn_send_message(CN_MESSAGE_PARSED_FUNCTION_DEFINITION, (Cn_Message) {
                        .parsed_function_definition = {
                            .node_idx = cn_ast_idx_from_message,
                        }
                    });

            if (modified) cn_ast_checkpoint_load(&cn_ast_checkpoint_message);
        }
    } else {
        if (!cn_ast_reparse_function_definition(cn_ast_idx_from_message)) goto error;
        ((Cn_Ast_External_Declaration *)cn_ast_get(idx))->child_idx = cn_ast_idx_from_message;
        cn_ast_node_set_parent(idx, cn_ast_idx_from_message);
    }

    cn_ast_checkpoint_remove(&cn_ast_checkpoint_message);

    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF void cn_get_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *last_declarator_idx) {
    if (declarator_idx == CN_AST_NIL_IDX) {
        *last_declarator_idx = CN_AST_NIL_IDX;
        return;
    }

    Cn_Ast_Declarator *decl = (Cn_Ast_Declarator *)cn_ast_get(declarator_idx);
    Cn_Ast_Idx dd_idx = decl->direct_declarator_idx;
    Cn_Ast_Node *dd = (Cn_Ast_Node *)decl;

    while (dd_idx != CN_AST_NIL_IDX) {
        dd = cn_ast_get(dd_idx);

        switch (dd->kind) {
            case CN_AST_IDENTIFIER:
                *last_declarator_idx = dd_idx;
                return;
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                dd_idx = ((Cn_Ast_Declarator *)cn_ast_get(((Cn_Ast_Direct_Declarator_Grouped *)dd)->declarator_idx))->direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                dd_idx = ((Cn_Ast_Direct_Declarator_Array *)dd)->direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                dd_idx = ((Cn_Ast_Direct_Declarator_Function *)dd)->direct_declarator_idx;
                continue;
            default:
                break;
        }
    }

    // NIL Direct declarator, case.
    *last_declarator_idx = cn_ast_idx_get(dd);
}

CNDEF bool cn_get_function_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *function_direct_declarator_idx, Cn_Ast_Idx *last_declarator_idx) {
    if (declarator_idx == CN_AST_NIL_IDX) {
        *last_declarator_idx = CN_AST_NIL_IDX;
        return false;
    }

    Cn_Ast_Declarator *decl = (Cn_Ast_Declarator *)cn_ast_get(declarator_idx);
    Cn_Ast_Idx dd_idx = decl->direct_declarator_idx;
    Cn_Ast_Node *dd = (Cn_Ast_Node *)decl;

    while (dd_idx != CN_AST_NIL_IDX) {
        dd = cn_ast_get(dd_idx);

        switch (dd->kind) {
            // If identifier case, then declarator is not function declarator.
            case CN_AST_IDENTIFIER:
                *last_declarator_idx = dd_idx;
                return false;
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                dd_idx = ((Cn_Ast_Declarator *)cn_ast_get(((Cn_Ast_Direct_Declarator_Grouped *)dd)->declarator_idx))->direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                dd_idx = ((Cn_Ast_Direct_Declarator_Array *)dd)->direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION: {
                Cn_Ast_Idx current_dd_idx = dd_idx;
                dd_idx = ((Cn_Ast_Direct_Declarator_Function *)dd)->direct_declarator_idx;

                // If followed by identifier, then return true, and get all outputs.
                if (dd_idx != CN_AST_NIL_IDX && ((Cn_Ast_Node *)cn_ast_get(dd_idx))->kind == CN_AST_IDENTIFIER) {
                    *function_direct_declarator_idx = current_dd_idx;
                    *last_declarator_idx = dd_idx;
                    return true;
                }

                continue;
            }
            default:
                break;
        }
    }

    // NIL Direct declarator, case.
    *last_declarator_idx = cn_ast_idx_get(dd);
    return false;
}

CNDEF Cn_Ast_Idx cn_ast_parse_function_or_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Location loc = cn_lexer_token(lexer).loc;
    Cn_Source src = cn_lexer_token(lexer).src;

    // Getting attributes [[]].
    bool ok;
    Cn_Ast_List attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    // Getting declration specifiers.
    Cn_Ast_Idx declaration_specifiers_idx = cn_ast_parse_declaration_specifiers(lexer);
    if (declaration_specifiers_idx == CN_AST_NIL_IDX) goto error;

    // Deciding whether node is function definiton or declaration.
    // There MUST be declarator,
    // which will be either part of init_declarators or just be by itself.
    // It might be completely abstract, meaning no tokens are part of it.
    Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    // Empty declaration case:
    if (declarator_idx == CN_AST_NIL_IDX) {
        
        // Expecting ';' at the end of empty declaration.
        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

        Cn_Ast_Node node = {
            .kind = CN_AST_DECLARATION,
            .loc = loc,
            .src = src,
        };

        node.flags |= CN_AST_DECLARATION_IS_EMPTY;
        node.declaration.attribute_specifiers = attribute_specifiers;
        node.declaration.declaration_specifiers_idx = declaration_specifiers_idx;

        Cn_Ast_Idx idx = cn_ast_node_list_append(node);
        cn_ast_list_set_parent(idx, &node.declaration.attribute_specifiers);
        
        // New types can be made, even in empty declaration.
        ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, (Cn_Ast_List) {0});
        if (!ok) goto error;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_list_set_parent(parent, &node.declaration.attribute_specifiers);
        cn_ast_node_set_parent(parent, node.declaration.declaration_specifiers_idx);
        return parent;
    }
    
    // Non-empty abstract declarator handling:
    if (cn_ast_get_as_node(declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator_idx, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator in declaration.");
        goto error;
    }


    // Now finally we can decide if next token is '{' it is definitely a function definition because, it is part of function definition body, anything else could be part of init_declarator_list or ';'.
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        Cn_Ast_Node node = {
            .kind = CN_AST_FUNCTION,
            .loc = loc,
            .src = src,
        };
        node.function.attribute_specifiers = attribute_specifiers;
        node.function.declaration_specifiers_idx = declaration_specifiers_idx;
        node.function.declarator_idx = declarator_idx;

        // Adding function type and binding.
        Cn_Ast_Binding_Idx function_binding_idx;
        ok = cn_ast_analyze_function(
                node.function.declaration_specifiers_idx,
                node.function.declarator_idx,
                &function_binding_idx
                );

        if (!ok) goto error;

        // Making new function scope to store parameters + compound statement symbols.
        cn_ast_scope_stack_push();

        // Binding non-abstract function parameters as variables from definition.
        if (!cn_ast_bind_function_params(function_binding_idx)) goto error;

        node.function.block_idx = cn_ast_parse_block(lexer, true);
        if (node.function.block_idx == CN_AST_NIL_IDX) goto error;

        cn_ast_scope_stack_pop();

        Cn_Ast_Idx idx = cn_ast_node_list_append(node);
        cn_ast_list_set_parent(idx, &node.function.attribute_specifiers);
        cn_ast_node_set_parent(idx,
                node.function.declaration_specifiers_idx,
                node.function.declarator_idx,
                node.function.block_idx
                );
        return idx;

    } else {
        Cn_Ast_Node node = {
            .kind = CN_AST_DECLARATION,
            .loc = loc,
            .src = src,
        };

        node.declaration.attribute_specifiers = attribute_specifiers;
        node.declaration.declaration_specifiers_idx = declaration_specifiers_idx;
        node.declaration.init_declarators = cn_ast_continue_init_declarators(lexer, declarator_idx, &ok);
        if (!ok) goto error_recover;

        // Collecting GNU attributes.
        node.declaration.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;

        // ';' at the end check.
        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error_recover;

        // Adding types that are used by declarators.
        ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, node.declaration.init_declarators);
        if (!ok) goto error;

        Cn_Ast_Idx idx = cn_ast_node_list_append(node);
        cn_ast_list_set_parent(idx, &node.declaration.attribute_specifiers);
        cn_ast_list_set_parent(idx, &node.declaration.init_declarators);
        cn_ast_node_set_parent(idx, node.declaration.declaration_specifiers_idx);
        cn_ast_list_set_parent(idx, &node.declaration.gnu_attribute_specifiers);
        return idx;
    }

error_recover:
    CN__TRACE_ERROR
    cn_ast_consume_till(lexer, CN_TOKEN_SEMICOLON);
    return CN_AST_ERROR_IDX;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_continue_declaration(Cn_Lexer *lexer, Cn_Ast_List attribute_specifiers) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = {
        .kind = CN_AST_DECLARATION,
        .loc = cn_lexer_token(lexer).loc,
        .src = cn_lexer_token(lexer).src,
    };

    node.declaration.attribute_specifiers = attribute_specifiers;
    
    // Getting declration specifiers.
    Cn_Ast_Idx declaration_specifiers_idx = cn_ast_parse_declaration_specifiers(lexer);
    if (declaration_specifiers_idx == CN_AST_NIL_IDX) goto error;
    node.declaration.declaration_specifiers_idx = declaration_specifiers_idx;

    // If first single declarator is abstract, it is empty declaration case.
    bool ok;
    Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    
    // Empty declaration case:
    if (declarator_idx == CN_AST_NIL_IDX) {
        node.flags |= CN_AST_DECLARATION_IS_EMPTY;

        // New types can be made, even in empty declaration.
        ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, (Cn_Ast_List) {0});
        if (!ok) goto error;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_list_set_parent(parent, &node.declaration.attribute_specifiers);
        cn_ast_node_set_parent(parent, node.declaration.declaration_specifiers_idx);
        return parent;
    }
    
    // Non-empty abstract declarator handling:
    if (cn_ast_get_as_node(declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator_idx, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator in declaration.");
        goto error;
    }
    
    node.declaration.init_declarators = cn_ast_continue_init_declarators(lexer, declarator_idx, &ok);
    if (!ok) goto error;

    // Collecting GNU attributes.
    node.declaration.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;
    
    // Adding types that are used by declarators.
    ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, node.declaration.init_declarators);
    if (!ok) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.declaration.attribute_specifiers);
    cn_ast_list_set_parent(parent, &node.declaration.init_declarators);
    cn_ast_node_set_parent(parent, node.declaration.declaration_specifiers_idx);
    cn_ast_list_set_parent(parent, &node.declaration.gnu_attribute_specifiers);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF bool cn_ast_analyze_declaration(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_List init_declarators) {
    Cn_Ast_Declaration_Specifiers *decl_spec = cn_ast_get(declaration_specifiers_idx);

    // No declarators, then just analzing declaration specifiers.
    if (init_declarators.length == 0) {
        Cn_Type *type = cn_ast_to_type(decl_spec->qualifiers, decl_spec->type_specifier_idx, CN_AST_NIL_IDX);
        return type != NULL;
    }

    // Iterating over declarators.
    for (int64_t i = 0; i < init_declarators.length; i++) {
        Cn_Ast_Init_Declarator *init_decl = cn_ast_get(init_declarators.idxs[i]);

        Cn_Type *type = cn_ast_to_type(decl_spec->qualifiers, decl_spec->type_specifier_idx, init_decl->declarator_idx);
        if (type == NULL) return false;

        // Adding declarator to variable or typedef binding, if it is not abstract.
        Cn_Ast_Idx identifier_idx;
        cn_get_declarator_info(init_decl->declarator_idx, &identifier_idx);

        if (((Cn_Ast_Node *)cn_ast_get(identifier_idx))->kind == CN_AST_IDENTIFIER) {
            Cn_String name = ((Cn_Ast_Identifier *)cn_ast_get(identifier_idx))->name;

            // Function declaration case.
            if (type->kind == CN_FUNCTION) {
                Cn_Ast_Idx parameter_type_list_idx;
                cn_get_function_declarator_info(
                        init_decl->declarator_idx,
                        &parameter_type_list_idx,
                        &identifier_idx
                        );

                Cn_Ast_List *parameter_declarations = NULL;
                if (parameter_type_list_idx != CN_AST_NIL_IDX)
                    parameter_declarations = &((Cn_Ast_Direct_Declarator_Function *)cn_ast_get(parameter_type_list_idx))->parameter_declarations;

                if (cn_ast_function_binding_declare(
                            name,
                            identifier_idx,
                            decl_spec->storage_specifiers,
                            decl_spec->function_specifiers,
                            type,
                            parameter_declarations,
                            false
                            ) == CN_AST_NIL_BINDING_IDX) return false;

                continue;
            }

            // Typedef declaration case.
            if (decl_spec->storage_specifiers & CN_STORAGE_SPECIFIER_TYPEDEF) {
                if (cn_ast_typedef_binding_declare(name, identifier_idx, type) == CN_AST_NIL_BINDING_IDX) return false;

                continue;
            }

            // Variable declaration case.
            bool is_definition = init_decl->initializer_idx != CN_AST_NIL_IDX;

            if (cn_ast_variable_binding_declare(
                        name,
                        identifier_idx,
                        decl_spec->storage_specifiers,
                        type,
                        is_definition
                        ) == CN_AST_NIL_BINDING_IDX) return false;


            // Analyze initializer here (only happens in variable declaration case).
            if (init_decl->initializer_idx != CN_AST_NIL_IDX) {
                if (!cn_ast_initializer_typecheck(init_decl->initializer_idx, type)) return false;
            }
        }
    }

    return true;
}

CNDEF bool cn_ast_analyze_function(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_Idx declarator_idx, Cn_Ast_Binding_Idx *function_binding_idx) {
    Cn_Ast_Declaration_Specifiers *decl_spec = cn_ast_get(declaration_specifiers_idx);

    Cn_Type *type = cn_ast_to_type(
            decl_spec->qualifiers, 
            decl_spec->type_specifier_idx, 
            declarator_idx
            );
    if (type == NULL) return false;

    Cn_Ast_Idx parameter_type_list_idx;
    Cn_Ast_Idx identifier_idx;

    bool ok = cn_get_function_declarator_info(
            declarator_idx, 
            &parameter_type_list_idx, 
            &identifier_idx
            );


    if (!ok) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, identifier_idx, CN_DC_INVALID_DECLARATOR, "Cannot have abstract or non function declarator in function definition.");
        return false;
    }

    Cn_Ast_List *parameter_declarations = NULL;
    if (parameter_type_list_idx != CN_AST_NIL_IDX) {
        parameter_declarations = &((Cn_Ast_Direct_Declarator_Function *)cn_ast_get(parameter_type_list_idx))->parameter_declarations;
    }

    *function_binding_idx = cn_ast_function_binding_declare(
            ((Cn_Ast_Identifier *)cn_ast_get(identifier_idx))->name,
            identifier_idx,
            decl_spec->storage_specifiers,
            decl_spec->function_specifiers,
            type,
            parameter_declarations,
            true
            );

    if (*function_binding_idx == CN_AST_NIL_BINDING_IDX) return false;

    return true;
}

CNDEF bool cn_ast_bind_function_params(Cn_Ast_Binding_Idx function_binding_idx) {
    Cn_Ast_Binding *function_binding = cn_ast_binding_get(function_binding_idx);
    Cn_Type_Function *func_type = (Cn_Type_Function *)function_binding->type;

    for (int64_t i = 0; i < func_type->params_length; i++) {
        if (cn_ast_variable_binding_declare(
                    function_binding->b_function.parameter_names[i],
                    function_binding->src,                              // TODO: Later supply actual parmeter declaration or declarator ast nodes.
                    0,
                    func_type->params[i].type,
                    true
                    ) == CN_AST_NIL_BINDING_IDX) return false;
    }

    return true;
}

CNDEF bool cn_ast_starts_declaration(Cn_Lexer *lexer) {
    return 
            cn_lexer_expect(lexer, CN_TOKEN_STATIC) ||
            cn_lexer_expect(lexer, CN_TOKEN_EXTERN) ||
            cn_lexer_expect(lexer, CN_TOKEN_REGISTER) ||
            cn_lexer_expect(lexer, CN_TOKEN_AUTO) ||
            cn_lexer_expect(lexer, CN_TOKEN_TYPEDEF) ||

            cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE) ||

            cn_ast_starts_type(lexer)
            ;
}

CNDEF bool cn_ast_starts_type(Cn_Lexer *lexer) {
    if (cn_lexer_token(lexer).type == CN_TOKEN_IDENTIFIER) {
        Cn_String str = cn_source_to_str(&cn_lexer_token(lexer).src);
        Cn_Ast_Binding_Idx *ref = cn_hash_table_get(&cn__ast_data->symbol_binding_table, &str);
        if (ref != NULL && cn__ast_data->binding_list[*ref].kind == CN_BINDING_TYPEDEF) 
            return true;
    }

    return 
            cn_lexer_expect(lexer, CN_TOKEN_CONST) ||
            cn_lexer_expect(lexer, CN_TOKEN_RESTRICT) ||
            cn_lexer_expect(lexer, CN_TOKEN_VOLATILE) ||
            cn_lexer_expect(lexer, CN_TOKEN_ATOMIC) ||

            cn_lexer_expect(lexer, CN_TOKEN_SIGNED) ||
            cn_lexer_expect(lexer, CN_TOKEN_UNSIGNED) ||

            cn_lexer_expect(lexer, CN_TOKEN_SHORT) ||
            cn_lexer_expect(lexer, CN_TOKEN_LONG) ||

            cn_lexer_expect(lexer, CN_TOKEN_INT) ||
            cn_lexer_expect(lexer, CN_TOKEN_CHAR) ||
            cn_lexer_expect(lexer, CN_TOKEN_FLOAT) ||
            cn_lexer_expect(lexer, CN_TOKEN_DOUBLE) ||
            cn_lexer_expect(lexer, CN_TOKEN_BOOL) ||
            cn_lexer_expect(lexer, CN_TOKEN_VOID) ||

            cn_lexer_expect(lexer, CN_TOKEN_STRUCT) ||
            cn_lexer_expect(lexer, CN_TOKEN_ENUM) ||
            cn_lexer_expect(lexer, CN_TOKEN_UNION) ||

            cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)
            ;
}

CNDEF Cn_Ast_Idx cn_ast_parse_statement(Cn_Lexer *lexer) {
    if (cn_lexer_token(lexer).type == CN_TOKEN_CURLY_OPEN) {
        return cn_ast_parse_block(lexer, false);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_IF)) {
        return cn_ast_parse_if(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_SWITCH)) {
        return cn_ast_parse_switch(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_WHILE) || cn_lexer_expect(lexer, CN_TOKEN_DO) || cn_lexer_expect(lexer, CN_TOKEN_FOR)) {
        CN_TODO("Iteration statements wrangling.");
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_GOTO)) {
        return cn_ast_parse_goto(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_RETURN)) {
        return cn_ast_parse_return(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_BREAK)) {
        return cn_ast_parse_break(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_CONTINUE)) {
        return cn_ast_parse_continue(lexer);
    }

    if (cn_lexer_peek(lexer, 1).type == CN_TOKEN_COLON) {
        return cn_ast_parse_label(lexer);
    }
    
    return cn_ast_parse_expression_statement(lexer);
}

CNDEF Cn_Ast_Idx cn_ast_parse_block(Cn_Lexer *lexer, bool use_current_scope) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_BLOCK, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_OPEN)) goto error;

    // Stepping into scope.
    if (!use_current_scope) cn_ast_scope_stack_push();

    int64_t mark = cn_ast_idx_stack_mark();

    while (true) {
        if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            cn_lexer_next_token(lexer);
            if (!use_current_scope) cn_ast_scope_stack_pop();

            node.block.statements = cn_ast_idx_stack_finalize(mark);

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_list_set_parent(parent, &node.block.statements);
            return parent;
        }

        Cn_Ast_Idx child_idx;

        // Parsing attribute specifiers here in order to get to the tokens that would 
        // disambiguate delcaration vs statement.
        bool ok;
        Cn_Ast_List attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;

        if (cn_ast_starts_declaration(lexer)) {
            child_idx = cn_ast_continue_declaration(lexer, attribute_specifiers);
            if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
        } else {
            // TODO: Replace cn_ast_parse_statement with cn_ast_continue_statement in order to pass 
            // attribute_specifiers.
            child_idx = cn_ast_parse_statement(lexer);
        }

        if (child_idx == CN_AST_NIL_IDX) goto error;

        if (((Cn_Ast_Node *)cn_ast_get(child_idx))->kind == CN_AST_FUNCTION) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, child_idx, CN_DC_UNEXPECTED_FUNCTION, "Function definition is not allowed inside a block.");
            goto error;
        }

        cn_ast_idx_stack_push(child_idx);
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_if(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_IF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_IF)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'if' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' in if statement.");
    }
    cn_lexer_next_token(lexer);

    node.if_statement.condition_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (node.if_statement.condition_idx == CN_AST_NIL_IDX) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in if statement.");
    }
    cn_lexer_next_token(lexer);

    node.if_statement.then_idx = cn_ast_parse_statement(lexer);
    if (node.if_statement.then_idx == CN_AST_NIL_IDX) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_ELSE)) {
        cn_lexer_next_token(lexer);
        node.if_statement.else_idx = cn_ast_parse_statement(lexer);
        if (node.if_statement.else_idx == CN_AST_NIL_IDX) goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.if_statement.condition_idx, node.if_statement.then_idx, node.if_statement.else_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_switch(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_SWITCH, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_SWITCH)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'switch' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' in switch statement.");
    }
    cn_lexer_next_token(lexer);

    node.switch_statement.condition_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (node.switch_statement.condition_idx == CN_AST_NIL_IDX) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in switch statement.");
    }
    cn_lexer_next_token(lexer);

    node.switch_statement.body_idx = cn_ast_parse_statement(lexer);
    if (node.switch_statement.body_idx == CN_AST_NIL_IDX) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.switch_statement.condition_idx, node.switch_statement.body_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_while_statement(Cn_Lexer *lexer) {
    CN_UNUSED(lexer);
    CN_TODO("Parse while statement.");
}

CNDEF Cn_Ast_Idx cn_ast_parse_do_while_statement(Cn_Lexer *lexer) {
    CN_UNUSED(lexer);
    CN_TODO("Parse do while statement.");
}

CNDEF Cn_Ast_Idx cn_ast_parse_for_statement(Cn_Lexer *lexer) {
    CN_UNUSED(lexer);
    CN_TODO("Parse for statement.");
}

CNDEF Cn_Ast_Idx cn_ast_parse_goto(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_GOTO, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_GOTO)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'goto' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    node.goto_statement.identifier_idx = cn_ast_parse_identifier(lexer);
    if (node.goto_statement.identifier_idx == CN_AST_NIL_IDX) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(*(Cn_Ast_Node *)&node);
    cn_ast_node_set_parent(parent, node.goto_statement.identifier_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_return(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_RETURN, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_RETURN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'return' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        node.return_statement.expression_idx = cn_ast_parse_expression(lexer, 0, 0);
        if (node.return_statement.expression_idx == CN_AST_NIL_IDX) goto error;

        // Typechecking return expression.
        if (cn_ast_expression_typecheck(node.return_statement.expression_idx) == NULL) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.return_statement.expression_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_break(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_BREAK, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_BREAK)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'break' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    return cn_ast_node_list_append(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_continue(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_CONTINUE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_CONTINUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'continue' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    return cn_ast_node_list_append(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_label(Cn_Lexer *lexer) {
    CN_UNUSED(lexer);
    CN_TODO("Parse label.");
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_EXPRESSION_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    node.expression_statement.expression_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (node.expression_statement.expression_idx == CN_AST_NIL_IDX) goto error;


    Cn_Type *type = cn_ast_expression_typecheck(node.expression_statement.expression_idx);
    if (type == NULL) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.expression_statement.expression_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_arguments(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    while (true) {
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(expression_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_increasing_precedence(Cn_Lexer *lexer, Cn_Ast_Idx left_idx, int min_precedence, Cn_Expression_Parsing_Flags flags) {;
    Cn_Lexer original_state = *lexer;

    // Ternary operator case. TODO: Make macro so its not hardcoded precedence 2.
    if (cn_lexer_expect(lexer, CN_TOKEN_QUESTION)) {
        if (2 < min_precedence) {
            return left_idx;
        }

        Cn_Ast_Node node = { .kind = CN_AST_TERNARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);
        
        node.ternary.condition_idx = left_idx;

        Cn_Ast_Idx child_idx = cn_ast_parse_expression(lexer, -1, flags);
        if (child_idx == CN_AST_NIL_IDX) goto error;

        node.ternary.true_idx = child_idx;

        if (!cn_parse_expect(lexer, CN_TOKEN_COLON)) goto error;

        child_idx = cn_ast_parse_expression(lexer, -1, flags);
        if (child_idx == CN_AST_NIL_IDX) goto error;

        node.ternary.false_idx = child_idx;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.ternary.condition_idx,
                node.ternary.true_idx,
                node.ternary.false_idx,
                );
        return parent;
    }

    int8_t op_kind;

    // Binary operator case.
    op_kind = cn_ast_is_binary_operator(lexer);
    if (op_kind != CN_BINARY_OP_NONE) {
        int precedence = CN_BINARY_OPERATORS[op_kind].precedence; 

        if (precedence <= min_precedence) {
            return left_idx;
        }

        // Ignoring comma operator if flag is specified.
        if (cn_lexer_expect(lexer, CN_TOKEN_COMMA) && (flags & CN_NO_COMMA_OPERATOR)) {
            return left_idx;
        }
        
        // Function call case.
        if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Ast_Node node = { .kind = CN_AST_CALL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
            cn_lexer_next_token(lexer);

            node.call.expression_idx = left_idx;
            
            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                bool ok;
                node.call.arguments = cn_ast_parse_arguments(lexer, &ok);
                if (!ok) goto error;
            }

            if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, node.call.expression_idx);
            cn_ast_list_set_parent(parent, &node.call.arguments);
            return parent;
        } 

        // Access operator cases.
        if (cn_lexer_expect(lexer, CN_TOKEN_ARROW) || cn_lexer_expect(lexer, CN_TOKEN_DOT)) {
            Cn_Ast_Node node = { .kind = CN_AST_ACCESS, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
            cn_lexer_next_token(lexer);
            
            node.access.expression_idx = left_idx;

            node.access.pointer = op_kind == CN_BINARY_OP_ARROW;

            Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
            if (identifier_idx == CN_AST_NIL_IDX) goto error;
            node.access.member_idx = identifier_idx;

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, 
                    node.access.expression_idx,
                    node.access.member_idx
                    );
            return parent;
        }

        // Array subscript case.
        Cn_Ast_Node node = { .kind = CN_AST_BINARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        if (cn_parse_optional(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
            Cn_Ast_Idx right_idx = cn_ast_parse_expression(lexer, precedence, flags);
            if (right_idx == CN_AST_NIL_IDX) goto error;

            if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;

            node.binary.left_idx = left_idx;
            node.binary.operator = CN_BINARY_OP_ARRAY_SUB;
            node.binary.right_idx = right_idx;

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, 
                    node.binary.left_idx,
                    node.binary.right_idx
                    );
            return parent;
        } 

        // Regualar binary operator case.
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx right_idx = cn_ast_parse_expression(lexer, precedence, flags);
        if (right_idx == CN_AST_NIL_IDX) goto error;

        node.binary.left_idx = left_idx;
        node.binary.operator = op_kind;
        node.binary.right_idx = right_idx;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.binary.left_idx,
                node.binary.right_idx
                );

        return parent;
    }

    // Assignment operator case.
    op_kind = cn_ast_is_assignment_operator(lexer);
    if (op_kind != CN_ASSIGNMENT_OP_NONE) {

        if (CN_ASSIGNMENT_OPERATOR_PRECEDENCE < min_precedence) {
            return left_idx;
        }

        Cn_Ast_Node node = { .kind = CN_AST_ASSIGN, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        Cn_Ast_Idx right_idx = cn_ast_parse_expression(lexer, CN_ASSIGNMENT_OPERATOR_PRECEDENCE, flags);
        if (right_idx == CN_AST_NIL_IDX) goto error;

        node.assign.left_idx = left_idx;
        node.assign.operator = op_kind;
        node.assign.right_idx = right_idx;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.assign.left_idx,
                node.assign.right_idx
                );
        return parent;
    }

    // Postfix operator case.
    op_kind = cn_ast_is_postfix_operator(lexer);
    if (op_kind != CN_POSTFIX_OP_NONE) {

        if (CN_POSTFIX_OPERATOR_PRECEDENCE <= min_precedence) {
            return left_idx;
        }

        Cn_Ast_Node node = { .kind = CN_AST_POSTFIX, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        node.postfix.expression_idx = left_idx;
        node.postfix.operator = op_kind;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.postfix.expression_idx,
                );
        return parent;
    }

    return left_idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}
 
CNDEF Cn_Ast_Idx cn_ast_parse_expression(Cn_Lexer *lexer, int min_precedence, Cn_Expression_Parsing_Flags flags) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Idx left_idx = cn_ast_parse_expression_leaf(lexer, flags);
    if (left_idx == CN_AST_NIL_IDX) goto error;

    Cn_Ast_Idx node_idx;

    while (true) {
        node_idx = cn_ast_parse_expression_increasing_precedence(lexer, left_idx, min_precedence, flags);
        if (node_idx == left_idx) break;

        if (node_idx == CN_AST_NIL_IDX) goto error;

        left_idx = node_idx;
    }

    Cn_Ast_Node *node = cn_ast_get(left_idx);
    CN_ASSERT(node->kind != CN_AST_UNKNOWN);
    return left_idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_leaf(Cn_Lexer *lexer, Cn_Expression_Parsing_Flags flags) {
    Cn_Lexer original_state = *lexer;

    // Checking if parenthesized expression, handling cast expression in here too.
    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_lexer_next_token(lexer);
        
        // Cast expression case.
        if (cn_ast_starts_type(lexer)) {
            Cn_Ast_Node node = { .kind = CN_AST_CAST, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            Cn_Ast_Idx type_name_idx = cn_ast_parse_type_name(lexer);
            if (type_name_idx == CN_AST_NIL_IDX) goto error;
            
            if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
            
            // Compound literal case.
            if (cn_parse_optional(lexer, CN_TOKEN_CURLY_OPEN)) {
                node.kind = CN_AST_COMPOUND;
                
                node.compound.type_name_idx = type_name_idx;

                bool ok;
                node.compound.designations = cn_ast_parse_designations(lexer, &ok);
                if (!ok) goto error;

                if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE)) goto error;
                
                Cn_Ast_Idx parent = cn_ast_node_list_append(node);
                cn_ast_node_set_parent(parent, node.compound.type_name_idx);
                cn_ast_list_set_parent(parent, &node.compound.designations);
                return parent;
            }

            Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, CN_UNARY_OPERATOR_PRECEDENCE, flags);
            if (expression_idx == CN_AST_NIL_IDX) goto error;

            node.cast.type_name_idx = type_name_idx;
            node.cast.expression_idx = expression_idx;

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, 
                    node.cast.type_name_idx,
                    node.cast.expression_idx
                    );
            return parent;
        }
        
        // Parenthesized expression case.
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        
        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

        return expression_idx;
    }

    // Handling sizeof expression.
    if (cn_lexer_expect(lexer, CN_TOKEN_SIZEOF)) {
        Cn_Ast_Node node = { .kind = CN_AST_SIZEOF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
        cn_lexer_next_token(lexer);

        if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Lexer peeking_lexer = *lexer;
            cn_lexer_next_token(&peeking_lexer);

            if (cn_ast_starts_type(&peeking_lexer)) {
                // Parsing type_name.
                cn_lexer_next_token(lexer);
                
                Cn_Ast_Idx type_name_idx = cn_ast_parse_type_name(lexer);
                if (type_name_idx == CN_AST_NIL_IDX) goto error;

                if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

                node.sizeof_expression.target_idx = type_name_idx;

                Cn_Ast_Idx parent = cn_ast_node_list_append(node);
                cn_ast_node_set_parent(parent, node.sizeof_expression.target_idx);
                return parent;
            }
        }

        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, CN_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression_idx == CN_AST_NIL_IDX) goto error;

        node.sizeof_expression.target_idx = expression_idx;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, node.sizeof_expression.target_idx);
        return parent;
    }

    // Handling unary expression.
    Cn_Unary_Operator_Kind op_kind = cn_ast_is_unary_operator(lexer);
    if (op_kind != CN_UNARY_OP_NONE) {
        Cn_Ast_Node node = { .kind = CN_AST_UNARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, CN_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression_idx == CN_AST_NIL_IDX) goto error;

        node.unary.operator = op_kind;
        node.unary.expression_idx = expression_idx;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, node.unary.expression_idx);
        return parent;
    }
    
    // Primary expression case.
    Cn_Ast_Node node = { .kind = CN_AST_PRIMARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    Cn_Ast_Idx literal_idx;

    switch (cn_lexer_token(lexer).type) {
        case CN_TOKEN_IDENTIFIER:
            literal_idx = cn_ast_parse_identifier(lexer);
            break;
        case CN_TOKEN_INTEGER_VALUE:
            literal_idx = cn_ast_parse_integer(lexer);
            break;
        case CN_TOKEN_FLOAT_VALUE:
            literal_idx = cn_ast_parse_float(lexer);
            break;
        case CN_TOKEN_STRING:
            literal_idx = cn_ast_parse_string(lexer);
            break;
        default:
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_EXPRESSION, "Expected symbol, integer, float or string token for primary expression.");
            goto error;
    }
    
    if (literal_idx == CN_AST_NIL_IDX) goto error;
    node.primary.literal_idx = literal_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.primary.literal_idx);

    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF bool cn_ast_is_primary(Cn_Ast_Idx node_idx) {
    Cn_Ast_Node *node = cn_ast_get(node_idx);
    return node->kind == CN_AST_IDENTIFIER || 
        node->kind == CN_AST_INTEGER ||
        node->kind == CN_AST_FLOAT || 
        node->kind == CN_AST_STRING;
}

CNDEF Cn_Ast_List cn_ast_continue_init_declarators(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    Cn_Ast_Idx child_idx = cn_ast_finish_init_declarator(lexer, declarator_idx);

    while (true) {
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(child_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);

        child_idx = cn_ast_parse_init_declarator(lexer);
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_List cn_ast_parse_init_declarators(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    bool decl_ok;
    Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer, &decl_ok);
    if (!decl_ok) goto error;

    if (declarator_idx == CN_AST_NIL_IDX || cn_ast_get_as_node(declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator_idx, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator inside init declarator.");
        goto error;
    }

    return cn_ast_continue_init_declarators(lexer, declarator_idx, ok);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_finish_init_declarator(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_INIT_DECLARATOR, .loc = ((Cn_Ast_Node *)cn_ast_get(declarator_idx))->loc, .src = ((Cn_Ast_Node *)cn_ast_get(declarator_idx))->src };

    node.init_declarator.declarator_idx = declarator_idx;

    if (cn_lexer_expect(lexer, CN_TOKEN_ASM)) {
        Cn_Ast_Idx gnu_asm_label_idx = cn_ast_parse_gnu_asm_label(lexer);
        if (gnu_asm_label_idx == CN_AST_NIL_IDX) goto error;
        node.init_declarator.gnu_asm_label_idx = gnu_asm_label_idx;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_ASSIGN)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx initializer_idx = cn_ast_parse_initializer(lexer);
        if (initializer_idx == CN_AST_NIL_IDX) goto error;
        node.init_declarator.initializer_idx = initializer_idx;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.init_declarator.declarator_idx,
            node.init_declarator.initializer_idx,
            node.init_declarator.gnu_asm_label_idx,
            );
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    bool ok;
    Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    if (declarator_idx == CN_AST_NIL_IDX || cn_ast_get_as_node(declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator_idx, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator inside init declarator.");
        goto error;
    }

    return cn_ast_finish_init_declarator(lexer, declarator_idx);
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_INITIALIZER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_parse_optional(lexer, CN_TOKEN_CURLY_OPEN)) {
            
        if (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            bool ok;
            node.initializer.designations = cn_ast_parse_designations(lexer, &ok);   
            if (!ok) goto error;
        }

        if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE)) goto error;
    } else {
        node.initializer.expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (node.initializer.expression_idx == CN_AST_NIL_IDX) goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.initializer.expression_idx);
    cn_ast_list_set_parent(parent, &node.initializer.designations);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_designations(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();
    
    while (true) {
        Cn_Ast_Idx designation_idx = cn_ast_parse_designation(lexer);
        if (designation_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(designation_idx);

        if (!cn_parse_optional(lexer, CN_TOKEN_COMMA)) break;
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_designation(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_DESIGNATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_ast_starts_designator(lexer)) {

        bool ok;
        node.designation.designators = cn_ast_parse_designators(lexer, &ok);
        if (!ok) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_ASSIGN)) goto error;
    }

    Cn_Ast_Idx initializer_idx = cn_ast_parse_initializer(lexer);
    if (initializer_idx == CN_AST_NIL_IDX) goto error;
    node.designation.initializer_idx = initializer_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.designation.designators);
    cn_ast_node_set_parent(parent, node.designation.initializer_idx);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_designators(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();
    
    while (true) {
        Cn_Ast_Idx designator_idx = cn_ast_parse_designator(lexer);
        if (designator_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(designator_idx);

        if (!cn_ast_starts_designator(lexer)) break;
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_designator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_DESIGNATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_parse_optional(lexer, CN_TOKEN_DOT)) {
        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
        if (identifier_idx == CN_AST_NIL_IDX) goto error;
        node.designator.identifier_idx = identifier_idx;

    } else {
        if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) goto error;

        Cn_Ast_Idx expression_idx;

        expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.designator.expression_idx = expression_idx;

        if (cn_parse_optional(lexer, CN_TOKEN_ELLIPSIS)) {
            expression_idx = cn_ast_parse_expression(lexer, -1, 0);
            if (expression_idx == CN_AST_NIL_IDX) goto error;
            node.designator.expression_range_end_idx = expression_idx;
        }

        if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.designator.identifier_idx,
            node.designator.expression_idx,
            node.designator.expression_range_end_idx,
            );
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF bool cn_ast_starts_designator(Cn_Lexer *lexer) {
    return cn_lexer_expect(lexer, CN_TOKEN_DOT) || cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN);
}

CNDEF Cn_Ast_Idx cn_ast_parse_declarator(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx child_idx;
    
    // Optional pointer in declarator.
    if (cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        child_idx = cn_ast_parse_pointer(lexer);

        if (child_idx == CN_AST_NIL_IDX) goto error;

        node.declarator.pointer_idx = child_idx;
    }
    
    // Direct declarator parsing.
    bool is_abstract;
    child_idx = cn_ast_parse_direct_declarator(lexer, &is_abstract);

    // Checking if it is abstract declarator, if it is then child_idx allowed to be NIL.
    if (is_abstract) {

        // Completely empty abstract declarator case:
        if (child_idx == CN_AST_NIL_IDX && node.declarator.pointer_idx == CN_AST_NIL_IDX) {
            *ok = true;
            return CN_AST_NIL_IDX;
        }

        node.flags |= CN_AST_DECLARATOR_IS_ABSTRACT;

    } else if (child_idx == CN_AST_NIL_IDX) goto error;

    node.declarator.direct_declarator_idx = child_idx;
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.declarator.pointer_idx,
            node.declarator.direct_declarator_idx
            );
    *ok = true;
    return parent;
    
error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_pointer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_POINTER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_ASTERISK)) goto error;
    
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
        node.pointer.pointer_idx = child_idx;
    }
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.pointer.pointer_idx);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_direct_declarator(Cn_Lexer *lexer, bool *is_abstract) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Idx direct_declarator_idx;

    // '(' declarator ')' case.
    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        Cn_Ast_Node node = { .kind = CN_AST_DIRECT_DECLARATOR_GROUPED, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        bool ok;
        Cn_Ast_Idx child_idx = cn_ast_parse_declarator(lexer, &ok);
        if (!ok) goto error;

        node.direct_declarator_grouped.declarator_idx = child_idx;

        // Making above declarator abstract if its child is abstract too.
        if (child_idx == CN_AST_NIL_IDX || cn_ast_get_as_node(child_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) *is_abstract = true;
        

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

        direct_declarator_idx = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(direct_declarator_idx, node.direct_declarator_grouped.declarator_idx);
    }
    // identifier case.
    else if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        *is_abstract = false;
        direct_declarator_idx = cn_ast_parse_identifier(lexer);
    }
    // abstract declarator case.
    else {
        *is_abstract = true;
        direct_declarator_idx = CN_AST_NIL_IDX;
    }

    // Postfix cases.
    while (true) {
        // direct_declarator '[' expression? ']' case.
        if (cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
            Cn_Ast_Node node = { .kind = CN_AST_DIRECT_DECLARATOR_ARRAY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            cn_lexer_next_token(lexer);

            if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
                Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
                if (expression_idx == CN_AST_NIL_IDX) goto error;
                node.direct_declarator_array.expression_idx = expression_idx;

                // Type checking expression.
                Cn_Type *type = cn_ast_expression_typecheck(expression_idx);
                if (type == NULL) goto error;
            }

            if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;

            // Extending source loc to highlight whole array '[...]' declarator.
            node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

            node.direct_declarator_array.direct_declarator_idx = direct_declarator_idx;

            direct_declarator_idx = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(direct_declarator_idx, node.direct_declarator_array.direct_declarator_idx);
            continue;
        }
        // direct_declarator '(' ('void' | parameter_declarations)? ')' case.
        else if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Ast_Node node = { .kind = CN_AST_DIRECT_DECLARATOR_FUNCTION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            cn_lexer_next_token(lexer);

            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                // If not '(' 'void' ')' case.
                if (cn_lexer_expect(lexer, CN_TOKEN_VOID) && cn_lexer_peek(lexer, 1).type == CN_TOKEN_PARAN_CLOSE) {
                    cn_lexer_next_token(lexer);
                } else {
                    bool variadic;
                    bool ok;
                    node.direct_declarator_function.parameter_declarations = cn_ast_parse_parameter_declarations(lexer, &variadic, &ok);
                    if (!ok) goto error;
                    if (variadic) node.flags |= CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC;
                }
            }

            if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

            // Extending source loc to highlight whole function '(...)' declarator.
            node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

            node.direct_declarator_function.direct_declarator_idx = direct_declarator_idx;

            direct_declarator_idx = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(direct_declarator_idx, node.direct_declarator_function.direct_declarator_idx);
            cn_ast_list_set_parent(direct_declarator_idx, &node.direct_declarator_function.parameter_declarations);
            continue;
        }

        // No postfix case.
        return direct_declarator_idx;
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_identifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_IDENTIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected identifier token.");
        goto error;
    }
    
    node.identifier.name = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_node_list_append(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_integer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_INTEGER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_INTEGER_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected integer token.");
        goto error;
    }

    node.integer.value = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_node_list_append(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_float(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_FLOAT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_FLOAT_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected float token.");
        goto error;
    }

    node.flt.value = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_node_list_append(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_string(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_STRING, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    
    if (!cn_lexer_expect(lexer, CN_TOKEN_STRING)){
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected string token.");
        goto error;
    }

    Cn_String str;
    do {
        str = cn_source_to_str(&cn_lexer_token(lexer).src);
        // Strip "" from beginning and end of the string.
        str.data++;
        str.length -= 2;

        if (!cn_str_is_empty(str)) {
            cn_sb_append_str(&sb, str);
        }

        cn_lexer_next_token(lexer);
    } while (cn_lexer_expect(lexer, CN_TOKEN_STRING));

    node.string.str = cn_sb_to_str(&sb);

    void *d = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, node.string.str.length);
    cn_str_copy_to(node.string.str, d);
    node.string.str.data = d;

    cn_sb_free(&sb);

    return cn_ast_node_list_append(node);

error:
    CN__TRACE_ERROR
    cn_sb_free(&sb);
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_declaration_specifiers(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_DECLARATION_SPECIFIERS, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // For accumulating primitive type info (int, char, signed, unsigned, long, short, etc.)
    Cn_Primitive_Type_Info primitive_info = {0};
    int64_t gnu_attr_mark = cn_ast_idx_stack_mark();

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

        ok = cn_ast_try_parse_function_specifier(lexer, &node.declaration_specifiers.function_specifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            Cn_Ast_Idx gnu_attribute_specifier = cn_ast_parse_gnu_attribute_specifier(lexer);
            if (gnu_attribute_specifier == CN_AST_NIL_IDX) goto error;
            cn_ast_idx_stack_push(gnu_attribute_specifier);
            at_least_one = true;
            continue;
        }

        ok = cn_ast_try_parse_type_specifier(lexer, &primitive_info, &node.declaration_specifiers.type_specifier_idx);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        break;
    }

    node.declaration_specifiers.gnu_attribute_specifiers = cn_ast_idx_stack_finalize(gnu_attr_mark);

    // If at least one other declaration specifier present continue, if not error.
    if (!at_least_one) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MISSING_DECLARATION_SPECIFIER, "At least one declaration specifier should be present.");
        goto error;
    }

    // If we have primitive type info, create the node now.
    // Otherwise type_specifier_idx was already set by cn_ast_try_parse_type_specifier.
    if (primitive_info.kind != CN_AST_TYPE_NONE || primitive_info.width != CN_AST_TYPE_WIDTH_NONE || primitive_info.sign != CN_AST_TYPE_SIGN_NONE) {
        // Implicit int case - if width or sign specified but no kind.
        if (primitive_info.kind == CN_AST_TYPE_NONE) {
            primitive_info.kind = CN_AST_TYPE_INT;
        }

        if (primitive_info.width != CN_AST_TYPE_WIDTH_NONE && primitive_info.kind != CN_AST_TYPE_INT) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &node.loc, &node.src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type width on non 'int' type.");
            goto error;
        }

        if (primitive_info.sign != CN_AST_TYPE_SIGN_NONE && primitive_info.kind != CN_AST_TYPE_INT && primitive_info.kind != CN_AST_TYPE_CHAR) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &node.loc, &node.src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type sign on non 'int' or 'char' type.");
            goto error;
        }

        Cn_Ast_Node ts_node = {
            .type_specifier_primitive.kind = CN_AST_TYPE_SPECIFIER_PRIMITIVE,
            .type_specifier_primitive.loc = node.loc,
            .type_specifier_primitive.src = node.src,
            .type_specifier_primitive.primitive_kind = primitive_info.kind,
            .type_specifier_primitive.width = primitive_info.width,
            .type_specifier_primitive.sign = primitive_info.sign,
        };
        node.declaration_specifiers.type_specifier_idx = cn_ast_node_list_append(ts_node);
    }

    Cn_Ast_Idx idx = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(idx, node.declaration_specifiers.type_specifier_idx);
    cn_ast_list_set_parent(idx, &node.declaration_specifiers.gnu_attribute_specifiers);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF int cn_ast_try_parse_storage_specifier(Cn_Lexer *lexer, Cn_Storage_Specifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    switch(cn_lexer_token(lexer).type) {
        case CN_TOKEN_STATIC:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_STORAGE_SPECIFIER_STATIC;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_EXTERN:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_STORAGE_SPECIFIER_EXTERN;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_REGISTER:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_STORAGE_SPECIFIER_REGISTER;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_AUTO:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_STORAGE_SPECIFIER_AUTO;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_TYPEDEF:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_STORAGE_SPECIFIER_TYPEDEF;

                cn_lexer_next_token(lexer);
                return 0;
            }
        default: 
            return 1;
    }

error_multiple_storage_specifier:
    cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MULTIPLE_STORAGE_SPECIFIERS, "Multiple storage specifiers are not allowed.");
    goto error;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return 2;
}

CNDEF int cn_ast_try_parse_qualifier(Cn_Lexer *lexer, Cn_Qualifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    switch(cn_lexer_token(lexer).type) {
        case CN_TOKEN_CONST:
            {
                cn_lexer_next_token(lexer);
                *output |= CN_AST_TYPE_QUALIFIER_CONST;
                return 0;
            }
        case CN_TOKEN_RESTRICT:
            {
                cn_lexer_next_token(lexer);
                *output |= CN_AST_TYPE_QUALIFIER_RESTRICT;
                return 0;
            }
        case CN_TOKEN_VOLATILE:
            {
                cn_lexer_next_token(lexer);
                *output |= CN_AST_TYPE_QUALIFIER_VOLATILE;
                return 0;
            }
        case CN_TOKEN_ATOMIC:
            {
                // Case where it is atomic specifier is not handled here, if such case appears just skip.
                if (cn_lexer_peek(lexer, 1).type == CN_TOKEN_PARAN_OPEN) return 1;

                cn_lexer_next_token(lexer);
                *output |= CN_AST_TYPE_QUALIFIER_ATOMIC;
                return 0;
            }
        default: 
            return 1;
    }

    *lexer = original_state;
    return 2;
}

CNDEF int cn_ast_try_parse_function_specifier(Cn_Lexer *lexer, Cn_Function_Specifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    switch(cn_lexer_token(lexer).type) {
        case CN_TOKEN_INLINE:
            {
                cn_lexer_next_token(lexer);
                *output |= CN_AST_FUNCTION_SPECIFIER_INLINE;
                return 0;
            }
        case CN_TOKEN_NORETURN:
            {
                cn_lexer_next_token(lexer);
                *output |= CN_AST_FUNCTION_SPECIFIER_NORETURN;
                return 0;
            }
        default: 
            return 1;
    }

    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_typeof(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;
    Cn_Ast_Node node = { .kind = CN_AST_GNU_TYPEOF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_TYPEOF)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Idx child_idx;

    if (cn_ast_starts_type(lexer)) {
        child_idx = cn_ast_parse_type_name(lexer);
    } else {
        child_idx = cn_ast_parse_expression(lexer, 0, 0);

        // Typechecking expression right after parsing.
        if (cn_ast_expression_typecheck(child_idx) == NULL) goto error;
    }

    if (child_idx == CN_AST_NIL_IDX) goto error;
    node.gnu_typeof.target_idx = child_idx;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.gnu_typeof.target_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF int cn_ast_try_parse_type_specifier(Cn_Lexer *lexer, Cn_Primitive_Type_Info *primitive_info, Cn_Ast_Idx *type_specifier_idx) {
    Cn_Lexer original_state = *lexer;

    // Checking if token is primitive type kind.
    for (Cn_Ast_Type_Kind kind = CN_AST_TYPE_INT; kind < CN_ARRAY_LENGTH(CN_AST_TYPE_KINDS); kind++) {
        if (cn_lexer_expect(lexer, CN_AST_TYPE_KINDS[kind])) {
            if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier_idx != CN_AST_NIL_IDX) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
                goto error;
            }

            cn_lexer_next_token(lexer);
            primitive_info->kind = kind;
            return 0;
        }
    }

    // Checking if token is typedef.
    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        Cn_String str = cn_source_to_str(&cn_lexer_token(lexer).src);
        Cn_Ast_Binding_Idx *ref = cn_hash_table_get(&cn__ast_data->symbol_binding_table, &str);

        if (ref != NULL && cn__ast_data->binding_list[*ref].kind == CN_BINDING_TYPEDEF) {
            if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier_idx != CN_AST_NIL_IDX) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
                goto error;
            }

            Cn_Ast_Node node = {
                .type_specifier_typedef.kind = CN_AST_TYPE_SPECIFIER_TYPEDEF,
                .type_specifier_typedef.loc = cn_lexer_token(lexer).loc,
                .type_specifier_typedef.src = cn_lexer_token(lexer).src,
                .type_specifier_typedef.typedef_name = str,
            };

            cn_lexer_next_token(lexer);
            *type_specifier_idx = cn_ast_node_list_append(node);
            return 0;
        }
    }

    // Checking if token is struct or union.
    if (cn_lexer_expect(lexer, CN_TOKEN_STRUCT) || cn_lexer_expect(lexer, CN_TOKEN_UNION)) {
        if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier_idx != CN_AST_NIL_IDX) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
            goto error;
        }

        Cn_Ast_Idx struct_or_union_idx = cn_ast_parse_struct_or_union_specifier(lexer);
        if (struct_or_union_idx == CN_AST_NIL_IDX) goto error;

        *type_specifier_idx = struct_or_union_idx;
        return 0;
    }

    // Checking if token is enum.
    if (cn_lexer_expect(lexer, CN_TOKEN_ENUM)) {
        if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier_idx != CN_AST_NIL_IDX) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
            goto error;
        }

        Cn_Ast_Idx enum_idx = cn_ast_parse_enum_specifier(lexer);
        if (enum_idx == CN_AST_NIL_IDX) goto error;

        *type_specifier_idx = enum_idx;
        return 0;
    }

    // Checking if typeof.
    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)) {
        if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier_idx != CN_AST_NIL_IDX) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
            goto error;
        }

        Cn_Ast_Idx gnu_typeof_idx = cn_ast_parse_gnu_typeof(lexer);
        if (gnu_typeof_idx == CN_AST_NIL_IDX) goto error;

        *type_specifier_idx = gnu_typeof_idx;
        return 0;
    }

    // Checking if token is type sign.
    if (cn_lexer_expect(lexer, CN_TOKEN_SIGNED)) {
        if (primitive_info->sign != CN_AST_TYPE_SIGN_NONE) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type sign 'signed' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        primitive_info->sign = CN_AST_TYPE_SIGN_SIGNED;
        return 0;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_UNSIGNED)) {
        if (primitive_info->sign != CN_AST_TYPE_SIGN_NONE) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type sign 'unsigned' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        primitive_info->sign = CN_AST_TYPE_SIGN_UNSIGNED;
        return 0;
    }

    // Checking if token is type width.
    if (cn_lexer_expect(lexer, CN_TOKEN_SHORT)) {
        if (primitive_info->width != CN_AST_TYPE_WIDTH_NONE) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type width 'short' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        primitive_info->width = CN_AST_TYPE_WIDTH_SHORT;
        return 0;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_LONG)) {
        if (primitive_info->width != CN_AST_TYPE_WIDTH_NONE) {
            if (primitive_info->width == CN_AST_TYPE_WIDTH_LONG) {
                cn_lexer_next_token(lexer);
                primitive_info->width = CN_AST_TYPE_WIDTH_LONG_LONG;
                return 0;
            }

            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type width 'long' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        primitive_info->width = CN_AST_TYPE_WIDTH_LONG;
        return 0;
    }

    // INCOMPLETE:
    // Checking if token(s) atomic_type_specifier.

    return 1;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Idx cn_ast_parse_type_name(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_TYPE_NAME, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
    if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
    node.type_name.specifier_qualifier_idx = specifier_qualifier_idx;

    bool ok;
    Cn_Ast_Idx abstract_declarator_idx = cn_ast_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    if (abstract_declarator_idx != CN_AST_NIL_IDX) {
        if (!(cn_ast_get_as_node(abstract_declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_NAME, "Only abstract declarator is allowed in type name.");
            goto error;
        }
    }

    node.type_name.abstract_declarator_idx = abstract_declarator_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.type_name.specifier_qualifier_idx,
            node.type_name.abstract_declarator_idx,
            );
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_specifier_qualifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_SPECIFIER_QUALIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // For accumulating primitive type info.
    Cn_Primitive_Type_Info primitive_info = {0};

    bool at_least_one = false;
    int ok;
    while (true) {
        ok = cn_ast_try_parse_qualifier(lexer, &node.specifier_qualifier.qualifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        ok = cn_ast_try_parse_type_specifier(lexer, &primitive_info, &node.specifier_qualifier.type_specifier_idx);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        break;
    }

    // If at least one specifier/qualifier present continue, if not error.
    if (!at_least_one) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MISSING_DECLARATION_SPECIFIER, "At least one specifier qualifier should be present.");
        goto error;
    }

    // If we have primitive type info, create the node now.
    if (primitive_info.kind != CN_AST_TYPE_NONE || primitive_info.width != CN_AST_TYPE_WIDTH_NONE || primitive_info.sign != CN_AST_TYPE_SIGN_NONE) {
        // Implicit int case.
        if (primitive_info.kind == CN_AST_TYPE_NONE) {
            primitive_info.kind = CN_AST_TYPE_INT;
        }

        if (primitive_info.width != CN_AST_TYPE_WIDTH_NONE && primitive_info.kind != CN_AST_TYPE_INT) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type width on non 'int' type.");
            goto error;
        }

        if (primitive_info.sign != CN_AST_TYPE_SIGN_NONE && primitive_info.kind != CN_AST_TYPE_INT && primitive_info.kind != CN_AST_TYPE_CHAR) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type sign on non 'int' or 'char' type.");
            goto error;
        }

        Cn_Ast_Node ts_node = {
            .type_specifier_primitive.kind = CN_AST_TYPE_SPECIFIER_PRIMITIVE,
            .type_specifier_primitive.loc = node.loc,
            .type_specifier_primitive.src = node.src,
            .type_specifier_primitive.primitive_kind = primitive_info.kind,
            .type_specifier_primitive.width = primitive_info.width,
            .type_specifier_primitive.sign = primitive_info.sign,
        };
        node.specifier_qualifier.type_specifier_idx = cn_ast_node_list_append(ts_node);
    }

    Cn_Ast_Idx idx = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(idx, node.specifier_qualifier.type_specifier_idx);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_parameter_declarations(Cn_Lexer *lexer, bool *variadic, bool *ok) {
    Cn_Lexer original_state = *lexer;

    *variadic = false;
    int64_t mark = cn_ast_idx_stack_mark();

    while (true) {
        // Checking if '...'.
        if (cn_lexer_expect(lexer, CN_TOKEN_ELLIPSIS)) {
            *variadic = true;
            cn_lexer_next_token(lexer);
            break;
        }

        Cn_Ast_Idx parameter_declaration = cn_ast_parse_parameter_declaration(lexer);
        if (parameter_declaration == CN_AST_NIL_IDX) goto error;

        cn_ast_idx_stack_push(parameter_declaration);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;

        cn_lexer_next_token(lexer);
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_parameter_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_PARAMETER_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    node.parameter_declaration.declaration_specifiers_idx = cn_ast_parse_declaration_specifiers(lexer);
    if (node.parameter_declaration.declaration_specifiers_idx == CN_AST_NIL_IDX) goto error;

    bool ok;
    node.parameter_declaration.declarator_idx = cn_ast_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    Cn_Ast_Idx idx = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(idx, node.parameter_declaration.declaration_specifiers_idx, node.parameter_declaration.declarator_idx);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_struct_or_union_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    switch (cn_lexer_token(lexer).type) {
        case CN_TOKEN_STRUCT:
            {
                node.kind = CN_AST_STRUCT_SPECIFIER;
                break;
            }
        case CN_TOKEN_UNION:
            {
                node.kind = CN_AST_UNION_SPECIFIER;
                break;
            }
        default:
            {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Not a struct or union specifier.");
                goto error;
            }
    }

    cn_lexer_next_token(lexer);
    
    // Parse tag identifier.
    if (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer); 
        node.kind == CN_AST_STRUCT_SPECIFIER ? (node.struct_specifier.identifier_idx = identifier_idx) : (node.union_specifier.identifier_idx = identifier_idx);
    } else {
        // Generating unique tag name for anonymous struct.
        Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
        cn_sb_append_format(&sb, "cn__struct_%lu", cn_ast_counter_next());
        Cn_String tag = cn_sb_to_str(&sb);

        void *data = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, tag.length);
        cn_str_copy_to(tag, data);
        tag.data = data;

        Cn_Ast_Idx identifier_idx = cn_ast_node_list_append((Cn_Ast_Node) {
                    .identifier.kind = CN_AST_IDENTIFIER,
                    .identifier.loc = cn_lexer_token(lexer).loc,
                    .identifier.name = tag,
                });

        node.kind == CN_AST_STRUCT_SPECIFIER ? (node.struct_specifier.identifier_idx = identifier_idx) : (node.union_specifier.identifier_idx = identifier_idx);

        cn_sb_free(&sb);
    }

    // Parsing struct or union body.
    int64_t mark = cn_ast_idx_stack_mark();
    
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        cn_lexer_next_token(lexer);

        while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            Cn_Ast_Idx member_declaration_idx = cn_ast_parse_member_declaration(lexer);
            if (member_declaration_idx == CN_AST_NIL_IDX) goto error;
            cn_ast_idx_stack_push(member_declaration_idx);

        }
        cn_lexer_next_token(lexer);

        node.kind == CN_AST_STRUCT_SPECIFIER ? 
            (node.struct_specifier.member_declarations = cn_ast_idx_stack_finalize(mark)) : 
            (node.union_specifier.member_declarations = cn_ast_idx_stack_finalize(mark));
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    if (node.kind == CN_AST_STRUCT_SPECIFIER) {
        cn_ast_node_set_parent(parent, node.struct_specifier.identifier_idx);
        cn_ast_list_set_parent(parent, &node.struct_specifier.member_declarations);
    } else {
        cn_ast_node_set_parent(parent, node.struct_specifier.identifier_idx);
        cn_ast_list_set_parent(parent, &node.struct_specifier.member_declarations);
    }
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_member_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_MEMBER_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
    if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
    node.member_declaration.specifier_qualifier_idx = specifier_qualifier_idx;

    bool ok;
    node.member_declaration.member_declarators = cn_ast_parse_member_declarators(lexer, &ok);
    if (!ok) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.member_declaration.specifier_qualifier_idx);
    cn_ast_list_set_parent(parent, &node.member_declaration.member_declarators);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_member_declarators(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    while (true) {
        Cn_Ast_Idx member_declarator_idx = cn_ast_parse_member_declarator(lexer);
        if (member_declarator_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(member_declarator_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);
    }
    
    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_member_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_MEMBER_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
        bool ok;
        Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer, &ok);
        if (!ok) goto error;

        if (declarator_idx != CN_AST_NIL_IDX && cn_ast_get_as_node(declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator_idx, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator inside member declarator.");
            goto error;
        }

        node.member_declarator.declarator_idx = declarator_idx;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, 0, CN_NO_COMMA_OPERATOR);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.member_declarator.bitfield_idx = expression_idx;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.member_declarator.declarator_idx,
            node.member_declarator.bitfield_idx
            );
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_enum_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_ENUM_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_ENUM)) goto error;
    
    bool ok;

    node.enum_specifier.attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    node.enum_specifier.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
        if (identifier_idx == CN_AST_NIL_IDX) goto error;
        node.enum_specifier.identifier_idx = identifier_idx;
    }

    if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
        Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
        if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
        node.enum_specifier.specifier_qualifier_idx = specifier_qualifier_idx;
    }

    bool is_enum_definition_optional = node.enum_specifier.identifier_idx != CN_AST_NIL_IDX;

    if (cn_parse_optional(lexer, CN_TOKEN_CURLY_OPEN)) {
        
        int64_t mark = cn_ast_idx_stack_mark();

        Cn_Ast_Idx enumerator_idx;
        while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            enumerator_idx = cn_ast_parse_enumerator(lexer);
            if (enumerator_idx == CN_AST_NIL_IDX) goto error;
            cn_ast_idx_stack_push(enumerator_idx);
        }

        node.enum_specifier.enumerators = cn_ast_idx_stack_finalize(mark);
        
        cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE);
    } else if (!is_enum_definition_optional) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected enum definition '{' in enum specifier.");
        goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.enum_specifier.attribute_specifiers);
    cn_ast_list_set_parent(parent, &node.enum_specifier.gnu_attribute_specifiers);
    cn_ast_list_set_parent(parent, &node.enum_specifier.enumerators);
    cn_ast_node_set_parent(parent, 
            node.enum_specifier.identifier_idx,
            node.enum_specifier.specifier_qualifier_idx,
            );
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_enumerator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_ENUMERATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
        if (identifier_idx == CN_AST_NIL_IDX) goto error;
        node.enumerator.identifier_idx = identifier_idx;
    }

    bool ok;

    node.enumerator.attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    node.enumerator.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    if (cn_parse_optional(lexer, CN_TOKEN_ASSIGN)) {
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.enumerator.expression_idx = expression_idx;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.enumerator.attribute_specifiers);
    cn_ast_list_set_parent(parent, &node.enumerator.gnu_attribute_specifiers);
    cn_ast_node_set_parent(parent, 
            node.enumerator.identifier_idx,
            node.enumerator.expression_idx,
            );
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_attribute_specifiers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    while (true) {
        if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN) || cn_lexer_peek(lexer, 1).type != CN_TOKEN_SQR_BRACES_OPEN) {
            *ok = true;
            return cn_ast_idx_stack_finalize(mark);
        }

        Cn_Ast_Idx attribute_specifier_idx = cn_ast_parse_attribute_specifier(lexer);
        if (attribute_specifier_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(attribute_specifier_idx);
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_attribute_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_ATTRIBUTE_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
        bool ok;
        node.attribute_specifier.attributes = cn_ast_parse_attributes(lexer, &ok); 
        if (!ok) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.attribute_specifier.attributes);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_attributes(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    Cn_Ast_Idx attribute_idx = cn_ast_parse_attribute(lexer);
    if (attribute_idx == CN_AST_NIL_IDX) goto error;
    cn_ast_idx_stack_push(attribute_idx);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        attribute_idx = cn_ast_parse_attribute(lexer);
        if (attribute_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(attribute_idx);
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_attribute(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_ATTRIBUTE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    // Extracting vendor identifier.
    if (cn_lexer_peek(lexer, 1).type == CN_TOKEN_COLON && cn_lexer_peek(lexer, 2).type == CN_TOKEN_COLON) {
        Cn_Ast_Idx vendor_identifier_idx = cn_ast_parse_identifier(lexer);
        if (vendor_identifier_idx == CN_AST_NIL_IDX) goto error;
        node.attribute.vendor_identifier_idx = vendor_identifier_idx;

        cn_lexer_next_token(lexer);
        cn_lexer_next_token(lexer);
    }

    Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
    if (identifier_idx == CN_AST_NIL_IDX) goto error;
    node.attribute.identifier_idx = identifier_idx;

    if (cn_parse_optional(lexer, CN_TOKEN_PARAN_OPEN)) {
        bool ok;
        node.attribute.arguments = cn_ast_parse_arguments(lexer, &ok);
        if (!ok) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.attribute.vendor_identifier_idx);
    cn_ast_node_set_parent(parent, node.attribute.identifier_idx);
    cn_ast_list_set_parent(parent, &node.attribute.arguments);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_gnu_attribute_specifiers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    while (true) {
        if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            *ok = true;
            return cn_ast_idx_stack_finalize(mark);
        }

        Cn_Ast_Idx attribute_specifier_idx = cn_ast_parse_gnu_attribute_specifier(lexer);
        if (attribute_specifier_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(attribute_specifier_idx);
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_GNU_ATTRIBUTE_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        bool ok;
        node.gnu_attribute_specifier.gnu_attributes = cn_ast_parse_gnu_attributes(lexer, &ok); 
        if (!ok) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.gnu_attribute_specifier.gnu_attributes);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_List cn_ast_parse_gnu_attributes(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_idx_stack_mark();

    Cn_Ast_Idx gnu_attribute_idx = cn_ast_parse_gnu_attribute(lexer);
    if (gnu_attribute_idx == CN_AST_NIL_IDX) goto error;
    cn_ast_idx_stack_push(gnu_attribute_idx);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        gnu_attribute_idx = cn_ast_parse_gnu_attribute(lexer);
        if (gnu_attribute_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(gnu_attribute_idx);
    }

    *ok = true;
    return cn_ast_idx_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_GNU_ATTRIBUTE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
    if (identifier_idx == CN_AST_NIL_IDX) goto error;
    node.gnu_attribute.identifier_idx = identifier_idx;

    if (cn_parse_optional(lexer, CN_TOKEN_PARAN_OPEN)) {
        bool ok;
        node.gnu_attribute.arguments = cn_ast_parse_arguments(lexer, &ok);
        if (!ok) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.gnu_attribute.identifier_idx);
    cn_ast_list_set_parent(parent, &node.gnu_attribute.arguments);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_asm_label(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_GNU_ASM_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_ASM)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_STRING)) {
        Cn_Ast_Idx string_idx = cn_ast_parse_string(lexer);
        if (string_idx == CN_AST_NIL_IDX) goto error;
        node.gnu_asm_label.string_idx = string_idx;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.gnu_asm_label.string_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Type *cn__ast_add_type_if_not(Cn_Type *type) {
    if (!cn_hash_set_contains(&cn__ast_data->type_ptr_set, type)) {
        // Arena allocate memory for type.
        Cn_Type *result = cn_chained_arena_alloc(&cn__ast_data->type_arena, sizeof(Cn_Type));
        *result = *type;

        return cn_hash_set_put(&cn__ast_data->type_ptr_set, result);
    } else {
        // Acts as a get, since type is already in there, 
        // the put will return actual item stored in the hash set.
        return cn_hash_set_put(&cn__ast_data->type_ptr_set, type);
    }
}

/**
 * Builds the base type for a primitive type specifier.
 *
 * RETURNS: The interned primitive type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_from_primitive(Cn_Ast_Idx ts_idx) {
    Cn_Ast_Type_Specifier_Primitive *prim = cn_ast_get(ts_idx);
    Cn_Type type = {0};
 
    switch (prim->primitive_kind) {
        case CN_AST_TYPE_INT:
            type.integer.kind = CN_INTEGER;
            type.integer.flags |= CN_TYPE_COMPLETE;
            switch (prim->width) {
                case CN_AST_TYPE_WIDTH_NONE:      type.integer.size = sizeof(int);       break;
                case CN_AST_TYPE_WIDTH_SHORT:     type.integer.size = sizeof(short);     break;
                case CN_AST_TYPE_WIDTH_LONG:      type.integer.size = sizeof(long);      break;
                case CN_AST_TYPE_WIDTH_LONG_LONG: type.integer.size = sizeof(long long); break;
            }
            type.integer.align = type.integer.size;
            type.integer.is_signed = prim->sign != CN_AST_TYPE_SIGN_UNSIGNED;
            return cn__ast_add_type_if_not((Cn_Type *)&type);
 
        case CN_AST_TYPE_CHAR:
            type.integer.kind = CN_INTEGER;
            type.integer.flags |= CN_TYPE_COMPLETE;
            type.integer.size = sizeof(char);
            type.integer.align = type.integer.size;
            type.integer.is_signed = prim->sign != CN_AST_TYPE_SIGN_UNSIGNED;
            return cn__ast_add_type_if_not((Cn_Type *)&type);
 
        case CN_AST_TYPE_FLOAT:
            type.kind = CN_FLOAT;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(float);
            type.align = type.integer.size;
            return cn__ast_add_type_if_not((Cn_Type *)&type);
 
        case CN_AST_TYPE_DOUBLE:
            type.kind = CN_FLOAT;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(double);
            type.align = type.size;
            return cn__ast_add_type_if_not((Cn_Type *)&type);
 
        case CN_AST_TYPE_BOOL:
            type.kind = CN_BOOL;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(_Bool);
            type.align = type.size;
            return cn__ast_add_type_if_not((Cn_Type *)&type);
 
        case CN_AST_TYPE_VOID:
            return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_VOID);
 
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts_idx, CN_DC_INVALID_TYPE_SPECIFIER, "Unknown primitive type specifier.");
            goto error;
    }
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Resolves a typedef name specifier to the type bound to that name.
 *
 * RETURNS: The bound type.
 */
CNDEF Cn_Type *cn__ast_type_from_typedef(Cn_Ast_Idx ts_idx) {
    Cn_Ast_Type_Specifier_Typedef *tdef = cn_ast_get(ts_idx);
    
    Cn_Ast_Binding_Idx binding_idx = cn_ast_binding_table_get(tdef->typedef_name, &cn__ast_data->symbol_binding_table);
    return cn_ast_binding_get(binding_idx)->type;
}
 
/**
 * Resolves typeof(...) to the type of its typename or expression operand.
 *
 * RETURNS: The resolved type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_from_gnu_typeof(Cn_Ast_Idx ts_idx) {
    Cn_Ast_Gnu_Typeof *typeof_spec = cn_ast_get(ts_idx);
    Cn_Ast_Node *typeof_child = cn_ast_get(typeof_spec->target_idx);
    Cn_Type *result;
 
    if (typeof_child->kind == CN_AST_TYPE_NAME) {
        Cn_Ast_Type_Name *tn = cn_ast_get(typeof_spec->target_idx);
        Cn_Ast_Specifier_Qualifier *sq = cn_ast_get(tn->specifier_qualifier_idx);
        result = cn_ast_to_type(sq->qualifiers, sq->type_specifier_idx, tn->abstract_declarator_idx);
    } else {
        result = cn_ast_expression_get_type(typeof_spec->target_idx);
    }
 
    if (result == NULL) goto error;
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Declares/completes the struct tag and builds its type: counts members, lays
 * out offsets and alignment, and marks the type complete when a body is present.
 *
 * RETURNS: The (possibly newly completed) struct type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_from_struct(Cn_Ast_Idx ts_idx) {
    Cn_Ast_Struct_Specifier *struct_spec = cn_ast_get(ts_idx);
    Cn_String tag = cn_ast_get_as_node(struct_spec->identifier_idx)->identifier.name;
    Cn_Ast_Binding_Idx binding_idx = cn_ast_tag_binding_declare(tag, CN_STRUCT);
    if (binding_idx == CN_AST_NIL_BINDING_IDX) goto error;
 
    Cn_Ast_Binding *binding = cn__ast_data->binding_list + binding_idx;
 
    if (struct_spec->member_declarations.length > 0) {
        if ((binding->type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts_idx, CN_DC_REDEFINITION, "Redefinition of 'struct %.*s' is not allowed within the same scope.", CN_UNPACK(tag));
            goto error;
        }
 
        Cn_Type_Struct *struct_type = (Cn_Type_Struct *)binding->type;
 
        // Count members across all declarations and their declarators.
        struct_type->members_length = 0;
        for (int64_t m = 0; m < struct_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_get(struct_spec->member_declarations.idxs[m]);
            struct_type->members_length += md->member_declarators.length;
        }
 
        struct_type->members = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, struct_type->members_length * sizeof(Cn_Type_Struct_Member));
 
        int i = 0;
        int64_t offset = 0, max_align = 0;
 
        for (int64_t m = 0; m < struct_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_get(struct_spec->member_declarations.idxs[m]);
            Cn_Ast_Specifier_Qualifier *sq = cn_ast_get(md->specifier_qualifier_idx);
 
            for (int64_t d = 0; d < md->member_declarators.length; d++) {
                Cn_Ast_Member_Declarator *decl = cn_ast_get(md->member_declarators.idxs[d]);
                Cn_Type *member_type = cn_ast_to_type(sq->qualifiers, sq->type_specifier_idx, decl->declarator_idx);
 
                if (!(member_type->flags & CN_TYPE_COMPLETE)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, md->member_declarators.idxs[d], CN_DC_INCOMPLETE_TYPE, "Cannot have incomplete member type in struct definition.");
                    goto error;
                }
 
                Cn_Ast_Idx identifier_idx;
                cn_get_declarator_info(decl->declarator_idx, &identifier_idx);
 
                struct_type->members[i] = (Cn_Type_Struct_Member) {
                    .type = member_type,
                    .name = cn__ast_permanent_save_string(((Cn_Ast_Identifier *)cn_ast_get(identifier_idx))->name),
                };
 
                CN_ASSERT(member_type->align != 0);
                offset = (offset + member_type->align - 1) / member_type->align * member_type->align;
                struct_type->members[i].offset = offset;
                offset += member_type->size;
 
                if (max_align < member_type->align) max_align = member_type->align;
                i++;
            }
        }
 
        binding->type->flags |= CN_TYPE_COMPLETE;
        binding->type->size = offset;
        binding->type->align = max_align;
    }
 
    return binding->type;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Declares/completes the union tag and builds its type: counts members and sizes
 * it to the largest member (rounded to the largest alignment).
 *
 * RETURNS: The (possibly newly completed) union type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_from_union(Cn_Ast_Idx ts_idx) {
    Cn_Ast_Union_Specifier *union_spec = cn_ast_get(ts_idx);
    Cn_String tag = cn_ast_get_as_node(union_spec->identifier_idx)->identifier.name;
    Cn_Ast_Binding_Idx binding_idx = cn_ast_tag_binding_declare(tag, CN_UNION);
    if (binding_idx == CN_AST_NIL_BINDING_IDX) goto error;
 
    Cn_Ast_Binding *binding = cn__ast_data->binding_list + binding_idx;
 
    if (union_spec->member_declarations.length > 0) {
        if ((binding->type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts_idx, CN_DC_REDEFINITION, "Redefinition of 'union %.*s' is not allowed within the same scope.", CN_UNPACK(tag));
            goto error;
        }
 
        Cn_Type_Union *union_type = (Cn_Type_Union *)binding->type;
 
        union_type->members_length = 0;
        for (int64_t m = 0; m < union_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_get(union_spec->member_declarations.idxs[m]);
            union_type->members_length += md->member_declarators.length;
        }
 
        union_type->members = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, union_type->members_length * sizeof(Cn_Type_Union_Member));
 
        int i = 0;
        int64_t max_size = 0, max_align = 0;
 
        for (int64_t m = 0; m < union_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_get(union_spec->member_declarations.idxs[m]);
            Cn_Ast_Specifier_Qualifier *sq = cn_ast_get(md->specifier_qualifier_idx);
 
            for (int64_t d = 0; d < md->member_declarators.length; d++) {
                Cn_Ast_Member_Declarator *decl = cn_ast_get(md->member_declarators.idxs[d]);
                Cn_Type *member_type = cn_ast_to_type(sq->qualifiers, sq->type_specifier_idx, decl->declarator_idx);
 
                if (!(member_type->flags & CN_TYPE_COMPLETE)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, md->member_declarators.idxs[d], CN_DC_INCOMPLETE_TYPE, "Cannot have incomplete member type in union definition.");
                    goto error;
                }
 
                Cn_Ast_Idx identifier_idx;
                cn_get_declarator_info(decl->declarator_idx, &identifier_idx);
 
                union_type->members[i] = (Cn_Type_Union_Member) {
                    .type = member_type,
                    .name = cn__ast_permanent_save_string(((Cn_Ast_Identifier *)cn_ast_get(identifier_idx))->name),
                };
 
                if (member_type->size > max_size)  max_size  = member_type->size;
                if (member_type->align > max_align) max_align = member_type->align;
                i++;
            }
        }
 
        if (max_align != 0) {
            max_size = (max_size + max_align - 1) / max_align * max_align;
        }
 
        binding->type->flags |= CN_TYPE_COMPLETE;
        binding->type->size = max_size;
        binding->type->align = max_align;
    }
 
    return binding->type;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Resolves the base type named by a type specifier, ignoring any declarator.
 *
 * RETURNS: The unqualified base type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_base_type_from_specifier(Cn_Ast_Idx type_specifier_idx) {
    Cn_Ast_Node *ts = cn_ast_get(type_specifier_idx);
    Cn_Type *result;
 
    switch (ts->kind) {
        case CN_AST_TYPE_SPECIFIER_PRIMITIVE:
            result = cn__ast_type_from_primitive(type_specifier_idx);
            break;
        case CN_AST_TYPE_SPECIFIER_TYPEDEF:
            result = cn__ast_type_from_typedef(type_specifier_idx);
            break;
        case CN_AST_STRUCT_SPECIFIER:
            result = cn__ast_type_from_struct(type_specifier_idx);
            break;
        case CN_AST_UNION_SPECIFIER:
            result = cn__ast_type_from_union(type_specifier_idx);
            break;
        case CN_AST_ENUM_SPECIFIER:
            CN_TODO("CN_AST_ENUM_SPECIFIER to type.");
            goto error;
        case CN_AST_GNU_TYPEOF:
            result = cn__ast_type_from_gnu_typeof(type_specifier_idx);
            break;
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, type_specifier_idx, CN_DC_INVALID_TYPE_SPECIFIER, "Unknown or invalid type specifier.");
            goto error;
    }
 
    if (result == NULL) goto error;
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Wraps 'result' in one pointer layer, plus optional qualifier, per '*' in the
 * pointer chain.
 *
 * RETURNS: The pointer wrapped type.
 */
CNDEF Cn_Type *cn__ast_type_apply_pointers(Cn_Type *result, Cn_Ast_Idx pointer_idx) {
    while (pointer_idx != CN_AST_NIL_IDX) {
        Cn_Ast_Pointer *ptr = cn_ast_get(pointer_idx);
 
        Cn_Type type = cn_type_make_pointer(result);
        result = cn__ast_add_type_if_not((Cn_Type *)&type);
 
        if (ptr->qualifiers != 0) {
            type = cn_type_make_qualified(cn_ast_qualifier_flags_to_type(ptr->qualifiers), result);
            result = cn__ast_add_type_if_not((Cn_Type *)&type);
        }
 
        pointer_idx = ptr->pointer_idx;
    }
 
    return result;
}
 
/**
 * Wraps 'result' in one array layer described by the array direct declarator at
 * dd_idx, evaluating its constant length. 
 *
 * TODO: Handle VLA.
 *
 * RETURNS: The array-wrapped type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_apply_array(Cn_Type *result, Cn_Ast_Idx dd_idx) {
    Cn_Ast_Direct_Declarator_Array *arr = cn_ast_get(dd_idx);
    Cn_Type type = {0};
 
    type.array.kind = CN_ARRAY;
    type.array.flags |= CN_TYPE_COMPLETE;
 
    // Allow only complete element types.
    if (!(result->flags & CN_TYPE_COMPLETE)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd_idx, CN_DC_INCOMPLETE_TYPE, "Incomplete type not allowed in array declarator.");
        goto error;
    }
 
    type.array.element_type = result;
 
    // Evaluate the constant length expression.
    uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any any = cn_ast_expression_evaluate(arr->expression_idx, buffer);
 
    if (cn_any_is_empty(any)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in array declarator.");
        goto error;
    }
 
    if (any.type->kind != CN_INTEGER) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array declarator.");
        goto error;
    }
 
    int64_t length = cn_any_read_int(any);
 
    if (length == 0) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "0 length array resulting from constant expression in array declarator.");
        goto error;
    }
 
    type.array.length = length;
    type.array.size = length * result->size;
    type.array.align = result->align;
 
    return cn__ast_add_type_if_not((Cn_Type *)&type);
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Wraps 'result' in one function layer described by the function direct declarator
 * at dd_idx, converting each parameter declaration to a parameter type.
 *
 * RETURNS: The function wrapped type.
 */
CNDEF Cn_Type *cn__ast_type_apply_function(Cn_Type *result, Cn_Ast_Idx dd_idx) {
    Cn_Ast_Direct_Declarator_Function *fn = cn_ast_get(dd_idx);
    Cn_Type type = {0};
 
    type.function.kind = CN_FUNCTION;
    type.function.size = 0;
    type.function.align = 0;
    type.function.return_type = result;
    type.function.is_variadic = (fn->flags & CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC) != 0;
    type.function.params_length = fn->parameter_declarations.length;
 
    if (type.function.params_length > 0) {
        type.function.params = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, sizeof(Cn_Type_Function_Param) * type.function.params_length);
 
        for (int64_t i = 0; i < fn->parameter_declarations.length; i++) {
            Cn_Ast_Parameter_Declaration *p = cn_ast_get(fn->parameter_declarations.idxs[i]);
            Cn_Ast_Declaration_Specifiers *decl_spec = cn_ast_get(p->declaration_specifiers_idx);
            Cn_Type *param_type = cn_ast_to_type(decl_spec->qualifiers, decl_spec->type_specifier_idx, p->declarator_idx);
 
            type.function.params[i] = (Cn_Type_Function_Param) {
                .type = param_type,
            };
        }
    }
 
    return cn__ast_add_type_if_not((Cn_Type *)&type);
}
 
/**
 * Walks the declarator chain, wrapping 'result' in pointer, array or function layers
 * and descending into grouped declarators until each direct declarator bottoms
 * out at the identifier or an abstract end.
 *
 * RETURNS: The fully wrapped type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_apply_declarator(Cn_Type *result, Cn_Ast_Idx declarator_idx) {
    Cn_Ast_Idx declarator_chain_idx = declarator_idx;
 
    while (declarator_chain_idx != CN_AST_NIL_IDX) {
        Cn_Ast_Declarator *decl = cn_ast_get(declarator_chain_idx);
 
        // Pointer layers first.
        result = cn__ast_type_apply_pointers(result, decl->pointer_idx);
 
        // Direct declarator: array/function layers until the identifier, an
        // abstract end (NIL), or a grouped declarator to descend into.
        Cn_Ast_Idx dd_idx = decl->direct_declarator_idx;
        Cn_Ast_Idx next_declarator_idx = CN_AST_NIL_IDX;
 
        while (dd_idx != CN_AST_NIL_IDX) {
            Cn_Ast_Node *dd = cn_ast_get(dd_idx);
 
            if (dd->kind == CN_AST_IDENTIFIER) {
                break;
            }
 
            if (dd->kind == CN_AST_DIRECT_DECLARATOR_GROUPED) {
                next_declarator_idx = ((Cn_Ast_Direct_Declarator_Grouped *)dd)->declarator_idx;
                break;
            }
 
            switch (dd->kind) {
                case CN_AST_DIRECT_DECLARATOR_ARRAY:
                    result = cn__ast_type_apply_array(result, dd_idx);
                    if (result == NULL) goto error;
                    dd_idx = ((Cn_Ast_Direct_Declarator_Array *)dd)->direct_declarator_idx;
                    break;
 
                case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                    result = cn__ast_type_apply_function(result, dd_idx);
                    if (result == NULL) goto error;
                    dd_idx = ((Cn_Ast_Direct_Declarator_Function *)dd)->direct_declarator_idx;
                    break;
 
                default:
                    dd_idx = CN_AST_NIL_IDX;
                    break;
            }
        }
 
        // Only continues when a grouped declarator handed us an inner one.
        declarator_chain_idx = next_declarator_idx;
    }
 
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Converts a qualifiers, type specifier and declarator triple into a resolved Cn_Type.
 * The specifier gives the base type, the qualifiers are applied to it,
 * and the declarator wraps it in pointer, array or function layers.
 *
 * IMPORTNAT: declarator_idx can be NIL, if no declarator present at all.
 * In that case the type is composed only from type specifier and qualifiers.
 *
 * RETURNS: The resolved type, or NULL on error.
 */
CNDEF Cn_Type *cn_ast_to_type(Cn_Qualifier_Flags flags, Cn_Ast_Idx type_specifier_idx, Cn_Ast_Idx declarator_idx) {
    // Base type named by the specifier.
    Cn_Type *result = cn__ast_base_type_from_specifier(type_specifier_idx);
    if (result == NULL) goto error;
 
    // Qualifiers on the base type itself.
    if (flags != 0) {
        Cn_Type type = cn_type_make_qualified(cn_ast_qualifier_flags_to_type(flags), result);
        result = cn__ast_add_type_if_not((Cn_Type *)&type);
    }
 
    // Declarator layers (* [] ()).
    result = cn__ast_type_apply_declarator(result, declarator_idx);
    if (result == NULL) goto error;
 
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__ast_integer_promote(Cn_Type *type) {
    // Integer types of rank lower than int (and _Bool) promote to int.
    if (type->kind == CN_BOOL) {
        return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
    }
    if (type->kind == CN_INTEGER && type->size < (int64_t)sizeof(int)) {
        return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
    }
    return type;
}

CNDEF Cn_Type *cn__ast_usual_arithmetic_conversion(Cn_Type *a, Cn_Type *b) {
    // Promote each operand, then pick the greater rank.
    a = cn__ast_integer_promote(a);
    b = cn__ast_integer_promote(b);
    return cn_type_greatest_arithmetic_rank(a, b);
}

CNDEF void cn__ast_illegal_binary(Cn_Ast_Node *node, Cn_Type *left, Cn_Type *right, const char *op_desc) {
    Cn_String left_str  = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), left);
    Cn_String right_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), right);
    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Illegal %s between %.*s and %.*s types.", op_desc, CN_UNPACK(left_str), CN_UNPACK(right_str));
}

CNDEF bool cn__ast_is_null_pointer_constant(Cn_Ast_Idx expression_idx) {
    Cn_Ast_Primary *node = cn_ast_get(expression_idx);

    if (node->kind == CN_AST_PRIMARY) {
        Cn_Ast_Integer *integer = cn_ast_get(node->literal_idx);
        if (integer->kind == CN_AST_INTEGER) {
            uint64_t value;
            if (!cn_parse_int_literal(integer->value, &value)) return false;
            return value == 0;
        }
    }

    return false;
}

CNDEF bool cn__ast_is_lvalue(Cn_Ast_Idx expression_idx) {
    Cn_Ast_Node *node = cn_ast_get(expression_idx);

    switch (node->kind) {
        case CN_AST_PRIMARY:
            // Only identifiers denote objects; integer/float/string literals don't.
            return ((Cn_Ast_Node *)cn_ast_get(node->primary.literal_idx))->kind == CN_AST_IDENTIFIER;

        case CN_AST_UNARY:
            // *p is an lvalue; -x, !x, &x, etc. are not.
            return node->unary.operator == CN_UNARY_OP_DEREF;

        case CN_AST_ACCESS:
            // p->m is always an lvalue; s.m is an lvalue iff s is one.
            if (!node->access.pointer) {
                return cn__ast_is_lvalue(node->access.expression_idx);
            }
            return true;

        case CN_AST_BINARY:
            // a[b] is an lvalue; no other binary result is.
            return node->binary.operator == CN_BINARY_OP_ARRAY_SUB;

        case CN_AST_COMPOUND:
            // Compound literals counted as lvalues.
            return true;

        default:
            return false;
    }
}

/**
 * Expects expression_idx to be typechecked.
 */
CNDEF bool cn__ast_is_modifiable_lvalue(Cn_Ast_Idx expression_idx) {
    Cn_Ast_Node *node = cn_ast_get(expression_idx);
    Cn_Type *type = NULL;

    switch (node->kind) {
        case CN_AST_PRIMARY:
            if (((Cn_Ast_Node *)cn_ast_get(node->primary.literal_idx))->kind == CN_AST_IDENTIFIER) {
                // Only non-constant identifiers denote objects; integer/float/string literals don't.
                type = node->primary.type;

                if (cn_type_is_constant(type)) return false;

                return true;
            }

            return false;

        case CN_AST_UNARY:
            // *p is an lvalue; -x, !x, &x, etc. are not.
            // *p type must be non constant.
            if (node->unary.operator == CN_UNARY_OP_DEREF) {
                type = node->unary.type;
                if (cn_type_is_constant(type)) return false;

                return true;
            }
            
            return false;

        case CN_AST_ACCESS:
            // p->m is always an lvalue; s.m is an lvalue iff s is one.
            // Base type of p must not be constant.
            // p->m type must not be constant.
            // s.m type must not be constant.
            type = node->access.type;
            if (cn_type_is_constant(type)) return false;

            if (!node->access.pointer) {
                return cn__ast_is_modifiable_lvalue(node->access.expression_idx);
            } 

            Cn_Type *p = cn_ast_expression_get_type(node->access.expression_idx);
            p = cn_type_unqualified(p);
            if (p->kind != CN_POINTER) return false;
            if (cn_type_is_constant(((Cn_Type_Pointer *)p)->ptr_to)) return false;

            return true;

        case CN_AST_BINARY:
            // a[b] is an lvalue; no other binary result is.
            // a[b] must not be constant.
            if (node->binary.operator == CN_BINARY_OP_ARRAY_SUB) {
                type = node->binary.type;
                if (cn_type_is_constant(type)) return false;

                return true;
            }

            return false;

        default:
            return false;
    }
}

CNDEF Cn_Type *cn__ast_binary_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_BINARY);

    Cn_Type *left = cn_ast_expression_typecheck(node->binary.left_idx);
    if (left == NULL) return NULL;

    Cn_Type *right = cn_ast_expression_typecheck(node->binary.right_idx);
    if (right == NULL) return NULL;

    switch (node->binary.operator) {
        case CN_BINARY_OP_ADDITION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn_type_greatest_arithmetic_rank(left, right);
                }
                if (left->kind == CN_POINTER || right->kind == CN_POINTER) {
                    Cn_Type *ptr_type   = left->kind == CN_POINTER ? left  : right;
                    Cn_Type *other_type = left->kind == CN_POINTER ? right : left;
                    if (other_type->kind != CN_INTEGER) {
                        cn__ast_illegal_binary(node, left, right, "pointer addition");
                        return NULL;
                    }
                    return ptr_type;
                }
                cn__ast_illegal_binary(node, left, right, "addition");
                return NULL;
            }

        case CN_BINARY_OP_SUBTRACTION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn_type_greatest_arithmetic_rank(left, right);
                }
                if (left->kind == CN_POINTER || right->kind == CN_POINTER) {
                    Cn_Type *ptr_type   = left->kind == CN_POINTER ? left  : right;
                    Cn_Type *other_type = left->kind == CN_POINTER ? right : left;
                    if (other_type->kind != CN_INTEGER) {
                        if (other_type->kind == CN_POINTER) {
                            return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_PTRDIFF);
                        }
                        cn__ast_illegal_binary(node, left, right, "pointer subtraction");
                        return NULL;
                    }
                    return ptr_type;
                }
                cn__ast_illegal_binary(node, left, right, "subtraction");
                return NULL;
            }

        case CN_BINARY_OP_ARRAY_SUB: 
            {
                if (left->kind != CN_POINTER) {
                    cn__ast_illegal_binary(node, left, right, "array subscript (left operand must be a pointer)");
                    return NULL;
                }
                if (right->kind != CN_INTEGER) {
                    cn__ast_illegal_binary(node, left, right, "array subscript (subscript must be an integer)");
                    return NULL;
                }

                return ((Cn_Type_Pointer *)left)->ptr_to;
            }

        // Arithmetic, usual arithmetic conversions.
        case CN_BINARY_OP_MULTIPLICATION:
        case CN_BINARY_OP_DIVISION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn__ast_usual_arithmetic_conversion(left, right);
                }
                cn__ast_illegal_binary(node, left, right,
                        node->binary.operator == CN_BINARY_OP_MULTIPLICATION
                        ? "multiplication" : "division");
                return NULL;
            }

        // Integer-only, usual arithmetic conversions.
        case CN_BINARY_OP_MODULO:
        case CN_BINARY_OP_BIT_AND:
        case CN_BINARY_OP_BIT_XOR:
        case CN_BINARY_OP_BIT_OR: 
            {
                if (left->kind == CN_INTEGER && right->kind == CN_INTEGER) {
                    return cn__ast_usual_arithmetic_conversion(left, right);
                }
                cn__ast_illegal_binary(node, left, right, "integer operation");
                return NULL;
            }

        // Shifts: each operand promoted independently; result is the promoted LEFT
        // operand. The right operand does NOT participate in the result type.
        case CN_BINARY_OP_LSHIFT:
        case CN_BINARY_OP_RSHIFT: 
            {
                if (left->kind == CN_INTEGER && right->kind == CN_INTEGER) {
                    return cn__ast_integer_promote(left);
                }
                cn__ast_illegal_binary(node, left, right, "shift");
                return NULL;
            }

        // Relational: arithmetic vs arithmetic, or pointer vs pointer. Result is int.
        case CN_BINARY_OP_LESS:
        case CN_BINARY_OP_LESS_EQ:
        case CN_BINARY_OP_GREATER:
        case CN_BINARY_OP_GREATER_EQ: 
            {
                bool ok = (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right))
                    || (left->kind == CN_POINTER && right->kind == CN_POINTER);
                if (!ok) {
                    cn__ast_illegal_binary(node, left, right, "relational comparison");
                    return NULL;
                }
                return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
            }

        // Equality: as relational, plus pointer vs integer (null constant). Result int.
        case CN_BINARY_OP_EQ:
        case CN_BINARY_OP_NOT_EQ: 
            {
                bool ok = (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right))
                    || (left->kind == CN_POINTER && right->kind == CN_POINTER)
                    || (left->kind == CN_POINTER && right->kind == CN_INTEGER)
                    || (left->kind == CN_INTEGER && right->kind == CN_POINTER);
                if (!ok) {
                    cn__ast_illegal_binary(node, left, right, "equality comparison");
                    return NULL;
                }
                return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
            }

        // Logical: scalar operands, no arithmetic conversions, result is int.
        case CN_BINARY_OP_AND:
        case CN_BINARY_OP_OR: {
            if (cn_type_is_scalar(left) && cn_type_is_scalar(right)) {
                return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
            }
            cn__ast_illegal_binary(node, left, right, "logical operation");
            return NULL;
        }

        // Comma: result is the type of the right operand.
        case CN_BINARY_OP_COMMA:
            return right;

        default: {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_TOKEN, "Expected binary operator.");
            return NULL;
        }
    }
}

CNDEF Cn_Type *cn__ast_access_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_ACCESS);

    Cn_Type *base = cn_ast_expression_typecheck(node->access.expression_idx);
    if (base == NULL) return NULL;

    // Removing qualifiers from base type here, won't need them.
    base = cn_type_unqualified(base);

    Cn_Type *struct_type;
    if (node->access.pointer) {
        // '->' : operand must be a pointer to a struct/union.
        if (base->kind != CN_POINTER) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "'->' requires a pointer operand.");
            return NULL;
        }
        struct_type = ((Cn_Type_Pointer *)base)->ptr_to;
    } else {
        // '.' : operand must be a struct/union directly.
        struct_type = base;
    }

    // Stripping qualifiers because, we won't need them in following checks.
    struct_type = cn_type_unqualified(struct_type);

    // TODO: Allow CN_UNION once union types are implemented.
    if (struct_type->kind != CN_STRUCT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Member access on non-struct type.");
        return NULL;
    }

    if (!(struct_type->flags & CN_TYPE_COMPLETE)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INCOMPLETE_TYPE, "Cannot access member of incomplete type.");
        return NULL;
    }

    Cn_Ast_Identifier *ident = cn_ast_get(node->access.member_idx);
    Cn_Type_Struct *ts = (Cn_Type_Struct *)struct_type;
    for (int64_t i = 0; i < ts->members_length; i++) {
        if (cn_str_equals(&ts->members[i].name, &ident->name)) {
            return ts->members[i].type;
        }
    }

    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_SYMBOL, "No member named '%.*s' in struct.", CN_UNPACK(ident->name));
    return NULL;
}

CNDEF Cn_Type *cn__ast_call_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_CALL);

    Cn_Type *callee = cn_ast_expression_typecheck(node->call.expression_idx);
    if (callee == NULL) return NULL;

    // If opaque, ignore type checking for this function, completely.
    if (callee->kind == CN_OPAQUE) return callee;

    // A function itself or a pointer to function may be called. Getting function type itself.
    Cn_Type *fn = callee;
    if (fn->kind == CN_POINTER) fn = ((Cn_Type_Pointer *)fn)->ptr_to;

    if (fn->kind != CN_FUNCTION) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Called object is not a function or function pointer.");
        return NULL;
    }

    Cn_Type_Function *func_type = (Cn_Type_Function *)fn;

    // Argument count must match parameter count.
    // NOTE: variadics aren't tracked in Cn_Type_Function yet, so this is exact-match only.
    int64_t arg_count = node->call.arguments.length;
    if (func_type->is_variadic) {
        if (arg_count < func_type->params_length) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Variadic function expects at least %lld argument(s) but %lld were provided.", func_type->params_length, arg_count);
            return NULL;
        }
    } else if (arg_count != func_type->params_length) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Function expects %lld argument(s) but %lld were provided.", func_type->params_length, arg_count);
        return NULL;
    }

    // Each argument must be assignable to its corresponding parameter type.
    for (int64_t i = 0; i < node->call.arguments.length; i++) {
        Cn_Ast_Idx arg_idx = node->call.arguments.idxs[i];

        Cn_Type *arg_type = cn_ast_expression_typecheck(arg_idx);
        if (arg_type == NULL) return NULL;


        // If parameter is not part of variadic list, typechecking with function param type, otherwise ignoring.
        if (!func_type->is_variadic || i < func_type->params_length) {
            Cn_Type *param_type = func_type->params[i].type;

            if (!cn_type_is_assignable(param_type, arg_type)) {
                // Additionally to checking type assignable checking edge case of NULL pointer assignment to pointer type, example int *a = 0
                if (!(param_type->kind == CN_POINTER && cn__ast_is_null_pointer_constant(arg_idx))) {
                        Cn_String a_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), arg_type);
                        Cn_String p_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), param_type);
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Argument %lld of type %.*s is not assignable to parameter type %.*s.", i + 1, CN_UNPACK(a_str), CN_UNPACK(p_str));
                        return NULL;
                }
            }
        }
    }

    return func_type->return_type;
}

CNDEF Cn_Type *cn__ast_unary_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_UNARY);

    Cn_Ast_Idx operand_idx = node->unary.expression_idx;
    Cn_Type *operand = cn_ast_expression_typecheck(operand_idx);
    if (operand == NULL) return NULL;

    switch (node->unary.operator) {
        // ++x / --x : modifiable lvalue of scalar type; result is the operand's type.
        case CN_UNARY_OP_INCREMENT:
        case CN_UNARY_OP_DECREMENT: 
            {
                if (!cn__ast_is_modifiable_lvalue(operand_idx)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of '++'/'--' is not a modifiable lvalue.");
                    return NULL;
                }
                if (!cn_type_is_scalar(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of '++'/'--' must be of scalar type.");
                    return NULL;
                }

                return operand;
            }

        // +x / -x : arithmetic operand; result is integer-promoted.
        case CN_UNARY_OP_POSITIVE:
        case CN_UNARY_OP_NEGATIVE: 
            {
                if (!cn_type_is_arithmetic(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of unary '+'/'-' must be of arithmetic type.");
                    return NULL;
                }
                return cn__ast_integer_promote(operand);
            }

        // ~x : integer operand; result is integer-promoted.
        case CN_UNARY_OP_BIT_NOT: 
            {
                if (operand->kind != CN_INTEGER) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of '~' must be of integer type.");
                    return NULL;
                }
                return cn__ast_integer_promote(operand);
            }

        // !x : scalar operand; result is always int.
        case CN_UNARY_OP_NOT: 
            {
                if (!cn_type_is_scalar(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of '!' must be of scalar type.");
                    return NULL;
                }
                return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
            }

        // *x : pointer operand; result is the pointed-to type (an lvalue).
        case CN_UNARY_OP_DEREF:
            {
                if (operand->kind != CN_POINTER) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot dereference a non-pointer type.");
                    return NULL;
                }

                Cn_Type *ptr_to = ((Cn_Type_Pointer *)operand)->ptr_to;
                if (ptr_to->kind == CN_VOID) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot dereference a void * type.");
                    return NULL;
                }

                return ptr_to;
            }

        // &x : operand must be an lvalue; result is pointer-to-operand-type.
        case CN_UNARY_OP_ADDROF: 
            {
                if (!cn__ast_is_lvalue(operand_idx)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot take the address of a non-lvalue.");
                    return NULL;
                }
                Cn_Type *void_t = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_VOID);
                Cn_Type ptr = cn_type_make_pointer(void_t);
                return cn__ast_add_type_if_not(&ptr);
            }

        default: 
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_TOKEN, "Expected unary operator.");
                return NULL;
            }
    }
}

CNDEF Cn_Type *cn__ast_cast_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_CAST);

    // Resolve the target type from the typename.
    Cn_Ast_Node *type_name = cn_ast_get(node->cast.type_name_idx);
    Cn_Ast_Node *spec_qual = cn_ast_get(type_name->type_name.specifier_qualifier_idx);
    Cn_Type *target = cn_ast_to_type(spec_qual->specifier_qualifier.qualifiers, spec_qual->specifier_qualifier.type_specifier_idx, type_name->type_name.abstract_declarator_idx);
    if (target == NULL) return NULL;

    Cn_Type *operand = cn_ast_expression_typecheck(node->cast.expression_idx);
    if (operand == NULL) return NULL;

    // Cast to void: always allowed, value is discarded.
    if (target->kind == CN_VOID) {
        return target;
    }

    // Target must be scalar.
    if (!cn_type_is_scalar(target)) {
        Cn_String t_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), target);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot cast to non-scalar type %.*s.", CN_UNPACK(t_str));
        return NULL;
    }

    // Operand must be scalar.
    if (!cn_type_is_scalar(operand)) {
        Cn_String o_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), operand);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot cast from non-scalar type %.*s.", CN_UNPACK(o_str));
        return NULL;
    }

    // Floating types can't be cast to or from pointers.
    if ((target->kind == CN_POINTER && operand->kind == CN_FLOAT) || (target->kind == CN_FLOAT && operand->kind == CN_POINTER)) {
        Cn_String t_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), target);
        Cn_String o_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), operand);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot cast between floating type and pointer %.*s to %.*s.", CN_UNPACK(o_str), CN_UNPACK(t_str));
        return NULL;
    }

    return target;
}

CNDEF Cn_Type *cn__ast_compound_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_COMPOUND);

    // Resolve the target type from the typename.
    Cn_Ast_Node *type_name = cn_ast_get(node->compound.type_name_idx);
    Cn_Ast_Node *spec_qual = cn_ast_get(type_name->type_name.specifier_qualifier_idx);
    Cn_Type *target = cn_ast_to_type(spec_qual->specifier_qualifier.qualifiers, spec_qual->specifier_qualifier.type_specifier_idx, type_name->type_name.abstract_declarator_idx);
    if (target == NULL) return NULL;

    if (!(cn_type_unqualified(target)->flags & CN_TYPE_COMPLETE)) {
        Cn_String t_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), target);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node->compound.type_name_idx, CN_DC_ILLEGAL_TYPE, "Cannot have incomplete target type %.*s in compound literal.", CN_UNPACK(t_str));
        return NULL;
    }
    
    if (!cn_ast_designations_typecheck(node->compound.designations, target)) return NULL;

    return target;
}

CNDEF Cn_Type *cn__ast_sizeof_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_SIZEOF);

    Cn_Ast_Node *child = cn_ast_get(node->sizeof_expression.target_idx);

    Cn_Type *operand;
    if (child->kind == CN_AST_TYPE_NAME) {
        // Case: sizeof typename.
        Cn_Ast_Specifier_Qualifier *spec_qual = cn_ast_get(child->type_name.specifier_qualifier_idx);
        operand = cn_ast_to_type(
            spec_qual->qualifiers,
            spec_qual->type_specifier_idx,
            child->type_name.abstract_declarator_idx);
    } else {
        // Case: sizeof expr, operand is not evaluated; only its type is needed.
        operand = cn_ast_expression_typecheck(node->sizeof_expression.target_idx);
    }

    if (operand == NULL) return NULL;

    if (operand->kind == CN_FUNCTION) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot take 'sizeof' of a function type.");
        return NULL;
    }

    if (operand->kind == CN_VOID) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot take 'sizeof' of a void type.");
        return NULL;
    }

    if (!(operand->flags & CN_TYPE_COMPLETE)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INCOMPLETE_TYPE, "Cannot take 'sizeof' of an incomplete type.");
        return NULL;
    }

    return cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_SIZE);
}

CNDEF Cn_Type *cn__ast_ternary_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_TERNARY);

    Cn_Type *cond = cn_ast_expression_typecheck(node->ternary.condition_idx);
    if (cond == NULL) return NULL;

    // Condition must be scalar.
    if (!cn_type_is_scalar(cond)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Condition of '?:' must be of scalar type.");
        return NULL;
    }

    Cn_Type *a = cn_ast_expression_typecheck(node->ternary.true_idx);
    if (a == NULL) return NULL;

    Cn_Type *b = cn_ast_expression_typecheck(node->ternary.false_idx);
    if (b == NULL) return NULL;

    // Both arithmetic.
    if (cn_type_is_arithmetic(a) && cn_type_is_arithmetic(b)) {
        return cn__ast_usual_arithmetic_conversion(a, b);
    }

    // Both void.
    if (a->kind == CN_VOID && b->kind == CN_VOID) {
        return a;
    }

    // Same struct type interned or tagged types share identity.
    if (a->kind == CN_STRUCT && b->kind == CN_STRUCT && a == b) {
        return a;
    }

    // Pointer combinations.
    {
        Cn_Ast_Idx ia = node->ternary.true_idx;
        Cn_Ast_Idx ib = node->ternary.false_idx;

        // pointer  ?:  null pointer constant  ->  the pointer type.
        if (a->kind == CN_POINTER && cn__ast_is_null_pointer_constant(ib)) return a;
        if (b->kind == CN_POINTER && cn__ast_is_null_pointer_constant(ia)) return b;

        if (a->kind == CN_POINTER && b->kind == CN_POINTER) {
            Cn_Type *a_ptr_to = ((Cn_Type_Pointer *)a)->ptr_to;
            Cn_Type *b_ptr_to = ((Cn_Type_Pointer *)b)->ptr_to;
            // If either side points to void, the result is pointer-to-void.
            if (a_ptr_to->kind == CN_VOID) return a;
            if (b_ptr_to->kind == CN_VOID) return b;
            // Compatible pointed-to types -> that pointer type.
            if (cn_type_is_compatible(a_ptr_to, b_ptr_to)) return a;
        }
    }

    // No common type int vs pointer or mismatched structs.
    {
        Cn_String a_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), a);
        Cn_String b_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), b);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Incompatible operand types in '?:' expression, %.*s and %.*s.", CN_UNPACK(a_str), CN_UNPACK(b_str));
    }
    return NULL;
}

CNDEF Cn_Type *cn__ast_assignment_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_ASSIGN);

    Cn_Type *left = cn_ast_expression_typecheck(node->assign.left_idx);
    if (left == NULL) return NULL;

    Cn_Type *right = cn_ast_expression_typecheck(node->assign.right_idx);
    if (right == NULL) return NULL;

    // Left must be a modifiable lvalue for every assignment form.
    if (!cn__ast_is_modifiable_lvalue(node->assign.left_idx)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Left operand of assignment is not a modifiable lvalue.");
        return NULL;
    }

    switch (node->assign.operator) {
        // Simple assignment: right must be assignable to left, with NULL constant to pointer case handled too.
        case CN_ASSIGNMENT_OP_ASSIGN: 
            {
                if (!cn_type_is_assignable(left, right)) {

                    if (!(left->kind == CN_POINTER && cn__ast_is_null_pointer_constant(node->assign.right_idx))) {
                        Cn_String left_str  = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), left);
                        Cn_String right_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), right);
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot assign %.*s to %.*s.", CN_UNPACK(right_str), CN_UNPACK(left_str));
                        return NULL;
                    }
                }
                return left;
            }

        // '+=' / '-=' : arithmetic both sides, OR pointer-left with integer-right.
        case CN_ASSIGNMENT_OP_PLUS:
        case CN_ASSIGNMENT_OP_MINUS: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) return left;
                if (left->kind == CN_POINTER && right->kind == CN_INTEGER)       return left;
                cn__ast_illegal_binary(node, left, right, node->assign.operator == CN_ASSIGNMENT_OP_PLUS ? "'+=' assignment" : "'-=' assignment");
                return NULL;
            }

        // '*=' / '/=' : arithmetic both sides.
        case CN_ASSIGNMENT_OP_MULTIPLY:
        case CN_ASSIGNMENT_OP_DIVIDE: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) return left;
                cn__ast_illegal_binary(node, left, right, "arithmetic assignment");
                return NULL;
            }

        // '%=' '<<=' '>>=' '&=' '^=' '|=' : integer both sides.
        case CN_ASSIGNMENT_OP_MODULO:
        case CN_ASSIGNMENT_OP_LSHIFT:
        case CN_ASSIGNMENT_OP_RSHIFT:
        case CN_ASSIGNMENT_OP_BIT_AND:
        case CN_ASSIGNMENT_OP_BIT_XOR:
        case CN_ASSIGNMENT_OP_BIT_OR: 
            {
                if (left->kind == CN_INTEGER && right->kind == CN_INTEGER) return left;
                cn__ast_illegal_binary(node, left, right, "integer assignment");
                return NULL;
            }

        default: 
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_TOKEN, "Expected assignment operator.");
                return NULL;
            }
    }
}

CNDEF Cn_Type *cn__ast_postfix_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_POSTFIX);

    Cn_Ast_Idx operand_idx = node->postfix.expression_idx;
    Cn_Type *operand = cn_ast_expression_typecheck(operand_idx);
    if (operand == NULL) return NULL;

    switch (node->postfix.operator) {
        // x++ / x-- : modifiable lvalue of scalar type; result is the operand's type.
        case CN_POSTFIX_OP_INCREMENT:
        case CN_POSTFIX_OP_DECREMENT: 
            {
                if (!cn__ast_is_modifiable_lvalue(operand_idx)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of postfix '++'/'--' is not a modifiable lvalue.");
                    return NULL;
                }
                if (!cn_type_is_scalar(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Operand of postfix '++'/'--' must be of scalar type.");
                    return NULL;
                }
                return operand;
            }

        default: 
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_TOKEN, "Expected postfix operator.");
                return NULL;
            }
    }
}

CNDEF Cn_Type *cn_ast_expression_typecheck(Cn_Ast_Idx expression_idx) {
    CN_ASSERT(expression_idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_get(expression_idx);

    Cn_Type *result = NULL;

    switch(node->kind) {
        case CN_AST_BINARY: 
            {   
                result = cn__ast_binary_expression_typecheck(node);
                node->binary.type = result;
                break;
            }
        case CN_AST_ACCESS:
            {
                result = cn__ast_access_expression_typecheck(node);
                node->access.type = result;
                break;
            }
        case CN_AST_CALL:
            {
                result = cn__ast_call_typecheck(node);
                node->call.type = result;
                break;
            }
        case CN_AST_UNARY:
            {
                result = cn__ast_unary_expression_typecheck(node);
                node->unary.type = result;
                break;
            }
        case CN_AST_CAST:
            {
                result = cn__ast_cast_expression_typecheck(node);
                node->cast.type = result;
                break;
            }
        case CN_AST_COMPOUND:
            {
                result = cn__ast_compound_expression_typecheck(node);
                node->cast.type = result;
                break;
            }
        case CN_AST_SIZEOF:
            {
                result = cn__ast_sizeof_expression_typecheck(node);
                node->sizeof_expression.type = result;
                break;
            }
        case CN_AST_TERNARY:
            {   
                result = cn__ast_ternary_expression_typecheck(node);
                node->ternary.type = result;
                break;
            }
        case CN_AST_ASSIGN:
            {
                result = cn__ast_assignment_expression_typecheck(node);
                node->assign.type = result;
                break;
            }
        case CN_AST_POSTFIX:
            {
                result = cn__ast_postfix_expression_typecheck(node);
                node->postfix.type = result;
                break;
            }
        case CN_AST_PRIMARY:
            {
                Cn_Ast_Node *literal = cn_ast_get(node->primary.literal_idx);
                switch(literal->kind) {
                    case CN_AST_IDENTIFIER:
                        {
                            Cn_Ast_Binding_Idx idx = cn_ast_binding_table_get(literal->identifier.name, &cn__ast_data->symbol_binding_table);

                            if (idx == CN_AST_NIL_BINDING_IDX) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(literal), CN_DC_INVALID_SYMBOL, "Primary expression identifier is not a known symbol.");
                                return NULL;
                            }

                            Cn_Ast_Binding *binding = cn__ast_data->binding_list + idx;
                            if (binding->kind == CN_BINDING_TAG || binding->kind == CN_BINDING_TYPEDEF) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(literal), CN_DC_INVALID_SYMBOL, "Primary expression identifier is expected to be a variable name, function name, or enum name.");
                                return NULL;
                            }

                            result = binding->type;
                            break;
                        }
                    case CN_AST_INTEGER:
                        {   
                            result = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
                            break;
                        }
                    case CN_AST_FLOAT:
                        {   
                            result = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_FLOAT);
                            break;
                        }
                    case CN_AST_STRING:
                        {   
                            result  = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_CHAR);
                            Cn_Type const_char = cn_type_make_qualified(CN_TYPE_QUALIFIED_CONSTANT, result);
                            result = cn__ast_add_type_if_not(&const_char);
                            Cn_Type const_char_ptr = cn_type_make_pointer(result);
                            result = cn__ast_add_type_if_not(&const_char_ptr);
                            break;
                        }
                    default:
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_EXPRESSION, "Expected primary expression to typecheck.");
                        break;
                }

                node->primary.type = result;
                break;
            }
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_EXPRESSION, "Expected expression to typecheck.");
            break;
    }
    
    // TEMPORARY: outputing typechecked result.
    // if (result != NULL) {
    //     Cn_String typename = CN_STR_BUFFER_EMPTY(128);
    //     cn_type_stringify(typename, result);
    //     cn_diagnostic_node(CN_DIAGNOSTIC_INFO, cn_ast_idx_get(node), CN_DC_ZERO, "%.*s", CN_UNPACK(typename));
    // }
    
    return result;
}

CNDEF Cn_Type *cn_ast_expression_get_type(Cn_Ast_Idx expression_idx) {
    CN_ASSERT(expression_idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_get(expression_idx);

    switch(node->kind) {
        case CN_AST_BINARY:
            return node->binary.type;
        case CN_AST_ACCESS:
            return node->access.type;
        case CN_AST_CALL:
            return node->call.type;
        case CN_AST_UNARY:
            return node->unary.type;
        case CN_AST_CAST:
            return node->cast.type;
        case CN_AST_COMPOUND:
            return node->compound.type;
        case CN_AST_SIZEOF:
            return node->sizeof_expression.type;
        case CN_AST_TERNARY:
            return node->ternary.type;
        case CN_AST_ASSIGN:
            return node->assign.type;
        case CN_AST_POSTFIX:
            return node->postfix.type;
        case CN_AST_PRIMARY:
            return node->primary.type;
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_EXPRESSION, "Expected expression to get it's resulting type.");
            return NULL;
    }
}
CNDEF bool cn_parse_int_literal(Cn_String str, uint64_t *out) {
    if (str.length <= 0) return false;

    int64_t i = 0;
    int base = 10;
    bool any_digit = false;

    if (str.length >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        base = 16;
        i = 2;
    } else if (str.data[0] == '0') {
        base = 8;
        i = 1;
        any_digit = true;   // The leading 0 is itself a valid digit.
    }

    uint64_t value = 0;
    for (; i < str.length; i++) {
        char c = str.data[i];
        int d;

        if (c >= '0' && c <= '9') 
            d = c - '0';
        else if (c >= 'a' && c <= 'f') 
            d = 10 + (c - 'a');
        else if (c >= 'A' && c <= 'F') 
            d = 10 + (c - 'A');
        else 
            break;

        if (d >= base) return false;    // For example if digits 8, 9 inside an octal literal.

        value = value * (uint64_t)base + (uint64_t)d;
        any_digit = true;
    }

    // Gather remaining integer suffix.
    for (; i < str.length; i++) {
        char c = str.data[i];
        if (c != 'u' && c != 'U' && c != 'l' && c != 'L') return false;
    }

    // If there were no digits encountered return false.
    if (!any_digit) return false;

    *out = value;
    return true;
}

CNDEF bool cn_parse_float_literal(Cn_String str, double *out) {
    if (str.length <= 0) return false;
 
    int64_t i = 0;
    double result = 0.0;
    bool any_digit = false;
 
    while (i < str.length && str.data[i] >= '0' && str.data[i] <= '9') {
        result = result * 10.0 + (str.data[i] - '0');
        any_digit = true;
        i++;
    }
 
    if (i < str.length && str.data[i] == '.') {
        i++;
        double scale = 0.1;
        while (i < str.length && str.data[i] >= '0' && str.data[i] <= '9') {
            result += (str.data[i] - '0') * scale;
            scale *= 0.1;
            any_digit = true;
            i++;
        }
    }
 
    // If there were no digits encountered return false.
    if (!any_digit) return false;
 
    if (i < str.length && (str.data[i] == 'e' || str.data[i] == 'E')) {
        i++;

        int exp_sign = 1;
        if (i < str.length && (str.data[i] == '+' || str.data[i] == '-')) {
            if (str.data[i] == '-') exp_sign = -1;
            i++;
        }

        bool any_exp_digit = false;
        int exp = 0;
        while (i < str.length && str.data[i] >= '0' && str.data[i] <= '9') {
            exp = exp * 10 + (str.data[i] - '0');
            any_exp_digit = true;
            i++;
        }
        
        // If there were no exp digits encountered return false.
        if (!any_exp_digit) return false;
 
        double factor = 1.0;
        for (int e = 0; e < exp; e++) 
            factor *= 10.0;

        if (exp_sign < 0) 
            result /= factor;
        else 
            result *= factor;
    }
 
    // Gather remaining float suffix.
    for (; i < str.length; i++) {
        char c = str.data[i];
        if (c != 'f' && c != 'F' && c != 'l' && c != 'L') return false;
    }
 
    *out = result;
    return true;
}

CNDEF Cn_Any cn__ast_binary_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = node->binary.type;
    if (rtype == NULL) return (Cn_Any) {0};
 
    Cn_Binary_Operator_Kind op = node->binary.operator;
    Cn_Ast_Idx left_idx  = node->binary.left_idx;
    Cn_Ast_Idx right_idx = node->binary.right_idx;
 
    Cn_Any result = { 
        .type = rtype, 
        .data = buffer 
    };
 
    uint8_t lbuf[CN_TYPE_SCALAR_MAX_SIZE];
    uint8_t rbuf[CN_TYPE_SCALAR_MAX_SIZE];
 
    switch (op) {
        // a , b  ->  value of b. Side effects of a are irrelevant to the value.
        case CN_BINARY_OP_COMMA:
            return cn_ast_expression_evaluate(right_idx, buffer);
 
        // This never resolve to a compile time scalar.
        case CN_BINARY_OP_ARRAY_SUB:
            return (Cn_Any) {0};
 
        // Short circuit logical ops: foldable even if one side is unknown, as
        // long as the known side already determines the outcome.
        case CN_BINARY_OP_AND: 
            {
                Cn_Any l = cn_ast_expression_evaluate(left_idx, lbuf);
                bool lv = !cn_any_is_zero(l);
                if (!cn_any_is_empty(l) && lv) { 
                    cn_any_write_int(result, 0); 
                    return result; 
                }

                Cn_Any r = cn_ast_expression_evaluate(right_idx, rbuf);
                bool rv = !cn_any_is_zero(r);
                if (!cn_any_is_empty(l) && !cn_any_is_empty(r)) {
                    cn_any_write_int(result, (lv && rv) ? 1 : 0);
                    return result;
                }

                if (!cn_any_is_empty(r) && rv) { 
                    cn_any_write_int(result, 0); 
                    return result; 
                }

                return (Cn_Any) {0};
            }
        case CN_BINARY_OP_OR: 
            {
                Cn_Any l = cn_ast_expression_evaluate(left_idx, lbuf);
                bool lv = !cn_any_is_zero(l);
                if (!cn_any_is_empty(l) && lv) { 
                    cn_any_write_int(result, 1); 
                    return result; 
                }

                Cn_Any r = cn_ast_expression_evaluate(right_idx, rbuf);
                bool rv = !cn_any_is_zero(r);
                if (!cn_any_is_empty(l) && !cn_any_is_empty(r)) {
                    cn_any_write_int(result, (lv || rv) ? 1 : 0);
                    return result;
                }

                if (!cn_any_is_empty(r) && rv) { 
                    cn_any_write_int(result, 1); 
                    return result; 
                }

                return (Cn_Any) {0};
            }
 
        default:
            break;
    }
 
    // Everything below needs both operands.
    Cn_Any l = cn_ast_expression_evaluate(left_idx, lbuf);
    if (cn_any_is_empty(l)) return (Cn_Any) {0};

    Cn_Any r = cn_ast_expression_evaluate(right_idx, rbuf);
    if (cn_any_is_empty(r)) return (Cn_Any) {0};
 
    Cn_Type *lt = cn_type_unqualified(l.type);
    Cn_Type *rt = cn_type_unqualified(r.type);
 
    // Pointer arithmetic or pointer differences have no known address.
    if (lt->kind == CN_POINTER || rt->kind == CN_POINTER) return (Cn_Any) {0};
 
    bool float_operands = (lt->kind == CN_FLOAT) || (rt->kind == CN_FLOAT);
 
    // Relational and equality operators -> int.
    switch (op) {
        case CN_BINARY_OP_LESS:
        case CN_BINARY_OP_LESS_EQ:
        case CN_BINARY_OP_GREATER:
        case CN_BINARY_OP_GREATER_EQ:
        case CN_BINARY_OP_EQ:
        case CN_BINARY_OP_NOT_EQ: 
            {
                bool res = false;
                if (float_operands) {
                    double a = cn_any_as_double(l), b = cn_any_as_double(r);
                    switch (op) {
                        case CN_BINARY_OP_LESS:       res = a <  b; break;
                        case CN_BINARY_OP_LESS_EQ:    res = a <= b; break;
                        case CN_BINARY_OP_GREATER:    res = a >  b; break;
                        case CN_BINARY_OP_GREATER_EQ: res = a >= b; break;
                        case CN_BINARY_OP_EQ:         res = a == b; break;
                        default:                      res = a != b; break;
                    }
                } else {
                    int64_t a = cn_any_read_int(l), b = cn_any_read_int(r);

                    Cn_Type *common = cn__ast_usual_arithmetic_conversion(lt, rt);
                    // Case: unsigned.
                    if ((common->kind == CN_INTEGER) && !((Cn_Type_Integer *)common)->is_signed) {
                        uint64_t ua = (uint64_t)a, ub = (uint64_t)b;
                        switch (op) {
                            case CN_BINARY_OP_LESS:       res = ua <  ub; break;
                            case CN_BINARY_OP_LESS_EQ:    res = ua <= ub; break;
                            case CN_BINARY_OP_GREATER:    res = ua >  ub; break;
                            case CN_BINARY_OP_GREATER_EQ: res = ua >= ub; break;
                            case CN_BINARY_OP_EQ:         res = ua == ub; break;
                            default:                      res = ua != ub; break;
                        }
                    } 
                    // Case: signed.
                    else {
                        switch (op) {
                            case CN_BINARY_OP_LESS:       res = a <  b; break;
                            case CN_BINARY_OP_LESS_EQ:    res = a <= b; break;
                            case CN_BINARY_OP_GREATER:    res = a >  b; break;
                            case CN_BINARY_OP_GREATER_EQ: res = a >= b; break;
                            case CN_BINARY_OP_EQ:         res = a == b; break;
                            default:                      res = a != b; break;
                        }
                    }
                }
                cn_any_write_int(result, res ? 1 : 0);
                return result;
            }
        default:
            break;
    }
 
    // Strip qualifiers on return type.
    Cn_Type *urtype = cn_type_unqualified(rtype);
 
    // Floating arithmetic.
    if (urtype->kind == CN_FLOAT) {
        double a = cn_any_as_double(l), b = cn_any_as_double(r);
        double res = 0.0;
        switch (op) {
            case CN_BINARY_OP_ADDITION:       res = a + b; break;
            case CN_BINARY_OP_SUBTRACTION:    res = a - b; break;
            case CN_BINARY_OP_MULTIPLICATION: res = a * b; break;
            case CN_BINARY_OP_DIVISION:       res = a / b; break; // Inf or NaN are valid doubles.
            default: return (Cn_Any) {0};
        }
        cn_any_write_float(result, res);
        return result;
    }
 
    // Integer arithmetic or bitwise. Computed in 64 bits; the store truncates to
    // the result width. Unsigned casts keep the evaluator from undefined behavior.
    if (urtype->kind == CN_INTEGER) {
        bool is_signed = ((Cn_Type_Integer *)urtype)->is_signed;
        int64_t a = cn_any_read_int(l), b = cn_any_read_int(r);
        int64_t res = 0;
        switch (op) {
            case CN_BINARY_OP_ADDITION:       
                res = (int64_t)((uint64_t)a + (uint64_t)b); 
                break;

            case CN_BINARY_OP_SUBTRACTION:    
                res = (int64_t)((uint64_t)a - (uint64_t)b); 
                break;

            case CN_BINARY_OP_MULTIPLICATION: 
                res = (int64_t)((uint64_t)a * (uint64_t)b); 
                break;
 
            case CN_BINARY_OP_DIVISION:
                {
                    if (b == 0) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_CONSTANT_EXPRESSION, "Integer division '/' by 0.");
                        return (Cn_Any) {0};
                    }
                    if (is_signed) res = (a == INT64_MIN && b == -1) ? INT64_MIN : (a / b);
                    else           res = (int64_t)((uint64_t)a / (uint64_t)b);
                    break;
                }
 
            case CN_BINARY_OP_MODULO:
                {
                    if (b == 0) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_CONSTANT_EXPRESSION, "Integer modulo '\%' by 0.");
                        return (Cn_Any) {0};
                    }
                    if (is_signed) res = (a == INT64_MIN && b == -1) ? 0 : (a % b);
                    else           res = (int64_t)((uint64_t)a % (uint64_t)b);
                    break;
                }
 
            case CN_BINARY_OP_BIT_AND: 
                res = a & b; 
                break;

            case CN_BINARY_OP_BIT_OR:  
                res = a | b; 
                break;

            case CN_BINARY_OP_BIT_XOR: 
                res = a ^ b; 
                break;
 
            case CN_BINARY_OP_LSHIFT:
            case CN_BINARY_OP_RSHIFT: 
                {
                    int64_t width = urtype->size * 8;

                    if (b < 0 || b >= width) return (Cn_Any) {0}; // Undefined behavior shift, not evaluatable.

                    if (op == CN_BINARY_OP_LSHIFT)
                        res = (int64_t)((uint64_t)a << (uint64_t)b);
                    else
                        res = is_signed ? (a >> b) : (int64_t)((uint64_t)a >> (uint64_t)b);
                    break;
                }
 
            default:
                return (Cn_Any) {0};
        }

        cn_any_write_int(result, res);
        return result;
    }
 
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_access_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_call_evaluate(Cn_Ast_Node *node, void *buffer) {
    // Cannot really evaluate runtime function during compile time.
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_unary_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Unary_Operator_Kind op = node->unary.operator;
    Cn_Ast_Idx operand_idx  = node->unary.expression_idx;
 
    // Not constant operations.
    if (op == CN_UNARY_OP_ADDROF || op == CN_UNARY_OP_DEREF || op == CN_UNARY_OP_INCREMENT || op == CN_UNARY_OP_DECREMENT) return (Cn_Any) {0};

    // Special case, positive operator doesn't really do anything, 
    // so just delegating buffer and evaluation to operand expression.
    if (op == CN_UNARY_OP_POSITIVE) {
        return cn_ast_expression_evaluate(operand_idx, buffer);
    }

    Cn_Type *rtype = node->unary.type;
    if (rtype == NULL) return (Cn_Any) {0};
 
    Cn_Any result = { 
        .type = rtype, 
        .data = buffer 
    };

    uint8_t operand_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any operand_value = cn_ast_expression_evaluate(operand_idx, operand_buffer);

    switch (op) {

        case CN_UNARY_OP_NEGATIVE: 
            {
                if (cn_type_unqualified(rtype)->kind == CN_FLOAT) 
                    cn_any_write_float(result, -cn_any_read_float(operand_value));
                else
                    cn_any_write_int(result, -cn_any_read_int(operand_value));

                return result;
            }

        case CN_UNARY_OP_BIT_NOT: 
            return cn_any_write_int(result, ~cn_any_read_int(operand_value));

        case CN_UNARY_OP_NOT: 
            return cn_any_write_int(result, cn_any_is_zero(operand_value) ? 0 : 1);
        default:
            return (Cn_Any) {0};
    }
}

CNDEF Cn_Any cn__ast_compound_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *target = node->cast.type;
    if (target == NULL) return (Cn_Any) {0};
 
    Cn_Type *ut = cn_type_unqualified(target);

    switch (ut->kind) {
        case CN_INTEGER:
        case CN_FLOAT:
        case CN_BOOL:
        case CN_POINTER:
            CN_ASSERT(node->compound.designations.length == 1);
            Cn_Ast_Designation *designation = cn_ast_get(node->compound.designations.idxs[0]);
            Cn_Ast_Initializer *initializer = cn_ast_get(designation->initializer_idx);
            CN_ASSERT(node->initializer.expression_idx != CN_AST_NIL_IDX);
            return cn_ast_expression_evaluate(initializer->expression_idx, buffer);
        
        case CN_ENUM:
            CN_TODO("Enum compound expression evaluation.");
            return (Cn_Any) {0};

        case CN_QUALIFIED:
        case CN_OPAQUE:
        case CN_UNKNOWN:
        case CN_FUNCTION:
        case CN_VOID:
            return (Cn_Any) {0};

        case CN_STRUCT:
        case CN_UNION:
        case CN_ARRAY:
            // Compound expression not evaluated, as of right now.
            // Will be implemented in the future.
            CN_TODO("Struct, union, array compound expression evaluation is incomplete.");
            return (Cn_Any) {0};
    }

    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_cast_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *target = node->cast.type;
    if (target == NULL) return (Cn_Any) {0};
 
    Cn_Type *ut = cn_type_unqualified(target);

    // Cast to void yields no value; cast to pointer has no known address.
    if (ut->kind == CN_VOID || ut->kind == CN_POINTER) return (Cn_Any) {0};
 
    uint8_t operand_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any operand_value = cn_ast_expression_evaluate(node->cast.expression_idx, operand_buffer);
    if (operand_value.type == NULL) return (Cn_Any) {0};
 
    return cn_any_convert(operand_value, target, buffer);
}

CNDEF Cn_Any cn__ast_sizeof_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = node->sizeof_expression.type; // size_t
    if (rtype == NULL) return (Cn_Any) {0};

    Cn_Ast_Node *child = cn_ast_get(node->sizeof_expression.target_idx);

    Cn_Type *operand_type;
    if (child->kind == CN_AST_TYPE_NAME) {
        Cn_Ast_Node *sq = cn_ast_get(child->type_name.specifier_qualifier_idx);
        operand_type = cn_ast_to_type(sq->specifier_qualifier.qualifiers, sq->specifier_qualifier.type_specifier_idx, child->type_name.abstract_declarator_idx);
    } else {
        // The operand of sizeof is unevaluated, only its type is needed.
        operand_type = cn_ast_expression_get_type(node->sizeof_expression.target_idx);
    }

    if (operand_type == NULL) return (Cn_Any) {0};

    operand_type = cn_type_unqualified(operand_type);

    if (!(operand_type->flags & CN_TYPE_COMPLETE) || operand_type->size < 0) return (Cn_Any) {0};      

    Cn_Any result = { 
        .type = rtype, 
        .data = buffer 
    };

    cn_any_write_int(result, operand_type->size);
    return result;
}

CNDEF Cn_Any cn__ast_ternary_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = node->ternary.type;
    if (rtype == NULL) return (Cn_Any) {0};

    Cn_Type *urtype = cn_type_unqualified(rtype);

    // Only scalar conditionals evaluated.
    if (!cn_type_is_scalar(urtype) || urtype->kind == CN_POINTER) return (Cn_Any) {0};

    uint8_t cond_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any cond = cn_ast_expression_evaluate(node->ternary.condition_idx, cond_buffer);
    if (cond.type == NULL) return (Cn_Any) {0};

    Cn_Ast_Idx chosen = cn_any_is_zero(cond) ? node->ternary.false_idx : node->ternary.true_idx;

    uint8_t value_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any value = cn_ast_expression_evaluate(chosen, value_buffer);
    if (value.type == NULL) return (Cn_Any) {0};

    // Convert the chosen branch to the conditional's common result type.
    return cn_any_convert(value, rtype, buffer);
}

CNDEF Cn_Any cn__ast_assignment_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_postfix_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_primary_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Any any = {0};
    any.data = buffer;
    any.type = node->primary.type;

    Cn_Ast_Node *literal = cn_ast_get(node->primary.literal_idx);
    switch (literal->kind) {
        case CN_AST_INTEGER:
            {
                uint64_t value;
                if (!cn_parse_int_literal(cn_source_to_str( &node->src), &value)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid integer literal in primary expression.");
                    return (Cn_Any) {0};
                }
                any = cn_any_write_int(any, (int64_t)value);
                break;
            }
        case CN_AST_FLOAT:
            {
                double value;
                if (!cn_parse_float_literal(cn_source_to_str(&node->src), &value)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid float literal in primary expression.");
                    return (Cn_Any) {0};
                }
                any = cn_any_write_float(any, value);
                break;
            }
        case CN_AST_STRING:
            {
                return (Cn_Any) {0};
            }
        case CN_AST_IDENTIFIER:
            {   
                return (Cn_Any) {0};
            }
        default:
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_TOKEN, "Expected identifier, float, integer, or string literal in primary expression.");
                return (Cn_Any) {0};
            }
    }

    return any;
}

CNDEF Cn_Any cn_ast_expression_evaluate(Cn_Ast_Idx expression_idx, void *buffer) {
    CN_ASSERT(expression_idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_get(expression_idx);

    switch(node->kind) {
        case CN_AST_BINARY: 
            {   
                return cn__ast_binary_expression_evaluate(node, buffer);
            }
        case CN_AST_ACCESS:
            {
                return cn__ast_access_expression_evaluate(node, buffer);
            }
        case CN_AST_CALL:
            {
                return cn__ast_call_evaluate(node, buffer);
            }
        case CN_AST_UNARY:
            {
                return cn__ast_unary_expression_evaluate(node, buffer);
            }
        case CN_AST_CAST:
            {
                return cn__ast_cast_expression_evaluate(node, buffer);
            }
        case CN_AST_COMPOUND:
            {
                return cn__ast_compound_expression_evaluate(node, buffer);
            }
        case CN_AST_SIZEOF:
            {
                return cn__ast_sizeof_expression_evaluate(node, buffer);
            }
        case CN_AST_TERNARY:
            {   
                return cn__ast_ternary_expression_evaluate(node, buffer);
            }
        case CN_AST_ASSIGN:
            {
                return cn__ast_assignment_expression_evaluate(node, buffer);
            }
        case CN_AST_POSTFIX:
            {
                return cn__ast_postfix_expression_evaluate(node, buffer);
            }
        case CN_AST_PRIMARY:
            {
                return cn__ast_primary_expression_evaluate(node, buffer);
            }
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_EXPRESSION, "Expected expression to evaluate.");
            break;
    }
    
    return (Cn_Any) {0};
}

CNDEF void cn_ast_expression_clear_types(Cn_Ast_Idx expression_idx) {
    CN_ASSERT(expression_idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_get(expression_idx);

    switch(node->kind) {
        case CN_AST_BINARY:
            node->binary.type = NULL;
            cn_ast_expression_clear_types(node->binary.left_idx);
            cn_ast_expression_clear_types(node->binary.right_idx);
            break;
        case CN_AST_ACCESS:
            node->access.type = NULL;
            cn_ast_expression_clear_types(node->access.expression_idx);
            break;
        case CN_AST_CALL:
            node->call.type = NULL;
            cn_ast_expression_clear_types(node->call.expression_idx);

            for (int64_t i = 0; i < node->call.arguments.length; i++) {
                cn_ast_expression_clear_types(node->call.arguments.idxs[i]);
            }
            break;
        case CN_AST_UNARY:
            node->unary.type = NULL;
            cn_ast_expression_clear_types(node->unary.expression_idx);
            break;
        case CN_AST_CAST:
            node->cast.type = NULL;
            cn_ast_expression_clear_types(node->cast.expression_idx);
            break;
        case CN_AST_SIZEOF:
            node->sizeof_expression.type = NULL;
    
            Cn_Ast_Node *t = cn_ast_get(node->sizeof_expression.target_idx);
            if (t->kind != CN_AST_TYPE_NAME) {
                cn_ast_expression_clear_types(node->sizeof_expression.target_idx);
            }
            break;
        case CN_AST_TERNARY:
            node->ternary.type = NULL;
            cn_ast_expression_clear_types(node->ternary.condition_idx);
            cn_ast_expression_clear_types(node->ternary.false_idx);
            cn_ast_expression_clear_types(node->ternary.true_idx);
            break;
        case CN_AST_ASSIGN:
            node->assign.type = NULL;
            cn_ast_expression_clear_types(node->assign.left_idx);
            cn_ast_expression_clear_types(node->assign.right_idx);
            break;
        case CN_AST_POSTFIX:
            node->postfix.type = NULL;
            cn_ast_expression_clear_types(node->postfix.expression_idx);
            break;
        case CN_AST_PRIMARY:
            node->primary.type = NULL;
            break;
        default: 
            break;
    }
}

/**
 * Helper function that typecheck designator, it returns type that designator access.
 *
 * RETURNS: NULL if error occured.
 */
CNDEF Cn_Type *cn__ast_designator_typecheck(Cn_Ast_Idx designator_idx, Cn_Type *type, int64_t *ordinal) {
    Cn_Ast_Designator *designator = cn_ast_get(designator_idx);
    
    type = cn_type_unqualified(type);
    // Shouldn't really happen, but still worth to check, if user decides to call this function.
    if (!(type->flags & CN_TYPE_COMPLETE)) {
        Cn_String type_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type);
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "Designator on incomplete type %.*s.", CN_UNPACK(type_str));
        return NULL;
    }

    switch (type->kind) {
        case CN_ARRAY: 
            {
                if (designator->identifier_idx != CN_AST_NIL_IDX) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "Identifier designator used on array type.");
                    return NULL;
                }

                
                Cn_Type *expression_type = cn_ast_expression_typecheck(designator->expression_idx);
                if (expression_type == NULL) return NULL;
                
                uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
                Cn_Any any = cn_ast_expression_evaluate(designator->expression_idx, buffer);

                if (cn_any_is_empty(any)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in array designator.");
                    return NULL;
                }

                if (any.type->kind != CN_INTEGER) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array designator.");
                    return NULL;
                }

                int64_t index = cn_any_read_int(any);

                if (index >= type->array.length) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Out of bound access from constant expression in array designator, at index %ld, for array length %ld.", index, type->array.length);
                    return NULL;
                }
                
                if (ordinal != NULL) *ordinal = index;
                
                if (designator->expression_range_end_idx != CN_AST_NIL_IDX) {
                    expression_type = cn_ast_expression_typecheck(designator->expression_range_end_idx);
                    if (expression_type == NULL) return NULL;

                    any = cn_ast_expression_evaluate(designator->expression_range_end_idx, buffer);

                    if (cn_any_is_empty(any)) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in array designator.");
                        return NULL;
                    }

                    if (any.type->kind != CN_INTEGER) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array designator.");
                        return NULL;
                    }

                    int64_t end_index = cn_any_read_int(any);

                    if (end_index >= type->array.length) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Out of bound access from constant expression in array designator, at index %ld, for array length %ld.", end_index, type->array.length);
                        return NULL;
                    }

                    if (end_index < index) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid range in array designator, end index %ld is less then start index %ld.", end_index, index);
                        return NULL;
                    }

                    if (ordinal != NULL) *ordinal = end_index;
                }
                
                return type->array.element_type;
            }
        case CN_STRUCT: 
            {
                if (designator->expression_idx != CN_AST_NIL_IDX) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "Array designator used on struct type.");
                    return NULL;
                }

                Cn_Ast_Identifier *identifier = cn_ast_get(designator->identifier_idx);
                for (int64_t i = 0; i < type->struct_t.members_length; i++) {
                    if (cn_str_equals(&identifier->name, &type->struct_t.members[i].name)) {
                        if (ordinal != NULL) *ordinal = i;
                        return type->struct_t.members[i].type;
                    }
                }

                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "No member %.*s in struct specified by identifier designator.", CN_UNPACK(identifier->name));
                return NULL;
            }
        case CN_UNION: 
            {
                if (designator->expression_idx != CN_AST_NIL_IDX) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "Array designator used on union type.");
                    return NULL;
                }

                Cn_Ast_Identifier *identifier = cn_ast_get(designator->identifier_idx);
                for (int64_t i = 0; i < type->union_t.members_length; i++) {
                    if (cn_str_equals(&identifier->name, &type->union_t.members[i].name)) {
                        if (ordinal != NULL) *ordinal = i;
                        return type->union_t.members[i].type;
                    }
                }

                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "No member %.*s in union specified by identifier designator.", CN_UNPACK(identifier->name));
                return NULL;
            }
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator_idx, CN_DC_ILLEGAL_TYPE, "Designator used on non-compound type.");
            return NULL;
    }
}

CNDEF bool cn__ast_designation_typecheck(Cn_Ast_Idx designation_idx, Cn_Type *type, int64_t *ordinal) {
    Cn_Ast_Designation *designation = cn_ast_get(designation_idx);

    if (designation->designators.length != 0) {
        type = cn__ast_designator_typecheck(designation->designators.idxs[0], type, ordinal);

        for (int64_t i = 1; i < designation->designators.length; i++) {
            type = cn__ast_designator_typecheck(designation->designators.idxs[i], type, NULL);
        }

    } else {
        type = cn_type_unqualified(type);

        // Shouldn't really happen, but still worth to check, if user decides to call this function.
        if (!(type->flags & CN_TYPE_COMPLETE)) {
            Cn_String type_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type);
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designation_idx, CN_DC_ILLEGAL_TYPE, "Designation on incomplete type %.*s.", CN_UNPACK(type_str));
            return false;
        }
        
        switch (type->kind) {
            case CN_ARRAY: 
                type = type->array.element_type;
                break;
            case CN_STRUCT:
                if (*ordinal >= type->struct_t.members_length) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designation_idx, CN_DC_ILLEGAL_TYPE, "Out of bounds designation access on struct.");
                    return false;
                } 
                type = type->struct_t.members[*ordinal].type;
                break;

            case CN_UNION: 
                if (*ordinal >= type->union_t.members_length) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designation_idx, CN_DC_ILLEGAL_TYPE, "Out of bounds designation access on union.");
                    return false;
                } 
                type = type->union_t.members[*ordinal].type;
                break;
            default: // Default case is just other complete types.
                break;
        }
    }

    return cn_ast_initializer_typecheck(designation->initializer_idx, type);
}

CNDEF bool cn_ast_designations_typecheck(Cn_Ast_List designations, Cn_Type *type) {
    int64_t ordinal = 0;
    for (int64_t i = 0; i < designations.length; i++) {
        if (!cn__ast_designation_typecheck(designations.idxs[i], type, &ordinal)) return false;
        ordinal++;
    }

    return true;
}

CNDEF bool cn_ast_initializer_typecheck(Cn_Ast_Idx initializer_idx, Cn_Type *type) {
    Cn_Ast_Initializer *initializer = cn_ast_get(initializer_idx);

    type = cn_type_unqualified(type);

    if (initializer->expression_idx != CN_AST_NIL_IDX) {
        Cn_Type *expression_type = cn_ast_expression_typecheck(initializer->expression_idx);

        // Simple assignment check, same as in assignment expression typechecking.
        if (!cn_type_is_assignable(type, expression_type)) {

            if (!(type->kind == CN_POINTER && cn__ast_is_null_pointer_constant(initializer->expression_idx))) {
                Cn_String left_str  = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type);
                Cn_String right_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), expression_type);
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, initializer->expression_idx, CN_DC_ILLEGAL_TYPE, "Cannot assign %.*s to %.*s in initializer.", CN_UNPACK(right_str), CN_UNPACK(left_str));
                return false;
            }
        }

        return true;
    }
        
    // Compound initializer case:
    return cn_ast_designations_typecheck(initializer->designations, type);
}

#define cn_ast_is_nil(idx) ((idx) == CN_AST_NIL_IDX)
#define cn_ast_is(idx, k) (((Cn_Ast_Node *)cn_ast_get(idx))->kind == (k))

CNDEF bool cn_ast_expect(Cn_Ast_Idx idx, Cn_Ast_Kind kind) {
    if (!cn_ast_is(idx, kind)) { 
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, idx, CN_DC_EXPECTED_AST_NODE, "Expected %s ast node, but received %s node.", cn_ast_node_kind_name(kind), cn_ast_node_kind_name(((Cn_Ast_Node *)cn_ast_get(idx))->kind)); 
        return false;
    }
    return true;
}

CNDEF bool cn_ast_reparse_translation_unit(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_TRANSLATION_UNIT)) return false;

    Cn_Ast_Translation_Unit *node = cn_ast_get(node_idx);

    for (int64_t i = 0; i < node->external_declarations.length; i++) {
        if (!cn_ast_reparse_external_declaration(node->external_declarations.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_external_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_EXTERNAL_DECLARATION)) return false;

    Cn_Ast_External_Declaration *node = cn_ast_get(node_idx);

    // Stray ';' case.
    if (cn_ast_is_nil(node->child_idx)) return true;

    Cn_Ast_Node *child = cn_ast_get(node->child_idx);
    switch (child->kind) {
        case CN_AST_FUNCTION:
            if (!cn_ast_reparse_function_definition(node->child_idx)) return false;
            break;
        default:
            if (!cn_ast_reparse_declaration(node->child_idx)) return false;
            break;
    }

    return true;
}

CNDEF bool cn_ast_reparse_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_DECLARATION)) return false;

    Cn_Ast_Declaration *node = cn_ast_get(node_idx);

    if (!cn_ast_reparse_declaration_specifiers(node->declaration_specifiers_idx)) return false;

    for (int64_t i = 0; i < node->init_declarators.length; i++) {
        if (!cn_ast_reparse_init_declarator(node->init_declarators.idxs[i])) return false;
    }

    for (int64_t i = 0; i < node->gnu_attribute_specifiers.length; i++) {
        if (!cn_ast_reparse_gnu_attribute_specifier(node->gnu_attribute_specifiers.idxs[i])) return false;
    }

    // Rebuild types and bindings, exactly like the parse path does.
    if (node->init_declarators.length > 0) {
        if (!cn_ast_analyze_declaration(node->declaration_specifiers_idx, node->init_declarators)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_function_definition(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_FUNCTION)) return false;

    Cn_Ast_Function *node = cn_ast_get(node_idx);

    if (!cn_ast_reparse_declaration_specifiers(node->declaration_specifiers_idx)) return false;

    if (!cn_ast_reparse_declarator(node->declarator_idx)) return false;

    // Function definition type and bindings resolution.
    Cn_Ast_Binding_Idx function_binding_idx;

    bool ok = cn_ast_analyze_function(
            node->declaration_specifiers_idx,
            node->declarator_idx,
            &function_binding_idx
            );

    if (!ok) return false;

    // Push function scope for parameters and body.
    cn_ast_scope_stack_push();

    // Bind function parameters.
    ok = cn_ast_bind_function_params(function_binding_idx);
    if (!ok) return false;

    // Analyze compound statement contents using current scope, parameters already bound.
    if (!cn_ast_reparse_block(node->block_idx, true)) return false;

    cn_ast_scope_stack_pop();

    return true;
}

CNDEF bool cn_ast_reparse_asm_definition(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_asm_definition");
}

CNDEF bool cn_ast_reparse_statement(Cn_Ast_Idx node_idx) {
    if (cn_ast_is_nil(node_idx)) return true;

    Cn_Ast_Node *node = cn_ast_get(node_idx);

    switch (node->kind) {
        case CN_AST_BLOCK:
            return cn_ast_reparse_block(node_idx, false);
        case CN_AST_IF:
            return cn_ast_reparse_if(node_idx);
        case CN_AST_SWITCH:
            return cn_ast_reparse_switch(node_idx);
        // TODO: case CN_AST_WHILE:
        // TODO: case CN_AST_DO_WHILE:
        // TODO: case CN_AST_FOR:
        //     return cn_ast_reparse_iteration_statement(node_idx);
        case CN_AST_GOTO:
            return cn_ast_reparse_goto(node_idx);
        case CN_AST_RETURN:
            return cn_ast_reparse_return(node_idx);
        case CN_AST_BREAK:
        case CN_AST_CONTINUE:
            return true; // No children to reparse.
        // TODO: case CN_AST_LABEL:
        //     return cn_ast_reparse_labeled_statement(node_idx);
        case CN_AST_EXPRESSION_STATEMENT:
            return cn_ast_reparse_expression_statement(node_idx);
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node_idx, CN_DC_EXPECTED_AST_NODE, "Unexpected statement kind in reparse.");
            return false;
    }
}

CNDEF bool cn_ast_reparse_block(Cn_Ast_Idx node_idx, bool use_current_scope) {
    if (!cn_ast_expect(node_idx, CN_AST_BLOCK)) return false;

    Cn_Ast_Block *node = cn_ast_get(node_idx);

    if (!use_current_scope) cn_ast_scope_stack_push();

    for (int64_t i = 0; i < node->statements.length; i++) {
        Cn_Ast_Idx stmt_idx = node->statements.idxs[i];
        if (cn_ast_is(stmt_idx, CN_AST_DECLARATION)) {
            if (!cn_ast_reparse_declaration(stmt_idx)) {
                if (!use_current_scope) cn_ast_scope_stack_pop();
                return false;
            }
        } else {
            if (!cn_ast_reparse_statement(stmt_idx)) {
                if (!use_current_scope) cn_ast_scope_stack_pop();
                return false;
            }
        }
    }

    if (!use_current_scope) cn_ast_scope_stack_pop();

    return true;
}

CNDEF bool cn_ast_reparse_if(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_IF)) return false;

    Cn_Ast_If *node = cn_ast_get(node_idx);

    // Type check condition expression.
    if (cn_ast_expression_typecheck(node->condition_idx) == NULL) return false;

    // Analyze then statement.
    if (!cn_ast_reparse_statement(node->then_idx)) return false;

    // Analyze else statement if present.
    if (!cn_ast_is_nil(node->else_idx)) {
        if (!cn_ast_reparse_statement(node->else_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_switch(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_SWITCH)) return false;

    Cn_Ast_Switch *node = cn_ast_get(node_idx);

    // Type check condition expression.
    if (cn_ast_expression_typecheck(node->condition_idx) == NULL) return false;

    // Analyze body statement.
    if (!cn_ast_reparse_statement(node->body_idx)) return false;

    return true;
}

CNDEF bool cn_ast_reparse_iteration_statement(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_iteration_statement");
}

CNDEF bool cn_ast_reparse_goto(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_GOTO)) return false;
    // Goto label resolution would happen here if needed.
    return true;
}

CNDEF bool cn_ast_reparse_return(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_RETURN)) return false;

    Cn_Ast_Return *node = cn_ast_get(node_idx);

    // Type check return expression if present.
    if (!cn_ast_is_nil(node->expression_idx)) {
        if (cn_ast_expression_typecheck(node->expression_idx) == NULL) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_labeled_statement(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_labeled_statement");
}

CNDEF bool cn_ast_reparse_expression_statement(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_EXPRESSION_STATEMENT)) return false;

    Cn_Ast_Expression_Statement *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->expression_idx)) {
        if (cn_ast_expression_typecheck(node->expression_idx) == NULL) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_argument_list(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_argument_list");
}

CNDEF bool cn_ast_reparse_expression(Cn_Ast_Idx node_idx) {
    if (cn_ast_is_nil(node_idx)) return true;

    Cn_Ast_Node *node = cn_ast_get(node_idx);

    switch (node->kind) {
        case CN_AST_BINARY:
            node->binary.type = NULL;
            if (!cn_ast_reparse_expression(node->binary.left_idx)) return false;
            if (!cn_ast_reparse_expression(node->binary.right_idx)) return false;
            break;
        case CN_AST_ACCESS:
            node->access.type = NULL;
            if (!cn_ast_reparse_expression(node->access.expression_idx)) return false;
            break;
        case CN_AST_CALL:
            node->call.type = NULL;
            if (!cn_ast_reparse_expression(node->call.expression_idx)) return false;
            for (int64_t i = 0; i < node->call.arguments.length; i++) {
                if (!cn_ast_reparse_expression(node->call.arguments.idxs[i])) return false;
            }
            break;
        case CN_AST_UNARY:
            node->unary.type = NULL;
            if (!cn_ast_reparse_expression(node->unary.expression_idx)) return false;
            break;
        case CN_AST_CAST:
            node->cast.type = NULL;
            if (!cn_ast_reparse_type_name(node->cast.type_name_idx)) return false;
            if (!cn_ast_reparse_expression(node->cast.expression_idx)) return false;
            break;
        case CN_AST_SIZEOF:
            node->sizeof_expression.type = NULL;
            if (!cn_ast_is_nil(node->sizeof_expression.target_idx)) {
                Cn_Ast_Node *child = cn_ast_get(node->sizeof_expression.target_idx);
                if (child->kind == CN_AST_TYPE_NAME) {
                    if (!cn_ast_reparse_type_name(node->sizeof_expression.target_idx)) return false;
                } else {
                    if (!cn_ast_reparse_expression(node->sizeof_expression.target_idx)) return false;
                }
            }
            break;
        case CN_AST_TERNARY:
            node->ternary.type = NULL;
            if (!cn_ast_reparse_expression(node->ternary.condition_idx)) return false;
            if (!cn_ast_reparse_expression(node->ternary.true_idx)) return false;
            if (!cn_ast_reparse_expression(node->ternary.false_idx)) return false;
            break;
        case CN_AST_ASSIGN:
            node->assign.type = NULL;
            if (!cn_ast_reparse_expression(node->assign.left_idx)) return false;
            if (!cn_ast_reparse_expression(node->assign.right_idx)) return false;
            break;
        case CN_AST_POSTFIX:
            node->postfix.type = NULL;
            if (!cn_ast_reparse_expression(node->postfix.expression_idx)) return false;
            break;
        case CN_AST_PRIMARY:
            node->primary.type = NULL;
            break;
        default:
            break;
    }

    return true;
}

CNDEF bool cn_ast_reparse_init_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_INIT_DECLARATOR)) return false;

    Cn_Ast_Init_Declarator *node = cn_ast_get(node_idx);

    if (!cn_ast_reparse_declarator(node->declarator_idx)) return false;

    if (!cn_ast_is_nil(node->initializer_idx)) {
        if (!cn_ast_reparse_initializer(node->initializer_idx)) return false;
    }

    if (!cn_ast_is_nil(node->gnu_asm_label_idx)) {
        if (!cn_ast_reparse_gnu_asm_label(node->gnu_asm_label_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_initializer(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_INITIALIZER)) return false;

    Cn_Ast_Initializer *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->expression_idx)) {
        if (!cn_ast_reparse_expression(node->expression_idx)) return false;
    }

    // TODO: Initializer list.

    return true;
}

CNDEF bool cn_ast_reparse_abstract_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_DECLARATOR)) return false;

    Cn_Ast_Declarator *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->pointer_idx)) {
        if (!cn_ast_reparse_pointer(node->pointer_idx)) return false;
    }

    if (!cn_ast_is_nil(node->direct_declarator_idx)) {
        if (!cn_ast_reparse_direct_declarator(node->direct_declarator_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_DECLARATOR)) return false;

    Cn_Ast_Declarator *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->pointer_idx)) {
        if (!cn_ast_reparse_pointer(node->pointer_idx)) return false;
    }

    if (!cn_ast_is_nil(node->direct_declarator_idx)) {
        if (!cn_ast_reparse_direct_declarator(node->direct_declarator_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_pointer(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_POINTER)) return false;

    Cn_Ast_Pointer *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->pointer_idx)) {
        if (!cn_ast_reparse_pointer(node->pointer_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_direct_declarator(Cn_Ast_Idx node_idx) {
    if (cn_ast_is_nil(node_idx)) return true;

    Cn_Ast_Node *node = cn_ast_get(node_idx);

    switch (node->kind) {
        case CN_AST_IDENTIFIER:
            return true;
        case CN_AST_DIRECT_DECLARATOR_GROUPED: {
            Cn_Ast_Direct_Declarator_Grouped *grouped = cn_ast_get(node_idx);
            if (!cn_ast_reparse_declarator(grouped->declarator_idx)) return false;
            break;
        }
        case CN_AST_DIRECT_DECLARATOR_ARRAY: {
            Cn_Ast_Direct_Declarator_Array *arr = cn_ast_get(node_idx);
            if (!cn_ast_is_nil(arr->direct_declarator_idx)) {
                if (!cn_ast_reparse_direct_declarator(arr->direct_declarator_idx)) return false;
            }
            if (!cn_ast_is_nil(arr->expression_idx)) {
                if (!cn_ast_reparse_expression(arr->expression_idx)) return false;
            }
            break;
        }
        case CN_AST_DIRECT_DECLARATOR_FUNCTION: {
            Cn_Ast_Direct_Declarator_Function *fn = cn_ast_get(node_idx);
            if (!cn_ast_is_nil(fn->direct_declarator_idx)) {
                if (!cn_ast_reparse_direct_declarator(fn->direct_declarator_idx)) return false;
            }
            for (int64_t i = 0; i < fn->parameter_declarations.length; i++) {
                if (!cn_ast_reparse_parameter_declaration(fn->parameter_declarations.idxs[i])) return false;
            }
            break;
        }
        default:
            return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_declaration_specifiers(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_DECLARATION_SPECIFIERS)) return false;

    Cn_Ast_Declaration_Specifiers *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->type_specifier_idx)) {
        if (!cn_ast_reparse_type_specifier(node->type_specifier_idx)) return false;
    }

    for (int64_t i = 0; i < node->gnu_attribute_specifiers.length; i++) {
        if (!cn_ast_reparse_gnu_attribute_specifier(node->gnu_attribute_specifiers.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_type_specifier(Cn_Ast_Idx node_idx) {
    if (cn_ast_is_nil(node_idx)) return true;

    Cn_Ast_Node *node = cn_ast_get(node_idx);

    switch (node->kind) {
        case CN_AST_TYPE_SPECIFIER_PRIMITIVE:
            // Nothing to reparse for primitive types.
            return true;
        case CN_AST_TYPE_SPECIFIER_TYPEDEF: {
            Cn_Ast_Type_Specifier_Typedef *tdef = cn_ast_get(node_idx);
            Cn_Ast_Binding_Idx idx = cn_ast_binding_table_get(tdef->typedef_name, &cn__ast_data->symbol_binding_table);

            if (idx == CN_AST_NIL_BINDING_IDX || cn_ast_binding_get(idx)->kind != CN_BINDING_TYPEDEF) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node_idx, CN_DC_EXPECTED_TOKEN, "Expected valid typedef identifier in typedef type specifier.");
                return false;
            }
            return true;
        }
        case CN_AST_STRUCT_SPECIFIER:
        case CN_AST_UNION_SPECIFIER:
            return cn_ast_reparse_struct_or_union_specifier(node_idx);
        case CN_AST_ENUM_SPECIFIER:
            return cn_ast_reparse_enum_specifier(node_idx);
        case CN_AST_GNU_TYPEOF:
            return cn_ast_reparse_gnu_typeof_specifier(node_idx);
        default:
            return true;
    }
}

CNDEF bool cn_ast_reparse_gnu_typeof_specifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_GNU_TYPEOF)) return false;

    Cn_Ast_Gnu_Typeof *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->target_idx)) {
        Cn_Ast_Node *child = cn_ast_get(node->target_idx);
        if (child->kind == CN_AST_TYPE_NAME) {
            if (!cn_ast_reparse_type_name(node->target_idx)) return false;
        } else {
            if (!cn_ast_reparse_expression(node->target_idx)) return false;
        }
    }

    return true;
}

CNDEF bool cn_ast_reparse_type_name(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_TYPE_NAME)) return false;

    Cn_Ast_Type_Name *node = cn_ast_get(node_idx);

    if (!cn_ast_reparse_specifier_qualifier(node->specifier_qualifier_idx)) return false;

    if (!cn_ast_is_nil(node->abstract_declarator_idx)) {
        if (!cn_ast_reparse_abstract_declarator(node->abstract_declarator_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_specifier_qualifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_SPECIFIER_QUALIFIER)) return false;

    Cn_Ast_Specifier_Qualifier *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->type_specifier_idx)) {
        if (!cn_ast_reparse_type_specifier(node->type_specifier_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_parameter_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_PARAMETER_DECLARATION)) return false;

    Cn_Ast_Parameter_Declaration *node = cn_ast_get(node_idx);

    if (!cn_ast_reparse_declaration_specifiers(node->declaration_specifiers_idx)) return false;

    if (!cn_ast_is_nil(node->declarator_idx)) {
        Cn_Ast_Node *decl = cn_ast_get(node->declarator_idx);
        if (decl->kind == CN_AST_DECLARATOR && (decl->flags & CN_AST_DECLARATOR_IS_ABSTRACT)) {
            if (!cn_ast_reparse_abstract_declarator(node->declarator_idx)) return false;
        } else {
            if (!cn_ast_reparse_declarator(node->declarator_idx)) return false;
        }
    }

    return true;
}

CNDEF bool cn_ast_reparse_struct_or_union_specifier(Cn_Ast_Idx node_idx) {
    Cn_Ast_Node *node = cn_ast_get(node_idx);

    if (node->kind != CN_AST_STRUCT_SPECIFIER && node->kind != CN_AST_UNION_SPECIFIER) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node_idx, CN_DC_EXPECTED_AST_NODE, "Expected struct or union specifier node.");
        return false;
    }

    // Both struct and union specifiers have member_declarations at the same offset.
    Cn_Ast_Struct_Specifier *struct_node = cn_ast_get(node_idx);
    for (int64_t i = 0; i < struct_node->member_declarations.length; i++) {
        if (!cn_ast_reparse_member_declaration(struct_node->member_declarations.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_member_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_MEMBER_DECLARATION)) return false;

    Cn_Ast_Member_Declaration *node = cn_ast_get(node_idx);

    if (!cn_ast_reparse_specifier_qualifier(node->specifier_qualifier_idx)) return false;

    for (int64_t i = 0; i < node->member_declarators.length; i++) {
        if (!cn_ast_reparse_member_declarator(node->member_declarators.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_member_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_MEMBER_DECLARATOR)) return false;

    Cn_Ast_Member_Declarator *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->declarator_idx)) {
        if (!cn_ast_reparse_declarator(node->declarator_idx)) return false;
    }

    if (!cn_ast_is_nil(node->bitfield_idx)) {
        if (!cn_ast_reparse_expression(node->bitfield_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_attribute_specifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_GNU_ATTRIBUTE_SPECIFIER)) return false;

    Cn_Ast_Gnu_Attribute_Specifier *node = cn_ast_get(node_idx);

    for (int64_t i = 0; i < node->gnu_attributes.length; i++) {
        if (!cn_ast_reparse_gnu_attribute(node->gnu_attributes.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_attribute(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_GNU_ATTRIBUTE)) return false;

    Cn_Ast_Gnu_Attribute *node = cn_ast_get(node_idx);

    for (int64_t i = 0; i < node->arguments.length; i++) {
        if (!cn_ast_reparse_expression(node->arguments.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_asm_label(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_GNU_ASM_LABEL)) return false;

    // String child doesn't need reparsing.

    return true;
}

CNDEF bool cn_ast_reparse_enum_specifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_ENUM_SPECIFIER)) return false;

    Cn_Ast_Enum_Specifier *node = cn_ast_get(node_idx);

    for (int64_t i = 0; i < node->enumerators.length; i++) {
        if (!cn_ast_reparse_enumerator(node->enumerators.idxs[i])) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_enumerator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_ENUMERATOR)) return false;

    Cn_Ast_Enumerator *node = cn_ast_get(node_idx);

    if (!cn_ast_is_nil(node->expression_idx)) {
        if (!cn_ast_reparse_expression(node->expression_idx)) return false;
    }

    return true;
}

// DIAGNOSTIC SECTION

const Cn_String CN_DIAGNOSTIC_CODES[CN__DC_COUNT] = {
    [CN_DC_DUPLICATE_TYPE_SPECIFIER]        = CN_STR_BUFFER("Duplicate type specifier"),
    [CN_DC_EXPECTED_TOKEN]                  = CN_STR_BUFFER("Expected token"),
    [CN_DC_EXPECTED_EXPRESSION]             = CN_STR_BUFFER("Expected expression"),
    [CN_DC_UNEXPECTED_TOKEN]                = CN_STR_BUFFER("Unexpected token"),
    [CN_DC_INVALID_DECLARATOR]              = CN_STR_BUFFER("Invalid declarator"),
    [CN_DC_INVALID_TYPE_NAME]               = CN_STR_BUFFER("Invalid type name"),
    [CN_DC_MULTIPLE_STORAGE_SPECIFIERS]     = CN_STR_BUFFER("Multiple storage specifiers"),
    [CN_DC_MISSING_DECLARATION_SPECIFIER]   = CN_STR_BUFFER("Missing declaration specifier"),
    [CN_DC_INVALID_TYPE_SPECIFIER]          = CN_STR_BUFFER("Invalid type specifier"),
    [CN_DC_REDEFINITION]                    = CN_STR_BUFFER("Redefinition"),
    [CN_DC_INCOMPLETE_TYPE]                 = CN_STR_BUFFER("Incomplete type"),
    [CN_DC_INVALID_SYMBOL]                  = CN_STR_BUFFER("Invalid symbol"),
    [CN_DC_ILLEGAL_TYPE]                    = CN_STR_BUFFER("Illegal type"),
    [CN_DC_INVALID_CONSTANT_EXPRESSION]     = CN_STR_BUFFER("Invalid constant expression"),
    [CN_DC_ILLEGAL_BINDING]                 = CN_STR_BUFFER("Illegal binding"),
    [CN_DC_FROM]                            = CN_STR_BUFFER("Comming from"),
    [CN_DC_EXPECTED_AST_NODE]               = CN_STR_BUFFER("Expected ast node"),
    [CN_DC_UNEXPECTED_FUNCTION]             = CN_STR_BUFFER("Unexpected function definition"),
};

Cn_Diagnostic_Level cn_min_diagnostic_level = CN_DIAGNOSTIC_INFO;

Cn_Diagnostic_Handler *cn_diagnostic_handler = &cn_default_diagnostic_handler;

CNDEF void cn_default_diagnostic_handler(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, Cn_Diagnostic_Annotation annotations[], size_t annotations_length, const char *format, va_list args) {
    if (level < cn_min_diagnostic_level) return;
    const char *ansi_color = "";

    switch (level) {
        case CN_DIAGNOSTIC_INFO:
            ansi_color = CN_ANSI_BRIGHT_BLUE CN_ANSI_BOLD ;
            fprintf(stderr, CN_ANSI_BRIGHT_BLUE CN_ANSI_BOLD "info: "CN_ANSI_RESET);
            break;
        case CN_DIAGNOSTIC_WARNING:
            ansi_color = CN_ANSI_YELLOW CN_ANSI_BOLD ;
            fprintf(stderr, CN_ANSI_YELLOW CN_ANSI_BOLD "warning: "CN_ANSI_RESET);
            break;
        case CN_DIAGNOSTIC_ERROR:
            ansi_color = CN_ANSI_BRIGHT_RED CN_ANSI_BOLD ;
            fprintf(stderr, CN_ANSI_BRIGHT_RED CN_ANSI_BOLD "error: "CN_ANSI_RESET);
            break;
    }

    if (loc->line == 0 && loc->column == 0) {
        fprintf(stderr, CN_ANSI_BOLD"%.*s"CN_ANSI_RESET" ", CN_UNPACK(loc->file));
    } else {
        fprintf(stderr, CN_ANSI_BOLD"%.*s:%ld:%ld:"CN_ANSI_RESET" ", CN_UNPACK(loc->file), loc->line, loc->column);
    }

    if (!cn_str_is_empty(CN_DIAGNOSTIC_CODES[code])) {
        fprintf(stderr, "%.*s: ", CN_UNPACK(CN_DIAGNOSTIC_CODES[code]));
    } else {
        fprintf(stderr, CN_ANSI_BRIGHT_BLACK"[CN%04d]"CN_ANSI_RESET": ", code);
    }
    

    // Simple sorting + overlap resolution.
    {
        Cn_Diagnostic_Annotation temp;
        size_t min;
        int64_t overlap = 0;
        for (size_t j = 0; j < annotations_length - 1; j++) {
            min = j;
            for (size_t i = j + 1; i < annotations_length; i++) {
                if (annotations[i].offset < annotations[min].offset) {
                    min = i;
                }
            }

            if (min != j) {
                temp = annotations[j];
                annotations[j] = annotations[min];
                annotations[min] = temp;

            }

            if (annotations[j].offset <= overlap) {
                // First annotation always wins, if overlap.
                annotations[j].length = 0;
            } else {
                overlap = annotations[j].offset + annotations[j].length;
            }
        }
    }

    vfprintf(stderr, format, args);
    fputs(CN_LINE_END CN_LINE_END, stderr);

    // Printing span line by line, offseting by 1 indent level.
    int64_t eol;
    int64_t line_offset = 0;
    Cn_String line;
    while (true) {
        eol = cn_str_find_left(span, CN_STR_LIT(CN_LINE_END));
        if (eol == -1) break;

        line = cn_str_get_chars(span, eol);
        span = cn_str_eat_chars(span, eol + sizeof(CN_LINE_END) - 1);

        // Applying annotations.
        fputs(CN_INDENT, stderr);
        int64_t printed = 0;
        Cn_String snippet = {0};
        while (annotations_length > 0 && annotations->offset < line.length + line_offset) {
            if (annotations->length == 0) {
                goto annotations_next;
            }

            int64_t inline_offset = annotations->offset - line_offset;
            if (inline_offset < 0) {
                goto annotations_next;
            }

            // Printing till annotation.
            if (inline_offset > printed) {
                snippet = cn_str_substring(line, printed, inline_offset);
                fprintf(stderr, "%.*s", CN_UNPACK(snippet));
                printed += inline_offset;
            }

            // Printing annotated snippet.
            int64_t length = annotations->length < line.length - printed ? annotations->length : line.length - printed;
            snippet = cn_str_substring(line, printed, printed + length);
            fprintf(stderr, "%s%.*s"CN_ANSI_RESET, ansi_color, CN_UNPACK(snippet));
            printed += length;


annotations_next:
            annotations++;
            annotations_length--;
        }

        if (printed < line.length) {
            snippet = cn_str_substring(line, printed, line.length);
            fprintf(stderr, "%.*s", CN_UNPACK(snippet));
        }

        fputs(CN_LINE_END, stderr);

        line_offset += line.length + sizeof(CN_LINE_END) - 1;
    }

    fputs(CN_ANSI_RESET CN_LINE_END, stderr);
}

CNDEF void cn_null_diagnostic_handler(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, Cn_Diagnostic_Annotation annotations[], size_t annotations_length, const char *format, va_list args) {
    CN_UNUSED(level);
    CN_UNUSED(loc);
    CN_UNUSED(code);
    CN_UNUSED(span);
    CN_UNUSED(annotations);
    CN_UNUSED(annotations_length);
    CN_UNUSED(format);
    CN_UNUSED(args);
}

CNDEF void cn__diagnostic_src(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Source *src, Cn_Diagnostic_Code code, const char *format, va_list args) {
    switch (level) {
        case CN_DIAGNOSTIC_WARNING:
            cn__ast_data->warning_count++;
            break;
        case CN_DIAGNOSTIC_ERROR:
            cn__ast_data->error_count++;
            break;
        default:
            break;
    }

    int64_t index;

    Cn_String span_left = cn_str_get_chars(src->source, src->offset);
    index = cn_str_find_right(span_left, CN_STR_LIT(CN_LINE_END));
    if (index != -1) {
        span_left = cn_str_eat_chars(span_left, index + sizeof(CN_LINE_END) - 1);
    }

    span_left = cn_str_eat_spaces(span_left);

    Cn_String span_right = cn_str_eat_chars(src->source, src->offset);
    index = cn_str_find_left(span_right, CN_STR_LIT(CN_LINE_END));
    if (index != -1) {
        span_right = cn_str_get_chars(span_right, index + sizeof(CN_LINE_END) - 1);
    }

    Cn_String span = CN_STR(span_left.length + span_right.length, span_left.data);

    Cn_Diagnostic_Annotation annotations[1] = {
        { .offset = src->offset - (span_left.data - src->source.data), .length = src->length }
    };
    cn_diagnostic_handler(level, loc, code, span, annotations, CN_ARRAY_LENGTH(annotations), format, args);
}

CNDEF void cn_diagnostic_src(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Source *src, Cn_Diagnostic_Code code, const char *format, ...) {
    va_list args;
    va_start(args, format);
    cn__diagnostic_src(level, loc, src, code, format, args);
    va_end(args);
}

CNDEF void cn_diagnostic_node(Cn_Diagnostic_Level level, Cn_Ast_Idx idx, Cn_Diagnostic_Code code, const char *format, ...) {
    CN_ASSERT(idx != CN_AST_NIL_IDX);

    Cn_Ast_Node *node = cn_ast_get(idx);

    if (!(node->flags & CN_AST_SYNTHETIC)) {
        va_list args;
        va_start(args, format);
        cn__diagnostic_src(level, &node->loc, &node->src, code, format, args);
        va_end(args);
        return;
    }

    switch (level) {
        case CN_DIAGNOSTIC_WARNING:
            cn__ast_data->warning_count++;
            break;
        case CN_DIAGNOSTIC_ERROR:
            cn__ast_data->error_count++;
            break;
        default:
            break;
    }

    Cn_Ast_Idx parent_idx = idx; 
    Cn_Ast_Node *parent;
    while (true) {
        CN_ASSERT(parent_idx != CN_AST_NIL_IDX);
        parent = cn_ast_get(parent_idx);
        switch (parent->kind) {
            case CN_AST_TRANSLATION_UNIT:
            case CN_AST_EXTERNAL_DECLARATION:
            case CN_AST_BLOCK:
            case CN_AST_EXPRESSION_STATEMENT:
            case CN_AST_IF:
            case CN_AST_SWITCH:
            case CN_AST_GOTO:
            case CN_AST_RETURN:
            case CN_AST_BREAK:
            case CN_AST_CONTINUE:
                break;
            default: 
                parent_idx = parent->parent_idx;
                continue;
        }

        break;
    }

    Cn_String span = {0};

    Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    
    int64_t length, offset;
    cn_emit(parent_idx, &cn_emit_write_sb, .ctx = &sb, .max_lines = 1, .highlight_idx = idx, .highlight_length = &length, .highlight_offset = &offset);

    span = cn_sb_to_str(&sb);

    Cn_Diagnostic_Annotation annotations[1] = {
        { .offset = offset, .length = length }
    };

    va_list args;
    va_start(args, format);
    cn_diagnostic_handler(level, &node->loc, code, span, annotations, CN_ARRAY_LENGTH(annotations), format, args);
    va_end(args);
    
    // Not checking for NULL, because synthetic nodes are guranteed to be replaced from somewhere.
    // So NIL parent won't be reached till replaced node is found.
    parent_idx = idx; 
    parent = cn_ast_get(parent_idx);
    while (!(parent->flags & CN_AST_IS_REPLACED)) {
        parent_idx = parent->parent_idx;
        CN_ASSERT(parent_idx != CN_AST_NIL_IDX);
    }

    node = cn_ast_get(parent_idx);
    if (node->replaced_idx != CN_AST_NIL_IDX) {
        node->flags |= CN_AST_USE_REPLACED;
        cn_diagnostic_node(CN_DIAGNOSTIC_INFO, node->replaced_idx, CN_DC_FROM, "From ast node here.");
        node->flags &= ~CN_AST_USE_REPLACED;
    }
}


// PRE-PROCESSING SECTION

Cn_Message_Handler *cn_message_handler = NULL;

CNDEF bool cn_send_message(Cn_Message_Kind kind, Cn_Message message) {
    if (cn_message_handler == NULL) return false;

    return cn_message_handler(kind, &message) != 0;
}

CNDEF void cn_log_types() {
    if (CN_INFO >= cn_min_log_level) {
        cn_log(CN_INFO, "Type universe main.i:" CN_ANSI_BLUE);

        void *block = cn__ast_data->type_arena.block;
        Cn_Chained_Arena_Block_Header *h;
        while(true) {
            cn_ast_chained_arena_foreach_in_block(Cn_Type, type, cn__ast_data->type_arena.block) {
                cn_type_print(type);
                fputc('\n', stderr);
            }

            h = CN_CHAINED_ARENA_BLOCK_HEADER(block);
            if (h->prev == NULL) break;

            block = h->prev;
        }
        fprintf(stderr, CN_ANSI_RESET"\n");
    }
}

CNDEF void cn_log_bindings() {
    if (CN_INFO >= cn_min_log_level) {
        // First binding is NIL, so skip index 0.
        cn_log(CN_INFO, "Bindings main.i:" CN_ANSI_BRIGHT_YELLOW);
        for (int i = 1; i < cn_array_list_length(&cn__ast_data->binding_list); i++) {
            switch (cn__ast_data->binding_list[i].kind) {
                case CN_BINDING_VARIABLE:
                    fputs("VARIABLE    ", stderr);
                    break;
                case CN_BINDING_FUCNTION:
                    fputs("FUNCTION    ", stderr);
                    break;
                case CN_BINDING_TYPEDEF:
                    fputs("TYPEDEF     ", stderr);
                    break;
                case CN_BINDING_ENUM_CONSTANT:
                    fputs("ENUM CONST  ", stderr);
                    break;
                case CN_BINDING_TAG:
                    fputs("TAG         ", stderr);
                    break;
                default:
                    break;
            }

            fprintf(stderr, "%.*s -> ", CN_UNPACK(cn__ast_data->binding_list[i].name));
            cn_type_print(cn__ast_data->binding_list[i].type);
            fputc('\n', stderr);
        }
        fprintf(stderr, CN_ANSI_RESET"\n");
    }
}

CNDEF Cn_Translation_Unit cn_tu_make_opt(char *intermidiate_path, Cn_Tu_Make_Opt opt) {
    if (!cn_str_is_empty(opt.source)) {
        Cn_Translation_Unit tu = {
            .path = intermidiate_path,
            .content = opt.source,
        };
        
        tu.no_malloc = true;
        cn_ast_init(&tu.ast_data);
        tu.ast_data.source = tu.content;

        return tu;
    }

    // Reading the whole .i file into memory.
    FILE *file = fopen(intermidiate_path, "rb");
    if (file == NULL) {
        cn_log(CN_ERROR, "Couldn't open the file '%s'.\n", intermidiate_path);
        return (Cn_Translation_Unit) {0};
    }

    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    void *buffer = CN_REALLOC(NULL, size);
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

    Cn_Translation_Unit tu = {
        .path = intermidiate_path,
        .content = CN_STR((int64_t)size, buffer),
    };


    cn_ast_init(&tu.ast_data);
    tu.ast_data.source = tu.content;

    return tu;
}

const Cn_Lexer_Blacklist cn_default_blacklist = {
    .length = 2,
    .ttypes = ((Cn_Token_Type[]){ CN_TOKEN_COMMENT, CN_TOKEN_LINE_MARKER }),
};

CNDEF int cn_tu_process(Cn_Translation_Unit *tu, Cn_Flags flags) {
    cn__ast_data = &tu->ast_data;

    // Printing source.
    if (flags & CN_PRINT_SOURCE) {
        cn_log(CN_INFO, "Received main.i:\n" CN_ANSI_BRIGHT_BLACK "%.*s" CN_ANSI_RESET, CN_UNPACK(tu->content));
    }

    // Setting up lexer.
    Cn_Lexer lexer = {0};

    // Printing tokens.
    if (flags & CN_PRINT_TOKENS) {
        cn_lexer_init(&lexer, tu->content, cn_default_blacklist);
        cn_log(CN_INFO, "Tokenized main.i:" CN_ANSI_CYAN);
        do {
            Cn_String str = cn_source_to_str(&cn_lexer_token(&lexer).src);
            fprintf(stderr, "TOKEN:     %.*s\n", CN_UNPACK(str));
            cn_lexer_next_token(&lexer);
        } while (cn_lexer_token(&lexer).type != CN_TOKEN_EOF);
        fprintf(stderr, CN_ANSI_RESET"\n");
    }
    
    // Building AST.
    cn_lexer_init(&lexer, tu->content, cn_default_blacklist);

    Cn_Ast_Idx idx = cn_ast_parse_translation_unit(&lexer);

    if (cn__ast_data->error_count > 0) return -1;
    CN_ASSERT(idx != CN_AST_NIL_IDX);

    // Printing AST.
    if (flags & CN_PRINT_AST) {
        cn_log(CN_INFO, "Parsed main.i:");
        cn_ast_print(idx, 0);
        fputc('\n', stderr);
    }

    // Printing type universe.
    if (flags & CN_PRINT_TYPES) {
        cn_log_types();
    }

    // Printing bindings.
    if (flags & CN_PRINT_BINDINGS) {
        cn_log_bindings();
    }

    // Emit AST back to the same .i file.
    if (!(flags & CN_NO_CODE_OUTPUT)) {
        FILE *out = fopen(tu->path, "w");
        if (out == NULL) {
            cn_log(CN_ERROR, "Failed to open '%s' for writing.", tu->path);
            return -1;
        }
        cn_emit(idx, &cn_emit_write_file, .ctx = out);
        fclose(out);
    }

    return 0;
}

CNDEF void cn_tu_free(Cn_Translation_Unit *tu) {
    cn_ast_free(&tu->ast_data);

    if (!tu->no_malloc) CN_FREE(tu->content.data);
}

CNDEF void cn_replace(Cn_Ast_Idx *original, Cn_Ast_Idx new, Cn_Ast_Idx parent_idx) {
    Cn_Ast_Node *node = cn_ast_get(new);
    node->flags |= CN_AST_IS_REPLACED;
    node->replaced_idx = *original;
    node->parent_idx = parent_idx;
    *original = new;
}

CNDEF Cn_Ast_Idx cn__build_wrap_if_primary(Cn_Ast_Idx idx) {
    if (cn_ast_is_primary(idx)) {
        Cn_Ast_Node *node = cn_ast_get(idx);
        Cn_Ast_Idx parent_idx = cn_ast_node_list_append((Cn_Ast_Node) { 
                    .primary.kind        = CN_AST_PRIMARY, 
                    .primary.loc         = node->loc, 
                    .primary.src         = node->src, 
                    .primary.literal_idx = (idx), 
                });

        node = cn_ast_get(idx);
        node->parent_idx = parent_idx;
        return parent_idx;
    }

    return idx;
}

CNDEF Cn_String cn__build_make_location(const char *func, const char *file, int64_t line) {
    Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    cn_sb_append_format(&sb, "<%s() at %s:%ld>", func, file, line);
    Cn_String loc = cn__ast_permanent_save_string(cn_sb_to_str(&sb));
    cn_sb_free(&sb);
    return loc;
}

CNDEF Cn_Ast_Idx cn__build_identifier(Cn_String name, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_IDENTIFIER,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (opt.alloc) {
        node.identifier.name = cn__ast_permanent_save_string(name);
    } else {
        node.identifier.name = name;
    }

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_integer(Cn_String value, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_INTEGER,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (opt.alloc) {
        node.integer.value = cn__ast_permanent_save_string(value);
    } else {
        node.integer.value = value;
    }

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_float(Cn_String value, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_FLOAT,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (opt.alloc) {
        node.flt.value = cn__ast_permanent_save_string(value);
    } else {
        node.flt.value = value;
    }

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_string(Cn_String str, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_STRING,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (opt.alloc) {
        node.string.str = cn__ast_permanent_save_string(str);
    } else {
        node.string.str = str;
    }

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_binary(Cn_Binary_Operator_Kind op, Cn_Ast_Idx left, Cn_Ast_Idx right, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_BINARY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.binary.operator = op;
    node.binary.left_idx = cn__build_wrap_if_primary(left);
    node.binary.right_idx = cn__build_wrap_if_primary(right);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.binary.left_idx,
            node.binary.right_idx,
            );
    return parent;
}

CNDEF Cn_Ast_Idx cn__build_unary(Cn_Unary_Operator_Kind op, Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_UNARY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.unary.operator = op;
    node.unary.expression_idx = cn__build_wrap_if_primary(expression);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.unary.expression_idx
            );
    return parent;
}

CNDEF Cn_Ast_Idx cn__build_func_call(Cn_Ast_Idx callee, Cn_Ast_List arguments, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_CALL,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.call.expression_idx = cn__build_wrap_if_primary(callee);

    for (int64_t i = 0; i < arguments.length; i++) {
        arguments.idxs[i] = cn__build_wrap_if_primary(arguments.idxs[i]);
    }

    node.call.arguments = arguments;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.call.expression_idx);
    cn_ast_list_set_parent(parent, &node.call.arguments);
    return parent;
}

CNDEF Cn_Ast_Idx cn__build_expr_statement(Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_EXPRESSION_STATEMENT,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.expression_statement.expression_idx = cn__build_wrap_if_primary(expression);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.expression_statement.expression_idx);
    return parent;
}


#endif // CN_IMPLEMENTATION


/* 
    Revision history:
        
        v0.1.0 (2026-07-24) Initial development release.
*/

/*
    Version conventions:

        Following MAJOR.MINOR.PATCH format described in https://semver.org/ Semantic Versioning 2.0.0:
            
      - Comments modification don't update the version.
      - Major version is incremented when any backward incompatible changes are made to the public API.
      - Minor version is incremented when backward compatible changes are made to the public API.
      - Patch version is incremented if only backward compatible bug fixes are made without changing public API.
      - Major version '0.y.z' is for intial development. Anything may change at any time. 
        Public API is not stable.
*/

/*
    MIT License
    
    Copyright (c) 2026 Daniil Yarmalovich
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
