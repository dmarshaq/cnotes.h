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
      - CN_AST_NODE_LIST_INITIAL_CAP        Redefine initial capacity of array list that holds ast nodes.

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

/**
 * ============================================
 * SECTION: Logging
 * ============================================
 */
typedef enum {
    CN_INFO,
    CN_WARNING,
    CN_ERROR,
    CN_NO_LOGS,
} Cn_Log_Level;

/**
 * Any messages with the level below cn_log_min_level are going to be suppressed. 
 */
extern Cn_Log_Level cn_log_min_level;

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

/**
 * ============================================
 * SECTION: Hashing
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: String
 * ============================================

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
 * RETURNS: True if str string ends with end string.
 */
CNDEF bool cn_str_ends_with(Cn_String str, Cn_String end);

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

/**
 * ============================================
 * SECTION: String Builder
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: Allocator 
 * ============================================
 *
 * This library uses a variety of different allocators: arenas, pools, block allocators, etc...
 * But also this library doesn't want to introduce extra complexity regarding usage of such allocators,
 * so the in order to provide the middle-ground between simplicty and functionality, all
 * allocators must implement same foundational interface and can be replaced to one another at 
 * any point in time. This allows use to customize and control memory while library has
 * stable and understood internals on how everything works.
 *
 * Each allocator must implement it's own interface for functions:
 *  
 *  - CNDEF void *cn_alloc(Cn_Allocator *allocator, size_t size);
 *  - CNDEF void *cn_realloc(Cn_Allocator *allocator, void *mem, size_t new_size);
 *  - CNDEF void cn_free(Cn_Allocator *allocator, void *mem);
 *  - CNDEF void cn_free_all(Cn_Allocator *allocator);
 *
 * NOTE: If allocator cannot perform certain function like arena cannot free allocated memory by pointer, 
 * it is allowed to stub out function and print warning on attempt of using it.
 *
 * Then switching global allocator becomes as simple as setting global allocator global variable.
 * And separate allocators for different parts of the library can be replaced too.
 */

#include <stdlib.h>

typedef union cn_allocator Cn_Allocator;

typedef CNDEF void *(Cn_Alloc)(Cn_Allocator *allocator, size_t size);
typedef CNDEF void *(Cn_Realloc)(Cn_Allocator *allocator, void *mem, size_t new_size);
typedef CNDEF void (Cn_Free)(Cn_Allocator *allocator, void *mem);
typedef CNDEF void (Cn_Free_All)(Cn_Allocator *allocator);

#define CN_ALLOCATOR_BASE_MEMBERS   \
    Cn_Alloc        *alloc;         \
    Cn_Realloc      *realloc;       \
    Cn_Free         *free;          \
    Cn_Free_All     *free_all;      \

typedef struct {
    CN_ALLOCATOR_BASE_MEMBERS
} Cn_Allocator_Base;

#define CN_ALLOCATOR_BASE                       \
    union {                                     \
        Cn_Allocator_Base base;                 \
        struct { CN_ALLOCATOR_BASE_MEMBERS };   \
    }


typedef struct { CN_ALLOCATOR_BASE;
    size_t capacity;
    void *allocation;
    void *ptr;
} Cn_Arena;

/**
 * Constructs arena and allocates memory of capacity size.
 */
CNDEF Cn_Arena cn_arena_make(size_t capacity);

/**
 * Allocates next chunk of memory from arena.
 */
CNDEF void *cn_arena_alloc(Cn_Arena *arena, size_t size);

/**
 * Reallocs chunk of memory from arena.
 * Is using cn_arena_alloc underneath.
 */
CNDEF void *cn_arena_realloc(Cn_Arena *arena, void *mem, size_t new_size);

/**
 * Stubbed out in actual implementation, 
 * since arena cannot free previously allocated memory
 */
CNDEF void cn_arena_free(Cn_Arena *arena, void *mem);

/**
 * Cleans up all memory used by the arena.
 */
CNDEF void cn_arena_free_all(Cn_Arena *arena);

/**
 * Completely destroys arena and frees all memory occupied by it.
 */
CNDEF void cn_arena_destroy(Cn_Arena *arena);


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
typedef struct { CN_ALLOCATOR_BASE;
    size_t block_capacity;
    void  *block;
} Cn_Chained_Arena;

#define CN_CHAINED_ARENA_BLOCK_HEADER(block) ((Cn_Chained_Arena_Block_Header *)((uint8_t *)(block) - sizeof(Cn_Chained_Arena_Block_Header)))

typedef struct {
    void  *prev;
    void  *next;
    size_t allocated;
} Cn_Chained_Arena_Block_Header;

/**
 * Initializes arena by allocating first block and setting all pointers.
 */
CNDEF Cn_Chained_Arena cn_chained_arena_make(size_t block_capacity);

/**
 * Allocates specified memory size from the arena.
 */
CNDEF void *cn_chained_arena_alloc(Cn_Chained_Arena *arena, size_t size);

/**
 * Reallocs chunk of memory from arena.
 * Is using cn_chained_arena_alloc underneath.
 */
CNDEF void *cn_chained_arena_realloc(Cn_Chained_Arena *arena, void *mem, size_t new_size);

/**
 * Stubbed out in actual implementation, 
 * since arena cannot free previously allocated memory
 */
CNDEF void cn_chained_arena_free(Cn_Chained_Arena *arena, void *mem);

/**
 * Cleans up all memory used by the arena.
 */
CNDEF void cn_chained_arena_free_all(Cn_Chained_Arena *arena);

/**
 * Dellocates specified memory size from the front of the arena.
 * If specified size completely deallocates whole block, it is freed.
 */
CNDEF void cn_chained_arena_dealloc(Cn_Chained_Arena *arena, size_t size);

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
CNDEF int64_t cn_chained_arena_allocation_info(Cn_Chained_Arena *arena, void *allocation_ptr, size_t *offset);

/**
 * Given chained arena computes total allocated size by the arena.
 *
 * RETURNS: Total allocated size.
 */
CNDEF size_t cn_chained_arena_allocated(Cn_Chained_Arena *arena);

#define cn_ast_chained_arena_foreach_in_block(type, it, block) for (type *it = (type *)(block); (uint8_t *)it <= (uint8_t *)(block) + CN_CHAINED_ARENA_BLOCK_HEADER(block)->allocated - sizeof(type); it++)

/**
 * Completely destroys arena and frees all memory occupied by it.
 */
CNDEF void cn_chained_arena_destroy(Cn_Chained_Arena *arena);


/**
 * Pool is an allocator that hands out fixed size slots cut from pre divided blocks.
 * It is constructed from a list of capacities, where every capacity defines a size class.
 * Each size class owns a chain of blocks, and each block is a bit field of occupied
 * slots followed by the memory those slots are cut from.
 *
 *  cn_pool_make(8, 64)
 *
 *              -------------------------     -------------------------
 *  blocks[0] = | 8  | flags | mem     | --> | 8  | flags | mem     |
 *              -------------------------     -------------------------
 *              -------------------------
 *  blocks[1] = | 64 | flags | mem     | --> NULL
 *              -------------------------
 *
 *  blocks_length = 2
 *
 * Blocks are always of CN_POOL_BLOCK_MAX_CAPACITY bytes, so a size class of capacity 8
 * cuts its block into 512 slots, while a size class of capacity 4096 cuts it into 1.
 * When every block of a size class is full, one more block is chained to its end.
 *
 * Pool doesn't store any header next to the returned memory, so the block a pointer
 * belongs to is found by searching the blocks for the one whose memory contains it.
 *
 * Pool struct itself only holds the array of size class chains and its length, and that
 * array is the array of capacities supplied to cn_pool_make, overwritten in place with
 * block pointers. Which is why:
 *
 *  - Capacities must be listed in strictly ascending order, so that the first size class
 *    big enough for an allocation is also the tightest fit for it.
 *  - Pool must not outlive the supplied array, cn_pool_make used with a compound literal
 *    inside a function is only valid until the end of the enclosing block.
 */
#define CN_POOL_BLOCK_MAX_CAPACITY 4096
#define CN_POOL_BLOCK_MIN_CAPACITY 8

typedef struct cn_pool_block Cn_Pool_Block;

struct cn_pool_block {
    Cn_Pool_Block *next;
    size_t capacity; // Capacity per allocation, size_t keeps mem aligned as malloc returned it.
    uint8_t flags[CN_POOL_BLOCK_MAX_CAPACITY / CN_POOL_BLOCK_MIN_CAPACITY / 8]; // Bit flags, if bit set to, specific allocation is occupied.
    uint8_t mem[CN_POOL_BLOCK_MAX_CAPACITY]; // Total allocation.
};

typedef struct { CN_ALLOCATOR_BASE;
    Cn_Pool_Block **blocks;
    size_t blocks_length;
} Cn_Pool;

/**
 * Number of slots a block of given capacity is divided into.
 */
#define CN_POOL_BLOCK_SLOTS(block) (CN_POOL_BLOCK_MAX_CAPACITY / (block)->capacity)

/**
 * Constructs pool and allocates one block per supplied capacity.
 * Capacities must be strictly ascending, multiples of CN_POOL_BLOCK_MIN_CAPACITY,
 * and not bigger than CN_POOL_BLOCK_MAX_CAPACITY.
 * Assumes sizeof(size_t) == sizeof(void *) because it reuses supplied array of capacities.
 */
#define cn_pool_make(...) cn__pool_make((size_t []) { __VA_ARGS__ }, (sizeof((size_t []) { __VA_ARGS__ }) / sizeof(size_t)))

CNDEF Cn_Pool cn__pool_make(size_t capacities[], size_t capacities_length);

/**
 * Allocates next free slot of memory from the pool.
 * Slot is taken from the smallest size class that fits the requested size.
 */
CNDEF void *cn_pool_alloc(Cn_Pool *pool, size_t size);

/**
 * Reallocs slot of memory from the pool.
 * If new size still fits the slot, same memory is returned.
 */
CNDEF void *cn_pool_realloc(Cn_Pool *pool, void *mem, size_t new_size);

/**
 * Frees slot of memory from the pool.
 */
CNDEF void cn_pool_free(Cn_Pool *pool, void *mem);

/**
 * Marks every slot of every block as free, keeping the blocks themselves allocated.
 */
CNDEF void cn_pool_free_all(Cn_Pool *pool);

/**
 * Given pool and pointer allocated by it, it will find the block the pointer was
 * allocated from, and index of the slot it occupies in that block.
 *
 * RETURNS: Block containing the pointer, NULL if pointer supplied doesn't point to the
 * beginning of an occupied slot, which is also the case for already freed memory.
 *
 * OUTPUTS: Index of the occupied slot in the found block.
 *
 * NOTE: This function will properly return if supplied pointer points to the memory held
 * by the pool. Even if address was not supplied correctly, but it happens to point to the
 * beginning of an occupied slot, this function will properly return.
 */
CNDEF Cn_Pool_Block *cn_pool_allocation_info(Cn_Pool *pool, void *allocation_ptr, size_t *slot);

/**
 * Completely destroys pool and frees all memory occupied by it.
 * Supplied array of capacities is not freed, since pool doesn't own it.
 */
CNDEF void cn_pool_destroy(Cn_Pool *pool);


union cn_allocator {
    CN_ALLOCATOR_BASE;
    Cn_Arena arena;
    Cn_Chained_Arena chained_arena;
    Cn_Pool pool;
};

extern Cn_Allocator cn_std_allocator;

extern Cn_Allocator *cn_default_allocator;

/**
 * Wrapper that calls alloc on allocator passing allocater self 
 * as a paremeter.
 */
CNDEF void *cn_alloc(Cn_Allocator *allocator, size_t size);

/**
 * Wrapper that calls realloc on allocator passing allocater self 
 * as a paremeter.
 */
CNDEF void *cn_realloc(Cn_Allocator *allocator, void *mem, size_t size);

/**
 * Wrapper that calls free on allocator passing allocater self 
 * as a paremeter.
 */
CNDEF void cn_free(Cn_Allocator *allocator, void *mem);

/**
 * Wrapper that calls free_all on allocator passing allocater self 
 * as a paremeter.
 */
CNDEF void cn_free_all(Cn_Allocator *allocator);

#define CN_REALLOC(mem, size) cn_realloc(cn_default_allocator, (mem), (size))
#define CN_FREE(mem) cn_free(cn_default_allocator, (mem))

/**
 * ============================================
 * SECTION: Array List
 * ============================================
 */
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
        Cn_Array_List_Header *cn__h_ = (Cn_Array_List_Header *)((uint8_t *)(*ptr_list) - sizeof(Cn_Array_List_Header));\
        (*ptr_list)[cn__h_->length++] = item;\
    } while(0)

#define cn_array_list_append_multiple(ptr_list, item_arr, count)\
    cn__array_list_append_multiple((void **)ptr_list, (void *)item_arr, count)

#define cn_array_list_add(ptr_list, index, item)\
    do {\
        cn__array_list_shift_to_fit((void **)(ptr_list), index, 1);\
        (*ptr_list)[index] = item;\
    } while(0)

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

CNDEF void cn__array_list_shift_to_fit(void **list, int64_t index, int64_t extra_length);

CNDEF int64_t cn__array_list_append_multiple(void **list, void *items, int64_t count);

CNDEF void cn__array_list_pop(void *list, int64_t count);

CNDEF void cn__array_list_clear(void *list);

CNDEF void cn__array_list_unordered_remove(void *list, int64_t index);

CNDEF void cn__array_list_free(void **list);

/**
 * ============================================
 * SECTION: Hash Table 
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: Hash Set
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: Source & Location
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: Lexer
 * ============================================
 */
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
    CN_TOKEN_CASE,
    CN_TOKEN_DEFAULT,
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

/**
 * ============================================
 * SECTION: Type
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: Any
 * ============================================
 */
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

/**
 * ============================================
 * SECTION: Abstract Syntax Tree
 * ============================================
 */
typedef uint32_t Cn_Ast_Idx;

#define CN_AST_NIL_IDX      0
#define CN_AST_ERROR_IDX    1

typedef struct {
    Cn_Ast_Idx *idxs;
    int64_t     length;
} Cn_Ast_List;

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
    X(BLOCK_ITEM,                       Block_Item,                         block_item)                     \
    X(IF,                               If,                                 if_statement)                   \
    X(SWITCH,                           Switch,                             switch_statement)               \
    X(WHILE,                            While,                              while_statement)                \
    X(DO_WHILE,                         Do_While,                           do_while)                       \
    X(FOR,                              For,                                for_statement)                  \
    X(LABEL,                            Label,                              label)                          \
    X(GOTO,                             Goto,                               goto_statement)                 \
    X(RETURN,                           Return,                             return_statement)               \
    X(BREAK,                            Break,                              break_statement)                \
    X(CONTINUE,                         Continue,                           continue_statement)             \
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
    /* --- declarator and declaration machinary --------------------------- */                              \
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

typedef enum : uint8_t {
    CN_AST_UNKNOWN = 0,
    CN_AST_ERROR   = 1,
#define X(K, T, m) CN_AST_##K,
    CN_AST_GEN_LIST(X)
#undef X
} Cn_Ast_Kind;

typedef enum {
    CN_AST_SYNTHETIC                    = 0x4,

    CN_AST_EXTERNAL_DECLARATION_HAS_EXTENSION     = 0x100,

    CN_AST_DECLARATOR_IS_ABSTRACT                 = 0x100,

    CN_AST_ACCESS_IS_POINTER                      = 0x100,

    CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC = 0x100,

    CN_AST_ENUM_HAS_DEFINITION                    = 0x100,

    CN_AST_UNION_HAS_DEFINITION                   = 0x100,

    CN_AST_STRUCT_HAS_DEFINITION                  = 0x100,

    CN_AST_DECLARATION_IS_EMPTY                   = 0x100,

    CN_AST_LABEL_IS_CASE                          = 0x100,
    CN_AST_LABEL_IS_DEFAULT                       = 0x200,
} Cn_Ast_Flags;

#define CN_AST_BASE_MEMBERS         \
    Cn_Ast_Kind     kind;           \
    Cn_Ast_Flags    flags;          \
    Cn_Location     loc;            \
    Cn_Source       src;            \
    Cn_Ast_Idx      parent_idx;

typedef struct {
    CN_AST_BASE_MEMBERS
} Cn_Ast_Base;

#define CN_AST_BASE                         \
    union {                                 \
        Cn_Ast_Base base;                   \
        struct { CN_AST_BASE_MEMBERS };     \
    }

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
} Cn_Ast_Declaration;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx  declaration_specifiers_idx;
    Cn_Ast_Idx  declarator_idx;
    Cn_Ast_Idx  block_idx;
} Cn_Ast_Function;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_List block_items; // List of ast nodes of type Cn_Ast_Block_Item.
} Cn_Ast_Block;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx declaration_or_statement_idx;
} Cn_Ast_Block_Item;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx then_idx;
    Cn_Ast_Idx else_idx; // Can be NIL.
} Cn_Ast_If;
 
typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx condition_idx;
    Cn_Ast_Idx body_idx;
} Cn_Ast_Switch;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx condition_idx; // Expression.
    Cn_Ast_Idx body_idx; // Statement.
} Cn_Ast_While;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx condition_idx; // Expression.
    Cn_Ast_Idx body_idx; // Statement.
} Cn_Ast_Do_While;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx initialization_idx; // Declaration or expression.
    Cn_Ast_Idx condition_idx; // Expression.
    Cn_Ast_Idx update_idx; // Expression.
    Cn_Ast_Idx body_idx; // Statement.
} Cn_Ast_For;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx identifier_idx; 
    Cn_Ast_Idx expression_idx; // Only in 'case' label.
    Cn_Ast_Idx statement_idx;
} Cn_Ast_Label;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx identifier_idx; // Name of the label.
} Cn_Ast_Goto;
 
typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx expression_idx; // Can be NIL.
} Cn_Ast_Return;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
} Cn_Ast_Break;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
} Cn_Ast_Continue;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
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
    Cn_Ast_List gnu_attribute_specifiers;
    Cn_Ast_Idx gnu_asm_label_idx;
    Cn_Ast_Idx initializer_idx;
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
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_List gnu_attribute_specifiers;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List member_declarations;
} Cn_Ast_Struct_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_List gnu_attribute_specifiers;
    Cn_Ast_Idx  identifier_idx;
    Cn_Ast_List member_declarations;
} Cn_Ast_Union_Specifier;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_Idx  specifier_qualifier_idx;
    Cn_Ast_List member_declarators;
} Cn_Ast_Member_Declaration;

