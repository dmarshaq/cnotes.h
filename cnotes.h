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
#    define CN_LINE_END "\r\n"
#else
#    define CN_LINE_END "\n"
#endif // _WIN32

#define CN_INDENT "    "

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

// TYPE SECTION

typedef struct cn_type Cn_Type;

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

typedef struct {
    bool is_signed;
} Cn_Type_Integer;

typedef struct {
    Cn_Type *ptr_to;
} Cn_Type_Pointer;

typedef struct {
    Cn_Type *type;
} Cn_Type_Function_Param;

typedef struct {
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

typedef struct {
    Cn_String tag;
    
    int64_t members_length;
    Cn_Type_Struct_Member *members;
} Cn_Type_Struct;

typedef struct {
    Cn_Type *type;
    Cn_String name;
} Cn_Type_Union_Member;

typedef struct {
    Cn_String tag;
    
    int64_t members_length;
    Cn_Type_Union_Member *members;
} Cn_Type_Union;

typedef struct {
    Cn_Type *element_type;
    int64_t length;
} Cn_Type_Array;

typedef struct {
    Cn_String name;
    int64_t value;
} Cn_Type_Enum_Member;

typedef struct {
    bool is_signed;
    int64_t members_length;
    Cn_Type_Enum_Member *members;
} Cn_Type_Enum;

typedef enum : uint8_t {
    CN_TYPE_CONSTANT = 0x01,
    CN_TYPE_RESTRICT = 0x02,
    CN_TYPE_VOLATILE = 0x04,
    CN_TYPE_ATOMIC   = 0x08,
} Cn_Type_Qualified_Flags;

typedef struct {
    Cn_Type_Qualified_Flags flags;
    Cn_Type *base_type;
} Cn_Type_Qualified;

typedef enum : uint8_t {
    CN_TYPE_COMPLETE = 0x01,
} Cn_Type_Flags;

struct cn_type {
    Cn_Type_Flags flags;
    int64_t size;
    int64_t align;

    Cn_Type_Kind kind;
    union {
        Cn_Type_Integer   t_integer;
        Cn_Type_Pointer   t_pointer;
        Cn_Type_Function  t_function;
        Cn_Type_Struct    t_struct;
        Cn_Type_Union     t_union;
        Cn_Type_Array     t_array;
        Cn_Type_Enum      t_enum;
        Cn_Type_Qualified t_qualified;
    };
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
CNDEF Cn_Type cn_type_make_qualified(Cn_Type_Qualified_Flags flags, const Cn_Type *type);

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

#define CN_AST_NIL_IDX 0

#define CN_AST_ERROR_IDX 1

typedef struct {
    Cn_Ast_Idx last_idx;
    Cn_Ast_Idx first_idx;
    int64_t length;
} Cn_Ast_Linked_List;

typedef enum : uint8_t {
    CN_AST_TYPE_QUALIFIER_CONST         = 0x01,
    CN_AST_TYPE_QUALIFIER_RESTRICT      = 0x02,
    CN_AST_TYPE_QUALIFIER_VOLATILE      = 0x04,
    CN_AST_TYPE_QUALIFIER_ATOMIC        = 0x08,
} Cn_Qualifier_Flags;

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

    CN_AST_TYPE_TYPEDEF,
    CN_AST_TYPE_STRUCT_OR_UNION,
    CN_AST_TYPE_GNU_TYPEOF,
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

/**
 * Grabs next token in the lexer the same way as lexer next token except,
 * it ignores blacklistted tokens, for example comments.
 */
CNDEF void cn_ast_next_token(Cn_Lexer *lexer);

/**
 * Peeks next token in the lexer the same way as lexer peek except,
 * it ignores blacklistted tokens, for example comments.
 * If blacklisted token encountered it peeks again till valid token encountered.
 */
CNDEF Cn_Token cn_ast_peek(Cn_Lexer lexer);

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
    CN_AST_NODE_ERROR                          = 1,
    CN_AST_NODE_CODE,
    CN_AST_NODE_TRANSLATION_UNIT,
    CN_AST_NODE_EXTERNAL_DECLARATION,
    CN_AST_NODE_DECLARATION,
    CN_AST_NODE_FUNCTION_DEFINITION,
    CN_AST_NODE_ASM_DEFINITION,
    CN_AST_NODE_COMPOUND_STATEMENT,
    CN_AST_NODE_SELECTION_STATEMENT,
    CN_AST_NODE_ITERATION_STATEMENT,
    CN_AST_NODE_JUMP_STATEMENT,
    CN_AST_NODE_LABELED_STATEMENT,
    CN_AST_NODE_EXPRESSION_STATEMENT,
    CN_AST_NODE_BINARY_EXPRESSION,
    CN_AST_NODE_ACCESS_EXPRESSION,
    CN_AST_NODE_FUNCTION_EXPRESSION,
    CN_AST_NODE_UNARY_EXPRESSION,
    CN_AST_NODE_CAST_EXPRESSION,
    CN_AST_NODE_SIZEOF_EXPRESSION,
    CN_AST_NODE_TERNARY_EXPRESSION,
    CN_AST_NODE_ASSIGNMENT_EXPRESSION,
    CN_AST_NODE_POSTFIX_EXPRESSION,
    CN_AST_NODE_PRIMARY_EXPRESSION,
    CN_AST_NODE_IDENTIFIER,
    CN_AST_NODE_INTEGER,
    CN_AST_NODE_FLOAT,
    CN_AST_NODE_STRING,
    CN_AST_NODE_INIT_DECLARATOR_LIST,
    CN_AST_NODE_INIT_DECLARATOR,
    CN_AST_NODE_INITIALIZER,
    CN_AST_NODE_ABSTRACT_DECLARATOR,
    CN_AST_NODE_DECLARATOR,
    CN_AST_NODE_POINTER,
    CN_AST_NODE_DIRECT_DECLARATOR,
    CN_AST_NODE_DECLARATION_SPECIFIERS,
    CN_AST_NODE_GNU_TYPEOF_SPECIFIER,
    CN_AST_NODE_TYPE_SPECIFIER,
    CN_AST_NODE_TYPE_NAME,
    CN_AST_NODE_SPECIFIER_QUALIFIER,
    CN_AST_NODE_PARAMETER_TYPE_LIST,
    CN_AST_NODE_PARAMETER_DECLARATION,
    CN_AST_NODE_STRUCT_SPECIFIER,
    CN_AST_NODE_UNION_SPECIFIER,
    CN_AST_NODE_MEMBER_DECLARATION,
    CN_AST_NODE_MEMBER_DECLARATOR,
    CN_AST_NODE_GNU_ATTRIBUTE_SPECIFIER,
    CN_AST_NODE_GNU_ATTRIBUTE,
    CN_AST_NODE_GNU_ASM_LABEL,
} Cn_Ast_Node_Kind;

typedef struct {
    Cn_String text;
} Cn_Ast_Node_Code;

typedef struct {
    Cn_Ast_Linked_List external_declaration_list;
} Cn_Ast_Node_Translation_Unit;

typedef struct {
    Cn_Ast_Idx child_idx;
    bool extension;
} Cn_Ast_Node_External_Declaration;

typedef struct {
    Cn_Ast_Idx declaration_specifiers_idx;
    Cn_Ast_Idx init_declarator_list_idx;
    Cn_Ast_Linked_List gnu_attribute_specifier_sequence;
} Cn_Ast_Node_Declaration;

typedef struct {
    Cn_Ast_Idx declaration_specifiers_idx;
    Cn_Ast_Idx declarator_idx;
    Cn_Ast_Idx compound_statement_idx;
} Cn_Ast_Node_Function_Definition;

typedef struct {
    Cn_Ast_Linked_List statement_or_declaration_list;
} Cn_Ast_Node_Compound_Statement;

typedef enum {
    CN_AST_SELECTION_IF,
    CN_AST_SELECTION_SWITCH,
} Cn_Ast_Selection_Kind;

typedef struct {
    Cn_Ast_Selection_Kind kind;
    Cn_Ast_Idx condition_expression_idx;
    Cn_Ast_Idx statement_idx;
    Cn_Ast_Idx else_statement_idx;
} Cn_Ast_Node_Selection_Statement;

typedef enum {
    CN_AST_JUMP_GOTO,
    CN_AST_JUMP_CONTINUE,
    CN_AST_JUMP_BREAK,
    CN_AST_JUMP_RETURN,
} Cn_Ast_Jump_Kind;

typedef struct {
    Cn_Ast_Jump_Kind kind;
    union {
        Cn_Ast_Idx identifier_idx;
        Cn_Ast_Idx expression_idx;
    };
} Cn_Ast_Node_Jump_Statement;

typedef struct {
    Cn_Ast_Idx expression_idx;
} Cn_Ast_Node_Expression_Statement;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx left_expression_idx;
    Cn_Binary_Operator_Kind operator_kind;
    Cn_Ast_Idx right_expression_idx;
} Cn_Ast_Node_Binary_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx expression_idx;
    bool pointer;
    Cn_Ast_Idx identifier_idx;
} Cn_Ast_Node_Access_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx expression_idx;
    Cn_Ast_Linked_List argument_list;
} Cn_Ast_Node_Function_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Unary_Operator_Kind operator_kind;
    Cn_Ast_Idx expression_idx;
} Cn_Ast_Node_Unary_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx type_name_idx;
    Cn_Ast_Idx expression_idx;
} Cn_Ast_Node_Cast_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx child_idx;   // Could be expression or type name.
} Cn_Ast_Node_Sizeof_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx condition_expression_idx;
    Cn_Ast_Idx true_expression_idx;
    Cn_Ast_Idx false_expression_idx;
} Cn_Ast_Node_Ternary_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx left_expression_idx;
    Cn_Assignment_Operator_Kind operator_kind;
    Cn_Ast_Idx right_expression_idx;
} Cn_Ast_Node_Assignment_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx expression_idx;
    Cn_Postfix_Operator_Kind operator_kind;
} Cn_Ast_Node_Postfix_Expression;

typedef struct {
    Cn_Type *type;
    Cn_Ast_Idx literal_idx; // Identifier, integer, float or string.
} Cn_Ast_Node_Primary_Expression;

typedef struct {
    Cn_String name;
} Cn_Ast_Node_Identifier;

typedef struct {
    Cn_String value;
} Cn_Ast_Node_Integer;

typedef struct {
    Cn_String value;
} Cn_Ast_Node_Float;

typedef struct {
    Cn_String str;
} Cn_Ast_Node_String;

typedef struct {
    Cn_Ast_Linked_List init_declarator_list;
} Cn_Ast_Node_Init_Declarator_List;

typedef struct {
    Cn_Ast_Idx declarator_idx;
    Cn_Ast_Idx initializer_idx;
    Cn_Ast_Idx gnu_asm_label_idx;
} Cn_Ast_Node_Init_Declarator;

typedef struct {
    Cn_Ast_Idx expression_idx;
    // TODO: Initializer list.
} Cn_Ast_Node_Initializer;

typedef struct {
    Cn_Ast_Idx pointer_idx;
    Cn_Ast_Idx direct_declarator_idx;
} Cn_Ast_Node_Declarator;

typedef struct {
    Cn_Qualifier_Flags qualifiers;
    Cn_Ast_Idx pointer_idx;
} Cn_Ast_Node_Pointer;

typedef enum {
    CN_AST_DIRECT_DECLARATOR_GROUPED,
    CN_AST_DIRECT_DECLARATOR_ARRAY,
    CN_AST_DIRECT_DECLARATOR_FUNCTION,
} Cn_Ast_Direct_Declarator_Kind;

typedef struct {
    Cn_Ast_Direct_Declarator_Kind kind;

    union {
        Cn_Ast_Idx declarator_idx;
        struct { Cn_Ast_Idx direct_declarator_idx; Cn_Ast_Idx expression_idx; } array;
        struct { Cn_Ast_Idx direct_declarator_idx; Cn_Ast_Idx parameter_type_list_idx; } function;
    };
} Cn_Ast_Node_Direct_Declarator;

typedef struct {
    Cn_Storage_Specifier_Flags  storage_specifiers;
    Cn_Qualifier_Flags          qualifiers;
    Cn_Function_Specifier_Flags function_specifiers;
    Cn_Ast_Linked_List          gnu_attribute_specifiers;
    Cn_Ast_Idx                  type_specifier_idx;
} Cn_Ast_Node_Declaration_Specifiers;

typedef struct {
    Cn_Ast_Idx expression_or_type_name_idx; // Either expression or type_name.
} Cn_Ast_Node_Gnu_Typeof_Specifier;

typedef struct {
    Cn_Ast_Type_Kind  kind;
    Cn_Ast_Type_Width width;
    Cn_Ast_Type_Sign  sign;
    union {
        Cn_String typedef_name;
        Cn_Ast_Idx struct_or_union_idx;
        Cn_Ast_Idx gnu_typeof_idx;
    };
} Cn_Ast_Node_Type_Specifier;

typedef struct {
    Cn_Ast_Idx specifier_qualifier_idx;
    Cn_Ast_Idx abstract_declarator_idx;
} Cn_Ast_Node_Type_Name;

typedef struct {
    Cn_Qualifier_Flags qualifiers;
    Cn_Ast_Idx         type_specifier_idx;
} Cn_Ast_Node_Specifier_Qualifier;

typedef struct {
    Cn_Ast_Linked_List parameter_declaration_list;
    bool variadic_args;
} Cn_Ast_Node_Parameter_Type_List;

typedef struct {
    Cn_Ast_Idx declaration_specifiers_idx;
    Cn_Ast_Idx declarator_idx;
} Cn_Ast_Node_Parameter_Declaration;

typedef struct {
    Cn_Ast_Idx identifier_idx;
    Cn_Ast_Linked_List member_declaration_list;
} Cn_Ast_Node_Struct_Specifier;

typedef struct {
    Cn_Ast_Idx identifier_idx;
    Cn_Ast_Linked_List member_declaration_list;
} Cn_Ast_Node_Union_Specifier;

typedef struct {
    Cn_Ast_Idx specifier_qualifier_idx;
    Cn_Ast_Linked_List member_declarator_list;
} Cn_Ast_Node_Member_Declaration;

typedef struct {
    Cn_Ast_Idx declarator_idx;
    Cn_Ast_Idx bitfield_expression_idx;
} Cn_Ast_Node_Member_Declarator;

typedef struct {
    Cn_Ast_Linked_List gnu_attribute_list;
} Cn_Ast_Node_Gnu_Attribute_Specifier;

typedef struct {
    Cn_Ast_Idx identifier_idx;
    Cn_Ast_Linked_List argument_list;
} Cn_Ast_Node_Gnu_Attribute;

typedef struct {
    Cn_Ast_Idx string_idx;
} Cn_Ast_Node_Gnu_Asm_Label;

typedef enum {
    CN_AST_NODE_IS_REPLACED     = 0x1,
    CN_AST_NODE_USE_REPLACED    = 0x2,
    CN_AST_NODE_SYNTHETIC       = 0x4,
} Cn_Ast_Node_Flags;

typedef struct cn_ast_node {
    Cn_Ast_Node_Kind kind;

    Cn_Location loc;
    Cn_Source   src;

    Cn_Ast_Node_Flags flags;

    // Replaced idx are here to track original ast nodes in case of error 
    // occuring in modified ast nodes.
    Cn_Ast_Idx replaced_idx;

    // Intrusive linked list data.
    // If node is a part of intrusive linked list this value might not be NIL.
    Cn_Ast_Idx next_idx;

    // Parent of the ast node, ast node that doesn't have parent is root of an ast branch.
    // For example translation unit doesn't have parent.
    Cn_Ast_Idx parent_idx;

    // Actual ast node data.
    union {
        Cn_Ast_Node_Code                        code;
        Cn_Ast_Node_Translation_Unit            translation_unit;
        Cn_Ast_Node_External_Declaration        external_declaration;
        Cn_Ast_Node_Declaration                 declaration;
        Cn_Ast_Node_Function_Definition         function_definition;
        // Cn_Ast_Node_Asm_Definition           asm_definition;
        Cn_Ast_Node_Compound_Statement          compound_statement;
        Cn_Ast_Node_Selection_Statement         selection_statement;
        // Cn_Ast_Node_Iteration_Statement      iteration_statement;
        Cn_Ast_Node_Jump_Statement              jump_statement;
        // Cn_Ast_Node_Labeled_Statement        labeled_statement;
        Cn_Ast_Node_Expression_Statement        expression_statement;
        Cn_Ast_Node_Binary_Expression           binary_expression;
        Cn_Ast_Node_Access_Expression           access_expression;
        Cn_Ast_Node_Function_Expression         function_expression;
        Cn_Ast_Node_Unary_Expression            unary_expression;
        Cn_Ast_Node_Cast_Expression             cast_expression;
        Cn_Ast_Node_Sizeof_Expression           sizeof_expression;
        Cn_Ast_Node_Ternary_Expression          ternary_expression;
        Cn_Ast_Node_Assignment_Expression       assignment_expression;
        Cn_Ast_Node_Postfix_Expression          postfix_expression;
        Cn_Ast_Node_Primary_Expression          primary_expression;
        Cn_Ast_Node_Identifier                  identifier;
        Cn_Ast_Node_Integer                     integer;
        Cn_Ast_Node_Float                       flt;
        Cn_Ast_Node_String                      string;
        Cn_Ast_Node_Init_Declarator_List        init_declarator_list;
        Cn_Ast_Node_Init_Declarator             init_declarator;
        Cn_Ast_Node_Initializer                 initializer;
        Cn_Ast_Node_Declarator                  declarator;
        Cn_Ast_Node_Pointer                     pointer;
        Cn_Ast_Node_Direct_Declarator           direct_declarator;
        Cn_Ast_Node_Declaration_Specifiers      declaration_specifiers;
        Cn_Ast_Node_Gnu_Typeof_Specifier        gnu_typeof_specifier;
        Cn_Ast_Node_Type_Specifier              type_specifier;
        Cn_Ast_Node_Type_Name                   type_name;
        Cn_Ast_Node_Specifier_Qualifier         specifier_qualifier;
        Cn_Ast_Node_Parameter_Type_List         parameter_type_list;
        Cn_Ast_Node_Parameter_Declaration       parameter_declaration;
        Cn_Ast_Node_Struct_Specifier            struct_specifier;
        Cn_Ast_Node_Union_Specifier             union_specifier;
        Cn_Ast_Node_Member_Declaration          member_declaration;
        Cn_Ast_Node_Member_Declarator           member_declarator;
        Cn_Ast_Node_Gnu_Attribute_Specifier     gnu_attribute_specifier;
        Cn_Ast_Node_Gnu_Attribute               gnu_attribute;
        Cn_Ast_Node_Gnu_Asm_Label               gnu_asm_label;
    };
} Cn_Ast_Node;

#ifndef CN_AST_NODE_LIST_INITIAL_CAP
#   define CN_AST_NODE_LIST_INITIAL_CAP 32
#endif // CN_AST_NODE_LIST_INITIAL_CAP

#ifndef CN_AST_TYPE_ARENA_BLOCK_CAP
#   define CN_AST_TYPE_ARENA_BLOCK_CAP (sizeof(Cn_Type) * 64)
#endif // CN_AST_TYPE_ARENA_BLOCK_CAP

#ifndef CN_AST_TYPE_PTR_SET_INITIAL_CAP
#   define CN_AST_TYPE_PTR_SET_INITIAL_CAP 32
#endif // CN_AST_TYPE_PTR_SET_INITIAL_CAP

#ifndef CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP
#   define CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP 4096
#endif // CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP

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
    // TODO: Binding enum constant.
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

#ifndef CN_AST_BINDING_LIST_INITIAL_CAP
#   define CN_AST_BINDING_LIST_INITIAL_CAP 64
#endif // CN_AST_BINDING_LIST_INITIAL_CAP

#ifndef CN_AST_TAG_BINDING_TABLE_INITIAL_CAP
#   define CN_AST_TAG_BINDING_TABLE_INITIAL_CAP 16
#endif // CN_AST_TAG_BINDING_TABLE_INITIAL_CAP

#ifndef CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP
#   define CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP 16
#endif // CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP

#ifndef CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP
#   define CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP 4096
#endif // CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP

#ifndef CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP
#   define CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP 4096
#endif // CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP

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

#ifndef CN_AST_SCOPE_STACK_INITIAL_CAP
#   define CN_AST_SCOPE_STACK_INITIAL_CAP 64
#endif // CN_AST_SCOPE_STACK_INITIAL_CAP

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
     * Error and warning count, is incremented by cn_diagnostic,
     * every time error or warning is reported.
     */
    int64_t error_count, warning_count;
    uint64_t counter;
} Cn_Ast_Data;

extern Cn_Ast_Data *cn__ast_data;

typedef enum {
    CN_AST_CHECKPOINT_IGNORE_AST_NODES = 0x01,
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
 * Simple macro to get ast node based on its idx.
 * It is abstracted away same way as append, if for some reason 
 * underlying implementaion will change in the future.
 */
CNDEF Cn_Ast_Node *cn_ast_node_get(Cn_Ast_Idx idx);

/**
 * Simple macro to get ast idx based on pointer to the node.
 */
#define cn_ast_idx_get(node)    ((Cn_Ast_Node *)(node) - cn__ast_data->node_list)

/**
 * Sets parent of all idx's supplied to parent_idx.
 */
#define cn_ast_node_set_parent(parent_idx, ...) cn__ast_node_set_parent(parent_idx, (Cn_Ast_Idx []) { __VA_ARGS__ }, sizeof (Cn_Ast_Idx []) { __VA_ARGS__ })

CNDEF void cn__ast_node_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_Idx idxs[], size_t length);

/**
 * Appends specified node to the cn_ast_node_list. 
 * It doesn't add or change nodes parent.
 *
 * RETURNS: Cn_Ast_Idx of where the node was inserted.
 */
CNDEF Cn_Ast_Idx cn_ast_node_list_append(Cn_Ast_Node node);

/**
 * This is a simple macro that unwrawps into for loop, where
 * 'it' is each node in the intrusive list of nodes.
 * list is pointer to the Cn_Ast_Linked_List.
 */
#define cn_ast_linked_list_foreach(it, list) for (Cn_Ast_Node *it = cn_ast_node_get((list)->first_idx); it != cn_ast_node_get(CN_AST_NIL_IDX); it = cn_ast_node_get(it->next_idx))

/**
 * This is a simple macro that unwrawps into for loop, where
 * 'it' is each node idx in the intrusive list of nodes.
 * list is pointer to the Cn_Ast_Linked_List.
 */
#define cn_ast_linked_list_foreach_idx(it, list) for (Cn_Ast_Idx it = (list)->first_idx; it != CN_AST_NIL_IDX; it = cn_ast_node_get(it)->next_idx)

CNDEF void cn_ast_linked_list_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_Linked_List *list);

/**
 * Adds next node to the specified intrusive list of nodes. 
 * it will add node to the end of the linked list of node(s) stored there.
 * It works because every node can be part of intrusive list.
 */
CNDEF void cn_ast_linked_list_add(Cn_Ast_Linked_List *list, Cn_Ast_Idx next);

/**
 * Recursivly prints ast tree to stdout.
 * IMPORTANT: Use starting depth as 0.
 */
CNDEF void cn_ast_print(Cn_Ast_Node *node, int depth);

/**
 * RETURNS: Static string with the enum name of the node kind. Used in messages.
 */
CNDEF const char *cn_ast_node_kind_name(Cn_Ast_Node_Kind kind);

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
CNDEF Cn_Ast_Binding_Idx cn_ast_function_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Storage_Specifier_Flags storage_flags, Cn_Function_Specifier_Flags function_flags, Cn_Type *type, Cn_Ast_Linked_List *parameter_declaration_list, bool is_definition);

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
 *              | asm_definition            // GCC
 *              | function_definition
 *              | declaration
 *              )
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_external_declaration(Cn_Lexer *lexer);

/**
 * Following function traverses declarator nodes to get declarator identifier.
 *
 * OUTPUTS: Last declarator idx in chain.
 */
CNDEF void cn_get_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *last_declarator_idx);

/**
 * Following function traverses declarator nodes to get function declarator identifier.
 * And other information.
 *
 * OUTPUTS: Parameter type list idx of the function, if supplied value is not NULL. 
 * OUTPUTS: Last declarator idx in chain.
 *
 * RETURNS: True on success, false if declarator is not a proper function.
 */
CNDEF bool cn_get_function_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *parameter_type_list_idx, Cn_Ast_Idx *last_declarator_idx);

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
 *          : declaration_specifiers init_declarator_list? gnu_attribute_specifier_sequence ';'
 *          ;
 *
 *  function_definition
 *          : declaration_specifiers declarator compound_statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_function_definition_or_declaration(Cn_Lexer *lexer);

/**
 * Analyzes already constructed ast nodes as declaration.
 *
 * RETURNS: True on success, false if any errors occured.
 */
CNDEF bool cn_ast_analyze_declaration(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_Idx init_declarator_list_idx);

/**
 * Analyzes already constructed ast nodes as function definition.
 *
 * OUTPUTS: Function binding on success.
 *
 * RETURNS: True on success, false if any errors occured.
 */
CNDEF bool cn_ast_analyze_function_definition(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_Idx declarator_idx, Cn_Ast_Binding_Idx *function_binding_idx);

/**
 * Binds function definition params to the current scope.
 *
 * RETURNS: True on success, false if any errors occured.
 */
CNDEF bool cn_ast_bind_function_definition_params(Cn_Ast_Binding_Idx function_binding_idx);

/**
 * Parses code starting of with lexer current token as asm defintion.
 *
 *  asm_definition
 *          : simple_asm_expression
 *          | asm '(' top_level_asm_argument ')'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_asm_definition(Cn_Lexer *lexer);

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
 * Parses code starting of with lexer current token as compound statement.
 *
 * IMPORTANT: Compound statement introduces new scope, 
 * but this can be controlled by setting use current scope to true.
 * By doing so, compound statement won't introduce new scope 
 * and everything will be defined and declared in current scope instead of a new one.
 *
 *  compound_statement
 *          : '{' (statement | declaration)* '}'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_compound_statement(Cn_Lexer *lexer, bool use_current_scope);

/**
 * Parses code starting of with lexer current token as selection_statement
 *
 *  selection_statement
 *          : 'if' '(' expression ')' statement ('else' statement)?
 *          | 'switch' '(' expression ')' statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_selection_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as iteration_statement
 *
 *  iteration_statement
 *          : TODO: Iteration statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_iteration_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as jump_statement
 *
 *  jump_statement
 *          : ( 
 *              'goto' identifier
 *              | 'continue'
 *              | 'break'
 *              | 'return' expression?
 *          ) ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_jump_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as labeled statement.
 *
 *  labeled_statement
 *          : TODO: Labeled statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_labeled_statement(Cn_Lexer *lexer);

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
CNDEF Cn_Ast_Linked_List cn_ast_parse_argument_list(Cn_Lexer *lexer, bool *ok);

/**
 *  C OPERATOR PRECEDENCE TABLE highest (14) to lowest (0)
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
 *  0                               Associativity: Left-to-right
 *  ---------------------------------------------------------------
 *    ,             Comma
 */


/**
 * Parses code starting of with lexer current token as expression.
 * Utilizing following precedence table as guide to properly structure AST nodes in the right order.
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
 *          | primary_expression
 *          ;
 *
 * IMPORTANT: Naturally this function constructs right leaning tree on binary expressions, based of precedence it will 
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
 * This function constructs right leaning tree if next binary operator is increaseing precedence, 
 * if not it just returns left leaf idx.
 * It recursivly calls parse expression to get right leaf idx.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_increasing_precedence(Cn_Lexer *lexer, Cn_Ast_Idx left_idx, int min_precedence, Cn_Expression_Parsing_Flags flags);

/**
 * This first parses left leaf, and then calls increasing precedence parsing, until it returns left leaf.
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
 *          | compound_literal
 *          ;
 *
 *  compound_literal
 *          : '(' type_name ')' '{' TODO: ... '}'
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
 * Parses non-binary leaf expression, it will detect and parse unary operators, 
 * but it doesn't parse binary precedent expressions or postfix expressions.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_expression_leaf(Cn_Lexer *lexer, Cn_Expression_Parsing_Flags flags);

/**
 * Continues parsing code starting of with lexer current token as init declarator list, taking supplied declarator and finishing it as first init declarator.
 *
 *  init_declarator_list
 *          : init_declarator (',' init_declarator)*
 *          ;
 *
 */
CNDEF Cn_Ast_Idx cn_ast_continue_init_declarator_list(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx);