typedef struct { CN_AST_BASE;
    Cn_Ast_Idx declarator_idx;
    Cn_Ast_Idx bitfield_idx;
    Cn_Ast_List gnu_attribute_specifiers;
} Cn_Ast_Member_Declarator;

typedef struct { CN_AST_BASE;
    Cn_Ast_List attribute_specifiers;
    Cn_Ast_List gnu_attribute_specifiers;
    Cn_Ast_Idx  identifier_idx;
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
    CN_BINDING_LABEL,
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

#define CN_AST_LABEL_BINDING_LIST_INITIAL_CAP       32
#define CN_AST_LABEL_BINDING_TABLE_INITIAL_CAP      16

#define CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP       4096
#define CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP    4096

/**
 * This struct simply contains information that each scope will have.
 */
typedef struct {
    int64_t saved_strings_length;
    int64_t binding_idx;
    int64_t label_binding_idx;
    int64_t binding_defined_idx;

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
     * Generated output by the emitter.
     */
    Cn_Chained_Arena output_arena;
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
     * Stores all non-label bindings in growing array list.
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
     * Stores all label bindings in growing array list.
     *
     * IMPORTANT: Access elements by indicies, 
     * so there are no unsafe situations occuring.
     * First element is considered NIL element, 
     * it is reserved to identify illegal references.
     */
    Cn_Ast_Binding *label_binding_list;
    /**
     * Label table stores only label bindings.
     */
    Cn_Ast_Binding_Idx *label_binding_table;
    /**
     * This array list does one job to prevent a really annoying binding related issue,
     * that comes up during reparsing (specifacly checkpoint loading) after user executed hook.
     *
     * To understand this, first understand that in C most of the bindings are allowed to be redeclared and 
     * some even redefined. For example tag types (struct, enum and union) are allowed to be redeclared
     * as many times as you want, and eventually defined (therefor completing previously declared tag type).
     * And C variables are allowed to be redefined under certain scope conditions too.
     *
     * That said, cannonicaly this works under the mechanism which simply ignores duplicate redeclarations and 
     * merges them into previous declarations, and if declaration completes a type (for example in tag type case),
     * the type that was PREVIOUSLY created gets filled with values and completed (defined).
     *
     * But that brings an issue if used with checkpoint/rollback system that this tool provides.
     * Since if rollback occurs, by the rules: all state is reverted to the checkpoint in order to reparse ast tree.
     * This includes deleting all types, bindings that were created AFTER checkpoint was set.
     * And thats the problem, since a binding with definition can occur AFTER checkpoint is set and it can complete (define)
     * the type that was PREVIOUSLY created by older declaration. So when rollback occurs the ast tree that made definition is erased,
     * but the definition is still saved in the type, since it was first declared before the checkpoint, and therefor is not reverted 
     * to previous undefined state.
     *
     * Solution is then NOT to destroy exisitng binding structure (because it would likely only overcomplicate and possibly break some C grammar rules), 
     * but rather to track binding idx in growing list. Each time the binding is defined, it's idx is added to this list. 
     * Then its simple, because checkpoint will place marker and every idx added after that marker will be taken out and undefined. 
     * This list is also scoped, so bindings that were removed on its own get their idx's removed from here as well.
     */
    Cn_Ast_Binding_Idx *binding_defined_idx_list;
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
     * Stores idx of the current function scope, 
     * if not inside function scope is NIL.
     */
    int64_t function_scope_idx;
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
    uint64_t                saved_output_length;
    int64_t                 saved_node_length;
    uint64_t                saved_type_length;
    uint64_t                saved_type_children_length;
    int64_t                 saved_binding_length;
    int64_t                 saved_label_binding_length;
    int64_t                 saved_binding_defined_idx_length;
    uint64_t                saved_scoped_strings_length;
    uint64_t                saved_permanent_strings_length;
    int64_t                 saved_scope_length;
    int64_t                 saved_function_scope_idx;
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
 * Prepends an idx to the idx_stack at mark.
 */
#define cn_ast_idx_stack_prepend(idx, mark) cn_array_list_add(&cn__ast_data->idx_stack, (mark), (idx))

/**
 * Discards any added idx's by returning to the mark.
 */
#define cn_ast_idx_stack_discard(mark) cn_array_list_pop_multiple(&cn__ast_data->idx_stack, cn_array_list_length(&cn__ast_data->idx_stack) - (mark))

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

/**
 * ============================================
 * SECTION: Emit
 * ============================================
 */

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

typedef enum {
    CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK     = 0x1,
    CN_EMITTER_END_DECLARATION_WITH_SEMICOLON   = 0x2,
} Cn_Emitter_Flags;

typedef struct {
    Cn_Emitter_Flags flags;
    /**
     * Total written length from the whole emit.
     */
    int64_t written_length;
    /**
     * Indentation level spacing to be added.
     */
    int indent;
    /**
     * User data or context that user wants to preserve, share with every 
     * write call made by emitter.
     */
    void *ctx;
    Cn_Emit_Write *write;
} Cn_Emitter;

/**
 * Emits C source code from AST node to output.
 * Reconstructs valid C source from AST representation.
 */
CNDEF void cn_emit(Cn_Emitter *e, Cn_Ast_Idx node_idx);

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
 * Declares new label binding in the current function scope. 
 *
 * RETURNS: NIL if error occured, binding idx on success.
 */
CNDEF Cn_Ast_Binding_Idx cn_ast_label_binding_declare(Cn_String name, Cn_Ast_Idx source_idx);

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
 *          : attribute_specifiers declaration_specifiers init_declarators? ';'
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
 *          : block
 *          | selection_statement
 *          | iteration_statement
 *          | jump_statement
 *          | labeled_statement
 *          | expression_statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as statement.
 *
 * IMPORTANT: CN_AST_NODE_STATEMENT doesn't exist by itself instead there are nodes variation, 
 * when calling this function to parse statement it returns on of the following types of nodes.
 *
 *  statement
 *          : block
 *          | selection_statement
 *          | iteration_statement
 *          | jump_statement
 *          | labeled_statement
 *          | expression_statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_continue_statement(Cn_Lexer *lexer, Cn_Ast_List attribute_specifiers);

/**
 * Parses code starting of with lexer current token as block.
 *
 * IMPORTANT: Block introduces new scope,
 * but this can be controlled by setting use current scope to true.
 * By doing so, block won't introduce new scope
 * and everything will be defined and declared in current scope instead of a new one.
 *
 *  block
 *          : '{' block_item* '}'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_block(Cn_Lexer *lexer, bool use_current_scope);

/**
 * Parses code starting of with lexer current token as block item.
 *
 *  block_item
 *          : declaration ';'
 *          | statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_block_item(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as if statement.
 *
 *  if_statement
 *          : 'if' '(' expression ')' statement ('else' statement)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_if_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as switch statement.
 *
 *  switch_statement
 *          : 'switch' '(' expression ')' statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_switch_statement(Cn_Lexer *lexer);

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
 *          : 'for' '(' (declaration | expression)? ';' expression? ';' expression? ')' statement
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_for_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as goto statement.
 *
 *  goto_statement
 *          : 'goto' identifier ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_goto_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as return statement.
 *
 *  return_statement
 *          : 'return' expression? ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_return_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as break statement.
 *
 *  break_statement
 *          : 'break' ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_break_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as continue statement.
 *
 *  continue_statement
 *          : 'continue' ';'
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_continue_statement(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as label.
 *
 * NOTE: Instead of making separate nodes, the label can be all three types.
 * the usual identifier label, case label, or default. And to know which one is it
 * flags will be used, also depending on the type of label some fields will be nil.
 *
 *  label
 *          : (identifier | ('case' expression) | 'default') ':' statement
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
 *          : '(' type_name ')' designations
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
 *          : declarator gnu_attribute_specifiers gnu_asm_label? ('=' initializer)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_finish_init_declarator(Cn_Lexer *lexer, Cn_Ast_Idx declarator_idx);

/**
 * Parses code starting of with lexer current token as init declarator.
 *
 *  init_declarator
 *          : declarator gnu_attribute_specifiers gnu_asm_label? ('=' initializer)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_init_declarator(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as initializer.
 *
 *  initializer
 *          : expression
 *          | designations
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_initializer(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as designations.
 *
 *  designations
 *          : '{' ( designation (',' designation)* )? ','? '}'
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
 *          : 'struct' attribute_specifiers gnu_attribute_specifiers
 *          ( identifier? '{' member_declaration* '}' ) 
 *          | identifier
 *          ;
 *
 *  union_specifier
 *          : 'union' attribute_specifiers gnu_attribute_specifiers
 *          ( identifier? '{' member_declaration* '}' ) 
 *          | identifier
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_struct_or_union_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as member declaration.
 *
 *  member_declaration
 *          : attribute_specifiers specifier_qualifier member_declarators ';'
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
 *          ( identifier? (':' specifier_qualifier)? '{' ( enumerator (',' enumerator)* ','? )? '}' ) 
 *          | identifier  (':' specifier_qualifier)?
 *          ;
 */
CNDEF Cn_Ast_Idx cn_ast_parse_enum_specifier(Cn_Lexer *lexer);

/**
 * Parses code starting of with lexer current token as enumerator.
 *
 *  enumerator
 *          : identifier attribute_specifiers gnu_attribute_specifiers ('=' expression)?
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
 * ============================================
 * SECTION: Diagnostics
 * ============================================
 */

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
    CN_DC_ILLEGAL_ATTRIBUTE,
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

/**
 * ============================================
 * SECTION: Pre-Processing
 * ============================================
 */

extern const Cn_Lexer_Blacklist cn_default_blacklist;

typedef enum {
    CN_MESSAGE_PARSED_FUNCTION
} Cn_Message_Kind;

typedef struct {
    Cn_Ast_Idx node_idx;
} Cn_Message_Parsed_Function;

typedef union {
    Cn_Message_Parsed_Function parsed_function;
} Cn_Message;

typedef enum {
    CN_RESULT_NONE,
    CN_RESULT_MODIFIED,
    CN_RESULT_MODIFIED_NO_REPEAT,
} Cn_Result;

typedef Cn_Result (Cn_Message_Handler)(Cn_Message_Kind kind, void *message);

extern Cn_Message_Handler *cn_message_handler;

/**
 * Simple wrapper that checks if message handler not NULL, 
 * if so it sends specified message to the user.
 */
CNDEF Cn_Result cn__send_message(Cn_Message_Kind kind, Cn_Message message);

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
CNDEF void cn_replace(Cn_Ast_Idx *original, Cn_Ast_Idx new);

/**
 * Replaces old ast list with a new, this function is 
 * a recommended way of modifying ast tree.
 * Because it automatically tracks node changes 
 * and guarantees proper diagnostics.
 */
CNDEF void cn_replace_list(Cn_Ast_List *original, Cn_Ast_List new);

/**
 * Orderly removes entry from the list completely, 
 * forever decreasing list's length.
 */
CNDEF Cn_Ast_Idx cn_remove_from_list(Cn_Ast_List *list, int64_t index);

/**
 * Deep copies list, allocating memory for the every node branch.
 * Use with caution, copying giant branches like translation unit is not recommended.
 * Returned idx's won't have it's parent set, and all ast nodes will have SYNTHETIC flag.
 * And all replaced information is erased. It is recursive.
 */
CNDEF Cn_Ast_List cn_copy_list(Cn_Ast_List list);

/**
 * Deep copies ast, allocating memory for the branch.
 * Use with caution, copying giant branches like translation unit is not recommended.
 * Returned idx's won't have it's parent set, and all ast nodes will have SYNTHETIC flag.
 * And all replaced information is erased. It is recursive.
 */
CNDEF Cn_Ast_Idx cn_copy(Cn_Ast_Idx idx);

/**
 * RETURNS: NIL if couldn't find attribute specific attribute on the node, or valid attribute idx
 * if found it.
 *
 * IMPORTNAT: If the are multiple attributes with the same name, finds the first one.
 */
CNDEF Cn_Ast_Idx cn_get_attribute(Cn_Ast_Idx idx, Cn_String attribute_name);

/**
 * Removes attribute from the attribute list.
 */
CNDEF void cn_remove_attribute(Cn_Ast_Idx attribute_idx);

/**
 * Optional values that can be set for most of the cn_build_* function.
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

/**
 * Builds code ast node, that containes formated text.
 * Once reparsed the node is expanded and parsed 
 * into actual ast nodes.
 *
 * RETURNS: Built code node.
 */
#define cn_build_format(...) cn__build_format(__FILE__, __LINE__, __VA_ARGS__)

CNDEF Cn_Ast_Idx cn__build_format(const char *file, int64_t line, const char *format, ...);

/**
 * Builds list out of supplied nodes.
 * Accepts variadic array of nodes in order.
 *
 * RETURNS: List composed of supplied nodes.
 */
#define cn_build_list(...) cn__build_list((Cn_Ast_Idx[]) { __VA_ARGS__ }, sizeof((Cn_Ast_Idx[]) { __VA_ARGS__ }) / sizeof(Cn_Ast_Idx))

CNDEF Cn_Ast_List cn__build_list(Cn_Ast_Idx idxs[], int64_t length);

/**
 * Builds identifier with supplied name.
 * 
 * RETURNS: Built identifier.
 */
#define cn_build_identifier(name, ...) cn__build_identifier(name, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_identifier(Cn_String name, Cn_Build_Opt opt);

/**
 * Builds integer with supplied value.
 *
 * RETURNS: Built integer.
 */
#define cn_build_integer(value, ...) cn__build_integer(value, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_integer(Cn_String value, Cn_Build_Opt opt);

/**
 * Builds float with supplied value.
 * 
 * RETURNS: Built float.
 */
#define cn_build_float(value, ...) cn__build_float(value, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_float(Cn_String value, Cn_Build_Opt opt);

/**
 * Builds string with supplied str.
 * 
 * RETURNS: Built string.
 */
#define cn_build_string(str, ...) cn__build_string(str, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

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
 * Builds assign expression, left and right params must be 
 * valid built node idx's.
 * 
 * RETURNS: Built assignment expression.
 */
#define cn_build_assign(op, left, right, ...) cn__build_assign(op, left, right, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_assign(Cn_Assignment_Operator_Kind op, Cn_Ast_Idx left, Cn_Ast_Idx right, Cn_Build_Opt opt);

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

/**
 * Builds goto statement, identifier must be 
 * valid built node idx.
 * 
 * RETURNS: Built goto statement.
 */
#define cn_build_goto_statement(identifier, ...) cn__build_goto_statement(identifier, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_goto_statement(Cn_Ast_Idx identifier, Cn_Build_Opt opt);

/**
 * Builds return statement, expression must be 
 * valid built node idx.
 * 
 * RETURNS: Built return statement.
 */
#define cn_build_return_statement(expression, ...) cn__build_return_statement(expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_return_statement(Cn_Ast_Idx expression, Cn_Build_Opt opt);

/**
 * Builds label, identifier and statement must be 
 * valid built node idxs.
 * 
 * RETURNS: Built label.
 */
#define cn_build_label(identifier, statement, ...) cn__build_label(identifier, statement, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_label(Cn_Ast_Idx identifier, Cn_Ast_Idx statement, Cn_Build_Opt opt);

/**
 * Builds block item, child must be 
 * valid built node idx.
 * 
 * RETURNS: Built block item.
 */
#define cn_build_block_item(declaration_or_statement, ...) cn__build_block_item(declaration_or_statement, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_block_item(Cn_Ast_Idx declaration_or_statement, Cn_Build_Opt opt);

/**
 * Builds pointer with supplied qualifiers, pointer param must be
 * NIL or valid built pointer idx, it chains pointer that stands closer
 * to the declared name, as in 'char * const * name' where outer built
 * pointer holds 'const' qualifier and chains the inner one.
 *
 * RETURNS: Built pointer.
 */
#define cn_build_pointer(qualifiers, pointer, ...) cn__build_pointer(qualifiers, pointer, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_pointer(Cn_Qualifier_Flags qualifiers, Cn_Ast_Idx pointer, Cn_Build_Opt opt);

/**
 * Builds grouped direct declarator '(' declarator ')', declarator must be
 * valid built node idx.
 *
 * NOTE: Identifier direct declarator variation has no dedicated build
 * function, plain cn_build_identifier() result is used in its place.
 *
 * RETURNS: Built grouped direct declarator.
 */
#define cn_build_direct_declarator_grouped(declarator, ...) cn__build_direct_declarator_grouped(declarator, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_direct_declarator_grouped(Cn_Ast_Idx declarator, Cn_Build_Opt opt);

/**
 * Builds array direct declarator 'direct_declarator '[' expression? ']''.
 * Direct declarator must be valid built node idx, or NIL if array declarator
 * is abstract. Expression can be NIL to build unsized '[]' array.
 *
 * RETURNS: Built array direct declarator.
 */
#define cn_build_direct_declarator_array(direct_declarator, expression, ...) cn__build_direct_declarator_array(direct_declarator, expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_direct_declarator_array(Cn_Ast_Idx direct_declarator, Cn_Ast_Idx expression, Cn_Build_Opt opt);

/**
 * Builds function direct declarator 'direct_declarator '(' parameter_declarations? ')''.
 * Direct declarator must be valid built node idx, or NIL if function declarator
 * is abstract. Parameter declarations should be constructed beforehand with
 * appropriate parameter declaration children, empty list builds '(void)'.
 * Variadic marks declarator as ending with '...'.
 *
 * RETURNS: Built function direct declarator.
 */
#define cn_build_direct_declarator_function(direct_declarator, parameter_declarations, variadic, ...) cn__build_direct_declarator_function(direct_declarator, parameter_declarations, variadic, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_direct_declarator_function(Cn_Ast_Idx direct_declarator, Cn_Ast_List parameter_declarations, bool variadic, Cn_Build_Opt opt);

/**
 * Builds declarator, pointer can be NIL if declarator is not wrapped with
 * pointer(s), direct declarator can be NIL only if declarator is abstract.
 *
 * NOTE: CN_AST_DECLARATOR_IS_ABSTRACT flag is deduced from supplied direct
 * declarator chain, declarator is abstract when that chain carries no identifier.
 *
 * RETURNS: Built declarator.
 */
#define cn_build_declarator(pointer, direct_declarator, ...) cn__build_declarator(pointer, direct_declarator, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_declarator(Cn_Ast_Idx pointer, Cn_Ast_Idx direct_declarator, Cn_Build_Opt opt);

/**
 * Builds init declarator, declarator must be valid non-abstract built node idx.
 * Initializer can be NIL if declared name is left uninitialized.
 *
 * RETURNS: Built init declarator.
 */
#define cn_build_init_declarator(declarator, initializer, ...) cn__build_init_declarator(declarator, initializer, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_init_declarator(Cn_Ast_Idx declarator, Cn_Ast_Idx initializer, Cn_Build_Opt opt);

/**
 * Builds declaration, declaration specifiers must be valid built node idx.
 * Init declarators should be constructed beforehand with appropriate init
 * declarator children, empty list builds empty declaration.
 *
 * RETURNS: Built declaration.
 */
#define cn_build_declaration(declaration_specifiers, init_declarators, ...) cn__build_declaration(declaration_specifiers, init_declarators, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Idx cn__build_declaration(Cn_Ast_Idx declaration_specifiers, Cn_Ast_List init_declarators, Cn_Build_Opt opt);


#endif // CN_H_
       

#ifdef CN_IMPLEMENTATION

/**
 * ============================================
 * IMPLEMENTATION SECTION: Logging
 * ============================================
 */
Cn_Log_Level cn_log_min_level = CN_INFO;

Cn_Log_Handler *cn_log_handler = &cn_default_log_handler;

CNDEF void cn_default_log_handler(Cn_Log_Level level, const char *format, va_list args) {
    
    if (level < cn_log_min_level)
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Hashing
 * ============================================
 */
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: String
 * ============================================
 */
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

CNDEF bool cn_str_ends_with(Cn_String str, Cn_String end) {
    if (str.length == 0 && end.length == 0) return true;

    int64_t index = cn_str_find_right(str, end);
    if (index == -1) return false;

    return str.length - end.length == index;
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: String Builder
 * ============================================
 */
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Allocator 
 * ============================================
 */
CNDEF Cn_Arena cn_arena_make(size_t capacity) {
    CN_ASSERT(capacity > 0);

    void *mem = realloc(NULL, capacity);

    if (mem == NULL) {
        cn_log(CN_ERROR, "Couldn't malloc %zu bytes of memory for the arena.", capacity);
        return (Cn_Arena) {0};
    }

    return (Cn_Arena) {
        .alloc      = (Cn_Alloc *)cn_arena_alloc,
        .realloc    = (Cn_Realloc *)cn_arena_realloc,
        .free       = (Cn_Free *)cn_arena_free,
        .free_all   = (Cn_Free_All *)cn_arena_free_all,
        .capacity   = capacity,
        .allocation = mem,
        .ptr        = mem,
    };
}

CNDEF void *cn_arena_alloc(Cn_Arena *arena, size_t size) {
    arena->ptr += size;

    if (arena->ptr > arena->allocation + arena->capacity) {
        cn_log(CN_ERROR, "Couldn't allocate %zu bytes of memory from the arena, this allocation exceeded arena's capacity.", size);
        return NULL;
    }

    return arena->ptr - size;
}

CNDEF void *cn_arena_realloc(Cn_Arena *arena, void *mem, size_t new_size) {
    CN_UNUSED(mem);
    return cn_arena_alloc(arena, new_size);
}

CNDEF void cn_arena_free(Cn_Arena *arena, void *mem) {
    CN_UNUSED(arena);
    CN_UNUSED(mem);
    cn_log(CN_WARNING, "Coudln't free memory in arena, arena doesn't implement free mechanism, use free_all instead, or destroy arena completely.");
}

CNDEF void cn_arena_free_all(Cn_Arena *arena) {
    arena->ptr = arena->allocation;
}

CNDEF void cn_arena_destroy(Cn_Arena *arena) {
    free(arena->allocation);
    *arena = (Cn_Arena) {0};
}

CNDEF Cn_Chained_Arena cn_chained_arena_make(size_t block_capacity) {
    CN_ASSERT(block_capacity > 0);

    Cn_Chained_Arena_Block_Header *header = realloc(NULL, sizeof(Cn_Chained_Arena_Block_Header) + block_capacity);

    header->prev = NULL;
    header->next = NULL;
    header->allocated = 0;
    
    return (Cn_Chained_Arena) {
        .alloc      = (Cn_Alloc *)cn_chained_arena_alloc,
        .realloc    = (Cn_Realloc *)cn_chained_arena_realloc,
        .free       = (Cn_Free *)cn_chained_arena_free,
        .free_all   = (Cn_Free_All *)cn_chained_arena_free_all,
        .block_capacity = block_capacity,
        .block = header + 1,
    };
}

CNDEF void *cn_chained_arena_alloc(Cn_Chained_Arena *arena, size_t size) {
    CN_ASSERT(size > 0);
    CN_ASSERT(size <= arena->block_capacity);

    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

    if ((header->allocated + size) > arena->block_capacity) {
        if (header->next == NULL) {
            header->next = realloc(NULL, sizeof(Cn_Chained_Arena_Block_Header) + arena->block_capacity);
            ((Cn_Chained_Arena_Block_Header *)header->next)->next = NULL;
        }
        header = header->next;

        header->prev = arena->block;
        header->allocated = 0;
        
        arena->block = header + 1;
    }

    header->allocated += size;
    return (uint8_t *)arena->block + header->allocated - size;
}

CNDEF void *cn_chained_arena_realloc(Cn_Chained_Arena *arena, void *mem, size_t new_size) {
    CN_UNUSED(mem);
    return cn_chained_arena_alloc(arena, new_size);
}

CNDEF void cn_chained_arena_free(Cn_Chained_Arena *arena, void *mem) {
    CN_UNUSED(arena);
    CN_UNUSED(mem);
    cn_log(CN_WARNING, "Coudln't free memory in chained arena, chained arena doesn't implement free mechanism, use free_all instead, or destroy chained arena completely.");
}

CNDEF void cn_chained_arena_free_all(Cn_Chained_Arena *arena) {
    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

    while (header->prev != NULL) {
        header->allocated = 0;
        arena->block = header->prev;
        header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
    }

    header->allocated = 0;
}

CNDEF void cn_chained_arena_dealloc(Cn_Chained_Arena *arena, size_t size) {
    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
    size_t decrease;

    while (size > 0) {
        if (header->allocated == 0) {
            // No more blocks to deallocate simply return.
            if (header->prev == NULL) return;
            
            // going back to the previous.
            arena->block = header->prev;
        }

        header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

        decrease = size > header->allocated ? header->allocated : size;
        header->allocated -= decrease;
        size -= decrease;
    }
}

CNDEF int64_t cn_chained_arena_allocation_info(Cn_Chained_Arena *arena, void *allocation_ptr, size_t *offset) {
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

CNDEF size_t cn_chained_arena_allocated(Cn_Chained_Arena *arena) {
    size_t allocated = 0;
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

CNDEF void cn_chained_arena_destroy(Cn_Chained_Arena *arena) {
    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

    while (header->prev != NULL) {
        arena->block = header->prev;
        free(header);
        header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
    }

    free(header);

    arena->block = NULL;
    arena->block_capacity = 0;
}

static Cn_Pool_Block *cn__pool_block_make(size_t capacity) {
    Cn_Pool_Block *block = realloc(NULL, sizeof(Cn_Pool_Block));

    if (block == NULL) {
        cn_log(CN_ERROR, "Couldn't malloc %zu bytes of memory for the pool block.", sizeof(Cn_Pool_Block));
        return NULL;
    }

    block->next = NULL;
    block->capacity = capacity;
    memset(block->flags, 0, sizeof(block->flags));

    return block;
}

/**
 * Occupies first free slot of the block, NULL if every slot of the block is occupied.
 */
static void *cn__pool_block_alloc(Cn_Pool_Block *block) {
    size_t slots = CN_POOL_BLOCK_SLOTS(block);

    for (size_t slot = 0; slot < slots; slot++) {
        // Whole byte of flags is occupied, skipping all 8 slots it covers at once.
        if (block->flags[slot / 8] == 0xFF) {
            slot += 7 - (slot % 8);
            continue;
        }

        if (block->flags[slot / 8] & (1 << (slot % 8))) continue;

        block->flags[slot / 8] |= 1 << (slot % 8);
        return block->mem + slot * block->capacity;
    }

    return NULL;
}

CNDEF Cn_Pool cn__pool_make(size_t capacities[], size_t capacities_length) {
    CN_ASSERT(capacities_length > 0);
    CN_ASSERT(sizeof(size_t) == sizeof(Cn_Pool_Block *));

    // IMPORTANT: Array of capacities is reused as an array of blocks, every capacity is
    // read before the block allocated from it overwrites the very same slot.
    Cn_Pool_Block **blocks = (Cn_Pool_Block **)capacities;
    size_t previous = 0;

    for (size_t i = 0; i < capacities_length; i++) {
        size_t capacity = capacities[i];

        CN_ASSERT(capacity >= CN_POOL_BLOCK_MIN_CAPACITY);
        CN_ASSERT(capacity <= CN_POOL_BLOCK_MAX_CAPACITY);
        CN_ASSERT(capacity % CN_POOL_BLOCK_MIN_CAPACITY == 0);
        CN_ASSERT(capacity > previous); // Capacities must be strictly ascending.
        previous = capacity;

        blocks[i] = cn__pool_block_make(capacity);
    }

    return (Cn_Pool) {
        .alloc          = (Cn_Alloc *)cn_pool_alloc,
        .realloc        = (Cn_Realloc *)cn_pool_realloc,
        .free           = (Cn_Free *)cn_pool_free,
        .free_all       = (Cn_Free_All *)cn_pool_free_all,
        .blocks         = blocks,
        .blocks_length  = capacities_length,
    };
}

CNDEF void *cn_pool_alloc(Cn_Pool *pool, size_t size) {
    CN_ASSERT(size > 0);

    for (size_t i = 0; i < pool->blocks_length; i++) {
        Cn_Pool_Block *block = pool->blocks[i];
        CN_ASSERT(block != NULL);

        // Blocks are ordered by capacity, so the first one that fits is the tightest fit.
        if (size > block->capacity) continue;

        while (true) {
            void *mem = cn__pool_block_alloc(block);
            if (mem != NULL) return mem;

            // Every block of this size class is full, chaining one more to the end.
            if (block->next == NULL) {
                block->next = cn__pool_block_make(block->capacity);
                if (block->next == NULL) return NULL;
            }

            block = block->next;
        }
    }

    cn_log(CN_ERROR, "Couldn't allocate %zu bytes of memory from the pool, no block has big enough capacity per allocation.", size);
    return NULL;
}

CNDEF void *cn_pool_realloc(Cn_Pool *pool, void *mem, size_t new_size) {
    if (mem == NULL) return cn_pool_alloc(pool, new_size);

    size_t slot;
    Cn_Pool_Block *block = cn_pool_allocation_info(pool, mem, &slot);

    if (block == NULL) {
        cn_log(CN_WARNING, "Couldn't realloc memory in the pool, supplied pointer doesn't point to the memory allocated by it.");
        return NULL;
    }

    // New size still fits the slot, nothing has to move.
    if (new_size <= block->capacity) return mem;

    void *new_mem = cn_pool_alloc(pool, new_size);
    if (new_mem == NULL) return NULL;

    memcpy(new_mem, mem, block->capacity);
    block->flags[slot / 8] &= ~(1 << (slot % 8));

    return new_mem;
}

CNDEF void cn_pool_free(Cn_Pool *pool, void *mem) {
    if (mem == NULL) return;

    size_t slot;
    Cn_Pool_Block *block = cn_pool_allocation_info(pool, mem, &slot);

    if (block == NULL) {
        cn_log(CN_WARNING, "Couldn't free memory in the pool, supplied pointer doesn't point to the memory allocated by it.");
        return;
    }

    block->flags[slot / 8] &= ~(1 << (slot % 8));
}

CNDEF void cn_pool_free_all(Cn_Pool *pool) {
    for (size_t i = 0; i < pool->blocks_length; i++) {
        for (Cn_Pool_Block *block = pool->blocks[i]; block != NULL; block = block->next) {
            memset(block->flags, 0, sizeof(block->flags));
        }
    }
}

CNDEF Cn_Pool_Block *cn_pool_allocation_info(Cn_Pool *pool, void *allocation_ptr, size_t *slot) {
    uint8_t *ptr = allocation_ptr;

    for (size_t i = 0; i < pool->blocks_length; i++) {
        for (Cn_Pool_Block *block = pool->blocks[i]; block != NULL; block = block->next) {
            size_t used = CN_POOL_BLOCK_SLOTS(block) * block->capacity;

            if (ptr < block->mem || ptr >= block->mem + used) continue;

            size_t offset = ptr - block->mem;

            // Pointer points inside of the slot, instead of the beginning of it.
            if (offset % block->capacity != 0) return NULL;

            size_t found = offset / block->capacity;

            // Slot is free, so nothing was allocated at this address.
            if (!(block->flags[found / 8] & (1 << (found % 8)))) return NULL;

            *slot = found;
            return block;
        }
    }

    // No block was found that contains address supplied by the user.
    return NULL;
}

CNDEF void cn_pool_destroy(Cn_Pool *pool) {
    for (size_t i = 0; i < pool->blocks_length; i++) {
        Cn_Pool_Block *block = pool->blocks[i];

        while (block != NULL) {
            Cn_Pool_Block *next = block->next;
            free(block);
            block = next;
        }

        pool->blocks[i] = NULL;
    }

    pool->blocks = NULL;
    pool->blocks_length = 0;
}

CNDEF void *cn_std_alloc(Cn_Allocator *allocator, size_t size) {
    CN_UNUSED(allocator);
    return malloc(size);
}

CNDEF void *cn_std_realloc(Cn_Allocator *allocator, void *mem, size_t size) {
    CN_UNUSED(allocator);
    return realloc(mem, size);
}

CNDEF void cn_std_free(Cn_Allocator *allocator, void *mem) {
    CN_UNUSED(allocator);
    free(mem);
}

CNDEF void cn_std_free_all(Cn_Allocator *allocator) {
    CN_UNUSED(allocator);
    cn_log(CN_WARNING, "Coudln't free all memory in std allocator, std allocator doesn't implement free all mechanism, use free instead.");
}

Cn_Allocator cn_std_allocator = {
    .alloc      = (Cn_Alloc *)cn_std_alloc, 
    .realloc    = (Cn_Realloc *)cn_std_realloc, 
    .free       = (Cn_Free *)cn_std_free, 
    .free_all   = (Cn_Free_All *)cn_std_free_all
};

Cn_Allocator *cn_default_allocator = &cn_std_allocator;

CNDEF void *cn_alloc(Cn_Allocator *allocator, size_t size) {
    CN_ASSERT(allocator != NULL);
    CN_ASSERT(allocator->alloc != NULL);
    return allocator->alloc(allocator, size);
}

CNDEF void *cn_realloc(Cn_Allocator *allocator, void *mem, size_t size) {
    CN_ASSERT(allocator != NULL);
    CN_ASSERT(allocator->realloc != NULL);
    return allocator->realloc(allocator, mem, size);
}

CNDEF void cn_free(Cn_Allocator *allocator, void *mem) {
    CN_ASSERT(allocator != NULL);
    CN_ASSERT(allocator->free != NULL);
    return allocator->free(allocator, mem);
}

CNDEF void cn_free_all(Cn_Allocator *allocator) {
    CN_ASSERT(allocator != NULL);
    CN_ASSERT(allocator->free_all != NULL);
    return allocator->free_all(allocator);
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Array List
 * ============================================
 */
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

CNDEF void cn__array_list_shift_to_fit(void **list, int64_t index, int64_t extra_length) {
    Cn_Array_List_Header *header = (Cn_Array_List_Header *)((uint8_t *)*list - sizeof(Cn_Array_List_Header));

    cn__array_list_resize_to_fit(list, extra_length + header->length);

    header = (Cn_Array_List_Header *)((uint8_t *)*list - sizeof(Cn_Array_List_Header));
    
    memmove(*list + (index + extra_length) * header->item_size, *list + index * header->item_size, (header->length - index) * header->item_size);

    header->length += extra_length;
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Hash Table
 * ============================================
 */
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Hash Set
 * ============================================
 */
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Source & Location
 * ============================================
 */
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Lexer
 * ============================================
 */
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
    { CN_TOKEN_CASE,            CN_STR_BUFFER("case") },
    { CN_TOKEN_DEFAULT,         CN_STR_BUFFER("default") },
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
        case CN_TOKEN_CASE:             return "case";
        case CN_TOKEN_DEFAULT:          return "default";
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Type
 * ============================================
 */
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
    return type->kind == CN_INTEGER || type->kind == CN_FLOAT || type->kind == CN_ENUM;
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Any
 * ============================================
 */
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Abstract Syntax Tree
 * ============================================
 */
Cn_Lexer          cn__saved_lexer            = {0};
Cn_String_Builder cn__emitter_sb             = {0};
Cn_Emitter        cn__emitter                = { .write = cn_emit_write_sb, .ctx = &cn__emitter_sb };
Cn_Emitter        cn__emitter_saved          = {0};
Cn_Ast_Checkpoint cn__ast_checkpoint_message = {0};
Cn_Result         cn__message_result         = CN_RESULT_NONE;

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
    checkpoint->saved_output_length = cn_chained_arena_allocated(&data->output_arena);
    if (!(checkpoint->flags & CN_AST_CHECKPOINT_IGNORE_AST_NODES)) {
        checkpoint->saved_node_length = cn_array_list_length(&data->node_list);
    }
    checkpoint->saved_type_length = cn_chained_arena_allocated(&data->type_arena);
    checkpoint->saved_type_children_length = cn_chained_arena_allocated(&data->type_children_arena);
    checkpoint->saved_binding_length = cn_array_list_length(&data->binding_list);
    checkpoint->saved_label_binding_length = cn_array_list_length(&data->label_binding_list);
    checkpoint->saved_binding_defined_idx_length = cn_array_list_length(&data->binding_defined_idx_list);
    checkpoint->saved_scoped_strings_length = cn_chained_arena_allocated(&data->scoped_strings_arena);
    // checkpoint->saved_permanent_strings_length = cn_chained_arena_allocated(&data->permanent_strings_arena);
    checkpoint->saved_scope_length = cn_array_list_length(&data->scope_stack);
    checkpoint->saved_function_scope_idx = data->function_scope_idx;
    checkpoint->saved_idx_stack_length = cn_array_list_length(&data->idx_stack);
    checkpoint->saved_counter = data->counter;
    checkpoint->data = data;
}

CNDEF void cn_ast_checkpoint_load(Cn_Ast_Checkpoint *checkpoint) {
    CN_ASSERT(checkpoint->data != NULL);

    Cn_Ast_Data *d = checkpoint->data;

    cn_chained_arena_dealloc(&d->output_arena, cn_chained_arena_allocated(&d->output_arena) - checkpoint->saved_output_length);

    if (!(checkpoint->flags & CN_AST_CHECKPOINT_IGNORE_AST_NODES)) {
        cn_array_list_pop_multiple(&d->node_list, cn_array_list_length(&d->node_list) - checkpoint->saved_node_length);
    }

    // Removing types from hash jset entries.
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


    // Removing bindings from hash table entries.
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
    
    // Removing label bindings from hash table entries.
    for (int i = checkpoint->saved_label_binding_length; i < cn_array_list_length(&cn__ast_data->label_binding_list); i++) {
        // Resolve each binding, properly dispose each binding.
        if (cn__ast_data->label_binding_list[i].next_idx == CN_AST_NIL_IDX) {
            cn_hash_table_remove(&cn__ast_data->label_binding_table, &cn__ast_data->label_binding_list[i].name);
        } else {
            cn_hash_table_put(&cn__ast_data->label_binding_table, cn__ast_data->label_binding_list[i].next_idx, &cn__ast_data->label_binding_list[i].name);
        }
    }
    cn_array_list_pop_multiple(&d->label_binding_list, cn_array_list_length(&d->label_binding_list) - checkpoint->saved_label_binding_length);

    // Undefining all bindings that contain definition that was made after checkpoint was set,
    // but that were declared before checkpint was set.
    for (int64_t i = checkpoint->saved_binding_defined_idx_length; i < cn_array_list_length(&cn__ast_data->binding_defined_idx_list); i++) {
        if (cn__ast_data->binding_defined_idx_list[i] < checkpoint->saved_binding_length) {
            Cn_Ast_Binding *binding = cn_ast_binding_get(cn__ast_data->binding_defined_idx_list[i]);
            
            // Should only be tag binding.
            CN_ASSERT(binding->kind == CN_BINDING_TAG);
            
            // Undefining binding.
            switch (binding->type->kind) {
                case CN_STRUCT:
                    binding->type->struct_t.flags          = 0;
                    binding->type->struct_t.members_length = 0;
                    binding->type->struct_t.members        = NULL;
                    binding->type->struct_t.align          = 0;
                    binding->type->struct_t.size           = 0;
                    break;
                case CN_UNION:
                    binding->type->union_t.flags          = 0;
                    binding->type->union_t.members_length = 0;
                    binding->type->union_t.members        = NULL;
                    binding->type->union_t.align          = 0;
                    binding->type->union_t.size           = 0;
                    break;
                case CN_ENUM:
                    binding->type->enum_t.flags          = 0;
                    binding->type->enum_t.member_type    = NULL;
                    binding->type->enum_t.members_length = 0;
                    binding->type->enum_t.members        = NULL;
                    binding->type->enum_t.align          = 0;
                    binding->type->enum_t.size           = 0;
                    break;
                default:
                    break;
            }
        }
    }

    cn_array_list_pop_multiple(&d->binding_defined_idx_list, cn_array_list_length(&d->binding_defined_idx_list) - checkpoint->saved_binding_defined_idx_length);

    cn_chained_arena_dealloc(&d->scoped_strings_arena, cn_chained_arena_allocated(&d->scoped_strings_arena) - checkpoint->saved_scoped_strings_length);

    // cn_chained_arena_dealloc(&d->permanent_strings_arena, cn_chained_arena_allocated(&d->permanent_strings_arena) - checkpoint->saved_permanent_strings_length);

    cn_array_list_pop_multiple(&d->scope_stack, cn_array_list_length(&d->scope_stack) - checkpoint->saved_scope_length);

    d->function_scope_idx = checkpoint->saved_function_scope_idx;

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

    data->output_arena = cn_chained_arena_make(4096);
    cn__emitter_sb = cn_sb_make(4096);

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

    data->label_binding_list = cn_array_list_make(Cn_Ast_Binding, CN_AST_LABEL_BINDING_LIST_INITIAL_CAP);

    data->label_binding_table = cn_hash_table_make(Cn_String, Cn_Ast_Idx, CN_AST_LABEL_BINDING_TABLE_INITIAL_CAP,(Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);

    data->binding_defined_idx_list = cn_array_list_make(Cn_Ast_Binding_Idx, 16);

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

    cn_chained_arena_destroy(&data->permanent_strings_arena);

    cn_chained_arena_destroy(&data->scoped_strings_arena);

    cn_hash_table_free(&data->tag_binding_table);

    cn_hash_table_free(&data->symbol_binding_table);

    cn_array_list_free(&data->binding_list);

    cn_array_list_free(&data->label_binding_list);

    cn_hash_table_free(&data->label_binding_table);

    cn_array_list_free(&data->binding_defined_idx_list);

    cn_chained_arena_destroy(&data->type_children_arena);

    cn_hash_set_free(&data->type_ptr_set);

    cn_chained_arena_destroy(&data->type_arena);

    cn_array_list_free(&data->node_list);

    cn_chained_arena_destroy(&data->output_arena);

    *data = (Cn_Ast_Data) {0};
}

CNDEF void *cn_ast_get(Cn_Ast_Idx idx) {
    CN_ASSERT(idx != CN_AST_NIL_IDX);
    return cn__ast_data->node_list + (idx);
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

    if (count <= 0) return (Cn_Ast_List) { .idxs = NULL, .length = 0 };
    

    // Allocate from permanent_strings_arena.
    Cn_Ast_Idx *idxs = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, count * sizeof(Cn_Ast_Idx));

    // Copy indices from stack.
    memcpy(idxs, cn__ast_data->idx_stack + mark, count * sizeof(Cn_Ast_Idx));

    // Pop the finalized indices from stack.
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
            break;
        case CN_AST_FUNCTION:
            ADD_LIST(&node->function.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->function.declaration_specifiers_idx);
            ADD_IDX(&node->function.declarator_idx);
            ADD_IDX(&node->function.block_idx);
            break;
        case CN_AST_BLOCK:
            ADD_LIST(&node->block.attribute_specifiers, "attribute_specifiers");
            ADD_LIST(&node->block.block_items, "block_items");
            break;
        case CN_AST_BLOCK_ITEM:
            ADD_IDX(&node->block_item.declaration_or_statement_idx);
            break;
        case CN_AST_IF:
            ADD_LIST(&node->if_statement.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->if_statement.condition_idx);
            ADD_IDX(&node->if_statement.then_idx);
            ADD_IDX(&node->if_statement.else_idx);
            break;
        case CN_AST_SWITCH:
            ADD_LIST(&node->switch_statement.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->switch_statement.condition_idx);
            ADD_IDX(&node->switch_statement.body_idx);
            break;
        case CN_AST_WHILE:
            ADD_LIST(&node->while_statement.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->while_statement.condition_idx);
            ADD_IDX(&node->while_statement.body_idx);
            break;
        case CN_AST_DO_WHILE:
            ADD_LIST(&node->do_while.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->do_while.body_idx);
            ADD_IDX(&node->do_while.condition_idx);
            break;
        case CN_AST_FOR:
            ADD_LIST(&node->for_statement.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->for_statement.initialization_idx);
            ADD_IDX(&node->for_statement.condition_idx);
            ADD_IDX(&node->for_statement.update_idx);
            ADD_IDX(&node->for_statement.body_idx);
            break;
        case CN_AST_LABEL:
            ADD_LIST(&node->label.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->label.identifier_idx);
            ADD_IDX(&node->label.expression_idx);
            ADD_IDX(&node->label.statement_idx);
            break;
        case CN_AST_GOTO:
            ADD_LIST(&node->goto_statement.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->goto_statement.identifier_idx);
            break;
        case CN_AST_RETURN:
            ADD_LIST(&node->return_statement.attribute_specifiers, "attribute_specifiers");
            ADD_IDX(&node->return_statement.expression_idx);
            break;
        case CN_AST_BREAK:
            ADD_LIST(&node->break_statement.attribute_specifiers, "attribute_specifiers");
            break;
        case CN_AST_CONTINUE:
            ADD_LIST(&node->continue_statement.attribute_specifiers, "attribute_specifiers");
            break;
        case CN_AST_EXPRESSION_STATEMENT:
            ADD_LIST(&node->expression_statement.attribute_specifiers, "attribute_specifiers");
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
            ADD_IDX(&node->init_declarator.gnu_asm_label_idx);
            ADD_LIST(&node->init_declarator.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            ADD_IDX(&node->init_declarator.initializer_idx);
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
            ADD_LIST(&node->struct_specifier.attribute_specifiers, "attribute_specifiers");
            ADD_LIST(&node->struct_specifier.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            ADD_IDX(&node->struct_specifier.identifier_idx);
            ADD_LIST(&node->struct_specifier.member_declarations, "member_declarations");
            break;
        case CN_AST_UNION_SPECIFIER:
            ADD_LIST(&node->union_specifier.attribute_specifiers, "attribute_specifiers");
            ADD_LIST(&node->union_specifier.gnu_attribute_specifiers, "gnu_attribute_specifiers");
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
            ADD_LIST(&node->enum_specifier.attribute_specifiers, "attribute_specifiers");
            ADD_LIST(&node->enum_specifier.gnu_attribute_specifiers, "gnu_attribute_specifiers");
            ADD_IDX(&node->enum_specifier.identifier_idx);
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
    }
    return "<invalid kind>";
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Emit
 * ============================================
 */
CNDEF void cn_emit_write_file(Cn_String str, void *ctx) {
    fwrite(str.data, 1, (size_t)str.length, (FILE *)ctx);
}

CNDEF void cn_emit_write_sb(Cn_String str, void *ctx) {
    cn_sb_append_str((Cn_String_Builder *)ctx, str);
}

/**
 * Helper to emit string and track written_length.
 */
CNDEF void cn__emit_str(Cn_Emitter *e, Cn_String str) {
    e->write(str, e->ctx);
    e->written_length += str.length;
}

#define cn__emit_str_lit(e, lit) cn__emit_str(e, CN_STR_LIT(lit))

/**
 * Helper to emit indentation.
 * Really only called after every new line emittion.
 * emit functions don't print indents themselves it is automatically 
 * inserted after new line.
 */
CNDEF void cn__emit_indent(Cn_Emitter *e) {
    for (int i = 0; i < e->indent; i++) cn__emit_str(e, CN_STR_LIT(CN_INDENT));
}

CNDEF void cn__emit_newline(Cn_Emitter *e) {
    cn__emit_str(e, CN_STR_LIT(CN_LINE_END));
    cn__emit_indent(e);
}

CNDEF void cn__emit_code(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Code *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CODE);

    // Emitter is responsible to add indent after every newline user inserts,
    // according to the ident level.
    Cn_String left = node->text;
    while (true) {
        int64_t newline = cn_str_find_left(left, CN_STR_LIT(CN_LINE_END));
        if (newline == -1) break;

        cn__emit_str(e, cn_str_get_chars(left, newline + sizeof(CN_LINE_END) - 1));
        cn__emit_indent(e);

        left = cn_str_eat_chars(left, newline + sizeof(CN_LINE_END) - 1);
    }

    // Writing whats left.
    cn__emit_str(e, left);
}

// Forward declarations of internal emit functions.
CNDEF void cn__emit_storage_specifiers(Cn_Emitter *e, Cn_Storage_Specifier_Flags storage);
CNDEF void cn__emit_qualifiers(Cn_Emitter *e, Cn_Qualifier_Flags qualifiers);
CNDEF void cn__emit_function_specifiers(Cn_Emitter *e, Cn_Function_Specifier_Flags func_spec);
CNDEF void cn__emit_gnu_attribute_specifiers(Cn_Emitter *e, Cn_Ast_List specifiers);
CNDEF void cn__emit_attribute_specifiers(Cn_Emitter *e, Cn_Ast_List specifiers);
CNDEF void cn__emit_designations(Cn_Emitter *e, Cn_Ast_List designations);

CNDEF void cn__emit_translation_unit(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Translation_Unit *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TRANSLATION_UNIT);

    for (int64_t i = 0; i < node->external_declarations.length; i++) {
        cn_emit(e, node->external_declarations.idxs[i]);
    }
}

CNDEF void cn__emit_external_declaration(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_External_Declaration *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_EXTERNAL_DECLARATION);

    if (node->flags & CN_AST_EXTERNAL_DECLARATION_HAS_EXTENSION) {
        cn__emit_str_lit(e, "__extension__ ");
    }

    if (node->child_idx == CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, ";");
        cn__emit_newline(e);
    } else {
        if (cn_ast_get_as_node(node->child_idx)->kind == CN_AST_DECLARATION)
            e->flags |= CN_EMITTER_END_DECLARATION_WITH_SEMICOLON;

        cn_emit(e, node->child_idx);
    }
}

CNDEF void cn__emit_declaration(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Declaration *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DECLARATION);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn_emit(e, node->declaration_specifiers_idx);

    for (int64_t i = 0; i < node->init_declarators.length; i++) {
        if (i == 0) cn__emit_str_lit(e, " ");
        else cn__emit_str_lit(e, ", ");

        cn_emit(e, node->init_declarators.idxs[i]);
    }

    if (e->flags & CN_EMITTER_END_DECLARATION_WITH_SEMICOLON) {
        cn__emit_str_lit(e, ";");
        cn__emit_newline(e);
        e->flags &= ~CN_EMITTER_END_DECLARATION_WITH_SEMICOLON;
    }
}

CNDEF void cn__emit_function(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Function *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_FUNCTION);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn_emit(e, node->declaration_specifiers_idx);
    cn__emit_str_lit(e, " ");
    cn_emit(e, node->declarator_idx);
    cn__emit_str_lit(e, " ");
    cn_emit(e, node->block_idx);
}

CNDEF void cn__emit_block(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Block *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BLOCK);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn__emit_str_lit(e, "{");

    e->indent++;
    cn__emit_newline(e);

    for (int64_t i = 0; i < node->block_items.length; i++) {
        cn_emit(e, node->block_items.idxs[i]);
    }

    e->indent--;
    cn__emit_newline(e);

    cn__emit_str_lit(e, "}");
    
    if (e->flags & CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK) {
        e->flags &= ~CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK;
        return;
    }

    cn__emit_newline(e);
}

CNDEF void cn__emit_block_item(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Block_Item *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BLOCK_ITEM);

    if (cn_ast_get_as_node(node->declaration_or_statement_idx)->kind == CN_AST_DECLARATION)
        e->flags |= CN_EMITTER_END_DECLARATION_WITH_SEMICOLON;
    cn_emit(e, node->declaration_or_statement_idx);
}

CNDEF void cn__emit_if_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_If *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_IF);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_str_lit(e, "if (");
    cn_emit(e, node->condition_idx);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *then_stmt = cn_ast_get(node->then_idx);
    if (then_stmt->kind == CN_AST_BLOCK) {
        e->flags |= CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK;
        cn_emit(e, node->then_idx);

        if (node->else_idx != CN_AST_NIL_IDX) {
            cn__emit_str_lit(e, " ");
        } else {
            cn__emit_newline(e);
        }
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->then_idx);
    }

    if (node->else_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, "else ");
        Cn_Ast_Node *else_stmt = cn_ast_get(node->else_idx);
        if (else_stmt->kind == CN_AST_BLOCK || else_stmt->kind == CN_AST_IF) {
            cn_emit(e, node->else_idx);
        } else {
            e->indent++;
            cn__emit_newline(e);
            e->indent--;

            cn_emit(e, node->else_idx);
        }
    }
}

CNDEF void cn__emit_switch_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Switch *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_SWITCH);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_str_lit(e, "switch (");
    cn_emit(e, node->condition_idx);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *body = cn_ast_get(node->body_idx);
    if (body->kind == CN_AST_BLOCK) {
        cn_emit(e, node->body_idx);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->body_idx);
    }
}

CNDEF void cn__emit_while_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_While *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_WHILE);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_str_lit(e, "while (");
    cn_emit(e, node->condition_idx);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *body = cn_ast_get(node->body_idx);
    if (body->kind == CN_AST_BLOCK) {
        cn_emit(e, node->body_idx);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->body_idx);
    }
}

CNDEF void cn__emit_do_while(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Do_While *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DO_WHILE);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_str_lit(e, "do ");

    Cn_Ast_Node *body = cn_ast_get(node->body_idx);
    if (body->kind == CN_AST_BLOCK) {
        // "while" part has to stay on the same line as the closing brace.
        e->flags |= CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK;
        cn_emit(e, node->body_idx);

        cn__emit_str_lit(e, " ");
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        // Body ends its own line, so "while" part starts already indented.
        cn_emit(e, node->body_idx);
    }

    cn__emit_str_lit(e, "while (");
    cn_emit(e, node->condition_idx);
    cn__emit_str_lit(e, ");");
    cn__emit_newline(e);
}