/**
 * Parses code starting of with lexer current token as init declarator list.
 *
 *  init_declarator_list
 *          : init_declarator (',' init_declarator)*
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator_list(Cn_Lexer *lexer);

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
 *          | '{' initializer_list ','? '}'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as declarator.
 *
 *  abstract_declarator
 *          : declarator
 *          ;
 *
 *  declarator
 *          : pointer? direct_declarator
 *          ;
 *
 * NOTE: This function can return ast node with type ABSTRACT_DECLARATOR, 
 * if there is no direct declarator identifier found.
 */
CNDEF Cn_Ast_Idx cn_ast_parse_declarator(Cn_Lexer *lexer);

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
CNDEF Cn_Ast_Idx cn_ast_parse_gnu_typeof_specifier(Cn_Lexer *lexer);

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
 *          | struct_specifier
 *          | union_specifier
 *          | enum_specifier
 *          | typedef
 *          | gnu_typeof_specifier
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_try_parse_type_specifier(Cn_Lexer *lexer, Cn_Ast_Idx output_idx);

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
 *          : gnu_attribute_specifier_sequence (qualifier | type_specifier)+
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
CNDEF Cn_Ast_Idx cn_ast_parse_parameter_type_list(Cn_Lexer *lexer);

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
 *          : 'struct' gnu_attribute_specifier_sequence
 *          ( identifier? '{' member_declaration* '}' ) 
 *          | identifier
 *          ;
 *
 *  union_specifier
 *          : 'union' gnu_attribute_specifier_sequence
 *          ( identifier? '{' member_declaration* '}' ) 
 *          | identifier
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_struct_or_union_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as member declaration.
 *
 *  member_declaration
 *          : specifier_qualifier member_declarator_list ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_member_declaration(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as member declarator list.
 *
 *  member_declarator_list
 *          : member_declarator (',' member_declarator)*
 *          ;
 *
 * RETURNS: 0 on success, -1 if error occured.
 */
CNDEF int cn_ast_parse_member_declarator_list(Cn_Lexer *lexer, Cn_Ast_Linked_List *member_declarator_list);

/**
 * Parses code starting of with lexer current token as member declarator.
 *
 *  member_declarator
 *          : declarator gnu_attribute_specifier_sequence
 *          | declarator? ':' expression gnu_attribute_specifier_sequence
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_member_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as attribute specifier sequence.
 *
 * attribute_specifier_sequence
 *          : attribute_specifier+
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_attribute_specifier_sequence(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as GNU attribute specifier sequence.
 *
 * gnu_attribute_specifier_sequence
 *          : gnu_attribute_specifier*
 *          ;
 */
CNDEF Cn_Ast_Linked_List cn_ast_parse_gnu_attribute_specifier_sequence(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as GNU attribute specifier.
 *
 * gnu_attribute_specifier
 *          : '__attribute__' '(' '(' gnu_attribute_list ')' ')'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as GNU attribute list.
 *
 * gnu_attribute_list
 *          : gnu_attribute (',' gnu_attribute)*
 *          ;
 */
CNDEF Cn_Ast_Linked_List cn_ast_parse_gnu_attribute_list(Cn_Lexer *lexer, bool *ok);

/**
 * Parses code starting of with lexer current token as GNU attribute.
 *
 * gnu_attribute
 *          : identifier
 *          | identifier '(' argument_list ')'
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
 * Reparse given ast node as translation unit.
 *
 * IMPORTANT: Analysis funtions perfom:
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
CNDEF bool cn_ast_reparse_compound_statement(Cn_Ast_Idx node_idx, bool use_current_scope);

/**
 * Reparse given ast node as selection statement.
 */
CNDEF bool cn_ast_reparse_selection_statement(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as iteration statement.
 */
CNDEF bool cn_ast_reparse_iteration_statement(Cn_Ast_Idx node_idx);

/**
 * Reparse given ast node as jump statement.
 */
CNDEF bool cn_ast_reparse_jump_statement(Cn_Ast_Idx node_idx);

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
 * Reparse given ast node as init declarator list.
 */
CNDEF bool cn_ast_reparse_init_declarator_list(Cn_Ast_Idx node_idx);

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
CNDEF bool cn_ast_reparse_parameter_type_list(Cn_Ast_Idx node_idx);

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
    CN__DC_COUNT,
} Cn_Diagnostic_Code;

extern const Cn_String CN_DIAGNOSTIC_CODES[CN__DC_COUNT];

typedef enum {
    CN_DIAGNOSTIC_INFO,
    CN_DIAGNOSTIC_WARNING,
    CN_DIAGNOSTIC_ERROR,
} Cn_Diagnostic_Level;

extern Cn_Diagnostic_Level cn_min_diagnostic_level;

typedef void (Cn_Diagnostic_Handler)(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, const char *format, va_list args);

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
 * Inserts new node into linked list, based of specified list_node.
 * Just like cn_replace this function tracks node 
 * changes and guarantees proper diagnostics.
 * If *list_node is NIL, the function will just assume its 
 * empty linked list and insert in the position of list_node. 
 */
CNDEF void cn_linked_list_insert(Cn_Ast_Idx *list_node, Cn_Ast_Idx new, Cn_Ast_Idx parent_idx);

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
 * Builds linked list out of supplied nodes.
 *
 * Accepts variadic array of nodes in order to be inserted into 
 * linked list.
 *
 * IMPORTANT: If supplied nodes were members of different 
 * linked list their references there will be corrupted.
 * Warning log will appear if this function detects such case.
 *
 * This function is variadic by nature so it doesn't accept 
 * build options through macro, since it would break other 
 * variadic args. But luckily it doens't need to. 
 *
 * RETURNS: Linekd list composed of supplied nodes.
 */
#define cn_build_linked_list(...) cn__build_linked_list((Cn_Ast_Idx[]) { __VA_ARGS__ }, sizeof((Cn_Ast_Idx[]) { __VA_ARGS__ }) / sizeof(Cn_Ast_Idx), __FILE__, __LINE__)

CNDEF Cn_Ast_Linked_List cn__build_linked_list(Cn_Ast_Idx members[], int64_t length, const char *file, int64_t line);

/**
 * Builds identifier with supplied name.
 * 
 * RETURNS: Built identifier.
 */
#define cn_build_identifier(name, ...) cn__build_identifier(name, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_identifier(Cn_String name, Cn_Build_Opt opt);

/**
 * Builds integer with supplied value.
 *
 * RETURNS: Built integer.
 */
#define cn_build_integer(value, ...) cn__build_integer(value, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_integer(Cn_String value, Cn_Build_Opt opt);

/**
 * Builds float with supplied value.
 * 
 * RETURNS: Built float.
 */
#define cn_build_float(value, ...) cn__build_float(value, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_float(Cn_String value, Cn_Build_Opt opt);

/**
 * Builds string with supplied str.
 * 
 * RETURNS: Built string.
 */
#define cn_build_string(str, ...) cn__build_string(str, (Cn_Build_Opt) { .alloc = CN_BUILD_OPT_DEFAULT_ALLOC, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_string(Cn_String str, Cn_Build_Opt opt);

/**
 * Builds binary expression, left and right params must be 
 * valid built node idx's.
 * 
 * RETURNS: Built binary expression.
 */
#define cn_build_binary(op, left, right, ...) cn__build_binary(op, left, right, (Cn_Build_Opt) { __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_binary(Cn_Binary_Operator_Kind op, Cn_Ast_Idx left, Cn_Ast_Idx right, Cn_Build_Opt opt);

/**
 * Builds unary expression, expression must be 
 * valid built node idx.
 * 
 * RETURNS: Built unary expression.
 */
#define cn_build_unary(op, expression, ...) cn__build_unary(op, expression, (Cn_Build_Opt) { __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_unary(Cn_Unary_Operator_Kind op, Cn_Ast_Idx expression, Cn_Build_Opt opt);

/**
 * Builds function call expression, callee must be 
 * valid built node idx. And arg_list should be constructed beforehand 
 * with appropriate expression children.
 *
 * NOTE: arg_list can be NIL, to represent no args.
 * 
 * RETURNS: Built function call expression.
 */
#define cn_build_func_call(callee, arg_list, ...) cn__build_func_call(callee, arg_list, (Cn_Build_Opt) { __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_func_call(Cn_Ast_Idx callee, Cn_Ast_Linked_List arg_list, Cn_Build_Opt opt);

/**
 * Builds expression statement, expression must be 
 * valid built node idx.
 * 
 * RETURNS: Built expression statement.
 */
#define cn_build_expr_statement(expression, ...) cn__build_expr_statement(expression, (Cn_Build_Opt) { __VA_ARGS__ })

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
 * 
 * TODO: Resolve reaching EOF, token corruption bug.
 * Found this bug in tests/lexer_tokens.c
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
    .flags = CN_TYPE_COMPLETE,
    .size = sizeof(int), 
    .align = sizeof(int), 
    .kind = CN_INTEGER,
    .t_integer.is_signed = true,
};

const Cn_Type CN_TYPE_FLOAT = { 
    .flags = CN_TYPE_COMPLETE,
    .size = sizeof(float), 
    .align = sizeof(float), 
    .kind = CN_FLOAT,
};

const Cn_Type CN_TYPE_CHAR = { 
    .flags = CN_TYPE_COMPLETE,
    .size = sizeof(char), 
    .align = sizeof(char), 
    .kind = CN_INTEGER,
    .t_integer.is_signed = true,
};

const Cn_Type CN_TYPE_VOID = { 
    .size = 0, 
    .align = 0, 
    .kind = CN_VOID,
};

const Cn_Type CN_TYPE_PTRDIFF = { 
    .flags = CN_TYPE_COMPLETE,
    .size = sizeof(ptrdiff_t), 
    .align = sizeof(ptrdiff_t), 
    .kind = CN_INTEGER,
    .t_integer.is_signed = true,
};

const Cn_Type CN_TYPE_SIZE = {
    .flags = CN_TYPE_COMPLETE,
    .size  = sizeof(size_t),
    .align = sizeof(size_t),
    .kind  = CN_INTEGER,
    .t_integer.is_signed = false,   // size_t is unsigned.
};

const Cn_Type CN_TYPE_OPAQUE = {
    .kind = CN_OPAQUE,
};

CNDEF Cn_Type cn_type_make_pointer(const Cn_Type *type) {
    return (Cn_Type) {
        .flags = CN_TYPE_COMPLETE,
        .size  = sizeof(void *),
        .align = sizeof(void *),
        .kind = CN_POINTER,
        .t_pointer.ptr_to = (Cn_Type *)type,
    };
}

CNDEF Cn_Type cn_type_make_qualified(Cn_Type_Qualified_Flags flags, const Cn_Type *type) {
    return (Cn_Type) {
        .kind = CN_QUALIFIED,
        .t_qualified.flags = flags,
        .t_qualified.base_type = (Cn_Type *)type,
    };
}

CNDEF bool cn_type_equals(const Cn_Type *a, const Cn_Type *b) {
    if (a == b) return true;

    if (a->kind != b->kind) return false;
    if (a->flags != b->flags) return false;
    if (a->size != b->size && a->align != b->align) return false;
     
    switch (a->kind) {
        case CN_INTEGER:
            if (a->t_integer.is_signed != b->t_integer.is_signed) return false;
            break;
        case CN_POINTER:
            if (a->t_pointer.ptr_to != b->t_pointer.ptr_to) return false;
            break;
        case CN_FUNCTION:
            if (a->t_function.return_type != b->t_function.return_type) return false;
            if (a->t_function.params_length != b->t_function.params_length) return false;
            for (int64_t i = 0; i < a->t_function.params_length; i++) {
                if (a->t_function.params[i].type != b->t_function.params[i].type) return false;
            }
            break;
        case CN_ARRAY:
            if (a->t_array.element_type != b->t_array.element_type) return false;
            if (a->t_array.length != b->t_array.length) return false;
            break;
        case CN_STRUCT:
            if (!cn_str_equals(&a->t_struct.tag, &b->t_struct.tag)) return false;
            if (a->t_struct.members_length != b->t_struct.members_length) return false;
            for (int64_t i = 0; i < a->t_struct.members_length; i++) {
                if (a->t_struct.members[i].type != b->t_struct.members[i].type) return false;
                if (!cn_str_equals(&a->t_struct.members[i].name, &b->t_struct.members[i].name)) return false;
            }
            break;
        case CN_ENUM:
            break;
        case CN_UNION:
            break;
        case CN_QUALIFIED:
            if (a->flags != b->flags) return false;

            if (a->t_qualified.base_type != b->t_qualified.base_type) return false;
            break;
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
        case CN_INTEGER:
            hash = cn_hash_mix(hash, cn_hash_u64(type->t_integer.is_signed));
            break;
        case CN_POINTER:
            hash = cn_hash_mix(hash, cn_hash_ptr(type->t_pointer.ptr_to));
            break;
        case CN_FUNCTION:
            hash = cn_hash_mix(hash, cn_hash_ptr(type->t_function.return_type));
            hash = cn_hash_mix(hash, cn_hash_u64(type->t_function.params_length));
            for (int64_t i = 0; i < type->t_function.params_length; i++) {
                hash = cn_hash_mix(hash, cn_hash_ptr(type->t_function.params[i].type));
            }
            break;
        case CN_ARRAY:
            hash = cn_hash_mix(hash, cn_hash_ptr(type->t_array.element_type));
            hash = cn_hash_mix(hash, cn_hash_u64(type->t_array.length));
            break;
        case CN_STRUCT:
            hash = cn_hash_mix(hash, cn_str_hash(&type->t_struct.tag));
            hash = cn_hash_mix(hash, cn_hash_u64(type->t_struct.members_length));
            for (int64_t i = 0; i < type->t_struct.members_length; i++) {
                hash = cn_hash_mix(hash, cn_hash_ptr(type->t_struct.members[i].type));
            }
            break;
        case CN_ENUM:
            break;
        case CN_UNION:
            break;
        case CN_QUALIFIED:
            hash = cn_hash_mix(hash, cn_hash_u64(type->t_qualified.flags));
            hash = cn_hash_mix(hash, cn_hash_ptr(type->t_qualified.base_type));
            break;
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

        case CN_INTEGER: 
            {
                if (!type->t_integer.is_signed) cn_sb_append_str(left, CN_CSTR("unsigned "));
                switch (type->size) {
                    case 1:  cn_sb_append_str(left, CN_CSTR("char"));      return;
                    case 2:  cn_sb_append_str(left, CN_CSTR("short"));     return;

                    case 4:  cn_sb_append_str(left, CN_CSTR("int"));       return;
                    case 8:  cn_sb_append_str(left, CN_CSTR("long long")); return;
                    default: cn_sb_append_format(left, "int%lld_t", type->size * 8); return;
                }
            }

        case CN_FLOAT: 
            {
                switch (type->size) {
                    case 4:  cn_sb_append_str(left, CN_CSTR("float"));       return;
                    case 8:  cn_sb_append_str(left, CN_CSTR("double"));      return;
                    case 16: cn_sb_append_str(left, CN_CSTR("long double")); return;
                    default: cn_sb_append_format(left, "float%lld", type->size * 8); return;
                }
            }

        case CN_STRUCT: 
            {
                cn_sb_append_str(left, CN_CSTR("struct "));
                cn_sb_append_str(left, type->t_struct.tag);
                return;
            }

        case CN_UNION: 
            {
                cn_sb_append_str(left, CN_CSTR("union "));
                cn_sb_append_str(left, type->t_struct.tag);
                return;
            }

        case CN_ENUM: 
            {
                cn_sb_append_str(left, CN_CSTR("enum "));
                CN_TODO("enum stringify.");
                return;
            }

        case CN_POINTER: 
            {
                Cn_Type *inner = type->t_pointer.ptr_to;
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

        case CN_ARRAY: 
            {
                cn__type_stringify(type->t_array.element_type, left, right);
                if (type->t_array.length >= 0)
                    cn_sb_append_format(right, "[%lld]", type->t_array.length);
                else
                    cn_sb_append_str(right, CN_CSTR("[]"));
                return;
            }

        case CN_FUNCTION: 
            {
                cn__type_stringify(type->t_function.return_type, left, right);

                cn_sb_append_char(right, '(');
                if (type->t_function.params_length == 0) {
                    cn_sb_append_str(right, CN_CSTR("void"));
                } else {
                    for (int64_t i = 0; i < type->t_function.params_length; i++) {
                        if (i > 0) cn_sb_append_str(right, CN_CSTR(", "));

                        Cn_String_Builder param_left  = cn_sb_make(32);
                        Cn_String_Builder param_right = cn_sb_make(32);

                        cn__type_stringify(type->t_function.params[i].type, &param_left, &param_right);

                        cn_sb_append_str(right, cn_sb_to_str(&param_left));
                        cn_sb_append_str(right, cn_sb_to_str(&param_right));

                        cn_sb_free(&param_left);
                        cn_sb_free(&param_right);
                    }
                }
                cn_sb_append_char(right, ')');
                return;
            }

        case CN_QUALIFIED:
            {
                if (type->t_qualified.base_type->kind == CN_POINTER) {
                    cn__type_stringify(type->t_qualified.base_type, left, right);
                    cn_sb_append_char(left, ' ');
                }

                if (type->t_qualified.flags & CN_TYPE_CONSTANT) 
                    cn_sb_append_str(left, CN_CSTR("const "));
                if (type->t_qualified.flags & CN_TYPE_VOLATILE) 
                    cn_sb_append_str(left, CN_CSTR("volatile "));
                if (type->t_qualified.flags & CN_TYPE_RESTRICT) 
                    cn_sb_append_str(left, CN_CSTR("restrict "));
                if (type->t_qualified.flags & CN_TYPE_ATOMIC) 
                    cn_sb_append_str(left, CN_CSTR("_Atomic "));

                if (type->t_qualified.base_type->kind != CN_POINTER) {
                    cn__type_stringify(type->t_qualified.base_type, left, right);
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
    return type->kind == CN_QUALIFIED && type->t_qualified.flags & CN_TYPE_CONSTANT;
}

CNDEF Cn_Type *cn_type_unqualified(const Cn_Type *type) {
    Cn_Type *t = (Cn_Type *)type;
    while (t->kind == CN_QUALIFIED) {
        t = type->t_qualified.base_type;
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
        // Void pointers can be assigned in either direction without explicit casting.
        if (to->t_pointer.ptr_to->kind == CN_VOID || from->t_pointer.ptr_to->kind == CN_VOID) return true;

        return cn_type_is_compatible_no_qualifiers(to->t_pointer.ptr_to, from->t_pointer.ptr_to);
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

    // Int even rank:
    return (Cn_Type *)(!a->t_integer.is_signed ? a : b);
}

CNDEF bool cn_type_is_scalar(Cn_Type *type) {
    return cn_type_is_arithmetic(type) || type->kind == CN_POINTER || type->kind == CN_BOOL;
}

CNDEF int64_t cn_any_read_int(Cn_Any any) {
    Cn_Type *type = cn_type_unqualified(any.type);
    int64_t size = type->size;
    bool is_signed = (type->kind == CN_INTEGER) && type->t_integer.is_signed;
 
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
    if (type->kind == CN_INTEGER && !type->t_integer.is_signed) {
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

CNDEF void cn_ast_next_token(Cn_Lexer *lexer) {
next_token:
    cn_lexer_next_token(lexer);

    for (uint64_t i = 0; i < CN_ARRAY_LENGTH(CN_TOKEN_BLACKLIST); i++) {
        if (cn_lexer_token(lexer).type == CN_TOKEN_BLACKLIST[i]) {
            goto next_token; // Skips token if current is in the blacklist.
        }
    }
}

CNDEF Cn_Token cn_ast_peek(Cn_Lexer lexer) {
next_token:
    cn_lexer_next_token(&lexer);

    for (uint64_t i = 0; i < CN_ARRAY_LENGTH(CN_TOKEN_BLACKLIST); i++) {
        if (cn_lexer_token(&lexer).type == CN_TOKEN_BLACKLIST[i]) {
            goto next_token; // Skips token if current is in the blacklist.
        }
    }

    return cn_lexer_token(&lexer);
}

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

        Cn_Ast_Node err = { .kind = CN_AST_NODE_ERROR };

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

    data->warning_count = 0;
    data->error_count = 0;

    cn__ast_data = data;

    return 0;
}

CNDEF void cn_ast_free(Cn_Ast_Data *data) {
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

CNDEF Cn_Ast_Node *cn_ast_node_get(Cn_Ast_Idx idx) {
    if (((cn__ast_data->node_list + idx)->flags & CN_AST_NODE_USE_REPLACED) && (cn__ast_data->node_list + idx)->replaced_idx != CN_AST_NIL_IDX) {
        return cn__ast_data->node_list + (cn__ast_data->node_list + idx)->replaced_idx;
    }

    return cn__ast_data->node_list + idx;
}

CNDEF void cn__ast_node_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_Idx idxs[], size_t length) {
    // Ignores alternate routing that cn_ast_node_get gurantees on replace.
    for (size_t i = 0; i < length; i++) 
        if (idxs[i] != CN_AST_NIL_IDX)
            (cn__ast_data->node_list + idxs[i])->parent_idx = parent_idx;
}

CNDEF Cn_Ast_Idx cn_ast_node_list_append(Cn_Ast_Node node) {
    cn_array_list_append(&cn__ast_data->node_list, node);
    return cn_array_list_length(&cn__ast_data->node_list) - 1;
}

CNDEF void cn_ast_linked_list_set_parent(Cn_Ast_Idx parent_idx, Cn_Ast_Linked_List *list) {
    cn_ast_linked_list_foreach(it, list) it->parent_idx = parent_idx;
}

CNDEF void cn_ast_linked_list_add(Cn_Ast_Linked_List *list, Cn_Ast_Idx next_idx) {
    if (list->first_idx == CN_AST_NIL_IDX) {
        list->first_idx = next_idx;
        list->last_idx = next_idx;
    } else {
        cn_ast_node_get(list->last_idx)->next_idx = next_idx;
        list->last_idx = next_idx;
    }
    list->length++;
}

#define CN__ENUM_PRINT_CASE(enum_name)\
    case enum_name: {\
        printf("%s", #enum_name);\
        break;\
        }\

CNDEF void cn__ast_print_kind(Cn_Ast_Node_Kind kind) {
    switch(kind) {
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ERROR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_TRANSLATION_UNIT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_EXTERNAL_DECLARATION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DECLARATION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_FUNCTION_DEFINITION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ASM_DEFINITION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_COMPOUND_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_SELECTION_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ITERATION_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_JUMP_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_LABELED_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_EXPRESSION_STATEMENT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_BINARY_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ACCESS_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_FUNCTION_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_UNARY_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_CAST_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_SIZEOF_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_TERNARY_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ASSIGNMENT_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_POSTFIX_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_PRIMARY_EXPRESSION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_IDENTIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INTEGER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_FLOAT);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_STRING);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INIT_DECLARATOR_LIST);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INIT_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_INITIALIZER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_ABSTRACT_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_POINTER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DIRECT_DECLARATOR);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_DECLARATION_SPECIFIERS);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_TYPE_SPECIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_TYPE_NAME);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_SPECIFIER_QUALIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_PARAMETER_TYPE_LIST);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_PARAMETER_DECLARATION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_STRUCT_SPECIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_UNION_SPECIFIER);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_MEMBER_DECLARATION);
        CN__ENUM_PRINT_CASE(CN_AST_NODE_MEMBER_DECLARATOR);

        CN__ENUM_PRINT_CASE(CN_AST_NODE_UNKNOWN);
        default: 
            printf("??");
            break;
    }
}

const char *cn_ast_print_prefixes[64] = {0};

const char *CN_AST_PRINT_EMPTY_TAB = "    ";
const char *CN_AST_PRINT_FLAT_TAB  = "│   ";
const char *CN_AST_PRINT_SPLIT_TAB = "├── ";
const char *CN_AST_PRINT_LAST_TAB  = "└── ";

#define CN__AST_PRINT_TABS(depth, prefix) for (int i = 0; i < depth; i++) { printf("%s", cn_ast_print_prefixes[i]); } printf("%s", prefix)

#define LAST() do { cn_ast_print_prefixes[depth] = CN_AST_PRINT_EMPTY_TAB; CN__AST_PRINT_TABS(depth, CN_AST_PRINT_LAST_TAB); } while(0)
#define SPLIT() do { CN__AST_PRINT_TABS(depth, CN_AST_PRINT_SPLIT_TAB); } while(0)

CNDEF void cn__ast_print_list(Cn_Ast_Node *node, int depth) {
    if (node == cn_ast_node_get(CN_AST_NIL_IDX)) {
        printf("LINKED_LIST empty\n");
        return;
    }

    printf("LINKED_LIST\n");

    cn_ast_print_prefixes[depth] = CN_AST_PRINT_FLAT_TAB;

    while (true) {
        if (node->next_idx == CN_AST_NIL_IDX) {
            LAST();
            cn_ast_print(node, depth + 1);
            break;
        }
        SPLIT();
        cn_ast_print(node, depth + 1);
        node = cn_ast_node_get(node->next_idx);
    }

    cn_ast_print_prefixes[depth] = NULL;
}