CNDEF void cn__emit_for_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_For *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_FOR);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_str_lit(e, "for (");

    // Initialization can be a declaration, but its semicolon is emitted here
    // as a part of the for clause, so the declaration flag is left unset.
    cn_emit(e, node->initialization_idx);
    cn__emit_str_lit(e, ";");

    if (node->condition_idx != CN_AST_NIL_IDX)
        cn__emit_str_lit(e, " ");

    cn_emit(e, node->condition_idx);
    cn__emit_str_lit(e, ";");

    if (node->update_idx != CN_AST_NIL_IDX)
        cn__emit_str_lit(e, " ");

    cn_emit(e, node->update_idx);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *body = cn_ast_get(node->body_idx);
    if (body->kind == CN_AST_BLOCK) {
        cn_emit(e, node->body_idx);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->body_idx);
    }
}

CNDEF void cn__emit_label(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Label *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_LABEL);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    if (node->flags & CN_AST_LABEL_IS_CASE) {
        cn__emit_str_lit(e, "case ");
        cn_emit(e, node->expression_idx);
    }
    else if (node->flags & CN_AST_LABEL_IS_DEFAULT) {
        cn__emit_str_lit(e, "default");
    } else {
        cn_emit(e, node->identifier_idx);
    }

    cn__emit_str_lit(e, ": ");

    if (cn_ast_get_as_node(node->statement_idx)->kind == CN_AST_BLOCK) {
        cn_emit(e, node->statement_idx);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->statement_idx);
    }
}