CNDEF void cn_ast_print(Cn_Ast_Node *node, int depth) {
    CN_ASSERT(depth < (int)CN_ARRAY_LENGTH(cn_ast_print_prefixes));

    cn__ast_print_kind(node->kind);
    cn_ast_print_prefixes[depth] = CN_AST_PRINT_FLAT_TAB;
    
    int next = 0;
    Cn_Ast_Idx idxs[4] = {0};
    bool is_list[4]    = {0};

#define ADD_IDX(idx) do { if (idx != CN_AST_NIL_IDX) { CN_ASSERT(next < (int)CN_ARRAY_LENGTH(idxs)); idxs[next++] = idx; } } while(0)

#define ADD_LIST(list) do { CN_ASSERT(next < (int)CN_ARRAY_LENGTH(idxs)); idxs[next] = (list)->first_idx; is_list[next++] = true; } while(0)
    
    // Printing info about specific nodes.
    switch(node->kind) {
        case CN_AST_NODE_TRANSLATION_UNIT:
            {
                printf("\n");

                ADD_LIST(&node->translation_unit.external_declaration_list);
                break;
            }
        case CN_AST_NODE_EXTERNAL_DECLARATION:
            {
                if (node->external_declaration.extension) {
                    printf(" '__extension__'");
                }

                if (node->external_declaration.child_idx == CN_AST_NIL_IDX) {
                    printf(" stray ';'\n");
                } else {
                    printf("\n");
                    ADD_IDX(node->external_declaration.child_idx);
                }
                break;
            }
        case CN_AST_NODE_DECLARATION:
            {
                printf("\n");
                ADD_IDX(node->declaration.declaration_specifiers_idx);
                ADD_IDX(node->declaration.init_declarator_list_idx);
                break;
            }
        case CN_AST_NODE_FUNCTION_DEFINITION:
            {
                printf("\n");
                ADD_IDX(node->function_definition.declaration_specifiers_idx);
                ADD_IDX(node->function_definition.declarator_idx);
                ADD_IDX(node->function_definition.compound_statement_idx);
                break;
            }
        case CN_AST_NODE_COMPOUND_STATEMENT:
            {
                printf("\n");
                ADD_LIST(&node->compound_statement.statement_or_declaration_list);
                break;
            }
        case CN_AST_NODE_SELECTION_STATEMENT:
            {   
                ADD_IDX(node->selection_statement.condition_expression_idx);
                ADD_IDX(node->selection_statement.statement_idx);

                if (node->selection_statement.kind == CN_AST_SELECTION_IF) {
                    printf(" kind: if\n");
                    ADD_IDX(node->selection_statement.else_statement_idx);
                }
                else {
                    printf(" kind: switch\n");
                }

                break;
            }
        case CN_AST_NODE_JUMP_STATEMENT:
            {
                printf(" kind: ");
                switch(node->jump_statement.kind) {
                    case CN_AST_JUMP_GOTO:
                        printf("goto ");
                        ADD_IDX(node->jump_statement.identifier_idx);
                        break;
                    case CN_AST_JUMP_CONTINUE:
                        printf("continue\n");
                        break;
                    case CN_AST_JUMP_BREAK:
                        printf("break\n");
                        break;
                    case CN_AST_JUMP_RETURN:
                        printf("return\n");
                        ADD_IDX(node->jump_statement.expression_idx);
                        break;
                }
                
            }
            break;
        case CN_AST_NODE_EXPRESSION_STATEMENT:
            {
                printf("\n");
                ADD_IDX(node->expression_statement.expression_idx);
                break;
            }
        case CN_AST_NODE_BINARY_EXPRESSION:
            {
                printf(" operator_kind: ");
                switch(node->binary_expression.operator_kind) {
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_ARRAY_SUB);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_MULTIPLICATION);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_DIVISION);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_MODULO);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_ADDITION);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_SUBTRACTION);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_LSHIFT);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_RSHIFT);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_LESS);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_LESS_EQ);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_GREATER);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_GREATER_EQ);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_EQ);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_NOT_EQ);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_BIT_AND);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_BIT_XOR);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_BIT_OR);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_AND);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_OR);
                    CN__ENUM_PRINT_CASE(CN_BINARY_OP_COMMA);
                    default: 
                        printf("??");
                        break;
                }
                printf("\n");
                ADD_IDX(node->binary_expression.left_expression_idx);
                ADD_IDX(node->binary_expression.right_expression_idx);
                break;
            }
        case CN_AST_NODE_ACCESS_EXPRESSION: 
            {
                if (node->access_expression.pointer) {
                    printf(" kind: '.'");
                } else {
                    printf(" kind: '->'"); 
                }
                printf("\n");
                ADD_IDX(node->access_expression.identifier_idx);
                ADD_IDX(node->access_expression.expression_idx);
                break;
            }
        case CN_AST_NODE_FUNCTION_EXPRESSION:
            {
                printf("\n");
                ADD_IDX(node->function_expression.expression_idx);
                ADD_LIST(&node->function_expression.argument_list);
                break;
            }
        case CN_AST_NODE_UNARY_EXPRESSION:
            {
                printf(" operator_kind: ");
                switch(node->unary_expression.operator_kind) {
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_INCREMENT);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_DECREMENT);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_POSITIVE);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_NEGATIVE);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_NOT);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_BIT_NOT);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_DEREF);
                    CN__ENUM_PRINT_CASE(CN_UNARY_OP_ADDROF);
                    default: 
                        printf("??");
                        break;
                }
                printf("\n");
                ADD_IDX(node->unary_expression.expression_idx);
                break;
            }
        case CN_AST_NODE_CAST_EXPRESSION:
            {
                printf("\n");
                ADD_IDX(node->cast_expression.type_name_idx);
                ADD_IDX(node->cast_expression.expression_idx);
                break;
            }
        case CN_AST_NODE_TERNARY_EXPRESSION:
            {
                printf("\n");
                ADD_IDX(node->ternary_expression.condition_expression_idx);
                ADD_IDX(node->ternary_expression.true_expression_idx);
                ADD_IDX(node->ternary_expression.false_expression_idx);
                break;
            }
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            {
                printf(" operator_kind: ");
                switch(node->assignment_expression.operator_kind) {
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_ASSIGN);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_MULTIPLY);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_DIVIDE);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_MODULO);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_PLUS);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_MINUS);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_LSHIFT);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_RSHIFT);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_BIT_AND);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_BIT_XOR);
                    CN__ENUM_PRINT_CASE(CN_ASSIGNMENT_OP_BIT_OR);
                    default: 
                        printf("??");
                        break;
                }
                printf("\n");
                ADD_IDX(node->assignment_expression.left_expression_idx);
                ADD_IDX(node->assignment_expression.right_expression_idx);
                break;
            }
        case CN_AST_NODE_POSTFIX_EXPRESSION:
            {
                printf(" operator_kind: ");
                switch(node->postfix_expression.operator_kind) {
                    CN__ENUM_PRINT_CASE(CN_POSTFIX_OP_INCREMENT);
                    CN__ENUM_PRINT_CASE(CN_POSTFIX_OP_DECREMENT);
                    default: 
                        printf("??");
                        break;
                }
                printf("\n");
                ADD_IDX(node->postfix_expression.expression_idx);
                break;
            }
        case CN_AST_NODE_PRIMARY_EXPRESSION: 
            { 
                printf("\n");
                ADD_IDX(node->primary_expression.literal_idx);
                break;
            }
        case CN_AST_NODE_IDENTIFIER:
            {   
                printf(" %.*s\n", CN_UNPACK(node->identifier.name));
                break;
            }
        case CN_AST_NODE_INTEGER:
            {   
                printf(" %.*s\n", CN_UNPACK(node->integer.value));
                break;
            }
        case CN_AST_NODE_FLOAT:
            {   
                printf(" %.*s\n", CN_UNPACK(node->flt.value));
                break;
            }
        case CN_AST_NODE_STRING:
            {   
                printf(" \"%.*s\"\n", CN_UNPACK(node->string.str));
                break;
            }
        case CN_AST_NODE_INIT_DECLARATOR_LIST:
            {
                printf("\n");
                ADD_LIST(&node->init_declarator_list.init_declarator_list);
                break;
            }
        case CN_AST_NODE_INIT_DECLARATOR:
            {
                printf("\n");
                ADD_IDX(node->init_declarator.declarator_idx);
                ADD_IDX(node->init_declarator.initializer_idx);
                break;
            }
        case CN_AST_NODE_INITIALIZER:
            {
                printf("\n");
                ADD_IDX(node->initializer.expression_idx);
                break;
            }
        case CN_AST_NODE_ABSTRACT_DECLARATOR:
        case CN_AST_NODE_DECLARATOR:
            {
                printf("\n");
                ADD_IDX(node->declarator.pointer_idx);
                ADD_IDX(node->declarator.direct_declarator_idx);
                break;
            }
        case CN_AST_NODE_POINTER:
            {
                for (int i = 0; i < 4; i++) {
                    switch (node->pointer.qualifiers & (1 << i)) {
                        case CN_AST_TYPE_QUALIFIER_CONST:
                            printf(" 'const'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_RESTRICT:
                            printf(" 'restrict'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_VOLATILE:
                            printf(" 'volatile'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_ATOMIC:
                            printf(" '_Atomic'");
                            break;
                    }
                }

                printf("\n");
                ADD_IDX(node->pointer.pointer_idx);
                break;
            }
        case CN_AST_NODE_DIRECT_DECLARATOR:
            {
                printf(" kind: ");
                switch(node->direct_declarator.kind) {
                    case CN_AST_DIRECT_DECLARATOR_GROUPED:
                        printf("grouped\n");
                        ADD_IDX(node->direct_declarator.declarator_idx);
                        break;
                    case CN_AST_DIRECT_DECLARATOR_ARRAY:
                        printf("array\n");
                        ADD_IDX(node->direct_declarator.array.expression_idx);
                        ADD_IDX(node->direct_declarator.array.direct_declarator_idx);
                        break;
                    case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                        printf("function\n");
                        ADD_IDX(node->direct_declarator.function.parameter_type_list_idx);
                        ADD_IDX(node->direct_declarator.function.direct_declarator_idx);
                        break;
                }


                break;
            }
        case CN_AST_NODE_DECLARATION_SPECIFIERS:
            {
                for (int i = 0; i < 5; i++) {
                    switch (node->declaration_specifiers.storage_specifiers & (1 << i)) {
                        case CN_STORAGE_SPECIFIER_STATIC:
                            printf(" 'static'");
                            break;
                        case CN_STORAGE_SPECIFIER_EXTERN:
                            printf(" 'extern'");
                            break;
                        case CN_STORAGE_SPECIFIER_REGISTER:
                            printf(" 'register'");
                            break;
                        case CN_STORAGE_SPECIFIER_AUTO:
                            printf(" 'auto'");
                            break;
                        case CN_STORAGE_SPECIFIER_TYPEDEF:
                            printf(" 'typedef'");
                            break;
                    }
                }
                for (int i = 0; i < 4; i++) {
                    switch (node->declaration_specifiers.qualifiers & (1 << i)) {
                        case CN_AST_TYPE_QUALIFIER_CONST:
                            printf(" 'const'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_RESTRICT:
                            printf(" 'restrict'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_VOLATILE:
                            printf(" 'volatile'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_ATOMIC:
                            printf(" '_Atomic'");
                            break;
                    }
                }
                printf("\n");

                ADD_IDX(node->declaration_specifiers.type_specifier_idx);
                break;
            }
        case CN_AST_NODE_TYPE_SPECIFIER:
            {
                switch(node->type_specifier.sign) {
                    case CN_AST_TYPE_SIGN_SIGNED:
                        printf(" 'signed'");
                        break;
                    case CN_AST_TYPE_SIGN_UNSIGNED:
                        printf(" 'unsigned'");
                        break;
                    default:
                        break;
                }

                switch(node->type_specifier.width) {
                    case CN_AST_TYPE_WIDTH_SHORT:
                        printf(" 'short'");
                        break;
                    case CN_AST_TYPE_WIDTH_LONG:
                        printf(" 'long'");
                        break;
                    case CN_AST_TYPE_WIDTH_LONG_LONG:
                        printf(" 'long long'");
                        break;
                    default:
                        break;
                }

                if (node->type_specifier.kind < CN_ARRAY_LENGTH(CN_AST_TYPE_KINDS)) {
                    printf(" '...'");
                } else {
                    switch (node->type_specifier.kind) {
                        case CN_AST_TYPE_TYPEDEF:
                            printf(" typedef: '%.*s'", CN_UNPACK(node->type_specifier.typedef_name));
                            break;
                        case CN_AST_TYPE_STRUCT_OR_UNION:
                            ADD_IDX(node->type_specifier.struct_or_union_idx);
                            break;
                        default:
                            break;
                    }
                }
                printf("\n");
                break;
            }
        case CN_AST_NODE_TYPE_NAME:
            {
                printf("\n");
                ADD_IDX(node->type_name.specifier_qualifier_idx);
                ADD_IDX(node->type_name.abstract_declarator_idx);
                break;
            }
        case CN_AST_NODE_SPECIFIER_QUALIFIER:
            {
                for (int i = 0; i < 4; i++) {
                    switch (node->specifier_qualifier.qualifiers & (1 << i)) {
                        case CN_AST_TYPE_QUALIFIER_CONST:
                            printf(" 'const'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_RESTRICT:
                            printf(" 'restrict'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_VOLATILE:
                            printf(" 'volatile'");
                            break;
                        case CN_AST_TYPE_QUALIFIER_ATOMIC:
                            printf(" '_Atomic'");
                            break;
                    }
                }
                printf("\n");

                ADD_IDX(node->specifier_qualifier.type_specifier_idx);
                break;
            }
        case CN_AST_NODE_PARAMETER_TYPE_LIST:
            {
                if (node->parameter_type_list.variadic_args) printf(" variadic");
                printf("\n");

                ADD_LIST(&node->parameter_type_list.parameter_declaration_list);
                break;
            }
        case CN_AST_NODE_PARAMETER_DECLARATION:
            {
                printf("\n");
                ADD_IDX(node->parameter_declaration.declaration_specifiers_idx);
                ADD_IDX(node->parameter_declaration.declarator_idx);
                break;
            }
        case CN_AST_NODE_STRUCT_SPECIFIER:
            {
                printf("\n");

                ADD_IDX(node->struct_specifier.identifier_idx);
                ADD_LIST(&node->struct_specifier.member_declaration_list);
                break;
            }
        case CN_AST_NODE_UNION_SPECIFIER:
            {
                printf("\n");

                ADD_IDX(node->union_specifier.identifier_idx);
                ADD_LIST(&node->union_specifier.member_declaration_list);
                break;
            }
        case CN_AST_NODE_MEMBER_DECLARATION:
            {
                printf("\n");
                ADD_IDX(node->member_declaration.specifier_qualifier_idx);
                ADD_LIST(&node->member_declaration.member_declarator_list);
                break;
            }
        case CN_AST_NODE_MEMBER_DECLARATOR:
            {
                ADD_IDX(node->member_declarator.declarator_idx);
                ADD_IDX(node->member_declarator.bitfield_expression_idx);
                printf("\n");
                break;
            }
        default:
            {
                printf("\n");
            }
            break;
    }

    for (int i = 0; i < next; i++) {
        node = cn_ast_node_get(idxs[i]);

        if (i == next - 1) {
            LAST();
            if (is_list[i]) 
                cn__ast_print_list(node, depth + 1);
            else 
                cn_ast_print(node, depth + 1);
        } else {
            SPLIT();
            if (is_list[i]) 
                cn__ast_print_list(node, depth + 1);
            else 
                cn_ast_print(node, depth + 1);
        }
        
    }


    cn_ast_print_prefixes[depth] = NULL;
}

CNDEF const char *cn_ast_node_kind_name(Cn_Ast_Node_Kind kind) {
    switch (kind) {
        case CN_AST_NODE_UNKNOWN:                 return "UNKNOWN";
        case CN_AST_NODE_ERROR:                   return "ERROR";
        case CN_AST_NODE_CODE:                    return "CODE";
        case CN_AST_NODE_TRANSLATION_UNIT:        return "TRANSLATION_UNIT";
        case CN_AST_NODE_EXTERNAL_DECLARATION:    return "EXTERNAL_DECLARATION";
        case CN_AST_NODE_DECLARATION:             return "DECLARATION";
        case CN_AST_NODE_FUNCTION_DEFINITION:     return "FUNCTION_DEFINITION";
        case CN_AST_NODE_ASM_DEFINITION:          return "ASM_DEFINITION";
        case CN_AST_NODE_COMPOUND_STATEMENT:      return "COMPOUND_STATEMENT";
        case CN_AST_NODE_SELECTION_STATEMENT:     return "SELECTION_STATEMENT";
        case CN_AST_NODE_ITERATION_STATEMENT:     return "ITERATION_STATEMENT";
        case CN_AST_NODE_JUMP_STATEMENT:          return "JUMP_STATEMENT";
        case CN_AST_NODE_LABELED_STATEMENT:       return "LABELED_STATEMENT";
        case CN_AST_NODE_EXPRESSION_STATEMENT:    return "EXPRESSION_STATEMENT";
        case CN_AST_NODE_BINARY_EXPRESSION:       return "BINARY_EXPRESSION";
        case CN_AST_NODE_ACCESS_EXPRESSION:       return "ACCESS_EXPRESSION";
        case CN_AST_NODE_FUNCTION_EXPRESSION:     return "FUNCTION_EXPRESSION";
        case CN_AST_NODE_UNARY_EXPRESSION:        return "UNARY_EXPRESSION";
        case CN_AST_NODE_CAST_EXPRESSION:         return "CAST_EXPRESSION";
        case CN_AST_NODE_SIZEOF_EXPRESSION:       return "SIZEOF_EXPRESSION";
        case CN_AST_NODE_TERNARY_EXPRESSION:      return "TERNARY_EXPRESSION";
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:   return "ASSIGNMENT_EXPRESSION";
        case CN_AST_NODE_POSTFIX_EXPRESSION:      return "POSTFIX_EXPRESSION";
        case CN_AST_NODE_PRIMARY_EXPRESSION:      return "PRIMARY_EXPRESSION";
        case CN_AST_NODE_IDENTIFIER:              return "IDENTIFIER";
        case CN_AST_NODE_INTEGER:                 return "INTEGER";
        case CN_AST_NODE_FLOAT:                   return "FLOAT";
        case CN_AST_NODE_STRING:                  return "STRING";
        case CN_AST_NODE_INIT_DECLARATOR_LIST:    return "INIT_DECLARATOR_LIST";
        case CN_AST_NODE_INIT_DECLARATOR:         return "INIT_DECLARATOR";
        case CN_AST_NODE_INITIALIZER:             return "INITIALIZER";
        case CN_AST_NODE_ABSTRACT_DECLARATOR:     return "ABSTRACT_DECLARATOR";
        case CN_AST_NODE_DECLARATOR:              return "DECLARATOR";
        case CN_AST_NODE_POINTER:                 return "POINTER";
        case CN_AST_NODE_DIRECT_DECLARATOR:       return "DIRECT_DECLARATOR";
        case CN_AST_NODE_DECLARATION_SPECIFIERS:  return "DECLARATION_SPECIFIERS";
        case CN_AST_NODE_GNU_TYPEOF_SPECIFIER:    return "GNU_TYPEOF_SPECIFIER";
        case CN_AST_NODE_TYPE_SPECIFIER:          return "TYPE_SPECIFIER";
        case CN_AST_NODE_TYPE_NAME:               return "TYPE_NAME";
        case CN_AST_NODE_SPECIFIER_QUALIFIER:     return "SPECIFIER_QUALIFIER";
        case CN_AST_NODE_PARAMETER_TYPE_LIST:     return "PARAMETER_TYPE_LIST";
        case CN_AST_NODE_PARAMETER_DECLARATION:   return "PARAMETER_DECLARATION";
        case CN_AST_NODE_STRUCT_SPECIFIER:        return "STRUCT_SPECIFIER";
        case CN_AST_NODE_UNION_SPECIFIER:         return "UNION_SPECIFIER";
        case CN_AST_NODE_MEMBER_DECLARATION:      return "MEMBER_DECLARATION";
        case CN_AST_NODE_MEMBER_DECLARATOR:       return "MEMBER_DECLARATOR";
        case CN_AST_NODE_GNU_ATTRIBUTE_SPECIFIER: return "GNU_ATTRIBUTE_SPECIFIER";
        case CN_AST_NODE_GNU_ATTRIBUTE:           return "GNU_ATTRIBUTE";
        case CN_AST_NODE_GNU_ASM_LABEL:           return "GNU_ASM_LABEL";
        default:                                  return "<invalid kind>";
    }
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
CNDEF int cn__emit_translation_unit(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_external_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_function_definition(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_compound_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_selection_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_jump_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_expression_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_expression(Cn_Ast_Idx expr_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_initializer(Cn_Ast_Idx init_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_declarator(Cn_Ast_Idx declarator_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_pointer(Cn_Ast_Idx pointer_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_direct_declarator(Cn_Ast_Idx direct_decl_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_identifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_integer(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_float(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_string(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_declaration_specifiers(Cn_Ast_Idx decl_spec_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF void cn__emit_storage_specifiers(Cn_Storage_Specifier_Flags storage, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF void cn__emit_qualifiers(Cn_Qualifier_Flags qualifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF void cn__emit_function_specifiers(Cn_Function_Specifier_Flags func_spec, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_type_specifier(Cn_Ast_Idx type_spec_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_type_name(Cn_Ast_Idx type_name_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_specifier_qualifier(Cn_Ast_Idx spec_qual_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_parameter_type_list(Cn_Ast_Idx param_list_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_member_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_gnu_attribute_specifier_sequence(Cn_Ast_Linked_List *specifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_gnu_attribute_specifier(Cn_Ast_Idx spec_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);
CNDEF int cn__emit_gnu_attribute(Cn_Ast_Idx attr_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt);


CNDEF int cn__emit_translation_unit(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_TRANSLATION_UNIT);

    int ok;
    cn_ast_linked_list_foreach(ext_decl, &node->translation_unit.external_declaration_list) {
        Cn_Ast_Idx ext_decl_idx = (Cn_Ast_Idx)(ext_decl - cn__ast_data->node_list);
        ok = cn__emit_opt(ext_decl_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_external_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_EXTERNAL_DECLARATION);

    int ok;
    if (node->external_declaration.extension) {
        cn__emit_str(CN_STR_LIT("__extension__ "), func, opt);
    }
    if (node->external_declaration.child_idx == CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(";"), func, opt);
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;
    } else {
        ok = cn__emit_opt(node->external_declaration.child_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_DECLARATION);

    int ok;
    cn__emit_indent(func, opt);
    ok = cn__emit_opt(node->declaration.declaration_specifiers_idx, func, opt);
    if (ok != 0) return ok;

    Cn_Ast_Node *init_decl_list = cn_ast_node_get(node->declaration.init_declarator_list_idx);
    if (init_decl_list != cn_ast_node_get(CN_AST_NIL_IDX)) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        bool first = true;
        cn_ast_linked_list_foreach(init_decl, &init_decl_list->init_declarator_list.init_declarator_list) {
            if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
            first = false;

            ok = cn__emit_opt(init_decl->init_declarator.declarator_idx, func, opt);
            if (ok != 0) return ok;

            if (init_decl->init_declarator.gnu_asm_label_idx != CN_AST_NIL_IDX) {
                Cn_Ast_Node *asm_label = cn_ast_node_get(init_decl->init_declarator.gnu_asm_label_idx);
                cn__emit_str(CN_STR_LIT(" __asm__("), func, opt);

                ok = cn__emit_opt(asm_label->gnu_asm_label.string_idx, func, opt);
                if (ok != 0) return ok;

                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }

            if (init_decl->init_declarator.initializer_idx != CN_AST_NIL_IDX) {
                cn__emit_str(CN_STR_LIT(" = "), func, opt);
                ok = cn__emit_opt(init_decl->init_declarator.initializer_idx, func, opt);
                if (ok != 0) return ok;
            }
        }
    }
    // Emit trailing GNU attribute specifier sequence.
    ok = cn__emit_gnu_attribute_specifier_sequence(&node->declaration.gnu_attribute_specifier_sequence, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_function_definition(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_FUNCTION_DEFINITION);

    int ok;
    cn__emit_indent(func, opt);
    ok = cn__emit_opt(node->function_definition.declaration_specifiers_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(" "), func, opt);
    ok = cn__emit_opt(node->function_definition.declarator_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(" "), func, opt);
    return cn__emit_opt(node->function_definition.compound_statement_idx, func, opt);
}

CNDEF int cn__emit_compound_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_COMPOUND_STATEMENT);

    int ok;
    cn__emit_str(CN_STR_LIT("{"), func, opt);
    ok = cn__emit_newline(func, opt);
    if (ok != 0) return ok;

    opt->indent++;
    cn_ast_linked_list_foreach(stmt, &node->compound_statement.statement_or_declaration_list) {
        Cn_Ast_Idx stmt_idx = (Cn_Ast_Idx)(stmt - cn__ast_data->node_list);
        ok = cn__emit_opt(stmt_idx, func, opt);
        if (ok != 0) { opt->indent--; return ok; }
    }
    opt->indent--;
    cn__emit_indent(func, opt);
    cn__emit_str(CN_STR_LIT("}"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_selection_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_SELECTION_STATEMENT);

    int ok;
    cn__emit_indent(func, opt);
    if (node->selection_statement.kind == CN_AST_SELECTION_IF) {
        cn__emit_str(CN_STR_LIT("if ("), func, opt);
    } else {
        cn__emit_str(CN_STR_LIT("switch ("), func, opt);
    }
    ok = cn__emit_opt(node->selection_statement.condition_expression_idx, func, opt);
    if (ok != 0) return ok;
    cn__emit_str(CN_STR_LIT(") "), func, opt);

    Cn_Ast_Node *stmt = cn_ast_node_get(node->selection_statement.statement_idx);
    if (stmt->kind == CN_AST_NODE_COMPOUND_STATEMENT) {
        ok = cn__emit_opt(node->selection_statement.statement_idx, func, opt);
        if (ok != 0) return ok;
    } else {
        ok = cn__emit_newline(func, opt);
        if (ok != 0) return ok;

        opt->indent++;
        ok = cn__emit_opt(node->selection_statement.statement_idx, func, opt);
        opt->indent--;
        if (ok != 0) return ok;
    }

    if (node->selection_statement.else_statement_idx != CN_AST_NIL_IDX) {
        cn__emit_indent(func, opt);
        cn__emit_str(CN_STR_LIT("else "), func, opt);
        Cn_Ast_Node *else_stmt = cn_ast_node_get(node->selection_statement.else_statement_idx);
        if (else_stmt->kind == CN_AST_NODE_COMPOUND_STATEMENT || else_stmt->kind == CN_AST_NODE_SELECTION_STATEMENT) {
            ok = cn__emit_opt(node->selection_statement.else_statement_idx, func, opt);
            if (ok != 0) return ok;
        } else {
            ok = cn__emit_newline(func, opt);
            if (ok != 0) return ok;

            opt->indent++;
            ok = cn__emit_opt(node->selection_statement.else_statement_idx, func, opt);
            opt->indent--;
            if (ok != 0) return ok;
        }
    }
    return 0;
}

CNDEF int cn__emit_jump_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_JUMP_STATEMENT);

    int ok;
    cn__emit_indent(func, opt);
    switch (node->jump_statement.kind) {
        case CN_AST_JUMP_GOTO:
            {
                cn__emit_str(CN_STR_LIT("goto "), func, opt);
                ok = cn__emit_opt(node->jump_statement.identifier_idx, func, opt);
                if (ok != 0) return ok;
            }
            break;
        case CN_AST_JUMP_CONTINUE:
            cn__emit_str(CN_STR_LIT("continue"), func, opt);
            break;
        case CN_AST_JUMP_BREAK:
            cn__emit_str(CN_STR_LIT("break"), func, opt);
            break;
        case CN_AST_JUMP_RETURN:
            cn__emit_str(CN_STR_LIT("return"), func, opt);
            if (node->jump_statement.expression_idx != CN_AST_NIL_IDX) {
                cn__emit_str(CN_STR_LIT(" "), func, opt);
                ok = cn__emit_opt(node->jump_statement.expression_idx, func, opt);
                if (ok != 0) return ok;
            }
            break;
    }
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_expression_statement(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_EXPRESSION_STATEMENT);

    int ok;
    cn__emit_indent(func, opt);
    if (node->expression_statement.expression_idx != CN_AST_NIL_IDX) {
        ok = cn__emit_opt(node->expression_statement.expression_idx, func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_expression(Cn_Ast_Idx expr_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (expr_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(expr_idx);

    int ok;
    switch (node->kind) {
        case CN_AST_NODE_UNARY_EXPRESSION:
            {
                Cn_String op = {0};
                switch (node->unary_expression.operator_kind) {
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
                ok = cn__emit_opt(node->unary_expression.expression_idx, func, opt);
                if (ok != 0) return ok;
            }
            break;

        case CN_AST_NODE_POSTFIX_EXPRESSION:
            {
                ok = cn__emit_opt(node->postfix_expression.expression_idx, func, opt);
                if (ok != 0) return ok;
                switch (node->postfix_expression.operator_kind) {
                    case CN_POSTFIX_OP_INCREMENT: cn__emit_str(CN_STR_LIT("++"), func, opt); break;
                    case CN_POSTFIX_OP_DECREMENT: cn__emit_str(CN_STR_LIT("--"), func, opt); break;
                    default: break;
                }
            }
            break;

        case CN_AST_NODE_BINARY_EXPRESSION:
            {
                Cn_String op = {0};
                switch (node->binary_expression.operator_kind) {
                    case CN_BINARY_OP_ARRAY_SUB:
                        cn__emit_str(CN_STR_LIT("("), func, opt);
                        ok = cn__emit_opt(node->binary_expression.left_expression_idx, func, opt);
                        if (ok != 0) return ok;
                        cn__emit_str(CN_STR_LIT("["), func, opt);
                        ok = cn__emit_opt(node->binary_expression.right_expression_idx, func, opt);
                        if (ok != 0) return ok;
                        cn__emit_str(CN_STR_LIT("])"), func, opt);
                        return 0;
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
                ok = cn__emit_opt(node->binary_expression.left_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(op, func, opt);
                ok = cn__emit_opt(node->binary_expression.right_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }
            break;

        case CN_AST_NODE_ACCESS_EXPRESSION:
            {
                ok = cn__emit_opt(node->access_expression.expression_idx, func, opt);
                if (ok != 0) return ok;
                if (node->access_expression.pointer) {
                    cn__emit_str(CN_STR_LIT("->"), func, opt);
                } else {
                    cn__emit_str(CN_STR_LIT("."), func, opt);
                }

                ok = cn__emit_opt(node->access_expression.identifier_idx, func, opt);
                if (ok != 0) return ok;
            }
            break;

        case CN_AST_NODE_FUNCTION_EXPRESSION:
            {
                ok = cn__emit_opt(node->function_expression.expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT("("), func, opt);
                bool first = true;
                cn_ast_linked_list_foreach(arg, &node->function_expression.argument_list) {
                    if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
                    first = false;
                    Cn_Ast_Idx arg_idx = (Cn_Ast_Idx)(arg - cn__ast_data->node_list);
                    ok = cn__emit_opt(arg_idx, func, opt);
                    if (ok != 0) return ok;
                }
                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }
            break;

        case CN_AST_NODE_CAST_EXPRESSION:
            {
                cn__emit_str(CN_STR_LIT("("), func, opt);
                ok = cn__emit_opt(node->cast_expression.type_name_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(")"), func, opt);
                ok = cn__emit_opt(node->cast_expression.expression_idx, func, opt);
                if (ok != 0) return ok;
            }
            break;

        case CN_AST_NODE_SIZEOF_EXPRESSION:
            {
                cn__emit_str(CN_STR_LIT("sizeof("), func, opt);
                Cn_Ast_Node *child = cn_ast_node_get(node->sizeof_expression.child_idx);
                if (child->kind == CN_AST_NODE_TYPE_NAME) {
                    ok = cn__emit_opt(node->sizeof_expression.child_idx, func, opt);
                } else {
                    ok = cn__emit_opt(node->sizeof_expression.child_idx, func, opt);
                }
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }
            break;

        case CN_AST_NODE_TERNARY_EXPRESSION:
            {
                cn__emit_str(CN_STR_LIT("("), func, opt);
                ok = cn__emit_opt(node->ternary_expression.condition_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(" ? "), func, opt);
                ok = cn__emit_opt(node->ternary_expression.true_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(" : "), func, opt);
                ok = cn__emit_opt(node->ternary_expression.false_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }
            break;

        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            {
                Cn_String op = {0};
                switch (node->assignment_expression.operator_kind) {
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
                cn__emit_str(CN_STR_LIT("("), func, opt);
                ok = cn__emit_opt(node->assignment_expression.left_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(op, func, opt);
                ok = cn__emit_opt(node->assignment_expression.right_expression_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }
            break;

        case CN_AST_NODE_PRIMARY_EXPRESSION:
            {
                ok = cn__emit_opt(node->primary_expression.literal_idx, func, opt);
                if (ok != 0) return ok;
            }
            break;

        default:
            // Fallback: try to use source location.
            {
                Cn_String value = cn_source_to_str(&node->src);
                cn__emit_str(value, func, opt);
            }
            break;
    }
    return 0;
}

CNDEF int cn__emit_initializer(Cn_Ast_Idx init_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (init_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *init = cn_ast_node_get(init_idx);
    CN_ASSERT(init->kind == CN_AST_NODE_INITIALIZER);

    return cn__emit_opt(init->initializer.expression_idx, func, opt);
}

CNDEF int cn__emit_declarator(Cn_Ast_Idx declarator_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (declarator_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *decl = cn_ast_node_get(declarator_idx);
    CN_ASSERT(decl->kind == CN_AST_NODE_DECLARATOR || decl->kind == CN_AST_NODE_ABSTRACT_DECLARATOR);

    int ok;
    ok = cn__emit_opt(decl->declarator.pointer_idx, func, opt);
    if (ok != 0) return ok;
    return cn__emit_opt(decl->declarator.direct_declarator_idx, func, opt);
}

CNDEF int cn__emit_pointer(Cn_Ast_Idx pointer_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (pointer_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *ptr = cn_ast_node_get(pointer_idx);
    CN_ASSERT(ptr->kind == CN_AST_NODE_POINTER);

    cn__emit_str(CN_STR_LIT("*"), func, opt);
    cn__emit_qualifiers(ptr->pointer.qualifiers, func, opt);
    return cn__emit_opt(ptr->pointer.pointer_idx, func, opt);
}

CNDEF int cn__emit_direct_declarator(Cn_Ast_Idx direct_decl_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (direct_decl_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *dd = cn_ast_node_get(direct_decl_idx);
    CN_ASSERT(dd->kind == CN_AST_NODE_DIRECT_DECLARATOR || dd->kind == CN_AST_NODE_IDENTIFIER);

    int ok;
    if (dd->kind == CN_AST_NODE_IDENTIFIER) {
        return cn__emit_opt(direct_decl_idx, func, opt);
    }

    switch (dd->direct_declarator.kind) {
        case CN_AST_DIRECT_DECLARATOR_GROUPED:
            cn__emit_str(CN_STR_LIT("("), func, opt);
            ok = cn__emit_opt(dd->direct_declarator.declarator_idx, func, opt);
            if (ok != 0) return ok;
            cn__emit_str(CN_STR_LIT(")"), func, opt);
            break;
        case CN_AST_DIRECT_DECLARATOR_ARRAY:
            ok = cn__emit_opt(dd->direct_declarator.array.direct_declarator_idx, func, opt);
            if (ok != 0) return ok;
            cn__emit_str(CN_STR_LIT("["), func, opt);
            if (dd->direct_declarator.array.expression_idx != CN_AST_NIL_IDX) {
                ok = cn__emit_opt(dd->direct_declarator.array.expression_idx, func, opt);
                if (ok != 0) return ok;
            }
            cn__emit_str(CN_STR_LIT("]"), func, opt);
            break;
        case CN_AST_DIRECT_DECLARATOR_FUNCTION:
            ok = cn__emit_opt(dd->direct_declarator.function.direct_declarator_idx, func, opt);
            if (ok != 0) return ok;
            cn__emit_str(CN_STR_LIT("("), func, opt);
            ok = cn__emit_opt(dd->direct_declarator.function.parameter_type_list_idx, func, opt);
            if (ok != 0) return ok;
            cn__emit_str(CN_STR_LIT(")"), func, opt);
            break;
    }
    return 0;
}

CNDEF int cn__emit_declaration_specifiers(Cn_Ast_Idx decl_spec_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (decl_spec_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *ds = cn_ast_node_get(decl_spec_idx);
    CN_ASSERT(ds->kind == CN_AST_NODE_DECLARATION_SPECIFIERS);

    int ok;
    cn__emit_storage_specifiers(ds->declaration_specifiers.storage_specifiers, func, opt);
    cn__emit_function_specifiers(ds->declaration_specifiers.function_specifiers, func, opt);
    ok = cn__emit_gnu_attribute_specifier_sequence(&ds->declaration_specifiers.gnu_attribute_specifiers, func, opt);
    if (ok != 0) return ok;
    cn__emit_qualifiers(ds->declaration_specifiers.qualifiers, func, opt);
    return cn__emit_opt(ds->declaration_specifiers.type_specifier_idx, func, opt);
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

CNDEF int cn__emit_type_specifier(Cn_Ast_Idx type_spec_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (type_spec_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *ts = cn_ast_node_get(type_spec_idx);
    CN_ASSERT(ts->kind == CN_AST_NODE_TYPE_SPECIFIER);

    int ok;
    // Sign.
    switch (ts->type_specifier.sign) {
        case CN_AST_TYPE_SIGN_SIGNED:   cn__emit_str(CN_STR_LIT("signed "), func, opt); break;
        case CN_AST_TYPE_SIGN_UNSIGNED: cn__emit_str(CN_STR_LIT("unsigned "), func, opt); break;
        default: break;
    }

    // Width.
    switch (ts->type_specifier.width) {
        case CN_AST_TYPE_WIDTH_SHORT:     cn__emit_str(CN_STR_LIT("short "), func, opt); break;
        case CN_AST_TYPE_WIDTH_LONG:      cn__emit_str(CN_STR_LIT("long "), func, opt); break;
        case CN_AST_TYPE_WIDTH_LONG_LONG: cn__emit_str(CN_STR_LIT("long long "), func, opt); break;
        default: break;
    }

    // Base type.
    switch (ts->type_specifier.kind) {
        case CN_AST_TYPE_NONE:   break;
        case CN_AST_TYPE_INT:    cn__emit_str(CN_STR_LIT("int"), func, opt); break;
        case CN_AST_TYPE_CHAR:   cn__emit_str(CN_STR_LIT("char"), func, opt); break;
        case CN_AST_TYPE_FLOAT:  cn__emit_str(CN_STR_LIT("float"), func, opt); break;
        case CN_AST_TYPE_DOUBLE: cn__emit_str(CN_STR_LIT("double"), func, opt); break;
        case CN_AST_TYPE_BOOL:   cn__emit_str(CN_STR_LIT("_Bool"), func, opt); break;
        case CN_AST_TYPE_VOID:   cn__emit_str(CN_STR_LIT("void"), func, opt); break;
        case CN_AST_TYPE_TYPEDEF:
            cn__emit_str(ts->type_specifier.typedef_name, func, opt);
            break;
        case CN_AST_TYPE_STRUCT_OR_UNION:
            {
                Cn_Ast_Node *su = cn_ast_node_get(ts->type_specifier.struct_or_union_idx);
                if (su->kind == CN_AST_NODE_STRUCT_SPECIFIER) {
                    cn__emit_str(CN_STR_LIT("struct "), func, opt);

                    ok = cn__emit_opt(su->struct_specifier.identifier_idx, func, opt);
                    if (ok != 0) return ok;

                    // Only emit body if there are members.
                    if (su->struct_specifier.member_declaration_list.first_idx != CN_AST_NIL_IDX) {
                        cn__emit_str(CN_STR_LIT(" {"), func, opt);
                        ok = cn__emit_newline(func, opt);
                        if (ok != 0) return ok;

                        opt->indent++;
                        cn_ast_linked_list_foreach(member, &su->struct_specifier.member_declaration_list) {
                            Cn_Ast_Idx member_idx = (Cn_Ast_Idx)(member - cn__ast_data->node_list);
                            ok = cn__emit_opt(member_idx, func, opt);
                            if (ok != 0) { opt->indent--; return ok; }
                        }
                        opt->indent--;
                        cn__emit_str(CN_STR_LIT("}"), func, opt);
                    }
                } else if (su->kind == CN_AST_NODE_UNION_SPECIFIER) {
                    cn__emit_str(CN_STR_LIT("union "), func, opt);

                    ok = cn__emit_opt(su->union_specifier.identifier_idx, func, opt);
                    if (ok != 0) return ok;

                    if (su->union_specifier.member_declaration_list.first_idx != CN_AST_NIL_IDX) {
                        cn__emit_str(CN_STR_LIT(" {"), func, opt);
                        ok = cn__emit_newline(func, opt);
                        if (ok != 0) return ok;

                        opt->indent++;
                        cn_ast_linked_list_foreach(member, &su->union_specifier.member_declaration_list) {
                            Cn_Ast_Idx member_idx = (Cn_Ast_Idx)(member - cn__ast_data->node_list);
                            ok = cn__emit_opt(member_idx, func, opt);
                            if (ok != 0) { opt->indent--; return ok; }
                        }
                        opt->indent--;
                        cn__emit_str(CN_STR_LIT("}"), func, opt);
                    }
                }
            }
            break;
        case CN_AST_TYPE_GNU_TYPEOF:
            {
                Cn_Ast_Node *typeof_node = cn_ast_node_get(ts->type_specifier.gnu_typeof_idx);
                cn__emit_str(CN_STR_LIT("__typeof__("), func, opt);
                ok = cn__emit_opt(typeof_node->gnu_typeof_specifier.expression_or_type_name_idx, func, opt);
                if (ok != 0) return ok;
                cn__emit_str(CN_STR_LIT(")"), func, opt);
            }
            break;
        default: break;
    }
    return 0;
}

CNDEF int cn__emit_type_name(Cn_Ast_Idx type_name_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (type_name_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *tn = cn_ast_node_get(type_name_idx);
    CN_ASSERT(tn->kind == CN_AST_NODE_TYPE_NAME);

    int ok;
    ok = cn__emit_opt(tn->type_name.specifier_qualifier_idx, func, opt);
    if (ok != 0) return ok;
    if (tn->type_name.abstract_declarator_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        ok = cn__emit_opt(tn->type_name.abstract_declarator_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_specifier_qualifier(Cn_Ast_Idx spec_qual_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (spec_qual_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *sq = cn_ast_node_get(spec_qual_idx);
    CN_ASSERT(sq->kind == CN_AST_NODE_SPECIFIER_QUALIFIER);

    cn__emit_qualifiers(sq->specifier_qualifier.qualifiers, func, opt);
    return cn__emit_opt(sq->specifier_qualifier.type_specifier_idx, func, opt);
}

CNDEF int cn__emit_parameter_type_list(Cn_Ast_Idx param_list_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (param_list_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *ptl = cn_ast_node_get(param_list_idx);
    CN_ASSERT(ptl->kind == CN_AST_NODE_PARAMETER_TYPE_LIST);

    int ok;
    bool first = true;
    cn_ast_linked_list_foreach(param, &ptl->parameter_type_list.parameter_declaration_list) {
        if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
        first = false;

        ok = cn__emit_opt(param->parameter_declaration.declaration_specifiers_idx, func, opt);
        if (ok != 0) return ok;
        if (param->parameter_declaration.declarator_idx != CN_AST_NIL_IDX) {
            cn__emit_str(CN_STR_LIT(" "), func, opt);
            ok = cn__emit_opt(param->parameter_declaration.declarator_idx, func, opt);
            if (ok != 0) return ok;
        }
    }
    if (ptl->parameter_type_list.variadic_args) {
        if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
        cn__emit_str(CN_STR_LIT("..."), func, opt);
    }
    return 0;
}

CNDEF int cn__emit_identifier(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_IDENTIFIER);

    cn__emit_str(node->identifier.name, func, opt);
    return 0;
}

CNDEF int cn__emit_integer(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_INTEGER);

    cn__emit_str(node->integer.value, func, opt);
    return 0;
}

CNDEF int cn__emit_float(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_FLOAT);

    cn__emit_str(node->flt.value, func, opt);
    return 0;
}

CNDEF int cn__emit_string(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_STRING);

    cn__emit_str(CN_STR_LIT("\""), func, opt);
    cn__emit_str(node->string.str, func, opt);
    cn__emit_str(CN_STR_LIT("\""), func, opt);
    return 0;
}

CNDEF int cn__emit_member_declaration(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_NODE_MEMBER_DECLARATION);

    int ok;
    cn__emit_indent(func, opt);
    ok = cn__emit_opt(node->member_declaration.specifier_qualifier_idx, func, opt);
    if (ok != 0) return ok;

    bool first = true;
    cn_ast_linked_list_foreach(member_decl, &node->member_declaration.member_declarator_list) {
        if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
        first = false;

        if (member_decl->member_declarator.declarator_idx != CN_AST_NIL_IDX) {
            cn__emit_str(CN_STR_LIT(" "), func, opt);
            ok = cn__emit_opt(member_decl->member_declarator.declarator_idx, func, opt);
            if (ok != 0) return ok;
        }
        if (member_decl->member_declarator.bitfield_expression_idx != CN_AST_NIL_IDX) {
            cn__emit_str(CN_STR_LIT(" : "), func, opt);
            ok = cn__emit_opt(member_decl->member_declarator.bitfield_expression_idx, func, opt);
            if (ok != 0) return ok;
        }
    }
    cn__emit_str(CN_STR_LIT(";"), func, opt);
    return cn__emit_newline(func, opt);
}

CNDEF int cn__emit_gnu_attribute_specifier_sequence(Cn_Ast_Linked_List *specifiers, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    int ok;
    cn_ast_linked_list_foreach(spec, specifiers) {
        cn__emit_str(CN_STR_LIT(" "), func, opt);
        Cn_Ast_Idx spec_idx = (Cn_Ast_Idx)(spec - cn__ast_data->node_list);
        ok = cn__emit_gnu_attribute_specifier(spec_idx, func, opt);
        if (ok != 0) return ok;
    }
    return 0;
}

CNDEF int cn__emit_gnu_attribute_specifier(Cn_Ast_Idx spec_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (spec_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *spec = cn_ast_node_get(spec_idx);
    CN_ASSERT(spec->kind == CN_AST_NODE_GNU_ATTRIBUTE_SPECIFIER);

    int ok;
    cn__emit_str(CN_STR_LIT("__attribute__(("), func, opt);
    bool first = true;
    cn_ast_linked_list_foreach(attr, &spec->gnu_attribute_specifier.gnu_attribute_list) {
        if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
        first = false;
        Cn_Ast_Idx attr_idx = (Cn_Ast_Idx)(attr - cn__ast_data->node_list);
        ok = cn__emit_gnu_attribute(attr_idx, func, opt);
        if (ok != 0) return ok;
    }
    cn__emit_str(CN_STR_LIT("))"), func, opt);
    return 0;
}

CNDEF int cn__emit_gnu_attribute(Cn_Ast_Idx attr_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (attr_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *attr = cn_ast_node_get(attr_idx);
    CN_ASSERT(attr->kind == CN_AST_NODE_GNU_ATTRIBUTE);

    int ok;
    // Emit attribute identifier.
    ok = cn__emit_opt(attr->gnu_attribute.identifier_idx, func, opt);
    if (ok != 0) return ok;

    // Emit arguments if present.
    if (attr->gnu_attribute.argument_list.first_idx != CN_AST_NIL_IDX) {
        cn__emit_str(CN_STR_LIT("("), func, opt);
        bool first = true;
        cn_ast_linked_list_foreach(arg, &attr->gnu_attribute.argument_list) {
            if (!first) cn__emit_str(CN_STR_LIT(", "), func, opt);
            first = false;
            Cn_Ast_Idx arg_idx = (Cn_Ast_Idx)(arg - cn__ast_data->node_list);
            ok = cn__emit_opt(arg_idx, func, opt);
            if (ok != 0) return ok;
        }
        cn__emit_str(CN_STR_LIT(")"), func, opt);
    }
    return 0;
}
 
CNDEF int cn__emit_opt(Cn_Ast_Idx node_idx, Cn_Emit_Write *func, Cn_Emit_Opt *opt) {
    if (node_idx == CN_AST_NIL_IDX) return 0;
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    // Check if this is the highlighted node.
    bool is_highlight = (node_idx == opt->highlight_idx && opt->highlight_offset != NULL);
    int64_t highlight_start = 0;
    if (is_highlight) {
        highlight_start = (opt->written_length != NULL) ? *opt->written_length : 0;
        *opt->highlight_offset = highlight_start;
    }

    int ok = 0;
    switch (node->kind) {
        case CN_AST_NODE_TRANSLATION_UNIT:
            ok = cn__emit_translation_unit(node_idx, func, opt);
            break;

        case CN_AST_NODE_EXTERNAL_DECLARATION:
            ok = cn__emit_external_declaration(node_idx, func, opt);
            break;

        case CN_AST_NODE_DECLARATION:
            ok = cn__emit_declaration(node_idx, func, opt);
            break;

        case CN_AST_NODE_FUNCTION_DEFINITION:
            ok = cn__emit_function_definition(node_idx, func, opt);
            break;

        case CN_AST_NODE_COMPOUND_STATEMENT:
            ok = cn__emit_compound_statement(node_idx, func, opt);
            break;

        case CN_AST_NODE_SELECTION_STATEMENT:
            ok = cn__emit_selection_statement(node_idx, func, opt);
            break;

        case CN_AST_NODE_EXPRESSION_STATEMENT:
            ok = cn__emit_expression_statement(node_idx, func, opt);
            break;

        case CN_AST_NODE_JUMP_STATEMENT:
            ok = cn__emit_jump_statement(node_idx, func, opt);
            break;

        case CN_AST_NODE_MEMBER_DECLARATION:
            ok = cn__emit_member_declaration(node_idx, func, opt);
            break;

        // Expression nodes - just emit the expression.
        case CN_AST_NODE_BINARY_EXPRESSION:
        case CN_AST_NODE_ACCESS_EXPRESSION:
        case CN_AST_NODE_FUNCTION_EXPRESSION:
        case CN_AST_NODE_UNARY_EXPRESSION:
        case CN_AST_NODE_CAST_EXPRESSION:
        case CN_AST_NODE_SIZEOF_EXPRESSION:
        case CN_AST_NODE_TERNARY_EXPRESSION:
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
        case CN_AST_NODE_POSTFIX_EXPRESSION:
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            ok = cn__emit_expression(node_idx, func, opt);
            break;

        case CN_AST_NODE_IDENTIFIER:
            ok = cn__emit_identifier(node_idx, func, opt);
            break;

        case CN_AST_NODE_INTEGER:
            ok = cn__emit_integer(node_idx, func, opt);
            break;

        case CN_AST_NODE_FLOAT:
            ok = cn__emit_float(node_idx, func, opt);
            break;

        case CN_AST_NODE_STRING:
            ok = cn__emit_string(node_idx, func, opt);
            break;

        case CN_AST_NODE_DECLARATOR:
        case CN_AST_NODE_ABSTRACT_DECLARATOR:
            ok = cn__emit_declarator(node_idx, func, opt);
            break;

        case CN_AST_NODE_POINTER:
            ok = cn__emit_pointer(node_idx, func, opt);
            break;

        case CN_AST_NODE_DIRECT_DECLARATOR:
            ok = cn__emit_direct_declarator(node_idx, func, opt);
            break;

        case CN_AST_NODE_DECLARATION_SPECIFIERS:
            ok = cn__emit_declaration_specifiers(node_idx, func, opt);
            break;

        case CN_AST_NODE_TYPE_SPECIFIER:
            ok = cn__emit_type_specifier(node_idx, func, opt);
            break;

        case CN_AST_NODE_TYPE_NAME:
            ok = cn__emit_type_name(node_idx, func, opt);
            break;

        case CN_AST_NODE_SPECIFIER_QUALIFIER:
            ok = cn__emit_specifier_qualifier(node_idx, func, opt);
            break;

        case CN_AST_NODE_PARAMETER_TYPE_LIST:
            ok = cn__emit_parameter_type_list(node_idx, func, opt);
            break;

        case CN_AST_NODE_INITIALIZER:
            ok = cn__emit_initializer(node_idx, func, opt);
            break;

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

CNDEF Cn_Ast_Binding_Idx cn_ast_function_binding_declare(Cn_String name, Cn_Ast_Idx source_idx, Cn_Storage_Specifier_Flags storage_flags, Cn_Function_Specifier_Flags function_flags, Cn_Type *type, Cn_Ast_Linked_List *parameter_declaration_list, bool is_definition) {
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
    if (parameter_declaration_list != NULL) {
        binding.b_function.parameter_names = cn_chained_arena_alloc(&cn__ast_data->scoped_strings_arena, sizeof(Cn_String) * parameter_declaration_list->length);
        cn_ast_scope_stack_peek()->saved_strings_length += sizeof(Cn_String) * parameter_declaration_list->length;

        int64_t i = 0;
        Cn_Ast_Idx param_identifier_idx;
        cn_ast_linked_list_foreach(p, parameter_declaration_list) {
            cn_get_declarator_info(p->parameter_declaration.declarator_idx, &param_identifier_idx);

            if (cn_ast_node_get(param_identifier_idx)->kind == CN_AST_NODE_IDENTIFIER) {
                binding.b_function.parameter_names[i] = cn__ast_scope_stack_save_string(cn_ast_node_get(param_identifier_idx)->identifier.name);
            }
            i++;
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
        *type = (Cn_Type) { .kind = kind, .flags = 0 };
        switch (kind) {
            case CN_STRUCT:
                type->t_struct.tag = tag;
                break;
            case CN_UNION:
                type->t_union.tag = tag;
                break;
            case CN_ENUM:
                CN_TODO("Enum binding.");
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
    *type = (Cn_Type) { .kind = kind, .flags = 0 };
    switch (kind) {
        case CN_STRUCT:
            type->t_struct.tag = tag;
            break;
        case CN_UNION:
            type->t_union.tag = tag;
            break;
        case CN_ENUM:
            CN_TODO("Enum binding.");
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

CNDEF Cn_Ast_Idx cn_ast_parse_translation_unit(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_TRANSLATION_UNIT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx next_idx;

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

    while (cn_lexer_token(lexer).type != CN_TOKEN_EOF) {
        next_idx = cn_ast_parse_external_declaration(lexer);

        if (next_idx == CN_AST_NIL_IDX)
            goto error;
        
        cn_ast_linked_list_add(&node.translation_unit.external_declaration_list, next_idx);
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_linked_list_set_parent(parent, &node.translation_unit.external_declaration_list);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_external_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_EXTERNAL_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src  };

    // Optional '__extension__' symbol.
    if (cn_lexer_token(lexer).type == CN_TOKEN_GNU_EXTENSION) {
        node.external_declaration.extension = true;
        cn_lexer_next_token(lexer);
    }
    
    // If stray ';' case.
    if (cn_lexer_token(lexer).type == CN_TOKEN_SEMICOLON) {
        cn_lexer_next_token(lexer);
        return cn_ast_node_list_append(node);
    }


    Cn_Ast_Idx child_idx;

    // TODO: Parse asm definition here.
    // ...

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);

    // Last possible case function definition or declaration.
    // Setting checkpoint.
    if (!cn_ast_checkpoint_set(&cn_ast_checkpoint_message, cn__ast_data, CN_AST_CHECKPOINT_IGNORE_AST_NODES)) {
        child_idx = cn_ast_parse_function_definition_or_declaration(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_node_get(parent)->external_declaration.child_idx = child_idx;
        cn_ast_node_set_parent(parent, child_idx);

        // Messaging function definition.
        cn_ast_idx_from_message = child_idx;

        if (cn_ast_node_get(child_idx)->kind == CN_AST_NODE_FUNCTION_DEFINITION) {
            bool modified = cn_send_message(CN_MESSAGE_PARSED_FUNCTION_DEFINITION, (Cn_Message) {
                        .parsed_function_definition = {
                            .node_idx = cn_ast_idx_from_message,
                        }
                    });

            if (modified) cn_ast_checkpoint_load(&cn_ast_checkpoint_message);
        }
    } else {
        if (!cn_ast_reparse_function_definition(cn_ast_idx_from_message)) goto error;
        cn_ast_node_get(parent)->external_declaration.child_idx = cn_ast_idx_from_message;
        cn_ast_node_set_parent(parent, cn_ast_idx_from_message);
    }

    cn_ast_checkpoint_remove(&cn_ast_checkpoint_message);

    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF void cn_get_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *last_declarator_idx) {
    CN_ASSERT(declarator_idx != CN_AST_NIL_IDX);

    Cn_Ast_Node *decl = cn_ast_node_get(declarator_idx);
    Cn_Ast_Idx dd_idx = decl->declarator.direct_declarator_idx;
    Cn_Ast_Node *dd = decl;
    

    while (dd_idx != CN_AST_NIL_IDX) {
        dd = cn_ast_node_get(dd_idx);

        if (dd->kind == CN_AST_NODE_IDENTIFIER){
            *last_declarator_idx = dd_idx;
            return;
        }

        switch (dd->direct_declarator.kind) {
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                dd_idx = cn_ast_node_get(dd->direct_declarator.declarator_idx)->declarator.direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                dd_idx = dd->direct_declarator.array.direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                dd_idx = dd->direct_declarator.function.direct_declarator_idx;
                continue;
        }
    }

    // NIL Direct declarator, case.
    *last_declarator_idx = cn_ast_idx_get(dd);
}

CNDEF bool cn_get_function_declarator_info(Cn_Ast_Idx declarator_idx, Cn_Ast_Idx *parameter_type_list_idx, Cn_Ast_Idx *last_declarator_idx) {
    CN_ASSERT(declarator_idx != CN_AST_NIL_IDX);

    Cn_Ast_Node *decl = cn_ast_node_get(declarator_idx);
    Cn_Ast_Idx dd_idx = decl->declarator.direct_declarator_idx;
    Cn_Ast_Node *dd = decl;
    

    while (dd_idx != CN_AST_NIL_IDX) {
        dd = cn_ast_node_get(dd_idx);

        // If identifier case, then declarator is not function declarator.
        if (dd->kind == CN_AST_NODE_IDENTIFIER) {
            *last_declarator_idx = dd_idx;
            return false;    
        }
        

        switch (dd->direct_declarator.kind) {
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                dd_idx = cn_ast_node_get(dd->direct_declarator.declarator_idx)->declarator.direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                dd_idx = dd->direct_declarator.array.direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                dd_idx = dd->direct_declarator.function.direct_declarator_idx;

                // If followed by identifier, then return true, and get all outputs.
                if (dd_idx != CN_AST_NIL_IDX && cn_ast_node_get(dd_idx)->kind == CN_AST_NODE_IDENTIFIER) {
                    *parameter_type_list_idx = dd->direct_declarator.function.parameter_type_list_idx;
                    *last_declarator_idx = dd_idx;
                    return true;
                }

                continue;
        }
    }

    // NIL Direct declarator, case.
    *last_declarator_idx = cn_ast_idx_get(dd);
    return false;
}

CNDEF Cn_Ast_Idx cn_ast_parse_function_definition_or_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx declaration_specifiers_idx, child_idx;

    // Getting declration specifiers.
    declaration_specifiers_idx = cn_ast_parse_declaration_specifiers(lexer);
    if (declaration_specifiers_idx == CN_AST_NIL_IDX) goto error;
    
    // Deciding whether node is function definiton or declaration.
    // There MUST be declarator, 
    // which will be either part of init_declarator_list or just be by itself.
    // It might be completely abstract, meaning no tokens are part of it.
    child_idx = cn_ast_parse_declarator(lexer);
    if (child_idx == CN_AST_NIL_IDX) goto error;

    // Now finally we can decide if next token is '{' it is definitely a function definition because, it is part of function definition body, anything else could be part of init_declarator_list or ';'.
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        node.kind = CN_AST_NODE_FUNCTION_DEFINITION;
        node.function_definition.declaration_specifiers_idx = declaration_specifiers_idx;

        node.function_definition.declarator_idx = child_idx;

        // Adding function type and binding.
        Cn_Ast_Binding_Idx function_binding_idx;
        bool ok = cn_ast_analyze_function_definition(
                node.function_definition.declaration_specifiers_idx,
                node.function_definition.declarator_idx,
                &function_binding_idx
                );

        if (!ok) goto error;


        // Making new function scope to store parameters + compound statement symbols.
        cn_ast_scope_stack_push();

        // Binding non-abstract function parameters as variables from definition.
        if (!cn_ast_bind_function_definition_params(function_binding_idx)) goto error;

        child_idx = cn_ast_parse_compound_statement(lexer, true);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        node.function_definition.compound_statement_idx = child_idx;

        cn_ast_scope_stack_pop();

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.function_definition.declaration_specifiers_idx,
                node.function_definition.declarator_idx,
                node.function_definition.compound_statement_idx,
                );
        return parent;

    } else {
        node.kind = CN_AST_NODE_DECLARATION;
        node.declaration.declaration_specifiers_idx = declaration_specifiers_idx;

        child_idx = cn_ast_continue_init_declarator_list(lexer, child_idx);
        if (child_idx == CN_AST_NIL_IDX) goto error_recover;
        node.declaration.init_declarator_list_idx = child_idx;

        // Collecting GNU attributes.
        bool ok;
        node.declaration.gnu_attribute_specifier_sequence = cn_ast_parse_gnu_attribute_specifier_sequence(lexer, &ok);
        if (!ok) goto error;

        // ';' at the end check.
        if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ';' at the end of declaration.");
            goto error_recover;
        }
        cn_lexer_next_token(lexer);
        
        // Adding types that are used by declarators.
        ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, node.declaration.init_declarator_list_idx);
        if (!ok) goto error;


        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.declaration.declaration_specifiers_idx,
                node.declaration.init_declarator_list_idx,
                );
        cn_ast_linked_list_set_parent(parent,
                &node.declaration.gnu_attribute_specifier_sequence
                );
        return parent;
    }

error_recover:
    cn_ast_consume_till(lexer, CN_TOKEN_SEMICOLON);
    return CN_AST_ERROR_IDX;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF bool cn_ast_analyze_declaration(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_Idx init_declarator_list_idx) {
    Cn_Ast_Node *decl_spec = cn_ast_node_get(declaration_specifiers_idx);
    Cn_Ast_Node *init_decl_list = cn_ast_node_get(init_declarator_list_idx);

    // Iterating over declarators.
    Cn_Type *type;
    Cn_String name;
    Cn_Ast_Idx identifier_idx;

    cn_ast_linked_list_foreach(init_decl, &init_decl_list->init_declarator_list.init_declarator_list) {
        type = cn_ast_to_type(decl_spec->declaration_specifiers.qualifiers, decl_spec->declaration_specifiers.type_specifier_idx, init_decl->init_declarator.declarator_idx);
        if (type == NULL) return false;

        // Adding declarator to variable or typedef bidning, if it is not abstract.
        cn_get_declarator_info(init_decl->init_declarator.declarator_idx, &identifier_idx);

        if (cn_ast_node_get(identifier_idx)->kind == CN_AST_NODE_IDENTIFIER) {
            name = cn_ast_node_get(identifier_idx)->identifier.name;

            // Function declaration case.
            if (type->kind == CN_FUNCTION) {
                Cn_Ast_Idx parameter_type_list_idx;
                cn_get_function_declarator_info(
                        init_decl->init_declarator.declarator_idx, 
                        &parameter_type_list_idx, 
                        &identifier_idx
                        );

                Cn_Ast_Linked_List *parameter_declaration_list = NULL;
                if (parameter_type_list_idx != CN_AST_NIL_IDX) 
                    parameter_declaration_list = &cn_ast_node_get(parameter_type_list_idx)->parameter_type_list.parameter_declaration_list;

                if (cn_ast_function_binding_declare(
                            name, 
                            identifier_idx, 
                            decl_spec->declaration_specifiers.storage_specifiers, 
                            decl_spec->declaration_specifiers.function_specifiers, 
                            type, 
                            parameter_declaration_list, 
                            false
                            ) == CN_AST_NIL_BINDING_IDX) return false;

                continue;
            }

            // Typedef declaration case.
            if (decl_spec->declaration_specifiers.storage_specifiers & CN_STORAGE_SPECIFIER_TYPEDEF) {
                if (cn_ast_typedef_binding_declare(name, identifier_idx, type) == CN_AST_NIL_BINDING_IDX) return false;

                continue;
            }

            // Variable declaration case.
            bool is_definition = init_decl->init_declarator.initializer_idx != CN_AST_NIL_IDX;

            if (cn_ast_variable_binding_declare(
                        name, 
                        identifier_idx, 
                        decl_spec->declaration_specifiers.storage_specifiers, 
                        type, 
                        is_definition
                        ) == CN_AST_NIL_BINDING_IDX) return false;


            // Analyze initializer here (only happens in variable declaration case).
            if (init_decl->init_declarator.initializer_idx != CN_AST_NIL_IDX) {
                Cn_Ast_Node *initializer = cn_ast_node_get(init_decl->init_declarator.initializer_idx);
                if (initializer->initializer.expression_idx != CN_AST_NIL_IDX) {
                    if (cn_ast_expression_typecheck(initializer->initializer.expression_idx) == NULL) return false;
                }
            }
        }
    }

    return true;
}

CNDEF bool cn_ast_analyze_function_definition(Cn_Ast_Idx declaration_specifiers_idx, Cn_Ast_Idx declarator_idx, Cn_Ast_Binding_Idx *function_binding_idx) {
    Cn_Ast_Node *decl_spec = cn_ast_node_get(declaration_specifiers_idx);

    Cn_Type *type = cn_ast_to_type(
            decl_spec->declaration_specifiers.qualifiers, 
            decl_spec->declaration_specifiers.type_specifier_idx, 
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

    Cn_Ast_Linked_List *parameter_declaration_list = NULL;
    if (parameter_type_list_idx != CN_AST_NIL_IDX) {
        parameter_declaration_list = &cn_ast_node_get(parameter_type_list_idx)->parameter_type_list.parameter_declaration_list;
    }

    *function_binding_idx = cn_ast_function_binding_declare(
            cn_ast_node_get(identifier_idx)->identifier.name, 
            identifier_idx, 
            decl_spec->declaration_specifiers.storage_specifiers, 
            decl_spec->declaration_specifiers.function_specifiers, 
            type, 
            parameter_declaration_list, 
            true
            );

    if (*function_binding_idx == CN_AST_NIL_BINDING_IDX) return false;

    return true;
}

CNDEF bool cn_ast_bind_function_definition_params(Cn_Ast_Binding_Idx function_binding_idx) {
    Cn_Ast_Binding *function_binding = cn_ast_binding_get(function_binding_idx);

    for (int64_t i = 0; i < function_binding->type->t_function.params_length; i++) {
        if (cn_ast_variable_binding_declare(
                    function_binding->b_function.parameter_names[i], 
                    function_binding->src,                              // TODO: Later supply actual parmeter declaration or declarator ast nodes.
                    0, 
                    function_binding->type->t_function.params[i].type, 
                    true
                    ) == CN_AST_NIL_BINDING_IDX) return false;
    }

    return true;
}

CNDEF Cn_Ast_Idx cn_ast_parse_asm_definition(Cn_Lexer *lexer) {
    CN_UNUSED(lexer);
    CN_TODO("AST asm definition.");
}

CNDEF bool cn_ast_starts_declaration(Cn_Lexer *lexer) {
    return 
            cn_lexer_expect(lexer, CN_TOKEN_STATIC) ||
            cn_lexer_expect(lexer, CN_TOKEN_EXTERN) ||
            cn_lexer_expect(lexer, CN_TOKEN_REGISTER) ||
            cn_lexer_expect(lexer, CN_TOKEN_AUTO) ||
            cn_lexer_expect(lexer, CN_TOKEN_TYPEDEF) ||

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
        return cn_ast_parse_compound_statement(lexer, false);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_IF) || cn_lexer_expect(lexer, CN_TOKEN_SWITCH)) {
        return cn_ast_parse_selection_statement(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_WHILE) || cn_lexer_expect(lexer, CN_TOKEN_DO) || cn_lexer_expect(lexer, CN_TOKEN_FOR)) {
        return cn_ast_parse_iteration_statement(lexer);
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_GOTO) || cn_lexer_expect(lexer, CN_TOKEN_CONTINUE) || cn_lexer_expect(lexer, CN_TOKEN_BREAK) || cn_lexer_expect(lexer, CN_TOKEN_RETURN)) {
        return cn_ast_parse_jump_statement(lexer);
    }

    if (cn_lexer_peek(lexer, 1).type == CN_TOKEN_COLON) {
        return cn_ast_parse_labeled_statement(lexer);
    }
    
    return cn_ast_parse_expression_statement(lexer);
}

CNDEF Cn_Ast_Idx cn_ast_parse_compound_statement(Cn_Lexer *lexer, bool use_current_scope) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_COMPOUND_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx child_idx;

    if (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '{' in the beginning of compound statement.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    // Stepping into scope.
    if (!use_current_scope) cn_ast_scope_stack_push();

    while (true) {
        if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            cn_lexer_next_token(lexer);
            if (!use_current_scope) cn_ast_scope_stack_pop();

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_linked_list_set_parent(parent, &node.compound_statement.statement_or_declaration_list);
            return parent;
        }

        if (cn_ast_starts_declaration(lexer)) {
            child_idx = cn_ast_parse_function_definition_or_declaration(lexer);
        } else {
            child_idx = cn_ast_parse_statement(lexer);
        }

        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(&node.compound_statement.statement_or_declaration_list, child_idx);
    }

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_selection_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_SELECTION_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    

    if (cn_lexer_expect(lexer, CN_TOKEN_IF)) {
        node.selection_statement.kind = CN_AST_SELECTION_IF;

    } else if (cn_lexer_expect(lexer, CN_TOKEN_SWITCH)) {
        node.selection_statement.kind = CN_AST_SELECTION_SWITCH;

    } else {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'if' or 'switch' in selection statement.");
        goto error;
    }

    cn_lexer_next_token(lexer);

    Cn_Ast_Idx condition_expression_idx, statement_idx;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' in selection statement.");
    }
    cn_lexer_next_token(lexer);

    condition_expression_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (condition_expression_idx == CN_AST_NIL_IDX) goto error;
    node.selection_statement.condition_expression_idx = condition_expression_idx;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in selection statement.");
    }
    cn_lexer_next_token(lexer);

    statement_idx = cn_ast_parse_statement(lexer);
    if (statement_idx == CN_AST_NIL_IDX) goto error;
    node.selection_statement.statement_idx = statement_idx;

    if (node.selection_statement.kind == CN_AST_SELECTION_IF && cn_lexer_expect(lexer, CN_TOKEN_ELSE)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx else_statement_idx = cn_ast_parse_statement(lexer);
        if (else_statement_idx == CN_AST_NIL_IDX) goto error;
        node.selection_statement.else_statement_idx = else_statement_idx;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.selection_statement.condition_expression_idx, 
            node.selection_statement.statement_idx, 
            node.selection_statement.else_statement_idx, 
            );
    return parent;
   
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_iteration_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_ITERATION_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx child_idx;

    
    CN_UNUSED(child_idx);
    // Unused label.
    goto error;
    
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    // cn_ast_node_set_parent(parent, );
    return parent;
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_jump_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_JUMP_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    switch (cn_lexer_token(lexer).type) {
        case CN_TOKEN_GOTO: 
            {
                cn_lexer_next_token(lexer);
                node.jump_statement.kind = CN_AST_JUMP_GOTO;

                Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
                if (identifier_idx == CN_AST_NIL_IDX) goto error;
                node.jump_statement.identifier_idx = identifier_idx;
                break;
            }
        case CN_TOKEN_CONTINUE: 
            {
                cn_lexer_next_token(lexer);
                node.jump_statement.kind = CN_AST_JUMP_CONTINUE;
                break;
            }
        case CN_TOKEN_BREAK: 
            {
                cn_lexer_next_token(lexer);
                node.jump_statement.kind = CN_AST_JUMP_BREAK;
                break;
            }
        case CN_TOKEN_RETURN: 
            {
                cn_lexer_next_token(lexer);
                node.jump_statement.kind = CN_AST_JUMP_RETURN;

                if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
                    node.jump_statement.expression_idx = cn_ast_parse_expression(lexer, 0, 0);
                    if (node.jump_statement.expression_idx == CN_AST_NIL_IDX) goto error;

                    // Typechecking return expression.
                    if (cn_ast_expression_typecheck(node.jump_statement.expression_idx) == NULL) goto error;
                }
                break;
            }
        default:
            {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Not a jump statement.");
                goto error;
            }
    }
    
    if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ';' at the end of jump statement.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.jump_statement.identifier_idx,
            node.jump_statement.expression_idx,
            );
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_labeled_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_LABELED_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx child_idx;

    CN_UNUSED(child_idx);
    // Unused label.
    goto error;
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, CN_AST_NIL_IDX);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_EXPRESSION_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (expression_idx == CN_AST_NIL_IDX) goto error;
    
    node.expression_statement.expression_idx = expression_idx;

    if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ';' at the end of expression statement.");
        goto error;
    }
    node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    // Type checking parsed expression.
    Cn_Type *type = cn_ast_expression_typecheck(node.expression_statement.expression_idx);
    if (type == NULL) goto error;

    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.expression_statement.expression_idx);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Linked_List cn_ast_parse_argument_list(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Idx expression_idx;
    Cn_Ast_Linked_List list = {0};

    while (true) {
        expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(&list, expression_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);
    }

    *ok = true;
    return list;

error:
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Linked_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_increasing_precedence(Cn_Lexer *lexer, Cn_Ast_Idx left_idx, int min_precedence, Cn_Expression_Parsing_Flags flags) {
    Cn_Lexer original_state = *lexer;

    // Ternary operator case. TODO: Make macro so its not hardcoded precedence 2.
    if (cn_lexer_expect(lexer, CN_TOKEN_QUESTION)) {
        if (2 < min_precedence) {
            return left_idx;
        }

        Cn_Ast_Node node = { .kind = CN_AST_NODE_TERNARY_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);
        
        node.ternary_expression.condition_expression_idx = left_idx;

        Cn_Ast_Idx child_idx = cn_ast_parse_expression(lexer, -1, flags);
        if (child_idx == CN_AST_NIL_IDX) goto error;

        node.ternary_expression.true_expression_idx = child_idx;

        if (!cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ':' in ternary expression.");
            goto error;
        }
        cn_lexer_next_token(lexer);

        child_idx = cn_ast_parse_expression(lexer, -1, flags);
        if (child_idx == CN_AST_NIL_IDX) goto error;

        node.ternary_expression.false_expression_idx = child_idx;

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.ternary_expression.condition_expression_idx,
                node.ternary_expression.true_expression_idx,
                node.ternary_expression.false_expression_idx,
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
            Cn_Ast_Node node = { .kind = CN_AST_NODE_FUNCTION_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
            cn_lexer_next_token(lexer);

            node.function_expression.expression_idx = left_idx;
            
            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                bool ok;
                node.function_expression.argument_list = cn_ast_parse_argument_list(lexer, &ok);
                if (!ok) goto error;

                if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                    cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in function call expression.");
                    goto error;
                }
            }
            cn_lexer_next_token(lexer);


            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, node.function_expression.expression_idx);
            cn_ast_linked_list_set_parent(parent, &node.function_expression.argument_list);
            return parent;
        } 

        // Access operator cases.
        if (cn_lexer_expect(lexer, CN_TOKEN_ARROW) || cn_lexer_expect(lexer, CN_TOKEN_DOT)) {
            Cn_Ast_Node node = { .kind = CN_AST_NODE_ACCESS_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
            cn_lexer_next_token(lexer);
            
            node.access_expression.expression_idx = left_idx;

            node.access_expression.pointer = op_kind == CN_BINARY_OP_ARROW;

            Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
            if (identifier_idx == CN_AST_NIL_IDX) goto error;
            node.access_expression.identifier_idx = identifier_idx;

            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, 
                    node.access_expression.expression_idx,
                    node.access_expression.identifier_idx
                    );
            return parent;
        }

        // Array subscript case.
        Cn_Ast_Node node = { .kind = CN_AST_NODE_BINARY_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        if (cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
            cn_lexer_next_token(lexer);

            Cn_Ast_Idx right_idx = cn_ast_parse_expression(lexer, precedence, flags);
            if (right_idx == CN_AST_NIL_IDX) goto error;

            if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ']' in array subscript expression.");
                goto error;
            }
            cn_lexer_next_token(lexer);

            node.binary_expression = ((Cn_Ast_Node_Binary_Expression) {
                    .left_expression_idx = left_idx, 
                    .operator_kind = CN_BINARY_OP_ARRAY_SUB, 
                    .right_expression_idx = right_idx
                });


            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, 
                    node.binary_expression.left_expression_idx,
                    node.binary_expression.right_expression_idx
                    );
            return parent;
        } 

        // Regualar binary operator case.
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx right_idx = cn_ast_parse_expression(lexer, precedence, flags);
        if (right_idx == CN_AST_NIL_IDX) goto error;

        node.binary_expression = ((Cn_Ast_Node_Binary_Expression) {
                .left_expression_idx = left_idx, 
                .operator_kind = op_kind, 
                .right_expression_idx = right_idx
                });

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.binary_expression.left_expression_idx,
                node.binary_expression.right_expression_idx
                );
        return parent;
    }

    // Assignment operator case.
    op_kind = cn_ast_is_assignment_operator(lexer);
    if (op_kind != CN_ASSIGNMENT_OP_NONE) {

        if (CN_ASSIGNMENT_OPERATOR_PRECEDENCE < min_precedence) {
            return left_idx;
        }

        Cn_Ast_Node node = { .kind = CN_AST_NODE_ASSIGNMENT_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        Cn_Ast_Idx right_idx = cn_ast_parse_expression(lexer, CN_ASSIGNMENT_OPERATOR_PRECEDENCE, flags);
        if (right_idx == CN_AST_NIL_IDX) goto error;

        node.assignment_expression = ((Cn_Ast_Node_Assignment_Expression) {
                .left_expression_idx = left_idx, 
                .operator_kind = op_kind, 
                .right_expression_idx = right_idx
                });


        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.assignment_expression.left_expression_idx,
                node.assignment_expression.right_expression_idx
                );
        return parent;
    }

    // Postfix operator case.
    op_kind = cn_ast_is_postfix_operator(lexer);
    if (op_kind != CN_POSTFIX_OP_NONE) {

        if (CN_POSTFIX_OPERATOR_PRECEDENCE <= min_precedence) {
            return left_idx;
        }

        Cn_Ast_Node node = { .kind = CN_AST_NODE_POSTFIX_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        node.postfix_expression = ((Cn_Ast_Node_Postfix_Expression) {
                .expression_idx = left_idx, 
                .operator_kind = op_kind 
                });


        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, 
                node.postfix_expression.expression_idx,
                );
        return parent;
    }

    return left_idx;

error:
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

    return left_idx;

error:
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
            Cn_Ast_Node node = { .kind = CN_AST_NODE_CAST_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            Cn_Ast_Idx type_name_idx = cn_ast_parse_type_name(lexer);
            if (type_name_idx == CN_AST_NIL_IDX) goto error;
            
            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' closing parenthesis at the end of cast expression.");
                goto error;
            }
            cn_lexer_next_token(lexer);
            
            // TODO: Compound literal case.

            Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, CN_UNARY_OPERATOR_PRECEDENCE, flags);
            if (expression_idx == CN_AST_NIL_IDX) goto error;

            node.cast_expression = ((Cn_Ast_Node_Cast_Expression) {
                    .type_name_idx = type_name_idx, 
                    .expression_idx = expression_idx
                    });


            Cn_Ast_Idx parent = cn_ast_node_list_append(node);
            cn_ast_node_set_parent(parent, 
                    node.cast_expression.type_name_idx,
                    node.cast_expression.expression_idx
                    );
            return parent;
        }
        
        // Parenthesized expression case.
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        
        if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' closing parenthesis at the end of parenthesized expression.");
            goto error;
        }
        cn_lexer_next_token(lexer);

        return expression_idx;
    }

    // Handling sizeof expression.
    if (cn_lexer_expect(lexer, CN_TOKEN_SIZEOF)) {
        Cn_Ast_Node node = { .kind = CN_AST_NODE_SIZEOF_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
        cn_lexer_next_token(lexer);

        if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Lexer peeking_lexer = *lexer;
            cn_lexer_next_token(&peeking_lexer);

            if (cn_ast_starts_type(&peeking_lexer)) {
                // Parsing type_name.
                cn_lexer_next_token(lexer);
                
                Cn_Ast_Idx type_name_idx = cn_ast_parse_type_name(lexer);
                if (type_name_idx == CN_AST_NIL_IDX) goto error;


                if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                    cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' closing parenthesis at the end of sizeof expression.");
                    goto error;
                }
                cn_lexer_next_token(lexer);

                node.sizeof_expression = (Cn_Ast_Node_Sizeof_Expression) {
                    .child_idx = type_name_idx
                };

                Cn_Ast_Idx parent = cn_ast_node_list_append(node);
                cn_ast_node_set_parent(parent, node.sizeof_expression.child_idx);
                return parent;
            }
        }

        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, CN_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression_idx == CN_AST_NIL_IDX) goto error;

        node.sizeof_expression = (Cn_Ast_Node_Sizeof_Expression) {
            .child_idx = expression_idx
        };

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, node.sizeof_expression.child_idx);
        return parent;
    }

    // Handling unary expression.
    Cn_Unary_Operator_Kind op_kind = cn_ast_is_unary_operator(lexer);
    if (op_kind != CN_UNARY_OP_NONE) {
        Cn_Ast_Node node = { .kind = CN_AST_NODE_UNARY_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, CN_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression_idx == CN_AST_NIL_IDX) goto error;

        node.unary_expression = (Cn_Ast_Node_Unary_Expression) {
            .operator_kind = op_kind, 
            .expression_idx = expression_idx
        };

        Cn_Ast_Idx parent = cn_ast_node_list_append(node);
        cn_ast_node_set_parent(parent, node.unary_expression.expression_idx);
        return parent;
    }
    
    // Primary expression case.
    Cn_Ast_Node node = { .kind = CN_AST_NODE_PRIMARY_EXPRESSION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
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
    node.primary_expression.literal_idx = literal_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.primary_expression.literal_idx);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF bool cn_ast_is_primary(Cn_Ast_Idx node_idx) {
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);
    return node->kind == CN_AST_NODE_IDENTIFIER || 
        node->kind == CN_AST_NODE_INTEGER ||
        node->kind == CN_AST_NODE_FLOAT || 
        node->kind == CN_AST_NODE_STRING;
}

CNDEF Cn_Ast_Idx cn_ast_continue_init_declarator_list(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx) {
    Cn_Lexer original_state = *lexer;
    
    Cn_Ast_Node node = { .kind = CN_AST_NODE_INIT_DECLARATOR_LIST, .loc = cn_ast_node_get(declarator_idx)->loc, .src = cn_ast_node_get(declarator_idx)->src };

    Cn_Ast_Idx child_idx = cn_ast_finish_init_declarator(lexer, declarator_idx);

    while(true) {
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(&node.init_declarator_list.init_declarator_list, child_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);

        child_idx = cn_ast_parse_init_declarator(lexer);
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_linked_list_set_parent(parent, &node.init_declarator_list.init_declarator_list);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator_list(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INIT_DECLARATOR_LIST, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    Cn_Ast_Idx child_idx;

    while(true) {
        child_idx = cn_ast_parse_init_declarator(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(&node.init_declarator_list.init_declarator_list, child_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;

        cn_lexer_next_token(lexer);
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_linked_list_set_parent(parent, &node.init_declarator_list.init_declarator_list);
    return parent;
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_finish_init_declarator(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INIT_DECLARATOR, .loc = cn_ast_node_get(declarator_idx)->loc, .src = cn_ast_node_get(declarator_idx)->src };

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
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer);
    if (declarator_idx == CN_AST_NIL_IDX) goto error;

    return cn_ast_finish_init_declarator(lexer, declarator_idx);
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INITIALIZER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // TODO: Initializer implementation.
    node.initializer.expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
    if (node.initializer.expression_idx == CN_AST_NIL_IDX) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.initializer.expression_idx);
    return parent;
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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
        node.kind = CN_AST_NODE_ABSTRACT_DECLARATOR;
    } else if (child_idx == CN_AST_NIL_IDX) {
        goto error;
    }

    node.declarator.direct_declarator_idx = child_idx;
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.declarator.pointer_idx,
            node.declarator.direct_declarator_idx
            );
    return parent;
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_pointer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_POINTER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '*' when parsing pointer.");
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
        node.pointer.pointer_idx = child_idx;
    }
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.pointer.pointer_idx);
    return parent;
    
error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_direct_declarator(Cn_Lexer *lexer, bool *is_abstract) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_DIRECT_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx child_idx;
    Cn_Ast_Idx direct_declarator_idx;
    
    // '(' declarator ')' case.
    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        node.direct_declarator.kind = CN_AST_DIRECT_DECLARATOR_GROUPED;

        cn_lexer_next_token(lexer);

        child_idx = cn_ast_parse_declarator(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        node.direct_declarator.declarator_idx = child_idx;

        // Making above declarator abstract if its child is abstract too.
        *is_abstract = cn_ast_node_get(child_idx)->kind == CN_AST_NODE_ABSTRACT_DECLARATOR;

        if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' when parsing direct declarator.");
            goto error;
        }

        cn_lexer_next_token(lexer);

        direct_declarator_idx = cn_ast_node_list_append(node);

        cn_ast_node_set_parent(direct_declarator_idx, node.direct_declarator.declarator_idx);
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
    // direct_declarator '[' expression? ']' case.
    while(true) {
        if (cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {

            node = (Cn_Ast_Node) { .kind = CN_AST_NODE_DIRECT_DECLARATOR, .loc = cn_lexer_token(lexer).loc };
            node.direct_declarator.kind = CN_AST_DIRECT_DECLARATOR_ARRAY;

            cn_lexer_next_token(lexer);

            if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
                Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
                if (expression_idx == CN_AST_NIL_IDX) goto error;
                node.direct_declarator.array.expression_idx = expression_idx;

                // Type checking expression.
                Cn_Type *type = cn_ast_expression_typecheck(expression_idx);
                if (type == NULL) goto error;
            }

            if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ']' when parsing direct declarator.");
                goto error;
            }

            cn_lexer_next_token(lexer);

            // Extending source loc to highlight whole array '[...]' declarator.
            node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

            node.direct_declarator.array.direct_declarator_idx = direct_declarator_idx;

            direct_declarator_idx = cn_ast_node_list_append(node);

            cn_ast_node_set_parent(direct_declarator_idx, node.direct_declarator.array.direct_declarator_idx);
            continue;
        }
    // direct_declarator '(' ('void' | parameter_type_list)? ')' case.
        else if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {

            node = (Cn_Ast_Node) { .kind = CN_AST_NODE_DIRECT_DECLARATOR, .loc = cn_lexer_token(lexer).loc};
            node.direct_declarator.kind = CN_AST_DIRECT_DECLARATOR_FUNCTION;

            cn_lexer_next_token(lexer);

            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                // If not '(' 'void' ')' case.
                if (cn_lexer_expect(lexer, CN_TOKEN_VOID) && cn_lexer_peek(lexer, 1).type == CN_TOKEN_PARAN_CLOSE) {
                    cn_lexer_next_token(lexer);
                } else {
                    Cn_Ast_Idx parameter_type_list_idx = cn_ast_parse_parameter_type_list(lexer);
                    if (parameter_type_list_idx == CN_AST_NIL_IDX) goto error;
                    node.direct_declarator.function.parameter_type_list_idx = parameter_type_list_idx;
                }
            }

            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' when parsing direct declarator.");
                goto error;
            }

            cn_lexer_next_token(lexer);

            // Extending source loc to highlight whole function '(...)' declarator.
            node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

            node.direct_declarator.function.direct_declarator_idx = direct_declarator_idx;

            direct_declarator_idx = cn_ast_node_list_append(node);

            cn_ast_node_set_parent(direct_declarator_idx, 
                    node.direct_declarator.function.direct_declarator_idx,
                    node.direct_declarator.function.parameter_type_list_idx,
                    );
            continue;
        }

        // No postfix case.
        return direct_declarator_idx;
    }

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_identifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_IDENTIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected identifier token.");
        goto error;
    }
    
    node.identifier.name = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_integer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_INTEGER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_INTEGER_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected integer token.");
        goto error;
    }

    node.integer.value = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_float(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_FLOAT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_FLOAT_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected float token.");
        goto error;
    }

    node.flt.value = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_node_list_append(node);

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_string(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_STRING, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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
    cn_sb_free(&sb);
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_declaration_specifiers(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_DECLARATION_SPECIFIERS, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    node.declaration_specifiers.type_specifier_idx = cn_ast_node_list_append((Cn_Ast_Node) { .kind = CN_AST_NODE_TYPE_SPECIFIER });

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
            cn_ast_linked_list_add(&node.declaration_specifiers.gnu_attribute_specifiers, gnu_attribute_specifier);

            continue;
        }
    
        ok = cn_ast_try_parse_type_specifier(lexer, node.declaration_specifiers.type_specifier_idx);
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
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MISSING_DECLARATION_SPECIFIER, "At least one declaration specifier should be present.");
        goto error;
    }

    // Validation of type specifier.
    Cn_Ast_Node *ts = cn_ast_node_get(node.declaration_specifiers.type_specifier_idx);

    // Implicit int case.
    if (ts->type_specifier.kind == CN_AST_TYPE_NONE) {
        ts->type_specifier.kind = CN_AST_TYPE_INT;
    }

    if (ts->type_specifier.width != CN_AST_TYPE_WIDTH_NONE && ts->type_specifier.kind != CN_AST_TYPE_INT) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &node.loc, &node.src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type width on non 'int' type.");
        goto error;
    }

    if (ts->type_specifier.sign != CN_AST_TYPE_SIGN_NONE && ts->type_specifier.kind != CN_AST_TYPE_INT && ts->type_specifier.kind != CN_AST_TYPE_CHAR) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &node.loc, &node.src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type sign on non 'int' or 'char' type.");
        goto error;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.declaration_specifiers.type_specifier_idx);
    cn_ast_linked_list_set_parent(parent, &node.declaration_specifiers.gnu_attribute_specifiers);
    return parent;

error:
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

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_typeof_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;
    Cn_Ast_Node node = { .kind = CN_AST_NODE_GNU_TYPEOF_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'typeof' in GNU typeof specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' in GNU typeof specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    Cn_Ast_Idx child_idx;

    if (cn_ast_starts_type(lexer)) {
        child_idx = cn_ast_parse_type_name(lexer);
    } else {
        child_idx = cn_ast_parse_expression(lexer, 0, 0);

        // Typechecking expression right after parsing.
        if (cn_ast_expression_typecheck(child_idx) == NULL) goto error;
    }

    if (child_idx == CN_AST_NIL_IDX) goto error;
    node.gnu_typeof_specifier.expression_or_type_name_idx = child_idx;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in GNU typeof specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.gnu_typeof_specifier.expression_or_type_name_idx);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_try_parse_type_specifier(Cn_Lexer *lexer, Cn_Ast_Idx output_idx) {
    Cn_Lexer original_state = *lexer;

    CN_ASSERT(output_idx != CN_AST_NIL_IDX);
    Cn_Ast_Node *node = cn_ast_node_get(output_idx);
    
    // Checking if token is primitive.
    for (Cn_Ast_Type_Kind kind = CN_AST_TYPE_INT; kind < CN_ARRAY_LENGTH(CN_AST_TYPE_KINDS); kind++) {
        if (cn_lexer_expect(lexer, CN_AST_TYPE_KINDS[kind])) {

            if (node->type_specifier.kind != CN_AST_TYPE_NONE) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
                goto error;
            }
            
            cn_lexer_next_token(lexer);
            node->type_specifier.kind = kind;
            return 0;
        }
    }

    // Checking if token is typedef.
    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {   
        Cn_String str = cn_source_to_str(&cn_lexer_token(lexer).src);
        Cn_Ast_Binding_Idx *ref = cn_hash_table_get(&cn__ast_data->symbol_binding_table, &str);

        // Printing bindings.
        if (ref != NULL && cn__ast_data->binding_list[*ref].kind == CN_BINDING_TYPEDEF) {
            // TODO: String saving.
            node->type_specifier.typedef_name = str;

            node->type_specifier.kind = CN_AST_TYPE_TYPEDEF;
            cn_lexer_next_token(lexer);

            return 0;
        }
    }
    
    // Checking if token is struct or union.
    if (cn_lexer_expect(lexer, CN_TOKEN_STRUCT) || cn_lexer_expect(lexer, CN_TOKEN_UNION)) {
        node->type_specifier.kind = CN_AST_TYPE_STRUCT_OR_UNION;

        Cn_Ast_Idx struct_or_union_idx = cn_ast_parse_struct_or_union_specifier(lexer);
        // Getting node again because array list that stores nodes might 
        // have been resized, invalidating previous pointer.
        node = cn_ast_node_get(output_idx);

        if (struct_or_union_idx == CN_AST_NIL_IDX) goto error;
        node->type_specifier.struct_or_union_idx = struct_or_union_idx;

        return 0;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)) {   
        node->type_specifier.kind = CN_AST_TYPE_GNU_TYPEOF;
        
        Cn_Ast_Idx gnu_typeof_idx = cn_ast_parse_gnu_typeof_specifier(lexer);
        // Getting node again because array list that stores nodes might 
        // have been resized, invalidating previous pointer.
        node = cn_ast_node_get(output_idx);
        
        if (gnu_typeof_idx == CN_AST_NIL_IDX) goto error;
        node->type_specifier.struct_or_union_idx = gnu_typeof_idx;

        return 0;
    }

    // INCOMPLETE:
    // Checking if token(s) is enum specifier.

    // Checking if token is type sign.
    if (cn_lexer_expect(lexer, CN_TOKEN_SIGNED)) {
        if (node->type_specifier.sign != CN_AST_TYPE_SIGN_NONE) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type sign 'signed' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.sign = CN_AST_TYPE_SIGN_SIGNED;
        return 0;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_UNSIGNED)) {
        if (node->type_specifier.sign != CN_AST_TYPE_SIGN_NONE) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type sign 'unsigned' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.sign = CN_AST_TYPE_SIGN_UNSIGNED;
        return 0;
    }

    // Checking if token is type width.
    if (cn_lexer_expect(lexer, CN_TOKEN_SHORT)) {
        if (node->type_specifier.width != CN_AST_TYPE_WIDTH_NONE) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type width 'short' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.width = CN_AST_TYPE_WIDTH_SHORT;
        return 0;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_LONG)) {
        if (node->type_specifier.width != CN_AST_TYPE_WIDTH_NONE) {
            if (node->type_specifier.width == CN_AST_TYPE_WIDTH_LONG) {
                cn_lexer_next_token(lexer);
                node->type_specifier.width = CN_AST_TYPE_WIDTH_LONG_LONG;
                return 0;
            }

            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_DUPLICATE_TYPE_SPECIFIER, "Duplicate type width 'long' is not allowed in type specifier.");
            goto error;
        }

        cn_lexer_next_token(lexer);
        node->type_specifier.width = CN_AST_TYPE_WIDTH_LONG;
        return 0;
    }


    // INCOMPLETE:
    // Checking if token(s) atomic_type_specifier.
    

    return 1;

error:
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Idx cn_ast_parse_type_name(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_TYPE_NAME, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
    if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
    node.type_name.specifier_qualifier_idx = specifier_qualifier_idx;
    
    Cn_Ast_Idx abstract_declarator_idx = cn_ast_parse_declarator(lexer);
    if (abstract_declarator_idx == CN_AST_NIL_IDX) goto error;
    
    if (cn_ast_node_get(abstract_declarator_idx)->kind != CN_AST_NODE_ABSTRACT_DECLARATOR) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_NAME, "Only abstract declarator is allowed in type name.");
        goto error;
    }

    node.type_name.abstract_declarator_idx = abstract_declarator_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.type_name.specifier_qualifier_idx,
            node.type_name.abstract_declarator_idx,
            );
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_specifier_qualifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_SPECIFIER_QUALIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    node.specifier_qualifier.type_specifier_idx = cn_ast_node_list_append((Cn_Ast_Node) { .kind = CN_AST_NODE_TYPE_SPECIFIER });

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
    
        ok = cn_ast_try_parse_type_specifier(lexer, node.specifier_qualifier.type_specifier_idx);
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
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MISSING_DECLARATION_SPECIFIER, "At least one declaration specifier should be present.");
        goto error;
    }

    // Validation of type specifier.
    Cn_Ast_Node *ts = cn_ast_node_get(node.specifier_qualifier.type_specifier_idx);

    // Implicit int case.
    if (ts->type_specifier.kind == CN_AST_TYPE_NONE) {
        ts->type_specifier.kind = CN_AST_TYPE_INT;
    }

    if (ts->type_specifier.width != CN_AST_TYPE_WIDTH_NONE && ts->type_specifier.kind != CN_AST_TYPE_INT) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type width on non 'int' type.");
        goto error;
    }

    if (ts->type_specifier.sign != CN_AST_TYPE_SIGN_NONE && ts->type_specifier.kind != CN_AST_TYPE_INT && ts->type_specifier.kind != CN_AST_TYPE_CHAR) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type sign on non 'int' or 'char' type.");
        goto error;
    }


    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.specifier_qualifier.type_specifier_idx
            );
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_parameter_type_list(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_PARAMETER_TYPE_LIST, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx parameter_declaration;
    while(true) {
        // Checking if '...'.
        if (cn_lexer_expect(lexer, CN_TOKEN_ELLIPSIS)) {
            node.parameter_type_list.variadic_args = true;
            cn_lexer_next_token(lexer);
            return cn_ast_node_list_append(node);
        }

        parameter_declaration = cn_ast_parse_parameter_declaration(lexer);
        if (parameter_declaration == CN_AST_NIL_IDX) goto error;

        cn_ast_linked_list_add(&node.parameter_type_list.parameter_declaration_list, parameter_declaration);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;

        cn_lexer_next_token(lexer);
    }
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_linked_list_set_parent(parent, &node.parameter_type_list.parameter_declaration_list);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_parameter_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_PARAMETER_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx declaration_specifiers_idx = cn_ast_parse_declaration_specifiers(lexer);
    if (declaration_specifiers_idx == CN_AST_NIL_IDX) goto error;
    node.parameter_declaration.declaration_specifiers_idx = declaration_specifiers_idx;

    Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer);
    if (declarator_idx == CN_AST_NIL_IDX) goto error;
    node.parameter_declaration.declarator_idx = declarator_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.parameter_declaration.declaration_specifiers_idx,
            node.parameter_declaration.declarator_idx
            );
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_struct_or_union_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    switch (cn_lexer_token(lexer).type) {
        case CN_TOKEN_STRUCT:
            {
                node.kind = CN_AST_NODE_STRUCT_SPECIFIER;
                break;
            }
        case CN_TOKEN_UNION:
            {
                node.kind = CN_AST_NODE_UNION_SPECIFIER;
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
        node.kind == CN_AST_NODE_STRUCT_SPECIFIER ? (node.struct_specifier.identifier_idx = identifier_idx) : (node.union_specifier.identifier_idx = identifier_idx);
    } else {
        // Generating unique tag name for anonymous struct.
        Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
        cn_sb_append_format(&sb, "cn__struct_%lu", cn_ast_counter_next());
        Cn_String tag = cn_sb_to_str(&sb);

        void *data = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, tag.length);
        cn_str_copy_to(tag, data);
        tag.data = data;

        Cn_Ast_Idx identifier_idx = cn_ast_node_list_append((Cn_Ast_Node) {
                    .kind = CN_AST_NODE_IDENTIFIER,
                    .loc = cn_lexer_token(lexer).loc,
                    .identifier.name = tag,
                });


        node.kind == CN_AST_NODE_STRUCT_SPECIFIER ? (node.struct_specifier.identifier_idx = identifier_idx) : (node.union_specifier.identifier_idx = identifier_idx);

        cn_sb_free(&sb);
    }

    // Parsing struct or union body.
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Linked_List member_declaration_list = {0};
        Cn_Ast_Idx member_declaration_idx;
        while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            member_declaration_idx = cn_ast_parse_member_declaration(lexer);
            if (member_declaration_idx == CN_AST_NIL_IDX) goto error;
            cn_ast_linked_list_add(&member_declaration_list, member_declaration_idx);
        }
        cn_lexer_next_token(lexer);

        node.kind == CN_AST_NODE_STRUCT_SPECIFIER ? (node.struct_specifier.member_declaration_list = member_declaration_list) : (node.union_specifier.member_declaration_list = member_declaration_list);
    }


    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    if (node.kind == CN_AST_NODE_STRUCT_SPECIFIER) {
        cn_ast_node_set_parent(parent, node.struct_specifier.identifier_idx);
        cn_ast_linked_list_set_parent(parent, &node.struct_specifier.member_declaration_list);
    } else {
        cn_ast_node_set_parent(parent, node.struct_specifier.identifier_idx);
        cn_ast_linked_list_set_parent(parent, &node.struct_specifier.member_declaration_list);
    }
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_member_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_MEMBER_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
    if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
    node.member_declaration.specifier_qualifier_idx = specifier_qualifier_idx;

    if (cn_ast_parse_member_declarator_list(lexer, &node.member_declaration.member_declarator_list) != 0) 
        goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ';' at the end of member declaration.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.member_declaration.specifier_qualifier_idx);
    cn_ast_linked_list_set_parent(parent, &node.member_declaration.member_declarator_list);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF int cn_ast_parse_member_declarator_list(Cn_Lexer *lexer, Cn_Ast_Linked_List *member_declarator_list) {
    Cn_Lexer original_state = *lexer;

    while (true) {
        Cn_Ast_Idx member_declarator_idx = cn_ast_parse_member_declarator(lexer);
        if (member_declarator_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(member_declarator_list, member_declarator_idx);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);
    }
    
    return 0;

error:
    *lexer = original_state;
    return -1;
}

CNDEF Cn_Ast_Idx cn_ast_parse_member_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_MEMBER_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
        Cn_Ast_Idx declarator_idx = cn_ast_parse_declarator(lexer);
        if (declarator_idx == CN_AST_NIL_IDX) goto error;
        node.member_declarator.declarator_idx = declarator_idx;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, 0, CN_NO_COMMA_OPERATOR);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.member_declarator.bitfield_expression_idx = expression_idx;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.member_declarator.declarator_idx,
            node.member_declarator.bitfield_expression_idx
            );
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_attribute_specifier_sequence(Cn_Lexer *lexer) {
    CN_UNUSED(lexer);
    CN_TODO("AST attribute specifier sequence.");
}

CNDEF Cn_Ast_Linked_List cn_ast_parse_gnu_attribute_specifier_sequence(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Idx attribute_specifier_idx;
    Cn_Ast_Linked_List sequence = {0};

    while (true) {
        if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            *ok = true;
            return sequence;
        }

        attribute_specifier_idx = cn_ast_parse_gnu_attribute_specifier(lexer);
        if (attribute_specifier_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(&sequence, attribute_specifier_idx);
    }

error:
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Linked_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_GNU_ATTRIBUTE_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '__attribute__' token in GNU attribute specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' token in GNU attribute specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' token in GNU attribute specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    bool ok;
    node.gnu_attribute_specifier.gnu_attribute_list = cn_ast_parse_gnu_attribute_list(lexer, &ok); 
    if (!ok) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' token in GNU attribute specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' token in GNU attribute specifier.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_linked_list_set_parent(parent, &node.gnu_attribute_specifier.gnu_attribute_list);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Linked_List cn_ast_parse_gnu_attribute_list(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Linked_List list = {0};

    Cn_Ast_Idx gnu_attribute = cn_ast_parse_gnu_attribute(lexer);
    if (gnu_attribute == CN_AST_NIL_IDX) goto error;
    cn_ast_linked_list_add(&list, gnu_attribute);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        gnu_attribute = cn_ast_parse_gnu_attribute(lexer);
        if (gnu_attribute == CN_AST_NIL_IDX) goto error;
        cn_ast_linked_list_add(&list, gnu_attribute);
    }

    *ok = true;
    return list;

error:
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Linked_List) {0};
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_attribute(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_GNU_ATTRIBUTE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
    if (identifier_idx == CN_AST_NIL_IDX) goto error;
    node.gnu_attribute.identifier_idx = identifier_idx;

    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_lexer_next_token(lexer);

        bool ok;
        node.gnu_attribute.argument_list = cn_ast_parse_argument_list(lexer, &ok);
        if (!ok) goto error;

        if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' token in GNU attribute.");
            goto error;
        }
        cn_lexer_next_token(lexer);
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.gnu_attribute.identifier_idx);
    cn_ast_linked_list_set_parent(parent, &node.gnu_attribute.argument_list);
    return parent;

error:
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_gnu_asm_label(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_NODE_GNU_ASM_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_ASM)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'asm' token in GNU asm label.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' token in GNU asm label.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (cn_lexer_expect(lexer, CN_TOKEN_STRING)) {
        Cn_Ast_Idx string_idx = cn_ast_parse_string(lexer);
        if (string_idx == CN_AST_NIL_IDX) goto error;
        node.gnu_asm_label.string_idx = string_idx;
    }

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' token in GNU asm label.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.gnu_asm_label.string_idx);
    return parent;

error:
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

CNDEF Cn_Type *cn_ast_to_type(Cn_Qualifier_Flags flags, Cn_Ast_Idx type_specifier_idx, Cn_Ast_Idx declarator_idx) {
    Cn_Type type = {0};

    Cn_Ast_Node *ts = cn_ast_node_get(type_specifier_idx);
    Cn_Type *result;
    
    // Type specifier to type.
    switch (ts->type_specifier.kind) {
        case CN_AST_TYPE_INT:
            type.kind = CN_INTEGER;
            type.flags |= CN_TYPE_COMPLETE;

            switch (ts->type_specifier.width) {
                case CN_AST_TYPE_WIDTH_NONE:
                    type.size = sizeof(int);
                    break;
                case CN_AST_TYPE_WIDTH_SHORT:
                    type.size = sizeof(short);
                    break;
                case CN_AST_TYPE_WIDTH_LONG:
                    type.size = sizeof(long);
                    break;
                case CN_AST_TYPE_WIDTH_LONG_LONG:
                    type.size = sizeof(long long);
                    break;
            }
            type.align = type.size;

            type.t_integer.is_signed = ts->type_specifier.sign != CN_AST_TYPE_SIGN_UNSIGNED;

            result = cn__ast_add_type_if_not(&type);
            break;
        case CN_AST_TYPE_CHAR:
            type.kind = CN_INTEGER;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(char);
            type.align = type.size;

            type.t_integer.is_signed = ts->type_specifier.sign != CN_AST_TYPE_SIGN_UNSIGNED;

            result = cn__ast_add_type_if_not(&type);
            break;
        case CN_AST_TYPE_FLOAT:
            type.kind = CN_FLOAT;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(float);
            type.align = type.size;

            result = cn__ast_add_type_if_not(&type);
            break;
        case CN_AST_TYPE_DOUBLE:
            type.kind = CN_FLOAT;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(double);
            type.align = type.size;

            result = cn__ast_add_type_if_not(&type);
            break;
        case CN_AST_TYPE_BOOL:
            type.kind = CN_BOOL;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(_Bool);
            type.align = type.size;

            result = cn__ast_add_type_if_not(&type);
            break;
        case CN_AST_TYPE_VOID:
            result = cn__ast_add_type_if_not((Cn_Type*)&CN_TYPE_VOID);
            break;
        case CN_AST_TYPE_STRUCT_OR_UNION:
            // Tagged types, are handled a little bit different from purely structural types.
            // Tags are used to reference types, references to such types is not stored 
            // in type set. Instead tag bindings are used.
            // And those types data still is stored in the same arena as every other type.
            // Tagged types are unique not just by name, but by both name and scope.
            // Since one tag with the same name can shadow tagged type from 
            // outer scope with the same name.
            // C also allows stucts and unions to be declared but not defined.
            // Such is possible because incomplete types are allowed.
            // Types are completed once definition is found in the same scope 
            // with the same tag.
            Cn_Ast_Node *struct_or_union = cn_ast_node_get(ts->type_specifier.struct_or_union_idx);
            Cn_String tag = cn_ast_node_get(struct_or_union->struct_specifier.identifier_idx)->identifier.name;
            if (struct_or_union->kind == CN_AST_NODE_STRUCT_SPECIFIER) {
                Cn_Ast_Binding_Idx binding_idx = cn_ast_tag_binding_declare(tag, CN_STRUCT);

                if (binding_idx == CN_AST_NIL_BINDING_IDX) return NULL;

                Cn_Ast_Binding *binding = cn__ast_data->binding_list + binding_idx;

                if (struct_or_union->struct_specifier.member_declaration_list.length > 0) {
                    // Checking if it is redifinition.
                    if ((binding->type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(struct_or_union), CN_DC_REDEFINITION, "Redefinition of 'struct %.*s' is not allowed within the same scope.", CN_UNPACK(tag));

                        return NULL;
                    }

                    // Defining struct below.
                    // Making sure to set is complete at the end.
                    // First determining members length.
                    binding->type->t_struct.members_length = 0;
                    cn_ast_linked_list_foreach(m, &struct_or_union->struct_specifier.member_declaration_list) {
                       binding->type->t_struct.members_length += m->member_declaration.member_declarator_list.length;
                    }

                    binding->type->t_struct.members = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, binding->type->t_struct.members_length * sizeof(Cn_Type_Struct_Member));

                    // Converting members, and figuring out sizes for the struct at the same time.
                    Cn_Ast_Node *spec;
                    Cn_Type *member_type;
                    int i = 0;

                    int64_t offset = 0, max_align = 0;
                    cn_ast_linked_list_foreach(m, &struct_or_union->struct_specifier.member_declaration_list) {
                        spec = cn_ast_node_get(m->member_declaration.specifier_qualifier_idx);
                        cn_ast_linked_list_foreach(d, &m->member_declaration.member_declarator_list) {
                            // Getting member type.
                            member_type = cn_ast_to_type(spec->specifier_qualifier.qualifiers, spec->specifier_qualifier.type_specifier_idx, d->member_declarator.declarator_idx);
                            if (!(member_type->flags & CN_TYPE_COMPLETE)) {    
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(d), CN_DC_INCOMPLETE_TYPE, "Cannot have incomplete member type in struct definition.");

                                return NULL;
                            }


                            Cn_Ast_Idx identifier_idx;
                            cn_get_declarator_info(d->member_declarator.declarator_idx, &identifier_idx);

                            // Adding member to struct type.
                            binding->type->t_struct.members[i] = (Cn_Type_Struct_Member) { 
                                .type = member_type, 
                                .name = cn__ast_permanent_save_string(cn_ast_node_get(identifier_idx)->identifier.name),
                            };
                            
                            // Setting offset to be next alligned offset and comparing with max align.
                            CN_ASSERT(member_type->align != 0);
                            offset = (offset + member_type->align - 1) / member_type->align * member_type->align;
                            binding->type->t_struct.members[i].offset = offset;
                            offset += member_type->size;

                            if (max_align < member_type->align) {
                                max_align = member_type->align;
                            }

                            // IMPORTANT: Incrementing to next member idx.
                            i++;
                        }
                    }

                    binding->type->flags |= CN_TYPE_COMPLETE;
                    binding->type->size = offset;
                    binding->type->align = max_align;
                }

                result = binding->type;
            } else {
                Cn_Ast_Binding_Idx binding_idx = cn_ast_tag_binding_declare(tag, CN_UNION);

                if (binding_idx == CN_AST_NIL_BINDING_IDX) return NULL;

                Cn_Ast_Binding *binding = cn__ast_data->binding_list + binding_idx;

                if (struct_or_union->union_specifier.member_declaration_list.length > 0) {
                    // Redefinition check.
                    if ((binding->type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(struct_or_union), CN_DC_REDEFINITION, "Redefinition of 'union %.*s' is not allowed within the same scope.", CN_UNPACK(tag));

                        return NULL;
                    }

                    // Count members.
                    binding->type->t_union.members_length = 0;
                    cn_ast_linked_list_foreach(m, &struct_or_union->union_specifier.member_declaration_list) {
                        binding->type->t_union.members_length += m->member_declaration.member_declarator_list.length;
                    }

                    binding->type->t_union.members = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, binding->type->t_union.members_length * sizeof(Cn_Type_Union_Member));

                    Cn_Ast_Node *spec;
                    Cn_Type *member_type;
                    int i = 0;

                    int64_t max_size = 0;
                    int64_t max_align = 0;

                    cn_ast_linked_list_foreach(m, &struct_or_union->union_specifier.member_declaration_list) {
                        spec = cn_ast_node_get(m->member_declaration.specifier_qualifier_idx);

                        cn_ast_linked_list_foreach(d, &m->member_declaration.member_declarator_list) {
                            member_type = cn_ast_to_type(spec->specifier_qualifier.qualifiers, spec->specifier_qualifier.type_specifier_idx, d->member_declarator.declarator_idx);

                            if (!(member_type->flags & CN_TYPE_COMPLETE)) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(d), CN_DC_INCOMPLETE_TYPE, "Cannot have incomplete member type in union definition.");

                                return NULL;
                            }

                            Cn_Ast_Idx identifier_idx;
                            cn_get_declarator_info(d->member_declarator.declarator_idx, &identifier_idx);

                            binding->type->t_union.members[i] = (Cn_Type_Union_Member) {
                                    .type = member_type,
                                    .name = cn__ast_permanent_save_string(cn_ast_node_get(identifier_idx)->identifier.name),
                                };

                            if (member_type->size > max_size) {
                                max_size = member_type->size;
                            }

                            if (member_type->align > max_align) {
                                max_align = member_type->align;
                            }

                            i++;
                        }
                    }

                    // Final union size must satisfy its own alignment.
                    if (max_align != 0) {
                        max_size = (max_size + max_align - 1) / max_align * max_align;
                    }

                    binding->type->flags |= CN_TYPE_COMPLETE;
                    binding->type->size = max_size;
                    binding->type->align = max_align;
                }

                result = binding->type;
            }
            break;
        case CN_AST_TYPE_TYPEDEF:
            Cn_Ast_Binding_Idx *ref = cn_hash_table_get(&cn__ast_data->symbol_binding_table, &ts->type_specifier.typedef_name);
            result = cn__ast_data->binding_list[*ref].type;
            break;
        case CN_AST_TYPE_GNU_TYPEOF:
            {
                Cn_Ast_Node *typeof_s = cn_ast_node_get(ts->type_specifier.gnu_typeof_idx);
                Cn_Ast_Node *typeof_child = cn_ast_node_get(typeof_s->gnu_typeof_specifier.expression_or_type_name_idx);

                if (typeof_child->kind == CN_AST_NODE_TYPE_NAME) {
                    Cn_Ast_Node *sq = cn_ast_node_get(typeof_child->type_name.specifier_qualifier_idx);
                    result = cn_ast_to_type(sq->specifier_qualifier.qualifiers, sq->specifier_qualifier.type_specifier_idx, typeof_child->type_name.abstract_declarator_idx);
                } else {
                    result = cn_ast_expression_get_type(typeof_s->gnu_typeof_specifier.expression_or_type_name_idx);
                }
            }
            break;
        case CN_AST_TYPE_NONE:
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(ts), CN_DC_INVALID_TYPE_SPECIFIER, "Unknown or invalid type specifier.");
            return NULL;
    }

    // Qualifying type.
    if (flags != 0) {
        type = cn_type_make_qualified((Cn_Type_Qualified_Flags)flags, result);
        result = cn__ast_add_type_if_not(&type);
    }

    {
        // Declarator traversal.
        Cn_Ast_Idx declarator_chain_idx = declarator_idx;

        while (declarator_chain_idx != CN_AST_NIL_IDX) {
            Cn_Ast_Node *decl = cn_ast_node_get(declarator_chain_idx);

            // Pointer traversal: wrap result in pointer (+ qualifiers) per '*'.
            Cn_Ast_Idx pointer_idx = decl->declarator.pointer_idx;
            while (pointer_idx != CN_AST_NIL_IDX) {
                Cn_Ast_Node *ptr = cn_ast_node_get(pointer_idx);

                type = cn_type_make_pointer(result);
                result = cn__ast_add_type_if_not(&type);

                if (ptr->pointer.qualifiers != 0) {
                    type = cn_type_make_qualified((Cn_Type_Qualified_Flags)ptr->pointer.qualifiers, result);
                    result = cn__ast_add_type_if_not(&type);
                }

                pointer_idx = ptr->pointer.pointer_idx;
            }

            // Direct declarator traversal: wrap result in array/function layers until
            // we hit the identifier, an abstract end (NIL), or a grouped declarator.
            Cn_Ast_Idx dd_idx = decl->declarator.direct_declarator_idx;

            // Non-NIL only if we descend into a grouped '(' declarator ')'.
            Cn_Ast_Idx next_declarator_idx = CN_AST_NIL_IDX;

            while (dd_idx != CN_AST_NIL_IDX) {
                Cn_Ast_Node *dd = cn_ast_node_get(dd_idx);

                if (dd->kind == CN_AST_NODE_IDENTIFIER) {
                    break;
                }

                if (dd->direct_declarator.kind == CN_AST_DIRECT_DECLARATOR_GROUPED) {
                    next_declarator_idx = dd->direct_declarator.declarator_idx;
                    break;
                }

                switch (dd->direct_declarator.kind) {
                    case CN_AST_DIRECT_DECLARATOR_ARRAY: 
                        {
                            type = (Cn_Type) { .kind = CN_ARRAY };
                            type.flags |= CN_TYPE_COMPLETE;

                            // Allow only complete types.
                            if (!(result->flags & CN_TYPE_COMPLETE)) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(dd), CN_DC_INCOMPLETE_TYPE, "Incomplete type not allowed in array declarator.");
                                return NULL;
                            }

                            type.t_array.element_type = result;

                            // Evaluate constant expression, diagnose if unable to get array size.
                            // TODO: Handle VLA.
                            uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
                            Cn_Any any = cn_ast_expression_evaluate(dd->direct_declarator.array.expression_idx, buffer);

                            if (cn_any_is_empty(any)) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(dd), CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in array declarator.");
                                return NULL;
                            }

                            if (any.type->kind != CN_INTEGER) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(dd), CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array declarator.");
                                return NULL;
                            }

                            int64_t length = cn_any_read_int(any);

                            if (length == 0) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(dd), CN_DC_INVALID_CONSTANT_EXPRESSION, "0 length array resulting from constant expression in array declarator.");
                                return NULL;
                            }

                            type.t_array.length = length;
                            type.size = length * result->size;
                            type.align = result->align;

                            result = cn__ast_add_type_if_not(&type);

                            dd_idx = dd->direct_declarator.array.direct_declarator_idx;
                            break;
                        }

                    case CN_AST_DIRECT_DECLARATOR_FUNCTION: 
                        {
                            type = (Cn_Type) { .kind = CN_FUNCTION };
                            type.size = 0;
                            type.align = 0;
                            type.t_function.return_type = result;

                            Cn_Ast_Node *params_list = cn_ast_node_get(dd->direct_declarator.function.parameter_type_list_idx);

                            // Is true if function declarator contains variadic args.
                            type.t_function.is_variadic = params_list->parameter_type_list.variadic_args;

                            // Iterating over parameters, converting them to types.
                            type.t_function.params_length = params_list->parameter_type_list.parameter_declaration_list.length;

                            if (type.t_function.params_length > 0) {
                                type.t_function.params = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, sizeof(Cn_Type_Function_Param) * type.t_function.params_length);

                                Cn_Ast_Node *decl_spec;
                                Cn_Type *param_type;
                                int64_t i = 0;
                                cn_ast_linked_list_foreach(p, &params_list->parameter_type_list.parameter_declaration_list) {
                                    decl_spec = cn_ast_node_get(p->parameter_declaration.declaration_specifiers_idx);
                                    param_type = cn_ast_to_type(decl_spec->declaration_specifiers.qualifiers, decl_spec->declaration_specifiers.type_specifier_idx, p->parameter_declaration.declarator_idx);

                                    type.t_function.params[i] = (Cn_Type_Function_Param) {              
                                        .type = param_type,
                                    };
                                    i++;
                                }
                            }

                            result = cn__ast_add_type_if_not(&type);

                            dd_idx = dd->direct_declarator.function.direct_declarator_idx;
                            break;
                        }

                    default:
                        dd_idx = CN_AST_NIL_IDX;
                        break;
                }
            }

            // Only loops again when a grouped declarator handed us an inner one.
            declarator_chain_idx = next_declarator_idx;
        }
    }
    
    return result;
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
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);

    if (node->kind == CN_AST_NODE_INTEGER) {
        // TODO: Replace cn_str_parse_int with a proper parsing constant expression evaluation.
        Cn_String s = cn_source_to_str(&node->src);
        return cn_str_parse_int(s) == 0;
    }

    return false;
}