CNDEF void cn__emit_goto_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Goto *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GOTO);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn__emit_str_lit(e, "goto ");
    cn_emit(e, node->identifier_idx);
    cn__emit_str_lit(e, ";");
    cn__emit_newline(e);
}

CNDEF void cn__emit_return_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Return *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_RETURN);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn__emit_str_lit(e, "return");
    if (node->expression_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->expression_idx);
    }
    cn__emit_str_lit(e, ";");
    cn__emit_newline(e);
}

CNDEF void cn__emit_break_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Break *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BREAK);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn__emit_str_lit(e, "break;");
    cn__emit_newline(e);
}

CNDEF void cn__emit_continue_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Continue *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CONTINUE);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn__emit_str_lit(e, "continue;");
    cn__emit_newline(e);
}

CNDEF void cn__emit_expression_statement(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Expression_Statement *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_EXPRESSION_STATEMENT);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn_emit(e, node->expression_idx);
    cn__emit_str_lit(e, ";");
    cn__emit_newline(e);
}

CNDEF void cn__emit_unary(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Unary *node = cn_ast_get(node_idx);
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
    cn__emit_str(e, op);
    cn_emit(e, node->expression_idx);
}

CNDEF void cn__emit_postfix(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Postfix *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_POSTFIX);

    cn_emit(e, node->expression_idx);

    switch (node->operator) {
        case CN_POSTFIX_OP_INCREMENT: cn__emit_str_lit(e, "++"); break;
        case CN_POSTFIX_OP_DECREMENT: cn__emit_str_lit(e, "--"); break;
        default: break;
    }
}

CNDEF void cn__emit_binary(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Binary *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_BINARY);

    // Special case for array subscript.
    if (node->operator == CN_BINARY_OP_ARRAY_SUB) {
        cn__emit_str_lit(e, "(");
        cn_emit(e, node->left_idx);
        cn__emit_str_lit(e, "[");
        cn_emit(e, node->right_idx);
        cn__emit_str_lit(e, "])");
        return;
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

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->left_idx);
    cn__emit_str(e, op);
    cn_emit(e, node->right_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_access(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Access *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ACCESS);

    cn_emit(e, node->expression_idx);

    if (node->pointer) {
        cn__emit_str_lit(e, "->");
    } else {
        cn__emit_str_lit(e, ".");
    }

    cn_emit(e, node->member_idx);
}

CNDEF void cn__emit_call(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Call *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CALL);

    cn_emit(e, node->expression_idx);

    cn__emit_str_lit(e, "(");
    for (int64_t i = 0; i < node->arguments.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->arguments.idxs[i]);
    }
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_cast(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Cast *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_CAST);

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->type_name_idx);
    cn__emit_str_lit(e, ")");
    cn_emit(e, node->expression_idx);
}

CNDEF void cn__emit_compound(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Compound *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_COMPOUND);

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->type_name_idx);
    cn__emit_str_lit(e, ") ");

    cn__emit_str_lit(e, "{ ");
    cn__emit_designations(e, node->designations);
    cn__emit_str_lit(e, "}");
}

CNDEF void cn__emit_sizeof_expression(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Sizeof *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_SIZEOF);

    cn__emit_str_lit(e, "sizeof(");
    cn_emit(e, node->target_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_ternary(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Ternary *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TERNARY);

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->condition_idx);
    cn__emit_str_lit(e, " ? ");
    cn_emit(e, node->true_idx);
    cn__emit_str_lit(e, " : ");
    cn_emit(e, node->false_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_assign(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Assign *node = cn_ast_get(node_idx);
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

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->left_idx);
    cn__emit_str(e, op);
    cn_emit(e, node->right_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_primary(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Primary *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_PRIMARY);

    cn_emit(e, node->literal_idx);
}

CNDEF void cn__emit_initializer(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Initializer *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_INITIALIZER);

    if (node->expression_idx != CN_AST_NIL_IDX) {
        cn_emit(e, node->expression_idx);
        return;
    }

    cn__emit_str_lit(e, "{ ");
    cn__emit_designations(e, node->designations);
    cn__emit_str_lit(e, "}");
}

CNDEF void cn__emit_designations(Cn_Emitter *e, Cn_Ast_List designations) {
    for (int64_t i = 0; i < designations.length; i++) {
        cn_emit(e, designations.idxs[i]);

        if (i < designations.length - 1) {
            cn__emit_str_lit(e, ", ");
        } else {
            cn__emit_str_lit(e, " ");
        }
    }
}

CNDEF void cn__emit_designation(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Designation *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DESIGNATION);

    for (int64_t i = 0; i < node->designators.length; i++) {
        cn_emit(e, node->designators.idxs[i]);

        if (i == node->designators.length - 1) {
            cn__emit_str_lit(e, " = ");
        }
    }

    cn_emit(e, node->initializer_idx);
}

CNDEF void cn__emit_designator(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Designator *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DESIGNATOR);

    if (node->identifier_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, ".");
        cn_emit(e, node->identifier_idx);
        return;
    }

    cn__emit_str_lit(e, "[");

    cn_emit(e, node->expression_idx);

    if (node->expression_range_end_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " ... ");
        cn_emit(e, node->expression_range_end_idx);
    }

    cn__emit_str_lit(e, "]");
}

CNDEF void cn__emit_declarator(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Declarator *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DECLARATOR);

    cn_emit(e, node->pointer_idx);
    cn_emit(e, node->direct_declarator_idx);
}

CNDEF void cn__emit_pointer(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Pointer *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_POINTER);

    cn__emit_str_lit(e, "*");
    cn__emit_qualifiers(e, node->qualifiers);
    cn_emit(e, node->pointer_idx);
}

CNDEF void cn__emit_direct_declarator_grouped(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Direct_Declarator_Grouped *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DIRECT_DECLARATOR_GROUPED);

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->declarator_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_direct_declarator_array(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Direct_Declarator_Array *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DIRECT_DECLARATOR_ARRAY);

    cn_emit(e, node->direct_declarator_idx);
    cn__emit_str_lit(e, "[");
    cn_emit(e, node->expression_idx);
    cn__emit_str_lit(e, "]");
}

CNDEF void cn__emit_direct_declarator_function(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Direct_Declarator_Function *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DIRECT_DECLARATOR_FUNCTION);

    cn_emit(e, node->direct_declarator_idx);
    cn__emit_str_lit(e, "(");
    for (int64_t i = 0; i < node->parameter_declarations.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->parameter_declarations.idxs[i]);
    }
    if (node->flags & CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC) {
        if (node->parameter_declarations.length > 0) cn__emit_str_lit(e, ", ");
        cn__emit_str_lit(e, "...");
    }
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_declaration_specifiers(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Declaration_Specifiers *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_DECLARATION_SPECIFIERS);

    cn__emit_storage_specifiers(e, node->storage_specifiers);
    cn__emit_function_specifiers(e, node->function_specifiers);
    cn__emit_gnu_attribute_specifiers(e, node->gnu_attribute_specifiers);
    cn__emit_qualifiers(e, node->qualifiers);
    cn_emit(e, node->type_specifier_idx);
}

CNDEF void cn__emit_storage_specifiers(Cn_Emitter *e, Cn_Storage_Specifier_Flags storage) {
    if (storage & CN_STORAGE_SPECIFIER_TYPEDEF)  cn__emit_str_lit(e, "typedef ");
    if (storage & CN_STORAGE_SPECIFIER_EXTERN)   cn__emit_str_lit(e, "extern ");
    if (storage & CN_STORAGE_SPECIFIER_STATIC)   cn__emit_str_lit(e, "static ");
    if (storage & CN_STORAGE_SPECIFIER_AUTO)     cn__emit_str_lit(e, "auto ");
    if (storage & CN_STORAGE_SPECIFIER_REGISTER) cn__emit_str_lit(e, "register ");
}

CNDEF void cn__emit_qualifiers(Cn_Emitter *e, Cn_Qualifier_Flags qualifiers) {
    if (qualifiers & CN_AST_TYPE_QUALIFIER_CONST)    cn__emit_str_lit(e, "const ");
    if (qualifiers & CN_AST_TYPE_QUALIFIER_RESTRICT) cn__emit_str_lit(e, "restrict ");
    if (qualifiers & CN_AST_TYPE_QUALIFIER_VOLATILE) cn__emit_str_lit(e, "volatile ");
    if (qualifiers & CN_AST_TYPE_QUALIFIER_ATOMIC)   cn__emit_str_lit(e, "_Atomic ");
}

CNDEF void cn__emit_function_specifiers(Cn_Emitter *e, Cn_Function_Specifier_Flags func_spec) {
    if (func_spec & CN_AST_FUNCTION_SPECIFIER_INLINE)   cn__emit_str_lit(e, "inline ");
    if (func_spec & CN_AST_FUNCTION_SPECIFIER_NORETURN) cn__emit_str_lit(e, "_Noreturn ");
}

CNDEF void cn__emit_type_specifier_primitive(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Type_Specifier_Primitive *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TYPE_SPECIFIER_PRIMITIVE);

    // Sign.
    switch (node->sign) {
        case CN_AST_TYPE_SIGN_SIGNED:   cn__emit_str_lit(e, "signed "); break;
        case CN_AST_TYPE_SIGN_UNSIGNED: cn__emit_str_lit(e, "unsigned "); break;
        default: break;
    }

    // Width.
    switch (node->width) {
        case CN_AST_TYPE_WIDTH_SHORT:     cn__emit_str_lit(e, "short "); break;
        case CN_AST_TYPE_WIDTH_LONG:      cn__emit_str_lit(e, "long "); break;
        case CN_AST_TYPE_WIDTH_LONG_LONG: cn__emit_str_lit(e, "long long "); break;
        default: break;
    }

    // Base type.
    switch (node->primitive_kind) {
        case CN_AST_TYPE_INT:    cn__emit_str_lit(e, "int"); break;
        case CN_AST_TYPE_CHAR:   cn__emit_str_lit(e, "char"); break;
        case CN_AST_TYPE_FLOAT:  cn__emit_str_lit(e, "float"); break;
        case CN_AST_TYPE_DOUBLE: cn__emit_str_lit(e, "double"); break;
        case CN_AST_TYPE_BOOL:   cn__emit_str_lit(e, "_Bool"); break;
        case CN_AST_TYPE_VOID:   cn__emit_str_lit(e, "void"); break;
        default: break;
    }
}

CNDEF void cn__emit_type_specifier_typedef(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Type_Specifier_Typedef *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TYPE_SPECIFIER_TYPEDEF);

    cn__emit_str(e, node->typedef_name);
}

CNDEF void cn__emit_struct_specifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Struct_Specifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_STRUCT_SPECIFIER);

    cn__emit_str_lit(e, "struct ");

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_gnu_attribute_specifiers(e, node->gnu_attribute_specifiers);

    cn_emit(e, node->identifier_idx);

    // Only emit body if there are members.
    if (node->member_declarations.length > 0) {
        cn__emit_str_lit(e, " {");

        e->indent++;
        for (int64_t i = 0; i < node->member_declarations.length; i++) {
            cn_emit(e, node->member_declarations.idxs[i]);
        }
        e->indent--;

        cn__emit_newline(e);
        cn__emit_str_lit(e, "}");
    }
}

CNDEF void cn__emit_union_specifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Union_Specifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_UNION_SPECIFIER);

    cn__emit_str_lit(e, "union ");

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_gnu_attribute_specifiers(e, node->gnu_attribute_specifiers);

    cn_emit(e, node->identifier_idx);

    if (node->member_declarations.length > 0) {
        cn__emit_str_lit(e, " {");

        e->indent++;
        for (int64_t i = 0; i < node->member_declarations.length; i++) {
            cn_emit(e, node->member_declarations.idxs[i]);
        }
        e->indent--;

        cn__emit_newline(e);
        cn__emit_str_lit(e, "}");
    }
}

CNDEF void cn__emit_enum_specifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Enum_Specifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ENUM_SPECIFIER);

    cn__emit_str_lit(e, "enum ");

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_gnu_attribute_specifiers(e, node->gnu_attribute_specifiers);

    cn_emit(e, node->identifier_idx);

    if (node->specifier_qualifier_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " : ");
        cn_emit(e, node->specifier_qualifier_idx);
    }

    if (node->flags & CN_AST_ENUM_HAS_DEFINITION) {
        cn__emit_str_lit(e, " {");

        e->indent++;
        for (int64_t i = 0; i < node->enumerators.length; i++) {
            cn_emit(e, node->enumerators.idxs[i]);
        }
        e->indent--;

        cn__emit_newline(e);
        cn__emit_str_lit(e, "}");
    }
}

CNDEF void cn__emit_gnu_typeof(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Gnu_Typeof *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_TYPEOF);

    cn__emit_str_lit(e, "__typeof__(");
    cn_emit(e, node->target_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_type_name(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Type_Name *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_TYPE_NAME);

    cn_emit(e, node->specifier_qualifier_idx);

    if (node->abstract_declarator_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->abstract_declarator_idx);
    }
}

CNDEF void cn__emit_specifier_qualifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Specifier_Qualifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_SPECIFIER_QUALIFIER);

    cn__emit_qualifiers(e, node->qualifiers);
    cn_emit(e, node->type_specifier_idx);
}

CNDEF void cn__emit_identifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Identifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_IDENTIFIER);

    cn__emit_str(e, node->name);
}

CNDEF void cn__emit_integer(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Integer *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_INTEGER);

    cn__emit_str(e, node->value);
}

CNDEF void cn__emit_flt(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Float *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_FLOAT);

    cn__emit_str(e, node->value);
}

CNDEF void cn__emit_string(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_String *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_STRING);

    cn__emit_str_lit(e, "\"");
    cn__emit_str(e, node->str);
    cn__emit_str_lit(e, "\"");
}

CNDEF void cn__emit_member_declaration(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Member_Declaration *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_MEMBER_DECLARATION);

    cn__emit_newline(e);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn_emit(e, node->specifier_qualifier_idx);

    for (int64_t i = 0; i < node->member_declarators.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ",");
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->member_declarators.idxs[i]);
    }
    cn__emit_str_lit(e, ";");
}

CNDEF void cn__emit_member_declarator(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Member_Declarator *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_MEMBER_DECLARATOR);

    cn_emit(e, node->declarator_idx);

    if (node->bitfield_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " : ");
        cn_emit(e, node->bitfield_idx);
    }

    cn__emit_attribute_specifiers(e, node->gnu_attribute_specifiers);
}

CNDEF void cn__emit_gnu_attribute_specifiers(Cn_Emitter *e, Cn_Ast_List specifiers) {
    for (int64_t i = 0; i < specifiers.length; i++) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, specifiers.idxs[i]);
    }
}

CNDEF void cn__emit_gnu_attribute_specifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Gnu_Attribute_Specifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_ATTRIBUTE_SPECIFIER);

    cn__emit_str_lit(e, "__attribute__((");
    for (int64_t i = 0; i < node->gnu_attributes.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->gnu_attributes.idxs[i]);
    }
    cn__emit_str_lit(e, "))");
}

CNDEF void cn__emit_gnu_attribute(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Gnu_Attribute *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_ATTRIBUTE);

    // Emit attribute identifier.
    cn_emit(e, node->identifier_idx);

    // Emit arguments if present.
    if (node->arguments.length > 0) {
        cn__emit_str_lit(e, "(");
        for (int64_t i = 0; i < node->arguments.length; i++) {
            if (i > 0) cn__emit_str_lit(e, ", ");
            cn_emit(e, node->arguments.idxs[i]);
        }
        cn__emit_str_lit(e, ")");
    }
}

CNDEF void cn__emit_init_declarator(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Init_Declarator *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_INIT_DECLARATOR);

    cn_emit(e, node->declarator_idx);

    if (node->gnu_asm_label_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->gnu_asm_label_idx);
    }

    cn__emit_gnu_attribute_specifiers(e, node->gnu_attribute_specifiers);

    if (node->initializer_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " = ");
        cn_emit(e, node->initializer_idx);
    }
}

CNDEF void cn__emit_parameter_declaration(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Parameter_Declaration *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_PARAMETER_DECLARATION);

    cn_emit(e, node->declaration_specifiers_idx);

    if (node->declarator_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->declarator_idx);
    }
}

CNDEF void cn__emit_enumerator(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Enumerator *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ENUMERATOR);

    cn__emit_newline(e);

    cn_emit(e, node->identifier_idx);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn__emit_gnu_attribute_specifiers(e, node->gnu_attribute_specifiers);

    if (node->expression_idx != CN_AST_NIL_IDX) {
        cn__emit_str_lit(e, " = ");
        cn_emit(e, node->expression_idx);
    }

    cn__emit_str_lit(e, ",");
}

CNDEF void cn__emit_attribute_specifiers(Cn_Emitter *e, Cn_Ast_List specifiers) {
    for (int64_t i = 0; i < specifiers.length; i++) {
        cn_emit(e, specifiers.idxs[i]);
        cn__emit_str_lit(e, " ");
    }
}

CNDEF void cn__emit_attribute_specifier(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Attribute_Specifier *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ATTRIBUTE_SPECIFIER);

    // Skipping empty attribute specifiers entirely.
    if (node->attributes.length == 0) return;

    cn__emit_str_lit(e, "[[");
    for (int64_t i = 0; i < node->attributes.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->attributes.idxs[i]);
    }
    cn__emit_str_lit(e, "]]");
}