CNDEF bool cn__ast_is_lvalue(Cn_Ast_Idx expression_idx) {
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);

    switch (node->kind) {
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            // Only identifiers denote objects; integer/float/string literals don't.
            return cn_ast_node_get(node->primary_expression.literal_idx)->kind == CN_AST_NODE_IDENTIFIER;

        case CN_AST_NODE_UNARY_EXPRESSION:
            // *p is an lvalue; -x, !x, &x, etc. are not.
            return node->unary_expression.operator_kind == CN_UNARY_OP_DEREF;

        case CN_AST_NODE_ACCESS_EXPRESSION:
            // p->m is always an lvalue; s.m is an lvalue iff s is one.
            if (!node->access_expression.pointer) {
                return cn__ast_is_lvalue(node->access_expression.expression_idx);
            }
            return true;

        case CN_AST_NODE_BINARY_EXPRESSION:
            // a[b] is an lvalue; no other binary result is.
            return node->binary_expression.operator_kind == CN_BINARY_OP_ARRAY_SUB;

        default:
            return false;
    }
}

/**
 * Expects expression_idx to be typechecked.
 */
CNDEF bool cn__ast_is_modifiable_lvalue(Cn_Ast_Idx expression_idx) {
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);
    Cn_Type *type = NULL;

    switch (node->kind) {
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            if (cn_ast_node_get(node->primary_expression.literal_idx)->kind == CN_AST_NODE_IDENTIFIER) {
                // Only non-constant identifiers denote objects; integer/float/string literals don't.
                type = node->primary_expression.type;

                if (cn_type_is_constant(type)) return false;

                return true;
            }

            return false;

        case CN_AST_NODE_UNARY_EXPRESSION:
            // *p is an lvalue; -x, !x, &x, etc. are not.
            // *p type must be non constant.
            if (node->unary_expression.operator_kind == CN_UNARY_OP_DEREF) {
                type = node->unary_expression.type;
                if (cn_type_is_constant(type)) return false;

                return true;
            }
            
            return false;

        case CN_AST_NODE_ACCESS_EXPRESSION:
            // p->m is always an lvalue; s.m is an lvalue iff s is one.
            // Base type of p must not be constant.
            // p->m type must not be constant.
            // s.m type must not be constant.
            type = node->access_expression.type;
            if (cn_type_is_constant(type)) return false;

            if (!node->access_expression.pointer) {
                return cn__ast_is_modifiable_lvalue(node->access_expression.expression_idx);
            } 

            Cn_Type *p = cn_ast_expression_get_type(node->access_expression.expression_idx);
            p = cn_type_unqualified(p);
            if (p->kind != CN_POINTER) return false;
            if (cn_type_is_constant(p->t_pointer.ptr_to)) return false;

            return true;

        case CN_AST_NODE_BINARY_EXPRESSION:
            // a[b] is an lvalue; no other binary result is.
            // a[b] must not be constant.
            if (node->binary_expression.operator_kind == CN_BINARY_OP_ARRAY_SUB) {
                type = node->binary_expression.type;
                if (cn_type_is_constant(type)) return false;

                return true;
            }

            return false;

        default:
            return false;
    }
}