CNDEF void cn__emit_attribute(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Attribute *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_ATTRIBUTE);

    // Emit vendor prefix if present.
    if (node->vendor_identifier_idx != CN_AST_NIL_IDX) {
        cn_emit(e, node->vendor_identifier_idx);
        cn__emit_str_lit(e, "::");
    }

    cn_emit(e, node->identifier_idx);

    // Emit arguments if present.
    if (node->arguments.length > 0) {
        cn__emit_str_lit(e, "(");
        for (int64_t i = 0; i < node->arguments.length; i++) {
            if (i > 0) cn__emit_str_lit(e, ", ");
            cn_emit(e, node->arguments.idxs[i]);
        }
        cn__emit_str_lit(e, ")");
    }
}

CNDEF void cn__emit_gnu_asm_label(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    CN_ASSERT(node_idx != CN_AST_NIL_IDX);

    Cn_Ast_Gnu_Asm_Label *node = cn_ast_get(node_idx);
    CN_ASSERT(node->kind == CN_AST_GNU_ASM_LABEL);

    cn__emit_str_lit(e, "__asm__(");
    cn_emit(e, node->string_idx);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn_emit(Cn_Emitter *e, Cn_Ast_Idx node_idx) {
    if (node_idx == CN_AST_NIL_IDX) return;

    Cn_Ast_Node *node = cn_ast_get_as_node(node_idx);
    switch (node->kind) {
#define X(K, T, m) case CN_AST_##K: cn__emit_##m(e, node_idx); break;
    CN_AST_GEN_LIST(X)
#undef X
        default:
            // For unhandled node types, try to emit source location if available.
            {
                Cn_String value = cn_source_to_str(&node->src);
                if (!cn_str_is_empty(value)) {
                    cn__emit_str(e, value);
                }
            }
            break;
    }
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
                .label_binding_idx = cn_array_list_length(&cn__ast_data->label_binding_list),
                .binding_defined_idx = cn_array_list_length(&cn__ast_data->binding_defined_idx_list),
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

    if (CN_AST_SCOPE_STACK_CURRENT_IDX == cn__ast_data->function_scope_idx) {

        // Removing label bindings from hash table entries.
        for (int i = scope->label_binding_idx; i < cn_array_list_length(&cn__ast_data->label_binding_list); i++) {
            // Resolve each binding, properly dispose each binding.
            if (cn__ast_data->label_binding_list[i].next_idx == CN_AST_NIL_IDX) {
                cn_hash_table_remove(&cn__ast_data->label_binding_table, &cn__ast_data->label_binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__ast_data->label_binding_table, cn__ast_data->label_binding_list[i].next_idx, &cn__ast_data->label_binding_list[i].name);
            }
        }
        cn_array_list_pop_multiple(&cn__ast_data->label_binding_list, cn_array_list_length(&cn__ast_data->label_binding_list) - scope->label_binding_idx);

        cn__ast_data->function_scope_idx = 0;
    }

    cn_array_list_pop_multiple(&cn__ast_data->binding_defined_idx_list, cn_array_list_length(&cn__ast_data->binding_defined_idx_list) - scope->binding_defined_idx);

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

    // Getting currently visible binding with the same name if such exists.
    Cn_Ast_Binding_Idx current_idx = cn_ast_binding_table_get(name, &cn__ast_data->symbol_binding_table);
    if (current_idx != CN_AST_NIL_BINDING_IDX) {
        Cn_Ast_Binding *current = cn_ast_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__ast_bindings_conflict(current, &binding)) return CN_AST_NIL_BINDING_IDX;

            // Checking if it is redefinition.
            if (current->b_function.is_definition && binding.b_function.is_definition) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' function redifinition.", CN_UNPACK(name));
                return CN_AST_NIL_BINDING_IDX;
            }

            // If newer binding is redeclaration, merge binding to definition, if not leave as it is.
            if (current->b_function.is_definition) {
                return current_idx;
            }
        }
    } else {
        binding.name = cn__ast_scope_stack_save_string(name);
    }

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

            // If newer binding is redeclaration, merge binding to definition, if not leave as it is.
            if (current->b_variable.is_definition) {
                return current_idx;
            }
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

CNDEF Cn_Ast_Binding_Idx cn_ast_label_binding_declare(Cn_String name, Cn_Ast_Idx source_idx) {
    CN_ASSERT(cn__ast_data->function_scope_idx > 0);

    Cn_Ast_Binding binding = { 
        .kind = CN_BINDING_LABEL,
        .src = source_idx,
        .scope_idx = cn__ast_data->function_scope_idx,
    };

    // Getting currently visible binding with the same name if such exists.
    Cn_Ast_Binding_Idx current_idx = cn_ast_binding_table_get(name, &cn__ast_data->label_binding_table);
    if (current_idx != CN_AST_NIL_BINDING_IDX) {
        Cn_Ast_Binding *current = cn_ast_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            // Labels are not allowed to be redefined.
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source_idx, CN_DC_ILLEGAL_BINDING, "'%.*s' label redifinition.", CN_UNPACK(name));
            return CN_AST_NIL_BINDING_IDX;
        }
    } else {
        binding.name = cn__ast_scope_stack_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_ast_binding_table_put(binding, &cn__ast_data->label_binding_table);
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

// #define CN__TRACE_ERROR
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

    Cn_Ast_Idx parent_idx = cn_ast_node_list_append(node);
    Cn_Ast_Idx child_idx;

    cn__saved_lexer = *lexer;
    cn__emitter_saved = cn__emitter;

    // Last possible case function definition or declaration.
    // Setting checkpoint.
    if (!cn_ast_checkpoint_set(&cn__ast_checkpoint_message, cn__ast_data, 0)) {
        child_idx = cn_ast_parse_function_or_declaration(lexer);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_get_as_node(parent_idx)->external_declaration.child_idx = child_idx;

        cn_ast_node_set_parent(parent_idx, child_idx);
    } else {
        child_idx = cn_ast_get_as_node(parent_idx)->external_declaration.child_idx;

        // Reparse logic -> emit + parse.
        cn__emitter = cn__emitter_saved;
        cn_sb_clear(&cn__emitter_sb);
        cn_emit(&cn__emitter, child_idx);
        
        Cn_String code = {
            .data = cn_chained_arena_alloc(&cn__ast_data->output_arena, cn__emitter_sb.length),
            .length = cn__emitter_sb.length,
        };

        cn_str_copy_to(cn_sb_to_str(&cn__emitter_sb), code.data);

        // fprintf(stderr, "KAWABANGA REPARSE OF:\n%.*s", CN_UNPACK(code));
        Cn_Lexer l = {0};
        cn_lexer_init(&l, code, cn_default_blacklist);
        l.file = cn__saved_lexer.file;
        l.line = cn__saved_lexer.line;
        
        child_idx = cn_ast_parse_function_or_declaration(&l);
        if (child_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_get_as_node(parent_idx)->external_declaration.child_idx = child_idx;

        cn_ast_node_set_parent(parent_idx, child_idx);
    }
    
    // Messaging function definition.
    if (cn__message_result != CN_RESULT_MODIFIED_NO_REPEAT) {
        if (cn_ast_get_as_node(child_idx)->kind == CN_AST_FUNCTION) {
            Cn_Message_Parsed_Function payload = {
                .node_idx = child_idx,
            };
            cn__message_result = cn__send_message(CN_MESSAGE_PARSED_FUNCTION, (Cn_Message) {
                    .parsed_function = payload,
                    });

            if (cn__message_result != CN_RESULT_NONE) cn_ast_checkpoint_load(&cn__ast_checkpoint_message);
        }
    }

    cn_ast_checkpoint_remove(&cn__ast_checkpoint_message);
    cn__message_result = CN_RESULT_NONE;

    return parent_idx;

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
        cn__ast_data->function_scope_idx = CN_AST_SCOPE_STACK_CURRENT_IDX;

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

        // ';' at the end check.
        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error_recover;

        // Adding types that are used by declarators.
        ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, node.declaration.init_declarators);
        if (!ok) goto error;

        Cn_Ast_Idx idx = cn_ast_node_list_append(node);
        cn_ast_list_set_parent(idx, &node.declaration.attribute_specifiers);
        cn_ast_list_set_parent(idx, &node.declaration.init_declarators);
        cn_ast_node_set_parent(idx, node.declaration.declaration_specifiers_idx);
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

    // Adding types that are used by declarators.
    ok = cn_ast_analyze_declaration(node.declaration.declaration_specifiers_idx, node.declaration.init_declarators);
    if (!ok) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.declaration.attribute_specifiers);
    cn_ast_list_set_parent(parent, &node.declaration.init_declarators);
    cn_ast_node_set_parent(parent, node.declaration.declaration_specifiers_idx);
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
    Cn_Lexer original_state = *lexer;

    bool ok;
    Cn_Ast_List attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    return cn_ast_continue_statement(lexer, attribute_specifiers);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_continue_statement(Cn_Lexer *lexer, Cn_Ast_List attribute_specifiers) {
    Cn_Ast_Idx statement_idx;

    if (cn_lexer_expect(lexer, CN_TOKEN_CASE) || cn_lexer_peek(lexer, 1).type == CN_TOKEN_COLON) {
        statement_idx = cn_ast_parse_label(lexer);
        if (statement_idx == CN_AST_NIL_IDX) goto error;

        cn_ast_get_as_node(statement_idx)->label.attribute_specifiers = attribute_specifiers;
    } else {
        switch (cn_lexer_token(lexer).type) {
            case CN_TOKEN_CURLY_OPEN:
                statement_idx = cn_ast_parse_block(lexer, false);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->block.attribute_specifiers = attribute_specifiers;

                break;

            case CN_TOKEN_IF:
                statement_idx = cn_ast_parse_if_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->if_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_SWITCH:
                statement_idx = cn_ast_parse_switch_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->switch_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_WHILE:
                statement_idx = cn_ast_parse_while_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->while_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_DO:
                statement_idx = cn_ast_parse_do_while(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->do_while.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_FOR:
                statement_idx = cn_ast_parse_for_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->for_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_GOTO:
                statement_idx = cn_ast_parse_goto_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->goto_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_RETURN:
                statement_idx = cn_ast_parse_return_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->return_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_BREAK:
                statement_idx = cn_ast_parse_break_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->break_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            case CN_TOKEN_CONTINUE:
                statement_idx = cn_ast_parse_continue_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->continue_statement.attribute_specifiers = attribute_specifiers;
                
                break;

            default:
                statement_idx = cn_ast_parse_expression_statement(lexer);
                if (statement_idx == CN_AST_NIL_IDX) goto error;

                cn_ast_get_as_node(statement_idx)->expression_statement.attribute_specifiers = attribute_specifiers;
                
                break;
        }
    }

    cn_ast_list_set_parent(statement_idx, &attribute_specifiers);
    return statement_idx;

error:
    CN__TRACE_ERROR
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_block(Cn_Lexer *lexer, bool use_current_scope) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_BLOCK, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_OPEN)) goto error;

    // Stepping into scope.
    if (!use_current_scope) cn_ast_scope_stack_push();

    int64_t mark = cn_ast_idx_stack_mark();

    while (!cn_parse_optional(lexer, CN_TOKEN_CURLY_CLOSE)) {
        Cn_Ast_Idx block_item_idx = cn_ast_parse_block_item(lexer);
        if (block_item_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(block_item_idx);
    }

    node.block.block_items = cn_ast_idx_stack_finalize(mark);

    if (!use_current_scope) cn_ast_scope_stack_pop();

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.block.block_items);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_block_item(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_BLOCK_ITEM, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // Parsing attribute specifiers here in order to get to the tokens that would 
    // disambiguate delcaration vs statement situation.
    bool ok;
    Cn_Ast_List attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    Cn_Ast_Idx declaration_or_statement_idx;

    if (cn_ast_starts_declaration(lexer)) {
        declaration_or_statement_idx = cn_ast_continue_declaration(lexer, attribute_specifiers);
        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    } else {
        declaration_or_statement_idx = cn_ast_continue_statement(lexer, attribute_specifiers);
    }

    if (declaration_or_statement_idx == CN_AST_NIL_IDX) goto error;
    node.block_item.declaration_or_statement_idx = declaration_or_statement_idx;
    
    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.block_item.declaration_or_statement_idx);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_if_statement(Cn_Lexer *lexer) {
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

CNDEF Cn_Ast_Idx cn_ast_parse_switch_statement(Cn_Lexer *lexer) {
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
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_WHILE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_WHILE)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (expression_idx == CN_AST_NIL_IDX) goto error;
    node.while_statement.condition_idx = expression_idx;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Idx statement_idx = cn_ast_parse_statement(lexer);
    if (statement_idx == CN_AST_NIL_IDX) goto error;
    node.while_statement.body_idx = statement_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.while_statement.condition_idx, node.while_statement.body_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_do_while(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_DO_WHILE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_DO)) goto error;

    Cn_Ast_Idx statement_idx = cn_ast_parse_statement(lexer);
    if (statement_idx == CN_AST_NIL_IDX) goto error;
    node.do_while.body_idx = statement_idx;

    if (!cn_parse_expect(lexer, CN_TOKEN_WHILE)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
    if (expression_idx == CN_AST_NIL_IDX) goto error;
    node.do_while.condition_idx = expression_idx;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, node.do_while.condition_idx, node.do_while.body_idx);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_for_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_FOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_FOR)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    // For statement introduces new scope.
    // for its initialization declaration to live in the scope till for loop dies.
    cn_ast_scope_stack_push();
    
    if (!cn_parse_optional(lexer, CN_TOKEN_SEMICOLON)) {
        bool ok;
        Cn_Ast_List attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;

        Cn_Ast_Idx declaration_or_expression_idx;
        if (cn_ast_starts_declaration(lexer)) {
            declaration_or_expression_idx = cn_ast_continue_declaration(lexer, attribute_specifiers);
        } else {
            if (attribute_specifiers.length > 0) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, attribute_specifiers.idxs[0], CN_DC_ILLEGAL_ATTRIBUTE, "Attribute can't appear in here.");
                goto error;
            }

            declaration_or_expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        }

        if (declaration_or_expression_idx == CN_AST_NIL_IDX) goto error;
        node.for_statement.initialization_idx = declaration_or_expression_idx;

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    }

    if (!cn_parse_optional(lexer, CN_TOKEN_SEMICOLON)) {
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.for_statement.condition_idx = expression_idx;

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    }

    if (!cn_parse_optional(lexer, CN_TOKEN_PARAN_CLOSE)) {
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.for_statement.update_idx = expression_idx;

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    }

    Cn_Ast_Idx statement_idx = cn_ast_parse_statement(lexer);
    if (statement_idx == CN_AST_NIL_IDX) goto error;
    node.for_statement.body_idx = statement_idx;
    
    cn_ast_scope_stack_pop();

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.for_statement.initialization_idx, 
            node.for_statement.condition_idx,
            node.for_statement.update_idx,
            node.for_statement.body_idx
            );

    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_goto_statement(Cn_Lexer *lexer) {
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

CNDEF Cn_Ast_Idx cn_ast_parse_return_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_RETURN, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_RETURN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'return' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        node.return_statement.expression_idx = cn_ast_parse_expression(lexer, -1, 0);
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

CNDEF Cn_Ast_Idx cn_ast_parse_break_statement(Cn_Lexer *lexer) {
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

CNDEF Cn_Ast_Idx cn_ast_parse_continue_statement(Cn_Lexer *lexer) {
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
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    if (cn_parse_optional(lexer, CN_TOKEN_CASE)) {
        node.flags |= CN_AST_LABEL_IS_CASE;
        Cn_Ast_Idx expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (expression_idx == CN_AST_NIL_IDX) goto error;
        node.label.expression_idx = expression_idx;

    } else if (cn_parse_optional(lexer, CN_TOKEN_DEFAULT)) {
        node.flags |= CN_AST_LABEL_IS_DEFAULT;
    } else {
        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
        if (identifier_idx == CN_AST_NIL_IDX) goto error;
        node.label.identifier_idx = identifier_idx;

        // Binding new label.
        if (cn_ast_label_binding_declare(cn_ast_get_as_node(identifier_idx)->identifier.name, identifier_idx) == CN_AST_NIL_BINDING_IDX) 
            goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_COLON)) goto error;

    Cn_Ast_Idx statement_idx = cn_ast_parse_statement(lexer);
    if (statement_idx == CN_AST_NIL_IDX) goto error;
    node.label.statement_idx = statement_idx;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.label.identifier_idx,
            node.label.expression_idx,
            node.label.statement_idx,
            );
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_ast_parse_expression_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node node = { .kind = CN_AST_EXPRESSION_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_optional(lexer, CN_TOKEN_SEMICOLON)) {

        node.expression_statement.expression_idx = cn_ast_parse_expression(lexer, -1, 0);
        if (node.expression_statement.expression_idx == CN_AST_NIL_IDX) goto error;

        // Typechecking expresion.
        Cn_Type *type = cn_ast_expression_typecheck(node.expression_statement.expression_idx);
        if (type == NULL) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    }

    // TODO: Later replace with a better way to construct ast source lengths.
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
            if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
                node.kind = CN_AST_COMPOUND;
                
                node.compound.type_name_idx = type_name_idx;

                bool ok;
                node.compound.designations = cn_ast_parse_designations(lexer, &ok);
                if (!ok) goto error;

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

    bool ok;
    node.init_declarator.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_ASSIGN)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Idx initializer_idx = cn_ast_parse_initializer(lexer);
        if (initializer_idx == CN_AST_NIL_IDX) goto error;
        node.init_declarator.initializer_idx = initializer_idx;
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.init_declarator.declarator_idx,
            node.init_declarator.gnu_asm_label_idx,
            node.init_declarator.initializer_idx,
            );
    cn_ast_list_set_parent(parent, &node.init_declarator.gnu_attribute_specifiers);
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

    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        bool ok;
        node.initializer.designations = cn_ast_parse_designations(lexer, &ok);   
        if (!ok) goto error;
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

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_OPEN)) goto error;

    int64_t mark = cn_ast_idx_stack_mark();
    
    while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
        Cn_Ast_Idx designation_idx = cn_ast_parse_designation(lexer);
        if (designation_idx == CN_AST_NIL_IDX) goto error;
        cn_ast_idx_stack_push(designation_idx);

        if (!cn_parse_optional(lexer, CN_TOKEN_COMMA)) break;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE)) goto error;

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
        if (direct_declarator_idx == CN_AST_NIL_IDX) goto error;
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
            cn_ast_node_set_parent(direct_declarator_idx,
                    node.direct_declarator_array.direct_declarator_idx,
                    node.direct_declarator_array.expression_idx
                    );
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

    // Parse attribute and gnu attribute specifiers.
    bool ok;

    if (node.kind == CN_AST_STRUCT_SPECIFIER) {
        node.struct_specifier.attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;
        node.struct_specifier.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;
    } else {
        node.union_specifier.attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;
        node.union_specifier.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;
    }
    
    // Parse tag identifier.
    if (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer); 
        if (identifier_idx == CN_AST_NIL_IDX) goto error;
        node.kind == CN_AST_STRUCT_SPECIFIER ? (node.struct_specifier.identifier_idx = identifier_idx) : (node.union_specifier.identifier_idx = identifier_idx);
    } else {
        // Generating unique tag name for anonymous struct or union.
        Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);

        if (node.kind == CN_AST_STRUCT_SPECIFIER)
            cn_sb_append_format(&sb, "cn__struct_%lu", cn_ast_counter_next());
        else 
            cn_sb_append_format(&sb, "cn__union_%lu", cn_ast_counter_next());

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

        node.kind == CN_AST_STRUCT_SPECIFIER ? 
            (node.struct_specifier.flags |= CN_AST_STRUCT_HAS_DEFINITION) : 
            (node.union_specifier.flags |= CN_AST_UNION_HAS_DEFINITION);
    }

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    if (node.kind == CN_AST_STRUCT_SPECIFIER) {
        cn_ast_list_set_parent(parent, &node.struct_specifier.attribute_specifiers);
        cn_ast_list_set_parent(parent, &node.struct_specifier.gnu_attribute_specifiers);
        cn_ast_node_set_parent(parent, node.struct_specifier.identifier_idx);
        cn_ast_list_set_parent(parent, &node.struct_specifier.member_declarations);
    } else {
        cn_ast_list_set_parent(parent, &node.union_specifier.attribute_specifiers);
        cn_ast_list_set_parent(parent, &node.union_specifier.gnu_attribute_specifiers);
        cn_ast_node_set_parent(parent, node.union_specifier.identifier_idx);
        cn_ast_list_set_parent(parent, &node.union_specifier.member_declarations);
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

    bool ok;

    node.member_declaration.attribute_specifiers = cn_ast_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
    if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
    node.member_declaration.specifier_qualifier_idx = specifier_qualifier_idx;

    node.member_declaration.member_declarators = cn_ast_parse_member_declarators(lexer, &ok);
    if (!ok) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_list_set_parent(parent, &node.member_declaration.attribute_specifiers);
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

    bool ok;
    node.member_declarator.gnu_attribute_specifiers = cn_ast_parse_gnu_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    Cn_Ast_Idx parent = cn_ast_node_list_append(node);
    cn_ast_node_set_parent(parent, 
            node.member_declarator.declarator_idx,
            node.member_declarator.bitfield_idx
            );
    cn_ast_list_set_parent(parent, &node.member_declarator.gnu_attribute_specifiers);
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

    bool is_enum_definition_optional;
    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        is_enum_definition_optional = true;

        Cn_Ast_Idx identifier_idx = cn_ast_parse_identifier(lexer);
        if (identifier_idx == CN_AST_NIL_IDX) goto error;
        node.enum_specifier.identifier_idx = identifier_idx;
    } else {
         is_enum_definition_optional = false;

         // Generating unique tag name for anonymous enum.
         Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);

         cn_sb_append_format(&sb, "cn__enum_%lu", cn_ast_counter_next());

         Cn_String tag = cn_sb_to_str(&sb);

         void *data = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, tag.length);
         cn_str_copy_to(tag, data);
         tag.data = data;

         Cn_Ast_Idx identifier_idx = cn_ast_node_list_append((Cn_Ast_Node) {
                 .identifier.kind = CN_AST_IDENTIFIER,
                 .identifier.loc = cn_lexer_token(lexer).loc,
                 .identifier.name = tag,
                 });

         node.enum_specifier.identifier_idx = identifier_idx;

         cn_sb_free(&sb);
    }

    if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
        Cn_Ast_Idx specifier_qualifier_idx = cn_ast_parse_specifier_qualifier(lexer);
        if (specifier_qualifier_idx == CN_AST_NIL_IDX) goto error;
        node.enum_specifier.specifier_qualifier_idx = specifier_qualifier_idx;
    }


    if (cn_parse_optional(lexer, CN_TOKEN_CURLY_OPEN)) {
        node.flags |= CN_AST_ENUM_HAS_DEFINITION;

        int64_t mark = cn_ast_idx_stack_mark();

        Cn_Ast_Idx enumerator_idx;
        while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            enumerator_idx = cn_ast_parse_enumerator(lexer);
            if (enumerator_idx == CN_AST_NIL_IDX) goto error;
            cn_ast_idx_stack_push(enumerator_idx);

            if (!cn_parse_optional(lexer, CN_TOKEN_COMMA)) break;
        }

        node.enum_specifier.enumerators = cn_ast_idx_stack_finalize(mark);
        
        if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE)) goto error;
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
 * Declares/completes the enum tag and builds its type: binding every enum constant 
 * that appears in enum defintion as a member.
 *
 * RETURNS: The (possibly newly completed) enum type, or NULL on error.
 */