CNDEF Cn_Type *cn__ast_binary_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_NODE_BINARY_EXPRESSION);

    Cn_Type *left = cn_ast_expression_typecheck(node->binary_expression.left_expression_idx);
    if (left == NULL) return NULL;

    Cn_Type *right = cn_ast_expression_typecheck(node->binary_expression.right_expression_idx);
    if (right == NULL) return NULL;

    switch (node->binary_expression.operator_kind) {
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

                return left->t_pointer.ptr_to;
            }

        // Arithmetic, usual arithmetic conversions.
        case CN_BINARY_OP_MULTIPLICATION:
        case CN_BINARY_OP_DIVISION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn__ast_usual_arithmetic_conversion(left, right);
                }
                cn__ast_illegal_binary(node, left, right,
                        node->binary_expression.operator_kind == CN_BINARY_OP_MULTIPLICATION
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
    CN_ASSERT(node->kind == CN_AST_NODE_ACCESS_EXPRESSION);

    Cn_Type *base = cn_ast_expression_typecheck(node->access_expression.expression_idx);
    if (base == NULL) return NULL;

    // Removing qualifiers from base type here, won't need them.
    base = cn_type_unqualified(base);

    Cn_Type *struct_type;
    if (node->access_expression.pointer) {
        // '->' : operand must be a pointer to a struct/union.
        if (base->kind != CN_POINTER) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "'->' requires a pointer operand.");
            return NULL;
        }
        struct_type = base->t_pointer.ptr_to;
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

    Cn_Ast_Node *ident = cn_ast_node_get(node->access_expression.identifier_idx);
    for (int64_t i = 0; i < struct_type->t_struct.members_length; i++) {
        if (cn_str_equals(&struct_type->t_struct.members[i].name, &ident->identifier.name)) {
            return struct_type->t_struct.members[i].type;
        }
    }

    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_SYMBOL, "No member named '%.*s' in struct.", CN_UNPACK(ident->identifier.name));
    return NULL;
}