CNDEF Cn_Type *cn__ast_type_from_enum(Cn_Ast_Idx ts_idx) {
    Cn_Ast_Enum_Specifier *enum_spec = cn_ast_get(ts_idx);
    Cn_String tag = cn_ast_get_as_node(enum_spec->identifier_idx)->identifier.name;

    Cn_Ast_Binding_Idx binding_idx = cn_ast_tag_binding_declare(tag, CN_ENUM);
    if (binding_idx == CN_AST_NIL_BINDING_IDX) goto error;
 
    Cn_Ast_Binding *binding = cn_ast_binding_get(binding_idx);
 
    if (enum_spec->flags & CN_AST_ENUM_HAS_DEFINITION) {
        if ((binding->type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_AST_SCOPE_STACK_CURRENT_IDX) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts_idx, CN_DC_REDEFINITION, "Redefinition of 'enum %.*s' is not allowed within the same scope.", CN_UNPACK(tag));
            goto error;
        }
 
        Cn_Type_Enum *enum_type = (Cn_Type_Enum *)binding->type;
 
        enum_type->members_length = enum_spec->enumerators.length;
        enum_type->members = cn_chained_arena_alloc(&cn__ast_data->type_children_arena, enum_type->members_length * sizeof(Cn_Type_Enum_Member));
 
        Cn_Type *enum_base_type;
        if (enum_spec->specifier_qualifier_idx != CN_AST_NIL_IDX) {
            Cn_Ast_Specifier_Qualifier *sq = cn_ast_get(enum_spec->specifier_qualifier_idx);
            enum_base_type = cn_ast_to_type(sq->qualifiers, sq->type_specifier_idx, CN_AST_NIL_IDX);

            if (enum_base_type == NULL) goto error;

            if (enum_base_type->kind != CN_INTEGER && enum_base_type->kind != CN_OPAQUE) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts_idx, CN_DC_ILLEGAL_TYPE, "Enum base type cannot be non-integer type.");
                goto error;
            }
        } else {
            enum_base_type = cn__ast_add_type_if_not((Cn_Type *)&CN_TYPE_INT);
        }
        enum_type->member_type = enum_base_type;

        enum_type->align = enum_base_type->align;
        enum_type->size = enum_base_type->size;

        int64_t enum_counter = 0;
        for (int64_t e = 0; e < enum_spec->enumerators.length; e++) {
            Cn_Ast_Enumerator *enumerator = cn_ast_get(enum_spec->enumerators.idxs[e]);

            // Reset counter, by evaluating expression_idx...
            if (enumerator->expression_idx != CN_AST_NIL_IDX) {
                uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
                
                Cn_Type *type = cn_ast_expression_typecheck(enumerator->expression_idx);
                if (type == NULL) goto error;

                if (type->kind != CN_INTEGER) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, enumerator->expression_idx, CN_DC_INVALID_CONSTANT_EXPRESSION, "Expected integer result from constant expression in enumerator.");
                    goto error;
                } 

                Cn_Any result = cn_ast_expression_evaluate(enumerator->expression_idx, buffer);

                enum_counter = cn_any_read_int(result);
            }

            Cn_String enumerator_name = cn_ast_get_as_node(enumerator->identifier_idx)->identifier.name;

            Cn_Ast_Binding_Idx enum_constant_idx = cn_ast_enum_constant_binding_declare(enumerator_name, enum_spec->enumerators.idxs[e], enum_base_type, enum_counter);
            if (enum_constant_idx == CN_AST_NIL_BINDING_IDX) goto error;

            // Use name allocated from permanent arena cause binding saves name in temporary arena.
            enum_type->members[e].name = cn__ast_permanent_save_string(enumerator_name);
            enum_type->members[e].value = enum_counter;

            enum_counter++;
        }
        
        binding->type->flags |= CN_TYPE_COMPLETE;
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
            result = cn__ast_type_from_enum(type_specifier_idx);
            break;
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
                if (left->kind != CN_POINTER && left->kind != CN_ARRAY) {
                    cn__ast_illegal_binary(node, left, right, "array subscript (left operand must be a pointer or array)");
                    return NULL;
                }
                if (right->kind != CN_INTEGER) {
                    cn__ast_illegal_binary(node, left, right, "array subscript (subscript must be an integer)");
                    return NULL;
                }

                return left->kind == CN_POINTER ? left->pointer.ptr_to : left->array.element_type;
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
                Cn_Ast_Binding_Idx idx = cn_ast_binding_table_get(literal->identifier.name, &cn__ast_data->symbol_binding_table);
                if (idx == CN_AST_NIL_BINDING_IDX) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_idx_get(literal), CN_DC_INVALID_SYMBOL, "Primary expression identifier is not a known symbol.");
                    return (Cn_Any) {0};
                }

                Cn_Ast_Binding *binding = cn_ast_binding_get(idx);
                if (binding->kind != CN_BINDING_ENUM_CONSTANT) {
                    return (Cn_Any) {0};
                }

                any = cn_any_write_int(any, binding->b_enum_constant.value);
                break;
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

/**
 * ============================================
 * IMPLEMENTATION SECTION: Diagnostics
 * ============================================
 */

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
    [CN_DC_ILLEGAL_ATTRIBUTE]               = CN_STR_BUFFER("Illegal attribute"),
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
    
    cn_log(CN_ERROR, "Ast based diagnostics are now implemented differently, diagnostic that was called doesn't have source.");
    exit(1);
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Pre-Processing
 * ============================================
 */

Cn_Message_Handler *cn_message_handler = NULL;

CNDEF Cn_Result cn__send_message(Cn_Message_Kind kind, Cn_Message message) {
    if (cn_message_handler == NULL) return false;

    return cn_message_handler(kind, &message);
}