CNDEF Cn_Type *cn__ast_function_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_NODE_FUNCTION_EXPRESSION);

    Cn_Type *callee = cn_ast_expression_typecheck(node->function_expression.expression_idx);
    if (callee == NULL) return NULL;

    // If opaque, ignore type checking for this function, completely.
    if (callee->kind == CN_OPAQUE) return callee;

    // A function itself or a pointer to function may be called. Getting function type itself.
    Cn_Type *fn = callee;
    if (fn->kind == CN_POINTER) fn = fn->t_pointer.ptr_to;

    if (fn->kind != CN_FUNCTION) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Called object is not a function or function pointer.");
        return NULL;
    }

    // Argument count must match parameter count.
    // NOTE: variadics aren't tracked in Cn_Type_Function yet, so this is exact-match only.
    int64_t arg_count = node->function_expression.argument_list.length;
    if (fn->t_function.is_variadic) {
        if (arg_count < fn->t_function.params_length) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Variadic function expects at least %lld argument(s) but %lld were provided.", fn->t_function.params_length, arg_count);
            return NULL;
        }
    } else if (arg_count != fn->t_function.params_length) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Function expects %lld argument(s) but %lld were provided.", fn->t_function.params_length, arg_count);
        return NULL;
    }

    // Each argument must be assignable to its corresponding parameter type.
    // NOTE: Not using linked list foreach cause we need to supply idx's to typechecking function.
    Cn_Ast_Idx arg_idx = node->function_expression.argument_list.first_idx;
    int64_t i = 0;
    while (arg_idx != CN_AST_NIL_IDX) {
        Cn_Type *arg_type = cn_ast_expression_typecheck(arg_idx);
        if (arg_type == NULL) return NULL;

        // If parameter is not part of variadic list, typechecking with function param type, otherwise ignoring.
        if (i < fn->t_function.params_length) {
            Cn_Type *param_type = fn->t_function.params[i].type;

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

        arg_idx = cn_ast_node_get(arg_idx)->next_idx;
        i++;
    }

    return fn->t_function.return_type;
}

CNDEF Cn_Type *cn__ast_unary_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_NODE_UNARY_EXPRESSION);

    Cn_Ast_Idx operand_idx = node->unary_expression.expression_idx;
    Cn_Type *operand = cn_ast_expression_typecheck(operand_idx);
    if (operand == NULL) return NULL;

    switch (node->unary_expression.operator_kind) {
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

                if (operand->t_pointer.ptr_to->kind == CN_VOID) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Cannot dereference a void * type.");
                    return NULL;
                }

                return operand->t_pointer.ptr_to;
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
    CN_ASSERT(node->kind == CN_AST_NODE_CAST_EXPRESSION);

    // Resolve the target type from the typename.
    Cn_Ast_Node *type_name = cn_ast_node_get(node->cast_expression.type_name_idx);
    Cn_Ast_Node *spec_qual = cn_ast_node_get(type_name->type_name.specifier_qualifier_idx);
    Cn_Type *target = cn_ast_to_type(spec_qual->specifier_qualifier.qualifiers, spec_qual->specifier_qualifier.type_specifier_idx, type_name->type_name.abstract_declarator_idx);
    if (target == NULL) return NULL;

    Cn_Type *operand = cn_ast_expression_typecheck(node->cast_expression.expression_idx);
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

CNDEF Cn_Type *cn__ast_sizeof_expression_typecheck(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_NODE_SIZEOF_EXPRESSION);

    Cn_Ast_Node *child = cn_ast_node_get(node->sizeof_expression.child_idx);

    Cn_Type *operand;
    if (child->kind == CN_AST_NODE_TYPE_NAME) {
        // Case: sizeof typename.
        Cn_Ast_Node *spec_qual = cn_ast_node_get(child->type_name.specifier_qualifier_idx);
        operand = cn_ast_to_type(
            spec_qual->specifier_qualifier.qualifiers,
            spec_qual->specifier_qualifier.type_specifier_idx,
            child->type_name.abstract_declarator_idx);
    } else {
        // Case: sizeof expr, operand is not evaluated; only its type is needed.
        operand = cn_ast_expression_typecheck(node->sizeof_expression.child_idx);
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
    CN_ASSERT(node->kind == CN_AST_NODE_TERNARY_EXPRESSION);

    Cn_Type *cond = cn_ast_expression_typecheck(node->ternary_expression.condition_expression_idx);
    if (cond == NULL) return NULL;

    // Condition must be scalar.
    if (!cn_type_is_scalar(cond)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Condition of '?:' must be of scalar type.");
        return NULL;
    }

    Cn_Type *a = cn_ast_expression_typecheck(node->ternary_expression.true_expression_idx);
    if (a == NULL) return NULL;

    Cn_Type *b = cn_ast_expression_typecheck(node->ternary_expression.false_expression_idx);
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
        Cn_Ast_Idx ia = node->ternary_expression.true_expression_idx;
        Cn_Ast_Idx ib = node->ternary_expression.false_expression_idx;

        // pointer  ?:  null pointer constant  ->  the pointer type.
        if (a->kind == CN_POINTER && cn__ast_is_null_pointer_constant(ib)) return a;
        if (b->kind == CN_POINTER && cn__ast_is_null_pointer_constant(ia)) return b;

        if (a->kind == CN_POINTER && b->kind == CN_POINTER) {
            // If either side points to void, the result is pointer-to-void.
            if (a->t_pointer.ptr_to->kind == CN_VOID) return a;
            if (b->t_pointer.ptr_to->kind == CN_VOID) return b;
            // Compatible pointed-to types -> that pointer type.
            if (cn_type_is_compatible(a->t_pointer.ptr_to, b->t_pointer.ptr_to)) return a;
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
    CN_ASSERT(node->kind == CN_AST_NODE_ASSIGNMENT_EXPRESSION);

    Cn_Type *left = cn_ast_expression_typecheck(node->assignment_expression.left_expression_idx);
    if (left == NULL) return NULL;

    Cn_Type *right = cn_ast_expression_typecheck(node->assignment_expression.right_expression_idx);
    if (right == NULL) return NULL;

    // Left must be a modifiable lvalue for every assignment form.
    if (!cn__ast_is_modifiable_lvalue(node->assignment_expression.left_expression_idx)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_ILLEGAL_TYPE, "Left operand of assignment is not a modifiable lvalue.");
        return NULL;
    }

    switch (node->assignment_expression.operator_kind) {
        // Simple assignment: right must be assignable to left, with NULL constant to pointer case handled too.
        case CN_ASSIGNMENT_OP_ASSIGN: 
            {
                if (!cn_type_is_assignable(left, right)) {

                    if (!(left->kind == CN_POINTER && cn__ast_is_null_pointer_constant(node->assignment_expression.right_expression_idx))) {
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
                cn__ast_illegal_binary(node, left, right, node->assignment_expression.operator_kind == CN_ASSIGNMENT_OP_PLUS ? "'+=' assignment" : "'-=' assignment");
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
    CN_ASSERT(node->kind == CN_AST_NODE_POSTFIX_EXPRESSION);

    Cn_Ast_Idx operand_idx = node->postfix_expression.expression_idx;
    Cn_Type *operand = cn_ast_expression_typecheck(operand_idx);
    if (operand == NULL) return NULL;

    switch (node->postfix_expression.operator_kind) {
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
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);

    Cn_Type *result = NULL;

    switch(node->kind) {
        case CN_AST_NODE_BINARY_EXPRESSION: 
            {   
                result = cn__ast_binary_expression_typecheck(node);
                node->binary_expression.type = result;
                break;
            }
        case CN_AST_NODE_ACCESS_EXPRESSION:
            {
                result = cn__ast_access_expression_typecheck(node);
                node->access_expression.type = result;
                break;
            }
        case CN_AST_NODE_FUNCTION_EXPRESSION:
            {
                result = cn__ast_function_expression_typecheck(node);
                node->function_expression.type = result;
                break;
            }
        case CN_AST_NODE_UNARY_EXPRESSION:
            {
                result = cn__ast_unary_expression_typecheck(node);
                node->unary_expression.type = result;
                break;
            }
        case CN_AST_NODE_CAST_EXPRESSION:
            {
                result = cn__ast_cast_expression_typecheck(node);
                node->cast_expression.type = result;
                break;
            }
        case CN_AST_NODE_SIZEOF_EXPRESSION:
            {
                result = cn__ast_sizeof_expression_typecheck(node);
                node->sizeof_expression.type = result;
                break;
            }
        case CN_AST_NODE_TERNARY_EXPRESSION:
            {   
                result = cn__ast_ternary_expression_typecheck(node);
                node->ternary_expression.type = result;
                break;
            }
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            {
                result = cn__ast_assignment_expression_typecheck(node);
                node->assignment_expression.type = result;
                break;
            }
        case CN_AST_NODE_POSTFIX_EXPRESSION:
            {
                result = cn__ast_postfix_expression_typecheck(node);
                node->postfix_expression.type = result;
                break;
            }
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            {
                Cn_Ast_Node *literal = cn_ast_node_get(node->primary_expression.literal_idx);
                switch(literal->kind) {
                    case CN_AST_NODE_IDENTIFIER:
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
                    case CN_AST_NODE_INTEGER:
                        {   
                            result = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
                            break;
                        }
                    case CN_AST_NODE_FLOAT:
                        {   
                            result = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_FLOAT);
                            break;
                        }
                    case CN_AST_NODE_STRING:
                        {   
                            result  = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_CHAR);
                            Cn_Type const_char = cn_type_make_qualified(CN_TYPE_CONSTANT, result);
                            result = cn__ast_add_type_if_not(&const_char);
                            Cn_Type const_char_ptr = cn_type_make_pointer(result);
                            result = cn__ast_add_type_if_not(&const_char_ptr);
                            break;
                        }
                    default:
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_EXPECTED_EXPRESSION, "Expected primary expression to typecheck.");
                        break;
                }

                node->primary_expression.type = result;
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
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);

    switch(node->kind) {
        case CN_AST_NODE_BINARY_EXPRESSION:
            return node->binary_expression.type;
        case CN_AST_NODE_ACCESS_EXPRESSION:
            return node->access_expression.type;
        case CN_AST_NODE_FUNCTION_EXPRESSION:
            return node->function_expression.type;
        case CN_AST_NODE_UNARY_EXPRESSION:
            return node->unary_expression.type;
        case CN_AST_NODE_CAST_EXPRESSION:
            return node->cast_expression.type;
        case CN_AST_NODE_SIZEOF_EXPRESSION:
            return node->sizeof_expression.type;
        case CN_AST_NODE_TERNARY_EXPRESSION:
            return node->ternary_expression.type;
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            return node->assignment_expression.type;
        case CN_AST_NODE_POSTFIX_EXPRESSION:
            return node->postfix_expression.type;
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            return node->primary_expression.type;
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
    Cn_Type *rtype = node->binary_expression.type;
    if (rtype == NULL) return (Cn_Any) {0};
 
    Cn_Binary_Operator_Kind op = node->binary_expression.operator_kind;
    Cn_Ast_Idx left_idx  = node->binary_expression.left_expression_idx;
    Cn_Ast_Idx right_idx = node->binary_expression.right_expression_idx;
 
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
                    if ((common->kind == CN_INTEGER) && !common->t_integer.is_signed) {
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
        bool is_signed = urtype->t_integer.is_signed;
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

CNDEF Cn_Any cn__ast_function_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    // Cannot really evaluate runtime function during compile time.
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__ast_unary_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Unary_Operator_Kind op = node->unary_expression.operator_kind;
    Cn_Ast_Idx operand_idx  = node->unary_expression.expression_idx;
 
    // Not constant operations.
    if (op == CN_UNARY_OP_ADDROF || op == CN_UNARY_OP_DEREF || op == CN_UNARY_OP_INCREMENT || op == CN_UNARY_OP_DECREMENT) return (Cn_Any) {0};

    // Special case, positive operator doesn't really do anything, 
    // so just delegating buffer and evaluation to operand expression.
    if (op == CN_UNARY_OP_POSITIVE) {
        return cn_ast_expression_evaluate(operand_idx, buffer);
    }

    Cn_Type *rtype = node->unary_expression.type;
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

CNDEF Cn_Any cn__ast_cast_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *target = node->cast_expression.type;
    if (target == NULL) return (Cn_Any) {0};
 
    Cn_Type *ut = cn_type_unqualified(target);

    // Cast to void yields no value; cast to pointer has no known address.
    if (ut->kind == CN_VOID || ut->kind == CN_POINTER) return (Cn_Any) {0};
 
    uint8_t operand_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any operand_value = cn_ast_expression_evaluate(node->cast_expression.expression_idx, operand_buffer);
    if (operand_value.type == NULL) return (Cn_Any) {0};
 
    return cn_any_convert(operand_value, target, buffer);
}

CNDEF Cn_Any cn__ast_sizeof_expression_evaluate(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = node->sizeof_expression.type; // size_t
    if (rtype == NULL) return (Cn_Any) {0};

    Cn_Ast_Node *child = cn_ast_node_get(node->sizeof_expression.child_idx);

    Cn_Type *operand_type;
    if (child->kind == CN_AST_NODE_TYPE_NAME) {
        Cn_Ast_Node *sq = cn_ast_node_get(child->type_name.specifier_qualifier_idx);
        operand_type = cn_ast_to_type(sq->specifier_qualifier.qualifiers, sq->specifier_qualifier.type_specifier_idx, child->type_name.abstract_declarator_idx);
    } else {
        // The operand of sizeof is unevaluated, only its type is needed.
        operand_type = cn_ast_expression_get_type(node->sizeof_expression.child_idx);
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
    Cn_Type *rtype = node->ternary_expression.type;
    if (rtype == NULL) return (Cn_Any) {0};

    Cn_Type *urtype = cn_type_unqualified(rtype);

    // Only scalar conditionals evaluated.
    if (!cn_type_is_scalar(urtype) || urtype->kind == CN_POINTER) return (Cn_Any) {0};

    uint8_t cond_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any cond = cn_ast_expression_evaluate(node->ternary_expression.condition_expression_idx, cond_buffer);
    if (cond.type == NULL) return (Cn_Any) {0};

    Cn_Ast_Idx chosen = cn_any_is_zero(cond) ? node->ternary_expression.false_expression_idx : node->ternary_expression.true_expression_idx;

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
    any.type = node->primary_expression.type;

    Cn_Ast_Node *literal = cn_ast_node_get(node->primary_expression.literal_idx);
    switch (literal->kind) {
        case CN_AST_NODE_INTEGER:
            {
                uint64_t value;
                if (!cn_parse_int_literal(cn_source_to_str( &node->src), &value)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid integer literal in primary expression.");
                    return (Cn_Any) {0};
                }
                any = cn_any_write_int(any, (int64_t)value);
                break;
            }
        case CN_AST_NODE_FLOAT:
            {
                double value;
                if (!cn_parse_float_literal(cn_source_to_str(&node->src), &value)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(node), CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid float literal in primary expression.");
                    return (Cn_Any) {0};
                }
                any = cn_any_write_float(any, value);
                break;
            }
        case CN_AST_NODE_STRING:
            {
                return (Cn_Any) {0};
            }
        case CN_AST_NODE_IDENTIFIER:
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
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);

    switch(node->kind) {
        case CN_AST_NODE_BINARY_EXPRESSION: 
            {   
                return cn__ast_binary_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_ACCESS_EXPRESSION:
            {
                return cn__ast_access_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_FUNCTION_EXPRESSION:
            {
                return cn__ast_function_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_UNARY_EXPRESSION:
            {
                return cn__ast_unary_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_CAST_EXPRESSION:
            {
                return cn__ast_cast_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_SIZEOF_EXPRESSION:
            {
                return cn__ast_sizeof_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_TERNARY_EXPRESSION:
            {   
                return cn__ast_ternary_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            {
                return cn__ast_assignment_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_POSTFIX_EXPRESSION:
            {
                return cn__ast_postfix_expression_evaluate(node, buffer);
            }
        case CN_AST_NODE_PRIMARY_EXPRESSION:
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
    Cn_Ast_Node *node = cn_ast_node_get(expression_idx);

    switch(node->kind) {
        case CN_AST_NODE_BINARY_EXPRESSION:
            node->binary_expression.type = NULL;
            cn_ast_expression_clear_types(node->binary_expression.left_expression_idx);
            cn_ast_expression_clear_types(node->binary_expression.right_expression_idx);
            break;
        case CN_AST_NODE_ACCESS_EXPRESSION:
            node->access_expression.type = NULL;
            cn_ast_expression_clear_types(node->access_expression.expression_idx);
            break;
        case CN_AST_NODE_FUNCTION_EXPRESSION:
            node->function_expression.type = NULL;
            cn_ast_expression_clear_types(node->function_expression.expression_idx);

            Cn_Ast_Idx arg_idx = node->function_expression.argument_list.first_idx; 
            int64_t i = 0; 
            while (arg_idx != CN_AST_NIL_IDX) {
                cn_ast_expression_clear_types(arg_idx);
                arg_idx = cn_ast_node_get(arg_idx)->next_idx;
                i++;
            }
            break;
        case CN_AST_NODE_UNARY_EXPRESSION:
            node->unary_expression.type = NULL;
            cn_ast_expression_clear_types(node->unary_expression.expression_idx);
            break;
        case CN_AST_NODE_CAST_EXPRESSION:
            node->cast_expression.type = NULL;
            cn_ast_expression_clear_types(node->cast_expression.expression_idx);
            break;
        case CN_AST_NODE_SIZEOF_EXPRESSION:
            node->sizeof_expression.type = NULL;

            if (node->kind != CN_AST_NODE_TYPE_NAME) {
                cn_ast_expression_clear_types(node->sizeof_expression.child_idx);
            }
            break;
        case CN_AST_NODE_TERNARY_EXPRESSION:
            node->ternary_expression.type = NULL;
            cn_ast_expression_clear_types(node->ternary_expression.condition_expression_idx);
            cn_ast_expression_clear_types(node->ternary_expression.false_expression_idx);
            cn_ast_expression_clear_types(node->ternary_expression.true_expression_idx);
            break;
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            node->assignment_expression.type = NULL;
            cn_ast_expression_clear_types(node->assignment_expression.left_expression_idx);
            cn_ast_expression_clear_types(node->assignment_expression.right_expression_idx);
            break;
        case CN_AST_NODE_POSTFIX_EXPRESSION:
            node->postfix_expression.type = NULL;
            cn_ast_expression_clear_types(node->postfix_expression.expression_idx);
            break;
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            node->primary_expression.type = NULL;
            break;
        default: 
            break;
    }
}

#define cn_ast_is_nil(idx) ((idx) == CN_AST_NIL_IDX)
#define cn_ast_is(idx, k) (cn_ast_node_get(idx)->kind == (k))

CNDEF bool cn_ast_expect(Cn_Ast_Idx idx, Cn_Ast_Node_Kind kind) {
    if (!cn_ast_is(idx, kind)) { 
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, idx, CN_DC_EXPECTED_AST_NODE, "Expected %s ast node, but received %s node.", cn_ast_node_kind_name(kind), cn_ast_node_kind_name(cn_ast_node_get(idx)->kind)); 
        return false;
    }
    return true;
}

CNDEF bool cn_ast_reparse_translation_unit(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_TRANSLATION_UNIT)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    cn_ast_linked_list_foreach_idx(i, &node->translation_unit.external_declaration_list) {
        if (!cn_ast_reparse_external_declaration(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_external_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_EXTERNAL_DECLARATION)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    // Stray ';' case.
    if (cn_ast_is_nil(node->external_declaration.child_idx)) return true;

    switch (cn_ast_node_get(node->external_declaration.child_idx)->kind) {
        case CN_AST_NODE_ASM_DEFINITION:
            if (!cn_ast_reparse_asm_definition(node->external_declaration.child_idx)) return false;
            break;
        case CN_AST_NODE_FUNCTION_DEFINITION:
            if (!cn_ast_reparse_function_definition(node->external_declaration.child_idx)) return false;
            break;
        default:
            if (!cn_ast_reparse_declaration(node->external_declaration.child_idx)) return false;
            break;
    }

    return true;
}

CNDEF bool cn_ast_reparse_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_DECLARATION)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_reparse_declaration_specifiers(node->declaration.declaration_specifiers_idx)) return false;

    if (!cn_ast_is_nil(node->declaration.init_declarator_list_idx)) {
        if (!cn_ast_reparse_init_declarator_list(node->declaration.init_declarator_list_idx)) return false;
    }

    cn_ast_linked_list_foreach_idx(i, &node->declaration.gnu_attribute_specifier_sequence) {
        if (!cn_ast_reparse_gnu_attribute_specifier(i)) return false;
    }

    // Rebuild types and bindings, exactly like the parse path does.
    if (!cn_ast_is_nil(node->declaration.init_declarator_list_idx)) {
        if (!cn_ast_analyze_declaration(node->declaration.declaration_specifiers_idx, node->declaration.init_declarator_list_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_function_definition(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_FUNCTION_DEFINITION)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_reparse_declaration_specifiers(node->function_definition.declaration_specifiers_idx)) return false;

    if (!cn_ast_reparse_declarator(node->function_definition.declarator_idx)) return false;

    // Function definition type and bindings resolution.
    Cn_Ast_Binding_Idx function_binding_idx;

    bool ok = cn_ast_analyze_function_definition(
            node->function_definition.declaration_specifiers_idx, 
            node->function_definition.declarator_idx, 
            &function_binding_idx
            );

    if (!ok) return false;

    // Push function scope for parameters and body.
    cn_ast_scope_stack_push();

    // Bind function parameters.
    ok = cn_ast_bind_function_definition_params(function_binding_idx);
    if (!ok) return false;

    // Analyze compound statement contents using current scope, parameters already bound.
    if (!cn_ast_reparse_compound_statement(node->function_definition.compound_statement_idx, true)) return false;

    cn_ast_scope_stack_pop();

    return true;
}

CNDEF bool cn_ast_reparse_asm_definition(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_asm_definition");
}

CNDEF bool cn_ast_reparse_statement(Cn_Ast_Idx node_idx) {
    if (cn_ast_is_nil(node_idx)) return true;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    switch (node->kind) {
        case CN_AST_NODE_COMPOUND_STATEMENT:
            return cn_ast_reparse_compound_statement(node_idx, false);
        case CN_AST_NODE_SELECTION_STATEMENT:
            return cn_ast_reparse_selection_statement(node_idx);
        case CN_AST_NODE_ITERATION_STATEMENT:
            return cn_ast_reparse_iteration_statement(node_idx);
        case CN_AST_NODE_JUMP_STATEMENT:
            return cn_ast_reparse_jump_statement(node_idx);
        case CN_AST_NODE_LABELED_STATEMENT:
            return cn_ast_reparse_labeled_statement(node_idx);
        default:
            return cn_ast_reparse_expression_statement(node_idx);
    }
}

CNDEF bool cn_ast_reparse_compound_statement(Cn_Ast_Idx node_idx, bool use_current_scope) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_COMPOUND_STATEMENT)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!use_current_scope) cn_ast_scope_stack_push();

    cn_ast_linked_list_foreach_idx(i, &node->compound_statement.statement_or_declaration_list) {
        if (cn_ast_is(i, CN_AST_NODE_DECLARATION)) {
            if (!cn_ast_reparse_declaration(i)) {
                if (!use_current_scope) cn_ast_scope_stack_pop();
                return false;   
            }
        } 
        else {
            if (!cn_ast_reparse_statement(i)) {
                if (!use_current_scope) cn_ast_scope_stack_pop();
                return false;
            }
        }
    }

    if (!use_current_scope) cn_ast_scope_stack_pop();

    return true;
}

CNDEF bool cn_ast_reparse_selection_statement(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_SELECTION_STATEMENT)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    // Type check condition expression.
    if (cn_ast_expression_typecheck(node->selection_statement.condition_expression_idx) == NULL) return false;

    // Analyze statement.
    if (!cn_ast_reparse_statement(node->selection_statement.statement_idx)) return false;

    // Analyze else statement if present (only for if statements).
    if (!cn_ast_is_nil(node->selection_statement.else_statement_idx)) {
        if (!cn_ast_reparse_statement(node->selection_statement.else_statement_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_iteration_statement(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_iteration_statement");
}

CNDEF bool cn_ast_reparse_jump_statement(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_JUMP_STATEMENT)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    // Type check return expression if present.
    if (node->jump_statement.kind == CN_AST_JUMP_RETURN) {
        if (!cn_ast_is_nil(node->jump_statement.expression_idx)) {
            if (cn_ast_expression_typecheck(node->jump_statement.expression_idx) == NULL) return false;
        }
    }

    return true;
}

CNDEF bool cn_ast_reparse_labeled_statement(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_labeled_statement");
}

CNDEF bool cn_ast_reparse_expression_statement(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_EXPRESSION_STATEMENT)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->expression_statement.expression_idx)) {
        if (cn_ast_expression_typecheck(node->expression_statement.expression_idx) == NULL) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_argument_list(Cn_Ast_Idx node_idx) {
    CN_UNUSED(node_idx);
    CN_TODO("cn_ast_reparse_argument_list");
}

CNDEF bool cn_ast_reparse_expression(Cn_Ast_Idx node_idx) {
    if (cn_ast_is_nil(node_idx)) return true;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    switch (node->kind) {
        case CN_AST_NODE_BINARY_EXPRESSION:
            node->binary_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->binary_expression.left_expression_idx)) return false;
            if (!cn_ast_reparse_expression(node->binary_expression.right_expression_idx)) return false;
            break;
        case CN_AST_NODE_ACCESS_EXPRESSION:
            node->access_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->access_expression.expression_idx)) return false;
            break;
        case CN_AST_NODE_FUNCTION_EXPRESSION:
            node->function_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->function_expression.expression_idx)) return false;
            cn_ast_linked_list_foreach_idx(i, &node->function_expression.argument_list) {
                if (!cn_ast_reparse_expression(i)) return false;
            }
            break;
        case CN_AST_NODE_UNARY_EXPRESSION:
            node->unary_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->unary_expression.expression_idx)) return false;
            break;
        case CN_AST_NODE_CAST_EXPRESSION:
            node->cast_expression.type = NULL;
            if (!cn_ast_reparse_type_name(node->cast_expression.type_name_idx)) return false;
            if (!cn_ast_reparse_expression(node->cast_expression.expression_idx)) return false;
            break;
        case CN_AST_NODE_SIZEOF_EXPRESSION:
            node->sizeof_expression.type = NULL;
            if (!cn_ast_is_nil(node->sizeof_expression.child_idx)) {
                Cn_Ast_Node *child = cn_ast_node_get(node->sizeof_expression.child_idx);
                if (child->kind == CN_AST_NODE_TYPE_NAME) {
                    if (!cn_ast_reparse_type_name(node->sizeof_expression.child_idx)) return false;
                } else {
                    if (!cn_ast_reparse_expression(node->sizeof_expression.child_idx)) return false;
                }
            }
            break;
        case CN_AST_NODE_TERNARY_EXPRESSION:
            node->ternary_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->ternary_expression.condition_expression_idx)) return false;
            if (!cn_ast_reparse_expression(node->ternary_expression.true_expression_idx)) return false;
            if (!cn_ast_reparse_expression(node->ternary_expression.false_expression_idx)) return false;
            break;
        case CN_AST_NODE_ASSIGNMENT_EXPRESSION:
            node->assignment_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->assignment_expression.left_expression_idx)) return false;
            if (!cn_ast_reparse_expression(node->assignment_expression.right_expression_idx)) return false;
            break;
        case CN_AST_NODE_POSTFIX_EXPRESSION:
            node->postfix_expression.type = NULL;
            if (!cn_ast_reparse_expression(node->postfix_expression.expression_idx)) return false;
            break;
        case CN_AST_NODE_PRIMARY_EXPRESSION:
            node->primary_expression.type = NULL;
            break;
        default:
            break;
    }

    return true;
}

CNDEF bool cn_ast_reparse_init_declarator_list(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_INIT_DECLARATOR_LIST)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    cn_ast_linked_list_foreach_idx(i, &node->init_declarator_list.init_declarator_list) {
        if (!cn_ast_reparse_init_declarator(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_init_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_INIT_DECLARATOR)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_reparse_declarator(node->init_declarator.declarator_idx)) return false;

    if (!cn_ast_is_nil(node->init_declarator.initializer_idx)) {
        if (!cn_ast_reparse_initializer(node->init_declarator.initializer_idx)) return false;
    }

    if (!cn_ast_is_nil(node->init_declarator.gnu_asm_label_idx)) {
        if (!cn_ast_reparse_gnu_asm_label(node->init_declarator.gnu_asm_label_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_initializer(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_INITIALIZER)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->initializer.expression_idx)) {
        if (!cn_ast_reparse_expression(node->initializer.expression_idx)) return false;
    }

    // TODO: Initializer list.

    return true;
}

CNDEF bool cn_ast_reparse_abstract_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_ABSTRACT_DECLARATOR)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->declarator.pointer_idx)) {
        if (!cn_ast_reparse_pointer(node->declarator.pointer_idx)) return false;
    }

    if (!cn_ast_is_nil(node->declarator.direct_declarator_idx)) {
        if (!cn_ast_reparse_direct_declarator(node->declarator.direct_declarator_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_DECLARATOR)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->declarator.pointer_idx)) {
        if (!cn_ast_reparse_pointer(node->declarator.pointer_idx)) return false;
    }

    if (!cn_ast_is_nil(node->declarator.direct_declarator_idx)) {
        if (!cn_ast_reparse_direct_declarator(node->declarator.direct_declarator_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_pointer(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_POINTER)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->pointer.pointer_idx)) {
        if (!cn_ast_reparse_pointer(node->pointer.pointer_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_direct_declarator(Cn_Ast_Idx node_idx) {
    if (cn_ast_is(node_idx, CN_AST_NODE_IDENTIFIER)) return true;

    if (!cn_ast_expect(node_idx, CN_AST_NODE_DIRECT_DECLARATOR)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    switch (node->direct_declarator.kind) {
        case CN_AST_DIRECT_DECLARATOR_GROUPED:
            if (!cn_ast_reparse_declarator(node->direct_declarator.declarator_idx)) return false;
            break;
        case CN_AST_DIRECT_DECLARATOR_ARRAY:
            if (!cn_ast_is_nil(node->direct_declarator.array.direct_declarator_idx)) {
                if (!cn_ast_reparse_direct_declarator(node->direct_declarator.array.direct_declarator_idx)) return false;
            }
            if (!cn_ast_is_nil(node->direct_declarator.array.expression_idx)) {
                if (!cn_ast_reparse_expression(node->direct_declarator.array.expression_idx)) return false;
            }
            break;
        case CN_AST_DIRECT_DECLARATOR_FUNCTION:
            if (!cn_ast_is_nil(node->direct_declarator.function.direct_declarator_idx)) {
                if (!cn_ast_reparse_direct_declarator(node->direct_declarator.function.direct_declarator_idx)) return false;
            }
            if (!cn_ast_is_nil(node->direct_declarator.function.parameter_type_list_idx)) {
                if (!cn_ast_reparse_parameter_type_list(node->direct_declarator.function.parameter_type_list_idx)) return false;
            }
            break;
    }

    return true;
}

CNDEF bool cn_ast_reparse_declaration_specifiers(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_DECLARATION_SPECIFIERS)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->declaration_specifiers.type_specifier_idx)) {
        if (!cn_ast_reparse_type_specifier(node->declaration_specifiers.type_specifier_idx)) return false;
    }

    cn_ast_linked_list_foreach_idx(i, &node->declaration_specifiers.gnu_attribute_specifiers) {
        if (!cn_ast_reparse_gnu_attribute_specifier(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_type_specifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_TYPE_SPECIFIER)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (node->type_specifier.kind == CN_AST_TYPE_TYPEDEF) {
        Cn_Ast_Binding_Idx idx = cn_ast_binding_table_get(node->type_specifier.typedef_name, &cn__ast_data->symbol_binding_table);

        if (idx == CN_AST_NIL_BINDING_IDX || cn_ast_binding_get(idx)->kind != CN_BINDING_TYPEDEF) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node_idx, CN_DC_EXPECTED_TOKEN, "Expected valid typedef identifier in typedef type specifier.");
            return false;
        }

        return true;
    }

    if (node->type_specifier.kind == CN_AST_TYPE_STRUCT_OR_UNION) {
        if (!cn_ast_is_nil(node->type_specifier.struct_or_union_idx)) {
            if (!cn_ast_reparse_struct_or_union_specifier(node->type_specifier.struct_or_union_idx)) return false;
        }

        return true;
    }

    if (node->type_specifier.kind == CN_AST_TYPE_GNU_TYPEOF) {
        if (!cn_ast_is_nil(node->type_specifier.gnu_typeof_idx)) {
            if (!cn_ast_reparse_gnu_typeof_specifier(node->type_specifier.gnu_typeof_idx)) return false;
        }

        return true;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_typeof_specifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_GNU_TYPEOF_SPECIFIER)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->gnu_typeof_specifier.expression_or_type_name_idx)) {
        Cn_Ast_Node *child = cn_ast_node_get(node->gnu_typeof_specifier.expression_or_type_name_idx);
        if (child->kind == CN_AST_NODE_TYPE_NAME) {
            if (!cn_ast_reparse_type_name(node->gnu_typeof_specifier.expression_or_type_name_idx)) return false;
        } else {
            if (!cn_ast_reparse_expression(node->gnu_typeof_specifier.expression_or_type_name_idx)) return false;
        }
    }

    return true;
}

CNDEF bool cn_ast_reparse_type_name(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_TYPE_NAME)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_reparse_specifier_qualifier(node->type_name.specifier_qualifier_idx)) return false;

    if (!cn_ast_is_nil(node->type_name.abstract_declarator_idx)) {
        if (!cn_ast_reparse_abstract_declarator(node->type_name.abstract_declarator_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_specifier_qualifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_SPECIFIER_QUALIFIER)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->specifier_qualifier.type_specifier_idx)) {
        if (!cn_ast_reparse_type_specifier(node->specifier_qualifier.type_specifier_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_parameter_type_list(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_PARAMETER_TYPE_LIST)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    cn_ast_linked_list_foreach_idx(i, &node->parameter_type_list.parameter_declaration_list) {
        if (!cn_ast_reparse_parameter_declaration(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_parameter_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_PARAMETER_DECLARATION)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_reparse_declaration_specifiers(node->parameter_declaration.declaration_specifiers_idx)) return false;

    if (!cn_ast_is_nil(node->parameter_declaration.declarator_idx)) {
        Cn_Ast_Node *decl = cn_ast_node_get(node->parameter_declaration.declarator_idx);
        if (decl->kind == CN_AST_NODE_ABSTRACT_DECLARATOR) {
            if (!cn_ast_reparse_abstract_declarator(node->parameter_declaration.declarator_idx)) return false;
        } else {
            if (!cn_ast_reparse_declarator(node->parameter_declaration.declarator_idx)) return false;
        }
    }

    return true;
}

CNDEF bool cn_ast_reparse_struct_or_union_specifier(Cn_Ast_Idx node_idx) {
    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (node->kind != CN_AST_NODE_STRUCT_SPECIFIER && node->kind != CN_AST_NODE_UNION_SPECIFIER) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node_idx, CN_DC_EXPECTED_AST_NODE, "Expected struct or union specifier node.");
        return false;
    }

    cn_ast_linked_list_foreach_idx(i, &node->struct_specifier.member_declaration_list) {
        if (!cn_ast_reparse_member_declaration(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_member_declaration(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_MEMBER_DECLARATION)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_reparse_specifier_qualifier(node->member_declaration.specifier_qualifier_idx)) return false;

    cn_ast_linked_list_foreach_idx(i, &node->member_declaration.member_declarator_list) {
        if (!cn_ast_reparse_member_declarator(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_member_declarator(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_MEMBER_DECLARATOR)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    if (!cn_ast_is_nil(node->member_declarator.declarator_idx)) {
        if (!cn_ast_reparse_declarator(node->member_declarator.declarator_idx)) return false;
    }

    if (!cn_ast_is_nil(node->member_declarator.bitfield_expression_idx)) {
        if (!cn_ast_reparse_expression(node->member_declarator.bitfield_expression_idx)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_attribute_specifier(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_GNU_ATTRIBUTE_SPECIFIER)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    cn_ast_linked_list_foreach_idx(i, &node->gnu_attribute_specifier.gnu_attribute_list) {
        if (!cn_ast_reparse_gnu_attribute(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_attribute(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_GNU_ATTRIBUTE)) return false;

    Cn_Ast_Node *node = cn_ast_node_get(node_idx);

    cn_ast_linked_list_foreach_idx(i, &node->gnu_attribute.argument_list) {
        if (!cn_ast_reparse_expression(i)) return false;
    }

    return true;
}

CNDEF bool cn_ast_reparse_gnu_asm_label(Cn_Ast_Idx node_idx) {
    if (!cn_ast_expect(node_idx, CN_AST_NODE_GNU_ASM_LABEL)) return false;

    // String child doesn't need reparsing.

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
};

Cn_Diagnostic_Level cn_min_diagnostic_level = CN_DIAGNOSTIC_INFO;

Cn_Diagnostic_Handler *cn_diagnostic_handler = &cn_default_diagnostic_handler;

/**
 * Given current bol, eol and source, it will find next bol 
 * with it's eol and overwrite supplied value of bol and eol.
 * RETURNS: True if successfuly found next bol, false otherwise.
 */
CNDEF bool cn__find_next_line(Cn_String source, int64_t *bol, int64_t *eol) {
    source = cn_str_eat_chars(source, *eol + strlen(CN_LINE_END));

    int64_t end = cn_str_find_left(source, CN_CSTR(CN_LINE_END));
    if (end < 0) return false;

    *bol = *eol + strlen(CN_LINE_END); 
    *eol = end + *bol;

    return true;
}

/**
 * Given current bol, eol and source, it will find previous bol 
 * with it's eol and overwrite supplied value of bol and eol.
 *
 * RETURNS: True if successfuly found previous bol, false otherwise.
 */
CNDEF bool cn__find_prev_line(Cn_String source, int64_t *bol, int64_t *eol) {
    if ((*bol - (int64_t)strlen(CN_LINE_END)) < 0) return false;

    source = cn_str_get_chars(source, *bol - strlen(CN_LINE_END));

    int64_t start = cn_str_find_right(source, CN_CSTR(CN_LINE_END));
    if (start < 0) 
        start = 0;
    else 
        start += strlen(CN_LINE_END);

    *eol = *bol - strlen(CN_LINE_END); 
    *bol = start;

    return true;
}

#define CN__DIAGNOSTIC_PREV_LINE_PRINT_COUNT 1
#define CN__DIAGNOSTIC_NEXT_LINE_PRINT_COUNT 1

CNDEF void cn_default_diagnostic_handler(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, const char *format, va_list args) {
    CN_UNUSED(level);
    CN_UNUSED(loc);
    CN_UNUSED(code);
    CN_UNUSED(span);
    CN_UNUSED(format);
    CN_UNUSED(args);
    CN_TODO("cn_default_diagnostic_handler");
    //if (level < cn_min_diagnostic_level)
    //    return;

    //const char *ansi_color = "";

    //switch (level) {
    //    case CN_DIAGNOSTIC_INFO:
    //        ansi_color = CN_ANSI_BRIGHT_BLUE;
    //        fprintf(stderr, CN_ANSI_BRIGHT_BLUE"info: "CN_ANSI_RESET);
    //        break;
    //    case CN_DIAGNOSTIC_WARNING:
    //        ansi_color = CN_ANSI_YELLOW;
    //        fprintf(stderr, CN_ANSI_YELLOW"warning: "CN_ANSI_RESET);
    //        break;
    //    case CN_DIAGNOSTIC_ERROR:
    //        ansi_color = CN_ANSI_BRIGHT_RED;
    //        fprintf(stderr, CN_ANSI_BRIGHT_RED"error: "CN_ANSI_RESET);
    //        break;
    //}

    //fprintf(stderr, CN_ANSI_BRIGHT_WHITE"%.*s:%ld:%ld:"CN_ANSI_RESET" ", CN_UNPACK(loc->file), loc->line, loc->column);

    //if (!cn_str_is_empty(CN_DIAGNOSTIC_CODES[code])) {
    //    fprintf(stderr, "%.*s:", CN_UNPACK(CN_DIAGNOSTIC_CODES[code]));
    //} else {
    //    fprintf(stderr, "CN%04d:", code);
    //}

    //fputs(CN_LINE_END, stderr);

    //Cn_String src = cn__ast_data->sourcei;
    //int64_t end = cn_str_find_left(cn_str_eat_chars(src, loc->bol), CN_CSTR(CN_LINE_END)) + loc->bol;

    //int64_t bol = loc->bol;
    //int64_t eol = end;
    //Cn_String line;

    //// Getting lines of all previous lines saved to temporary stack.
    //if (loc->line > 1) {
    //    Cn_String stack[CN__DIAGNOSTIC_PREV_LINE_PRINT_COUNT];
    //    int64_t len = 0;
    //    for (int64_t i = 0; i < CN__DIAGNOSTIC_PREV_LINE_PRINT_COUNT; i++) {
    //        if (len >= loc->line - 1 || !cn__find_prev_line(src, &bol, &eol)) break;

    //        stack[i] = cn_str_substring(src, bol, eol);
    //        len++;
    //    }

    //    // Printing previous lines.
    //    for (int64_t i = len; i > 0; i--) {
    //        line = stack[i - 1];
    //        fprintf(stderr, "%4lu | %.*s"CN_LINE_END, loc->line - i, CN_UNPACK(line));
    //    }
    //}

    //// Printing current line.
    //{
    //    line = cn_str_substring(src, loc->bol, end);

    //    int64_t underline_offset = loc->column - 1;
    //    fprintf(stderr, "%4lu | %.*s", loc->line, CN_UNPACK(cn_str_substring(line, 0, underline_offset)));
    //    fputs(ansi_color, stderr);
    //    fprintf(stderr, "%.*s"CN_ANSI_RESET, CN_UNPACK(cn_str_substring(line, underline_offset, underline_offset + loc->length)));
    //    fprintf(stderr, "%.*s"CN_LINE_END, CN_UNPACK(cn_str_substring(line, underline_offset + loc->length, line.length)));
    //    fprintf(stderr, "     | %*s", (int)underline_offset, "");
    //    fputs(ansi_color, stderr);
    //    fputc('^', stderr);

    //    for(int64_t i = 1; i < loc->length; i++)
    //        fputc('~', stderr);
    //    fputc(' ', stderr);
    //    vfprintf(stderr, format, args);
    //    fputs(CN_ANSI_RESET CN_LINE_END, stderr);
    //}


    //// Printing next lines.
    //bol = loc->bol;
    //eol = end;
    //for (int64_t i = 0; i < CN__DIAGNOSTIC_NEXT_LINE_PRINT_COUNT; i++) {
    //    if (!cn__find_next_line(src, &bol, &eol)) break;

    //    line = cn_str_substring(src, bol, eol);
    //    fprintf(stderr, "%4lu | %.*s"CN_LINE_END, loc->line + i + 1, CN_UNPACK(line));
    //}

}

CNDEF void cn_null_diagnostic_handler(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, const char *format, va_list args) {
    CN_UNUSED(level);
    CN_UNUSED(loc);
    CN_UNUSED(code);
    CN_UNUSED(span);
    CN_UNUSED(format);
    CN_UNUSED(args);
}

CNDEF void cn_diagnostic_src(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Source *src, Cn_Diagnostic_Code code, const char *format, ...) {
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

    CN_TODO("cn_diagnostic_src");
    // va_list args;
    // va_start(args, format);
    // cn_diagnostic_handler(level, loc, code, format, args);
    // va_end(args);
}

CNDEF void cn_diagnostic_node(Cn_Diagnostic_Level level, Cn_Ast_Idx idx, Cn_Diagnostic_Code code, const char *format, ...) {
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

    Cn_Ast_Node *node = cn_ast_node_get(idx);
    CN_TODO("cn_diagnostic_node");

    // va_list args;
    // va_start(args, format);
    // cn_diagnostic_handler(level, &node->loc, code, format, args);
    // va_end(args);

    // // TODO: Handle purely created nodes.
    // if (node->flags & CN_AST_NODE_IS_REPLACED && node->replaced_idx != CN_AST_NIL_IDX) {
    //     node = cn_ast_node_get(node->replaced_idx);
    //     cn_diagnostic_node(CN_DIAGNOSTIC_INFO, node->replaced_idx, CN_DC_FROM, "From ast node here.");
    // }
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

    if (cn__ast_data->error_count > 0) {
        return -1;
    }

    // Printing AST.
    if (flags & CN_PRINT_AST) {
        cn_log(CN_INFO, "Parsed main.i:");
        cn_ast_print(cn_ast_node_get(idx), 0);
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
    Cn_Ast_Node *node = cn_ast_node_get(new);
    node->flags |= CN_AST_NODE_IS_REPLACED;
    node->replaced_idx = *original;
    node->parent_idx = parent_idx;
    *original = new;
}

CNDEF void cn_linked_list_insert(Cn_Ast_Idx *list_node, Cn_Ast_Idx new, Cn_Ast_Idx parent_idx) {
    if (*list_node == CN_AST_NIL_IDX) {
        cn_replace(list_node, new, parent_idx);
        return;
    }

    Cn_Ast_Node *node = cn_ast_node_get(new);
    node->flags |= CN_AST_NODE_IS_REPLACED;
    node->replaced_idx = CN_AST_NIL_IDX;
    node->parent_idx = parent_idx;
    node->next_idx = *list_node;
    *list_node = new;
}

CNDEF Cn_Ast_Linked_List cn__build_linked_list(Cn_Ast_Idx members[], int64_t length, const char *file, int64_t line) {
    Cn_Ast_Linked_List list = {0};
        
    for (int i = 0; i < length; i++) {
        if (cn_ast_node_get(members[i])->next_idx != CN_AST_NIL_IDX) {
            cn_log(CN_WARNING, "Linked list node with idx: %ld, already is in other linked, it's reinsertion corrupts that list, function call at %s:%ld.", members[i], file, line);
        }
        cn_ast_linked_list_add(&list, members[i]);
    }

    return list;
}

const Cn_Location cn_build_loc = { CN_STR_BUFFER("<built>"), 0, 0 };

CNDEF Cn_Ast_Idx cn__build_wrap_if_primary(Cn_Ast_Idx idx) {
    if (cn_ast_is_primary(idx)) {
        return cn_ast_node_list_append((Cn_Ast_Node) { 
                    .kind = CN_AST_NODE_PRIMARY_EXPRESSION, 
                    .loc = cn_ast_node_get(idx)->loc, 
                    .src = cn_ast_node_get(idx)->src, 
                    .primary_expression.literal_idx = (idx), 
                });
    }

    return idx;
}

CNDEF Cn_String cn__build_make_location(const char *func, const char *file, int64_t line) {
    Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    Cn_String loc = cn_sb_to_str(&sb);
    cn_sb_append_format(&sb, "<from %s() at %s:%ld>", func, file, line);
    loc =  cn__ast_permanent_save_string(loc);
    cn_sb_free(&sb);
    return loc;
}

CNDEF Cn_Ast_Idx cn__build_identifier(Cn_String name, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_NODE_IDENTIFIER,
        .flags = CN_AST_NODE_SYNTHETIC,
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
        .kind = CN_AST_NODE_INTEGER,
        .flags = CN_AST_NODE_SYNTHETIC,
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
        .kind = CN_AST_NODE_FLOAT,
        .flags = CN_AST_NODE_SYNTHETIC,
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
        .kind = CN_AST_NODE_STRING,
        .flags = CN_AST_NODE_SYNTHETIC,
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
        .kind = CN_AST_NODE_BINARY_EXPRESSION,
        .flags = CN_AST_NODE_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.binary_expression.operator_kind = op;
    node.binary_expression.left_expression_idx = cn__build_wrap_if_primary(left);
    node.binary_expression.right_expression_idx = cn__build_wrap_if_primary(right);

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_unary(Cn_Unary_Operator_Kind op, Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_NODE_UNARY_EXPRESSION,
        .flags = CN_AST_NODE_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.unary_expression.operator_kind = op;
    node.unary_expression.expression_idx = cn__build_wrap_if_primary(expression);

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_func_call(Cn_Ast_Idx callee, Cn_Ast_Linked_List arg_list, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_NODE_FUNCTION_EXPRESSION,
        .flags = CN_AST_NODE_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.function_expression.expression_idx = cn__build_wrap_if_primary(callee);

    Cn_Ast_Idx *idx = &arg_list.first_idx;
    Cn_Ast_Idx *next_idx;
    while(true) {
        if (*idx == CN_AST_NIL_IDX) break;
        next_idx = &cn_ast_node_get(*idx)->next_idx;
        
        *idx = cn__build_wrap_if_primary(*idx);
        cn_ast_node_get(*idx)->next_idx = *next_idx;
        *next_idx = CN_AST_NIL_IDX;
        
        idx =  &cn_ast_node_get(*idx)->next_idx;
    }

    node.function_expression.argument_list = arg_list;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_expr_statement(Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_NODE_EXPRESSION_STATEMENT,
        .flags = CN_AST_NODE_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.expression_statement.expression_idx = cn__build_wrap_if_primary(expression);

    return cn_ast_node_list_append(node);
}


#endif // CN_IMPLEMENTATION