CNDEF void cn_log_types() {
    if (CN_INFO >= cn_log_min_level) {
        cn_log(CN_INFO, "Type universe:" CN_ANSI_BLUE);

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
    if (CN_INFO >= cn_log_min_level) {
        // First binding is NIL, so skip index 0.
        cn_log(CN_INFO, "Bindings:" CN_ANSI_BRIGHT_YELLOW);
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
        cn_log(CN_INFO, "Received:\n" CN_ANSI_BRIGHT_BLACK "%.*s" CN_ANSI_RESET, CN_UNPACK(tu->content));
    }

    // Setting up lexer.
    Cn_Lexer lexer = {0};

    // Printing tokens.
    if (flags & CN_PRINT_TOKENS) {
        cn_lexer_init(&lexer, tu->content, cn_default_blacklist);
        cn_log(CN_INFO, "Tokenized:" CN_ANSI_CYAN);
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
        cn_log(CN_INFO, "Parsed:");
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
        Cn_Emitter emitter = { .write = &cn_emit_write_file, .ctx = out };
        cn_emit(&emitter, idx);
        fclose(out);
    }

    return 0;
}

CNDEF void cn_tu_free(Cn_Translation_Unit *tu) {
    cn_ast_free(&tu->ast_data);

    if (!tu->no_malloc) CN_FREE(tu->content.data);
}

CNDEF void cn_replace(Cn_Ast_Idx *original, Cn_Ast_Idx new) {
    CN_ASSERT(new != CN_AST_NIL_IDX);

    Cn_Ast_Node *node = cn_ast_get(new);
    *original = new;
}

CNDEF void cn_replace_list(Cn_Ast_List *original, Cn_Ast_List new) {
    Cn_Ast_Node *node;
    for (int64_t i = 0; i < new.length; i++) {
        node = cn_ast_get(new.idxs[i]);
    }
    *original = new;
}

CNDEF Cn_Ast_Idx cn_remove_from_list(Cn_Ast_List *list, int64_t index) {
    Cn_Ast_Idx removed = list->idxs[index];
    memmove(list->idxs + index, list->idxs + index + 1, list->length - index - 1);
    list->length--;
    return removed;
}

CNDEF Cn_Ast_List cn_copy_list(Cn_Ast_List list) {
    int64_t mark = cn_ast_idx_stack_mark();

    for (int64_t i = 0; i < list.length; i++) {
        cn_ast_idx_stack_push(cn_copy(list.idxs[i]));
    }

    return cn_ast_idx_stack_finalize(mark);
}

CNDEF Cn_Ast_Idx cn_copy(Cn_Ast_Idx idx) {
    if (idx == CN_AST_NIL_IDX) return CN_AST_NIL_IDX;

    Cn_Ast_Idx copy_idx = cn_ast_node_list_append(*cn_ast_get_as_node(idx));

    cn_ast_get_as_node(copy_idx)->flags |= CN_AST_SYNTHETIC;
    cn_ast_get_as_node(copy_idx)->parent_idx = CN_AST_NIL_IDX;

    // NOTE: Copy of the child is made into a temporary first, because copying
    // can grow the node list and invalidate any pointer into it.
#define COPY_LIST(member)                                                       \
    do {                                                                        \
        Cn_Ast_List cn__l_ = cn_copy_list(cn_ast_get_as_node(copy_idx)->member);\
        cn_ast_get_as_node(copy_idx)->member = cn__l_;                          \
    } while (0)

#define COPY_IDX(member)                                                        \
    do {                                                                        \
        Cn_Ast_Idx cn__i_ = cn_copy(cn_ast_get_as_node(copy_idx)->member);      \
        cn_ast_get_as_node(copy_idx)->member = cn__i_;                          \
    } while (0)

    switch (cn_ast_get_as_node(copy_idx)->kind) {
        case CN_AST_TRANSLATION_UNIT:
            COPY_LIST(translation_unit.external_declarations);
            return copy_idx;
        case CN_AST_EXTERNAL_DECLARATION:
            COPY_IDX(external_declaration.child_idx);
            return copy_idx;
        case CN_AST_DECLARATION:
            COPY_LIST(declaration.attribute_specifiers);
            COPY_IDX(declaration.declaration_specifiers_idx);
            COPY_LIST(declaration.init_declarators);
            return copy_idx;
        case CN_AST_FUNCTION:
            COPY_LIST(function.attribute_specifiers);
            COPY_IDX(function.declaration_specifiers_idx);
            COPY_IDX(function.declarator_idx);
            COPY_IDX(function.block_idx);
            return copy_idx;
        case CN_AST_BLOCK:
            COPY_LIST(block.attribute_specifiers);
            COPY_LIST(block.block_items);
            return copy_idx;
        case CN_AST_BLOCK_ITEM:
            COPY_IDX(block_item.declaration_or_statement_idx);
            return copy_idx;
        case CN_AST_IF:
            COPY_LIST(if_statement.attribute_specifiers);
            COPY_IDX(if_statement.condition_idx);
            COPY_IDX(if_statement.then_idx);
            COPY_IDX(if_statement.else_idx);
            return copy_idx;
        case CN_AST_SWITCH:
            COPY_LIST(switch_statement.attribute_specifiers);
            COPY_IDX(switch_statement.condition_idx);
            COPY_IDX(switch_statement.body_idx);
            return copy_idx;
        case CN_AST_WHILE:
            COPY_LIST(while_statement.attribute_specifiers);
            COPY_IDX(while_statement.condition_idx);
            COPY_IDX(while_statement.body_idx);
            return copy_idx;
        case CN_AST_DO_WHILE:
            COPY_LIST(do_while.attribute_specifiers);
            COPY_IDX(do_while.condition_idx);
            COPY_IDX(do_while.body_idx);
            return copy_idx;
        case CN_AST_FOR:
            COPY_LIST(for_statement.attribute_specifiers);
            COPY_IDX(for_statement.initialization_idx);
            COPY_IDX(for_statement.condition_idx);
            COPY_IDX(for_statement.update_idx);
            COPY_IDX(for_statement.body_idx);
            return copy_idx;
        case CN_AST_LABEL:
            COPY_LIST(label.attribute_specifiers);
            COPY_IDX(label.identifier_idx);
            COPY_IDX(label.expression_idx);
            COPY_IDX(label.statement_idx);
            return copy_idx;
        case CN_AST_GOTO:
            COPY_LIST(goto_statement.attribute_specifiers);
            COPY_IDX(goto_statement.identifier_idx);
            return copy_idx;
        case CN_AST_RETURN:
            COPY_LIST(return_statement.attribute_specifiers);
            COPY_IDX(return_statement.expression_idx);
            return copy_idx;
        case CN_AST_BREAK:
            COPY_LIST(break_statement.attribute_specifiers);
            return copy_idx;
        case CN_AST_CONTINUE:
            COPY_LIST(continue_statement.attribute_specifiers);
            return copy_idx;
        case CN_AST_EXPRESSION_STATEMENT:
            COPY_LIST(expression_statement.attribute_specifiers);
            COPY_IDX(expression_statement.expression_idx);
            return copy_idx;
        case CN_AST_BINARY:
            COPY_IDX(binary.left_idx);
            COPY_IDX(binary.right_idx);
            return copy_idx;
        case CN_AST_ACCESS:
            COPY_IDX(access.expression_idx);
            COPY_IDX(access.member_idx);
            return copy_idx;
        case CN_AST_CALL:
            COPY_IDX(call.expression_idx);
            COPY_LIST(call.arguments);
            return copy_idx;
        case CN_AST_UNARY:
            COPY_IDX(unary.expression_idx);
            return copy_idx;
        case CN_AST_CAST:
            COPY_IDX(cast.type_name_idx);
            COPY_IDX(cast.expression_idx);
            return copy_idx;
        case CN_AST_COMPOUND:
            COPY_IDX(compound.type_name_idx);
            COPY_LIST(compound.designations);
            return copy_idx;
        case CN_AST_SIZEOF:
            COPY_IDX(sizeof_expression.target_idx);
            return copy_idx;
        case CN_AST_TERNARY:
            COPY_IDX(ternary.condition_idx);
            COPY_IDX(ternary.true_idx);
            COPY_IDX(ternary.false_idx);
            return copy_idx;
        case CN_AST_ASSIGN:
            COPY_IDX(assign.left_idx);
            COPY_IDX(assign.right_idx);
            return copy_idx;
        case CN_AST_POSTFIX:
            COPY_IDX(postfix.expression_idx);
            return copy_idx;
        case CN_AST_PRIMARY:
            COPY_IDX(primary.literal_idx);
            return copy_idx;
        case CN_AST_IDENTIFIER:
            return copy_idx;
        case CN_AST_INTEGER:
            return copy_idx;
        case CN_AST_FLOAT:
            return copy_idx;
        case CN_AST_STRING:
            return copy_idx;
        case CN_AST_INIT_DECLARATOR:
            COPY_IDX(init_declarator.declarator_idx);
            COPY_LIST(init_declarator.gnu_attribute_specifiers);
            COPY_IDX(init_declarator.gnu_asm_label_idx);
            COPY_IDX(init_declarator.initializer_idx);
            return copy_idx;
        case CN_AST_INITIALIZER:
            COPY_IDX(initializer.expression_idx);
            COPY_LIST(initializer.designations);
            return copy_idx;
        case CN_AST_DESIGNATION:
            COPY_LIST(designation.designators);
            COPY_IDX(designation.initializer_idx);
            return copy_idx;
        case CN_AST_DESIGNATOR:
            COPY_IDX(designator.identifier_idx);
            COPY_IDX(designator.expression_idx);
            COPY_IDX(designator.expression_range_end_idx);
            return copy_idx;
        case CN_AST_DECLARATOR:
            COPY_IDX(declarator.pointer_idx);
            COPY_IDX(declarator.direct_declarator_idx);
            return copy_idx;
        case CN_AST_POINTER:
            COPY_IDX(pointer.pointer_idx);
            return copy_idx;
        case CN_AST_DIRECT_DECLARATOR_GROUPED:
            COPY_IDX(direct_declarator_grouped.declarator_idx);
            return copy_idx;
        case CN_AST_DIRECT_DECLARATOR_ARRAY:
            COPY_IDX(direct_declarator_array.direct_declarator_idx);
            COPY_IDX(direct_declarator_array.expression_idx);
            return copy_idx;
        case CN_AST_DIRECT_DECLARATOR_FUNCTION:
            COPY_IDX(direct_declarator_function.direct_declarator_idx);
            COPY_LIST(direct_declarator_function.parameter_declarations);
            return copy_idx;
        case CN_AST_DECLARATION_SPECIFIERS:
            COPY_LIST(declaration_specifiers.gnu_attribute_specifiers);
            COPY_IDX(declaration_specifiers.type_specifier_idx);
            return copy_idx;
        case CN_AST_GNU_TYPEOF:
            COPY_IDX(gnu_typeof.target_idx);
            return copy_idx;
        case CN_AST_TYPE_SPECIFIER_PRIMITIVE:
            return copy_idx;
        case CN_AST_TYPE_SPECIFIER_TYPEDEF:
            return copy_idx;
        case CN_AST_TYPE_NAME:
            COPY_IDX(type_name.specifier_qualifier_idx);
            COPY_IDX(type_name.abstract_declarator_idx);
            return copy_idx;
        case CN_AST_SPECIFIER_QUALIFIER:
            COPY_IDX(specifier_qualifier.type_specifier_idx);
            return copy_idx;
        case CN_AST_PARAMETER_DECLARATION:
            COPY_IDX(parameter_declaration.declaration_specifiers_idx);
            COPY_IDX(parameter_declaration.declarator_idx);
            return copy_idx;
        case CN_AST_STRUCT_SPECIFIER:
            COPY_LIST(struct_specifier.attribute_specifiers);
            COPY_LIST(struct_specifier.gnu_attribute_specifiers);
            COPY_IDX(struct_specifier.identifier_idx);
            COPY_LIST(struct_specifier.member_declarations);
            return copy_idx;
        case CN_AST_UNION_SPECIFIER:
            COPY_LIST(union_specifier.attribute_specifiers);
            COPY_LIST(union_specifier.gnu_attribute_specifiers);
            COPY_IDX(union_specifier.identifier_idx);
            COPY_LIST(union_specifier.member_declarations);
            return copy_idx;
        case CN_AST_MEMBER_DECLARATION:
            COPY_LIST(member_declaration.attribute_specifiers);
            COPY_IDX(member_declaration.specifier_qualifier_idx);
            COPY_LIST(member_declaration.member_declarators);
            return copy_idx;
        case CN_AST_MEMBER_DECLARATOR:
            COPY_IDX(member_declarator.declarator_idx);
            COPY_IDX(member_declarator.bitfield_idx);
            COPY_LIST(member_declarator.gnu_attribute_specifiers);
            return copy_idx;
        case CN_AST_ENUM_SPECIFIER:
            COPY_LIST(enum_specifier.attribute_specifiers);
            COPY_LIST(enum_specifier.gnu_attribute_specifiers);
            COPY_IDX(enum_specifier.identifier_idx);
            COPY_IDX(enum_specifier.specifier_qualifier_idx);
            COPY_LIST(enum_specifier.enumerators);
            return copy_idx;
        case CN_AST_ENUMERATOR:
            COPY_IDX(enumerator.identifier_idx);
            COPY_LIST(enumerator.attribute_specifiers);
            COPY_LIST(enumerator.gnu_attribute_specifiers);
            COPY_IDX(enumerator.expression_idx);
            return copy_idx;
        case CN_AST_ATTRIBUTE_SPECIFIER:
            COPY_LIST(attribute_specifier.attributes);
            return copy_idx;
        case CN_AST_ATTRIBUTE:
            COPY_IDX(attribute.vendor_identifier_idx);
            COPY_IDX(attribute.identifier_idx);
            COPY_LIST(attribute.arguments);
            return copy_idx;
        case CN_AST_GNU_ATTRIBUTE_SPECIFIER:
            COPY_LIST(gnu_attribute_specifier.gnu_attributes);
            return copy_idx;
        case CN_AST_GNU_ATTRIBUTE:
            COPY_IDX(gnu_attribute.identifier_idx);
            COPY_LIST(gnu_attribute.arguments);
            return copy_idx;
        case CN_AST_GNU_ASM_LABEL:
            COPY_IDX(gnu_asm_label.string_idx);
            return copy_idx;
        case CN_AST_UNKNOWN:
        case CN_AST_ERROR:
        case CN_AST_CODE:
            cn_log(CN_ERROR, "Unsupported copy ast node kind.");
            return copy_idx;
    }

#undef COPY_LIST
#undef COPY_IDX

    return CN_AST_NIL_IDX;
}

CNDEF Cn_Ast_Idx cn_get_attribute(Cn_Ast_Idx node, Cn_String attribute_name) {
    if (node == CN_AST_NIL_IDX) return CN_AST_NIL_IDX;

    Cn_Ast_List attribute_specifiers;

    switch (cn_ast_get_as_node(node)->kind) {
        case CN_AST_DECLARATION:
            attribute_specifiers = cn_ast_get_as_node(node)->declaration.attribute_specifiers;
            break;
        case CN_AST_FUNCTION:
            attribute_specifiers = cn_ast_get_as_node(node)->function.attribute_specifiers;
            break;
        case CN_AST_BLOCK:
            attribute_specifiers = cn_ast_get_as_node(node)->block.attribute_specifiers;
            break;
        case CN_AST_IF:
            attribute_specifiers = cn_ast_get_as_node(node)->if_statement.attribute_specifiers;
            break;
        case CN_AST_SWITCH:
            attribute_specifiers = cn_ast_get_as_node(node)->switch_statement.attribute_specifiers;
            break;
        case CN_AST_WHILE:
            attribute_specifiers = cn_ast_get_as_node(node)->while_statement.attribute_specifiers;
            break;
        case CN_AST_DO_WHILE:
            attribute_specifiers = cn_ast_get_as_node(node)->do_while.attribute_specifiers;
            break;
        case CN_AST_FOR:
            attribute_specifiers = cn_ast_get_as_node(node)->for_statement.attribute_specifiers;
            break;
        case CN_AST_LABEL:
            attribute_specifiers = cn_ast_get_as_node(node)->label.attribute_specifiers;
            break;
        case CN_AST_GOTO:
            attribute_specifiers = cn_ast_get_as_node(node)->goto_statement.attribute_specifiers;
            break;
        case CN_AST_RETURN:
            attribute_specifiers = cn_ast_get_as_node(node)->return_statement.attribute_specifiers;
            break;
        case CN_AST_BREAK:
            attribute_specifiers = cn_ast_get_as_node(node)->break_statement.attribute_specifiers;
            break;
        case CN_AST_CONTINUE:
            attribute_specifiers = cn_ast_get_as_node(node)->continue_statement.attribute_specifiers;
            break;
        case CN_AST_EXPRESSION_STATEMENT:
            attribute_specifiers = cn_ast_get_as_node(node)->expression_statement.attribute_specifiers;
            break;
        // case CN_AST_PARAMETER_DECLARATION:
        case CN_AST_STRUCT_SPECIFIER:
            attribute_specifiers = cn_ast_get_as_node(node)->struct_specifier.attribute_specifiers;
            break;
        case CN_AST_UNION_SPECIFIER:
            attribute_specifiers = cn_ast_get_as_node(node)->union_specifier.attribute_specifiers;
            break;
        case CN_AST_MEMBER_DECLARATION:
            attribute_specifiers = cn_ast_get_as_node(node)->member_declaration.attribute_specifiers;
            break;
        case CN_AST_ENUM_SPECIFIER:
            attribute_specifiers = cn_ast_get_as_node(node)->enum_specifier.attribute_specifiers;
            break;
        case CN_AST_ENUMERATOR:
            attribute_specifiers = cn_ast_get_as_node(node)->enumerator.attribute_specifiers;
            break;
            
        case CN_AST_ATTRIBUTE_SPECIFIER: 
            {
                Cn_Ast_Attribute_Specifier *attribute_specifier = cn_ast_get(node);
                for (int64_t j = 0; j < attribute_specifier->attributes.length; j++) {
                    Cn_Ast_Attribute *attribute = cn_ast_get(attribute_specifier->attributes.idxs[j]);
                    if (cn_str_equals(&cn_ast_get_as_node(attribute->identifier_idx)->identifier.name, &attribute_name)) {
                        return attribute_specifier->attributes.idxs[j];
                    }
                }

                return CN_AST_NIL_IDX;
            }
        default:
            return CN_AST_NIL_IDX;
    }

    for (int64_t i = 0; i < attribute_specifiers.length; i++) {
        Cn_Ast_Attribute_Specifier *attribute_specifier = cn_ast_get(attribute_specifiers.idxs[i]);
        for (int64_t j = 0; j < attribute_specifier->attributes.length; j++) {
            Cn_Ast_Attribute *attribute = cn_ast_get(attribute_specifier->attributes.idxs[j]);
            if (cn_str_equals(&cn_ast_get_as_node(attribute->identifier_idx)->identifier.name, &attribute_name)) {
                return attribute_specifier->attributes.idxs[j];
            }
        }
    }

    return CN_AST_NIL_IDX;
}

CNDEF void cn_remove_attribute(Cn_Ast_Idx attribute_idx) {
    Cn_Ast_Attribute *attribute = cn_ast_get(attribute_idx);
    Cn_Ast_Attribute_Specifier *attribute_specifier = cn_ast_get(attribute->parent_idx);
    
    int64_t index = -1;
    for (int64_t i = 0; i < attribute_specifier->attributes.length; i++) {
        if (attribute_specifier->attributes.idxs[i] == attribute_idx) {
            index = i;
            break;
        }
    }

    CN_ASSERT(index != -1);

    cn_remove_from_list(&attribute_specifier->attributes, index);
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

CNDEF Cn_Ast_Idx cn__build_format(const char *file, int64_t line, const char *format, ...) {
    Cn_Ast_Node node = {
        .kind = CN_AST_CODE,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, file, line),
    };

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        cn_log(CN_ERROR, "Couldn't figure out needed length of build format string.");
        va_end(args);
        return CN_AST_NIL_IDX;
    }

    void *data = cn_chained_arena_alloc(&cn__ast_data->permanent_strings_arena, needed + 1);
    vsnprintf(data, needed + 1, format, args);
    va_end(args);

    node.code.text.data = data;
    node.code.text.length = needed;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_List cn__build_list(Cn_Ast_Idx idxs[], int64_t length) {
    int64_t mark = cn_ast_idx_stack_mark();

    for (int64_t i = 0; i < length; i++) {
        cn_ast_idx_stack_push(idxs[i]);
    }

    return cn_ast_idx_stack_finalize(mark);
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

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_assign(Cn_Assignment_Operator_Kind op, Cn_Ast_Idx left, Cn_Ast_Idx right, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_ASSIGN,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.assign.operator = op;
    node.assign.left_idx = cn__build_wrap_if_primary(left);
    node.assign.right_idx = cn__build_wrap_if_primary(right);

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_unary(Cn_Unary_Operator_Kind op, Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_UNARY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.unary.operator = op;
    node.unary.expression_idx = cn__build_wrap_if_primary(expression);

    return cn_ast_node_list_append(node);
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

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_expr_statement(Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_EXPRESSION_STATEMENT,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.expression_statement.expression_idx = cn__build_wrap_if_primary(expression);

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_goto_statement(Cn_Ast_Idx identifier, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_GOTO,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.goto_statement.identifier_idx = identifier;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_return_statement(Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_RETURN,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.return_statement.expression_idx = cn__build_wrap_if_primary(expression);

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_label(Cn_Ast_Idx identifier, Cn_Ast_Idx statement, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_LABEL,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.label.identifier_idx = identifier;
    node.label.statement_idx = statement;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_block_item(Cn_Ast_Idx declaration_or_statement, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_BLOCK_ITEM,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.block_item.declaration_or_statement_idx = declaration_or_statement;

    return cn_ast_node_list_append(node);
}

CNDEF bool cn__build_is_abstract(Cn_Ast_Idx direct_declarator_idx) {
    while (direct_declarator_idx != CN_AST_NIL_IDX) {
        Cn_Ast_Node *node = cn_ast_get(direct_declarator_idx);

        switch (node->kind) {
            case CN_AST_IDENTIFIER:
                return false;
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                if (node->direct_declarator_grouped.declarator_idx == CN_AST_NIL_IDX) return true;
                return (cn_ast_get_as_node(node->direct_declarator_grouped.declarator_idx)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) != 0;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                direct_declarator_idx = node->direct_declarator_array.direct_declarator_idx;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                direct_declarator_idx = node->direct_declarator_function.direct_declarator_idx;
                continue;
            default:
                return true;
        }
    }

    return true;
}

CNDEF Cn_Ast_Idx cn__build_pointer(Cn_Qualifier_Flags qualifiers, Cn_Ast_Idx pointer, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_POINTER,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.pointer.qualifiers = qualifiers;
    node.pointer.pointer_idx = pointer;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_direct_declarator_grouped(Cn_Ast_Idx declarator, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_DIRECT_DECLARATOR_GROUPED,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.direct_declarator_grouped.declarator_idx = declarator;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_direct_declarator_array(Cn_Ast_Idx direct_declarator, Cn_Ast_Idx expression, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_DIRECT_DECLARATOR_ARRAY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.direct_declarator_array.direct_declarator_idx = direct_declarator;

    if (expression != CN_AST_NIL_IDX) {
        node.direct_declarator_array.expression_idx = cn__build_wrap_if_primary(expression);
    }

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_direct_declarator_function(Cn_Ast_Idx direct_declarator, Cn_Ast_List parameter_declarations, bool variadic, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_DIRECT_DECLARATOR_FUNCTION,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (variadic) node.flags |= CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC;

    node.direct_declarator_function.direct_declarator_idx = direct_declarator;
    node.direct_declarator_function.parameter_declarations = parameter_declarations;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_declarator(Cn_Ast_Idx pointer, Cn_Ast_Idx direct_declarator, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_DECLARATOR,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (cn__build_is_abstract(direct_declarator)) node.flags |= CN_AST_DECLARATOR_IS_ABSTRACT;

    node.declarator.pointer_idx = pointer;
    node.declarator.direct_declarator_idx = direct_declarator;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_init_declarator(Cn_Ast_Idx declarator, Cn_Ast_Idx initializer, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_INIT_DECLARATOR,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.init_declarator.declarator_idx = declarator;
    node.init_declarator.initializer_idx = initializer;

    return cn_ast_node_list_append(node);
}

CNDEF Cn_Ast_Idx cn__build_declaration(Cn_Ast_Idx declaration_specifiers, Cn_Ast_List init_declarators, Cn_Build_Opt opt) {
    Cn_Ast_Node node = {
        .kind = CN_AST_DECLARATION,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (init_declarators.length == 0) node.flags |= CN_AST_DECLARATION_IS_EMPTY;

    node.declaration.declaration_specifiers_idx = declaration_specifiers;
    node.declaration.init_declarators = init_declarators;

    return cn_ast_node_list_append(node);
}



#endif // CN_IMPLEMENTATION


/* 
    Revision history:
        
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
