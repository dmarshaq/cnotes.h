/*  cnotes - v0.3.0 - MIT License - https://github.com/dmarshaq/cnotes.h

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
      - CN_GNU                              Enables GNU support.

    ## Redefinable Macros

        Redefine default behaviors of nob.h.

      - CNDEF                               Appends additional things to function declarations.
      - CN_ASSERT(condition)                Redefine which assert() cnotes.h shall use.

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
        
        Below are prefixes that appear after initial `cn_` or `cn__` prefix that denote which section the item is coming from.
        
        `log_`                          Responsible for libraries logging interface, with separate log handler idea from nob.h.
        `hash_`                         Used for various hashing operations by the library.
        `str_`                          Interface for Cn_String operations.
        `sb_`                           Interface for Cn_String_Builder operations.
        `arena_`                        Interface for Cn_Arena operations.
        `chained_arena_`                Interface for Cn_Chained_Arena operations.
        `pool_`                         Interface for Cn_Pool operations.
        `array_list_`                   Interface to operate on dynamically allocated array list.
        `hash_table_`                   Interface to operate on dynamically allocated hash table.
        `hash_set_`                     Interface to operate on dynamically allocated hash set.
        `source_`                       Small interface for Cn_Source basic operations
        `lexer_`                        Interface for Cn_Lexer operations, is coupled with `token_` functionality.
        `type_`                         Interface for basic type operations and introspection, inspired by Jai Type_Info.
        `any_`                          Interface to operate on coupled type and data, idea also inspired by Jai.
        `scope_`                        Small interface, it is resposible for scope operations.
        `ast_`                          Mostly structural interface, it specifies all node types, basic node operations and print traversal.
        `get_`                          Getter interface, doesn't modify anything only provides ast introspection ability.
        `diagnostic_`                   Interface to produce variety of source based diagnostics.
        `binding_`                      Binding interface, it is resposible for all names that are glued to some type, handles binding operations.
        `hook_`                         Interface to set up user hooks and send or receive messages between user and parser.
        `analyze_`                      Interface to perfom complex analysis on ast, typechecking, expression evaluation, etc.
        `emit_`                         Interface for C code emittion from Abstract Syntax Tree.
        `parse_`                        Interface for parsing, utilizes other sections to perfrom full source code to typed ast transition.
        `build_`                        Interface for user friendly construction of Abstract Syntax Tree, can be used in code transformations.
        `tu_`                           Interface to manage high level Translation Unit interactions, owns global state and used as entry for user.
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

/**
 * Log levels that are given to each logged message.
 *
 * SINCE: 0.3.0 
 */
typedef enum {
    CN_INFO,
    CN_WARNING,
    CN_ERROR,
    CN_NO_LOGS,
} Cn_Log_Level;

/**
 * Messages with a level below `cn_log_min_level` are suppressed by `cn_default_log_handler`.
 *
 * SINCE: 0.3.0
 */
extern Cn_Log_Level cn_log_min_level;

/**
 * Type definition for log handler, `cn_log_handler` is declared with this type.
 *
 * SINCE: 0.3.0
 */
typedef void (Cn_Log_Handler)(Cn_Log_Level level, const char *format, va_list args);

/**
 * Current log handler, set to `cn_default_log_handler` by default.
 *
 * SINCE: 0.3.0
 */
extern Cn_Log_Handler *cn_log_handler;

/**
 * Logs formatted message to `stderr`, prefixed with its log level.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Log_Handler cn_default_log_handler;

/**
 * Ignores all incoming logs, doesn't output anything anywhere.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Log_Handler cn_null_log_handler;

/**
 * Passes formatted log message to the current log handler.
 *
 * Message is printed by calling `cn_log_handler` with `level`, `format` and its arguments.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_log(Cn_Log_Level level, const char *format, ...);

/**
 * ============================================
 * SECTION: Hashing
 * ============================================
 */

/**
 * Type definition for hashing functions.
 *
 * Should return `uint64_t` hash of the specified `data`.
 *
 * SINCE: 0.3.0
 */
typedef uint64_t (Cn_Hash_Function)(const void *data);

/**
 * Type definition for equals functions.
 * 
 * If `a == b` should return `true`, false otherwise.
 *
 * SINCE: 0.3.0
 */
typedef bool (Cn_Equals_Function)(const void *a, const void *b);

/**
 * Hashes unsigned 64-bit integer using the `splitmix64` algorithm.
 *
 * RETURNS: Resulting hash.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_hash_u64(uint64_t value);

/**
 * Hashes the address of `ptr`.
 *
 * NOTE: Wraps around `cn_hash_u64`.
 *
 * RETURNS: Resulting hash.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_hash_ptr(void *ptr);

/**
 * Hashes sequence of `length` bytes using the `FNV-1a` algorithm.
 *
 * RETURNS: Resulting hash.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_hash_bytes(void *data, int64_t length);

/**
 * Mixes two hashes `a` and `b` into one.
 *
 * NOTE: Order matters, bit shifts are used to ensure `cn_hash_mix(a, b) != cn_hash_mix(b, a)`.
 *
 * RETURNS: Resulting hash.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_hash_mix(uint64_t a, uint64_t b);

/**
 * ============================================
 * SECTION: String
 * ============================================
 */

/**
 * Length based string definition.
 *
 * Length based strings are exactly the same as string views,
 * they are a simple structure of `length` and `data`,
 * paired with an interface to modify and use them in 
 * various ways.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t length;
    char *data;
} Cn_String;

/**
 * Constructs `Cn_String` directly from its `length` and `data`.
 *
 * SINCE: 0.3.0
 */
#define CN_STR(length, data)            ((Cn_String) { (int64_t)length, (char *)data } )

/**
 * Constructs `Cn_String` directly from string `literal`.
 *
 * SINCE: 0.3.0
 */
#define CN_STR_LIT(literal)             CN_STR(sizeof(literal) - 1, literal)

/**
 * Constructs `Cn_String` from a stack buffer filled with `literal`.
 *
 * Characters of `literal` are copied into a char array allocated on the stack,
 * so the resulting string can be modified.
 *
 * IMPORTANT: Buffer lives only until the end of the enclosing block.
 *
 * SINCE: 0.3.0
 */
#define CN_STR_BUFFER(literal)          CN_STR(sizeof(literal) - 1, (char[]){literal})

/**
 * Constructs `Cn_String` from an empty stack buffer of `size` chars.
 *
 * Works exactly like `CN_STR_BUFFER`, but doesn't take a literal and fill the 
 * buffer with any characters.
 *
 * IMPORTANT: Buffer lives only until the end of the enclosing block.
 *
 * SINCE: 0.3.0
 */
#define CN_STR_BUFFER_EMPTY(size)       CN_STR(size, (char[size]){})

/**
 * Unpacks `Cn_String` into its length and data components, for use with `%.*s` in formatting.
 *
 * For example given string `str`:
 *
 * ```c
 * printf("value of str: %.*s", CN_STR_UNPACK(str));
 * ```
 *
 * NOTE: Cast to `int` is required, because `%.*s` expects length of at most `INT_MAX`,
 * so string is truncated if its length exceeds `INT_MAX`.
 *
 * SINCE: 0.3.0
 */
#define CN_STR_UNPACK(str)                  (int)((str).length), (str).data

/**
 * Gets substring of `str` from index `start` up until index `end`.
 *
 * RETURNS: `Cn_String` that points to the memory of `str` at index `start`, with length up until index `end`.
 *
 * IMPORTANT: Doesn't copy memory. If `str` memory is freed later, 
 * returned string will no longer point to valid memory.
 *
 * NOTE: Character at index `end` is not included, domain of the resulting substring is always `[start, end)`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_substring(Cn_String str, int64_t start, int64_t end);

/**
 * Linearly searches for the first occurrence of `search` in `str` from the left.
 *
 * Strings are compared through `cn_str_equals`.
 *
 * RETURNS: Index of the first character of the occurrence, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_left(Cn_String str, Cn_String search);

/**
 * Linearly searches for the first occurrence of `search` in `str` from the right.
 *
 * Strings are compared through `cn_str_equals`.
 *
 * RETURNS: Index of the first character of the occurrence, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_right(Cn_String str, Cn_String search);

/**
 * Checks whether `str` ends with `end`.
 *
 * RETURNS: `true` if `str` ends with `end`, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_ends_with(Cn_String str, Cn_String end);

/**
 * Checks whether `str` starts with `start`.
 *
 * RETURNS: `true` if `str` starts with `start`, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_starts_with(Cn_String str, Cn_String start);

/**
 * Linearly searches for the first occurrence of char `symbol` in `str` from the left.
 *
 * RETURNS: Index of the occurrence, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_char_left(Cn_String str, char symbol);

/**
 * Linearly searches for the first occurrence of char `symbol` in `str` from the right.
 *
 * RETURNS: Index of the occurrence, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_char_right(Cn_String str, char symbol);

/**
 * Finds the first non-whitespace char in `str` from the left.
 *
 * RETURNS: Index of the first non-whitespace char, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_non_whitespace_left(Cn_String str);

/**
 * Finds the first non-whitespace char in `str` from the right.
 *
 * RETURNS: Index of the first non-whitespace char, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_non_whitespace_right(Cn_String str);

/**
 * Finds the first whitespace char in `str` from the left.
 *
 * RETURNS: Index of the first whitespace char, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_whitespace_left(Cn_String str);

/**
 * Finds the first whitespace char in `str` from the right.
 *
 * RETURNS: Index of the first whitespace char, otherwise `-1`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_find_whitespace_right(Cn_String str);

/**
 * Shortens `str` by excluding `count` chars from the left.
 *
 * RETURNS: Resulting string, pointing to the memory of `str`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_eat_chars(Cn_String str, int64_t count);

/**
 * Shortens `str` by excluding all whitespace chars from the left.
 *
 * RETURNS: Resulting string, pointing to the memory of `str`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_eat_spaces(Cn_String str);

/**
 * Shortens `str` by excluding chars from the left until it hits a whitespace.
 *
 * The whitespace itself is not excluded.
 *
 * RETURNS: Resulting string, pointing to the memory of `str`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_eat_until_space(Cn_String str);

/**
 * Gets first `count` chars of `str`.
 *
 * RETURNS: String that contains `count` chars from the left, pointing to the memory of `str`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_get_chars(Cn_String str, int64_t count);

/**
 * Gets everything in `str` until the first whitespace.
 *
 * RETURNS: String that contains everything until a whitespace, not including the whitespace itself.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_get_until_space(Cn_String str);

/**
 * Checks whether `str` is a valid symbol: a letter or `_`, followed by letters, digits or `_`.
 *
 * RETURNS: `true` if `str` contains only symbol chars, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_is_symbol(Cn_String str);

/**
 * Checks whether `str` is an integer.
 *
 * RETURNS: `true` if `str` is an integer, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_is_int(Cn_String str);

/**
 * Checks whether `str` is a float.
 *
 * RETURNS: `true` if `str` is a float, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_is_float(Cn_String str);

/**
 * Parses integer from `str`.
 *
 * RETURNS: Integer parsed from the string.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_parse_int(Cn_String str);

/**
 * Parses float from `str`.
 *
 * RETURNS: Float parsed from the string.
 *
 * SINCE: 0.3.0
 */
CNDEF float cn_str_parse_float(Cn_String str);

/**
 * Counts occurrences of char `c` in `str`.
 *
 * RETURNS: Number of occurrences of `c` in the string.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_str_count_chars(Cn_String str, char c);

/**
 * Copies contents of `str` into `buffer`.
 *
 * RETURNS: Pointer to `buffer`.
 *
 * IMPORTANT: `buffer` must have enough space to hold `str` data.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_str_copy_to(Cn_String str, void *buffer);

/**
 * Formats string and outputs it into the memory of `buffer`.
 *
 * RETURNS: String that corresponds to the formatted string, with `NULL` data if formatting failed.
 *
 * IMPORTANT: `buffer` must be big enough to hold the formatted string.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_str_format(Cn_String buffer, char *format, ...);

/**
 * Checks whether `str` is empty, by checking whether its length is `0`.
 *
 * RETURNS: `true` if string is empty, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_is_empty(Cn_String str);

/**
 * Compares `str1` and `str2`, checking for length equality first and then comparing char by char.
 *
 * RETURNS: `true` if strings are identical, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_is(Cn_String str1, Cn_String str2);

/**
 * Compares `str1` and `str2`, checking for length equality first and then comparing char by char.
 *
 * RETURNS: `true` if strings are identical, `false` otherwise.
 *
 * NOTE: Takes pointers to strings to be compatible with `Cn_Equals_Function`,
 * which allows it to be used in hash table and hash set.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_str_equals(const Cn_String *str1, const Cn_String *str2);

/**
 * Hashes contents of `str`.
 *
 * RETURNS: Hash of the string.
 *
 * NOTE: Takes pointer to string to be compatible with `Cn_Hash_Function`,
 * which allows it to be used in hash table and hash set.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_str_hash(const Cn_String *str);

/**
 * ============================================
 * SECTION: String Builder
 * ============================================
 */

/**
 * Dynamic array based string builder definition.
 *
 * This is a simple string builder to provide tools for
 * quick and dirty string formatting, concatenation and other
 * important operations.
 *
 * IMPORTANT: For the certain capacities this string builder allocated on the stack.
 * See `CN_SB_STACK_STORAGE_CAP` and `cn_sb_make` for more info.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t capacity;
    int64_t length;
    char *data;
} Cn_String_Builder;

/**
 * Maximum capacity under which string builder will be allocated on the stack.
 *
 * SINCE: 0.3.0
 */
#define CN_SB_STACK_STORAGE_CAP 32

/**
 * Makes new `Cn_String_Builder` on the stack if `cap` is small enough, otherwise on the heap.
 *
 * Stack storage is used when `cap` is not bigger than `CN_SB_STACK_STORAGE_CAP`.
 *
 * IMPORTANT: Lifetime of stack allocated string builder ends with the enclosing block.
 * If longer lifetime is needed, make sure to heap allocate the string builder,
 * or make sure it is resized to be heap allocated.
 *
 * SINCE: 0.3.0
 */
#define cn_sb_make(cap) ((cap) <= CN_SB_STACK_STORAGE_CAP ? ((Cn_String_Builder) { CN_SB_STACK_STORAGE_CAP, 0, (char *)(char[CN_SB_STACK_STORAGE_CAP]){}}) : cn__sb_make(cap))

CNDEF Cn_String_Builder cn__sb_make(int64_t initial_capacity);

/**
 * Frees heap memory occupied by `sb`.
 *
 * Memory is freed only if capacity is bigger than `CN_SB_STACK_STORAGE_CAP`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_free(Cn_String_Builder *sb);

/**
 * Clears all contents of `sb`, by setting its length to `0`.
 *
 * Doesn't allocate or free any memory.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_clear(Cn_String_Builder *sb);

/**
 * Appends char `c` to `sb`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_append_char(Cn_String_Builder *sb, char c);

/**
 * Appends `str` to `sb`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_append_str(Cn_String_Builder *sb, Cn_String str);

/**
 * Inserts `str` at the front of `sb`, shifting existing contents to the right.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_prepend_str(Cn_String_Builder *sb, Cn_String str);

/**
 * Formats string and appends the output to `sb`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_append_format(Cn_String_Builder *sb, char *format, ...);

/**
 * Reverses contents of `sb`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_sb_reverse(Cn_String_Builder *sb);

/**
 * Converts string builder to string that points to the memory used by `sb`.
 *
 * RETURNS: String pointing to the contents of `sb`.
 *
 * IMPORTANT: If `sb` is freed or grows, memory pointed to by the string might be invalid. 
 * Contents of the returned string can be copied elsewhere to prevent such invalidation.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_sb_to_str(Cn_String_Builder *sb);

/**
 * ============================================
 * SECTION: Allocator
 * ============================================
 */

#include <stdlib.h>

/**
 * Forward declaration of `cn_allocator`.
 *
 * SINCE: 0.3.0
 */
typedef struct cn_allocator Cn_Allocator;

/**
 * Type defintion of allocator alloc.
 *
 * SINCE: 0.3.0
 */
typedef CNDEF void *(Cn_Alloc)(Cn_Allocator *allocator, size_t size);

/**
 * Type defintion of allocator realloc.
 *
 * SINCE: 0.3.0
 */
typedef CNDEF void *(Cn_Realloc)(Cn_Allocator *allocator, void *mem, size_t new_size);

/**
 * Type defintion of allocator free.
 *
 * SINCE: 0.3.0
 */
typedef CNDEF void (Cn_Free)(Cn_Allocator *allocator, void *mem);

/**
 * Type defintion of allocator free all.
 *
 * SINCE: 0.3.0
 */
typedef CNDEF void (Cn_Free_All)(Cn_Allocator *allocator);

/**
 * Base members definition for allocators.
 *
 * This library uses a variety of allocators: arenas, pools, block allocators, etc.
 * But it also doesn't want to introduce extra complexity regarding usage of such allocators,
 * so in order to provide a middle ground between simplicity and functionality, all
 * allocators must implement the same foundational interface and can be replaced with one another
 * at any point in time. This allows users to customize and control memory, while the library has
 * stable and well understood internals.
 *
 * Each allocator must implement its own version of the following functions:
 *
 * ```c
 * CNDEF void *cn_alloc(Cn_Allocator *allocator, size_t size);
 * CNDEF void *cn_realloc(Cn_Allocator *allocator, void *mem, size_t new_size);
 * CNDEF void cn_free(Cn_Allocator *allocator, void *mem);
 * CNDEF void cn_free_all(Cn_Allocator *allocator);
 * ```
 *
 * NOTE: If an allocator cannot perform a certain function, like an arena that cannot free memory by pointer,
 * it is allowed to stub out the function and log a warning on attempt to use it.
 *
 * Switching the global allocator then becomes as simple as setting the global allocator variable,
 * and separate allocators for different parts of the library can be replaced too.
 *
 * SINCE: 0.3.0
 */
#define CN_ALLOCATOR_BASE_MEMBERS   \
    Cn_Alloc        *alloc;         \
    Cn_Realloc      *realloc;       \
    Cn_Free         *free;          \
    Cn_Free_All     *free_all;      \

/**
 * Allocator definition.
 *
 * SINCE: 0.3.0
 */
struct cn_allocator {
    CN_ALLOCATOR_BASE_MEMBERS
};

/**
 * Allocator base that is added to each allocator struct definition.
 *
 * SINCE: 0.3.0
 */
#define CN_ALLOCATOR_BASE                       \
    union {                                     \
        Cn_Allocator base;                      \
        struct { CN_ALLOCATOR_BASE_MEMBERS };   \
    }

/**
 * Arena, linearly gives memory from the pre-allocated block.
 *
 * All memory is freed at once, and error will be issued if overflow occurs.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_ALLOCATOR_BASE;
    size_t capacity;
    void *allocation;
    void *ptr;
} Cn_Arena;

/**
 * Constructs arena and allocates `capacity` bytes of memory for it.
 *
 * RETURNS: New arena, zeroed out if memory couldn't be allocated.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Arena cn_arena_make(size_t capacity);

/**
 * Allocates next chunk of `size` bytes from `arena`.
 *
 * RETURNS: Pointer to the allocated memory, `NULL` if allocation exceeds arena's capacity.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_arena_alloc(Cn_Arena *arena, size_t size);

/**
 * Reallocates chunk of memory `mem` from `arena`.
 *
 * Allocates new chunk of `new_size` bytes through `cn_arena_alloc`.
 *
 * RETURNS: Pointer to the new chunk of memory.
 *
 * IMPORTANT: Contents of `mem` are not copied into the new chunk.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_arena_realloc(Cn_Arena *arena, void *mem, size_t new_size);

/**
 * Stubbed out, since arena cannot free previously allocated memory.
 *
 * NOTE: Logs a warning when called, use `cn_arena_free_all` or `cn_arena_destroy` instead.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_arena_free(Cn_Arena *arena, void *mem);

/**
 * Releases all allocations of `arena` at once, by resetting it to the beginning of its memory.
 *
 * Arena memory itself stays allocated and can be reused.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_arena_free_all(Cn_Arena *arena);

/**
 * Completely destroys `arena` and frees all memory occupied by it.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_arena_destroy(Cn_Arena *arena);

/**
 * Chained arena is an allocator that linearly hands out memory and allocates next block when needed.
 *
 * Pointers it returns are guaranteed to point to valid memory at all times, until freed or popped.
 * Instead of making a forward linked list of blocks,
 * it makes a reverse linked list of blocks and always points to the most recent block.
 *
 * ```
 *  block_capacity
 *  <--------->
 *
 *  -----------     -----------     -----------
 *  | Block 1 | <-- | Block 2 | <-- | Block 3 |
 *  -----------     -----------     -----------
 *                                  ^      ^
 *                                  *block *ptr
 *  <------------- allocated ------------->
 * ```
 *
 * This setup allows quick append allocation and pop deallocation.
 *
 * Every block carries its own capacity, which normally is the `block_capacity` set by the user.
 * An allocation that doesn't fit in a regular block gets an oversized block of exactly its size,
 * so no allocation is ever refused because of the `block_capacity` chosen.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_ALLOCATOR_BASE;
    size_t block_capacity;
    void  *block;
} Cn_Chained_Arena;

/**
 * Each block allocated by `Cn_Chained_Arena` contains a header.
 * This macro gets this information from the specific block.
 *
 * SINCE: 0.3.0
 */
#define CN_CHAINED_ARENA_BLOCK_HEADER(block) ((Cn_Chained_Arena_Block_Header *)((uint8_t *)(block) - sizeof(Cn_Chained_Arena_Block_Header)))

/**
 * Definition of the block header that is allocated by `Cn_Chained_Arena` 
 * for each block.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    void  *prev;
    void  *next;
    size_t capacity;
    size_t allocated;
} Cn_Chained_Arena_Block_Header;

/**
 * Constructs chained arena, allocating its first block of `block_capacity` bytes.
 *
 * RETURNS: New chained arena.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Chained_Arena cn_chained_arena_make(size_t block_capacity);

/**
 * Allocates `size` bytes from `arena`.
 *
 * RETURNS: Pointer to the allocated memory.
 *
 * NOTE: If `size` is bigger than the arena's `block_capacity`, an oversized block of exactly
 * that size is allocated to hold it, and a warning is logged.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_chained_arena_alloc(Cn_Chained_Arena *arena, size_t size);

/**
 * Reallocates chunk of memory `mem` from `arena`.
 *
 * Allocates new chunk of `new_size` bytes through `cn_chained_arena_alloc`.
 *
 * RETURNS: Pointer to the new chunk of memory.
 *
 * IMPORTANT: Contents of `mem` are not copied into the new chunk.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_chained_arena_realloc(Cn_Chained_Arena *arena, void *mem, size_t new_size);

/**
 * Stubbed out, since arena cannot free previously allocated memory.
 *
 * NOTE: Use `cn_chained_arena_dealloc`, `cn_chained_arena_free_all` or `cn_chained_arena_destroy` instead.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_chained_arena_free(Cn_Chained_Arena *arena, void *mem);

/**
 * Releases all allocations of `arena` at once, by resetting every block and returning to the first one.
 *
 * Blocks themselves stay allocated and are reused by later allocations.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_chained_arena_free_all(Cn_Chained_Arena *arena);

/**
 * Deallocates last `size` bytes allocated from `arena`.
 *
 * If `size` covers a whole block, arena moves back to the previous block.
 * Emptied blocks stay allocated and are reused by later allocations.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_chained_arena_dealloc(Cn_Chained_Arena *arena, size_t size);

/**
 * Finds the block and offset of memory allocated by `arena`.
 *
 * Given chained arena and pointer allocated by it, computes index of the block
 * it was allocated in, and byte offset from the beginning of that block to the allocated memory.
 *
 * RETURNS: Index of the block, `-1` if `allocation_ptr` doesn't point to the allocated memory.
 *
 * OUTPUTS: Byte offset to the allocated memory from the beginning of the block, written to `offset`.
 *
 * NOTE: Returns properly whenever `allocation_ptr` points to memory held by the arena,
 * even if it isn't the exact address returned by an allocation.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_chained_arena_allocation_info(Cn_Chained_Arena *arena, void *allocation_ptr, size_t *offset);

/**
 * Computes total size allocated by `arena`.
 *
 * RETURNS: Total allocated size in bytes.
 *
 * SINCE: 0.3.0
 */
CNDEF size_t cn_chained_arena_allocated(Cn_Chained_Arena *arena);

/**
 * For each loop macro to linearly iterate items in the block.
 *
 * SINCE: 0.3.0
 */
#define cn_chained_arena_foreach_in_block(type, it, block) for (type *it = (type *)(block); (uint8_t *)it <= (uint8_t *)(block) + CN_CHAINED_ARENA_BLOCK_HEADER(block)->allocated - sizeof(type); it++)

/**
 * Completely destroys `arena` and frees all memory occupied by it.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_chained_arena_destroy(Cn_Chained_Arena *arena);

/**
 * Maximum capacity of a single pool block.
 *
 * SINCE: 0.3.0
 */
#define CN_POOL_BLOCK_MAX_CAPACITY 4096

/**
 * Minimum capacity of a single pool block.
 *
 * SINCE: 0.3.0
 */
#define CN_POOL_BLOCK_MIN_CAPACITY 8

/**
 * Forward declaration of `cn_pool_block`.
 *
 * SINCE: 0.3.0
 */
typedef struct cn_pool_block Cn_Pool_Block;

/**
 * Struct definition of single pool allocator block.
 *
 * MEMBER capacity: Capacity per allocation, `size_t` keeps mem aligned as malloc returned it.
 *
 * MEMBER flags: Bit flags, if bit set to, specific allocation is occupied.
 *
 * MEMBER mem: Whole allocation.
 *
 * SINCE: 0.3.0
 */
struct cn_pool_block {
    Cn_Pool_Block *next;
    size_t capacity;
    uint8_t flags[CN_POOL_BLOCK_MAX_CAPACITY / CN_POOL_BLOCK_MIN_CAPACITY / 8];
    uint8_t mem[CN_POOL_BLOCK_MAX_CAPACITY];
};

/**
 * Pool is an allocator that hands out fixed size slots cut from pre-divided blocks.
 *
 * It is constructed from a list of capacities, where every capacity defines a size class.
 * Each size class owns a chain of blocks, and each block is a bit field of occupied
 * slots followed by the memory those slots are cut from.
 *
 * ```
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
 * ```
 *
 * Blocks are always `CN_POOL_BLOCK_MAX_CAPACITY` bytes big, so a size class of capacity `8`
 * cuts its block into `512` slots, while a size class of capacity `4096` cuts it into `1`.
 * When every block of a size class is full, one more block is chained to its end.
 *
 * Pool doesn't store any header next to the returned memory, so the block a pointer
 * belongs to is found by searching the blocks for the one whose memory contains it.
 *
 * Pool struct itself only holds the array of size class chains and its length, and that
 * array is the array of capacities supplied to `cn_pool_make`, overwritten in place with
 * block pointers.
 *
 * IMPORTANT: Capacities must be listed in strictly ascending order, so that the first size class
 * big enough for an allocation is also the tightest fit for it.
 *
 * IMPORTANT: Pool must not outlive the supplied array. `cn_pool_make` used with a compound literal
 * inside a function is only valid until the end of the enclosing block.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_ALLOCATOR_BASE;
    Cn_Pool_Block **blocks;
    size_t blocks_length;
} Cn_Pool;

/**
 * Number of slots a block of given capacity is divided into.
 *
 * SINCE: 0.3.0
 */
#define CN_POOL_BLOCK_SLOTS(block) (CN_POOL_BLOCK_MAX_CAPACITY / (block)->capacity)

/**
 * Constructs pool and allocates one block per supplied capacity.
 *
 * IMPORTANT: Capacities must be strictly ascending, multiples of `CN_POOL_BLOCK_MIN_CAPACITY`,
 * and not bigger than `CN_POOL_BLOCK_MAX_CAPACITY`.
 *
 * IMPORTANT: Assumes `sizeof(size_t) == sizeof(void *)`, because it reuses the supplied array of capacities.
 *
 * SINCE: 0.3.0
 */
#define cn_pool_make(...) cn__pool_make((size_t []) { __VA_ARGS__ }, (sizeof((size_t []) { __VA_ARGS__ }) / sizeof(size_t)))

CNDEF Cn_Pool cn__pool_make(size_t capacities[], size_t capacities_length);

/**
 * Allocates next free slot of memory from `pool`.
 *
 * Slot is taken from the smallest size class that fits the requested `size`.
 *
 * RETURNS: Pointer to the allocated slot.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_pool_alloc(Cn_Pool *pool, size_t size);

/**
 * Reallocates slot of memory `mem` from `pool`.
 *
 * If `new_size` still fits the slot, same memory is returned.
 * If `mem` is `NULL`, works like `cn_pool_alloc`.
 *
 * RETURNS: Pointer to the reallocated memory, `NULL` if `mem` wasn't allocated by `pool`.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_pool_realloc(Cn_Pool *pool, void *mem, size_t new_size);

/**
 * Frees slot of memory `mem` from `pool`.
 *
 * NOTE: Does nothing if `mem` is `NULL`, logs a warning if `mem` wasn't allocated by `pool`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_pool_free(Cn_Pool *pool, void *mem);

/**
 * Marks every slot of every block as free, keeping the blocks themselves allocated.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_pool_free_all(Cn_Pool *pool);

/**
 * Finds the block and slot of memory allocated by `pool`.
 *
 * Given pool and pointer allocated by it, finds the block the pointer was
 * allocated from, and index of the slot it occupies in that block.
 *
 * RETURNS: Block containing the pointer, `NULL` if `allocation_ptr` doesn't point to the
 * beginning of an occupied slot, which is also the case for already freed memory.
 *
 * OUTPUTS: Index of the occupied slot in the found block, written to `slot`.
 *
 * NOTE: Returns properly whenever `allocation_ptr` points to the beginning of an occupied slot,
 * even if it isn't the exact address returned by an allocation.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Pool_Block *cn_pool_allocation_info(Cn_Pool *pool, void *allocation_ptr, size_t *slot);

/**
 * Completely destroys `pool` and frees all memory occupied by it.
 *
 * NOTE: Supplied array of capacities is not freed, since pool doesn't own it.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_pool_destroy(Cn_Pool *pool);

/**
 * Default allocator that this library uses for most of the allocations.
 * See `CN_REALLOC` and `CN_FREE`.
 *
 * SINCE: 0.3.0
 */
extern Cn_Allocator *cn_allocator;

/**
 * Calls `alloc` of `allocator`, passing the allocator itself as a parameter.
 *
 * RETURNS: Pointer to the allocated memory.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_alloc(Cn_Allocator *allocator, size_t size);

/**
 * Calls `realloc` of `allocator`, passing the allocator itself as a parameter.
 *
 * RETURNS: Pointer to the reallocated memory.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_realloc(Cn_Allocator *allocator, void *mem, size_t size);

/**
 * Calls `free` of `allocator`, passing the allocator itself as a parameter.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_free(Cn_Allocator *allocator, void *mem);

/**
 * Calls `free_all` of `allocator`, passing the allocator itself as a parameter.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_free_all(Cn_Allocator *allocator);

/**
 * Realloc wrapper that uses `cn_allocator` underneath.
 *
 * SINCE: 0.3.0
 */
#define CN_REALLOC(mem, size)   cn_realloc(cn_allocator, (mem), (size))

/**
 * Free wrapper that uses `cn_allocator` underneath.
 *
 * SINCE: 0.3.0
 */
#define CN_FREE(mem)            cn_free(cn_allocator, (mem))

/**
 * ============================================
 * SECTION: Array List
 * ============================================
 */

/**
 * Array list header that is allocated before allocation block to store 
 * all of the important array list information.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t capacity;
    int64_t length;
    int64_t item_size;
} Cn_Array_List_Header;

/**
 * Makes new array list allocated dynamically with a `Cn_Array_List_Header` included.
 * 
 * RETURNS: Pointer of specified `type` that can passed to `cn_array_list_*` functionality.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_make(type, capacity)\
    (type *)cn__array_list_make(sizeof(type), capacity)

/**
 * RETURNS: `int64_t` length of an array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_length(ptr_list)\
    cn__array_list_length((void *)*ptr_list)

/**
 * RETURNS: `int64_t` capacity of an array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_capacity(ptr_list)\
    cn__array_list_capacity((void *)*ptr_list)

/**
 * RETURNS: `int64_t` item size of an array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_item_size(ptr_list)\
    cn__array_list_item_size((void *)*ptr_list)

/**
 * Appends `item` to the specified array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_append(ptr_list, item)\
    do {\
        cn__array_list_resize_to_fit((void **)(ptr_list), cn_array_list_length(ptr_list) + 1);\
        Cn_Array_List_Header *cn__h_ = (Cn_Array_List_Header *)((uint8_t *)(*ptr_list) - sizeof(Cn_Array_List_Header));\
        (*ptr_list)[cn__h_->length++] = item;\
    } while(0)

/**
 * Appends array of items, `item_arr`, to the specified array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_append_multiple(ptr_list, item_arr, count)\
    cn__array_list_append_multiple((void **)ptr_list, (void *)item_arr, count)

/**
 * Adds `item` to the specified `index` in array list, shifting all other items one place to the right.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_add(ptr_list, index, item)\
    do {\
        cn__array_list_shift_to_fit((void **)(ptr_list), index, 1);\
        (*ptr_list)[index] = item;\
    } while(0)

/**
 * Removes last item from specified array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_pop(ptr_list)\
    cn__array_list_pop((void *)*ptr_list, 1)

/**
 * Removes `count` of items from the end of specified array list.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_pop_multiple(ptr_list, count)\
    cn__array_list_pop((void *)*ptr_list, count)

/**
 * Sets array list length to `0`, clearing all its items.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_clear(ptr_list)\
    cn__array_list_clear((void *)*ptr_list)

/**
 * Removes item at `index` from the specified array list in constant time,
 * by swapping item with the last item, and decreasing length by -1.
 *
 * SINCE: 0.3.0
 */
#define cn_array_list_unordered_remove(ptr_list, index)\
    cn__array_list_unordered_remove((void *)*ptr_list, index)

/**
 * Frees all memory occupied by the array list.
 *
 * SINCE: 0.3.0
 */
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

/**
 * Hash table header that is allocated before allocation block to store 
 * all of the important hast table information.
 *
 * SINCE: 0.3.0
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

/**
 * Possible states that slots in hash table can contain.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_HASH_TABLE_SLOT_EMPTY,
    CN_HASH_TABLE_SLOT_OCCUPIED,
    CN_HASH_TABLE_SLOT_DELETED,
} Cn_Hash_Table_Slot_State;

/**
 * Definition of a hash table slot.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Hash_Table_Slot_State state;
    uint64_t hash;
} Cn_Hash_Table_Slot;

/**
 * Makes new hash table allocated dynamically with a `Cn_Hash_Table_Header` included.
 *
 * RETURNS: Pointer of specified `type` that can passed to `cn_hash_table_*` functionality.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_make(key_type, value_type, capacity, hash_func, equals_func)\
    (value_type *)cn__hash_table_make(sizeof(key_type), sizeof(value_type), capacity, hash_func, equals_func) 

/**
 * Given the pointer to the hash table, gets its header.
 *
 * RETURNS: `Cn_Hash_Table_Header *` pointer to the table's header.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_header(table_ptr)\
    ((Cn_Hash_Table_Header *)((uint8_t *)*table_ptr - sizeof(Cn_Hash_Table_Header)))

/**
 * RETURNS: `int64_t` count of a hash table.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_count(ptr_table)\
    cn__hash_table_count((void *)*ptr_table)

/**
 * RETURNS: `int64_t` capacity of a hash table.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_capacity(ptr_table)\
    cn__hash_table_capacity((void *)*ptr_table)

/**
 * RETURNS: `int64_t` item size of a hash table.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_item_size(ptr_table)\
    cn__hash_table_item_size((void *)*ptr_table)

/**
 * Puts `item` into the hash table, using hash of the speicified key.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_put(ptr_table, item, ptr_key)\
    do {\
        cn__hash_table_resize_to_fit((void **)(ptr_table), cn_hash_table_count(ptr_table) + 1);\
        (*ptr_table)[cn__hash_table_push_key((void **)(ptr_table), ptr_key)] = item;\
    } while(0)

/**
 * Gets `item` from the hash table, by the hash of its key.
 * 
 * RETURNS: Pointer to the `item` if found, `NULL` if `item` was not found.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_get(ptr_table, ptr_key)\
    cn__hash_table_get((void **)(ptr_table), ptr_key)

/**
 * Removes `item` from the hash table, by the hash of its key.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_remove(ptr_table, ptr_key)\
    cn__hash_table_remove((void **)(ptr_table), ptr_key) 

/**
 * Frees all memory occupied by the hash table.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_table_free(ptr_table)\
    cn__hash_table_free((void **)(ptr_table))

/**
 * Prints contents of the hash table to `out`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_hash_table_print(FILE *out, void **table);

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

/**
 * Hash set header that is allocated before allocation block to store 
 * all of the important hast set information.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t capacity;
    int64_t count;
    int64_t item_size;
    Cn_Hash_Function *hash_func; 
    Cn_Equals_Function *equals_func;
    float load_factor;
} Cn_Hash_Set_Header;

/**
 * Possible states that slots in hash set can contain. Same as `Cn_Hash_Table_Slot_State`.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_HASH_SET_SLOT_EMPTY,
    CN_HASH_SET_SLOT_OCCUPIED,
    CN_HASH_SET_SLOT_DELETED,
} Cn_Hash_Set_Slot_State;

/**
 * Definition of a hash set slot. Same as `Cn_Hash_Table_Slot`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Hash_Set_Slot_State state;
    uint64_t hash;
} Cn_Hash_Set_Slot;

/**
 * Makes new hash set allocated dynamically with a `Cn_Hash_Set_Header` included.
 *
 * RETURNS: Pointer of specified `type` that can passed to `cn_hash_set_*` functionality.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_set_make(type, capacity, hash_func, equals_func)\
    (type *)cn__hash_set_make(sizeof(type), capacity, hash_func, equals_func) 

/**
 * Given the pointer to the hash set, gets its header.
 *
 * RETURNS: `Cn_Hash_Set_Header *` pointer to the set's header.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_set_header(ptr_set)\
    ((Cn_Hash_Set_Header *)((uint8_t *)((*ptr_set) - 1) - sizeof(Cn_Hash_Set_Header)))

/**
 * Puts `item` into the hash set, using its hash.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_set_put(ptr_set, item)\
    (*((*ptr_set) - 1) = item, cn__hash_set_put(cn_hash_set_header(ptr_set), (void **)ptr_set), *((*ptr_set) - 1))

/**
 * Checks if `item` is inside the hash set.
 *
 * RETURNS: `true` if item is found, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_set_contains(ptr_set, item)\
    (*((*ptr_set) - 1) = item, cn__hash_set_contains(cn_hash_set_header(ptr_set)))

/**
 * Removes `item` from the hash set.
 *
 * SINCE: 0.3.0
 */
#define cn_hash_set_remove(ptr_set, item)\
    (*((*ptr_set) - 1) = item, cn__hash_set_remove(cn_hash_set_header(ptr_set)))

/**
 * Frees all memory occupied by the hash set.
 *
 * SINCE: 0.3.0
 */
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

/**
 * Struct definition that reprsents source location, 
 * storing full information about source and offsets.
 *
 * Used to track source strings and print snippets.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_String source;
    int64_t offset;
    int64_t length;
} Cn_Source;

/**
 * Struct definition that represents file location, 
 * storing `file:line:col:` values.
 *
 * Used to track file location for the diagnostics.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_String file;
    int64_t line;
    int64_t column;
} Cn_Location;

/**
 * Gets string of the source code that `src` points to.
 *
 * RETURNS: String based on the supplied source.
 *
 * NOTE: If length of `src` is `0`, empty string is returned,
 * but its data still points to the valid memory specified by the source.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_source_to_str(Cn_Source *src);

/**
 * Computes signed distance between offsets of sources `a` and `b`.
 * 
 * Used to manually improve region highlighted by diagnostics.
 *
 * RETURNS: Signed distance from source `a` to source `b`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_source_dist(Cn_Source *a, Cn_Source *b);

/**
 * Checks whether `src` is empty, meaning its length is `0`.
 *
 * RETURNS: Non-zero if source is empty, `0` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_source_is_empty(Cn_Source *src);

/**
 * ============================================
 * SECTION: Lexer
 * ============================================
 */

/**
 * Enum collection of all core token kinds.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    // General tokens.
    CN_TOKEN_EOF,
    CN_TOKEN_UNKNOWN,
    CN_TOKEN_STRING,
    CN_TOKEN_COMMENT,
    CN_TOKEN_LINE_MARKER,
    CN_TOKEN_INTEGER_VALUE,
    CN_TOKEN_FLOAT_VALUE,
    CN_TOKEN_CHAR_VALUE,
    CN_TOKEN_IDENTIFIER,

    // Literal tokens, non-identifer.
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

    // Keyword tokens.
    CN_TOKEN_CONST,
    CN_TOKEN_RESTRICT,
    CN_TOKEN_VOLATILE,
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
    CN_TOKEN_CASE,
    CN_TOKEN_DEFAULT,
    CN_TOKEN_COMPLEX,
    CN_TOKEN_IMAGINARY,

    // TEMPORARY: Post C99.
    CN_TOKEN_ATOMIC,
    CN_TOKEN_NORETURN,

#ifdef CN_GNU
    CN_TOKEN_GNU_ASM,
    CN_TOKEN_GNU_EXTENSION,
    CN_TOKEN_GNU_ATTRIBUTE,
    CN_TOKEN_GNU_TYPEOF,
#endif // CN_GNU
} Cn_Token_Kind;

/**
 * Definition of token struct, each token is identified by `kind` and
 * keeps track of it's source and location.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Token_Kind   kind;
    Cn_Location     loc;
    Cn_Source       src;
} Cn_Token;

/**
 * Struct to group token kind and its respective string literal.
 *
 * For example:
 * ```c
 *     { CN_TOKEN_INT, CN_STR_BUFFER("int") }
 * ```
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Token_Kind kind;
    Cn_String     literal;
} Cn_Token_Literal;

/**
 * Count of tokens ahead of the current one, that lexer can look up.
 *
 * SINCE: 0.3.0
 */
#define CN_LEXER_LOOKUP_COUNT       8

/**
 * Count of tokens behind the current one, that lexer can look down to.
 *
 * SINCE: 0.3.0
 */
#define CN_LEXER_LOOKDOWN_COUNT     8

/**
 * Total count of tokens that lexer has in the queue.
 *
 * SINCE: 0.3.0
 */
#define CN_LEXER_QUEUE_COUNT        (CN_LEXER_LOOKDOWN_COUNT + 1 + CN_LEXER_LOOKUP_COUNT)

/**
 * Struct to store array of blacklisted token kinds.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Token_Kind *ttypes;
    int64_t length;
} Cn_Lexer_Blacklist;

/**
 * Definition of the lexer struct.
 *
 * This lexer goes through source linearly producing tokens. Tokens are stored in the `queue`,
 * which is done so that user can easily `peek` previous or upcoming tokens, while the current token
 * is always stored at `current_idx` in the `queue`.
 *
 * MEMBER cursor: Index in `content` at which lexer is currently sitting.
 *
 * MEMBER bol: Index in `content` that marks beginning of currently lexed line.
 *
 * MEMBER line: Line number the lexer is on.
 *
 * MEMBER file: File name of the lexed text.
 *
 * MEMBER content: Source of the lexed text.
 * 
 * MEMBER head_idx: Queue index of the most recently lexed upcoming token.
 *
 * MEMBER current_idx: Queue index of the token that is currently viewed.
 *
 * MEMBER tail_idx: Queue index of the most previous lexed token.
 *
 * MEMBER queue: Queue buffer that can store maximum of `CN_LEXER_QUEUE_COUNT` tokens at once.
 *
 * MEMBER blacklist: Tokens specified in here will be skipped by the lexer completely.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t cursor;
    int64_t bol;
    int64_t line;
    Cn_String file;
    Cn_String content;
    int64_t head_idx;
    int64_t tail_idx;
    int64_t current_idx;
    Cn_Token queue[CN_LEXER_QUEUE_COUNT];
    Cn_Lexer_Blacklist blacklist;
} Cn_Lexer;

/**
 * Unwraps to the current token of `lexer`.
 *
 * Can change whenever `cn_lexer_next_token` is called.
 *
 * SINCE: 0.3.0
 */
#define cn_lexer_token(lexer)   (lexer)->queue[(lexer)->current_idx]

/**
 * Unwraps to the most recently lexed upcoming token of the `lexer`.
 *
 * Can change whenever `cn_lexer_next_token` is called.
 *
 * SINCE: 0.3.0
 */
#define cn_lexer_head(lexer)    (lexer)->queue[(lexer)->head_idx]

/**
 * Unwraps to the most previous token of the `lexer`.
 *
 * Can change whenever `cn_lexer_next_token` is called.
 *
 * SINCE: 0.3.0
 */
#define cn_lexer_tail(lexer)    (lexer)->queue[(lexer)->tail_idx]

/**
 * Initializes `lexer` with specific `content`, setting up queue and getting ready to output tokens.
 *
 * PARAM blacklist: Optional, but if set, tokens from that list won't appear in the lexed output.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_lexer_init(Cn_Lexer *lexer, Cn_String content, Cn_Lexer_Blacklist blacklist);

/**
 * Loads new `content` into `lexer`, resetting queue cursor and everything else.
 *
 * Compared to `cn_lexer_init`, only initializes content. It overwrites previous content
 * and tokens that were enqueued in lexer, without touching `file`, `line` or `blacklist` values.
 *
 * Can be used to inject any content into the lexer without changing location 
 * from which the lexer is starting.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_lexer_load_content(Cn_Lexer *lexer, Cn_String content);

/**
 * Advances `lexer` to the next token.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_lexer_next_token(Cn_Lexer *lexer);

/**
 * Checks whether current token of `lexer` matches expected `type`.
 *
 * RETURNS: `true` if it matches, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_lexer_expect(Cn_Lexer *lexer, Cn_Token_Kind kind);

/**
 * Peeks token based on its `offset` from the current token.
 *
 * `offset == 0` peeks current token, `offset < 0` peeks past tokens.
 *
 * RETURNS: Token at the requested offset. `CN_TOKEN_UNKNOWN` if requested token is out of queue bounds,
 * `CN_TOKEN_EOF` if requested token comes after already found `CN_TOKEN_EOF`.
 *
 * IMPORTANT: Expects `-CN_LEXER_LOOKDOWN_COUNT < offset < CN_LEXER_LOOKUP_COUNT`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Token cn_lexer_peek(Cn_Lexer *lexer, int64_t offset);

/**
 * Logs next `n` tokens from `lexer`, if `n == -1` logs all available tokens.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_lexer_log_tokens(Cn_Lexer *lexer, int64_t n);

/**
 * Checks if `lexer` state was changed from the given `saved` state.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_lexer_changed(Cn_Lexer *lexer, Cn_Lexer *saved);

/**
 * Gets name of the token `type`, used in messages.
 *
 * RETURNS: Static string of the token type.
 *
 * SINCE: 0.3.0
 */
CNDEF const char *cn_token_kind_name(Cn_Token_Kind kind);

/**
 * RETURNS: `true` if tokens `a == b`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_token_equals(Cn_Token *a, Cn_Token *b);

/**
 * ============================================
 * SECTION: Type
 * ============================================
 */

/**
 * Type flags, most of which are specific to different `Cn_Type_Kind`.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_TYPE_COMPLETE = 0x1,

    CN_TYPE_QUALIFIED_CONSTANT = 0x2,
    CN_TYPE_QUALIFIED_RESTRICT = 0x4,
    CN_TYPE_QUALIFIED_VOLATILE = 0x8,
    CN_TYPE_QUALIFIED_ATOMIC   = 0x10,

    CN_TYPE_ARRAY_VARIABLE     = 0x20,
    CN_TYPE_ARRAY_UNSPECIFIED  = 0x40,
} Cn_Type_Flags;

/**
 * Type kinds, list of all interned type kinds that are constructed from 
 * abstract syntax tree during analysis.
 *
 * SINCE: 0.3.0
 */
typedef enum {
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

/**
 * Definition of the base members of any type.
 *
 * SINCE: 0.3.0
 */
#define CN_TYPE_BASE_MEMBERS    \
    Cn_Type_Kind kind;          \
    Cn_Type_Flags flags;        \
    int64_t size;               \
    int64_t align;              \

/**
 * Definition of the base type struct. Used by every other type struct definition.
 *
 * SINCE: 0.3.0
 */
#define CN_TYPE_BASE                    \
    struct { CN_TYPE_BASE_MEMBERS };    \

/**
 * Forward declaration for `cn_type` union.
 *
 * SINCE: 0.3.0
 */
typedef union cn_type Cn_Type;

/**
 * Definition of integer type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    bool is_signed;
} Cn_Type_Integer;

/**
 * Definition of pointer type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_Type *ptr_to;
} Cn_Type_Pointer;

/**
 * Definition of function parameter
 *
 * NOTE: It's not really a type but more a like a wrapper for `Cn_Type_Function` to use 
 * for its parameters.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Type *type;
} Cn_Type_Function_Param;

/**
 * Definition of function type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_Type *return_type;

    int64_t params_length;
    Cn_Type_Function_Param *params;

    bool is_variadic;
} Cn_Type_Function;

/**
 * Definition of struct member.
 *
 * NOTE: It's not a type, used similarly to `Cn_Type_Function_Param`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Type *type;
    Cn_String name;
    int64_t offset;
} Cn_Type_Struct_Member;

/**
 * Definition of struct type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_String tag;
    
    int64_t members_length;
    Cn_Type_Struct_Member *members;
} Cn_Type_Struct;

/**
 * Definition of union member.
 *
 * NOTE: It's not a type, used similarly to `Cn_Type_Function_Param`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Type *type;
    Cn_String name;
} Cn_Type_Union_Member;

/**
 * Definition of union type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_String tag;
    
    int64_t members_length;
    Cn_Type_Union_Member *members;
} Cn_Type_Union;

/**
 * Definition of array type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_Type *element_type;
    int64_t length;
} Cn_Type_Array;

/**
 * Definition of enum member.
 *
 * NOTE: It's not a type, used similarly to `Cn_Type_Function_Param`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_String name;
    int64_t value;
} Cn_Type_Enum_Member;

/**
 * Definition of enum type.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_String tag;

    Cn_Type *member_type;

    int64_t members_length;
    Cn_Type_Enum_Member *members;
} Cn_Type_Enum;

/**
 * Definition of qualified type.
 *
 * In this type universe, qualifiers `const`, `_Atomic`, etc...
 * are stored as type wrappers around base type. The reason this is done is 
 * due to how C type completion for tagged types is set up.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_TYPE_BASE;
    Cn_Type *base_type;
} Cn_Type_Qualified;

/**
 * Definition of all types union.
 *
 * SINCE: 0.3.0
 */
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

/**
 * Constant struct of 32 bit signed integer `int` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_INT;

/**
 * Constant struct of 32 bit float `float` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_FLOAT;

/**
 * Constant struct of 64 bit float `double` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_DOUBLE;

/**
 * Constant struct of signed character `char` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_CHAR;

/**
 * Constant struct of `void` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_VOID;

/**
 * Constant struct of `ptrdiff_t` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_PTRDIFF;

/**
 * Constant struct of `size_t` type.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_SIZE;

/**
 * Constant struct of opaque type.
 *
 * IMPORTANT: Opaque types are kinda of hack for this library.
 * Any opaque type encountered by the analyzer is "trusted". Meaning
 * it would satisfy type checker no matter what happens.
 *
 * SINCE: 0.3.0
 */
extern const Cn_Type CN_TYPE_OPAQUE;

/**
 * Makes pointer type that points to `type`.
 *
 * RETURNS: Pointer type struct with `type` as its base type.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type cn_type_make_pointer(const Cn_Type *type);

/**
 * Makes qualified version of `type` with qualifier `flags`.
 *
 * RETURNS: Qualified type struct with `type` as its base type.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type cn_type_make_qualified(Cn_Type_Flags flags, const Cn_Type *type);

/**
 * Checks whether `type1` equals `type2`.
 *
 * RETURNS: `true` if types are identical, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_equals(const Cn_Type *type1, const Cn_Type *type2);

/**
 * Hashes `type`.
 *
 * RETURNS: Hash of the type.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_type_hash(const Cn_Type *type);

/**
 * Converts `type` to string and copies its contents into `buffer`.
 *
 * PARAM identifier: If not empty, inserted as a declarator name into the type, e.g. `int (*f)(void)`.
 *
 * RETURNS: Resulting string, with `NULL` data if `buffer` is too small to hold it.
 *
 * IMPORTANT: `buffer` must be big enough to hold the string, otherwise an error is logged.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_String cn_type_stringify(Cn_String buffer, const Cn_Type *type, Cn_String identifier);

/**
 * Prints `type` to `stderr`, using `cn_type_stringify`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_type_print(const Cn_Type *type);

/**
 * Checks whether `type` is constant.
 *
 * RETURNS: `true` if type is constant, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_is_constant(const Cn_Type *type);

/**
 * Gets unqualified version of `type`.
 *
 * RETURNS: Unqualified version of the type.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type *cn_type_unqualified(const Cn_Type *type);

/**
 * Checks whether `type` is arithmetic: integer or float, including complex.
 *
 * RETURNS: `true` if type is arithmetic, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_is_arithmetic(const Cn_Type *type);

/**
 * Checks whether value of type `from` can be assigned to value of type `to`.
 *
 * RETURNS: `true` if assignment is allowed, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_is_assignable(const Cn_Type *to, const Cn_Type *from);

/**
 * Checks whether types `a` and `b` are structurally compatible.
 *
 * RETURNS: `true` if types are compatible, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_is_compatible(const Cn_Type *a, const Cn_Type *b);

/**
 * Checks whether types `a` and `b` are structurally compatible, ignoring qualifiers.
 *
 * RETURNS: `true` if types are compatible, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_is_compatible_no_qualifiers(const Cn_Type *a, const Cn_Type *b);

/**
 * Picks the type with greater arithmetic rank out of `a` and `b`.
 *
 * RETURNS: Type that has the greater arithmetic rank.
 *
 * IMPORTANT: Expects `a` and `b` to be arithmetic types.
 *
 * NOTE: If either type is `CN_OPAQUE`, opaque type is returned, since its rank is unknown.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type *cn_type_greatest_arithmetic_rank(const Cn_Type *a, const Cn_Type *b);

/**
 * Checks whether `type` is scalar: arithmetic, pointer or `bool`.
 *
 * RETURNS: `true` if type is scalar, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_type_is_scalar(Cn_Type *type);

/**
 * ============================================
 * SECTION: Any
 * ============================================
 */

/**
 * Struct that groups `data` and its `type`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Type *type;
    void *data;
} Cn_Any;

/**
 * Maximum size in bytes that scalar type can be.
 *
 * SINCE: 0.3.0
 */
#define CN_TYPE_SCALAR_MAX_SIZE 16

/**
 * Reads contents of `any`, interpreting them as integer.
 *
 * RETURNS: Integer value of `any`.
 *
 * SINCE: 0.3.0
 */
CNDEF int64_t cn_any_read_int(Cn_Any any);

/**
 * Reads contents of `any`, interpreting them as floating point.
 *
 * RETURNS: Floating point value of `any`.
 *
 * SINCE: 0.3.0
 */
CNDEF double cn_any_read_float(Cn_Any any);

/**
 * Writes integer `value` to contents of `any`, using its type to properly size the integer.
 *
 * RETURNS: Resulting any.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Any cn_any_write_int(Cn_Any any, int64_t value);

/**
 * Writes floating point `value` to contents of `any`, using its type to properly size the floating point.
 *
 * RETURNS: Resulting any.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Any cn_any_write_float(Cn_Any any, double value);

/**
 * Checks whether value of `any` is zero.
 *
 * RETURNS: `true` if `any` is `0`, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_any_is_zero(Cn_Any any);

/**
 * Converts `any` of any scalar type to `double`.
 *
 * RETURNS: Resulting `double` value.
 *
 * NOTE: If `any` is not of a scalar type, it is still read as a raw integer and converted to `double`.
 *
 * SINCE: 0.3.0
 */
CNDEF double cn_any_as_double(Cn_Any any);

/**
 * Safely converts `double` to `int64_t`, used by casts.
 *
 * Rejects `NaN` and out of range values, whose conversion would be undefined.
 *
 * RETURNS: `true` on success, `false` on failure.
 *
 * OUTPUTS: Converted value, written to `out`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_double_to_int64(double d, int64_t *out);

/**
 * Converts already evaluated scalar value `src` to `target` type, used by casts.
 *
 * RETURNS: Any of the converted value on success. `NIL` when target or source isn't a
 * foldable scalar, or when a float to int conversion would be out of range.
 *
 * NOTE: Converted value is stored in the supplied `buffer`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Any cn_any_convert(Cn_Any src, Cn_Type *target, void *buffer);

/**
 * Checks whether `any` is empty, meaning its data and type are both `NULL`.
 *
 * RETURNS: `true` if `any` is empty, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_any_is_empty(Cn_Any any);

/**
 * ============================================
 * SECTION: Scope
 * ============================================
 */

/**
 * Information that each scope has.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t saved_strings_length;
    int64_t binding_idx;
    int64_t label_binding_idx;
    int64_t binding_defined_idx;
} Cn_Scope;

/**
 * Index of the currently active scope.
 *
 * SINCE: 0.3.0
 */
#define CN_SCOPE_STACK_CURRENT_IDX (cn_array_list_length(&cn__tu_data->scope_stack) - 1)

/**
 * Index of the file scope.
 *
 * SINCE: 0.3.0
 */
#define CN_SCOPE_FILE_IDX 0

/**
 * Peeks scope at the top of the scope stack.
 *
 * RETURNS: Scope that sits at the top of the scope stack.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Scope *cn_scope_stack_peek();

/**
 * Pushes new scope to the top of the scope stack.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_scope_stack_push();

/**
 * Pops scope from the top of the scope stack.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_scope_stack_pop();

/**
 * ============================================
 * SECTION: Ast
 * ============================================
 */

// TODO: Make this macro be active only when needed through compilation.
#define CN__TRACE_ERROR
#ifndef CN__TRACE_ERROR
#   define CN__TRACE_ERROR   fprintf(stderr, "   error from '%s()'\n", __FUNCTION__);
#endif // CN__TRACE_ERROR

/**
 * Gets next fresh counter value, incrementing the counter.
 *
 * RETURNS: Counter value before increment.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_counter_next() (cn__tu_data->counter++)

/**
 * Compares types that `type1_ptr` and `type2_ptr` point to, wrapping around `cn_type_equals`.
 *
 * Used in a set that holds pointers to the types,
 * so that actual type data stays at the same memory addresses.
 *
 * RETURNS: `true` if types are identical, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_ast_type_ptr_equals(const Cn_Type **type1_ptr, const Cn_Type **type2_ptr);

/**
 * Hashes type that `type_ptr` points to, wrapping around `cn_type_hash`.
 *
 * Used in a set that holds pointers to the types,
 * so that actual type data stays at the same memory addresses.
 *
 * RETURNS: Hash of the type.
 *
 * SINCE: 0.3.0
 */
CNDEF uint64_t cn_ast_type_ptr_hash(const Cn_Type **type_ptr);

/**
 * Enum to store type qualifier information.
 *
 * When qualifiers are parsed these flags are set on type specifier.
 * 
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_TYPE_QUALIFIER_CONST    = 0x1,
    CN_AST_TYPE_QUALIFIER_RESTRICT = 0x2,
    CN_AST_TYPE_QUALIFIER_VOLATILE = 0x4,
    // TEMPORARY: CN_AST_TYPE_QUALIFIER_ATOMIC        = 0x8,
} Cn_Ast_Qualifier_Flags;

/**
 * Simple helper that maps ast qualifiers to type qualifiers.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Type_Flags cn_ast_qualifier_flags_to_type(Cn_Ast_Qualifier_Flags flags);

/**
 * Enum to store function specifier information.
 * 
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_FUNCTION_SPECIFIER_INLINE = 0x1,
    // TEMPORARY: CN_AST_FUNCTION_SPECIFIER_NORETURN  = 0x2,
} Cn_Ast_Function_Specifier_Flags;

/**
 * Enum to store storage specifier information.
 * 
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_STORAGE_SPECIFIER_STATIC   = 0x1,
    CN_AST_STORAGE_SPECIFIER_EXTERN   = 0x2,
    CN_AST_STORAGE_SPECIFIER_REGISTER = 0x4,
    CN_AST_STORAGE_SPECIFIER_AUTO     = 0x8,
    CN_AST_STORAGE_SPECIFIER_TYPEDEF  = 0x10,
} Cn_Ast_Storage_Specifier_Flags;

/**
 * Enum to store signess on type specifier.
 *
 * NOTE: `CN_AST_TYPE_SIGN_NONE` means that signess was not explicitly specified.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_TYPE_SIGN_NONE     = 0,
    CN_AST_TYPE_SIGN_SIGNED   = 1,
    CN_AST_TYPE_SIGN_UNSIGNED = 2,
} Cn_Ast_Type_Sign;

/**
 * Enum that store width on type specifier.
 *
 * NOTE: `CN_AST_TYPE_WIDTH_NONE` means that width was not explicitly specified.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_TYPE_WIDTH_NONE      = 0,
    CN_AST_TYPE_WIDTH_SHORT     = 1,
    CN_AST_TYPE_WIDTH_LONG      = 2,
    CN_AST_TYPE_WIDTH_LONG_LONG = 3,
} Cn_Ast_Type_Width;

/**
 * Enum that indicates primitive base type on type specifier.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_TYPE_NONE    = 0,
    CN_AST_TYPE_INT     = 1,
    CN_AST_TYPE_CHAR    = 2,
    CN_AST_TYPE_FLOAT   = 3,
    CN_AST_TYPE_DOUBLE  = 4,
    CN_AST_TYPE_BOOL    = 5,
    CN_AST_TYPE_VOID    = 6,
} Cn_Ast_Type_Kind;

#ifdef CN_GNU
/**
 * Qualifiers of GNU `asm` statement.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_GNU_ASM_QUALIFIER_VOLATILE   = 0x1,
    CN_AST_GNU_ASM_QUALIFIER_INLINE     = 0x2,
    CN_AST_GNU_ASM_QUALIFIER_GOTO       = 0x4,
} Cn_Ast_Gnu_Asm_Qualifier_Flags;
#endif // CN_GNU

/**
 * Enum that stores binary operator kinds.
 *
 * ```
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
 * ```
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_BINARY_OP_NONE           = -1,
    CN_AST_BINARY_OP_ARRAY_SUB      = 0,
    CN_AST_BINARY_OP_FUNCTION       = 1,
    CN_AST_BINARY_OP_DOT            = 2,
    CN_AST_BINARY_OP_ARROW          = 3,
    CN_AST_BINARY_OP_MULTIPLICATION = 4,
    CN_AST_BINARY_OP_DIVISION       = 5,
    CN_AST_BINARY_OP_MODULO         = 6,
    CN_AST_BINARY_OP_ADDITION       = 7,
    CN_AST_BINARY_OP_SUBTRACTION    = 8,
    CN_AST_BINARY_OP_LSHIFT         = 9,
    CN_AST_BINARY_OP_RSHIFT         = 10,
    CN_AST_BINARY_OP_LESS           = 11,
    CN_AST_BINARY_OP_LESS_EQ        = 12,
    CN_AST_BINARY_OP_GREATER        = 13,
    CN_AST_BINARY_OP_GREATER_EQ     = 14,
    CN_AST_BINARY_OP_EQ             = 15,
    CN_AST_BINARY_OP_NOT_EQ         = 16,
    CN_AST_BINARY_OP_BIT_AND        = 17,
    CN_AST_BINARY_OP_BIT_XOR        = 18,
    CN_AST_BINARY_OP_BIT_OR         = 19,
    CN_AST_BINARY_OP_AND            = 20,
    CN_AST_BINARY_OP_OR             = 21,
    CN_AST_BINARY_OP_COMMA          = 22,
} Cn_Ast_Binary_Operator_Kind;

/**
 * Struct that defines single binary operator.
 *
 * MEMBER token_identifier: `Cn_Token_Kind` that is associated with the operator.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Binary_Operator_Kind kind;
    Cn_Token_Kind token_identifier;
    int precedence;
} Cn_Ast_Binary_Operator;

/**
 * Checks table of binary operators, compares whether current token of `lexer` is a binary operator.
 *
 * RETURNS: Kind of the binary operator, `CN_AST_BINARY_OP_NONE` if token is not one.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Binary_Operator_Kind cn_ast_is_binary_operator(Cn_Lexer *lexer);

/**
 * Enum that stores unary operator kinds.
 *
 * ```
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
 * ```
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_UNARY_OP_NONE           = -1,
    CN_AST_UNARY_OP_INCREMENT      = 0,
    CN_AST_UNARY_OP_DECREMENT      = 1,
    CN_AST_UNARY_OP_POSITIVE       = 2,
    CN_AST_UNARY_OP_NEGATIVE       = 3,
    CN_AST_UNARY_OP_NOT            = 4,
    CN_AST_UNARY_OP_BIT_NOT        = 5,
    CN_AST_UNARY_OP_DEREF          = 6,
    CN_AST_UNARY_OP_ADDROF         = 7,
} Cn_Ast_Unary_Operator_Kind;

/**
 * Precedence value for all unary operators.
 *
 * SINCE: 0.3.0
 */
#define CN_AST_UNARY_OPERATOR_PRECEDENCE 13

/**
 * Struct that defines single unary operator.
 *
 * MEMBER token_identifier: `Cn_Token_Kind` that is associated with the operator.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Unary_Operator_Kind kind;
    Cn_Token_Kind token_identifier;
} Cn_Ast_Unary_Operator;

/**
 * Checks table of unary operators, compares whether current token of `lexer` is a unary operator.
 *
 * RETURNS: Kind of the unary operator, `CN_AST_UNARY_OP_NONE` if token is not one.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Unary_Operator_Kind cn_ast_is_unary_operator(Cn_Lexer *lexer);

/**
 * Enum that stores assignment operator kinds.
 *
 * ```
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
 * ```
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_ASSIGNMENT_OP_NONE           = -1,
    CN_AST_ASSIGNMENT_OP_ASSIGN         = 0,
    CN_AST_ASSIGNMENT_OP_MULTIPLY       = 1,
    CN_AST_ASSIGNMENT_OP_DIVIDE         = 2,
    CN_AST_ASSIGNMENT_OP_MODULO         = 3,
    CN_AST_ASSIGNMENT_OP_PLUS           = 4,
    CN_AST_ASSIGNMENT_OP_MINUS          = 5,
    CN_AST_ASSIGNMENT_OP_LSHIFT         = 6,
    CN_AST_ASSIGNMENT_OP_RSHIFT         = 7,
    CN_AST_ASSIGNMENT_OP_BIT_AND        = 8,
    CN_AST_ASSIGNMENT_OP_BIT_XOR        = 9,
    CN_AST_ASSIGNMENT_OP_BIT_OR         = 10,
} Cn_Ast_Assignment_Operator_Kind;

/**
 * Precedence value for all assignment operators.
 *
 * SINCE: 0.3.0
 */
#define CN_AST_ASSIGNMENT_OPERATOR_PRECEDENCE 1

/**
 * Struct that defines single assignment operator.
 *
 * MEMBER token_identifier: `Cn_Token_Kind` that is associated with the operator.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Assignment_Operator_Kind kind;
    Cn_Token_Kind token_identifier;
} Cn_Ast_Assignment_Operator;

/**
 * Checks table of assignment operators, compares whether current token of `lexer` is an assignment operator.
 *
 * RETURNS: Kind of the assignment operator, `CN_AST_ASSIGNMENT_OP_NONE` if token is not one.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Assignment_Operator_Kind cn_ast_is_assignment_operator(Cn_Lexer *lexer);

/**
 * Enum that stores postfix operator kinds.
 *
 * ```
 *  postfix_operator
 *          : '++'
 *          | '--'
 *          ;
 * ```
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_POSTFIX_OP_NONE              = -1,
    CN_AST_POSTFIX_OP_INCREMENT         = 0,
    CN_AST_POSTFIX_OP_DECREMENT         = 1,
} Cn_Ast_Postfix_Operator_Kind;

/**
 * Precedence value for all postfix operators.
 *
 * SINCE: 0.3.0
 */
#define CN_AST_POSTFIX_OPERATOR_PRECEDENCE 14

/**
 * Struct that defines single postfix operator.
 *
 * MEMBER token_identifier: `Cn_Token_Kind` that is associated with the operator.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Postfix_Operator_Kind kind;
    Cn_Token_Kind token_identifier;
} Cn_Ast_Postfix_Operator;

/**
 * Checks table of postfix operators, compares whether current token of `lexer` is a postfix operator.
 *
 * RETURNS: Kind of the postfix operator, `CN_AST_POSTFIX_OP_NONE` if token is not one.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Postfix_Operator_Kind cn_ast_is_postfix_operator(Cn_Lexer *lexer);

/**
 * Kinds of AST nodes, each kind has its own `Cn_Ast_...` struct.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_UNKNOWN = 0,

    // Misc.
    CN_AST_CODE,
    CN_AST_TRANSLATION_UNIT,
    CN_AST_EXTERNAL_DECLARATION,
    CN_AST_DECLARATION,
    CN_AST_FUNCTION,

    // Statements.
    CN_AST_BLOCK,
    CN_AST_BLOCK_ITEM,
    CN_AST_IF,
    CN_AST_SWITCH,
    CN_AST_WHILE,
    CN_AST_DO_WHILE,
    CN_AST_FOR,
    CN_AST_LABEL,
    CN_AST_GOTO,
    CN_AST_RETURN,
    CN_AST_BREAK,
    CN_AST_CONTINUE,
    CN_AST_EXPRESSION_STATEMENT,

    // Expressions.
    CN_AST_BINARY,
    CN_AST_ACCESS,
    CN_AST_CALL,
    CN_AST_UNARY,
    CN_AST_CAST,
    CN_AST_COMPOUND,
    CN_AST_SIZEOF,
    CN_AST_TERNARY,
    CN_AST_ASSIGN,
    CN_AST_POSTFIX,
    CN_AST_PRIMARY,

    // Literals.
    CN_AST_IDENTIFIER,
    CN_AST_INTEGER,
    CN_AST_FLOAT,
    CN_AST_CHAR,
    CN_AST_STRING,

    // Declarator and declaration machinary.
    CN_AST_INIT_DECLARATOR,
    CN_AST_INITIALIZER,
    CN_AST_DESIGNATION,
    CN_AST_DESIGNATOR,
    CN_AST_DECLARATOR,
    CN_AST_POINTER,
    CN_AST_DIRECT_DECLARATOR_GROUPED,
    CN_AST_DIRECT_DECLARATOR_ARRAY,
    CN_AST_DIRECT_DECLARATOR_FUNCTION,
    CN_AST_DECLARATION_SPECIFIERS,
    CN_AST_TYPE_SPECIFIER_PRIMITIVE,
    CN_AST_TYPE_SPECIFIER_TYPEDEF,
    CN_AST_TYPE_NAME,
    CN_AST_SPECIFIER_QUALIFIER,
    CN_AST_PARAMETER_DECLARATION,

    // Tag types.
    CN_AST_STRUCT_SPECIFIER,
    CN_AST_UNION_SPECIFIER,
    CN_AST_MEMBER_DECLARATION,
    CN_AST_MEMBER_DECLARATOR,
    CN_AST_ENUM_SPECIFIER,
    CN_AST_ENUMERATOR,

    // Attributes.
    CN_AST_ATTRIBUTE_SPECIFIER,
    CN_AST_ATTRIBUTE,

#ifdef CN_GNU
    CN_AST_GNU_TYPEOF,
    CN_AST_GNU_ASM_STATEMENT,
    CN_AST_GNU_ASM_OPERAND,
    CN_AST_GNU_ASM_LABEL,
#endif // CN_GNU

    CN_AST__END
} Cn_Ast_Kind;


/**
 * Flags of the ast node.
 *
 * Values below `0x100` are shared by every node kind.
 * Values starting from `0x100` are specific to a node kind, so they can overlap between kinds.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_SYNTHETIC                    = 0x1,
#ifdef CN_GNU
    CN_AST_GNU_EXTENSION                = 0x2,
#endif // CN_GNU

    CN_AST_DECLARATOR_IS_ABSTRACT                 = 0x100,

    CN_AST_ACCESS_IS_POINTER                      = 0x100,

    CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC = 0x100,

    CN_AST_ENUM_HAS_DEFINITION                    = 0x100,

    CN_AST_UNION_HAS_DEFINITION                   = 0x100,

    CN_AST_STRUCT_HAS_DEFINITION                  = 0x100,

    CN_AST_DECLARATION_IS_EMPTY                   = 0x100,

    CN_AST_LABEL_IS_CASE                          = 0x100,
    CN_AST_LABEL_IS_DEFAULT                       = 0x200,

#ifdef CN_GNU
    CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU             = 0x100,

    CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED          = 0x100,
#endif // CN_GNU
} Cn_Ast_Flags;

/**
 * Definition of the base members of any ast node.
 *
 * SINCE: 0.3.0
 */
#define CN_AST_BASE_MEMBERS         \
    Cn_Ast_Kind         kind;       \
    Cn_Ast_Flags        flags;      \
    Cn_Location         loc;        \
    Cn_Source           src;        \

/**
 * Generic ast node, every `Cn_Ast_...` struct starts with the same members.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    CN_AST_BASE_MEMBERS
} Cn_Ast_Node;

/**
 * Finalized, immutable-length array of node pointers.
 *
 * Memory stored in `Cn_Tu_Data.ast_arena`, so it is rolled
 * back together with the nodes it points when rollback occurs.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Node **ptrs;
    int64_t       length;
} Cn_Ast_Nodes;

/**
 * Ast node base that is added to each ast node struct definition.
 *
 * SINCE: 0.3.0
 */
#define CN_AST_BASE                         \
    union {                                 \
        Cn_Ast_Node base;                   \
        struct { CN_AST_BASE_MEMBERS };     \
    }

/**
 * Enum that represents kind of member entry of ast node struct.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_AST_MEMBER_INFO_NODE,
    CN_AST_MEMBER_INFO_NODES,
    CN_AST_MEMBER_INFO_STRING,
    CN_AST_MEMBER_INFO_UINT64,
    CN_AST_MEMBER_INFO_INT32,
    CN_AST_MEMBER_INFO_DOUBLE,
    CN_AST_MEMBER_INFO_TYPE,
} Cn_Ast_Member_Info_Kind;

/**
 * Struct definition that represents member entry of ast node struct.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    const char              *name;
    size_t                   offset;
    Cn_Ast_Member_Info_Kind  kind;
} Cn_Ast_Member_Info;

/**
 * This struct is used to store generically meta information for each ast node type.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    const char          *name;
    size_t               size;
    Cn_Ast_Member_Info  *members;
    int64_t              members_count;
    void                *emit_func;
} Cn_Ast_Info;

/**
 * Gets info of any ast node type, based on its kind.
 *
 * IMPORTANT: Expects kind to be a valid AST kind.
 * Expects all ast nodes to have their respective infos.
 * Never returns `NULL` but asserts expectations set above.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Info *cn_ast_info(Cn_Ast_Kind kind);

/**
 * Downcasts generic node pointer `p` to a concrete node type.
 *
 * `T` is the struct name without `Cn_Ast_` prefix, e.g. `cn_ast_as(Function, node)`.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_as(T, p) ((Cn_Ast_##T *)(p))

/**
 * Upcasts concrete node pointer `p` to the generic `Cn_Ast_Node *`.
 *
 * Every `Cn_Ast_...` struct starts with `CN_AST_BASE`, so this is always valid.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_upcast(p) ((Cn_Ast_Node *)(p))

// Definitions of ast structs.

// --- misc --------------------------------------------------------

/**
 * Raw code fragment, usually built by meta programs with `cn_build`, it is emitted as is.
 *
 * MEMBER text: Code text, newlines in it are indented to the current emit level.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_String text;
} Cn_Ast_Code;

/**
 * Root node of the translation unit.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes external_declarations;
} Cn_Ast_Translation_Unit;

/**
 * Single file scope declaration, function definition, stray `;` or GNU file scope `asm`.
 *
 * MEMBER child: `Function`, `Declaration` or `Gnu_Asm_Statement` node, `NULL` for stray `;`.
 *
 * NOTE: With `CN_GNU`, node is flagged with `CN_AST_GNU_EXTENSION` if it is preceded by `__extension__`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Node *child;
} Cn_Ast_External_Declaration;


// --- literals ----------------------------------------------------

/**
 * Identifier, name of a symbol, tag, member or label.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_String name;
} Cn_Ast_Identifier;

/**
 * Integer literal.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    uint64_t value;
} Cn_Ast_Integer;

/**
 * Floating point literal.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    double value;
} Cn_Ast_Float;

/**
 * Character literal.
 *
 * MEMBER unit_count: Number of character units in the literal, more than `1` for multi-character literals.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    uint64_t value;
    int      unit_count;
} Cn_Ast_Char;

/**
 * String literal, adjacent literals are concatenated into one.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_String str;
} Cn_Ast_String;


// --- attributes --------------------------------------------------

/**
 * Attribute specifier, `[[attributes]]`, or with `CN_GNU` also `__attribute__((attributes))`.
 *
 * NOTE: Flagged with `CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU` if it is written in GNU syntax.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes attributes;
} Cn_Ast_Attribute_Specifier;

/**
 * Single attribute, `vendor::identifier(arguments)`.
 *
 * MEMBER vendor_identifier: Attribute namespace, `NULL` if not specified, always `NULL` in GNU syntax.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Identifier *vendor_identifier;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Nodes       arguments;
} Cn_Ast_Attribute;

#ifdef CN_GNU

/**
 * GNU `typeof` type specifier, `typeof(type_name)` or `typeof(expression)`.
 *
 * MEMBER target: `Type_Name` node or expression.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Node *target;
} Cn_Ast_Gnu_Typeof;

/**
 * GNU `asm` statement, at block scope or at file scope.
 *
 * MEMBER template: Assembler template string.
 *
 * MEMBER outputs: Output `Gnu_Asm_Operand` nodes.
 *
 * MEMBER inputs: Input `Gnu_Asm_Operand` nodes.
 *
 * MEMBER clobbers: Clobber `String` nodes.
 *
 * MEMBER goto_labels: `Identifier` nodes of `asm goto` labels.
 *
 * NOTE: Flagged with `CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED` if it has any `:` section,
 * otherwise it is basic asm.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Gnu_Asm_Qualifier_Flags  qualifiers;
    Cn_Ast_String                  *template;
    Cn_Ast_Nodes                    outputs;
    Cn_Ast_Nodes                    inputs;
    Cn_Ast_Nodes                    clobbers;
    Cn_Ast_Nodes                    goto_labels;
} Cn_Ast_Gnu_Asm_Statement;

/**
 * Single operand of GNU `asm` statement, `[identifier] string (expression)`.
 *
 * MEMBER identifier: Symbolic name, referenced from template as `%[name]`, `NULL` if not specified.
 *
 * MEMBER string: Constraint, e.g. `"=r"`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_String     *string;
    Cn_Ast_Node       *expression;
} Cn_Ast_Gnu_Asm_Operand;

/**
 * GNU `asm("name")` label of a declarator, sets assembler name of the declared symbol.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_String *string;
} Cn_Ast_Gnu_Asm_Label;

#endif // CN_GNU

// --- declarator and declaration machinary ------------------------

/**
 * Single `*` of a declarator with its qualifiers, chained to the next `*`.
 *
 * MEMBER attribute_specifiers: GNU attribute specifiers among the qualifiers.
 *
 * MEMBER pointer: Next `*`, `NULL` if this is the last one.
 *
 * SINCE: 0.3.0
 */
typedef struct cn_ast_pointer { CN_AST_BASE;
    Cn_Ast_Qualifier_Flags  qualifiers;
#ifdef CN_GNU
    Cn_Ast_Nodes            attribute_specifiers;
#endif // CN_GNU
    struct cn_ast_pointer  *pointer;
} Cn_Ast_Pointer;

/**
 * Declarator, optional pointer followed by a direct declarator.
 *
 * MEMBER pointer: `NULL` if declarator has no pointer.
 *
 * MEMBER direct_declarator: `Identifier`, `Direct_Declarator_Grouped`, `Direct_Declarator_Array` or
 * `Direct_Declarator_Function` node, `NULL` for an abstract declarator without one.
 *
 * NOTE: Flagged with `CN_AST_DECLARATOR_IS_ABSTRACT` if it declares no name.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Pointer *pointer;
    Cn_Ast_Node    *direct_declarator;
} Cn_Ast_Declarator;

/**
 * Parenthesized declarator, `(declarator)`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Declarator *declarator;
} Cn_Ast_Direct_Declarator_Grouped;

/**
 * Array declarator, `direct_declarator[expression]`.
 *
 * MEMBER expression: Array size, `NULL` if size is not specified.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Node *direct_declarator;
    Cn_Ast_Node *expression;
} Cn_Ast_Direct_Declarator_Array;

/**
 * Function declarator, `direct_declarator(parameter_declarations)`.
 *
 * NOTE: Flagged with `CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC` if parameters end with `...`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Node  *direct_declarator;
    Cn_Ast_Nodes  parameter_declarations;
} Cn_Ast_Direct_Declarator_Function;

/**
 * Initializer, either a single expression or a braced list.
 *
 * MEMBER expression: Initializing expression, `NULL` for a braced list.
 *
 * MEMBER designations: `Designation` nodes of a braced list.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Node  *expression;
    Cn_Ast_Nodes  designations;
} Cn_Ast_Initializer;

/**
 * Single element of a braced initializer list, `designators = initializer`.
 *
 * MEMBER designators: `Designator` nodes, empty if element has no designation.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes        designators;
    Cn_Ast_Initializer *initializer;
} Cn_Ast_Designation;

/**
 * Single designator, `.identifier`, `[expression]` or GNU range `[expression ... expression_range_end]`.
 *
 * MEMBER identifier: Member name, `NULL` for an index designator.
 *
 * MEMBER expression: Index, `NULL` for a member designator.
 *
 * MEMBER expression_range_end: Last index of a range designator, `NULL` otherwise.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Node       *expression;
    Cn_Ast_Node       *expression_range_end;
} Cn_Ast_Designator;

/**
 * Single declarator of a declaration, with its optional initializer.
 *
 * MEMBER initializer: `NULL` if there is no initializer.
 *
 * MEMBER asm_label: GNU `asm("name")` label, `NULL` if there is none.
 *
 * MEMBER attribute_specifiers: GNU attribute specifiers that follow the declarator.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Declarator    *declarator;
#ifdef CN_GNU
    Cn_Ast_Gnu_Asm_Label *asm_label;
    Cn_Ast_Nodes          attribute_specifiers;
#endif // CN_GNU
    Cn_Ast_Initializer   *initializer;
} Cn_Ast_Init_Declarator;

/**
 * Primitive type specifier, combination of keywords like `unsigned long int`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Type_Kind  primitive_kind;
    Cn_Ast_Type_Width width;
    Cn_Ast_Type_Sign  sign;
} Cn_Ast_Type_Specifier_Primitive;

/**
 * Type specifier that names a `typedef`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_String typedef_name;
} Cn_Ast_Type_Specifier_Typedef;

/**
 * Specifier qualifier list, qualifiers and a type specifier.
 *
 * MEMBER attribute_specifiers: GNU attribute specifiers among the qualifiers.
 *
 * MEMBER type_specifier: Any type specifier node.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Qualifier_Flags  qualifiers;
#ifdef CN_GNU
    Cn_Ast_Nodes            attribute_specifiers;
#endif // CN_GNU
    Cn_Ast_Node            *type_specifier;
} Cn_Ast_Specifier_Qualifier;

/**
 * Type name, used by casts, `sizeof` and compound literals.
 *
 * MEMBER abstract_declarator: `NULL` if type name has no declarator.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Specifier_Qualifier *specifier_qualifier;
    Cn_Ast_Declarator          *abstract_declarator;
} Cn_Ast_Type_Name;

/**
 * Declaration specifiers, storage class, qualifiers, function specifiers and a type specifier.
 *
 * MEMBER attribute_specifiers: GNU attribute specifiers among the specifiers.
 *
 * MEMBER type_specifier: Any type specifier node.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Storage_Specifier_Flags   storage_specifiers;
    Cn_Ast_Qualifier_Flags           qualifiers;
    Cn_Ast_Function_Specifier_Flags  function_specifiers;
#ifdef CN_GNU
    Cn_Ast_Nodes                     attribute_specifiers;
#endif // CN_GNU
    Cn_Ast_Node                     *type_specifier;
} Cn_Ast_Declaration_Specifiers;

/**
 * Single parameter of a function declarator.
 *
 * MEMBER declarator: Named or abstract declarator, `NULL` if parameter has none.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Declaration_Specifiers *declaration_specifiers;
    Cn_Ast_Declarator             *declarator;
} Cn_Ast_Parameter_Declaration;


// --- tag types ---------------------------------------------------

/**
 * `struct` specifier, a definition or a reference to a tag.
 *
 * MEMBER attribute_specifiers: Attribute specifiers right after the `struct` keyword.
 *
 * MEMBER identifier: Tag name, `NULL` for an anonymous struct.
 *
 * NOTE: Flagged with `CN_AST_STRUCT_HAS_DEFINITION` if it has a member list.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes       attribute_specifiers;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Nodes       member_declarations;
} Cn_Ast_Struct_Specifier;

/**
 * `union` specifier, a definition or a reference to a tag.
 *
 * MEMBER attribute_specifiers: Attribute specifiers right after the `union` keyword.
 *
 * MEMBER identifier: Tag name, `NULL` for an anonymous union.
 *
 * NOTE: Flagged with `CN_AST_UNION_HAS_DEFINITION` if it has a member list.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes       attribute_specifiers;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Nodes       member_declarations;
} Cn_Ast_Union_Specifier;

/**
 * Member declaration of a struct or union, `int a, b : 3;`.
 *
 * NOTE: With `CN_GNU`, flagged with `CN_AST_GNU_EXTENSION` if it is preceded by `__extension__`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes                attribute_specifiers;
    Cn_Ast_Specifier_Qualifier *specifier_qualifier;
    Cn_Ast_Nodes                member_declarators;
} Cn_Ast_Member_Declaration;

/**
 * Single declarator of a member declaration.
 *
 * MEMBER declarator: `NULL` for an unnamed bit-field.
 *
 * MEMBER bitfield: Bit-field width, `NULL` if member is not a bit-field.
 *
 * MEMBER attribute_specifiers: GNU attribute specifiers that follow the declarator.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Declarator *declarator;
    Cn_Ast_Node       *bitfield;
#ifdef CN_GNU
    Cn_Ast_Nodes       attribute_specifiers;
#endif // CN_GNU
} Cn_Ast_Member_Declarator;

/**
 * `enum` specifier, a definition or a reference to a tag.
 *
 * MEMBER attribute_specifiers: Attribute specifiers right after the `enum` keyword.
 *
 * MEMBER identifier: Tag name, `NULL` for an anonymous enum.
 *
 * MEMBER specifier_qualifier: Fixed underlying type, `NULL` if not specified.
 *
 * NOTE: Flagged with `CN_AST_ENUM_HAS_DEFINITION` if it has an enumerator list.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes                attribute_specifiers;
    Cn_Ast_Identifier          *identifier;
    Cn_Ast_Specifier_Qualifier *specifier_qualifier;
    Cn_Ast_Nodes                enumerators;
} Cn_Ast_Enum_Specifier;

/**
 * Single enumerator of an enum, `identifier = expression`.
 *
 * MEMBER expression: Explicit value, `NULL` if not specified.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Nodes       attribute_specifiers;
    Cn_Ast_Node       *expression;
} Cn_Ast_Enumerator;


// --- expressions -------------------------------------------------
// Every expression node stores its resulting `type`, set when the expression is typechecked.
// With `CN_GNU`, expression is flagged with `CN_AST_GNU_EXTENSION` if it is preceded by `__extension__`.

/**
 * Binary expression, `left operator right`, including the comma operator.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type                     *type;
    Cn_Ast_Node                 *left;
    Cn_Ast_Binary_Operator_Kind  operator;
    Cn_Ast_Node                 *right;
} Cn_Ast_Binary;

/**
 * Member access, `expression.member` or `expression->member`.
 *
 * NOTE: Flagged with `CN_AST_ACCESS_IS_POINTER` for `->`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type           *type;
    Cn_Ast_Node       *expression;
    Cn_Ast_Identifier *member;
} Cn_Ast_Access;

/**
 * Function call, `expression(arguments)`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type      *type;
    Cn_Ast_Node  *expression;
    Cn_Ast_Nodes  arguments;
} Cn_Ast_Call;

/**
 * Prefix unary expression, `operator expression`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type                    *type;
    Cn_Ast_Unary_Operator_Kind  operator;
    Cn_Ast_Node                *expression;
} Cn_Ast_Unary;

/**
 * Cast expression, `(type_name) expression`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type          *type;
    Cn_Ast_Type_Name *type_name;
    Cn_Ast_Node      *expression;
} Cn_Ast_Cast;

/**
 * Compound literal, `(type_name) { designations }`.
 *
 * MEMBER designations: `Designation` nodes of the initializer list.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type          *type;
    Cn_Ast_Type_Name *type_name;
    Cn_Ast_Nodes      designations;
} Cn_Ast_Compound;

/**
 * `sizeof` expression.
 *
 * MEMBER target: `Type_Name` node or expression.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type     *type;
    Cn_Ast_Node *target;
} Cn_Ast_Sizeof;

/**
 * Conditional expression, `condition ? if_true : if_false`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type     *type;
    Cn_Ast_Node *condition;
    Cn_Ast_Node *if_true;
    Cn_Ast_Node *if_false;
} Cn_Ast_Ternary;

/**
 * Assignment expression, `left operator right`, where `operator` is `=` or compound assignment.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type                         *type;
    Cn_Ast_Node                     *left;
    Cn_Ast_Assignment_Operator_Kind  operator;
    Cn_Ast_Node                     *right;
} Cn_Ast_Assign;

/**
 * Postfix expression, `expression++` or `expression--`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type                      *type;
    Cn_Ast_Node                  *expression;
    Cn_Ast_Postfix_Operator_Kind  operator;
} Cn_Ast_Postfix;

/**
 * Primary expression, wraps a single literal so it can be typed.
 *
 * MEMBER literal: `Identifier`, `Integer`, `Float`, `Char` or `String` node.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Type     *type;
    Cn_Ast_Node *literal;
} Cn_Ast_Primary;


// --- statements --------------------------------------------------

/**
 * Compound statement, `{ ... }`, that opens a new scope.
 *
 * MEMBER block_items: `Block_Item` nodes, in order.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes attribute_specifiers;
    Cn_Ast_Nodes block_items;
} Cn_Ast_Block;

/**
 * Single item of a block.
 *
 * MEMBER declaration_or_statement: `Declaration` node or any statement node.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Node *declaration_or_statement;
} Cn_Ast_Block_Item;

/**
 * `if` statement.
 *
 * MEMBER else_statement: `NULL` if there is no `else` branch.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *condition;
    Cn_Ast_Node  *then_statement;
    Cn_Ast_Node  *else_statement;
} Cn_Ast_If;

/**
 * `switch` statement, its `case` and `default` labels are `Label` nodes inside of `body`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *condition;
    Cn_Ast_Node  *body;
} Cn_Ast_Switch;

/**
 * `while` statement.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *condition;
    Cn_Ast_Node  *body;
} Cn_Ast_While;

/**
 * `do ... while` statement.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *condition;
    Cn_Ast_Node  *body;
} Cn_Ast_Do_While;

/**
 * `for` statement, each of its clauses is `NULL` if omitted.
 *
 * MEMBER initialization: `Declaration` node or expression.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *initialization;
    Cn_Ast_Node  *condition;
    Cn_Ast_Node  *update;
    Cn_Ast_Node  *body;
} Cn_Ast_For;

/**
 * Labeled statement: named label, `case` or `default`.
 *
 * MEMBER identifier: Label name, `NULL` for `case` and `default`.
 *
 * MEMBER expression: Constant expression of `case`, `NULL` otherwise.
 *
 * NOTE: Flagged with `CN_AST_LABEL_IS_CASE` or `CN_AST_LABEL_IS_DEFAULT`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes       attribute_specifiers;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Node       *expression;
    Cn_Ast_Node       *statement;
} Cn_Ast_Label;

/**
 * `goto` statement.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes       attribute_specifiers;
    Cn_Ast_Identifier *identifier;
} Cn_Ast_Goto;

/**
 * `return` statement.
 *
 * MEMBER expression: `NULL` if no value is returned.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *expression;
} Cn_Ast_Return;

/**
 * `break` statement.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes attribute_specifiers;
} Cn_Ast_Break;

/**
 * `continue` statement.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes attribute_specifiers;
} Cn_Ast_Continue;

/**
 * Expression followed by `;`.
 *
 * MEMBER expression: `NULL` for the empty statement `;`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes  attribute_specifiers;
    Cn_Ast_Node  *expression;
} Cn_Ast_Expression_Statement;


// --- declarations ------------------------------------------------

/**
 * Declaration of zero or more names, `int a, *b = 0;`.
 *
 * NOTE: Flagged with `CN_AST_DECLARATION_IS_EMPTY` if it declares no names, e.g. `struct S { int x; };`.
 * With `CN_GNU`, flagged with `CN_AST_GNU_EXTENSION` if it is preceded by `__extension__`.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes                   attribute_specifiers;
    Cn_Ast_Declaration_Specifiers *declaration_specifiers;
    Cn_Ast_Nodes                   init_declarators;
} Cn_Ast_Declaration;

/**
 * Function definition, a declaration followed by its body.
 *
 * SINCE: 0.3.0
 */
typedef struct { CN_AST_BASE;
    Cn_Ast_Nodes                   attribute_specifiers;
    Cn_Ast_Declaration_Specifiers *declaration_specifiers;
    Cn_Ast_Declarator             *declarator;
    Cn_Ast_Block                  *block;
} Cn_Ast_Function;


/**
 * Allocates a copy of any `Cn_Ast_...` struct in the ast arena.
 *
 * Only `sizeof(node)` bytes are allocated, so small nodes stay small.
 *
 * RETURNS: `void *`, so the result can be assigned to any concrete `Cn_Ast_...` pointer
 * without a cast.
 *
 * NOTE: Variadic, so a compound literal with commas can be passed directly.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_new(...) cn__ast_new(&(__VA_ARGS__.base), sizeof(__VA_ARGS__))
CNDEF void *cn__ast_new(Cn_Ast_Node *node, size_t size);

/**
 * Recursively prints ast tree of `idx` to `stdout`.
 *
 * PARAM depth: Use `0` for starting value.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_ast_print(void *node, int depth);

/**
 * Gets enum name of the node `kind`, used in messages.
 *
 * RETURNS: Static string with the enum name of the node kind.
 *
 * SINCE: 0.3.0
 */
CNDEF const char *cn_ast_node_kind_name(Cn_Ast_Kind kind);

/**
 * Gets size of the concrete node struct for the given `kind`.
 *
 * Nodes are allocated at their own size, so anything that copies or moves a
 * node generically needs this.
 *
 * RETURNS: Size in bytes of the concrete `Cn_Ast_...` struct.
 *
 * SINCE: 0.3.0
 */
CNDEF size_t cn_ast_node_size(Cn_Ast_Kind kind);

/**
 * Checks whether `node` is primary: identifier, integer, float or string.
 *
 * RETURNS: `true` if node is primary, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_ast_is_primary(void * node);

/**
 * Gets current `ptr_stack` length, used to mark start of a list.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_stack_mark() cn_array_list_length(&cn__tu_data->ptr_stack)

/**
 * Pushes `ptr` onto the `ptr_stack`.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_stack_push(ptr) cn_array_list_append(&cn__tu_data->ptr_stack, cn_ast_upcast(ptr))

/**
 * Inserts `ptr` into the `ptr_stack` at `mark`.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_stack_prepend(ptr, mark) cn_array_list_add(&cn__tu_data->ptr_stack, (mark), cn_ast_upcast(ptr))

/**
 * Discards all pointers added after `mark`, by returning the `ptr_stack` to it.
 *
 * SINCE: 0.3.0
 */
#define cn_ast_stack_discard(mark) cn_array_list_pop_multiple(&cn__tu_data->ptr_stack, cn_array_list_length(&cn__tu_data->ptr_stack) - (mark))

/**
 * Finalizes range of the `ptr_stack`, from `mark` to the end, into `Cn_Ast_Nodes`.
 *
 * Finalized pointers are popped from the stack.
 *
 * RETURNS: Finalized list, allocated in the ast arena. Empty list if range is empty.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_ast_stack_finalize(int64_t mark);

/**
 * Removes entry at `index` from `list`, keeping order of the remaining entries.
 *
 * Doesn't reallocate memory, removing happens in place.
 *
 * RETURNS: Removed node.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_ast_remove_from_list(Cn_Ast_Nodes *list, int64_t index);

/**
 * Deep copies `list`, allocating memory for every node branch.
 *
 * RETURNS: Copied list.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_ast_copy_list(Cn_Ast_Nodes list);

/**
 * Deep copies `node`, allocating memory for the whole branch.
 *
 * RETURNS: Copied node.
 *
 * SINCE: 0.3.0
 */
CNDEF void *cn_ast_copy(void *node);

/**
 * Removes attribute `attribute_name` from attribute list of `target`.
 *
 * RETURNS: `true` if attribute was found and removed, `false` otherwise.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_ast_remove_attribute(void *target, Cn_String attribute_name);

/**
 * ============================================
 * SECTION: Get
 * ============================================
 */

/**
 * Traverses declarator nodes to get declarator identifier.
 *
 * OUTPUTS: Last declarator in chain, written to `last_declarator`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_get_declarator_info(Cn_Ast_Declarator *declarator, Cn_Ast_Node **last_declarator);

/**
 * Traverses declarator nodes to get function declarator identifier and other information.
 *
 * RETURNS: `true` on success, `false` if declarator is not a proper function.
 *
 * OUTPUTS: Parameter type list of the function, written to `function_direct_declarator` if it is not `NULL`.
 *
 * OUTPUTS: Last declarator in chain, written to `last_declarator`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_get_function_declarator_info(Cn_Ast_Declarator *declarator, Cn_Ast_Node **function_direct_declarator, Cn_Ast_Node **last_declarator);

/**
 * Finds attribute `attribute_name` on `target` node.
 *
 * RETURNS: Found attribute, `NULL` if there is none.
 *
 * OUTPUTS: Attribute specifier holding the attribute, written to `owner` if it is not `NULL`.
 *
 * OUTPUTS: Position of the attribute in the list of that specifier, written to `index` if it is not `NULL`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Attribute *cn_get_attribute_info(void *target, Cn_String attribute_name, Cn_Ast_Attribute_Specifier **owner, int64_t *index);

/**
 * Finds attribute `attribute_name` on `target` node.
 *
 * RETURNS: Found attribute, `NULL` if there is none.
 *
 * NOTE: If there are multiple attributes with the same name, the first one is found.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Attribute *cn_get_attribute(void *target, Cn_String attribute_name);

/**
 * Gets resulting type of `expression`.
 *
 * Type is read from the first `CN_AST_MEMBER_INFO_TYPE` member of the node, so custom
 * expression nodes are supported too, as long as they declare such member.
 *
 * RETURNS: Valid type on success, `NULL` if expression wasn't typechecked.
 *
 * IMPORTANT: Expects `expression != NULL`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type *cn_get_expression_type(void *expression);

/**
 * ============================================
 * SECTION: Diagnostics
 * ============================================
 */

/**
 * Enum for each possible diagnostic code.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_DC_ZERO = 0,
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

/**
 * Array that maps `Cn_Diagnostic_Code` to diagnostic code messages.
 *
 * SINCE: 0.3.0
 */
extern const Cn_String CN_DIAGNOSTIC_CODES[CN__DC_COUNT];

/**
 * Enum for diagnostic level that is given to each diagnostic message. 
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_DIAGNOSTIC_INFO    = 0,
    CN_DIAGNOSTIC_WARNING = 1,
    CN_DIAGNOSTIC_ERROR   = 2,
} Cn_Diagnostic_Level;

/**
 * Diagnostics with a level below `cn_min_diagnostic_level` are suppressed by `cn_default_log_handler`.
 *
 * Works the same as: `cn_log_min_level`.
 *
 * SINCE: 0.3.0
 */
extern Cn_Diagnostic_Level cn_min_diagnostic_level;

/**
 * Represents region in the diagnostic snippet that is highlighted.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t offset;
    int64_t length;
} Cn_Diagnostic_Annotation;

/**
 * Type definition of diagnostic handler, `cn_diagnostic_handler` is declared with this type.
 *
 * SINCE: 0.3.0
 */
typedef void (Cn_Diagnostic_Handler)(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, Cn_Diagnostic_Annotation annotations[], size_t annotations_length, const char *format, va_list args);

/**
 * Current diagnostic handler, set to `cn_default_diagnostic_handler` by default.
 *
 * SINCE: 0.3.0
 */
extern Cn_Diagnostic_Handler *cn_diagnostic_handler;

/**
 * Prints formatted diagnostic with code snippet to `stderr`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Diagnostic_Handler cn_default_diagnostic_handler;

/**
 * Ignores all incoming diagnostics, doesn't output anything anywhere.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Diagnostic_Handler cn_null_diagnostic_handler;

/**
 * Passes formatted diagnostic message to the current diagnostic handler.
 *
 * Uses `loc` and `src` to directly specify location and code source for the diagnostic.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_diagnostic_src(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Source *src, Cn_Diagnostic_Code code, const char *format, ...);

/**
 * Reports diagnostic using current diagnostic handler per node.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_diagnostic_node(Cn_Diagnostic_Level level, void *node, Cn_Diagnostic_Code code, const char *format, ...);


/**
 * ============================================
 * SECTION: Binding
 * ============================================
 */

/**
 * Type definition for index type used for binding's intrusive list.
 *
 * SINCE: 0.3.0
 */
typedef uint32_t Cn_Binding_Idx;

/**
 * `NIL` binding idx value.
 *
 * SINCE: 0.3.0
 */
#define CN_BINDING_NIL_IDX 0

/**
 * Enum defining bindings' kinds.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_BINDING_TAG,
    CN_BINDING_VARIABLE,
    CN_BINDING_FUCNTION,
    CN_BINDING_TYPEDEF,
    CN_BINDING_ENUM_CONSTANT,
    CN_BINDING_LABEL,
    CN_BINDING_BUILTIN,
} Cn_Binding_Kind;

/**
 * Variable binding struct definition.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Storage_Specifier_Flags storage_flags;
    bool is_definition;
} Cn_Binding_Variable;

/**
 * Function binding struct definition.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Ast_Storage_Specifier_Flags storage_flags;
    Cn_Ast_Function_Specifier_Flags function_flags;
    Cn_String *parameter_names;
    bool is_definition;
} Cn_Binding_Function;

/**
 * Enum constant binding struct definition.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t value; 
} Cn_Binding_Enum_Constant;

/**
 * Struct definition of binding.
 *
 * MEMBER kind: Kind of the binding, different kinds can be stored and modified differently.
 *
 * MEMBER name: Name of the binding, a single keyword that identifies it.
 * Depending on the kind of the binding, it could be a variable name, typedef name or tag name.
 *
 * MEMBER src: Ast node the binding is coming from, usually an identifier.
 *
 * MEMBER type: Resolved type of the binding.
 *
 * MEMBER next_idx: Next binding in the linked list of bindings with the same name.
 * Each binding can shadow a previous binding, in such case
 * the most recent binding is at the head of the linked list.
 *
 * MEMBER scope_idx: Index of the scope in which this binding was added.
 * Binding must NOT be used if its scope no longer exists.
 *
 * NOTE: `b_variable`, `b_function` and `b_enum_constant` contain additional data
 * per binding kind---storage flags, parameter names, whether it is a definition or not,
 * and so on.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Binding_Kind kind;           
    Cn_String       name;
    Cn_Ast_Node    *src;
    Cn_Type        *type;
    Cn_Binding_Idx  next_idx;        
    int64_t         scope_idx;                  
    union {
        Cn_Binding_Variable         b_variable;
        Cn_Binding_Function         b_function;
        Cn_Binding_Enum_Constant    b_enum_constant;
    };
} Cn_Binding;

/**
 * Gets binding based on its `idx`.
 *
 * SINCE: 0.3.0
 */
#define cn_binding_get(idx)    (cn__tu_data->binding_list + (idx))

/**
 * Adds `binding` to the binding list, and puts its index into `binding_table` under its name.
 *
 * RETURNS: Index of the added binding.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_table_put(Cn_Binding binding, Cn_Binding_Idx **binding_table);
    
/**
 * Looks up binding `name` in `binding_table`.
 *
 * RETURNS: Index of the found binding, `CN_BINDING_NIL_IDX` if there is none.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_table_get(Cn_String name, Cn_Binding_Idx **binding_table);

/**
 * Adds new function binding to the current scope.
 *
 * If same binding already exists, handles redeclaration and redefinition of the binding.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_function(Cn_String name, void *source, Cn_Ast_Storage_Specifier_Flags storage_flags, Cn_Ast_Function_Specifier_Flags function_flags, Cn_Type *type, Cn_Ast_Nodes *parameter_declarations, bool is_definition);

/**
 * Adds new variable binding to the current scope.
 *
 * If same binding already exists, handles redeclaration and redefinition of the binding.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_variable(Cn_String name, void *source, Cn_Ast_Storage_Specifier_Flags storage_flags, Cn_Type *type, bool is_definition);

/**
 * Adds new typedef binding to the current scope.
 *
 * If same binding already exists, handles redeclaration and redefinition of the binding.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_typedef(Cn_String name, void *source, Cn_Type *type);

/**
 * Adds new enum constant binding to the current scope.
 *
 * If same binding already exists, handles redeclaration and redefinition of the binding.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_enum_constant(Cn_String name, void *source, Cn_Type *type, int64_t value);

/**
 * Declares new tag binding in the current scope.
 *
 * If same binding already exists, handles redeclaration:
 *
 * - If binding exists and is complete in any scope, including current scope,
 * that binding is returned.
 *
 * - If binding exists but is not complete in a different scope,
 * new binding is declared in current scope.
 *
 * - If binding exists but is not complete in current scope,
 * that declaration is referenced.
 *
 * - If binding doesn't exist, new binding is declared in current scope.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_tag(Cn_String tag, Cn_Type_Kind kind);

/**
 * Declares new label binding in the current function scope.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_label(Cn_String name, void *source);

/**
 * Adds new builtin binding to the current scope.
 *
 * If same binding already exists, errors.
 *
 * PARAM source: Only used to print error diagnostics of the source location.
 *
 * RETURNS: Binding index on success, `NIL` if an error occurred.
 *
 * NOTE: Builtin's don't have source location, 
 * they are names that are only binded to `CN_OPAQUE` type.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Binding_Idx cn_binding_declare_builtin(Cn_String name, void *source);

/**
 * Binds parameters of function definition `function_binding_idx` as arguments in the current scope.
 *
 * RETURNS: `true` on success, `false` if any errors occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_binding_declare_function_arguments(Cn_Binding_Idx function_binding_idx);

/**
 * ============================================
 * SECTION: Hook
 * ============================================
 */

/**
 * Response flags, they are returned by every hook function.
 *
 * They are used to communicate on how processing should 
 * continue after hook's execution.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_HOOK_RESPONSE_NONE     = 0,
    CN_HOOK_RESPONSE_ERROR    = 0x1,
} Cn_Hook_Response;    

/**
 * Type definition of classic meta-programming hook, it is called after code 
 * was parsed into AST. 
 *
 * It guarantees that given AST is typechecked and is correct.
 * It allows any modification of AST, those modifications will be reparsed
 * sending proper diagnostics if errors occured.
 *
 * IMPORTANT: Each registered hook will be called at least once.
 *
 * OUTPUTS: `true` to `ast_is_modified` if AST modifications occured.
 *
 * PARAM first: Is `true` if and only if this is the first call of this specific hook.
 *
 * NOTE: The reason why `node_ptr` is `Cn_Ast_Node **` is because the hook
 * can completely delete the AST node by setting `*node_ptr = NULL`.
 * Essentially `*node_ptr` value is what being utilized by the library,
 * meaning it can point to a completely different AST and library will
 * utilize that new node.
 *
 * SINCE: 0.3.0
 */
typedef Cn_Hook_Response (Cn_Hook_Ast_Parsed)(Cn_Ast_Node **node_ptr, bool *ast_is_modified, bool first);

/**
 * Meta-programming hook that is called right before parsing of translation unit starts.
 *
 * It guarantees that given `*null_ptr == NULL` is `true`.
 * It allows prepending of AST into translation unit, by setting `*null_ptr` to whatever AST node needed.
 * That AST is reparsed as external declarations, see `cn_parse_external_declaration`.
 *
 * IMPORTANT: Each such hook registered is called only once.
 *
 * SINCE: 0.3.0
 */
typedef Cn_Hook_Response (Cn_Hook_Tu_Start)(Cn_Ast_Node **null_ptr);

/**
 * Meta programming hook that is called right after parsing of translation unit ends.
 *
 * Exaclty the same as `Cn_Hook_Tu_Start`, it guarantees that given `*null_ptr == NULL` is `true`.
 * Difference is that it allows appending of AST into translation unit instead of prepending, 
 * by setting `*null_ptr` to whatever AST node needed. That AST is reparsed as external declarations, 
 * see `cn_parse_external_declaration`.
 *
 * IMPORTANT: Each such hook registered is called only once.
 *
 * SINCE: 0.3.0
 */
typedef Cn_Hook_Response (Cn_Hook_Tu_End)(Cn_Ast_Node **null_ptr);

/**
 * Analyze hook that is called whenever declarator was analyzed to type.
 * 
 * IMPORTANT: It is not allowed to perfom AST modifications in here.
 * But modification of the analyzed type are expected.
 *
 * SINCE: 0.3.0
 */
typedef Cn_Hook_Response (Cn_Hook_Analyze_Declarator)(const Cn_Ast_Node *declarator, Cn_Type **type_ptr);

/**
 * Analyze hook that lays out tag `type` defined by `tag_specifier`: `struct`, `union` or `enum` specifier.
 *
 * Called once types and names of all members are resolved into `type`, while it is still incomplete.
 * Hook lays out the type by filling its `size`, `align` and offsets of struct members, and then marks it
 * with `CN_TYPE_COMPLETE`. Usually done by calling `cn_analyze_layout_default` with custom options.
 *
 * Member at index `i` of the type comes from the `i`-th member declarator of `tag_specifier`, in declaration
 * order, so member specific attributes can be found on those declarators.
 *
 * OUTPUTS: Laid out `type`, marked with `CN_TYPE_COMPLETE`. Left incomplete if not handled by the hook.
 *
 * SINCE: 0.3.0
 */
typedef Cn_Hook_Response (Cn_Hook_Analyze_Layout)(const Cn_Ast_Node *tag_specifier, Cn_Type *type);

/**
 * Struct of dynamic arrays of hooks.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Hook_Ast_Parsed         **ast_parsed_hooks;
    Cn_Hook_Tu_Start           **tu_start_hooks;
    Cn_Hook_Tu_End             **tu_end_hooks;  
    Cn_Hook_Analyze_Declarator **analyze_declarator_hooks;
    Cn_Hook_Analyze_Layout     **analyze_layout_hooks;
} Cn_Hooks;

/**
 * Stores all hooks used during processing.
 *
 * SINCE: 0.3.0
 */
extern Cn_Hooks cn_hooks;

/**
 * Type definition for any function that extends hooks.
 *
 * IMPORTANT: Should return `true` on success, `false` on error.
 *
 * SINCE: 0.3.0
 */
typedef bool (Cn_Extension)(Cn_Hooks *);

/**
 * Registers all supplied extensions.
 *
 * IMPORTANT: Must be called only once, all memory allocated inside this function is expected
 * to live throughout the program`s life time, no freeing is required.
 *
 * RETURNS: `true` on success, `false` on error.
 *
 * SINCE: 0.3.0
 */
#define cn_extension_register(...) cn__extension_register((Cn_Extension *[]) { __VA_ARGS__ }, sizeof((Cn_Extension *[]) { __VA_ARGS__ }) / sizeof(Cn_Extension *))

CNDEF bool cn__extension_register(Cn_Extension *extensions[], int64_t length);


/**
 * ============================================
 * SECTION: Analyze
 * ============================================
 */

/**
 * TODO: Make sure if exposed to public API, that it properly handles interning of tagged types.
 *
 * Interns `type` into the type universe of the current translation unit.
 *
 * Structurally equal types are stored only once, so interned types can be compared by pointer.
 *
 * RETURNS: Interned type that is structurally equal to `type`, newly stored if it wasn't there yet.
 *
 * IMPORTANT: Every type created outside the library, for example by analyze hooks,
 * must be interned before it is used, so it is deduplicated and properly rolled back.
 *
 * NOTE: `type` itself can be a temporary, it is copied when stored.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type *cn_analyze_intern_type(Cn_Type *type);

/**
 * Flags to specify details on how certain analyze functions should work.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_ANALYZE_NO_FLAGS                     = 0x0,
    CN_ANALYZE_APPLY_PARAMETER_ADJUSTMENT   = 0x1,
} Cn_Analyze_Flags;

/**
 * Optional values passed to `cn_analyze_declaration`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    void *declarator;
    void *initializer;
    Cn_Analyze_Flags flags;
} Cn_Analyze_Declaration_Opt;

/**
 * Analyzes already constructed specifiers, declarator and initializer as one binding.
 *
 * Modifies the global state, by recording new binding from the given ast nodes.
 * Initializer can be passed as `NULL` if binding doesn't contain a definition.
 * Declarator itself can also be passed as `NULL`, in that case only declaration specifiers are checked.
 *
 * Performs typechecking, can error if the binding is illegal in the current context,
 * or if there is any type mismatch, or undefined names.
 *
 * RETURNS: `true` on success, `false` if any errors occurred.
 *
 * SINCE: 0.3.0
 */
#define cn_analyze_declaration(declaration_specifiers, ...) cn__analyze_declaration_opt(declaration_specifiers, (Cn_Analyze_Declaration_Opt) { __VA_ARGS__ })
CNDEF bool cn__analyze_declaration_opt(void *declaration_specifiers, Cn_Analyze_Declaration_Opt opt);

/**
 * Analyzes already constructed ast nodes as function definition, adding proper function binding.
 *
 * Modifies the global state and performs typechecking.
 *
 * RETURNS: `true` on success, `false` if any errors occurred.
 *
 * OUTPUTS: Function binding on success, written to `function_binding_idx`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_analyze_function_definition(void *declaration_specifiers, void *declarator, Cn_Binding_Idx *function_binding_idx);

/**
 * Converts `qualifier`, `type_specifier` and `declarator` triple into a resolved `Cn_Type`.
 *
 * Type specifier gives the base type, qualifiers are applied to it,
 * and declarator wraps it in pointer, array or function layers.
 *
 * PARAM declarator: Can be `NULL` if no declarator is present at all.
 * In that case, the type is composed only from type specifier and qualifiers.
 *
 * RETURNS: Resolved type, `NULL` on error.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type *cn_analyze_to_type(Cn_Ast_Qualifier_Flags qualifier, void *type_specifier, void *declarator, Cn_Analyze_Flags flags);

/**
 * Optional values of `cn_analyze_layout_default`, zeroed options give natural C layout.
 *
 * MEMBER max_align: Caps natural alignment of every member, `0` for no cap, `1` packs the type.
 *
 * MEMBER min_align: Minimal alignment of the whole type, `0` for none.
 *
 * MEMBER member_aligns: Optional array with one entry per member, explicit alignment of the member,
 * `0` entry keeps natural alignment. Explicit alignment is not capped by `max_align`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    int64_t  max_align;
    int64_t  min_align;
    int64_t *member_aligns;
} Cn_Layout_Opt;

/**
 * Lays out tag `type`, `struct`, `union` or `enum`, whose members are already resolved.
 *
 * Struct members are placed in order, each at an offset aligned to its alignment, union members
 * all start at `0`. Size is padded to the alignment of the type. Enum takes size and alignment
 * of its member type, only `min_align` option applies to it.
 *
 * OUTPUTS: `size`, `align` and struct member offsets of `type`, which is then marked with `CN_TYPE_COMPLETE`.
 *
 * NOTE: Meant to be called by layout analyze hooks with custom options, see `Cn_Hook_Analyze_Layout`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_analyze_layout_default(Cn_Type *type, Cn_Layout_Opt opt);

/**
 * Typechecks `expression`, assigning appropriate type to every node.
 *
 * Recursively traverses ast nodes from the bottom up.
 *
 * RETURNS: Resulting type of the expression on success, `NULL` if an error occurred.
 *
 * IMPORTANT: Expects `expression != NULL`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Type *cn_analyze_typecheck_expression(void *expression);

/**
 * Evaluates `expression`, using supplied `buffer` to store data.
 *
 * PARAM expression: If `NULL`, `NIL` is returned.
 *
 * RETURNS: Valid any struct on success. `NIL` if expression wasn't typechecked,
 * or if it cannot be evaluated at compile time.
 *
 * IMPORTANT: Expects `expression` to be typechecked, or to be `NULL`.
 *
 * NOTE: Size of `buffer` can be determined from the resulting type of the expression.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Any cn_analyze_evaluate_expression(void *expression, void *buffer);

/**
 * Typechecks `designations` against known `type`, recursively.
 *
 * RETURNS: `true` on success, `false` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_analyze_typecheck_designations(Cn_Ast_Nodes designations, Cn_Type *type);

/**
 * Typechecks `initializer` against known `type`, recursively.
 *
 * RETURNS: `true` on success, `false` if an error occurred.
 *
 * IMPORTANT: Expects `initializer != NULL`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_analyze_typecheck_initializer(void *initializer, Cn_Type *type);

/**
 * ============================================
 * SECTION: Emit
 * ============================================
 */

/**
 * Write function used by emitter.
 *
 * Takes in `str`, which is the text sent to write, and `ctx`, which is any user data supplied.
 * `ctx` is directly copied from the optional `ctx` of the emitter, so when calling emitter
 * user can specify their own `ctx` and use it however they want at the write call site.
 *
 * SINCE: 0.3.0
 */
typedef void (Cn_Emit_Write)(Cn_String str, void *ctx);

/**
 * Wrapper that casts `ctx` to `FILE *` and writes `str` to it.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_emit_write_file(Cn_String str, void *ctx);

/**
 * Wrapper that casts `ctx` to `Cn_String_Builder *` and writes `str` to it.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_emit_write_sb(Cn_String str, void *ctx);

/**
 * Emitter flags that control emitter behaviour.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK     = 0x1,
    CN_EMITTER_END_DECLARATION_WITH_SEMICOLON   = 0x2,
} Cn_Emitter_Flags;

/**
 * Struct definition of emitter.
 *
 * MEMBER written_length: Total length written during the whole emit.
 *
 * MEMBER indent: Indentation level to be added.
 *
 * MEMBER ctx: User data or context that is shared with every write call made by emitter.
 * 
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Emitter_Flags flags;
    int64_t          written_length;
    int              indent;
    void            *ctx;
    Cn_Emit_Write   *write;
} Cn_Emitter;

/**
 * Emits C source code of `node` using emitter `e`.
 *
 * Reconstructs valid C source code from the AST representation.
 * 
 * SINCE: 0.3.0
 */
CNDEF void cn_emit(Cn_Emitter *e, void *node);


/**
 * ============================================
 * SECTION: Parse
 * ============================================
 */

/**
 * Parses single token of expected `type`, reporting error diagnostic if token doesn't match.
 *
 * RETURNS: `true` if token was parsed, `false` otherwise.
 * 
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_expect(Cn_Lexer *lexer, Cn_Token_Kind kind);

/**
 * Parses single token of `type` if it is present, without reporting any error otherwise.
 *
 * RETURNS: `true` if token was parsed, `false` otherwise.
 * 
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_optional(Cn_Lexer *lexer, Cn_Token_Kind kind);

/**
 * Parses translation unit from the current token of `lexer`.
 *
 * ```
 *  translation_unit
 *          : external_declaration* EOF
 *          ;
 * ```
 *
 * RETURNS: Parsed translation unit, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Translation_Unit *cn_parse_translation_unit(Cn_Lexer *lexer);

/**
 * Parses external declaration from the current token of `lexer`.
 *
 * ```
 *  external_declaration
 *          : gnu_extensions? (
 *              ';'                         // Stray ';'
 *              | gnu_asm_statement
 *              | function
 *              | declaration ';'
 *              )
 *          ;
 * ```
 *
 * RETURNS: Parsed external declaration, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_External_Declaration *cn_parse_external_declaration(Cn_Lexer *lexer);

/**
 * Parses function definition or declaration from the current token of `lexer`.
 *
 * It can't be known for certain which one it is, until the ast structure is parsed far enough.
 * Ultimately, existence of a function body at the end of the declaration signifies that the
 * declaration is a function definition.
 *
 * The deciding logic is to first try to parse a single declarator, and if it is followed by a function
 * body, then it is a function definition.
 *
 * ```
 *  declaration ';'
 *          : attribute_specifiers declaration_specifiers init_declarators? ';'
 *          ;
 *
 *  function
 *          : attribute_specifiers declaration_specifiers declarator block
 *          ;
 * ```
 *
 * RETURNS: Parsed function or declaration, `NULL` on error.
 *
 * NOTE: K&R styled function definitions with classic declaration lists are not supported.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_function_or_declaration(Cn_Lexer *lexer);

/**
 * Finishes parsing declaration from the current token of `lexer`, using already parsed
 * `attribute_specifiers`.
 *
 * ```
 *  declaration
 *          : attribute_specifiers declaration_specifiers init_declarators? gnu_attribute_specifiers
 *          ;
 * ```
 *
 * RETURNS: Parsed declaration, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_finish_declaration(Cn_Lexer *lexer, Cn_Ast_Nodes attribute_specifiers);

/**
 * Determines whether tokens of `lexer` indicate start of a declaration.
 *
 * Used to distinguish between declaration and statement before parsing them.
 *
 * RETURNS: `true` if tokens indicate start of a declaration, `false` otherwise.
 *
 * NOTE: Doesn't alter state of `lexer`.
 * 
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_starts_declaration(Cn_Lexer *lexer);

/**
 * Determines whether tokens of `lexer` indicate start of a type.
 *
 * Used to distinguish type cast during parsing of an expression.
 *
 * RETURNS: `true` if tokens indicate start of a type, `false` otherwise.
 *
 * NOTE: Doesn't alter state of `lexer`.
 * 
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_starts_type(Cn_Lexer *lexer);

/**
 * Parses statement from the current token of `lexer`.
 *
 * There is no single statement node kind, instead one of the following statement nodes is returned.
 *
 * ```
 *  statement
 *          : block
 *          | selection_statement
 *          | iteration_statement
 *          | jump_statement
 *          | labeled_statement
 *          | expression_statement
 *          | gnu_asm_statement
 *          ;
 * ```
 *
 * RETURNS: Parsed statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_statement(Cn_Lexer *lexer);

/**
 * Finishes parsing statement from the current token of `lexer`, using already parsed
 * `attribute_specifiers`.
 *
 * There is no single statement node kind, instead one of the following statement nodes is returned.
 *
 * ```
 *  statement
 *          : block
 *          | selection_statement
 *          | iteration_statement
 *          | jump_statement
 *          | labeled_statement
 *          | expression_statement
 *          | gnu_asm_statement
 *          ;
 * ```
 *
 * RETURNS: Parsed statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_finish_statement(Cn_Lexer *lexer, Cn_Ast_Nodes attribute_specifiers);

/**
 * Parses block from the current token of `lexer`.
 *
 * Block introduces new scope, unless `use_current_scope` is set.
 *
 * ```
 *  block
 *          : '{' block_item* '}'
 *          ;
 * ```
 *
 * PARAM use_current_scope: If `true`, block doesn't introduce new scope, everything is declared and
 * defined in the current scope instead.
 *
 * RETURNS: Parsed block, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Block *cn_parse_block(Cn_Lexer *lexer, bool use_current_scope);

/**
 * Parses block item from the current token of `lexer`.
 *
 * ```
 *  block_item
 *          : gnu_extensions? declaration ';'
 *          | statement
 *          ;
 * ```
 *
 * RETURNS: Parsed block item, `NULL` on error.
 *
 * NOTE: If `gnu_extensions` are present, `CN_AST_GNU_EXTENSION` flag is set on the declaration.
 * If `gnu_extensions` aren't followed by a declaration, item is parsed as a statement, where they
 * are a part of the expression, see `cn_parse_expression`.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Block_Item *cn_parse_block_item(Cn_Lexer *lexer);

/**
 * Parses `if` statement from the current token of `lexer`.
 *
 * ```
 *  if_statement
 *          : 'if' '(' expression ')' statement ('else' statement)?
 *          ;
 * ```
 *
 * RETURNS: Parsed `if` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_if_statement(Cn_Lexer *lexer);

/**
 * Parses `switch` statement from the current token of `lexer`.
 *
 * ```
 *  switch_statement
 *          : 'switch' '(' expression ')' statement
 *          ;
 * ```
 *
 * RETURNS: Parsed `switch` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_switch_statement(Cn_Lexer *lexer);

/**
 * Parses `while` statement from the current token of `lexer`.
 *
 * ```
 *  while_statement
 *          : 'while' '(' expression ')' statement
 *          ;
 * ```
 *
 * RETURNS: Parsed `while` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_while_statement(Cn_Lexer *lexer);

/**
 * Parses `do while` statement from the current token of `lexer`.
 *
 * ```
 *  do_while
 *          : 'do' statement 'while' '(' expression ')' ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed `do while` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_do_while(Cn_Lexer *lexer);

/**
 * Parses `for` statement from the current token of `lexer`.
 *
 * ```
 *  for_statement
 *          : 'for' '(' (gnu_extensions? declaration | expression)? ';' expression? ';' expression? ')' statement
 *          ;
 * ```
 *
 * RETURNS: Parsed `for` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_for_statement(Cn_Lexer *lexer);

/**
 * Parses `goto` statement from the current token of `lexer`.
 *
 * ```
 *  goto_statement
 *          : 'goto' identifier ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed `goto` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_goto_statement(Cn_Lexer *lexer);

/**
 * Parses `return` statement from the current token of `lexer`.
 *
 * ```
 *  return_statement
 *          : 'return' expression? ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed `return` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_return_statement(Cn_Lexer *lexer);

/**
 * Parses `break` statement from the current token of `lexer`.
 *
 * ```
 *  break_statement
 *          : 'break' ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed `break` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_break_statement(Cn_Lexer *lexer);

/**
 * Parses `continue` statement from the current token of `lexer`.
 *
 * ```
 *  continue_statement
 *          : 'continue' ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed `continue` statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_continue_statement(Cn_Lexer *lexer);

/**
 * Parses label from the current token of `lexer`.
 *
 * ```
 *  label
 *          : (identifier | ('case' expression) | 'default') ':' statement
 *          ;
 * ```
 *
 * RETURNS: Parsed label, `NULL` on error.
 *
 * NOTE: Instead of separate nodes, label node represents all three kinds of labels: identifier label,
 * `case` label and `default` label. Kind is stored in flags, and fields that don't apply to the kind
 * are `NULL`.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_label(Cn_Lexer *lexer);

/**
 * Parses expression statement from the current token of `lexer`.
 *
 * ```
 *  expression_statement
 *          : expression? ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed expression statement, `NULL` on error.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Node *cn_parse_expression_statement(Cn_Lexer *lexer);

/**
 * Parses argument list from the current token of `lexer`.
 *
 * ```
 *  argument_list
 *          : expression (',' expression)*
 *          ;
 * ```
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * RETURNS: Parsed list of argument expressions.
 * 
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_parse_arguments(Cn_Lexer *lexer, bool *ok);

/**
 * Flags that are passed to expression parsing function to control its behaviour.
 *
 * MEMBER CN_NO_COMMA_OPERATOR: If set, comma operator is ignored by this particular parse expression call.
 * It doesn't mean that all recursive calls ignore comma operators too,
 * for example, this flag is reset to `0` when parenthesized expression is encountered.
 * This flag is used when top level comma token acts as a separator,
 * for example in declarations with multiple declarators, or in function arguments.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_NO_COMMA_OPERATOR = 0x1, 
} Cn_Parse_Expression_Flags;

/**
 * Parses expression from the current token of `lexer`.
 *
 * First parses left leaf, and then calls increasing precedence parsing until it returns left leaf,
 * utilizing the following precedence table as a guide to properly structure ast nodes in order.
 *
 * ```
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
 * ```
 *
 * Following grammar rules group expressions into certain kinds, but in reality all parsing is based
 * on the precedence table. Grammar rules only show the general logic, and which specific tokens
 * parser might expect in certain expressions.
 *
 * ```
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
 *          | gnu_extensions expression
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
 * ```
 *
 * PARAM min_precedence: Lowest precedence of binary operators that are parsed, `-1` to parse all of them.
 *
 * RETURNS: Parsed expression, `NULL` on error.
 */
CNDEF Cn_Ast_Node *cn_parse_expression(Cn_Lexer *lexer, int min_precedence, Cn_Parse_Expression_Flags flags);

/**
 * Continues parsing binary expression after already parsed `left` leaf.
 *
 * Constructs right leaning tree if next binary operator has increasing precedence,
 * otherwise just returns `left` leaf. Recursively calls `cn_parse_expression` to get right leaf.
 *
 * Example of `a + b * c`, strictly increasing precedence results in right leaning tree:
 *
 * ```
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
 * ```
 *
 * But there is also a situation where left leaning tree must be constructed.
 * Example of `a + b - c`, same precedence results in left leaning tree:
 *
 * ```
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
 * ```
 *
 * A more sophisticated example, `a + b * c + d` is parsed like `((a + (b * c)) + d)`:
 *
 * ```
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
 * ```
 *
 * RETURNS: Parsed expression, `NULL` on error.
 *
 * NOTE: Naturally, this function constructs right leaning tree on binary expressions.
 */
CNDEF Cn_Ast_Node *cn_parse_expression_increasing_precedence(Cn_Lexer *lexer, void * left, int min_precedence, Cn_Parse_Expression_Flags flags);

/**
 * Parses non-binary leaf expression from the current token of `lexer`.
 *
 * Detects and parses unary operators, but doesn't parse binary or postfix expressions.
 *
 * RETURNS: Parsed expression, `NULL` on error.
 */
CNDEF Cn_Ast_Node *cn_parse_expression_leaf(Cn_Lexer *lexer, Cn_Parse_Expression_Flags flags);

/**
 * Finishes parsing init declarator list from the current token of `lexer`, using already parsed
 * `declarator` as the first init declarator.
 *
 * ```
 *  init_declarators
 *          : init_declarator (',' init_declarator)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of init declarators.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_finish_init_declarators(Cn_Lexer *lexer, void *declarator, bool *ok, void *declaration_specifiers);

/**
 * Parses init declarator list from the current token of `lexer`.
 *
 * ```
 *  init_declarators
 *          : init_declarator (',' init_declarator)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of init declarators.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_init_declarators(Cn_Lexer *lexer, bool *ok, void *declaration_specifiers);

/**
 * Finishes parsing init declarator from the current token of `lexer`, using already parsed
 * `declarator`.
 *
 * ```
 *  init_declarator
 *          : declarator (gnu_asm_label | gnu_attribute_specifier)* ('=' initializer)?
 *          ;
 * ```
 *
 * RETURNS: Parsed init declarator, `NULL` on error.
 */
CNDEF Cn_Ast_Init_Declarator *cn_parse_finish_init_declarator(Cn_Lexer *lexer, Cn_Ast_Declarator *declarator, void *declaration_specifiers);

/**
 * Parses init declarator from the current token of `lexer`.
 *
 * ```
 *  init_declarator
 *          : declarator (gnu_asm_label | gnu_attribute_specifier)* ('=' initializer)?
 *          ;
 * ```
 *
 * RETURNS: Parsed init declarator, `NULL` on error.
 */
CNDEF Cn_Ast_Init_Declarator *cn_parse_init_declarator(Cn_Lexer *lexer, void *declaration_specifiers);

/**
 * Parses initializer from the current token of `lexer`.
 *
 * ```
 *  initializer
 *          : expression
 *          | designations
 *          ;
 * ```
 *
 * RETURNS: Parsed initializer, `NULL` on error.
 */
CNDEF Cn_Ast_Initializer *cn_parse_initializer(Cn_Lexer *lexer);

/**
 * Parses designations from the current token of `lexer`.
 *
 * ```
 *  designations
 *          : '{' ( designation (',' designation)* )? ','? '}'
 *          ;
 * ```
 *
 * RETURNS: Parsed list of designations.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_designations(Cn_Lexer *lexer, bool *ok);

/**
 * Parses designation from the current token of `lexer`.
 *
 * ```
 *  designation
 *          : (designators '=')? initializer
 *          ;
 * ```
 *
 * RETURNS: Parsed designation, `NULL` on error.
 */
CNDEF Cn_Ast_Designation *cn_parse_designation(Cn_Lexer *lexer);

/**
 * Parses designators from the current token of `lexer`.
 *
 * ```
 *  designators
 *          : designator+
 *          ;
 * ```
 *
 * RETURNS: Parsed list of designators.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_designators(Cn_Lexer *lexer, bool *ok);

/**
 * Parses designator from the current token of `lexer`.
 *
 * ```
 *  designator
 *          : '.' identifier
 *          | '[' expression ']'
 *          | '[' expression '...' expression ']'
 *          ;
 * ```
 *
 * RETURNS: Parsed designator, `NULL` on error.
 */
CNDEF Cn_Ast_Designator *cn_parse_designator(Cn_Lexer *lexer);

/**
 * Determines whether tokens of `lexer` indicate start of a designator.
 *
 * Used to distinguish between designator and initializer before parsing them.
 *
 * RETURNS: `true` if tokens indicate start of a designator, `false` otherwise.
 *
 * NOTE: Doesn't alter state of `lexer`.
 */
CNDEF bool cn_parse_starts_designator(Cn_Lexer *lexer);

/**
 * Parses declarator from the current token of `lexer`.
 *
 * ```
 *  declarator
 *          : pointer? direct_declarator
 *          ;
 * ```
 *
 * RETURNS: Parsed declarator, `NULL` on error or if declarator is completely empty.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * NOTE: `NULL` can be returned on success, only in case of a completely empty abstract declarator,
 * meaning abstract declarator without direct declarator. In that case no tokens are consumed, since
 * nothing is parsed.
 */
CNDEF Cn_Ast_Declarator *cn_parse_declarator(Cn_Lexer *lexer, bool *ok);

/**
 * Parses pointer from the current token of `lexer`.
 *
 * ```
 *  pointer
 *          : '*' (qualifier | gnu_attribute_specifier)* pointer?
 *          ;
 * ```
 *
 * RETURNS: Parsed pointer, `NULL` on error.
 */
CNDEF Cn_Ast_Pointer *cn_parse_pointer(Cn_Lexer *lexer);

/**
 * Parses direct declarator from the current token of `lexer`.
 *
 * ```
 *  direct_declarator
 *          : identifier
 *          | '(' declarator ')'
 *          | direct_declarator '[' expression? ']'
 *          | direct_declarator '(' ('void' | parameter_type_list)? ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed direct declarator, `NULL` on error.
 *
 * OUTPUTS: `true` if no identifier was found at the end of the direct declarator, `false` otherwise,
 * written to `is_abstract`.
 */
CNDEF Cn_Ast_Node *cn_parse_direct_declarator(Cn_Lexer *lexer, bool *is_abstract);

/**
 * Parses identifier from the current token of `lexer`.
 *
 * RETURNS: Parsed identifier, `NULL` on error.
 */
CNDEF Cn_Ast_Identifier *cn_parse_identifier(Cn_Lexer *lexer);

/**
 * Parses integer from the current token of `lexer`.
 *
 * RETURNS: Parsed integer, `NULL` on error.
 */
CNDEF Cn_Ast_Integer *cn_parse_integer(Cn_Lexer *lexer);

/**
 * Parses float from the current token of `lexer`.
 *
 * RETURNS: Parsed float, `NULL` on error.
 */
CNDEF Cn_Ast_Float *cn_parse_float(Cn_Lexer *lexer);

/**
 * Parses character from the current token of `lexer`.
 *
 * RETURNS: Parsed character, `NULL` on error.
 *
 * NOTE: Value of the char is parsed, so any escape sequence is converted.
 */
CNDEF Cn_Ast_Char *cn_parse_character(Cn_Lexer *lexer);

/**
 * Parses string from the current token of `lexer`.
 *
 * RETURNS: Parsed string, `NULL` on error.
 *
 * NOTE: Any neighboring string tokens are concatenated, until a non-string token is reached.
 */
CNDEF Cn_Ast_String *cn_parse_string(Cn_Lexer *lexer);

/**
 * Parses declaration specifiers from the current token of `lexer`.
 *
 * ```
 *  declaration_specifiers
 *          : (
 *              storage_specifier
 *              | qualifier
 *              | function_specifier
 *              | gnu_attribute_specifier
 *              | type_specifier
 *              )+
 *          ;
 * ```
 *
 * RETURNS: Parsed declaration specifiers, `NULL` on error.
 */
CNDEF Cn_Ast_Declaration_Specifiers *cn_parse_declaration_specifiers(Cn_Lexer *lexer);

/**
 * Tries to parse storage specifier from the current token of `lexer`.
 *
 * ```
 *  storage_specifier
 *          : 'static'
 *          | 'extern'
 *          | 'register'
 *          | 'auto'
 *          | 'typedef'
 *          ;
 * ```
 *
 * RETURNS: `0` on success, `1` if token wasn't accepted, `2` on error.
 *
 * OUTPUTS: Parsed storage specifier, written to `output`.
 */
CNDEF int cn_parse_try_storage_specifier(Cn_Lexer *lexer, Cn_Ast_Storage_Specifier_Flags *output);

/**
 * Tries to parse qualifier from the current token of `lexer`.
 *
 * ```
 *  qualifier
 *          : 'const'
 *          | 'restrict'
 *          | 'volatile'
 *          | '_Atomic'
 *          ;
 * ```
 *
 * RETURNS: `0` on success, `1` if token wasn't accepted, `2` on error.
 *
 * OUTPUTS: Parsed qualifier, written to `output`.
 */
CNDEF int cn_parse_try_qualifier(Cn_Lexer *lexer, Cn_Ast_Qualifier_Flags *output);

/**
 * Tries to parse function specifier from the current token of `lexer`.
 *
 * ```
 *  function_specifier
 *          : 'inline'
 *          | '_Noreturn'
 *          ;
 * ```
 *
 * RETURNS: `0` on success, `1` if token wasn't accepted, `2` on error.
 *
 * OUTPUTS: Parsed function specifier, written to `output`.
 */
CNDEF int cn_parse_try_function_specifier(Cn_Lexer *lexer, Cn_Ast_Function_Specifier_Flags *output);

/**
 * Helper struct for accumulating primitive type specifier info during parsing.
 */
typedef struct {
    Cn_Ast_Type_Kind  kind;
    Cn_Ast_Type_Width width;
    Cn_Ast_Type_Sign  sign;
} Cn_Primitive_Type_Info;

/**
 * Tries to parse type specifier from the current token of `lexer`.
 *
 * ```
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
 * ```
 *
 * RETURNS: `0` on success, `1` if token wasn't accepted, `2` on error.
 *
 * OUTPUTS: For primitive types like `int`, `char`, `signed`, `unsigned`, `long`, `short`, etc.,
 * updates `primitive_info`.
 *
 * OUTPUTS: For typedef, `struct`, `union`, `enum` and `typeof`, sets `type_specifier` to the node.
 *
 * NOTE: Returning `1` is NOT an error. It occurs when current token is not a valid token
 * type specifier can accept, and no errors occurred. This way type specifier can check everything
 * it needs, and if there is no work to be done, signal to the outer function
 * that current token is definitely not a type specifier.
 *
 * TODO: Refactor implementation of this function and its interface.
 * It can be simpler, without need of the helper primitive type info struct.
 */
CNDEF int cn_parse_try_type_specifier(Cn_Lexer *lexer, Cn_Primitive_Type_Info *primitive_info, Cn_Ast_Node **type_specifier);

/**
 * Parses type name from the current token of `lexer`.
 *
 * ```
 *  type_name
 *          : specifier_qualifier abstract_declarator
 *          ;
 * ```
 *
 * RETURNS: Parsed type name, `NULL` on error.
 */
CNDEF Cn_Ast_Type_Name *cn_parse_type_name(Cn_Lexer *lexer);

/**
 * Parses specifier qualifier from the current token of `lexer`.
 *
 * ```
 *  specifier_qualifier
 *          : gnu_attribute_specifiers (qualifier | type_specifier)+
 *          ;
 * ```
 *
 * RETURNS: Parsed specifier qualifier, `NULL` on error.
 */
CNDEF Cn_Ast_Specifier_Qualifier *cn_parse_specifier_qualifier(Cn_Lexer *lexer);

/**
 * Parses parameter type list from the current token of `lexer`.
 *
 * ```
 *  parameter_type_list
 *          : '...'
 *          | parameter_declaration_list (',' '...')?
 *          ;
 *
 *  parameter_declaration_list
 *          : parameter_declaration (',' parameter_declaration)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of parameter declarations.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * OUTPUTS: `true` if list ends with `...`, written to `variadic`.
 */
CNDEF Cn_Ast_Nodes cn_parse_parameter_declarations(Cn_Lexer *lexer, bool *variadic, bool *ok);

/**
 * Parses parameter declaration from the current token of `lexer`.
 *
 * ```
 *  parameter_declaration
 *          : declaration_specifiers declarator
 *          ;
 * ```
 *
 * RETURNS: Parsed parameter declaration, `NULL` on error.
 */
CNDEF Cn_Ast_Parameter_Declaration *cn_parse_parameter_declaration(Cn_Lexer *lexer);

/**
 * Parses `struct` or `union` specifier from the current token of `lexer`.
 *
 * ```
 *  struct_specifier
 *          : 'struct' (attribute_specifier | gnu_attribute_specifier)*
 *          ( identifier? '{' member_declaration* '}' )
 *          | identifier
 *          ;
 *
 *  union_specifier
 *          : 'union' (attribute_specifier | gnu_attribute_specifier)*
 *          ( identifier? '{' member_declaration* '}' )
 *          | identifier
 *          ;
 * ```
 *
 * RETURNS: Parsed `struct` or `union` specifier, `NULL` on error.
 */
CNDEF Cn_Ast_Node *cn_parse_struct_or_union_specifier(Cn_Lexer *lexer);

/**
 * Parses member declaration from the current token of `lexer`.
 *
 * ```
 *  member_declaration
 *          : gnu_extensions? attribute_specifiers specifier_qualifier member_declarators ';'
 *          ;
 * ```
 *
 * RETURNS: Parsed member declaration, `NULL` on error.
 */
CNDEF Cn_Ast_Member_Declaration *cn_parse_member_declaration(Cn_Lexer *lexer);

/**
 * Parses member declarator list from the current token of `lexer`.
 *
 * ```
 *  member_declarators
 *          : member_declarator (',' member_declarator)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of member declarators.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_member_declarators(Cn_Lexer *lexer, bool *ok);

/**
 * Parses member declarator from the current token of `lexer`.
 *
 * ```
 *  member_declarator
 *          : declarator gnu_attribute_specifiers
 *          | declarator? ':' expression gnu_attribute_specifiers
 *          ;
 * ```
 *
 * RETURNS: Parsed member declarator, `NULL` on error.
 */
CNDEF Cn_Ast_Member_Declarator *cn_parse_member_declarator(Cn_Lexer *lexer);

/**
 * Parses `enum` specifier from the current token of `lexer`.
 *
 * ```
 *  enum_specifier
 *          : 'enum' (attribute_specifier | gnu_attribute_specifier)*
 *          ( identifier? (':' specifier_qualifier)? '{' ( enumerator (',' enumerator)* ','? )? '}' )
 *          | identifier  (':' specifier_qualifier)?
 *          ;
 * ```
 *
 * RETURNS: Parsed `enum` specifier, `NULL` on error.
 */
CNDEF Cn_Ast_Node *cn_parse_enum_specifier(Cn_Lexer *lexer);

/**
 * Parses enumerator from the current token of `lexer`.
 *
 * ```
 *  enumerator
 *          : identifier (attribute_specifier | gnu_attribute_specifier)* ('=' expression)?
 *          ;
 * ```
 *
 * RETURNS: Parsed enumerator, `NULL` on error.
 */
CNDEF Cn_Ast_Enumerator *cn_parse_enumerator(Cn_Lexer *lexer);

/**
 * Parses attribute specifier sequence from the current token of `lexer`.
 *
 * ```
 *  attribute_specifiers
 *          : attribute_specifier*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of attribute specifiers.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_attribute_specifiers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses attribute specifier from the current token of `lexer`.
 *
 * ```
 *  attribute_specifier
 *          : '[' '[' attribute_list? ']' ']'
 *          ;
 * ```
 *
 * RETURNS: Parsed attribute specifier, `NULL` on error.
 */
CNDEF Cn_Ast_Attribute_Specifier *cn_parse_attribute_specifier(Cn_Lexer *lexer);

/**
 * Parses attribute list from the current token of `lexer`.
 *
 * ```
 *  attributes
 *          : attribute (',' attribute)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of attributes.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_attributes(Cn_Lexer *lexer, bool *ok);

/**
 * Parses attribute from the current token of `lexer`.
 *
 * ```
 *  attribute
 *          : ( identifier ':' ':' )? identifier ( '(' arguments ')' )?
 *          ;
 * ```
 *
 * RETURNS: Parsed attribute, `NULL` on error.
 */
CNDEF Cn_Ast_Attribute *cn_parse_attribute(Cn_Lexer *lexer);

#ifdef CN_GNU
/**
 * Parses GNU `__extension__` keywords from the current token of `lexer`, if present.
 *
 * ```
 *  gnu_extensions
 *          : '__extension__'+
 *          ;
 * ```
 *
 * RETURNS: `true` if at least one keyword was parsed.
 *
 * NOTE: Keyword only disables pedantic diagnostics in GCC, so it is not stored as a node,
 * callers set `CN_AST_GNU_EXTENSION` flag on the node that follows it.
 */
CNDEF bool cn__parse_gnu_extension(Cn_Lexer *lexer);

/**
 * Parses GNU `typeof` specifier from the current token of `lexer`.
 *
 * ```
 *  gnu_typeof_specifier
 *          : 'typeof' '(' type_name ')'
 *          | 'typeof' '(' expression ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed `typeof` specifier, `NULL` on error.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Gnu_Typeof *cn_parse_gnu_typeof(Cn_Lexer *lexer);

/**
 * Parses GNU attribute specifier sequence from the current token of `lexer`.
 *
 * ```
 *  gnu_attribute_specifiers
 *          : gnu_attribute_specifier*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of GNU attribute specifiers.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_attribute_specifiers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses attribute specifier sequence of both `[[...]]` and GNU syntax, in any order,
 * from the current token of `lexer`.
 *
 * ```
 *  mixed_attribute_specifiers
 *          : (attribute_specifier | gnu_attribute_specifier)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of attribute specifiers.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_parse_mixed_attribute_specifiers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU attribute specifier from the current token of `lexer`.
 *
 * ```
 *  gnu_attribute_specifier
 *          : '__attribute__' '(' '(' attributes? ')' ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed attribute specifier flagged with `CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU`, `NULL` on error.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Attribute_Specifier *cn_parse_gnu_attribute_specifier(Cn_Lexer *lexer);

/**
 * Parses GNU `asm` statement from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_statement
 *          : 'asm' gnu_asm_qualifier* '(' string gnu_asm_sections? ')' ';'
 *          ;
 *
 *  gnu_asm_sections
 *          : ':' gnu_asm_operands? ( ':' gnu_asm_operands? ( ':' gnu_asm_clobbers? ( ':' gnu_asm_goto_labels )? )? )?
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU `asm` statement, `NULL` on error.
 *
 * NOTE: Without `gnu_asm_sections` the statement is basic asm, otherwise it is extended asm.
 * First operand list holds outputs, second holds inputs. Lists that are not present are empty.
 *
 * NOTE: Constraints not expressed by the grammar, reported as errors:
 *  - each qualifier may appear at most once, in any order,
 *  - `goto` qualifier is not allowed in basic asm,
 *  - `goto` qualifier requires the goto labels section, with at least one label,
 *  - goto labels section is not allowed without `goto` qualifier.
 *
 * NOTE: Also used for file-scope asm, which is a basic asm statement appearing as an external
 * declaration. Callers at file scope should reject extended asm and any qualifiers, GCC and Clang
 * reject them too.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Gnu_Asm_Statement *cn_parse_gnu_asm_statement(Cn_Lexer *lexer);

/**
 * Tries to parse GNU `asm` qualifier from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_qualifier
 *          : 'volatile'
 *          | 'inline'
 *          | 'goto'
 *          ;
 * ```
 *
 * RETURNS: `0` on success, `1` if token wasn't accepted, `2` on error.
 *
 * OUTPUTS: Parsed qualifier, added to `output`.
 *
 * NOTE: Returns `2` if the qualifier is already set in `output`.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF int cn_parse_try_gnu_asm_qualifier(Cn_Lexer *lexer, Cn_Ast_Gnu_Asm_Qualifier_Flags *output);

/**
 * Parses GNU `asm` operand list from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_operands
 *          : gnu_asm_operand (',' gnu_asm_operand)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of GNU `asm` operands.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * NOTE: Returns an empty list without consuming tokens if current token is `:` or `)`,
 * since operand lists may be empty.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_operands(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `asm` operand from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_operand
 *          : ('[' identifier ']')? string '(' expression ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU `asm` operand, `NULL` on error.
 *
 * NOTE: `string` is the constraint, e.g. `"=r"`, `"+m"`, `"i"`. Identifier is the optional
 * symbolic name referenced from the template as `%[name]`.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Gnu_Asm_Operand *cn_parse_gnu_asm_operand(Cn_Lexer *lexer);

/**
 * Parses GNU `asm` clobber list from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_clobbers
 *          : string (',' string)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of clobber strings.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * NOTE: Returns an empty list without consuming tokens if current token is `:` or `)`.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_clobbers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `asm goto` label list from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_goto_labels
 *          : identifier (',' identifier)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of label identifiers.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_goto_labels(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `asm` label from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_label
 *          : 'asm' '(' string ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU `asm` label, `NULL` on error.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Gnu_Asm_Label *cn_parse_gnu_asm_label(Cn_Lexer *lexer);
#endif // CN_GNU


/**
 * Parses integer literal into its magnitude.
 *
 * Accepts decimal, octal and `0x` hex literals, with optional `u`, `U`, `l` or `L` suffix.
 * No leading sign is accepted, since unary minus is its own ast node.
 *
 * RETURNS: `true` on success, `false` on error.
 *
 * OUTPUTS: Magnitude of the literal, written to `out`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_int_literal(Cn_String literal, uint64_t *out);

/**
 * Parses float literal.
 *
 * Accepts mantissa with optional exponent and optional `f`, `F`, `l` or `L` suffix.
 * Rounding is not bit exact.
 *
 * RETURNS: `true` on success, `false` on error.
 *
 * OUTPUTS: Value of the literal, written to `out`.
 *
 * TODO: Hex floats are not handled, they output `NIL`.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_float_literal(Cn_String literal, double *out);

/**
 * Parses char literal into its numeric value.
 *
 * Accepts single characters and escape sequences.
 *
 * RETURNS: `true` on success, `false` on error.
 *
 * OUTPUTS: Value of the literal, written to `out`. It is 64 bits wide, because multi-character
 * literal can occupy more than one byte.
 *
 * OUTPUTS: Number of character units in the literal, written to `unit_count_out`.
 *
 * AI: Generated.
 *
 * SINCE: 0.3.0
 */
CNDEF bool cn_parse_char_literal(Cn_String literal, uint64_t *out, int *unit_count_out);

/**
 * ============================================
 * SECTION: Build
 * ============================================
 */

/**
 * Optional values that can be set for most of the `cn_build_*` functions.
 *
 * MEMBER alloc: Defines whether call to build function should allocate memory internally,
 * in permanent arena, for any supplied data. If set to `false`, the function trusts its user 
 * to manage passed memory, for example any names, snippets of code, etc.
 * Setting it to `false` is helpful if certain piece of code is inserted a lot.
 * For example, it prevents build function from allocating memory over and over again
 * for the same piece of string data.
 *
 * MEMBER file: File and line indicate where specific build function was called from,
 * used to trace meta program code insertions.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    bool        alloc;
    const char *file;
    int64_t     line;
} Cn_Build_Opt;

/**
 * Builds code ast node that contains formatted text.
 *
 * Once reparsed, the node is expanded and parsed into actual ast nodes.
 *
 * RETURNS: Built code node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_format(...) cn__build_format(__FILE__, __LINE__, __VA_ARGS__)

CNDEF Cn_Ast_Code *cn__build_format(const char *file, int64_t line, const char *format, ...);

/**
 * Builds list out of supplied nodes, passed as variadic arguments in order.
 *
 * RETURNS: List composed of supplied nodes, allocated internally in arena.
 *
 * SINCE: 0.3.0
 */
#define cn_build_list(...) cn_build_list_from((void *[]) { __VA_ARGS__ }, sizeof((void *[]) { __VA_ARGS__ }) / sizeof(void *))

/**
 * Builds list out of `length` nodes in `ptrs`.
 *
 * RETURNS: List composed of supplied nodes, allocated internally in arena.
 *
 * SINCE: 0.3.0
 */
CNDEF Cn_Ast_Nodes cn_build_list_from(void *ptrs[], int64_t length);

/**
 * Builds identifier with supplied `name`.
 *
 * RETURNS: Built identifier.
 *
 * SINCE: 0.3.0
 */
#define cn_build_identifier(name, ...) cn__build_identifier(name, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Identifier *cn__build_identifier(Cn_String name, Cn_Build_Opt opt);

/**
 * Builds integer with supplied `value`.
 *
 * RETURNS: Built integer.
 *
 * SINCE: 0.3.0
 */
#define cn_build_integer(value, ...) cn__build_integer(value, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Integer *cn__build_integer(uint64_t value, Cn_Build_Opt opt);

/**
 * Builds float with supplied `value`.
 *
 * RETURNS: Built float.
 *
 * SINCE: 0.3.0
 */
#define cn_build_float(value, ...) cn__build_float(value, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Float *cn__build_float(double value, Cn_Build_Opt opt);

/**
 * Builds string with supplied `str`.
 *
 * RETURNS: Built string.
 *
 * SINCE: 0.3.0
 */
#define cn_build_string(str, ...) cn__build_string(str, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_String *cn__build_string(Cn_String str, Cn_Build_Opt opt);

/**
 * Builds binary expression with operator `op`.
 *
 * RETURNS: Built binary expression.
 *
 * IMPORTANT: `left` and `right` must be valid built nodes.
 *
 * SINCE: 0.3.0
 */
#define cn_build_binary(op, left, right, ...) cn__build_binary(op, left, right, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Binary *cn__build_binary(Cn_Ast_Binary_Operator_Kind op, void *left, void *right, Cn_Build_Opt opt);

/**
 * Builds assignment expression with operator `op`.
 *
 * RETURNS: Built assignment expression.
 *
 * IMPORTANT: `left` and `right` must be valid built nodes.
 *
 * SINCE: 0.3.0
 */
#define cn_build_assign(op, left, right, ...) cn__build_assign(op, left, right, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Assign *cn__build_assign(Cn_Ast_Assignment_Operator_Kind op, void *left, void *right, Cn_Build_Opt opt);

/**
 * Builds unary expression with operator `op`.
 *
 * RETURNS: Built unary expression.
 *
 * IMPORTANT: `expression` must be a valid built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_unary(op, expression, ...) cn__build_unary(op, expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Unary *cn__build_unary(Cn_Ast_Unary_Operator_Kind op, void *expression, Cn_Build_Opt opt);

/**
 * Builds function call expression.
 *
 * RETURNS: Built function call expression.
 *
 * IMPORTANT: `callee` must be a valid built node, and `arguments` must be constructed beforehand
 * with appropriate expression children.
 *
 * SINCE: 0.3.0
 */
#define cn_build_func_call(callee, arguments, ...) cn__build_func_call(callee, arguments, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Call *cn__build_func_call(void *callee, Cn_Ast_Nodes arguments, Cn_Build_Opt opt);

/**
 * Builds expression statement.
 *
 * RETURNS: Built expression statement.
 *
 * IMPORTANT: `expression` must be a valid built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_expr_statement(expression, ...) cn__build_expr_statement(expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Expression_Statement *cn__build_expr_statement(void *expression, Cn_Build_Opt opt);

/**
 * Builds `goto` statement.
 *
 * RETURNS: Built `goto` statement.
 *
 * IMPORTANT: `identifier` must be a valid built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_goto_statement(identifier, ...) cn__build_goto_statement(identifier, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Goto *cn__build_goto_statement(Cn_Ast_Identifier *identifier, Cn_Build_Opt opt);

/**
 * Builds `return` statement.
 *
 * RETURNS: Built `return` statement.
 *
 * IMPORTANT: `expression` must be a valid built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_return_statement(expression, ...) cn__build_return_statement(expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Return *cn__build_return_statement(void *expression, Cn_Build_Opt opt);

/**
 * Builds label.
 *
 * RETURNS: Built label.
 *
 * IMPORTANT: `identifier` and `statement` must be valid built nodes.
 *
 * SINCE: 0.3.0
 */
#define cn_build_label(identifier, statement, ...) cn__build_label(identifier, statement, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Label *cn__build_label(Cn_Ast_Identifier *identifier, void *statement, Cn_Build_Opt opt);

/**
 * Builds block item.
 *
 * RETURNS: Built block item.
 *
 * IMPORTANT: `declaration_or_statement` must be a valid built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_block_item(declaration_or_statement, ...) cn__build_block_item(declaration_or_statement, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Block_Item *cn__build_block_item(void *declaration_or_statement, Cn_Build_Opt opt);

/**
 * Builds pointer with supplied `qualifiers`.
 *
 * Chains `pointer` that stands closer to the declared name. As in `char * const * name`,
 * where outer built pointer holds `const` qualifier and chains the inner one.
 *
 * RETURNS: Built pointer.
 *
 * IMPORTANT: `pointer` must be `NIL` or a valid built pointer.
 *
 * SINCE: 0.3.0
 */
#define cn_build_pointer(qualifiers, pointer, ...) cn__build_pointer(qualifiers, pointer, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Pointer *cn__build_pointer(Cn_Ast_Qualifier_Flags qualifiers, Cn_Ast_Pointer *pointer, Cn_Build_Opt opt);

/**
 * Builds grouped direct declarator `'(' declarator ')'`.
 *
 * RETURNS: Built grouped direct declarator.
 *
 * IMPORTANT: `declarator` must be a valid built node.
 *
 * NOTE: Identifier direct declarator variation has no dedicated build function,
 * plain `cn_build_identifier` result is used in its place.
 *
 * SINCE: 0.3.0
 */
#define cn_build_direct_declarator_grouped(declarator, ...) cn__build_direct_declarator_grouped(declarator, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Direct_Declarator_Grouped *cn__build_direct_declarator_grouped(Cn_Ast_Declarator *declarator, Cn_Build_Opt opt);

/**
 * Builds array direct declarator `direct_declarator '[' expression? ']'`.
 *
 * PARAM direct_declarator: Valid built node, or `NIL` if array declarator is abstract.
 *
 * PARAM expression: Can be `NIL` to build unsized `[]` array.
 *
 * RETURNS: Built array direct declarator.
 *
 * SINCE: 0.3.0
 */
#define cn_build_direct_declarator_array(direct_declarator, expression, ...) cn__build_direct_declarator_array(direct_declarator, expression, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Direct_Declarator_Array *cn__build_direct_declarator_array(void *direct_declarator, void *expression, Cn_Build_Opt opt);

/**
 * Builds function direct declarator `direct_declarator '(' parameter_declarations? ')'`.
 *
 * PARAM direct_declarator: Valid built node, or `NIL` if function declarator is abstract.
 *
 * PARAM parameter_declarations: Must be constructed beforehand with appropriate parameter
 * declaration children, empty list builds `(void)`.
 *
 * PARAM variadic: Marks declarator as ending with `...`.
 *
 * RETURNS: Built function direct declarator.
 *
 * SINCE: 0.3.0
 */
#define cn_build_direct_declarator_function(direct_declarator, parameter_declarations, variadic, ...) cn__build_direct_declarator_function(direct_declarator, parameter_declarations, variadic, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Direct_Declarator_Function *cn__build_direct_declarator_function(void *direct_declarator, Cn_Ast_Nodes parameter_declarations, bool variadic, Cn_Build_Opt opt);

/**
 * Builds declarator.
 *
 * PARAM pointer: Can be `NIL` if declarator is not wrapped with pointers.
 *
 * PARAM direct_declarator: Can be `NIL` only if declarator is abstract.
 *
 * RETURNS: Built declarator.
 *
 * NOTE: `CN_AST_DECLARATOR_IS_ABSTRACT` flag is deduced from supplied direct declarator chain,
 * declarator is abstract when that chain carries no identifier.
 *
 * SINCE: 0.3.0
 */
#define cn_build_declarator(pointer, direct_declarator, ...) cn__build_declarator(pointer, direct_declarator, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Declarator *cn__build_declarator(Cn_Ast_Pointer *pointer, void *direct_declarator, Cn_Build_Opt opt);

/**
 * Builds init declarator.
 *
 * PARAM initializer: Can be `NIL` if declared name is left uninitialized.
 *
 * RETURNS: Built init declarator.
 *
 * IMPORTANT: `declarator` must be a valid non-abstract built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_init_declarator(declarator, initializer, ...) cn__build_init_declarator(declarator, initializer, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Init_Declarator *cn__build_init_declarator(Cn_Ast_Declarator *declarator, Cn_Ast_Initializer *initializer, Cn_Build_Opt opt);

/**
 * Builds declaration.
 *
 * PARAM init_declarators: Must be constructed beforehand with appropriate init declarator children,
 * empty list builds empty declaration.
 *
 * RETURNS: Built declaration.
 *
 * IMPORTANT: `declaration_specifiers` must be a valid built node.
 *
 * SINCE: 0.3.0
 */
#define cn_build_declaration(declaration_specifiers, init_declarators, ...) cn__build_declaration(declaration_specifiers, init_declarators, (Cn_Build_Opt) { .file = __FILE__, .line =__LINE__, __VA_ARGS__ })

CNDEF Cn_Ast_Declaration *cn__build_declaration(Cn_Ast_Declaration_Specifiers *declaration_specifiers, Cn_Ast_Nodes init_declarators, Cn_Build_Opt opt);

/**
 * ============================================
 * SECTION: Translation Unit
 * ============================================
 */

/**
 * Groups all translation unit data together.
 *
 * Owned by a user, and acts as a global state for the library.
 *
 * MEMBER binding_list: Stores all non-label bindings in growing array list.
 * Access elements by indices, so no unsafe situations occur.
 * First element is considered `NIL` element, it is reserved to identify illegal references.
 *
 * MEMBER tag_binding_table: Tag table uses tag names of `struct`, `enum` and `union` as keys, 
 * and bindings as values.
 *
 * MEMBER symbol_binding_table: Symbol table acts similar to the tag table, except it contains
 * variables, functions, typedefs and enum constants.
 *
 * MEMBER label_binding_list: Stores all label bindings in growing array list.
 * Access elements by indices, so no unsafe situations occur.
 * First element is considered `NIL` element, it is reserved to identify illegal references.
 *
 * MEMBER label_binding_table: Label table stores only label bindings.
 *
 * MEMBER binding_defined_idx_list: 
 *  Prevents a really annoying binding related issue, that comes up during reparsing,
 * specifically checkpoint loading, after user executed a hook.
 *  To understand this, first understand that in C most of the bindings are allowed to be redeclared,
 * and some even redefined. For example tag types, `struct`, `enum` and `union`, are allowed to be
 * redeclared as many times as you want, and eventually defined, therefore completing previously
 * declared tag type. And C variables are allowed to be redefined under certain scope conditions too.
 * Canonically, this works under a mechanism that simply ignores duplicate redeclarations and
 * merges them into previous declarations, and if declaration completes a type, for example in tag
 * type case, the type that was PREVIOUSLY created gets filled with values and completed (defined).
 *  But that brings an issue when used with the checkpoint and rollback system that this tool provides.
 * If rollback occurs, by the rules, all state is reverted to the checkpoint in order to reparse
 * ast tree. This includes deleting all types and bindings that were created AFTER checkpoint was set.
 * And that's the problem, since a binding with definition can occur AFTER checkpoint is set, and it
 * can complete (define) the type that was PREVIOUSLY created by an older declaration. So when rollback
 * occurs, the ast tree that made the definition is erased, but the definition is still saved in the
 * type, since it was first declared before the checkpoint, and therefore is not reverted to its
 * previous undefined state.
 *  Solution is then NOT to destroy existing binding structure, because it would likely only
 * overcomplicate and possibly break some C grammar rules, but rather to track binding indices in a
 * growing list. Each time a binding is defined, its index is added to this list. Then it's simple,
 * because checkpoint places a marker, and every index added after that marker is taken out and
 * undefined. This list is also scoped, so bindings that were removed on their own get their
 * indices removed from here as well.
 *
 * MEMBER type_arena: Arena that stores type structs, so that pointers to types never change.
 *
 * MEMBER type_children_arena: Arena that stores various type children, for example struct members, function parameters, etc.
 * Since each type has its own array of members, which doesn't overlap, all of them can be stored in a single arena.
 *
 * MEMBER type_ptr_set: Set of pointers to the types, used to deduplicate structural types.
 * Whenever such type is found, it is inserted into this set if it is not already there.
 *
 * MEMBER scoped_strings_arena: Arena that stores every scoped string data.
 * Pointers to the data remain the same throughout the execution,
 * since chained arena guarantees that memory once allocated is not moved.
 * Deallocation mechanism of the arena is used to pop string data once it is out of scope.
 * Used for binding names, such as functions, variables, typedefs and enums.
 *
 * MEMBER permanent_strings_arena: Arena that stores string data, that lives globally outside of source.
 * Pointers to the data remain the same throughout the execution,
 * since chained arena guarantees that memory once allocated is not moved.
 * Used for type names and persistent names, such as struct members and tags.
 *
 * MEMBER source: Source string of the code being parsed.
 *
 * MEMBER ast_arena: Arena that stores data of all nodes.
 *
 * MEMBER ptr_stack: Stack of `Cn_Ast_Node *` used during parsing to build `Cn_Ast_Nodes` arrays.
 * Parsing functions push nodes onto this stack, then finalize ranges
 * into `Cn_Ast_Nodes` when the list is complete.
 *
 * MEMBER error_count: Error count, incremented by `cn_diagnostic_handler` every time an error is reported.
 *
 * MEMBER warning_count: Warning count, incremented by `cn_diagnostic_handler` every time a warning is reported.
 *
 * MEMBER scope_stack: Regular array list used as a stack to properly handle scoping throughout parsing.
 *
 * MEMBER function_scope_idx: Index of the current function scope, `NIL` if not inside a function scope.
 *
 * MEMBER output_arena: Output generated by the emitter.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Binding         *binding_list;
    Cn_Binding_Idx     *tag_binding_table;
    Cn_Binding_Idx     *symbol_binding_table;
    Cn_Binding         *label_binding_list;
    Cn_Binding_Idx     *label_binding_table;
    Cn_Binding_Idx     *binding_defined_idx_list;
    Cn_Chained_Arena    type_arena;
    Cn_Chained_Arena    type_children_arena;
    Cn_Type           **type_ptr_set;
    Cn_Chained_Arena    scoped_strings_arena;
    Cn_Chained_Arena    permanent_strings_arena;
    Cn_String           source;
    Cn_Chained_Arena    ast_arena;
    Cn_Ast_Node       **ptr_stack;
    int64_t             error_count;
    int64_t             warning_count;
    Cn_Scope           *scope_stack;
    int64_t             function_scope_idx;
    Cn_Chained_Arena    output_arena;
    uint64_t            counter;
} Cn_Tu_Data;

extern Cn_Tu_Data *cn__tu_data;

/**
 * Struct to preserve state needed for full rollback of processing.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    uint64_t saved_output_length;
    uint64_t saved_ast_length;
    uint64_t saved_type_length;
    uint64_t saved_type_children_length;
    int64_t  saved_binding_length;
    int64_t  saved_label_binding_length;
    int64_t  saved_binding_defined_idx_length;
    uint64_t saved_scoped_strings_length;
    uint64_t saved_permanent_strings_length;
    int64_t  saved_scope_length;
    int64_t  saved_function_scope_idx;
    int64_t  saved_ptr_stack_length;
    uint64_t saved_counter;
} Cn_Tu_Saved;

// Saves current state of `cn__tu_data` to `state`.
CNDEF void cn__tu_save(Cn_Tu_Saved *state);

// Loads `state` back into `cn__tu_data`.
CNDEF void cn__tu_rollback(Cn_Tu_Saved *state);

// Emits code of `node` and then rolls back to the saved `state`.
// Makes sure new code is recorded right after the rollback.
CNDEF Cn_String cn__tu_emit_and_rollback(Cn_Ast_Node *node, Cn_Tu_Saved *state);

#define CN_AST_ARENA_BLOCK_CAP                      4096
#define CN_AST_TYPE_ARENA_BLOCK_CAP                 (sizeof(Cn_Type) * 64)
#define CN_AST_TYPE_PTR_SET_INITIAL_CAP             32
#define CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP        4096
#define CN_AST_BINDING_LIST_INITIAL_CAP             64
#define CN_AST_TAG_BINDING_TABLE_INITIAL_CAP        32
#define CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP     32
#define CN_AST_LABEL_BINDING_LIST_INITIAL_CAP       32
#define CN_AST_LABEL_BINDING_TABLE_INITIAL_CAP      16
#define CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP       4096
#define CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP    4096
#define CN_AST_SCOPE_STACK_INITIAL_CAP              32
#define CN_AST_PTR_STACK_INITIAL_CAP                128

/**
 * Flags that can be passed to `cn_tu_init` to enable or disable some behaviour.
 *
 * SINCE: 0.3.0
 */
typedef enum {
    CN_TU_PRINT_SOURCE   = 0x1,
    CN_TU_PRINT_TOKENS   = 0x2,
    CN_TU_PRINT_AST      = 0x4,
    CN_TU_PRINT_TYPES    = 0x8,
    CN_TU_PRINT_BINDINGS = 0x10,
    CN_TU_NO_CODE_OUTPUT = 0x20,
    CN__TU_OPT_SOURCE    = 0x40,
} Cn_Tu_Flags;

/**
 * Optionals that can be passed to `cn_tu_init`.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_String   source;
    char       *output_path;
    Cn_Tu_Flags flags;
} Cn_Tu_Make_Opt;

/**
 * Struct that represents one whole translation unit which canonically is a single `.c` file.
 *
 * SINCE: 0.3.0
 */
typedef struct {
    Cn_Tu_Flags flags;
    char       *output_path;
    char       *path;
    Cn_Tu_Data  data;
} Cn_Translation_Unit;

/**
 * Initializes translation unit `tu` from the pre-processed file at `intermidiate_path`.
 *
 * Translation unit represents classic C translation unit, in which all lexing, parsing,
 * inferring and modification occurs.
 *
 * Path must point to an `.i` file, that is the intermediate pre-processed representation of
 * a translation unit. Simply, it is a `.c` file that has all macros expanded and is ready to be
 * sent to the compiler. In `gcc` it would be generated by executing the following:
 *
 * ```
 * $ gcc -E -o file.i file.c
 * ```
 *
 * RETURNS: `true` on success, `false` on error.
 *
 * IMPORTANT: Supplying `.c` files with macros that are not expanded results in an error,
 * make sure to pass the `.c` file through the compiler's pre-processor first.
 *
 * SINCE: 0.3.0
 */
#define cn_tu_init(tu, intermidiate_path, ...) cn__tu_init_opt(tu, intermidiate_path, (Cn_Tu_Make_Opt) { __VA_ARGS__ })
CNDEF bool cn__tu_init_opt(Cn_Translation_Unit *tu, char *intermidiate_path, Cn_Tu_Make_Opt opt);

/**
 * Processes translation unit `tu` from top to bottom.
 *
 * Translation unit passes through Infer and Size stages. Meaning ast is built, type and symbol
 * tables are constructed, and sizes of the types are calculated too.
 *
 * Throughout the processing, various hooks are triggered, which can be intercepted to introspect
 * and modify code, state, etc. This can be done by adding custom hooks to `cn_hooks`.
 * See `Hook` section for more info.
 *
 * RETURNS: `0` if processing is successful, `-1` if an error occurred.
 *
 * SINCE: 0.3.0
 */
CNDEF int cn_tu_process(Cn_Translation_Unit *tu); 

/**
 * Frees all memory used by translation unit `tu`, including closing previously opened file.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_tu_free(Cn_Translation_Unit *tu);

/**
 * Logs current type universe in `tu`.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_tu_log_types(Cn_Translation_Unit *tu);

/**
 * Logs current bindings in `tu`.
 *
 * NOTE: Bindings are popped when their scope ends. 
 * This log shows only current scope visible bindings.
 *
 * SINCE: 0.3.0
 */
CNDEF void cn_tu_log_bindings(Cn_Translation_Unit *tu);


#endif // CN_H_
       

#ifdef CN_IMPLEMENTATION
#ifndef CN_IMPLEMENTATION_GUARD_
#define CN_IMPLEMENTATION_GUARD_

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

CNDEF bool cn_str_starts_with(Cn_String str, Cn_String start) {
    if (str.length == 0 && start.length == 0) return true;

    return cn_str_find_left(str, start) == 0;
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

CNDEF bool cn_str_is(Cn_String str1, Cn_String str2) {
    return cn_str_equals(&str1, &str2);
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

/**
 * Constructs new heap allocated string builder with specified initial capacity.
 *
 * IMPORTANT: Expects `initial_capacity > CN_SB_STACK_STORAGE_CAP`.
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

/**
 * Grows capacity of `sb`, so it can hold at least `required_length` bytes.
 *
 * Does nothing if it already fits.
 */
CNDEF void cn__sb_resize_to_fit(Cn_String_Builder *sb, int64_t required_length) {
    if (required_length <= sb->capacity) return;

    // IMPORTANT: To understand where this calculation comes from check cn__array_list_resize_to_fit implementation.
    // It uses same calculation that simplifies pow and log of base 2 caluclation to just using bit manipulation.
    int64_t ratio = required_length / sb->capacity;
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

CNDEF void cn_sb_append_str(Cn_String_Builder *sb, Cn_String str) {
    cn__sb_resize_to_fit(sb, sb->length + str.length);

    memcpy(sb->data + sb->length, str.data, str.length);
    sb->length += str.length;
}

CNDEF void cn_sb_prepend_str(Cn_String_Builder *sb, Cn_String str) {
    cn__sb_resize_to_fit(sb, sb->length + str.length);

    // Shifting first, so that the front is free for the incoming string.
    // memmove, not memcpy, because source and destination overlap.
    memmove(sb->data + str.length, sb->data, sb->length);
    memcpy(sb->data, str.data, str.length);
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
    header->capacity = block_capacity;
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

    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);

    if ((header->allocated + size) > header->capacity) {
        Cn_Chained_Arena_Block_Header *next = header->next;

        // Already chained block is reused only if the allocation actually fits in it.
        // Every block is at least block_capacity big, so this can only fail for oversized allocations.
        if (next == NULL || next->capacity < size) {
            size_t capacity = size > arena->block_capacity ? size : arena->block_capacity;

            if (capacity > arena->block_capacity) {
                cn_log(CN_WARNING, "Oversized allocation of %zu bytes in chained arena of %zu bytes block capacity, block of %zu bytes was allocated to hold it.", size, arena->block_capacity, capacity);
            }

            Cn_Chained_Arena_Block_Header *block = realloc(NULL, sizeof(Cn_Chained_Arena_Block_Header) + capacity);
            block->capacity = capacity;

            // Chaining in front of whatever was already there, so blocks that were too small
            // for this allocation are kept for the next one that fits in them.
            block->next = next;
            header->next = block;
            next = block;
        }

        header = next;

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
    while (header->next != NULL) {
        header = (Cn_Chained_Arena_Block_Header *) header->next;
    }

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

Cn_Allocator cn__std_allocator = {
    .alloc      = (Cn_Alloc *)cn_std_alloc, 
    .realloc    = (Cn_Realloc *)cn_std_realloc, 
    .free       = (Cn_Free *)cn_std_free, 
    .free_all   = (Cn_Free_All *)cn_std_free_all
};

Cn_Allocator *cn_allocator = &cn__std_allocator;

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
    { CN_TOKEN_CONST,           CN_STR_BUFFER("const") },
    { CN_TOKEN_RESTRICT,        CN_STR_BUFFER("restrict") },
    { CN_TOKEN_VOLATILE,        CN_STR_BUFFER("volatile") },
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
    { CN_TOKEN_CASE,            CN_STR_BUFFER("case") },
    { CN_TOKEN_DEFAULT,         CN_STR_BUFFER("default") },
    { CN_TOKEN_COMPLEX,         CN_STR_BUFFER("_Complex") },
    { CN_TOKEN_IMAGINARY,       CN_STR_BUFFER("_Imaginary") },

    // TEMPORARY: Post C99.
    // { CN_TOKEN_NORETURN,        CN_STR_BUFFER("_Noreturn") },
    // { CN_TOKEN_ATOMIC,          CN_STR_BUFFER("_Atomic") },

#ifdef CN_GNU
    // GNU keywords.
    { CN_TOKEN_GNU_ASM,         CN_STR_BUFFER("asm") },
    { CN_TOKEN_GNU_ASM,         CN_STR_BUFFER("__asm__") },
    { CN_TOKEN_GNU_ASM,         CN_STR_BUFFER("__asm") },
    { CN_TOKEN_GNU_EXTENSION,   CN_STR_BUFFER("__extension__") }, 
    { CN_TOKEN_GNU_ATTRIBUTE,   CN_STR_BUFFER("__attribute__") }, 
    { CN_TOKEN_GNU_TYPEOF,      CN_STR_BUFFER("typeof") },
    { CN_TOKEN_GNU_TYPEOF,      CN_STR_BUFFER("__typeof") },
    { CN_TOKEN_GNU_TYPEOF,      CN_STR_BUFFER("__typeof__") },
#endif

    { CN_TOKEN_RESTRICT,        CN_STR_BUFFER("__restrict") },
    { CN_TOKEN_INLINE,          CN_STR_BUFFER("__inline") },
    { CN_TOKEN_INLINE,          CN_STR_BUFFER("__inline__") },
    { CN_TOKEN_VOLATILE,        CN_STR_BUFFER("__volatile__") },
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
 * RETURNS: `true` if newline character was eaten.
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
 * RETURNS: `true` if stopped right after after newline character was eaten.
 */
CNDEF void cn__lexer_consume_spaces(Cn_Lexer *l) {

    while (l->cursor < l->content.length && isspace(l->content.data[l->cursor])) {
        cn__lexer_eat_char(l);
    }

}

/**
 * RETURNS: `true` if reached end of file.
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
    token->kind = CN_TOKEN_UNKNOWN;
}

/**
 * RETURNS: `true` if token is blacklisted.
 */
CNDEF bool cn__lexer_blacklisted(Cn_Token *token, Cn_Lexer_Blacklist *blacklist) {
    for (int64_t i = 0; i < blacklist->length; i++) {
        if (token->kind == blacklist->ttypes[i]) return true;
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
        .kind = CN_TOKEN_UNKNOWN,
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
        token.kind = CN_TOKEN_EOF;
        return token;
    }

    if (cn__valid_number_start(l->content.data[l->cursor])) {
        int digit_count = 0;
        token.kind = CN_TOKEN_INTEGER_VALUE;
        
        digit_count += cn__lexer_get_digits(l, &token);
        if (cn__lexer_eof(l)) return token;

        if (l->content.data[l->cursor] == '.') {
            token.kind = CN_TOKEN_FLOAT_VALUE;
            token.src.length++;
            l->cursor++;
        }
       
        if (token.kind == CN_TOKEN_FLOAT_VALUE && digit_count == 0) {
            cn__lexer_backtrack_token(l, &token);
            goto number_lexer_fail;
        }

        digit_count += cn__lexer_get_digits(l, &token);
        if (cn__lexer_eof(l)) return token;
        
        if (l->content.data[l->cursor] == 'e' || l->content.data[l->cursor] == 'E') {
            token.kind = CN_TOKEN_FLOAT_VALUE;
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

        if (token.kind == CN_TOKEN_FLOAT_VALUE) {
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
        token.kind = CN_TOKEN_LINE_MARKER;
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
        token.kind = CN_TOKEN_STRING;
        token.src.length++;
        l->cursor++;

        while (l->cursor < l->content.length) {
            if (l->content.data[l->cursor] == '"') {
                // Is a valid string end.
                break;
            }

            if (l->content.data[l->cursor] == '\\') {
                token.src.length++;
                l->cursor++;

                // TODO: For the future, error reporting in lexer, is not implemented?
                // Although, the lexer does return unknown token on failed lexing, sometimes that state is not reached.
                // It doesn't catch any errors related to incorrect token syntaxing that is parsed during lexing...
                // However such errors only occur in cases where either lexer is built wrong or supplied source is substantially damaged.
                // Either way making lexer be more robust, will probably be required as library grows more...
            }

            token.src.length++;
            l->cursor++;
        }

        token.src.length++;
        l->cursor++;

        return token;
    }

    if (l->content.data[l->cursor] == '\'') {
        token.kind = CN_TOKEN_CHAR_VALUE;
        token.src.length++;
        l->cursor++;

        while (l->cursor < l->content.length) {
            if (l->content.data[l->cursor] == '\'') {
                // Is a valid char end.
                break;
            }

            if (l->content.data[l->cursor] == '\\') {
                token.src.length++;
                l->cursor++;
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
            token.kind = CN_TOKEN_COMMENT;
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
            token.kind = CN_TOKEN_COMMENT;
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
                token.kind = CN_LITERAL_TOKENS[i].kind;
                token.src.length = CN_LITERAL_TOKENS[i].literal.length;
                l->cursor += token.src.length;
                return token;
            }
        }
    }


    if (cn__valid_symbol_start(l->content.data[l->cursor])) {
        token.kind = CN_TOKEN_IDENTIFIER;
        while (l->cursor < l->content.length && cn__valid_symbol(l->content.data[l->cursor])) {
            token.src.length++;
            l->cursor++;
        }
       
        Cn_String symbol = cn_source_to_str(&token.src);
        for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_KEYWORD_TOKENS); i++) {
            if (cn_str_equals(&CN_KEYWORD_TOKENS[i].literal, &symbol)) {
                token.kind = CN_KEYWORD_TOKENS[i].kind;
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
    lexer->blacklist = blacklist;

    cn_lexer_load_content(lexer, content);
}

CNDEF void cn_lexer_load_content(Cn_Lexer *lexer, Cn_String content) {
    lexer->cursor   = 0;
    lexer->bol      = 0;
    lexer->content  = content;

    // Fill queue.
    lexer->head_idx     = 0;
    lexer->tail_idx     = 0;
    lexer->current_idx  = 0;
    Cn_Token token;
    for (int64_t i = 0; i < CN_LEXER_QUEUE_COUNT; i++) {
        do {
            token = cn__lexer_lex(lexer);
        } while(cn__lexer_blacklisted(&token, &lexer->blacklist));

        lexer->queue[i] = token;

        if (lexer->queue[i].kind == CN_TOKEN_EOF) {
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
    if (cn_lexer_head(l).kind == CN_TOKEN_EOF) {
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

CNDEF bool cn_lexer_expect(Cn_Lexer *lexer, Cn_Token_Kind kind) {
    return cn_lexer_token(lexer).kind == kind;
}

CNDEF Cn_Token cn_lexer_peek(Cn_Lexer *lexer, int64_t offset) {
    CN_ASSERT(-CN_LEXER_LOOKDOWN_COUNT < offset && offset < CN_LEXER_LOOKUP_COUNT);
    
    // Peeking past tokens.
    if (offset < 0) {
        if (-offset > cn__lexer_tail_dist(lexer)) {
            return (Cn_Token) { .kind = CN_TOKEN_UNKNOWN };
        }

        return lexer->queue[(lexer->current_idx + offset) % CN_LEXER_QUEUE_COUNT];
    }

    if (offset > cn__lexer_head_dist(lexer)) {
        if (cn_lexer_head(lexer).kind == CN_TOKEN_EOF) return cn_lexer_head(lexer);

        return (Cn_Token) { .kind = CN_TOKEN_UNKNOWN };
    }
    
    return lexer->queue[(lexer->current_idx + offset) % CN_LEXER_QUEUE_COUNT];
}

CNDEF void cn_lexer_log_tokens(Cn_Lexer *lexer, int64_t n) {
    Cn_Lexer copy = *lexer;
    cn_log(CN_INFO, "Tokenized:" CN_ANSI_CYAN);
    do {
        fprintf(stderr, "TOKEN:     %s\n", cn_token_kind_name(cn_lexer_token(&copy).kind));
        cn_lexer_next_token(&copy);

        if (n > 0) {
            n--;
            if (n == 0) break;
        }
    } while (cn_lexer_token(&copy).kind != CN_TOKEN_EOF);

    fprintf(stderr, CN_ANSI_RESET"\n");
}

CNDEF bool cn_lexer_changed(Cn_Lexer *lexer, Cn_Lexer *saved) {
    if (lexer->cursor != saved->cursor) return true;

    // Once EOF is lexed into the queue, cursor stops moving and consuming tokens only moves queue indices.
    if (lexer->current_idx != saved->current_idx) return true;
    if (lexer->head_idx != saved->head_idx || lexer->tail_idx != saved->tail_idx) return true;

    // Checking queue.
    for (int i = 0; i < CN_LEXER_QUEUE_COUNT; i++) {
        if (!cn_token_equals(&lexer->queue[i], &saved->queue[i])) return true;
    }

    return false;
}

CNDEF const char *cn_token_kind_name(Cn_Token_Kind kind) {
    switch(kind) {
        case CN_TOKEN_EOF:              return "<eof>";
        case CN_TOKEN_UNKNOWN:          return "<unknown>";
        case CN_TOKEN_STRING:           return "<string literal>";
        case CN_TOKEN_COMMENT:          return "<comment>";
        case CN_TOKEN_LINE_MARKER:      return "<line marker>";
        case CN_TOKEN_INTEGER_VALUE:    return "<integer literal>";
        case CN_TOKEN_FLOAT_VALUE:      return "<float literal>";
        case CN_TOKEN_CHAR_VALUE:       return "<value literal>";
        case CN_TOKEN_IDENTIFIER:       return "<identifier>";
        // Literal tokens.
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
        // Keyword tokens.
        case CN_TOKEN_CONST:            return "const";
        case CN_TOKEN_RESTRICT:         return "restrict";
        case CN_TOKEN_VOLATILE:         return "volatile";
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
        case CN_TOKEN_CASE:             return "case";
        case CN_TOKEN_DEFAULT:          return "default";
        case CN_TOKEN_COMPLEX:          return "_Complex";
        case CN_TOKEN_IMAGINARY:        return "_Imaginary";

        // TEMPORARY: Post C99.
        case CN_TOKEN_NORETURN:         return "_Noreturn";
        case CN_TOKEN_ATOMIC:           return "_Atomic";

        // TEMPORARY: GNU keywords.
#ifdef CN_GNU
        case CN_TOKEN_GNU_ASM:          return "asm";
        case CN_TOKEN_GNU_EXTENSION:    return "__extension__";
        case CN_TOKEN_GNU_ATTRIBUTE:    return "__attribute__";
        case CN_TOKEN_GNU_TYPEOF:       return "typeof";
#endif
}

    return "<invalid token>";
}

CNDEF bool cn_token_equals(Cn_Token *a, Cn_Token *b) {
    if (a == b) return true;

    return  a->kind == b->kind                              &&
            a->loc.column == b->loc.column                  &&
            a->loc.line == b->loc.line                      &&
            a->src.length == b->src.length                  &&
            a->src.offset == b->src.offset                  &&
            cn_str_equals(&a->loc.file, &b->loc.file)       &&
            cn_str_equals(&a->src.source, &b->src.source)
            ;
}

CNDEF void cn_lexer_print_snippet(Cn_Lexer *lexer, uint64_t index, int64_t length) {
    Cn_String line = cn_str_eat_chars(lexer->content, lexer->bol);
    int64_t end = cn_str_find_char_left(line, '\n');
    if (end == -1) {
        end = lexer->content.length - lexer->bol;
    }
    line = cn_str_substring(line, 0, end);
    uint64_t underline_offset = index - lexer->bol;

    fprintf(stderr, "\n%4lu | %.*s", lexer->line, CN_STR_UNPACK(cn_str_substring(line, 0, underline_offset)));
    fprintf(stderr, "\033[31m%.*s\033[0m", CN_STR_UNPACK(cn_str_substring(line, underline_offset, underline_offset + length)));
    fprintf(stderr, "%.*s\n", CN_STR_UNPACK(cn_str_substring(line, underline_offset + length, line.length)));
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

const Cn_Type CN_TYPE_DOUBLE = { 
    .flags = CN_TYPE_COMPLETE,
    .size = sizeof(double), 
    .align = sizeof(double), 
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
    if (a->size != b->size || a->align != b->align) return false;

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
            CN_TODO("Implement structural equality for enum type.");
            break;
        case CN_UNION:
            CN_TODO("Implement structural equality for union type.");
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
        case CN_INTEGER: 
            {
                const Cn_Type_Integer *ti = (const Cn_Type_Integer *)type;
                hash = cn_hash_mix(hash, cn_hash_u64(ti->is_signed));
                break;
            } 
        case CN_POINTER: 
            {
                const Cn_Type_Pointer *tp = (const Cn_Type_Pointer *)type;
                hash = cn_hash_mix(hash, cn_hash_ptr(tp->ptr_to));
                break;
            } 
        case CN_FUNCTION: 
            {
                const Cn_Type_Function *tf = (const Cn_Type_Function *)type;
                hash = cn_hash_mix(hash, cn_hash_ptr(tf->return_type));
                hash = cn_hash_mix(hash, cn_hash_u64(tf->params_length));
                for (int64_t i = 0; i < tf->params_length; i++) {
                    hash = cn_hash_mix(hash, cn_hash_ptr(tf->params[i].type));
                }
                break;
            } 
        case CN_ARRAY: 
            {
                const Cn_Type_Array *ta = (const Cn_Type_Array *)type;
                hash = cn_hash_mix(hash, cn_hash_ptr(ta->element_type));
                hash = cn_hash_mix(hash, cn_hash_u64(ta->length));
                break;
            } 
        case CN_STRUCT: 
            {
                const Cn_Type_Struct *ts = (const Cn_Type_Struct *)type;
                hash = cn_hash_mix(hash, cn_str_hash(&ts->tag));
                hash = cn_hash_mix(hash, cn_hash_u64(ts->members_length));
                for (int64_t i = 0; i < ts->members_length; i++) {
                    hash = cn_hash_mix(hash, cn_hash_ptr(ts->members[i].type));
                }
                break;
            } 
        case CN_ENUM: 
            {
                const Cn_Type_Enum *te = (const Cn_Type_Enum *)type;
                hash = cn_hash_mix(hash, cn_str_hash(&te->tag));
                break;
            } 
        case CN_UNION:
            {
                CN_TODO("Implement union for enum type.");
                break;
            }
        case CN_QUALIFIED: 
            {
                const Cn_Type_Qualified *tq = (const Cn_Type_Qualified *)type;
                hash = cn_hash_mix(hash, cn_hash_ptr(tq->base_type));
                break;
            } 
        default:
            break;
    }

    return hash;
}

CNDEF void cn__type_stringify(const Cn_Type *type, Cn_String_Builder *left, Cn_String_Builder *right) {

    switch (type->kind) {
        case CN_OPAQUE:
            cn_sb_append_str(left, CN_STR_LIT("<opaque>"));  return;
        case CN_VOID:
            cn_sb_append_str(left, CN_STR_LIT("void"));      return;
        case CN_BOOL:
            cn_sb_append_str(left, CN_STR_LIT("_Bool"));     return;
        case CN_UNKNOWN:
            cn_sb_append_str(left, CN_STR_LIT("<unknown>")); return;

        case CN_INTEGER: {
            const Cn_Type_Integer *ti = (const Cn_Type_Integer *)type;
            if (!ti->is_signed) cn_sb_append_str(left, CN_STR_LIT("unsigned "));
            switch (type->size) {
                case 1:  cn_sb_append_str(left, CN_STR_LIT("char"));      return;
                case 2:  cn_sb_append_str(left, CN_STR_LIT("short"));     return;
                case 4:  cn_sb_append_str(left, CN_STR_LIT("int"));       return;
                case 8:  cn_sb_append_str(left, CN_STR_LIT("long long")); return;
                default: cn_sb_append_format(left, "int%lld_t", type->size * 8); return;
            }
        }

        case CN_FLOAT: {
            switch (type->size) {
                case 4:  cn_sb_append_str(left, CN_STR_LIT("float"));       return;
                case 8:  cn_sb_append_str(left, CN_STR_LIT("double"));      return;
                case 16: cn_sb_append_str(left, CN_STR_LIT("long double")); return;
                default: cn_sb_append_format(left, "float%lld", type->size * 8); return;
            }
        }

        case CN_STRUCT: {
            const Cn_Type_Struct *ts = (const Cn_Type_Struct *)type;
            cn_sb_append_str(left, CN_STR_LIT("struct "));
            cn_sb_append_str(left, ts->tag);
            return;
        }

        case CN_UNION: {
            const Cn_Type_Union *tu = (const Cn_Type_Union *)type;
            cn_sb_append_str(left, CN_STR_LIT("union "));
            cn_sb_append_str(left, tu->tag);
            return;
        }

        case CN_ENUM: {
            const Cn_Type_Enum *te = (const Cn_Type_Enum *)type;
            cn_sb_append_str(left, CN_STR_LIT("enum "));
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

                cn_sb_append_str(left, CN_STR_LIT("(*"));

                cn_sb_append_char(right, ')');
                cn_sb_append_str(right, cn_sb_to_str(&inner_right));

                cn_sb_free(&inner_right);
            } else {
                cn__type_stringify(inner, left, right);

                if (needs_space) cn_sb_append_char(left, ' ');

                cn_sb_append_str(left, CN_STR_LIT("*"));
            }

            return;
        }

        case CN_ARRAY: {
            const Cn_Type_Array *ta = (const Cn_Type_Array *)type;
            cn__type_stringify(ta->element_type, left, right);

            // Suffix goes in FRONT of whatever the element type already put into 'right'.
            // Recursion runs outermost first, so prepending is what keeps outer dimensions
            // ahead of inner ones ("int m[3][4]") and keeps the brackets inside the
            // parentheses for an array of function pointers ("int (*fps[3])(int)").
            Cn_String_Builder suffix = cn_sb_make(32);

            if (!(ta->flags & CN_TYPE_COMPLETE)) {
                cn_sb_append_str(&suffix, CN_STR_LIT("[]"));
            } else if (ta->flags & CN_TYPE_ARRAY_UNSPECIFIED) {
                cn_sb_append_str(&suffix, CN_STR_LIT("[*]"));
            } else if (ta->flags & CN_TYPE_ARRAY_VARIABLE) {
                cn_sb_append_str(&suffix, CN_STR_LIT("[]"));    // Have a better way to stringify VLA?
            } else {
                cn_sb_append_format(&suffix, "[%lld]", ta->length);
            }

            cn_sb_prepend_str(right, cn_sb_to_str(&suffix));

            cn_sb_free(&suffix);
            return;
        }

        case CN_FUNCTION: {
            const Cn_Type_Function *tf = (const Cn_Type_Function *)type;
            cn__type_stringify(tf->return_type, left, right);

            // Same reasoning as CN_ARRAY: the parameter list has to land in front of any
            // suffix the return type left behind, so it is built separately and prepended
            // in one go. That is what gives "int (*f(void))[10]" instead of "int (*f)[10](void)".
            Cn_String_Builder params = cn_sb_make(32);

            cn_sb_append_char(&params, '(');
            if (tf->params_length == 0) {
                cn_sb_append_str(&params, CN_STR_LIT("void"));
            } else {
                for (int64_t i = 0; i < tf->params_length; i++) {
                    if (i > 0) cn_sb_append_str(&params, CN_STR_LIT(", "));

                    Cn_String_Builder param_left  = cn_sb_make(32);
                    Cn_String_Builder param_right = cn_sb_make(32);

                    cn__type_stringify(tf->params[i].type, &param_left, &param_right);

                    cn_sb_append_str(&params, cn_sb_to_str(&param_left));
                    cn_sb_append_str(&params, cn_sb_to_str(&param_right));

                    cn_sb_free(&param_left);
                    cn_sb_free(&param_right);
                }
            }
            cn_sb_append_char(&params, ')');

            cn_sb_prepend_str(right, cn_sb_to_str(&params));

            cn_sb_free(&params);
            return;
        }

        case CN_QUALIFIED: {
            const Cn_Type_Qualified *tq = (const Cn_Type_Qualified *)type;
            if (tq->base_type->kind == CN_POINTER) {
                cn__type_stringify(tq->base_type, left, right);
                cn_sb_append_char(left, ' ');
            }

            if (type->flags & CN_TYPE_QUALIFIED_CONSTANT)
                cn_sb_append_str(left, CN_STR_LIT("const "));
            if (type->flags & CN_TYPE_QUALIFIED_VOLATILE)
                cn_sb_append_str(left, CN_STR_LIT("volatile "));
            if (type->flags & CN_TYPE_QUALIFIED_RESTRICT)
                cn_sb_append_str(left, CN_STR_LIT("restrict "));
            if (type->flags & CN_TYPE_QUALIFIED_ATOMIC)
                cn_sb_append_str(left, CN_STR_LIT("_Atomic "));

            if (tq->base_type->kind != CN_POINTER) {
                cn__type_stringify(tq->base_type, left, right);
            }

            return;
        }
    }
}

CNDEF Cn_String cn_type_stringify(Cn_String buffer, const Cn_Type *type, Cn_String identifier) {
    Cn_String_Builder left  = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    Cn_String_Builder right = cn_sb_make(CN_SB_STACK_STORAGE_CAP);

    cn__type_stringify(type, &left, &right);

    // The declarator name goes between the two halves, not after them.
    // That is the whole reason 'right' exists: for arrays and function pointers the
    // suffix has to stay outside the name, giving "int a[10]" and "int (*f)(void)"
    // rather than "int[10] a" and "int (*)(void) f".
    bool has_identifier = identifier.data != NULL && identifier.length > 0;

    // A separator is only wanted when 'left' ends on a type name. When it ends on
    // '*' or '(' the name has to sit flush against it, as in "int *p".
    bool needs_space = false;
    if (has_identifier && left.length > 0) {
        char last = left.data[left.length - 1];
        needs_space = (last != '*' && last != '(');
    }

    int64_t total = left.length + right.length;
    if (has_identifier) total += identifier.length;
    if (needs_space)    total += 1;

    if (total > buffer.length) {
        cn_log(CN_ERROR, "cn_type_stringify: buffer too small, needed %lld got %lld", total, buffer.length);
        cn_sb_free(&left);
        cn_sb_free(&right);
        return CN_STR(0, NULL);
    }

    int64_t offset = 0;

    memcpy(buffer.data + offset, left.data, left.length);
    offset += left.length;

    if (needs_space) buffer.data[offset++] = ' ';

    if (has_identifier) {
        memcpy(buffer.data + offset, identifier.data, identifier.length);
        offset += identifier.length;
    }

    memcpy(buffer.data + offset, right.data, right.length);

    cn_sb_free(&left);
    cn_sb_free(&right);

    return CN_STR(total, buffer.data);
}


CNDEF void cn_type_print(const Cn_Type *t) {
    if (t == NULL) { fputs("<null>", stderr); return; }

    char buf[256];
    Cn_String str = cn_type_stringify(CN_STR(sizeof(buf), buf), t, (Cn_String) {0});
    if (str.data == NULL) {
        fputs("<stringify failed>", stderr);
        return;
    }

    fprintf(stderr, "%.*s", CN_STR_UNPACK(str));
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
    // _Bool is unsigned integer type in C. 
    return type->kind == CN_INTEGER || type->kind == CN_FLOAT || type->kind == CN_ENUM || type->kind == CN_BOOL || type->kind == CN_OPAQUE;
}

CNDEF bool cn_type_is_assignable(const Cn_Type *to, const Cn_Type *from) {
    to = cn_type_unqualified(to);
    from = cn_type_unqualified(from);

    if (to == from) return true;

    // If either type is opaque, then trust those are assignable.
    if (to->kind == CN_OPAQUE || from->kind == CN_OPAQUE)
        return true;

    // Any pointer can be assigned to bool.
    if (to->kind == CN_BOOL && from->kind == CN_POINTER) return true;

    if (cn_type_is_arithmetic(to) && cn_type_is_arithmetic(from)) return true;

    // If to is pointer.
    if (to->kind == CN_POINTER) {

        // If from is pointer.
        if (from->kind == CN_POINTER) {
            const Cn_Type_Pointer *to_ptr = (const Cn_Type_Pointer *)to;
            const Cn_Type_Pointer *from_ptr = (const Cn_Type_Pointer *)from;
            // Void pointers can be assigned in either direction without explicit casting.
            if (cn_type_unqualified(to_ptr->ptr_to)->kind == CN_VOID || cn_type_unqualified(from_ptr->ptr_to)->kind == CN_VOID) return true;

            return cn_type_is_compatible_no_qualifiers(to_ptr->ptr_to, from_ptr->ptr_to);
        }

        // If from is an array of same base type. 
        if (from->kind == CN_ARRAY) {
            const Cn_Type_Pointer *to_ptr = (const Cn_Type_Pointer *)to;
            const Cn_Type_Array *from_arr = (const Cn_Type_Array *)from;

            // Any array to void pointer can be assigned without explicit casting.
            if (cn_type_unqualified(to_ptr->ptr_to)->kind == CN_VOID) return true;

            return cn_type_is_compatible_no_qualifiers(to_ptr->ptr_to, from_arr->element_type);
        }

        // If from is a function, then it decays to the pointer to the function type.
        if (from->kind == CN_FUNCTION) {
            const Cn_Type_Pointer *to_ptr = (const Cn_Type_Pointer *)to;

            if (cn_type_unqualified(to_ptr->ptr_to)->kind == CN_VOID) return true;

            return cn_type_is_compatible_no_qualifiers(to_ptr->ptr_to, from);
        }
    }


    return false;
}

/**
 * Helper function, collects qualifier flags across the whole qualified chain,
 * since qualifiers can stack through typedef.
 *
 * RETURNS: Union of all qualifier flags, without CN_TYPE_COMPLETE.
 */
CNDEF Cn_Type_Flags cn__type_qualifier_flags(const Cn_Type *type) {
    Cn_Type_Flags flags = 0;
    while (type->kind == CN_QUALIFIED) {
        flags |= type->flags;
        type = ((const Cn_Type_Qualified *)type)->base_type;
    }
    return flags & ~CN_TYPE_COMPLETE;
}

/**
 * Helper function, compares two parameter types the way they would be adjusted
 * in a prototype: top-level qualifiers are dropped, arrays decay to pointers to
 * their element type and functions decay to pointers to function.
 *
 * RETURNS: `true` if adjusted parameter types are compatible.
 */
CNDEF bool cn__type_is_compatible_param(const Cn_Type *a, const Cn_Type *b) {
    // TODO: This function might be unnecessary. C does perfom decaying of arrays to pointers, and functions to pointers to the functions.
    // But I wonder if actually enforcing stricter rules that are only written in `cn_type_is_compatible` is actually better.
    // Or making array and pointer be compatible by default...

    a = cn_type_unqualified(a);
    b = cn_type_unqualified(b);

    // Adjusting to the pointed-to type, so 'int a[]' and 'int *a' compare equal.
    bool a_is_ptr = a->kind == CN_POINTER || a->kind == CN_ARRAY || a->kind == CN_FUNCTION;
    bool b_is_ptr = b->kind == CN_POINTER || b->kind == CN_ARRAY || b->kind == CN_FUNCTION;
    if (a_is_ptr && b_is_ptr) {
        const Cn_Type *a_to = a->kind == CN_POINTER ? a->pointer.ptr_to : a->kind == CN_ARRAY ? a->array.element_type : a;
        const Cn_Type *b_to = b->kind == CN_POINTER ? b->pointer.ptr_to : b->kind == CN_ARRAY ? b->array.element_type : b;
        return cn_type_is_compatible(a_to, b_to);
    }

    return cn_type_is_compatible(a, b);
}

CNDEF bool cn_type_is_compatible(const Cn_Type *a, const Cn_Type *b) {
    if (a == b) return true;

    // If either type is opaque, then trust those are compatible.
    if (a->kind == CN_OPAQUE || b->kind == CN_OPAQUE) return true;

    // Qualifiers must match exactly, then compare unqualified types.
    if (a->kind == CN_QUALIFIED || b->kind == CN_QUALIFIED) {
        if (cn__type_qualifier_flags(a) != cn__type_qualifier_flags(b)) return false;
        return cn_type_is_compatible(cn_type_unqualified(a), cn_type_unqualified(b));
    }

    if (a->kind != b->kind) return false;

    switch (a->kind) {
        case CN_POINTER:
            return cn_type_is_compatible(a->pointer.ptr_to, b->pointer.ptr_to);

        case CN_ARRAY: {
            if (!cn_type_is_compatible(a->array.element_type, b->array.element_type)) return false;
            // Incomplete array is compatible with any length.
            // VLA is also compatible with any length, once it is represented.
            if (!(a->flags & CN_TYPE_COMPLETE) || (a->flags & CN_TYPE_ARRAY_VARIABLE) || !(b->flags & CN_TYPE_COMPLETE) || (b->flags & CN_TYPE_ARRAY_VARIABLE)) return true;

            return a->array.length == b->array.length;
        }

        case CN_FUNCTION: {
            const Cn_Type_Function *fa = &a->function;
            const Cn_Type_Function *fb = &b->function;
            if (fa->is_variadic != fb->is_variadic) return false;
            if (fa->params_length != fb->params_length) return false;
            if (!cn_type_is_compatible(fa->return_type, fb->return_type)) return false;
            for (int64_t i = 0; i < fa->params_length; i++) {
                if (!cn__type_is_compatible_param(fa->params[i].type, fb->params[i].type)) return false;
            }
            return true;
        }

        // Tagged types share identity, so recursion never descends into members,
        // which also keeps self-referential structs from looping.
        // If you ask: "why not compare tag names here?" we can and it would be correct.
        // But if tag names are equal that implies that a == b is true above.
        // Because tag types are uniquenly stored by, well, tags. 
        // So doing string comparising here is redundant.
        case CN_STRUCT:
        case CN_UNION:
        case CN_ENUM:
            return false;

        default:
            // Scalars and void, structural equality.
            return cn_type_equals(a, b);
    }
}

CNDEF bool cn_type_is_compatible_no_qualifiers(const Cn_Type *a, const Cn_Type *b) {
    return cn_type_is_compatible(cn_type_unqualified(a), cn_type_unqualified(b));
}

CNDEF Cn_Type *cn_type_greatest_arithmetic_rank(const Cn_Type *a, const Cn_Type *b) {
    // AI: Generated.
    // Opaque rank is unknown, so result is trusted to be opaque.
    if (a->kind == CN_OPAQUE) return (Cn_Type *)a;
    if (b->kind == CN_OPAQUE) return (Cn_Type *)b;

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
    // Opaque is trusted to be a scalar, 
    // for example `__builtin_mul_overflow` used as a condition.
    return cn_type_is_arithmetic(type) || type->kind == CN_POINTER || type->kind == CN_OPAQUE;
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
 * Permanently saves string.
 */
CNDEF Cn_String cn__ast_permanent_save_string(Cn_String str) {
    void *data = cn_chained_arena_alloc(&cn__tu_data->permanent_strings_arena, str.length);
    cn_str_copy_to(str, data);
    str.data = data;

    return str;
}

CNDEF Cn_Type *cn_analyze_intern_type(Cn_Type *type) {
    if (!cn_hash_set_contains(&cn__tu_data->type_ptr_set, type)) {
        // Arena allocate memory for type.
        Cn_Type *result = cn_chained_arena_alloc(&cn__tu_data->type_arena, sizeof(Cn_Type));
        *result = *type;

        return cn_hash_set_put(&cn__tu_data->type_ptr_set, result);
    } else {
        // Acts as a get, since type is already in there, 
        // the put will return actual item stored in the hash set.
        return cn_hash_set_put(&cn__tu_data->type_ptr_set, type);
    }
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Scope
 * ============================================
 */

CNDEF Cn_Scope *cn_scope_stack_peek() {
    if (cn_array_list_length(&cn__tu_data->scope_stack) == 0) return NULL;

    return cn__tu_data->scope_stack + (cn_array_list_length(&cn__tu_data->scope_stack) - 1);
}

CNDEF void cn_scope_stack_push() {
    cn_array_list_append(&cn__tu_data->scope_stack, ((Cn_Scope) { 
                // .saved_strings_idx = cn_array_list_length(&cn_ast_saved_strings_list), 
                .saved_strings_length = 0,
                .binding_idx = cn_array_list_length(&cn__tu_data->binding_list),
                .label_binding_idx = cn_array_list_length(&cn__tu_data->label_binding_list),
                .binding_defined_idx = cn_array_list_length(&cn__tu_data->binding_defined_idx_list),
                } ));
}

CNDEF void cn_scope_stack_pop() {
    CN_ASSERT(cn_array_list_length(&cn__tu_data->scope_stack) > 0);

    Cn_Scope *scope = cn_scope_stack_peek();

    int64_t binding_list_length = cn_array_list_length(&cn__tu_data->binding_list);

    // AI: Assisted.
    // Undo bindings newest-first, so the restored next_idx values are always valid.
    for (int64_t i = binding_list_length - 1; i >= scope->binding_idx; i--) {
        Cn_Binding *b = &cn__tu_data->binding_list[i];
        if (b->kind == CN_BINDING_BUILTIN) continue;

        Cn_Binding_Idx **table = (b->kind == CN_BINDING_TAG)
            ? &cn__tu_data->tag_binding_table
            : &cn__tu_data->symbol_binding_table;

        if (b->next_idx == CN_BINDING_NIL_IDX) {
            cn_hash_table_remove(table, &b->name);
        } else {
            cn_hash_table_put(table, b->next_idx, &b->name);
        }
    }

    // AI: Assisted.
    // Builtins are never removed once added, instead they live through the stack pop.
    // Compact them to the front of this scope's region and re-point the table at their new indices.
    int64_t preserved_builtin_offset = 0;
    for (int64_t i = scope->binding_idx; i < binding_list_length; i++) {
        if (cn__tu_data->binding_list[i].kind != CN_BINDING_BUILTIN) continue;

        int64_t dst = scope->binding_idx + preserved_builtin_offset++;
        cn__tu_data->binding_list[dst] = cn__tu_data->binding_list[i];
        cn_hash_table_put(&cn__tu_data->symbol_binding_table, dst, &cn__tu_data->binding_list[dst].name);
    }

    cn_array_list_pop_multiple(&cn__tu_data->binding_list, binding_list_length - (scope->binding_idx + preserved_builtin_offset));

    if (CN_SCOPE_STACK_CURRENT_IDX == cn__tu_data->function_scope_idx) {

        // Removing label bindings from hash table entries.
        for (int i = scope->label_binding_idx; i < cn_array_list_length(&cn__tu_data->label_binding_list); i++) {
            // Resolve each binding, properly dispose each binding.
            if (cn__tu_data->label_binding_list[i].next_idx == CN_BINDING_NIL_IDX) {
                cn_hash_table_remove(&cn__tu_data->label_binding_table, &cn__tu_data->label_binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__tu_data->label_binding_table, cn__tu_data->label_binding_list[i].next_idx, &cn__tu_data->label_binding_list[i].name);
            }
        }
        cn_array_list_pop_multiple(&cn__tu_data->label_binding_list, cn_array_list_length(&cn__tu_data->label_binding_list) - scope->label_binding_idx);

        cn__tu_data->function_scope_idx = 0;
    }

    cn_array_list_pop_multiple(&cn__tu_data->binding_defined_idx_list, cn_array_list_length(&cn__tu_data->binding_defined_idx_list) - scope->binding_defined_idx);

    cn_chained_arena_dealloc(&cn__tu_data->scoped_strings_arena, scope->saved_strings_length);

    cn_array_list_pop(&cn__tu_data->scope_stack);
}

/**
 * Saves string respective to the current scope.
 * Once scope is popped the string data is lost.
 */
CNDEF Cn_String cn__scope_save_string(Cn_String str) {
    Cn_Scope *scope = cn_scope_stack_peek();

    void *data = cn_chained_arena_alloc(&cn__tu_data->scoped_strings_arena, str.length);
    cn_str_copy_to(str, data);
    str.data = data;

    scope->saved_strings_length += str.length;

    return str;
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Ast
 * ============================================
 */

CNDEF Cn_Type_Flags cn_ast_qualifier_flags_to_type(Cn_Ast_Qualifier_Flags flags) {
    Cn_Type_Flags f = 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_CONST) ? CN_TYPE_QUALIFIED_CONSTANT : 0;
    // f |= (flags & CN_AST_TYPE_QUALIFIER_ATOMIC) ? CN_TYPE_QUALIFIED_ATOMIC : 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_RESTRICT) ? CN_TYPE_QUALIFIED_RESTRICT : 0;
    f |= (flags & CN_AST_TYPE_QUALIFIER_VOLATILE) ? CN_TYPE_QUALIFIED_VOLATILE : 0;
    return f;
}

const Cn_Ast_Binary_Operator CN_AST_BINARY_OPERATORS[] = {
    { CN_AST_BINARY_OP_ARRAY_SUB,       CN_TOKEN_SQR_BRACES_OPEN,   14  },
    { CN_AST_BINARY_OP_FUNCTION,        CN_TOKEN_PARAN_OPEN,        14  },
    { CN_AST_BINARY_OP_DOT,             CN_TOKEN_DOT,               14  },
    { CN_AST_BINARY_OP_ARROW,           CN_TOKEN_ARROW,             14  },
    { CN_AST_BINARY_OP_MULTIPLICATION,  CN_TOKEN_ASTERISK,          12  },
    { CN_AST_BINARY_OP_DIVISION,        CN_TOKEN_SLASH,             12  },
    { CN_AST_BINARY_OP_MODULO,          CN_TOKEN_PERCENT,           12  },
    { CN_AST_BINARY_OP_ADDITION,        CN_TOKEN_PLUS,              11  },
    { CN_AST_BINARY_OP_SUBTRACTION,     CN_TOKEN_MINUS,             11  },
    { CN_AST_BINARY_OP_LSHIFT,          CN_TOKEN_LSHIFT,            10  },
    { CN_AST_BINARY_OP_RSHIFT,          CN_TOKEN_RSHIFT,            10  },
    { CN_AST_BINARY_OP_LESS,            CN_TOKEN_LESS,              9   },
    { CN_AST_BINARY_OP_LESS_EQ,         CN_TOKEN_LESS_EQ,           9   },
    { CN_AST_BINARY_OP_GREATER,         CN_TOKEN_GREATER,           9   },
    { CN_AST_BINARY_OP_GREATER_EQ,      CN_TOKEN_GREATER_EQ,        9   },
    { CN_AST_BINARY_OP_EQ,              CN_TOKEN_EQ,                8   },
    { CN_AST_BINARY_OP_NOT_EQ,          CN_TOKEN_NOT_EQ,            8   },
    { CN_AST_BINARY_OP_BIT_AND,         CN_TOKEN_AMPERSAND,         7   },
    { CN_AST_BINARY_OP_BIT_XOR,         CN_TOKEN_HAT,               6   },
    { CN_AST_BINARY_OP_BIT_OR,          CN_TOKEN_BAR,               5   },
    { CN_AST_BINARY_OP_AND,             CN_TOKEN_AND,               4   },
    { CN_AST_BINARY_OP_OR,              CN_TOKEN_OR,                3   },
    { CN_AST_BINARY_OP_COMMA,           CN_TOKEN_COMMA,             0   },
};

CNDEF Cn_Ast_Binary_Operator_Kind cn_ast_is_binary_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_AST_BINARY_OPERATORS); i++) {
        if (cn_lexer_token(lexer).kind == CN_AST_BINARY_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_AST_BINARY_OP_NONE;
}

const Cn_Ast_Unary_Operator CN_AST_UNARY_OPERATORS[] = {
    { CN_AST_UNARY_OP_INCREMENT,    CN_TOKEN_INCREMENT   },
    { CN_AST_UNARY_OP_DECREMENT,    CN_TOKEN_DECREMENT   },
    { CN_AST_UNARY_OP_POSITIVE,     CN_TOKEN_PLUS        },
    { CN_AST_UNARY_OP_NEGATIVE,     CN_TOKEN_MINUS       },
    { CN_AST_UNARY_OP_NOT,          CN_TOKEN_EXCLAMATION },
    { CN_AST_UNARY_OP_BIT_NOT,      CN_TOKEN_TILDE       },
    { CN_AST_UNARY_OP_DEREF,        CN_TOKEN_ASTERISK    },
    { CN_AST_UNARY_OP_ADDROF,       CN_TOKEN_AMPERSAND   },
};

CNDEF Cn_Ast_Unary_Operator_Kind cn_ast_is_unary_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_AST_UNARY_OPERATORS); i++) {
        if (cn_lexer_token(lexer).kind == CN_AST_UNARY_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_AST_UNARY_OP_NONE;
}

const Cn_Ast_Assignment_Operator CN_AST_ASSIGNMENT_OPERATORS[] = {
    { CN_AST_ASSIGNMENT_OP_ASSIGN,      CN_TOKEN_ASSIGN          },
    { CN_AST_ASSIGNMENT_OP_MULTIPLY,    CN_TOKEN_MULTIPLY_ASSIGN },
    { CN_AST_ASSIGNMENT_OP_DIVIDE,      CN_TOKEN_DIVIDE_ASSIGN   },
    { CN_AST_ASSIGNMENT_OP_MODULO,      CN_TOKEN_MODULO_ASSIGN   },
    { CN_AST_ASSIGNMENT_OP_PLUS,        CN_TOKEN_PLUS_ASSIGN     },
    { CN_AST_ASSIGNMENT_OP_MINUS,       CN_TOKEN_MINUS_ASSIGN    },
    { CN_AST_ASSIGNMENT_OP_LSHIFT,      CN_TOKEN_LSHIFT_ASSIGN   },
    { CN_AST_ASSIGNMENT_OP_RSHIFT,      CN_TOKEN_RSHIFT_ASSIGN   },
    { CN_AST_ASSIGNMENT_OP_BIT_AND,     CN_TOKEN_BIT_AND_ASSIGN  },
    { CN_AST_ASSIGNMENT_OP_BIT_XOR,     CN_TOKEN_BIT_XOR_ASSIGN  },
    { CN_AST_ASSIGNMENT_OP_BIT_OR,      CN_TOKEN_BIT_OR_ASSIGN   },
};

CNDEF Cn_Ast_Assignment_Operator_Kind cn_ast_is_assignment_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_AST_ASSIGNMENT_OPERATORS); i++) {
        if (cn_lexer_token(lexer).kind == CN_AST_ASSIGNMENT_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_AST_ASSIGNMENT_OP_NONE;
}

const Cn_Ast_Postfix_Operator CN_AST_POSTFIX_OPERATORS[] = {
    { CN_AST_POSTFIX_OP_INCREMENT,    CN_TOKEN_INCREMENT },
    { CN_AST_POSTFIX_OP_DECREMENT,    CN_TOKEN_DECREMENT },
};

CNDEF Cn_Ast_Postfix_Operator_Kind cn_ast_is_postfix_operator(Cn_Lexer *lexer) {
    for (int i = 0; i < (int)CN_ARRAY_LENGTH(CN_AST_POSTFIX_OPERATORS); i++) {
        if (cn_lexer_token(lexer).kind == CN_AST_POSTFIX_OPERATORS[i].token_identifier) {
            return i;
        }
    }

    return CN_AST_POSTFIX_OP_NONE;
}

const Cn_Token_Kind CN_AST_TYPE_KINDS[] = {
    CN_TOKEN_UNKNOWN,
    CN_TOKEN_INT,
    CN_TOKEN_CHAR,
    CN_TOKEN_FLOAT,
    CN_TOKEN_DOUBLE,
    CN_TOKEN_BOOL,
    CN_TOKEN_VOID,
};

CNDEF bool cn_ast_type_ptr_equals(const Cn_Type **type1_ptr, const Cn_Type **type2_ptr) {
    return cn_type_equals(*type1_ptr, *type2_ptr);
}

CNDEF uint64_t cn_ast_type_ptr_hash(const Cn_Type **type_ptr) {
    return cn_type_hash(*type_ptr);
}

// Ast meta info, defined at the very end of the implementation.
static Cn_Ast_Info cn__ast_infos[CN_AST__END];

CNDEF Cn_Ast_Info *cn_ast_info(Cn_Ast_Kind kind) {
    CN_ASSERT(kind < CN_AST__END);
    CN_ASSERT(cn__ast_infos[kind].name != NULL);
    return &cn__ast_infos[kind];
}


CNDEF void *cn__ast_new(Cn_Ast_Node *node, size_t size) {
    void *data = cn_chained_arena_alloc(&cn__tu_data->ast_arena, size);
    memcpy(data, node, size);
    return data;
}

const char *cn__ast_print_prefixes[64] = {0};
const char *CN__AST_PRINT_EMPTY_TAB = "    ";
const char *CN__AST_PRINT_FLAT_TAB  = "│   ";
const char *CN__AST_PRINT_SPLIT_TAB = "├── ";
const char *CN__AST_PRINT_LAST_TAB  = "└── ";
#define CN__AST_PRINT_TABS(depth, prefix) for (int i = 0; i < depth; i++) { fprintf(stderr, "%s", cn__ast_print_prefixes[i]); } fprintf(stderr, "%s", prefix)
#define CN__AST_PRINT_LAST()  do { cn__ast_print_prefixes[depth] = CN__AST_PRINT_EMPTY_TAB; CN__AST_PRINT_TABS(depth, CN__AST_PRINT_LAST_TAB); } while(0)
#define CN__AST_PRINT_SPLIT() do { CN__AST_PRINT_TABS(depth, CN__AST_PRINT_SPLIT_TAB); } while(0)

CNDEF void cn__ast_print_list(Cn_Ast_Nodes list, const char *list_name, int depth) {
    CN_ASSERT(depth < (int)CN_ARRAY_LENGTH(cn__ast_print_prefixes));

    cn__ast_print_prefixes[depth] = CN__AST_PRINT_FLAT_TAB;

    if (list.length > 0) {
        fprintf(stderr, "LIST %s:"CN_LINE_END, list_name);
        for (int64_t i = 0; i < list.length - 1; i++) {
            CN__AST_PRINT_SPLIT();
            cn_ast_print(list.ptrs[i], depth + 1);
        }

        CN__AST_PRINT_LAST();
        cn_ast_print(list.ptrs[list.length - 1], depth + 1);
    } else {
        fprintf(stderr, "LIST %s: <empty>"CN_LINE_END, list_name);
    }

    cn__ast_print_prefixes[depth] = NULL;
}

CNDEF void cn_ast_print(void * idx, int depth) {
    CN_ASSERT(depth < (int)CN_ARRAY_LENGTH(cn__ast_print_prefixes));

    CN_ASSERT(idx != NULL);
    Cn_Ast_Node *node = idx;
    Cn_Ast_Info *info = cn_ast_info(node->kind);

    fprintf(stderr, "%s", info->name);
    cn__ast_print_prefixes[depth] = CN__AST_PRINT_FLAT_TAB;

    if (node->flags != 0) fprintf(stderr, " flags=0x%x", (unsigned)node->flags);

    // Leaf members are printed inline, child nodes are counted to know which one is last.
    int child_count = 0;
    for (int64_t i = 0; i < info->members_count; i++) {
        Cn_Ast_Member_Info *member = &info->members[i];
        void *field = (char *)node + member->offset;

        switch (member->kind) {
            case CN_AST_MEMBER_INFO_NODE:
                if (*(Cn_Ast_Node **)field != NULL) child_count++;
                break;
            case CN_AST_MEMBER_INFO_NODES:
                child_count++;
                break;
            case CN_AST_MEMBER_INFO_STRING:
                fprintf(stderr, " %s=\"%.*s\"", member->name, CN_STR_UNPACK(*(Cn_String *)field));
                break;
            case CN_AST_MEMBER_INFO_UINT64:
                fprintf(stderr, " %s=%llu", member->name, (unsigned long long)*(uint64_t *)field);
                break;
            case CN_AST_MEMBER_INFO_INT32:
                fprintf(stderr, " %s=%d", member->name, (int)*(int32_t *)field);
                break;
            case CN_AST_MEMBER_INFO_DOUBLE:
                fprintf(stderr, " %s=%lf", member->name, *(double *)field);
                break;
            case CN_AST_MEMBER_INFO_TYPE:
                if (*(Cn_Type **)field != NULL) {
                    fprintf(stderr, " %s=", member->name);
                    cn_type_print(*(Cn_Type **)field);
                }
                break;
        }
    }

    fputs(CN_LINE_END, stderr);

    int printed = 0;
    for (int64_t i = 0; i < info->members_count; i++) {
        Cn_Ast_Member_Info *member = &info->members[i];
        void *field = (char *)node + member->offset;

        if (member->kind == CN_AST_MEMBER_INFO_NODE) {
            Cn_Ast_Node *child = *(Cn_Ast_Node **)field;
            if (child == NULL) continue;

            if (++printed == child_count) CN__AST_PRINT_LAST();
            else                          CN__AST_PRINT_SPLIT();

            fprintf(stderr, "%s: ", member->name);
            cn_ast_print(child, depth + 1);
        } else if (member->kind == CN_AST_MEMBER_INFO_NODES) {
            if (++printed == child_count) CN__AST_PRINT_LAST();
            else                          CN__AST_PRINT_SPLIT();

            cn__ast_print_list(*(Cn_Ast_Nodes *)field, member->name, depth + 1);
        }
    }

    cn__ast_print_prefixes[depth] = NULL;
}

CNDEF const char *cn_ast_node_kind_name(Cn_Ast_Kind kind) {
    return cn_ast_info(kind)->name;
}

CNDEF size_t cn_ast_node_size(Cn_Ast_Kind kind) {
    return cn_ast_info(kind)->size;
}

CNDEF bool cn_ast_is_primary(void * node_ptr) {
    Cn_Ast_Node *node = node_ptr;
    return node->kind == CN_AST_IDENTIFIER || 
        node->kind == CN_AST_INTEGER ||
        node->kind == CN_AST_FLOAT || 
        node->kind == CN_AST_STRING;
}

CNDEF Cn_Ast_Nodes cn_ast_stack_finalize(int64_t mark) {
    int64_t stack_len = cn_array_list_length(&cn__tu_data->ptr_stack);
    int64_t count = stack_len - mark;

    if (count <= 0) return (Cn_Ast_Nodes) { .ptrs = NULL, .length = 0 };
    

    // Allocated from the ast arena, so the list is rolled back by a checkpoint
    // together with the nodes it points at.
    Cn_Ast_Node **ptrs = cn_chained_arena_alloc(&cn__tu_data->ast_arena, count * sizeof(Cn_Ast_Node *));

    // Copy pointers from stack.
    memcpy(ptrs, cn__tu_data->ptr_stack + mark, count * sizeof(Cn_Ast_Node *));

    // Pop the finalized indices from stack.
    cn_array_list_pop_multiple(&cn__tu_data->ptr_stack, count);

    return (Cn_Ast_Nodes) { .ptrs = ptrs, .length = count };
}

CNDEF Cn_Ast_Node *cn_ast_remove_from_list(Cn_Ast_Nodes *list, int64_t index) {
    CN_ASSERT(index >= 0 && index < list->length);

    Cn_Ast_Node *removed = list->ptrs[index];
    memmove(list->ptrs + index, list->ptrs + index + 1, (size_t)(list->length - index - 1) * sizeof(Cn_Ast_Node *));
    list->length--;
    return removed;
}

CNDEF Cn_Ast_Nodes cn_ast_copy_list(Cn_Ast_Nodes list) {
    int64_t mark = cn_ast_stack_mark();

    for (int64_t i = 0; i < list.length; i++) {
        cn_ast_stack_push(cn_ast_copy(list.ptrs[i]));
    }

    return cn_ast_stack_finalize(mark);
}

CNDEF void *cn_ast_copy(void * idx) {
    if (idx == NULL) return NULL;

    Cn_Ast_Node *node = idx;
    Cn_Ast_Info *info = cn_ast_info(node->kind);

    // Nodes are allocated at their own size, so a copy has to know the size of
    // the concrete kind it is copying.
    Cn_Ast_Node *copy = cn__ast_new(node, info->size);

    copy->flags |= CN_AST_SYNTHETIC;

    // Leaf members are already copied by value, only node branches need a deep copy.
    for (int64_t i = 0; i < info->members_count; i++) {
        Cn_Ast_Member_Info *member = &info->members[i];
        void *field = (char *)copy + member->offset;

        switch (member->kind) {
            case CN_AST_MEMBER_INFO_NODE:
                *(Cn_Ast_Node **)field = cn_ast_copy(*(Cn_Ast_Node **)field);
                break;
            case CN_AST_MEMBER_INFO_NODES:
                *(Cn_Ast_Nodes *)field = cn_ast_copy_list(*(Cn_Ast_Nodes *)field);
                break;
            case CN_AST_MEMBER_INFO_STRING:
            case CN_AST_MEMBER_INFO_UINT64:
            case CN_AST_MEMBER_INFO_INT32:
            case CN_AST_MEMBER_INFO_DOUBLE:
            case CN_AST_MEMBER_INFO_TYPE:
                break;
        }
    }

    return copy;
}


CNDEF bool cn_ast_remove_attribute(void *target, Cn_String attribute_name) {
    Cn_Ast_Attribute_Specifier *owner = NULL;
    int64_t index = -1;

    if (cn_get_attribute_info(target, attribute_name, &owner, &index) == NULL) return false;

    cn_ast_remove_from_list(&owner->attributes, index);
    return true;
}


/**
 * ============================================
 * IMPLEMENTATION SECTION: Get
 * ============================================
 */

CNDEF void cn_get_declarator_info(Cn_Ast_Declarator *declarator, Cn_Ast_Node **last_declarator) {
    if (declarator == NULL) {
        *last_declarator = NULL;
        return;
    }

    // 'next' walks down the direct declarator chain, 'dd' keeps the last
    // non-NULL link so an abstract declarator still reports something.
    Cn_Ast_Node *next = declarator->direct_declarator;
    Cn_Ast_Node *dd = cn_ast_upcast(declarator);

    while (next != NULL) {
        dd = next;

        switch (dd->kind) {
            case CN_AST_IDENTIFIER:
                *last_declarator = dd;
                return;
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                next = cn_ast_as(Direct_Declarator_Grouped, dd)->declarator->direct_declarator;
                continue;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                next = cn_ast_as(Direct_Declarator_Array, dd)->direct_declarator;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                next = cn_ast_as(Direct_Declarator_Function, dd)->direct_declarator;
                continue;
            default:
                break;
        }
    }

    // NULL Direct declarator, case.
    *last_declarator = dd;
}

CNDEF bool cn_get_function_declarator_info(Cn_Ast_Declarator *declarator, Cn_Ast_Node **function_direct_declarator, Cn_Ast_Node **last_declarator) {
    if (declarator == NULL) {
        *last_declarator = NULL;
        return false;
    }

    // 'next' walks down the direct declarator chain, 'dd' keeps the last
    // non-NULL link so an abstract declarator still reports something.
    Cn_Ast_Node *next = declarator->direct_declarator;
    Cn_Ast_Node *dd = cn_ast_upcast(declarator);

    while (next != NULL) {
        dd = next;

        switch (dd->kind) {
            // If identifier case, then declarator is not function declarator.
            case CN_AST_IDENTIFIER:
                *last_declarator = dd;
                return false;
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                next = cn_ast_as(Direct_Declarator_Grouped, dd)->declarator->direct_declarator;
                continue;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                next = cn_ast_as(Direct_Declarator_Array, dd)->direct_declarator;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION: {
                next = cn_ast_as(Direct_Declarator_Function, dd)->direct_declarator;

                // If followed by identifier, then return true, and get all outputs.
                if (next != NULL && next->kind == CN_AST_IDENTIFIER) {
                    if (function_direct_declarator != NULL) *function_direct_declarator = dd;
                    *last_declarator = next;
                    return true;
                }

                continue;
            }
            default:
                break;
        }
    }

    // NULL Direct declarator, case.
    *last_declarator = dd;
    return false;
}

CNDEF Cn_Ast_Attribute *cn_get_attribute_info(void *target, Cn_String attribute_name, Cn_Ast_Attribute_Specifier **owner, int64_t *index) {
    if (target == NULL) return NULL;

    Cn_Ast_Node *node = target;
    Cn_Ast_Nodes attribute_specifiers;

    switch (node->kind) {
        case CN_AST_DECLARATION:
            attribute_specifiers = cn_ast_as(Declaration, node)->attribute_specifiers;
            break;
        case CN_AST_FUNCTION:
            attribute_specifiers = cn_ast_as(Function, node)->attribute_specifiers;
            break;
        case CN_AST_BLOCK:
            attribute_specifiers = cn_ast_as(Block, node)->attribute_specifiers;
            break;
        case CN_AST_IF:
            attribute_specifiers = cn_ast_as(If, node)->attribute_specifiers;
            break;
        case CN_AST_SWITCH:
            attribute_specifiers = cn_ast_as(Switch, node)->attribute_specifiers;
            break;
        case CN_AST_WHILE:
            attribute_specifiers = cn_ast_as(While, node)->attribute_specifiers;
            break;
        case CN_AST_DO_WHILE:
            attribute_specifiers = cn_ast_as(Do_While, node)->attribute_specifiers;
            break;
        case CN_AST_FOR:
            attribute_specifiers = cn_ast_as(For, node)->attribute_specifiers;
            break;
        case CN_AST_LABEL:
            attribute_specifiers = cn_ast_as(Label, node)->attribute_specifiers;
            break;
        case CN_AST_GOTO:
            attribute_specifiers = cn_ast_as(Goto, node)->attribute_specifiers;
            break;
        case CN_AST_RETURN:
            attribute_specifiers = cn_ast_as(Return, node)->attribute_specifiers;
            break;
        case CN_AST_BREAK:
            attribute_specifiers = cn_ast_as(Break, node)->attribute_specifiers;
            break;
        case CN_AST_CONTINUE:
            attribute_specifiers = cn_ast_as(Continue, node)->attribute_specifiers;
            break;
        case CN_AST_EXPRESSION_STATEMENT:
            attribute_specifiers = cn_ast_as(Expression_Statement, node)->attribute_specifiers;
            break;
        // case CN_AST_PARAMETER_DECLARATION:
        case CN_AST_STRUCT_SPECIFIER:
            attribute_specifiers = cn_ast_as(Struct_Specifier, node)->attribute_specifiers;
            break;
        case CN_AST_UNION_SPECIFIER:
            attribute_specifiers = cn_ast_as(Union_Specifier, node)->attribute_specifiers;
            break;
        case CN_AST_MEMBER_DECLARATION:
            attribute_specifiers = cn_ast_as(Member_Declaration, node)->attribute_specifiers;
            break;
        case CN_AST_ENUM_SPECIFIER:
            attribute_specifiers = cn_ast_as(Enum_Specifier, node)->attribute_specifiers;
            break;
        case CN_AST_ENUMERATOR:
            attribute_specifiers = cn_ast_as(Enumerator, node)->attribute_specifiers;
            break;
#ifdef CN_GNU
        case CN_AST_INIT_DECLARATOR:
            attribute_specifiers = cn_ast_as(Init_Declarator, node)->attribute_specifiers;
            break;
        case CN_AST_POINTER:
            attribute_specifiers = cn_ast_as(Pointer, node)->attribute_specifiers;
            break;
        case CN_AST_DECLARATION_SPECIFIERS:
            attribute_specifiers = cn_ast_as(Declaration_Specifiers, node)->attribute_specifiers;
            break;
        case CN_AST_SPECIFIER_QUALIFIER:
            attribute_specifiers = cn_ast_as(Specifier_Qualifier, node)->attribute_specifiers;
            break;
        case CN_AST_MEMBER_DECLARATOR:
            attribute_specifiers = cn_ast_as(Member_Declarator, node)->attribute_specifiers;
            break;
#endif

        case CN_AST_ATTRIBUTE_SPECIFIER:
            // A specifier is searched directly, it owns its attributes.
            attribute_specifiers = (Cn_Ast_Nodes) { .ptrs = &node, .length = 1 };
            break;
        default:
            return NULL;
    }

    for (int64_t i = 0; i < attribute_specifiers.length; i++) {
        Cn_Ast_Attribute_Specifier *attribute_specifier = cn_ast_as(Attribute_Specifier, attribute_specifiers.ptrs[i]);
        for (int64_t j = 0; j < attribute_specifier->attributes.length; j++) {
            Cn_Ast_Attribute *attribute = cn_ast_as(Attribute, attribute_specifier->attributes.ptrs[j]);
            if (cn_str_equals(&attribute->identifier->name, &attribute_name)) {
                if (owner != NULL) *owner = attribute_specifier;
                if (index != NULL) *index = j;
                return attribute;
            }
        }
    }

    return NULL;
}

CNDEF Cn_Ast_Attribute *cn_get_attribute(void *target, Cn_String attribute_name) {
    return cn_get_attribute_info(target, attribute_name, NULL, NULL);
}

/**
 * Helper function, finds the member that stores resulting type of `expression` node,
 * by its ast meta info, it is the first member of `CN_AST_MEMBER_INFO_TYPE` kind.
 *
 * RETURNS: Pointer to the member, `NULL` if node kind has no such member.
 */
CNDEF Cn_Type **cn__get_expression_type_member(Cn_Ast_Node *expression) {
    Cn_Ast_Info *info = cn_ast_info(expression->kind);

    for (int64_t i = 0; i < info->members_count; i++) {
        if (info->members[i].kind == CN_AST_MEMBER_INFO_TYPE) {
            return (Cn_Type **)((uint8_t *)expression + info->members[i].offset);
        }
    }

    return NULL;
}

CNDEF Cn_Type *cn_get_expression_type(void * expression) {
    CN_ASSERT(expression != NULL);
    Cn_Ast_Node *node = expression;

    Cn_Type **type = cn__get_expression_type_member(node);
    if (type == NULL) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_EXPRESSION, "Expected expression to get it's resulting type.");
        return NULL;
    }

    return *type;
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
        fprintf(stderr, CN_ANSI_BOLD"%.*s"CN_ANSI_RESET" ", CN_STR_UNPACK(loc->file));
    } else {
        fprintf(stderr, CN_ANSI_BOLD"%.*s:%ld:%ld:"CN_ANSI_RESET" ", CN_STR_UNPACK(loc->file), loc->line, loc->column);
    }

    if (!cn_str_is_empty(CN_DIAGNOSTIC_CODES[code])) {
        fprintf(stderr, "%.*s: ", CN_STR_UNPACK(CN_DIAGNOSTIC_CODES[code]));
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
                fprintf(stderr, "%.*s", CN_STR_UNPACK(snippet));
                printed += inline_offset;
            }

            // Printing annotated snippet.
            int64_t length = annotations->length < line.length - printed ? annotations->length : line.length - printed;
            snippet = cn_str_substring(line, printed, printed + length);
            fprintf(stderr, "%s%.*s"CN_ANSI_RESET, ansi_color, CN_STR_UNPACK(snippet));
            printed += length;


annotations_next:
            annotations++;
            annotations_length--;
        }

        if (printed < line.length) {
            snippet = cn_str_substring(line, printed, line.length);
            fprintf(stderr, "%.*s", CN_STR_UNPACK(snippet));
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
            cn__tu_data->warning_count++;
            break;
        case CN_DIAGNOSTIC_ERROR:
            cn__tu_data->error_count++;
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

CNDEF void cn_diagnostic_node(Cn_Diagnostic_Level level, void * idx, Cn_Diagnostic_Code code, const char *format, ...) {
    CN_ASSERT(idx != NULL);

    Cn_Ast_Node *node = idx;

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
 * IMPLEMENTATION SECTION: Binding
 * ============================================
 */

CNDEF Cn_Binding_Idx cn_binding_table_put(Cn_Binding binding, Cn_Binding_Idx **binding_table) {
    cn_array_list_append(&cn__tu_data->binding_list, binding);
    cn_hash_table_put(binding_table, cn_array_list_length(&cn__tu_data->binding_list) - 1, &binding.name);
    return cn_array_list_length(&cn__tu_data->binding_list) - 1;
}

CNDEF Cn_Binding_Idx cn_binding_table_get(Cn_String name, Cn_Binding_Idx **binding_table) {
    Cn_Binding_Idx *reference = cn_hash_table_get(binding_table, &name);
    return reference == NULL ? CN_BINDING_NIL_IDX : *reference;
}

/**
 * Helper function, diagnoses error if binding a conflicts with binding b based 
 * on kind or type.
 * NOTE: Expects a and b to be in the same scope and contain the same name.
 *
 * RETURNS: `true` if there is conflict, `false` otherwise.
 */
CNDEF bool cn__binding_conflict(Cn_Binding *a, Cn_Binding *b) {
    CN_ASSERT(cn_str_equals(&a->name, &b->name));
    CN_ASSERT(a->scope_idx == b->scope_idx);

    // Different binding kind.
    if (a->kind != b->kind) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, b->src, CN_DC_ILLEGAL_BINDING, "'%.*s' redeclared as different kind of symbol.", CN_STR_UNPACK(a->name));
        return true;
    }
    // Confliciting types.
    if (!cn_type_is_compatible(a->type, b->type)) {
        Cn_String current_t = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), a->type, (Cn_String) {0});
        Cn_String binding_t = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), b->type, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, b->src, CN_DC_ILLEGAL_BINDING, "'%.*s' redeclared with a conflicting type, from %.*s to %.*s.", CN_STR_UNPACK(b->name), CN_STR_UNPACK(current_t), CN_STR_UNPACK(binding_t));
        return true;
    }

    return false;
}

CNDEF Cn_Binding_Idx cn_binding_declare_function(Cn_String name, void * source, Cn_Ast_Storage_Specifier_Flags storage_flags, Cn_Ast_Function_Specifier_Flags function_flags, Cn_Type *type, Cn_Ast_Nodes *parameter_declarations, bool is_definition) {
    CN_ASSERT(cn_array_list_length(&cn__tu_data->scope_stack) > 0);

    Cn_Binding binding = {
        .kind = CN_BINDING_FUCNTION,
        .src = source,
        .type = type,
        .scope_idx = CN_SCOPE_STACK_CURRENT_IDX,
    };
    binding.b_function.storage_flags    = storage_flags;
    binding.b_function.function_flags   = function_flags;
    binding.b_function.is_definition    = is_definition;

    // Getting currently visible binding with the same name if such exists.
    Cn_Binding_Idx current_idx = cn_binding_table_get(name, &cn__tu_data->symbol_binding_table);
    if (current_idx != CN_BINDING_NIL_IDX) {
        Cn_Binding *current = cn_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__binding_conflict(current, &binding)) return CN_BINDING_NIL_IDX;

            // Checking if it is redefinition.
            if (current->b_function.is_definition && binding.b_function.is_definition) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' function redifinition.", CN_STR_UNPACK(name));
                return CN_BINDING_NIL_IDX;
            }

            // If newer binding is redeclaration, merge binding to definition, if not leave as it is.
            if (current->b_function.is_definition) {
                return current_idx;
            }
        }
    } else {
        binding.name = cn__scope_save_string(name);
    }

    // Storing param names on the function binding.
    if (parameter_declarations != NULL && parameter_declarations->length > 0) {
        binding.b_function.parameter_names = cn_chained_arena_alloc(&cn__tu_data->scoped_strings_arena, sizeof(Cn_String) * parameter_declarations->length);
        cn_scope_stack_peek()->saved_strings_length += sizeof(Cn_String) * parameter_declarations->length;

        for (int64_t i = 0; i < parameter_declarations->length; i++) {
            Cn_Ast_Parameter_Declaration *p = cn_ast_as(Parameter_Declaration, parameter_declarations->ptrs[i]);
            Cn_Ast_Node *param_identifier;
            cn_get_declarator_info(p->declarator, &param_identifier);

            if (param_identifier != NULL && param_identifier->kind == CN_AST_IDENTIFIER) {
                binding.b_function.parameter_names[i] = cn__scope_save_string(((Cn_Ast_Identifier *)param_identifier)->name);
            }
        }
    }
    

    binding.next_idx = current_idx;
    return cn_binding_table_put(binding, &cn__tu_data->symbol_binding_table);
}

CNDEF Cn_Binding_Idx cn_binding_declare_variable(Cn_String name, void * source, Cn_Ast_Storage_Specifier_Flags storage_flags, Cn_Type *type, bool is_definition) {
    CN_ASSERT(cn_array_list_length(&cn__tu_data->scope_stack) > 0);

    // Checking if binding has valid type.
    if (is_definition && !(type->flags & CN_TYPE_COMPLETE)) {
        Cn_String type_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' variable defined with incomplete type %.*s.", CN_STR_UNPACK(name), CN_STR_UNPACK(type_str));
        return CN_BINDING_NIL_IDX;
    }

    Cn_Binding binding = { 
        .kind = CN_BINDING_VARIABLE,
        .src = source,
        .type = type,
        .scope_idx = CN_SCOPE_STACK_CURRENT_IDX,
    };
    binding.b_variable.storage_flags = storage_flags;
    binding.b_variable.is_definition = is_definition;
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Binding_Idx current_idx = cn_binding_table_get(name, &cn__tu_data->symbol_binding_table);
    if (current_idx != CN_BINDING_NIL_IDX) {
        Cn_Binding *current = cn_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__binding_conflict(current, &binding)) return CN_BINDING_NIL_IDX;

            if (CN_SCOPE_STACK_CURRENT_IDX != CN_SCOPE_FILE_IDX) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' variable redeclared in non-file scope.", CN_STR_UNPACK(name));
                return CN_BINDING_NIL_IDX;
            }

            if (current->b_variable.is_definition && binding.b_variable.is_definition) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' variable redifinition.", CN_STR_UNPACK(name));
                return CN_BINDING_NIL_IDX;
            }

            // If newer binding is redeclaration, merge binding to definition, if not leave as it is.
            if (current->b_variable.is_definition) {
                return current_idx;
            }
        }
    } else {
        binding.name = cn__scope_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_binding_table_put(binding, &cn__tu_data->symbol_binding_table);
}

CNDEF Cn_Binding_Idx cn_binding_declare_typedef(Cn_String name, void * source, Cn_Type *type) {
    CN_ASSERT(cn_array_list_length(&cn__tu_data->scope_stack) > 0);

    Cn_Binding binding = { 
        .kind = CN_BINDING_TYPEDEF,
        .src = source,
        .type = type,
        .scope_idx = CN_SCOPE_STACK_CURRENT_IDX,
    };
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Binding_Idx current_idx = cn_binding_table_get(name, &cn__tu_data->symbol_binding_table);
    if (current_idx != CN_BINDING_NIL_IDX) {
        Cn_Binding *current = cn_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__binding_conflict(current, &binding)) return CN_BINDING_NIL_IDX;

            // Typedef is not allowed to be redefined.
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' typedef redifinition.", CN_STR_UNPACK(name));
            return CN_BINDING_NIL_IDX;
        }
    } else {
        binding.name = cn__scope_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_binding_table_put(binding, &cn__tu_data->symbol_binding_table);
}

CNDEF Cn_Binding_Idx cn_binding_declare_enum_constant(Cn_String name, void * source, Cn_Type *type, int64_t value) {
    CN_ASSERT(cn_array_list_length(&cn__tu_data->scope_stack) > 0);

    Cn_Binding binding = { 
        .kind = CN_BINDING_ENUM_CONSTANT,
        .src = source,
        .type = type,
        .scope_idx = CN_SCOPE_STACK_CURRENT_IDX,
    };

    binding.b_enum_constant.value = value;
    
    // Getting currently visible binding with the same name if such exists.
    Cn_Binding_Idx current_idx = cn_binding_table_get(name, &cn__tu_data->symbol_binding_table);
    if (current_idx != CN_BINDING_NIL_IDX) {
        Cn_Binding *current = cn_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            // If bindings conflict in general, based on type or kind.
            if (cn__binding_conflict(current, &binding)) return CN_BINDING_NIL_IDX;

            // Enum constant is not allowed to be redefined.
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' enum constant redifinition.", CN_STR_UNPACK(name));
            return CN_BINDING_NIL_IDX;
        }
    } else {
        binding.name = cn__scope_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_binding_table_put(binding, &cn__tu_data->symbol_binding_table);
}

CNDEF Cn_Binding_Idx cn_binding_declare_tag(Cn_String tag, Cn_Type_Kind kind) {
    CN_ASSERT(cn_array_list_length(&cn__tu_data->scope_stack) > 0);
    CN_ASSERT(kind == CN_STRUCT || kind == CN_UNION || kind == CN_ENUM);

    Cn_Binding_Idx *ref = cn_hash_table_get(&cn__tu_data->tag_binding_table, &tag);

    // No previous declaration case.
    if (ref == NULL) {
        // Saving string.
        tag = cn__ast_permanent_save_string(tag);

        // Making type.
        Cn_Type *type = cn_chained_arena_alloc(&cn__tu_data->type_arena, sizeof(Cn_Type));
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
        cn_array_list_append(&cn__tu_data->binding_list, ((Cn_Binding) { 
                    .name = tag, 
                    .kind = CN_BINDING_TAG, 
                    .type = type, 
                    .next_idx = CN_BINDING_NIL_IDX, 
                    .scope_idx = cn_array_list_length(&cn__tu_data->scope_stack) - 1 
                    }));

        // Finally, putting it into the table, and returning.
        cn_hash_table_put(&cn__tu_data->tag_binding_table, cn_array_list_length(&cn__tu_data->binding_list) - 1, &tag);
        
        return cn_array_list_length(&cn__tu_data->binding_list) - 1;
    }


    Cn_Binding *binding = cn__tu_data->binding_list + *ref;
    if (binding->type->kind != kind) {
        cn_log(CN_ERROR, "Tag '%.*s' is already declared under different type. This tool deosn't support that in any form.", CN_STR_UNPACK(tag));
        return CN_BINDING_NIL_IDX;
    }

    if ((binding->type->flags & CN_TYPE_COMPLETE) || binding->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) return *ref;
    
    // Making type.
    Cn_Type *type = cn_chained_arena_alloc(&cn__tu_data->type_arena, sizeof(Cn_Type));
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

    cn_array_list_append(&cn__tu_data->binding_list, ((Cn_Binding) {
                .name = binding->name, 
                .kind = CN_BINDING_TAG, 
                .type = type, 
                .next_idx = *ref, 
                .scope_idx = cn_array_list_length(&cn__tu_data->scope_stack) - 1 
                }));

    cn_hash_table_put(&cn__tu_data->tag_binding_table, cn_array_list_length(&cn__tu_data->binding_list) - 1, &tag);


    return cn_array_list_length(&cn__tu_data->binding_list) - 1;
}

CNDEF Cn_Binding_Idx cn_binding_declare_label(Cn_String name, void * source) {
    CN_ASSERT(cn__tu_data->function_scope_idx > 0);

    Cn_Binding binding = { 
        .kind = CN_BINDING_LABEL,
        .src = source,
        .scope_idx = cn__tu_data->function_scope_idx,
    };

    // Getting currently visible binding with the same name if such exists.
    Cn_Binding_Idx current_idx = cn_binding_table_get(name, &cn__tu_data->label_binding_table);
    if (current_idx != CN_BINDING_NIL_IDX) {
        Cn_Binding *current = cn_binding_get(current_idx);
        binding.name = current->name;

        // Handling redeclaration, redifinition.
        if (current->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            // Labels are not allowed to be redefined.
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' label redifinition.", CN_STR_UNPACK(name));
            return CN_BINDING_NIL_IDX;
        }
    } else {
        binding.name = cn__scope_save_string(name);
    }

    binding.next_idx = current_idx;
    return cn_binding_table_put(binding, &cn__tu_data->label_binding_table);
}

CNDEF Cn_Binding_Idx cn_binding_declare_builtin(Cn_String name, void *source) {
    CN_ASSERT(!cn_str_is_empty(name));

    Cn_Type *opaque = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_OPAQUE);

    Cn_Binding binding = { 
        .kind = CN_BINDING_BUILTIN,
        .scope_idx = CN_SCOPE_FILE_IDX,
        .type = opaque,
    };

    Cn_Binding_Idx current_idx = cn_binding_table_get(name, &cn__tu_data->symbol_binding_table);
    if (current_idx != CN_BINDING_NIL_IDX) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, source, CN_DC_ILLEGAL_BINDING, "'%.*s' builtin redifinition.", CN_STR_UNPACK(name));
        return CN_BINDING_NIL_IDX;
    }

    binding.name = cn__ast_permanent_save_string(name);
    return cn_binding_table_put(binding, &cn__tu_data->symbol_binding_table);
}


CNDEF bool cn_binding_declare_function_arguments(Cn_Binding_Idx function_binding_idx) {
    Cn_Binding *function_binding = cn_binding_get(function_binding_idx);
    Cn_Type_Function *func_type = (Cn_Type_Function *)function_binding->type;

    for (int64_t i = 0; i < func_type->params_length; i++) {
        if (cn_binding_declare_variable(
                    function_binding->b_function.parameter_names[i],
                    function_binding->src,                              // TODO: Later supply actual parmeter declaration or declarator ast nodes.
                    0,
                    func_type->params[i].type,
                    true
                    ) == CN_BINDING_NIL_IDX) return false;
    }

    return true;
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Hook
 * ============================================
 */

Cn_Hooks cn_hooks = {0};

#define CN__EXECUTE_HOOK_AST_PARSED(ast_node, saved_state, saved_lexer, parse_function)\
    for (int64_t i = 0; i < cn_array_list_length(&cn_hooks.ast_parsed_hooks); i++) {\
        bool first = true;\
        do {\
            bool ast_modified = false;\
            Cn_Hook_Response response = cn_hooks.ast_parsed_hooks[i](&ast_node, &ast_modified, first);\
            first = false;\
            if (response & CN_HOOK_RESPONSE_ERROR) goto error;\
            if (!ast_modified) break;\
            \
            Cn_Lexer l = saved_lexer;\
            cn_lexer_load_content(&l, cn__tu_emit_and_rollback(ast_node, &saved_state));\
            \
            ast_node = parse_function(&l);\
            if (ast_node == NULL) goto error;\
        } while (true);\
    }\


CNDEF bool cn__hook_analyze_layout(Cn_Ast_Node *tag_specifier, Cn_Type *type) {
    for (int64_t i = 0; i < cn_array_list_length(&cn_hooks.analyze_layout_hooks); i++) {
        if (cn_hooks.analyze_layout_hooks[i](tag_specifier, type) & CN_HOOK_RESPONSE_ERROR) return false;
        if (type->flags & CN_TYPE_COMPLETE) return true;
    }

    return true;
}

CNDEF bool cn__extension_register(Cn_Extension *extensions[], int64_t length) {
    static bool enabled = false;
    if (!enabled) {
        // Allocating hook lists.
        cn_hooks.ast_parsed_hooks           = cn_array_list_make(Cn_Hook_Ast_Parsed *, 8);
        cn_hooks.tu_start_hooks             = cn_array_list_make(Cn_Hook_Tu_Start *, 8);
        cn_hooks.tu_end_hooks               = cn_array_list_make(Cn_Hook_Tu_End *, 8);
        cn_hooks.analyze_declarator_hooks   = cn_array_list_make(Cn_Hook_Analyze_Declarator *, 8);
        cn_hooks.analyze_layout_hooks       = cn_array_list_make(Cn_Hook_Analyze_Layout *, 8);

        for (int64_t i = 0; i < length; i++) {
            if (extensions[i] != NULL) {
                if (!extensions[i](&cn_hooks)) return false;
            }
        }
        
        enabled = true;
    }
    return true;
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Analyze
 * ============================================
 */


// AI: Generated.
/**
 * Helper function, opaque type is trusted by every typecheck, to be whatever makes the checked
 * construct valid. Typecheck only errors if non-opaque operands make it invalid on their own.
 *
 * RETURNS: `true` if `type` is opaque, ignoring qualifiers.
 */
CNDEF bool cn__analyze_is_opaque(Cn_Type *type) {
    return cn_type_unqualified(type)->kind == CN_OPAQUE;
}

CNDEF bool cn__analyze_declaration_opt(void *declaration_specifiers, Cn_Analyze_Declaration_Opt opt) {
    Cn_Ast_Declaration_Specifiers *decl_spec = declaration_specifiers;
    Cn_Ast_Declarator *decl = opt.declarator;

    // No declarator, then just analyzing declaration specifiers.
    if (decl == NULL) {
        // In the case of attribute only declarations we just let it pass by for now. 
        // Later they can be utilized to generate type by meta program that user made. 
        // But for now they are not analyzed for any type information.
        if (decl_spec->type_specifier == NULL) return true;
            
        return cn_analyze_to_type(decl_spec->qualifiers, decl_spec->type_specifier, NULL, 0) != NULL;
    }

    if (decl_spec->type_specifier == NULL) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, decl_spec, CN_DC_MISSING_DECLARATION_SPECIFIER, "Type specifier must be present in analyzed declaration.");
    }

    // Getting type.
    Cn_Type *type = cn_analyze_to_type(
            decl_spec->qualifiers, 
            decl_spec->type_specifier, 
            decl, 
            CN_ANALYZE_NO_FLAGS
            );

    if (type == NULL) return false;

    // Getting identifier at the end of declarator chain.
    Cn_Ast_Node *identifier;
    cn_get_declarator_info(decl, &identifier);

    if (identifier->kind == CN_AST_IDENTIFIER) {
        Cn_String name = cn_ast_as(Identifier, identifier)->name;

        // Typedef declaration case.
        // IMPORTANT: Checked before everything else, since typedef NAMES a type.
        // For example: `typedef int fn_t(int);` if typdef case is not first,
        // the function `fn_t` would be declared and typedef would be ignored.
        if (decl_spec->storage_specifiers & CN_AST_STORAGE_SPECIFIER_TYPEDEF) {
            if (cn_binding_declare_typedef(name, identifier, type) == CN_BINDING_NIL_IDX) return false;

            return true;
        }

        // Function declaration case.
        if (type->kind == CN_FUNCTION) {
            Cn_Ast_Node *parameter_type_list;
            cn_get_function_declarator_info(
                    decl,
                    &parameter_type_list,
                    &identifier
                    );

            Cn_Ast_Nodes *parameter_declarations = NULL;
            if (parameter_type_list != NULL)
                parameter_declarations = &((Cn_Ast_Direct_Declarator_Function *)parameter_type_list)->parameter_declarations;

            if (cn_binding_declare_function(
                        name,
                        identifier,
                        decl_spec->storage_specifiers,
                        decl_spec->function_specifiers,
                        type,
                        parameter_declarations,
                        false
                        ) == CN_BINDING_NIL_IDX) return false;

            return true;
        }

        // Variable declaration case.
        bool is_definition = opt.initializer != NULL;

        if (cn_binding_declare_variable(
                    name,
                    identifier,
                    decl_spec->storage_specifiers,
                    type,
                    is_definition
                    ) == CN_BINDING_NIL_IDX) return false;


        // Analyze initializer here (only happens in variable declaration case).
        if (opt.initializer != NULL) {
            if (!cn_analyze_typecheck_initializer(opt.initializer, type)) return false;
        }

    }

    return true;
}

// CNDEF bool cn_analyze_declaration(void *declaration_specifiers, Cn_Ast_Nodes init_declarators) {
//     Cn_Ast_Declaration_Specifiers *decl_spec = declaration_specifiers;
// 
//     // No declarators, then just analyzing declaration specifiers.
//     if (init_declarators.length == 0) {
//         Cn_Type *type = cn_analyze_to_type(decl_spec->qualifiers, decl_spec->type_specifier, NULL);
//         return type != NULL;
//     }
// 
//     // Iterating over declarators.
//     for (int64_t i = 0; i < init_declarators.length; i++) {
//         Cn_Ast_Init_Declarator *init_decl = cn_ast_as(Init_Declarator, init_declarators.ptrs[i]);
// 
//         Cn_Type *type = cn_analyze_to_type(decl_spec->qualifiers, decl_spec->type_specifier, init_decl->declarator);
//         if (type == NULL) return false;
// 
//         // Adding declarator to variable or typedef binding, if it is not abstract.
//         Cn_Ast_Node *identifier;
//         cn_get_declarator_info(init_decl->declarator, &identifier);
// 
//         if (((Cn_Ast_Node *)identifier)->kind == CN_AST_IDENTIFIER) {
//             Cn_String name = ((Cn_Ast_Identifier *)identifier)->name;
// 
//             // Function declaration case.
//             if (type->kind == CN_FUNCTION) {
//                 Cn_Ast_Node *parameter_type_list;
//                 cn_get_function_declarator_info(
//                         init_decl->declarator,
//                         &parameter_type_list,
//                         &identifier
//                         );
// 
//                 Cn_Ast_Nodes *parameter_declarations = NULL;
//                 if (parameter_type_list != NULL)
//                     parameter_declarations = &((Cn_Ast_Direct_Declarator_Function *)parameter_type_list)->parameter_declarations;
// 
//                 if (cn_binding_declare_function(
//                             name,
//                             identifier,
//                             decl_spec->storage_specifiers,
//                             decl_spec->function_specifiers,
//                             type,
//                             parameter_declarations,
//                             false
//                             ) == CN_BINDING_NIL_IDX) return false;
// 
//                 continue;
//             }
// 
//             // Typedef declaration case.
//             if (decl_spec->storage_specifiers & CN_AST_STORAGE_SPECIFIER_TYPEDEF) {
//                 if (cn_binding_declare_typedef(name, identifier, type) == CN_BINDING_NIL_IDX) return false;
// 
//                 continue;
//             }
// 
//             // Variable declaration case.
//             bool is_definition = init_decl->initializer != NULL;
// 
//             if (cn_binding_declare_variable(
//                         name,
//                         identifier,
//                         decl_spec->storage_specifiers,
//                         type,
//                         is_definition
//                         ) == CN_BINDING_NIL_IDX) return false;
// 
// 
//             // Analyze initializer here (only happens in variable declaration case).
//             if (init_decl->initializer != NULL) {
//                 if (!cn_analyze_typecheck_initializer(init_decl->initializer, type)) return false;
//             }
//         }
//     }
// 
//     return true;
// }


CNDEF bool cn_analyze_function_definition(void * declaration_specifiers, void * declarator, Cn_Binding_Idx *function_binding_idx) {
    Cn_Ast_Declaration_Specifiers *decl_spec = declaration_specifiers;

    Cn_Type *type = cn_analyze_to_type(
            decl_spec->qualifiers, 
            decl_spec->type_specifier, 
            declarator,
            0
            );
    if (type == NULL) return false;

    Cn_Ast_Node *parameter_type_list;
    Cn_Ast_Node *identifier;

    bool ok = cn_get_function_declarator_info(
            declarator, 
            &parameter_type_list, 
            &identifier
            );


    if (!ok) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, identifier, CN_DC_INVALID_DECLARATOR, "Cannot have abstract or non function declarator in function definition.");
        return false;
    }

    Cn_Ast_Nodes *parameter_declarations = NULL;
    if (parameter_type_list != NULL) {
        parameter_declarations = &((Cn_Ast_Direct_Declarator_Function *)parameter_type_list)->parameter_declarations;
    }

    *function_binding_idx = cn_binding_declare_function(
            ((Cn_Ast_Identifier *)identifier)->name,
            identifier,
            decl_spec->storage_specifiers,
            decl_spec->function_specifiers,
            type,
            parameter_declarations,
            true
            );

    if (*function_binding_idx == CN_BINDING_NIL_IDX) return false;

    return true;
}

/**
 * Builds the base type for a primitive type specifier.
 *
 * RETURNS: The interned primitive type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_type_from_primitive(Cn_Ast_Node *ts) {
    Cn_Ast_Type_Specifier_Primitive *prim = cn_ast_as(Type_Specifier_Primitive, ts);
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
            return cn_analyze_intern_type((Cn_Type *)&type);
 
        case CN_AST_TYPE_CHAR:
            type.integer.kind = CN_INTEGER;
            type.integer.flags |= CN_TYPE_COMPLETE;
            type.integer.size = sizeof(char);
            type.integer.align = type.integer.size;
            type.integer.is_signed = prim->sign != CN_AST_TYPE_SIGN_UNSIGNED;
            return cn_analyze_intern_type((Cn_Type *)&type);
 
        case CN_AST_TYPE_FLOAT:
            type.kind = CN_FLOAT;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(float);
            type.align = type.integer.size;
            return cn_analyze_intern_type((Cn_Type *)&type);
 
        case CN_AST_TYPE_DOUBLE:
            type.kind = CN_FLOAT;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(double);
            type.align = type.size;
            return cn_analyze_intern_type((Cn_Type *)&type);
 
        case CN_AST_TYPE_BOOL:
            type.kind = CN_BOOL;
            type.flags |= CN_TYPE_COMPLETE;
            type.size = sizeof(_Bool);
            type.align = type.size;
            return cn_analyze_intern_type((Cn_Type *)&type);
 
        case CN_AST_TYPE_VOID:
            return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_VOID);
 
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts, CN_DC_INVALID_TYPE_SPECIFIER, "Unknown primitive type specifier.");
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
CNDEF Cn_Type *cn__analyze_type_from_typedef(Cn_Ast_Node *ts) {
    Cn_Ast_Type_Specifier_Typedef *tdef = cn_ast_as(Type_Specifier_Typedef, ts);
    
    Cn_Binding_Idx binding_idx = cn_binding_table_get(tdef->typedef_name, &cn__tu_data->symbol_binding_table);
    return cn_binding_get(binding_idx)->type;
}
 
CNDEF void cn_analyze_layout_default(Cn_Type *type, Cn_Layout_Opt opt) {
    CN_ASSERT(type->kind == CN_STRUCT || type->kind == CN_UNION || type->kind == CN_ENUM);

    int64_t size = 0, align = 0;

    switch (type->kind) {
        case CN_STRUCT:
        case CN_UNION:
            {
                bool is_struct = type->kind == CN_STRUCT;
                int64_t members_length = is_struct ? type->struct_t.members_length : type->union_t.members_length;

                for (int64_t i = 0; i < members_length; i++) {
                    Cn_Type *member_type = is_struct ? type->struct_t.members[i].type : type->union_t.members[i].type;
                    CN_ASSERT(member_type->align != 0);

                    // Explicit member alignment wins, otherwise natural alignment capped by max_align.
                    int64_t member_align = member_type->align;
                    if (opt.member_aligns != NULL && opt.member_aligns[i] > 0) {
                        member_align = opt.member_aligns[i];
                    } else if (opt.max_align > 0 && member_align > opt.max_align) {
                        member_align = opt.max_align;
                    }

                    if (is_struct) {
                        size = (size + member_align - 1) / member_align * member_align;
                        type->struct_t.members[i].offset = size;
                        size += member_type->size;
                    } else if (member_type->size > size) {
                        size = member_type->size;
                    }

                    if (member_align > align) align = member_align;
                }

                if (opt.min_align > align) align = opt.min_align;

                // Trailing padding, so every element of an array of this type is aligned.
                if (align > 0) size = (size + align - 1) / align * align;
                break;
            }
        case CN_ENUM:
            {
                size = type->enum_t.member_type->size;
                align = type->enum_t.member_type->align;
                if (opt.min_align > align) align = opt.min_align;
                break;
            }
        default:
            break;
    }

    type->size = size;
    type->align = align;
    type->flags |= CN_TYPE_COMPLETE;
}

#ifdef CN_GNU
/**
 * Alignment used by `aligned` attribute without argument, the biggest alignment of the target.
 */
#define CN_GNU_BIGGEST_ALIGNMENT 16

/**
 * Finds GNU attribute `name` among `specifiers`, written either in GNU syntax `__attribute__((name))`,
 * or as `[[gnu::name]]`. Both `name` and `__name__` spellings are matched.
 *
 * RETURNS: Found attribute, `NULL` if there is none.
 */
CNDEF Cn_Ast_Attribute *cn__gnu_find_attribute(Cn_Ast_Nodes specifiers, Cn_String name) {
    for (int64_t i = 0; i < specifiers.length; i++) {
        Cn_Ast_Attribute_Specifier *specifier = cn_ast_as(Attribute_Specifier, specifiers.ptrs[i]);
        bool is_gnu = (specifier->flags & CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU) != 0;

        for (int64_t j = 0; j < specifier->attributes.length; j++) {
            Cn_Ast_Attribute *attribute = cn_ast_as(Attribute, specifier->attributes.ptrs[j]);

            if (!is_gnu && (attribute->vendor_identifier == NULL || !cn_str_is(attribute->vendor_identifier->name, CN_STR_LIT("gnu")))) continue;

            Cn_String attribute_name = attribute->identifier->name;

            // Stripping `__` from both sides, for `__name__` spelling.
            if (attribute_name.length > 4 && cn_str_starts_with(attribute_name, CN_STR_LIT("__")) && cn_str_ends_with(attribute_name, CN_STR_LIT("__"))) {
                attribute_name = cn_str_substring(attribute_name, 2, attribute_name.length - 2);
            }

            if (cn_str_is(attribute_name, name)) return attribute;
        }
    }

    return NULL;
}

/**
 * Evaluates alignment requested by `aligned` attribute, `aligned` without argument requests
 * `CN_GNU_BIGGEST_ALIGNMENT`.
 *
 * RETURNS: Requested alignment, `-1` on error.
 */
CNDEF int64_t cn__gnu_attribute_alignment(Cn_Ast_Attribute *attribute) {
    if (attribute->arguments.length == 0) return CN_GNU_BIGGEST_ALIGNMENT;

    if (attribute->arguments.length != 1) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, attribute, CN_DC_ILLEGAL_ATTRIBUTE, "Attribute 'aligned' takes at most one argument.");
        return -1;
    }

    Cn_Ast_Node *expression = attribute->arguments.ptrs[0];

    // Attribute arguments are not typechecked when parsed, since their meaning depends on the attribute.
    if (cn_analyze_typecheck_expression(expression) == NULL) return -1;

    uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any value = cn_analyze_evaluate_expression(expression, buffer);
    if (cn_any_is_empty(value) || cn_type_unqualified(value.type)->kind != CN_INTEGER) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Argument of attribute 'aligned' must be an integer constant expression.");
        return -1;
    }

    int64_t alignment = cn_any_read_int(value);
    if (alignment <= 0 || (alignment & (alignment - 1)) != 0) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Argument of attribute 'aligned' must be a power of 2, got %lld.", (long long)alignment);
        return -1;
    }

    return alignment;
}

/**
 * Lays out tag `type` according to `packed` and `aligned` attributes of `tag_specifier` and of
 * struct or union members. Leaves type incomplete, for default layout, if none of them are present.
 *
 * RETURNS: `true` on success, `false` on error.
 */
CNDEF bool cn__gnu_analyze_layout(Cn_Ast_Node *tag_specifier, Cn_Type *type) {
    // Struct, union and enum specifiers all start with their attribute specifiers.
    Cn_Ast_Nodes tag_attributes = tag_specifier->kind == CN_AST_ENUM_SPECIFIER
        ? cn_ast_as(Enum_Specifier, tag_specifier)->attribute_specifiers
        : cn_ast_as(Struct_Specifier, tag_specifier)->attribute_specifiers;

    Cn_Ast_Attribute *packed  = cn__gnu_find_attribute(tag_attributes, CN_STR_LIT("packed"));
    Cn_Ast_Attribute *aligned = cn__gnu_find_attribute(tag_attributes, CN_STR_LIT("aligned"));

    Cn_Layout_Opt opt = {0};

    if (aligned != NULL) {
        opt.min_align = cn__gnu_attribute_alignment(aligned);
        if (opt.min_align < 0) return false;
    }

    if (type->kind == CN_ENUM) {
        if (packed == NULL && aligned == NULL) return true;

        cn_analyze_layout_default(type, opt);

        // Packed enum shrinks to the smallest integer that holds every enumerator value.
        if (packed != NULL) {
            int64_t size = 1;
            for (int64_t i = 0; i < type->enum_t.members_length; i++) {
                int64_t v = type->enum_t.members[i].value;
                while (size < 8 && (v < -(INT64_C(1) << (size * 8 - 1)) || v >= (INT64_C(1) << (size * 8)))) size *= 2;
            }

            type->size = size;
            type->align = size > opt.min_align ? size : opt.min_align;
        }

        return true;
    }

    // Struct and union, both specifiers share the same layout.
    Cn_Ast_Struct_Specifier *specifier = cn_ast_as(Struct_Specifier, tag_specifier);
    int64_t members_length = type->kind == CN_STRUCT ? type->struct_t.members_length : type->union_t.members_length;

    int64_t *member_aligns = CN_REALLOC(NULL, sizeof(int64_t) * (members_length > 0 ? members_length : 1));
    bool any_member_attribute = false;

    // Member `i` comes from `i`-th member declarator, in declaration order.
    int64_t i = 0;
    for (int64_t m = 0; m < specifier->member_declarations.length; m++) {
        Cn_Ast_Member_Declaration *md = cn_ast_as(Member_Declaration, specifier->member_declarations.ptrs[m]);

        for (int64_t d = 0; d < md->member_declarators.length; d++, i++) {
            Cn_Ast_Member_Declarator *declarator = cn_ast_as(Member_Declarator, md->member_declarators.ptrs[d]);
            Cn_Type *member_type = type->kind == CN_STRUCT ? type->struct_t.members[i].type : type->union_t.members[i].type;

            Cn_Ast_Attribute *member_packed  = cn__gnu_find_attribute(declarator->attribute_specifiers, CN_STR_LIT("packed"));
            Cn_Ast_Attribute *member_aligned = cn__gnu_find_attribute(declarator->attribute_specifiers, CN_STR_LIT("aligned"));

            // Packed member, or member of packed type, drops to alignment of 1.
            // Aligned member raises its alignment, but never lowers it unless packed.
            int64_t member_align = (packed != NULL || member_packed != NULL) ? 1 : member_type->align;

            if (member_aligned != NULL) {
                int64_t requested = cn__gnu_attribute_alignment(member_aligned);
                if (requested < 0) {
                    CN_FREE(member_aligns);
                    return false;
                }
                if (requested > member_align) member_align = requested;
            }

            if (member_packed != NULL || member_aligned != NULL) any_member_attribute = true;
            member_aligns[i] = member_align;
        }
    }

    if (packed == NULL && aligned == NULL && !any_member_attribute) {
        CN_FREE(member_aligns);
        return true;
    }

    opt.member_aligns = member_aligns;
    cn_analyze_layout_default(type, opt);

    CN_FREE(member_aligns);
    return true;
}

/**
 * Resolves `typeof(...)` to the type of its type name or expression operand.
 *
 * RETURNS: Resolved type, `NULL` on error.
 */
CNDEF Cn_Type *cn__gnu_analyze_typeof(Cn_Ast_Node *type_specifier) {
    Cn_Ast_Gnu_Typeof *typeof_spec = cn_ast_as(Gnu_Typeof, type_specifier);
    Cn_Type *result;

    if (typeof_spec->target->kind == CN_AST_TYPE_NAME) {
        Cn_Ast_Type_Name *tn = cn_ast_as(Type_Name, typeof_spec->target);
        Cn_Ast_Specifier_Qualifier *sq = tn->specifier_qualifier;
        result = cn_analyze_to_type(sq->qualifiers, sq->type_specifier, tn->abstract_declarator, 0);
    } else {
        // Expression operand was already typechecked when it was parsed.
        result = cn_get_expression_type(typeof_spec->target);
    }

    if (result == NULL) {
        CN__TRACE_ERROR
    }
    return result;
}
#endif // CN_GNU

/**
 * Helper function, lays out tag `type` whose members are already resolved, by analyze hooks
 * or by default layout, and records that binding got defined, so checkpoint rollback undefines it.
 *
 * RETURNS: `true` on success, `false` if a hook errored.
 */
CNDEF bool cn__analyze_complete_tag(Cn_Ast_Node *tag_specifier, Cn_Binding_Idx binding_idx, Cn_Type *type) {
    if (!cn__hook_analyze_layout(tag_specifier, type)) return false;

#ifdef CN_GNU
    // GNU `packed` and `aligned` attributes, only if no hook laid out the type already.
    if (!(type->flags & CN_TYPE_COMPLETE)) {
        if (!cn__gnu_analyze_layout(tag_specifier, type)) return false;
    }
#endif

    if (!(type->flags & CN_TYPE_COMPLETE)) {
        cn_analyze_layout_default(type, (Cn_Layout_Opt) {0});
    }

    cn_array_list_append(&cn__tu_data->binding_defined_idx_list, binding_idx);
    return true;
}

/**
 * Declares/completes the struct tag and builds its type: counts members, lays
 * out offsets and alignment, and marks the type complete when a body is present.
 *
 * RETURNS: The (possibly newly completed) struct type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_type_from_struct(Cn_Ast_Node *ts) {
    Cn_Ast_Struct_Specifier *struct_spec = cn_ast_as(Struct_Specifier, ts);
    Cn_String tag = struct_spec->identifier->name;
    Cn_Binding_Idx binding_idx = cn_binding_declare_tag(tag, CN_STRUCT);
    if (binding_idx == CN_BINDING_NIL_IDX) goto error;
 
    // IMPORTANT: Binding itself is not kept, member analysis can add bindings and move binding list.
    // Type is stored in type arena, so its pointer stays valid.
    Cn_Binding *binding = cn_binding_get(binding_idx);
    Cn_Type *type = binding->type;
 
    if (struct_spec->member_declarations.length > 0) {
        if ((type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts, CN_DC_REDEFINITION, "Redefinition of 'struct %.*s' is not allowed within the same scope.", CN_STR_UNPACK(tag));
            goto error;
        }
 
        Cn_Type_Struct *struct_type = (Cn_Type_Struct *)type;
 
        // Count members across all declarations and their declarators.
        struct_type->members_length = 0;
        for (int64_t m = 0; m < struct_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_as(Member_Declaration, struct_spec->member_declarations.ptrs[m]);
            struct_type->members_length += md->member_declarators.length;
        }
 
        struct_type->members = cn_chained_arena_alloc(&cn__tu_data->type_children_arena, struct_type->members_length * sizeof(Cn_Type_Struct_Member));
 
        int i = 0;
 
        for (int64_t m = 0; m < struct_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_as(Member_Declaration, struct_spec->member_declarations.ptrs[m]);
            Cn_Ast_Specifier_Qualifier *sq = md->specifier_qualifier;
 
            for (int64_t d = 0; d < md->member_declarators.length; d++) {
                Cn_Ast_Member_Declarator *decl = cn_ast_as(Member_Declarator, md->member_declarators.ptrs[d]);
                Cn_Type *member_type = cn_analyze_to_type(sq->qualifiers, sq->type_specifier, decl->declarator, 0);
                if (member_type == NULL) goto error;
 
                if (!(member_type->flags & CN_TYPE_COMPLETE)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, md->member_declarators.ptrs[d], CN_DC_INCOMPLETE_TYPE, "Cannot have incomplete member type in struct definition.");
                    goto error;
                }
 
                Cn_Ast_Node *identifier;
                cn_get_declarator_info(decl->declarator, &identifier);
 
                struct_type->members[i] = (Cn_Type_Struct_Member) {
                    .type = member_type,
                    .name = cn__ast_permanent_save_string(((Cn_Ast_Identifier *)identifier)->name),
                };
                i++;
            }
        }
 
        if (!cn__analyze_complete_tag(ts, binding_idx, type)) goto error;
    }
 
    return type;
 
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
CNDEF Cn_Type *cn__analyze_type_from_union(Cn_Ast_Node *ts) {
    Cn_Ast_Union_Specifier *union_spec = cn_ast_as(Union_Specifier, ts);
    Cn_String tag = union_spec->identifier->name;
    Cn_Binding_Idx binding_idx = cn_binding_declare_tag(tag, CN_UNION);
    if (binding_idx == CN_BINDING_NIL_IDX) goto error;
 
    // IMPORTANT: Binding itself is not kept, member analysis can add bindings and move binding list.
    Cn_Binding *binding = cn_binding_get(binding_idx);
    Cn_Type *type = binding->type;
 
    if (union_spec->member_declarations.length > 0) {
        if ((type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts, CN_DC_REDEFINITION, "Redefinition of 'union %.*s' is not allowed within the same scope.", CN_STR_UNPACK(tag));
            goto error;
        }
 
        Cn_Type_Union *union_type = (Cn_Type_Union *)type;
 
        union_type->members_length = 0;
        for (int64_t m = 0; m < union_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_as(Member_Declaration, union_spec->member_declarations.ptrs[m]);
            union_type->members_length += md->member_declarators.length;
        }
 
        union_type->members = cn_chained_arena_alloc(&cn__tu_data->type_children_arena, union_type->members_length * sizeof(Cn_Type_Union_Member));
 
        int i = 0;
 
        for (int64_t m = 0; m < union_spec->member_declarations.length; m++) {
            Cn_Ast_Member_Declaration *md = cn_ast_as(Member_Declaration, union_spec->member_declarations.ptrs[m]);
            Cn_Ast_Specifier_Qualifier *sq = md->specifier_qualifier;
 
            for (int64_t d = 0; d < md->member_declarators.length; d++) {
                Cn_Ast_Member_Declarator *decl = cn_ast_as(Member_Declarator, md->member_declarators.ptrs[d]);
                Cn_Type *member_type = cn_analyze_to_type(sq->qualifiers, sq->type_specifier, decl->declarator, 0);
                if (member_type == NULL) goto error;
 
                if (!(member_type->flags & CN_TYPE_COMPLETE)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, md->member_declarators.ptrs[d], CN_DC_INCOMPLETE_TYPE, "Cannot have incomplete member type in union definition.");
                    goto error;
                }
 
                Cn_Ast_Node *identifier;
                cn_get_declarator_info(decl->declarator, &identifier);
 
                union_type->members[i] = (Cn_Type_Union_Member) {
                    .type = member_type,
                    .name = cn__ast_permanent_save_string(((Cn_Ast_Identifier *)identifier)->name),
                };
                i++;
            }
        }
 
        if (!cn__analyze_complete_tag(ts, binding_idx, type)) goto error;
    }
 
    return type;
 
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
CNDEF Cn_Type *cn__analyze_type_from_enum(Cn_Ast_Node *ts) {
    Cn_Ast_Enum_Specifier *enum_spec = cn_ast_as(Enum_Specifier, ts);
    Cn_String tag = enum_spec->identifier->name;

    Cn_Binding_Idx binding_idx = cn_binding_declare_tag(tag, CN_ENUM);
    if (binding_idx == CN_BINDING_NIL_IDX) goto error;
 
    // IMPORTANT: Binding itself is not kept, enumerators add bindings and can move binding list.
    Cn_Binding *binding = cn_binding_get(binding_idx);
    Cn_Type *type = binding->type;
 
    if (enum_spec->flags & CN_AST_ENUM_HAS_DEFINITION) {
        if ((type->flags & CN_TYPE_COMPLETE) && binding->scope_idx == CN_SCOPE_STACK_CURRENT_IDX) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts, CN_DC_REDEFINITION, "Redefinition of 'enum %.*s' is not allowed within the same scope.", CN_STR_UNPACK(tag));
            goto error;
        }
 
        Cn_Type_Enum *enum_type = (Cn_Type_Enum *)type;
 
        enum_type->members_length = enum_spec->enumerators.length;
        enum_type->members = cn_chained_arena_alloc(&cn__tu_data->type_children_arena, enum_type->members_length * sizeof(Cn_Type_Enum_Member));
 
        Cn_Type *enum_base_type;
        if (enum_spec->specifier_qualifier != NULL) {
            Cn_Ast_Specifier_Qualifier *sq = enum_spec->specifier_qualifier;
            enum_base_type = cn_analyze_to_type(sq->qualifiers, sq->type_specifier, NULL, 0);

            if (enum_base_type == NULL) goto error;

            if (enum_base_type->kind != CN_INTEGER && enum_base_type->kind != CN_OPAQUE) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, ts, CN_DC_ILLEGAL_TYPE, "Enum base type cannot be non-integer type.");
                goto error;
            }
        } else {
            enum_base_type = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
        }
        enum_type->member_type = enum_base_type;

        int64_t enum_counter = 0;
        for (int64_t e = 0; e < enum_spec->enumerators.length; e++) {
            Cn_Ast_Enumerator *enumerator = cn_ast_as(Enumerator, enum_spec->enumerators.ptrs[e]);

            // Reset counter, by evaluating expression...
            if (enumerator->expression != NULL) {
                uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
                
                Cn_Type *type = cn_analyze_typecheck_expression(enumerator->expression);
                if (type == NULL) goto error;

                // AI: Generated (opaque case).
                if (type->kind != CN_INTEGER && !cn__analyze_is_opaque(type)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, enumerator->expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Expected integer result from constant expression in enumerator.");
                    goto error;
                } 

                Cn_Any result = cn_analyze_evaluate_expression(enumerator->expression, buffer);

                // AI: Generated.
                // Enumerator value must be known, opaque type is trusted, but its value can't be evaluated.
                if (cn_any_is_empty(result)) {
                    if (cn__analyze_is_opaque(type)) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, enumerator->expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Value of opaque expression in enumerator can't be evaluated.");
                    } else {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, enumerator->expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in enumerator.");
                    }
                    goto error;
                }

                enum_counter = cn_any_read_int(result);
            }

            Cn_String enumerator_name = enumerator->identifier->name;

            Cn_Binding_Idx enum_constant = cn_binding_declare_enum_constant(enumerator_name, enum_spec->enumerators.ptrs[e], enum_base_type, enum_counter);
            if (enum_constant == CN_BINDING_NIL_IDX) goto error;

            // Use name allocated from permanent arena cause binding saves name in temporary arena.
            enum_type->members[e].name = cn__ast_permanent_save_string(enumerator_name);
            enum_type->members[e].value = enum_counter;

            enum_counter++;
        }
        
        if (!cn__analyze_complete_tag(ts, binding_idx, type)) goto error;
    }
 
    return type;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Resolves the base type named by a type specifier, ignoring any declarator.
 *
 * RETURNS: The unqualified base type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_base_type_from_specifier(Cn_Ast_Node *type_specifier) {
    Cn_Ast_Node *ts = type_specifier;
    Cn_Type *result;

    switch (ts->kind) {
        case CN_AST_TYPE_SPECIFIER_PRIMITIVE:
            result = cn__analyze_type_from_primitive(type_specifier);
            break;
        case CN_AST_TYPE_SPECIFIER_TYPEDEF:
            result = cn__analyze_type_from_typedef(type_specifier);
            break;
        case CN_AST_STRUCT_SPECIFIER:
            result = cn__analyze_type_from_struct(type_specifier);
            break;
        case CN_AST_UNION_SPECIFIER:
            result = cn__analyze_type_from_union(type_specifier);
            break;
        case CN_AST_ENUM_SPECIFIER:
            result = cn__analyze_type_from_enum(type_specifier);
            break;
#ifdef CN_GNU
        case CN_AST_GNU_TYPEOF:
            result = cn__gnu_analyze_typeof(type_specifier);
            break;
#endif
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, type_specifier, CN_DC_INVALID_TYPE_SPECIFIER, "Unknown or invalid type specifier.");
            goto error;
    }
 
    if (result == NULL) goto error;
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Wraps `result` in one pointer layer, plus optional qualifier, per '*' in the
 * pointer chain.
 *
 * RETURNS: The pointer wrapped type.
 */
CNDEF Cn_Type *cn__analyze_type_apply_pointers(Cn_Type *result, Cn_Ast_Pointer *pointer) {
    while (pointer != NULL) {
        Cn_Ast_Pointer *ptr = pointer;
 
        Cn_Type type = cn_type_make_pointer(result);
        result = cn_analyze_intern_type((Cn_Type *)&type);
 
        if (ptr->qualifiers != 0) {
            type = cn_type_make_qualified(cn_ast_qualifier_flags_to_type(ptr->qualifiers), result);
            result = cn_analyze_intern_type((Cn_Type *)&type);
        }
 
        pointer = ptr->pointer;
    }
 
    return result;
}
 
/**
 * Wraps `result` in one array layer described by the array direct declarator at
 * dd, evaluating its constant length. 
 *
 * RETURNS: The array-wrapped type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_type_apply_array(Cn_Type *result, Cn_Ast_Node *dd, bool apply_parameter_adjustment) {
    
    Cn_Ast_Direct_Declarator_Array *arr = cn_ast_as(Direct_Declarator_Array, dd);
    
    // Casting down to pointer type if adjusting parameter.
    // TODO: Add qualifiers to pointer, 
    // once parsing qualifiers in array declarator is supported.
    if (apply_parameter_adjustment && (arr->direct_declarator == NULL || arr->direct_declarator->kind == CN_AST_IDENTIFIER)) {
        Cn_Type ptr_type = cn_type_make_pointer(result);
        result = cn_analyze_intern_type((Cn_Type *)&ptr_type);
        return result;
    }

    Cn_Type type = {0};
 
    type.array.kind = CN_ARRAY;
    type.array.flags |= CN_TYPE_COMPLETE;
 
    // Allow only complete element types.
    if (!(result->flags & CN_TYPE_COMPLETE)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd, CN_DC_INCOMPLETE_TYPE, "Incomplete type not allowed in array declarator.");
        goto error;
    }
 
    type.array.element_type = result;
    type.array.align = result->align;       // Align can be set in here, cause even if array size unknown, align is only dependent on the underlying element_type.

    // If arr->expression is NULL, it means that there is no expression, 
    // therefore we have incomplete array: '[]' case.
    if (arr->expression == NULL) {
        type.array.flags &= ~CN_TYPE_COMPLETE;
        return cn_analyze_intern_type((Cn_Type *)&type);
    }
 
    // Evaluate the constant length expression.
    uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any any = cn_analyze_evaluate_expression(arr->expression, buffer);
 
    if (cn_any_is_empty(any)) {
        // This is the case where expression inside '[...]' cannot be evaluated.
        // Therefore we have VLA.
        type.array.flags |= CN_TYPE_ARRAY_VARIABLE;
        return cn_analyze_intern_type((Cn_Type *)&type);
    }
 
    if (any.type->kind != CN_INTEGER) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd, CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array declarator.");
        goto error;
    }
 
    int64_t length = cn_any_read_int(any);
 
    if (length == 0) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, dd, CN_DC_INVALID_CONSTANT_EXPRESSION, "0 length array resulting from constant expression in array declarator.");
        goto error;
    }
 
    type.array.length = length;
    type.array.size = length * result->size;
 
    return cn_analyze_intern_type((Cn_Type *)&type);
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Wraps `result` in one function layer described by the function direct declarator
 * at dd, converting each parameter declaration to a parameter type.
 *
 * RETURNS: The function wrapped type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_type_apply_function(Cn_Type *result, Cn_Ast_Node *dd, bool apply_parameter_adjustment) {

    Cn_Ast_Direct_Declarator_Function *fn = cn_ast_as(Direct_Declarator_Function, dd);


    Cn_Type type = {0};
 
    type.function.kind = CN_FUNCTION;
    type.function.size = 0;
    type.function.align = 0;
    type.function.return_type = result;
    type.function.is_variadic = (fn->flags & CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC) != 0;
    type.function.params_length = fn->parameter_declarations.length;
 

    if (type.function.params_length > 0) {
        // This requires some explanation.
        // Basically C tracks all parameter scopes.
        // That means that parameter names matter, for example:
        //
        //  void foo(int n, int arr[n]);
        //                          ^~~~~~ If `int n` parameter is not tracked,
        //                                 the error `unknown primary identifer`
        //                                 will occure.
        //                                 
        // And the awful part is that since VLA's exist in parameter declarations, 
        // we are required to check whether certain constant expression is
        // valid whenever we analyze the type, 
        // so we must bind all parameters whenver we are deriving function type.
        //
        // It is not necessarely slow to push and pop stack each type function 
        // type is encountered. But it is defenitely  extra work, 
        // that I was not able to forsee, when thinking through this type system.
        cn_scope_stack_push();

        // Because if not, then VLA's would error on unknown names, example:
        //      void foo(int n, int arr[n]);
        type.function.params = cn_chained_arena_alloc(&cn__tu_data->type_children_arena, sizeof(Cn_Type_Function_Param) * type.function.params_length);
 
        for (int64_t i = 0; i < fn->parameter_declarations.length; i++) {
            Cn_Ast_Parameter_Declaration *p = cn_ast_as(Parameter_Declaration, fn->parameter_declarations.ptrs[i]);
            Cn_Ast_Declaration_Specifiers *decl_spec = p->declaration_specifiers;

            // Analyzing new parameter declaration.
            bool ok = cn_analyze_declaration(decl_spec, .declarator = p->declarator, .flags = CN_ANALYZE_APPLY_PARAMETER_ADJUSTMENT);
            if (!ok) goto error;


            Cn_Type *param_type = cn_analyze_to_type(decl_spec->qualifiers, decl_spec->type_specifier, p->declarator, CN_ANALYZE_APPLY_PARAMETER_ADJUSTMENT);
 
            type.function.params[i] = (Cn_Type_Function_Param) {
                .type = param_type,
            };
        }
        
        // Popping parameter scope.
        cn_scope_stack_pop();
        
    }

    result = cn_analyze_intern_type((Cn_Type *)&type);
    
    // Casting up to pointer type if adjusting parameter.
    if (apply_parameter_adjustment && (fn->direct_declarator == NULL || fn->direct_declarator->kind == CN_AST_IDENTIFIER)) {
        Cn_Type ptr_type = cn_type_make_pointer(result);
        result = cn_analyze_intern_type((Cn_Type *)&ptr_type);
    }
 
    return result;

error:
    CN__TRACE_ERROR
    return NULL;
}
 
/**
 * Walks the declarator chain, wrapping `result` in pointer, array or function layers
 * and descending into grouped declarators until each direct declarator bottoms
 * out at the identifier or an abstract end.
 *
 * RETURNS: The fully wrapped type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_type_apply_declarator(Cn_Type *result, Cn_Ast_Node *declarator, Cn_Analyze_Flags flags) {
    Cn_Ast_Node *declarator_chain = declarator;

    while (declarator_chain != NULL) {
        Cn_Ast_Declarator *decl = cn_ast_as(Declarator, declarator_chain);
 
        result = cn__analyze_type_apply_pointers(result, decl->pointer);
 
        // Direct declarator: array/function layers until the identifier, an
        // abstract end (NIL), or a grouped declarator to descend into.
        Cn_Ast_Node *dd = decl->direct_declarator;
        Cn_Ast_Node *next_declarator = NULL;
 
        while (dd != NULL) {
            if (dd->kind == CN_AST_IDENTIFIER) {
                break;
            }
 
            if (dd->kind == CN_AST_DIRECT_DECLARATOR_GROUPED) {
                next_declarator = cn_ast_upcast(((Cn_Ast_Direct_Declarator_Grouped *)dd)->declarator);
                break;
            }
 
            switch (dd->kind) {
                case CN_AST_DIRECT_DECLARATOR_ARRAY:
                    result = cn__analyze_type_apply_array(result, dd, flags & CN_ANALYZE_APPLY_PARAMETER_ADJUSTMENT);
                    if (result == NULL) goto error;
                    dd = ((Cn_Ast_Direct_Declarator_Array *)dd)->direct_declarator;
                    break;
 
                case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                    result = cn__analyze_type_apply_function(result, dd, flags & CN_ANALYZE_APPLY_PARAMETER_ADJUSTMENT);
                    if (result == NULL) goto error;
                    dd = ((Cn_Ast_Direct_Declarator_Function *)dd)->direct_declarator;
                    break;
 
                default:
                    dd = NULL;
                    break;
            }
        }
 
        // Only continues when a grouped declarator handed us an inner one.
        declarator_chain = next_declarator;
    }
 
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}
 
CNDEF Cn_Type *cn_analyze_to_type(Cn_Ast_Qualifier_Flags qualifier, void *type_specifier, void *declarator, Cn_Analyze_Flags flags) {
    CN_ASSERT(type_specifier != NULL);

    // Base type named by the specifier.
    Cn_Type *result = cn__analyze_base_type_from_specifier(type_specifier);
    if (result == NULL) goto error;
 
    // Qualifiers on the base type itself.
    if (qualifier != 0) {
        Cn_Type type = cn_type_make_qualified(cn_ast_qualifier_flags_to_type(qualifier), result);
        result = cn_analyze_intern_type((Cn_Type *)&type);
    }
 
    // Declarator layers (* [] ()).
    result = cn__analyze_type_apply_declarator(result, declarator, flags);
    if (result == NULL) goto error;
 
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_integer_promote(Cn_Type *type) {
    // Integer types of rank lower than int (and _Bool) promote to int.
    if (type->kind == CN_BOOL) {
        return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
    }
    if (type->kind == CN_INTEGER && type->size < (int64_t)sizeof(int)) {
        return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
    }
    return type;
}

CNDEF Cn_Type *cn__analyze_usual_arithmetic_conversion(Cn_Type *a, Cn_Type *b) {
    // Promote each operand, then pick the greater rank.
    a = cn__analyze_integer_promote(a);
    b = cn__analyze_integer_promote(b);
    return cn_type_greatest_arithmetic_rank(a, b);
}

CNDEF void cn__analyze_error_illegal_binary(Cn_Ast_Node *node, Cn_Type *left, Cn_Type *right, const char *op_desc) {
    Cn_String left_str  = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), left, (Cn_String) {0});
    Cn_String right_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), right, (Cn_String) {0});
    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Illegal %s between %.*s and %.*s types.", op_desc, CN_STR_UNPACK(left_str), CN_STR_UNPACK(right_str));
}

CNDEF bool cn__analyze_is_null_pointer_constant(Cn_Ast_Node *expression) {
    Cn_Ast_Primary *node = cn_ast_as(Primary, expression);

    if (node->kind == CN_AST_PRIMARY) {
        Cn_Ast_Integer *integer = cn_ast_as(Integer, node->literal);
        if (integer->kind == CN_AST_INTEGER) {
            return integer->value == 0;
        }
    }

    return false;
}

CNDEF bool cn__analyze_is_lvalue(Cn_Ast_Node *expression) {
    Cn_Ast_Node *node = expression;

    switch (node->kind) {
        case CN_AST_PRIMARY:
            // Only identifiers denote objects; integer/float/string literals don't.
            return ((Cn_Ast_Node *)(cn_ast_as(Primary, node)->literal))->kind == CN_AST_IDENTIFIER;

        case CN_AST_UNARY:
            // *p is an lvalue; -x, !x, &x, etc. are not.
            return cn_ast_as(Unary, node)->operator == CN_AST_UNARY_OP_DEREF;

        case CN_AST_ACCESS:
            // p->m is always an lvalue; s.m is an lvalue iff s is one.
            if (!(node->flags & CN_AST_ACCESS_IS_POINTER)) {
                return cn__analyze_is_lvalue(cn_ast_as(Access, node)->expression);
            }
            return true;

        case CN_AST_BINARY:
            // a[b] is an lvalue; no other binary result is.
            return cn_ast_as(Binary, node)->operator == CN_AST_BINARY_OP_ARRAY_SUB;

        case CN_AST_COMPOUND:
            // Compound literals counted as lvalues.
            return true;

        default:
            return false;
    }
}

/**
 * Expects expression to be typechecked.
 */
CNDEF bool cn__analyze_is_modifiable_lvalue(Cn_Ast_Node *expression) {
    Cn_Ast_Node *node = expression;
    Cn_Type *type = NULL;

    switch (node->kind) {
        case CN_AST_PRIMARY:
            if (((Cn_Ast_Node *)(cn_ast_as(Primary, node)->literal))->kind == CN_AST_IDENTIFIER) {
                // Only non-constant identifiers denote objects; integer/float/string literals don't.
                type = cn_ast_as(Primary, node)->type;

                if (cn_type_is_constant(type)) return false;

                return true;
            }

            return false;

        case CN_AST_UNARY:
            // *p is an lvalue; -x, !x, &x, etc. are not.
            // *p type must be non constant.
            if (cn_ast_as(Unary, node)->operator == CN_AST_UNARY_OP_DEREF) {
                type = cn_ast_as(Unary, node)->type;
                if (cn_type_is_constant(type)) return false;

                return true;
            }
            
            return false;

        case CN_AST_ACCESS:
            // p->m is always an lvalue; s.m is an lvalue iff s is one.
            // Base type of p must not be constant.
            // p->m type must not be constant.
            // s.m type must not be constant.
            type = cn_ast_as(Access, node)->type;
            if (cn_type_is_constant(type)) return false;

            if (!(node->flags & CN_AST_ACCESS_IS_POINTER)) {
                return cn__analyze_is_modifiable_lvalue(cn_ast_as(Access, node)->expression);
            } 

            Cn_Type *p = cn_get_expression_type(cn_ast_as(Access, node)->expression);
            p = cn_type_unqualified(p);
            // AI: Generated.
            if (p->kind == CN_OPAQUE) return true;
            if (p->kind != CN_POINTER) return false;
            if (cn_type_is_constant(((Cn_Type_Pointer *)p)->ptr_to)) return false;

            return true;

        case CN_AST_BINARY:
            // a[b] is an lvalue; no other binary result is.
            // a[b] must not be constant.
            if (cn_ast_as(Binary, node)->operator == CN_AST_BINARY_OP_ARRAY_SUB) {
                type = cn_ast_as(Binary, node)->type;
                if (cn_type_is_constant(type)) return false;

                return true;
            }

            return false;

        default:
            return false;
    }
}

CNDEF Cn_Type *cn__analyze_typecheck_binary(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_BINARY);

    Cn_Type *left = cn_analyze_typecheck_expression(cn_ast_as(Binary, node)->left);
    if (left == NULL) goto error;

    Cn_Type *right = cn_analyze_typecheck_expression(cn_ast_as(Binary, node)->right);
    if (right == NULL) goto error;

    // Dropping qualifiers for binary typechecing.
    left = cn_type_unqualified(left);
    right = cn_type_unqualified(right);

    // AI: Generated.
    // Opaque operands are trusted, see `cn__analyze_is_opaque`.
    bool left_opaque  = left->kind  == CN_OPAQUE;
    bool right_opaque = right->kind == CN_OPAQUE;

    switch (cn_ast_as(Binary, node)->operator) {
        case CN_AST_BINARY_OP_ADDITION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn_type_greatest_arithmetic_rank(left, right);
                }
                if (left->kind == CN_POINTER || right->kind == CN_POINTER) {
                    Cn_Type *ptr_type   = left->kind == CN_POINTER ? left  : right;
                    Cn_Type *other_type = left->kind == CN_POINTER ? right : left;
                    // AI: Generated.
                    // Opaque is trusted to be an integer.
                    if (other_type->kind != CN_INTEGER && other_type->kind != CN_OPAQUE) {
                        cn__analyze_error_illegal_binary(node, left, right, "pointer addition");
                        goto error;
                    }
                    return ptr_type;
                }
                cn__analyze_error_illegal_binary(node, left, right, "addition");
                goto error;
            }

        case CN_AST_BINARY_OP_SUBTRACTION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn_type_greatest_arithmetic_rank(left, right);
                }
                if (left->kind == CN_POINTER || right->kind == CN_POINTER) {
                    Cn_Type *ptr_type   = left->kind == CN_POINTER ? left  : right;
                    Cn_Type *other_type = left->kind == CN_POINTER ? right : left;
                    // AI: Generated.
                    // 'p - opaque' is either a pointer or a difference, so result is opaque.
                    // 'opaque - p' is only valid as a difference of two pointers.
                    if (other_type->kind == CN_OPAQUE) {
                        if (left->kind == CN_POINTER) return other_type;
                        return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_PTRDIFF);
                    }
                    if (other_type->kind != CN_INTEGER) {
                        if (other_type->kind == CN_POINTER) {
                            return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_PTRDIFF);
                        }
                        cn__analyze_error_illegal_binary(node, left, right, "pointer subtraction");
                        goto error;
                    }
                    return ptr_type;
                }
                cn__analyze_error_illegal_binary(node, left, right, "subtraction");
                goto error;
            }

        case CN_AST_BINARY_OP_ARRAY_SUB: 
            {
                // AI: Generated (opaque cases).
                if (left->kind != CN_POINTER && left->kind != CN_ARRAY && !left_opaque) {
                    cn__analyze_error_illegal_binary(node, left, right, "array subscript (left operand must be a pointer or array)");
                    goto error;
                }
                if (right->kind != CN_INTEGER && !right_opaque) {
                    cn__analyze_error_illegal_binary(node, left, right, "array subscript (subscript must be an integer)");
                    goto error;
                }

                // Element type of opaque is unknown.
                if (left_opaque) return left;

                return left->kind == CN_POINTER ? left->pointer.ptr_to : left->array.element_type;
            }

        // Arithmetic, usual arithmetic conversions.
        case CN_AST_BINARY_OP_MULTIPLICATION:
        case CN_AST_BINARY_OP_DIVISION: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) {
                    return cn__analyze_usual_arithmetic_conversion(left, right);
                }
                cn__analyze_error_illegal_binary(node, left, right,
                        cn_ast_as(Binary, node)->operator == CN_AST_BINARY_OP_MULTIPLICATION
                        ? "multiplication" : "division");
                goto error;
            }

        // Integer-only, usual arithmetic conversions.
        case CN_AST_BINARY_OP_MODULO:
        case CN_AST_BINARY_OP_BIT_AND:
        case CN_AST_BINARY_OP_BIT_XOR:
        case CN_AST_BINARY_OP_BIT_OR: 
            {
                // AI: Generated (opaque cases).
                if ((left->kind == CN_INTEGER || left_opaque) && (right->kind == CN_INTEGER || right_opaque)) {
                    return cn__analyze_usual_arithmetic_conversion(left, right);
                }
                cn__analyze_error_illegal_binary(node, left, right, "integer operation");
                goto error;
            }

        // Shifts: each operand promoted independently; result is the promoted LEFT
        // operand. The right operand does NOT participate in the result type.
        case CN_AST_BINARY_OP_LSHIFT:
        case CN_AST_BINARY_OP_RSHIFT: 
            {
                // AI: Generated (opaque cases).
                // Opaque left operand stays opaque after promotion.
                if ((left->kind == CN_INTEGER || left_opaque) && (right->kind == CN_INTEGER || right_opaque)) {
                    return cn__analyze_integer_promote(left);
                }
                cn__analyze_error_illegal_binary(node, left, right, "shift");
                goto error;
            }

        // Relational: arithmetic vs arithmetic, or pointer vs pointer. Result is int.
        case CN_AST_BINARY_OP_LESS:
        case CN_AST_BINARY_OP_LESS_EQ:
        case CN_AST_BINARY_OP_GREATER:
        case CN_AST_BINARY_OP_GREATER_EQ: 
            {
                // AI: Generated (opaque cases).
                // Arithmetic covers opaque, so only pointer vs opaque is added.
                bool ok = (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right))
                    || (left->kind == CN_POINTER && right->kind == CN_POINTER)
                    || (left->kind == CN_POINTER && right_opaque)
                    || (left_opaque && right->kind == CN_POINTER);
                if (!ok) {
                    cn__analyze_error_illegal_binary(node, left, right, "relational comparison");
                    goto error;
                }
                return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
            }

        // Equality: as relational, plus pointer vs integer (null constant). Result int.
        case CN_AST_BINARY_OP_EQ:
        case CN_AST_BINARY_OP_NOT_EQ: 
            {
                // AI: Generated (opaque cases).
                // Arithmetic covers opaque, so only pointer vs opaque is added.
                bool ok = (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right))
                    || (left->kind == CN_POINTER && right->kind == CN_POINTER)
                    || (left->kind == CN_POINTER && right->kind == CN_INTEGER)
                    || (left->kind == CN_INTEGER && right->kind == CN_POINTER)
                    || (left->kind == CN_POINTER && right_opaque)
                    || (left_opaque && right->kind == CN_POINTER);
                if (!ok) {
                    cn__analyze_error_illegal_binary(node, left, right, "equality comparison");
                    goto error;
                }
                return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
            }

        // Logical: scalar operands, no arithmetic conversions, result is int.
        case CN_AST_BINARY_OP_AND:
        case CN_AST_BINARY_OP_OR: {
            if (cn_type_is_scalar(left) && cn_type_is_scalar(right)) {
                return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
            }
            cn__analyze_error_illegal_binary(node, left, right, "logical operation");
            goto error;
        }

        // Comma: result is the type of the right operand.
        case CN_AST_BINARY_OP_COMMA:
            return right;

        default: {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_TOKEN, "Expected binary operator.");
            goto error;
        }
    }

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_typecheck_access(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_ACCESS);

    Cn_Type *base = cn_analyze_typecheck_expression(cn_ast_as(Access, node)->expression);
    if (base == NULL) goto error;

    // Removing qualifiers from base type here, won't need them.
    base = cn_type_unqualified(base);

    // AI: Generated.
    // Opaque base is trusted to have the member, its type is unknown.
    if (base->kind == CN_OPAQUE) return base;

    Cn_Type *struct_or_union_type;
    if (node->flags & CN_AST_ACCESS_IS_POINTER) {
        // '->' : operand must be a pointer to a struct/union.
        if (base->kind != CN_POINTER) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "'->' requires a pointer operand.");
            goto error;
        }
        struct_or_union_type = ((Cn_Type_Pointer *)base)->ptr_to;
    } else {
        // '.' : operand must be a struct/union directly.
        struct_or_union_type = base;
    }

    // Stripping qualifiers because, we won't need them in following checks.
    struct_or_union_type = cn_type_unqualified(struct_or_union_type);

    // AI: Generated.
    // Pointer to opaque, same as opaque base.
    if (struct_or_union_type->kind == CN_OPAQUE) return struct_or_union_type;

    if (struct_or_union_type->kind != CN_STRUCT && struct_or_union_type->kind != CN_UNION) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Member access on non-struct or non-union type.");
        goto error;
    }

    if (!(struct_or_union_type->flags & CN_TYPE_COMPLETE)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_INCOMPLETE_TYPE, "Cannot access member of incomplete type.");
        goto error;
    }

    Cn_Ast_Identifier *ident = (cn_ast_as(Access, node)->member);
    
    if (struct_or_union_type->kind == CN_STRUCT) {
        Cn_Type_Struct *ts = (Cn_Type_Struct *)struct_or_union_type;
        for (int64_t i = 0; i < ts->members_length; i++) {
            if (cn_str_equals(&ts->members[i].name, &ident->name)) {
                return ts->members[i].type;
            }
        }
    } else {
        Cn_Type_Union *tu = (Cn_Type_Union *)struct_or_union_type;
        for (int64_t i = 0; i < tu->members_length; i++) {
            if (cn_str_equals(&tu->members[i].name, &ident->name)) {
                return tu->members[i].type;
            }
        }
    }


    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_INVALID_SYMBOL, "No member named '%.*s' in struct.", CN_STR_UNPACK(ident->name));
    goto error;

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_typecheck_call(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_CALL);

    Cn_Type *callee = cn_analyze_typecheck_expression(cn_ast_as(Call, node)->expression);
    if (callee == NULL) goto error;

    // A function itself or a pointer to function may be called. Getting function type itself.
    Cn_Type *fn = callee;
    if (fn->kind == CN_POINTER) fn = ((Cn_Type_Pointer *)fn)->ptr_to;

    // AI: Generated.
    // If opaque, or pointer to opaque, call is trusted, arguments can't be checked against parameters.
    // But arguments themselves are still typechecked, so their symbols are resolved and types are known.
    if (cn__analyze_is_opaque(fn)) {
        for (int64_t i = 0; i < cn_ast_as(Call, node)->arguments.length; i++) {
            if (cn_analyze_typecheck_expression(cn_ast_as(Call, node)->arguments.ptrs[i]) == NULL) goto error;
        }
        return fn;
    }
    
    if (fn->kind != CN_FUNCTION) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Called object is not a function or function pointer.");
        goto error;
    }


    Cn_Type_Function *func_type = (Cn_Type_Function *)fn;

    // Argument count must match parameter count.
    // NOTE: variadics aren't tracked in Cn_Type_Function yet, 
    // so this is exact-match only.
    // TODO: ^^^ Is this still a thing?
    int64_t arg_count = cn_ast_as(Call, node)->arguments.length;
    if (func_type->is_variadic) {
        if (arg_count < func_type->params_length) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Variadic function expects at least %lld argument(s) but %lld were provided.", func_type->params_length, arg_count);
            goto error;
        }
    } else if (arg_count != func_type->params_length) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Function expects %lld argument(s) but %lld were provided.", func_type->params_length, arg_count);
        goto error;
    }

    // Each argument must be assignable to its corresponding parameter type.
    for (int64_t i = 0; i < cn_ast_as(Call, node)->arguments.length; i++) {
        Cn_Ast_Node *arg = cn_ast_as(Call, node)->arguments.ptrs[i];

        Cn_Type *arg_type = cn_analyze_typecheck_expression(arg);
        if (arg_type == NULL) goto error;


        // If parameter is not part of variadic list, typechecking with function param type, otherwise ignoring.
        if (!func_type->is_variadic || i < func_type->params_length) {
            Cn_Type *param_type = func_type->params[i].type;

            if (!cn_type_is_assignable(param_type, arg_type)) {
                // Additionally to checking type assignable checking edge case of NULL pointer assignment to pointer type, example int *a = 0
                if (!(param_type->kind == CN_POINTER && cn__analyze_is_null_pointer_constant(arg))) {
                        Cn_String a_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), arg_type, (Cn_String) {0});
                        Cn_String p_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), param_type, (Cn_String) {0});
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Argument %lld of type %.*s is not assignable to parameter type %.*s.", i + 1, CN_STR_UNPACK(a_str), CN_STR_UNPACK(p_str));
                        goto error;
                }
            }
        }
    }

    return func_type->return_type;

error:
    CN__TRACE_ERROR
    return NULL;

}

CNDEF Cn_Type *cn__analyze_typecheck_unary(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_UNARY);

    Cn_Ast_Node *operand_node = cn_ast_as(Unary, node)->expression;
    Cn_Type *operand = cn_analyze_typecheck_expression(operand_node);
    if (operand == NULL) goto error;

    switch (cn_ast_as(Unary, node)->operator) {
        // ++x / --x : modifiable lvalue of scalar type; result is the operand's type.
        case CN_AST_UNARY_OP_INCREMENT:
        case CN_AST_UNARY_OP_DECREMENT: 
            {
                if (!cn__analyze_is_modifiable_lvalue(operand_node)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of '++'/'--' is not a modifiable lvalue.");
                    goto error;
                }
                if (!cn_type_is_scalar(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of '++'/'--' must be of scalar type.");
                    goto error;
                }

                return operand;
            }

        // +x / -x : arithmetic operand; result is integer-promoted.
        case CN_AST_UNARY_OP_POSITIVE:
        case CN_AST_UNARY_OP_NEGATIVE: 
            {
                if (!cn_type_is_arithmetic(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of unary '+'/'-' must be of arithmetic type.");
                    goto error;
                }
                return cn__analyze_integer_promote(operand);
            }

        // ~x : integer operand; result is integer-promoted.
        case CN_AST_UNARY_OP_BIT_NOT: 
            {
                // AI: Generated (opaque case).
                if (operand->kind != CN_INTEGER && !cn__analyze_is_opaque(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of '~' must be of integer type.");
                    goto error;
                }
                return cn__analyze_integer_promote(operand);
            }

        // !x : scalar operand; result is always int.
        case CN_AST_UNARY_OP_NOT: 
            {
                if (!cn_type_is_scalar(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of '!' must be of scalar type.");
                    goto error;
                }
                return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
            }

        // *x : pointer operand; result is the pointed-to type (an lvalue).
        case CN_AST_UNARY_OP_DEREF:
            {
                // AI: Generated.
                // Opaque is trusted to be a pointer, pointed-to type is unknown.
                if (cn__analyze_is_opaque(operand)) return operand;

                if (operand->kind != CN_POINTER) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot dereference a non-pointer type.");
                    goto error;
                }

                Cn_Type *ptr_to = ((Cn_Type_Pointer *)operand)->ptr_to;
                if (ptr_to->kind == CN_VOID) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot dereference a void * type.");
                    goto error;
                }

                return ptr_to;
            }

        // &x : operand must be an lvalue; result is pointer-to-operand-type.
        case CN_AST_UNARY_OP_ADDROF: 
            {
                if (!cn__analyze_is_lvalue(operand_node)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot take the address of a non-lvalue.");
                    goto error;
                }
                Cn_Type *void_t = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_VOID);
                Cn_Type ptr = cn_type_make_pointer(void_t);
                return cn_analyze_intern_type(&ptr);
            }

        default: 
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_TOKEN, "Expected unary operator.");
                goto error;
            }
    }

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_typecheck_cast(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_CAST);

    // Resolve the target type from the typename.
    Cn_Ast_Node *type_name = cn_ast_upcast((cn_ast_as(Cast, node)->type_name));
    Cn_Ast_Node *spec_qual = cn_ast_upcast((cn_ast_as(Type_Name, type_name)->specifier_qualifier));
    Cn_Type *target = cn_analyze_to_type(cn_ast_as(Specifier_Qualifier, spec_qual)->qualifiers, cn_ast_as(Specifier_Qualifier, spec_qual)->type_specifier, cn_ast_as(Type_Name, type_name)->abstract_declarator, 0);
    if (target == NULL) goto error;

    Cn_Type *operand = cn_analyze_typecheck_expression(cn_ast_as(Cast, node)->expression);
    if (operand == NULL) goto error;

    // Cast to void: always allowed, value is discarded.
    if (target->kind == CN_VOID) {
        return target;
    }

    // Dropping qualifiers, as they are not needed for casting.
    target = cn_type_unqualified(target);
    operand = cn_type_unqualified(operand);

    // Target must be scalar.
    if (!cn_type_is_scalar(target)) {
        Cn_String t_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), target, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot cast to non-scalar type %.*s.", CN_STR_UNPACK(t_str));
        goto error;
    }

    // Operand must be scalar.
    if (!cn_type_is_scalar(operand)) {
        Cn_String o_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), operand, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot cast from non-scalar type %.*s.", CN_STR_UNPACK(o_str));
        goto error;
    }

    // Floating types can't be cast to or from pointers.
    if ((target->kind == CN_POINTER && operand->kind == CN_FLOAT) || (target->kind == CN_FLOAT && operand->kind == CN_POINTER)) {
        Cn_String t_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), target, (Cn_String) {0});
        Cn_String o_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), operand, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot cast between floating type and pointer %.*s to %.*s.", CN_STR_UNPACK(o_str), CN_STR_UNPACK(t_str));
        goto error;
    }

    return target;


error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_typecheck_compound(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_COMPOUND);

    // Resolve the target type from the typename.
    Cn_Ast_Node *type_name = cn_ast_upcast((cn_ast_as(Compound, node)->type_name));
    Cn_Ast_Node *spec_qual = cn_ast_upcast((cn_ast_as(Type_Name, type_name)->specifier_qualifier));
    Cn_Type *target = cn_analyze_to_type(cn_ast_as(Specifier_Qualifier, spec_qual)->qualifiers, cn_ast_as(Specifier_Qualifier, spec_qual)->type_specifier, cn_ast_as(Type_Name, type_name)->abstract_declarator, 0);
    if (target == NULL) goto error;

    if (!(cn_type_unqualified(target)->flags & CN_TYPE_COMPLETE)) {
        Cn_String t_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), target, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_as(Compound, node)->type_name, CN_DC_ILLEGAL_TYPE, "Cannot have incomplete target type %.*s in compound literal.", CN_STR_UNPACK(t_str));
         goto error;
    }
    
    if (!cn_analyze_typecheck_designations(cn_ast_as(Compound, node)->designations, target))  goto error;

    return target;

error:
    CN__TRACE_ERROR
    return NULL;

}

CNDEF Cn_Type *cn__analyze_typecheck_sizeof(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_SIZEOF);

    Cn_Ast_Node *child = (cn_ast_as(Sizeof, node)->target);

    Cn_Type *operand;
    if (child->kind == CN_AST_TYPE_NAME) {
        // Case: sizeof typename.
        Cn_Ast_Specifier_Qualifier *spec_qual = (cn_ast_as(Type_Name, child)->specifier_qualifier);
        operand = cn_analyze_to_type(
            spec_qual->qualifiers,
            spec_qual->type_specifier,
            cn_ast_as(Type_Name, child)->abstract_declarator,
            0
            );
    } else {
        // Case: sizeof expr, operand is not evaluated; only its type is needed.
        operand = cn_analyze_typecheck_expression(cn_ast_as(Sizeof, node)->target);
    }

    if (operand == NULL) goto error;

    if (operand->kind == CN_FUNCTION) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot take 'sizeof' of a function type.");
        goto error;
    }

    if (operand->kind == CN_VOID) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot take 'sizeof' of a void type.");
        goto error;
    }

    if (!(operand->flags & CN_TYPE_COMPLETE)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_INCOMPLETE_TYPE, "Cannot take 'sizeof' of an incomplete type.");
        goto error;
    }

    return cn_analyze_intern_type((Cn_Type *)&CN_TYPE_SIZE);

error:
    CN__TRACE_ERROR
    return NULL;

}

CNDEF Cn_Type *cn__analyze_typecheck_ternary(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_TERNARY);

    Cn_Type *cond = cn_analyze_typecheck_expression(cn_ast_as(Ternary, node)->condition);
    if (cond == NULL) goto error;

    // Condition must be scalar.
    if (!cn_type_is_scalar(cond)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Condition of '?:' must be of scalar type.");
        goto error;
    }

    Cn_Type *a = cn_analyze_typecheck_expression(cn_ast_as(Ternary, node)->if_true);
    if (a == NULL) goto error;

    Cn_Type *b = cn_analyze_typecheck_expression(cn_ast_as(Ternary, node)->if_false);
    if (b == NULL) goto error;

    // AI: Generated.
    // Opaque operand is trusted to have a common type with the other operand, which is unknown.
    if (cn__analyze_is_opaque(a)) return a;
    if (cn__analyze_is_opaque(b)) return b;

    // Both arithmetic.
    if (cn_type_is_arithmetic(a) && cn_type_is_arithmetic(b)) {
        return cn__analyze_usual_arithmetic_conversion(a, b);
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
        Cn_Ast_Node *ia = cn_ast_as(Ternary, node)->if_true;
        Cn_Ast_Node *ib = cn_ast_as(Ternary, node)->if_false;

        // pointer  ?:  null pointer constant  ->  the pointer type.
        if (a->kind == CN_POINTER && cn__analyze_is_null_pointer_constant(ib)) return a;
        if (b->kind == CN_POINTER && cn__analyze_is_null_pointer_constant(ia)) return b;

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
        Cn_String a_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), a, (Cn_String) {0});
        Cn_String b_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), b, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Incompatible operand types in '?:' expression, %.*s and %.*s.", CN_STR_UNPACK(a_str), CN_STR_UNPACK(b_str));
    }
    goto error;

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_typecheck_assignment(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_ASSIGN);

    Cn_Type *left = cn_analyze_typecheck_expression(cn_ast_as(Assign, node)->left);
    if (left == NULL) goto error;

    Cn_Type *right = cn_analyze_typecheck_expression(cn_ast_as(Assign, node)->right);
    if (right == NULL) goto error;

    // Left must be a modifiable lvalue for every assignment form.
    if (!cn__analyze_is_modifiable_lvalue(cn_ast_as(Assign, node)->left)) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Left operand of assignment is not a modifiable lvalue.");
        goto error;
    }
    
    // Droping qualifiers, since we already checked the lvalue.
    left = cn_type_unqualified(left);
    right = cn_type_unqualified(right);

    switch (cn_ast_as(Assign, node)->operator) {
        // Simple assignment: right must be assignable to left, with NULL constant to pointer case handled too.
        case CN_AST_ASSIGNMENT_OP_ASSIGN: 
            {
                if (!cn_type_is_assignable(left, right)) {

                    if (!(left->kind == CN_POINTER && cn__analyze_is_null_pointer_constant(cn_ast_as(Assign, node)->right))) {
                        Cn_String left_str  = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), left, (Cn_String) {0});
                        Cn_String right_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), right, (Cn_String) {0});
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Cannot assign %.*s to %.*s.", CN_STR_UNPACK(right_str), CN_STR_UNPACK(left_str));
                        goto error;
                    }
                }
                return left;
            }

        // '+=' / '-=' : arithmetic both sides, OR pointer-left with integer-right.
        case CN_AST_ASSIGNMENT_OP_PLUS:
        case CN_AST_ASSIGNMENT_OP_MINUS: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) return left;
                // AI: Generated (opaque case).
                if (left->kind == CN_POINTER && (right->kind == CN_INTEGER || right->kind == CN_OPAQUE)) return left;
                cn__analyze_error_illegal_binary(node, left, right, cn_ast_as(Assign, node)->operator == CN_AST_ASSIGNMENT_OP_PLUS ? "'+=' assignment" : "'-=' assignment");
                goto error;
            }

        // '*=' / '/=' : arithmetic both sides.
        case CN_AST_ASSIGNMENT_OP_MULTIPLY:
        case CN_AST_ASSIGNMENT_OP_DIVIDE: 
            {
                if (cn_type_is_arithmetic(left) && cn_type_is_arithmetic(right)) return left;
                cn__analyze_error_illegal_binary(node, left, right, "arithmetic assignment");
                goto error;
            }

        // '%=' '<<=' '>>=' '&=' '^=' '|=' : integer both sides.
        case CN_AST_ASSIGNMENT_OP_MODULO:
        case CN_AST_ASSIGNMENT_OP_LSHIFT:
        case CN_AST_ASSIGNMENT_OP_RSHIFT:
        case CN_AST_ASSIGNMENT_OP_BIT_AND:
        case CN_AST_ASSIGNMENT_OP_BIT_XOR:
        case CN_AST_ASSIGNMENT_OP_BIT_OR: 
            {
                // AI: Generated (opaque cases).
                if ((left->kind == CN_INTEGER || left->kind == CN_OPAQUE) && (right->kind == CN_INTEGER || right->kind == CN_OPAQUE)) return left;
                cn__analyze_error_illegal_binary(node, left, right, "integer assignment");
                goto error;
            }

        default: 
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_TOKEN, "Expected assignment operator.");
                goto error;
            }
    }

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn__analyze_typecheck_postfix(Cn_Ast_Node *node) {
    CN_ASSERT(node->kind == CN_AST_POSTFIX);

    Cn_Ast_Node *operand_node = cn_ast_as(Postfix, node)->expression;
    Cn_Type *operand = cn_analyze_typecheck_expression(operand_node);
    if (operand == NULL) goto error;

    switch (cn_ast_as(Postfix, node)->operator) {
        // x++ / x-- : modifiable lvalue of scalar type; result is the operand's type.
        case CN_AST_POSTFIX_OP_INCREMENT:
        case CN_AST_POSTFIX_OP_DECREMENT: 
            {
                if (!cn__analyze_is_modifiable_lvalue(operand_node)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of postfix '++'/'--' is not a modifiable lvalue.");
                    goto error;
                }
                if (!cn_type_is_scalar(operand)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_ILLEGAL_TYPE, "Operand of postfix '++'/'--' must be of scalar type.");
                    goto error;
                }
                return operand;
            }

        default: 
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_TOKEN, "Expected postfix operator.");
                goto error;
            }
    }

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Type *cn_analyze_typecheck_expression(void * expression) {
    CN_ASSERT(expression != NULL);
    Cn_Ast_Node *node = expression;

    Cn_Type *result = NULL;

    switch(node->kind) {
        case CN_AST_BINARY: 
            {   
                result = cn__analyze_typecheck_binary(node);
                cn_ast_as(Binary, node)->type = result;
                break;
            }
        case CN_AST_ACCESS:
            {
                result = cn__analyze_typecheck_access(node);
                cn_ast_as(Access, node)->type = result;
                break;
            }
        case CN_AST_CALL:
            {
                result = cn__analyze_typecheck_call(node);
                cn_ast_as(Call, node)->type = result;
                break;
            }
        case CN_AST_UNARY:
            {
                result = cn__analyze_typecheck_unary(node);
                cn_ast_as(Unary, node)->type = result;
                break;
            }
        case CN_AST_CAST:
            {
                result = cn__analyze_typecheck_cast(node);
                cn_ast_as(Cast, node)->type = result;
                break;
            }
        case CN_AST_COMPOUND:
            {
                result = cn__analyze_typecheck_compound(node);
                cn_ast_as(Cast, node)->type = result;
                break;
            }
        case CN_AST_SIZEOF:
            {
                result = cn__analyze_typecheck_sizeof(node);
                cn_ast_as(Sizeof, node)->type = result;
                break;
            }
        case CN_AST_TERNARY:
            {   
                result = cn__analyze_typecheck_ternary(node);
                cn_ast_as(Ternary, node)->type = result;
                break;
            }
        case CN_AST_ASSIGN:
            {
                result = cn__analyze_typecheck_assignment(node);
                cn_ast_as(Assign, node)->type = result;
                break;
            }
        case CN_AST_POSTFIX:
            {
                result = cn__analyze_typecheck_postfix(node);
                cn_ast_as(Postfix, node)->type = result;
                break;
            }
        case CN_AST_PRIMARY:
            {
                Cn_Ast_Node *literal = (cn_ast_as(Primary, node)->literal);
                switch(literal->kind) {
                    case CN_AST_IDENTIFIER:
                        {
                            Cn_Binding_Idx idx = cn_binding_table_get(cn_ast_as(Identifier, literal)->name, &cn__tu_data->symbol_binding_table);

                            if (idx == CN_BINDING_NIL_IDX) {
                                
                                // Hook to detect and handle builtins as they appear.
                                Cn_String name = cn_ast_as(Identifier, literal)->name;
                                if (cn_str_starts_with(name, CN_STR_LIT("__builtin_"))) {
                                    idx = cn_binding_declare_builtin(name, literal);
                                    if (idx == CN_BINDING_NIL_IDX) goto error;
                                } else {
                                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, literal, CN_DC_INVALID_SYMBOL, "Primary expression identifier is not a known symbol.");
                                    goto error;
                                }
                            }

                            Cn_Binding *binding = cn__tu_data->binding_list + idx;
                            if (binding->kind == CN_BINDING_TAG || binding->kind == CN_BINDING_TYPEDEF) {
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, literal, CN_DC_INVALID_SYMBOL, "Primary expression identifier is expected to be a variable name, function name, or enum name.");
                                goto error;
                            }

                            result = binding->type;
                            break;
                        }
                    case CN_AST_INTEGER:
                        {   
                            result = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_INT);
                            break;
                        }
                    case CN_AST_FLOAT:
                        {   
                            result = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_FLOAT);
                            break;
                        }
                    case CN_AST_STRING:
                        {   
                            result  = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_CHAR);
                            Cn_Type const_char = cn_type_make_qualified(CN_TYPE_QUALIFIED_CONSTANT, result);
                            result = cn_analyze_intern_type(&const_char);
                            Cn_Type const_char_ptr = cn_type_make_pointer(result);
                            result = cn_analyze_intern_type(&const_char_ptr);
                            break;
                        }
                    case CN_AST_CHAR:
                        {   
                            result = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_CHAR);
                            break;
                        }
                    default:
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_EXPRESSION, "Expected primary expression to typecheck.");
                        goto error;
                }

                cn_ast_as(Primary, node)->type = result;
                break;
            }
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_EXPRESSION, "Expected expression to typecheck.");
            goto error;
    }

    return result;

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Any cn__analyze_evaluate_binary(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = cn_ast_as(Binary, node)->type;
    if (rtype == NULL) return (Cn_Any) {0};
 
    Cn_Ast_Binary_Operator_Kind op = cn_ast_as(Binary, node)->operator;
    Cn_Ast_Node *left  = cn_ast_as(Binary, node)->left;
    Cn_Ast_Node *right = cn_ast_as(Binary, node)->right;
 
    Cn_Any result = { 
        .type = rtype, 
        .data = buffer 
    };
 
    uint8_t lbuf[CN_TYPE_SCALAR_MAX_SIZE];
    uint8_t rbuf[CN_TYPE_SCALAR_MAX_SIZE];
 
    switch (op) {
        // a , b  ->  value of b. Side effects of a are irrelevant to the value.
        case CN_AST_BINARY_OP_COMMA:
            return cn_analyze_evaluate_expression(right, buffer);
 
        // This never resolve to a compile time scalar.
        case CN_AST_BINARY_OP_ARRAY_SUB:
            return (Cn_Any) {0};
 
        // Short circuit logical ops: foldable even if one side is unknown, as
        // long as the known side already determines the outcome.
        case CN_AST_BINARY_OP_AND: 
            {
                Cn_Any l = cn_analyze_evaluate_expression(left, lbuf);
                bool lv = !cn_any_is_zero(l);
                if (!cn_any_is_empty(l) && lv) { 
                    cn_any_write_int(result, 0); 
                    return result; 
                }

                Cn_Any r = cn_analyze_evaluate_expression(right, rbuf);
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
        case CN_AST_BINARY_OP_OR: 
            {
                Cn_Any l = cn_analyze_evaluate_expression(left, lbuf);
                bool lv = !cn_any_is_zero(l);
                if (!cn_any_is_empty(l) && lv) { 
                    cn_any_write_int(result, 1); 
                    return result; 
                }

                Cn_Any r = cn_analyze_evaluate_expression(right, rbuf);
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
    Cn_Any l = cn_analyze_evaluate_expression(left, lbuf);
    if (cn_any_is_empty(l)) return (Cn_Any) {0};

    Cn_Any r = cn_analyze_evaluate_expression(right, rbuf);
    if (cn_any_is_empty(r)) return (Cn_Any) {0};
 
    Cn_Type *lt = cn_type_unqualified(l.type);
    Cn_Type *rt = cn_type_unqualified(r.type);
 
    // Pointer arithmetic or pointer differences have no known address.
    if (lt->kind == CN_POINTER || rt->kind == CN_POINTER) return (Cn_Any) {0};
 
    bool float_operands = (lt->kind == CN_FLOAT) || (rt->kind == CN_FLOAT);
 
    // Relational and equality operators -> int.
    switch (op) {
        case CN_AST_BINARY_OP_LESS:
        case CN_AST_BINARY_OP_LESS_EQ:
        case CN_AST_BINARY_OP_GREATER:
        case CN_AST_BINARY_OP_GREATER_EQ:
        case CN_AST_BINARY_OP_EQ:
        case CN_AST_BINARY_OP_NOT_EQ: 
            {
                bool res = false;
                if (float_operands) {
                    double a = cn_any_as_double(l), b = cn_any_as_double(r);
                    switch (op) {
                        case CN_AST_BINARY_OP_LESS:       res = a <  b; break;
                        case CN_AST_BINARY_OP_LESS_EQ:    res = a <= b; break;
                        case CN_AST_BINARY_OP_GREATER:    res = a >  b; break;
                        case CN_AST_BINARY_OP_GREATER_EQ: res = a >= b; break;
                        case CN_AST_BINARY_OP_EQ:         res = a == b; break;
                        default:                      res = a != b; break;
                    }
                } else {
                    int64_t a = cn_any_read_int(l), b = cn_any_read_int(r);

                    Cn_Type *common = cn__analyze_usual_arithmetic_conversion(lt, rt);
                    // Case: unsigned.
                    if ((common->kind == CN_INTEGER) && !((Cn_Type_Integer *)common)->is_signed) {
                        uint64_t ua = (uint64_t)a, ub = (uint64_t)b;
                        switch (op) {
                            case CN_AST_BINARY_OP_LESS:       res = ua <  ub; break;
                            case CN_AST_BINARY_OP_LESS_EQ:    res = ua <= ub; break;
                            case CN_AST_BINARY_OP_GREATER:    res = ua >  ub; break;
                            case CN_AST_BINARY_OP_GREATER_EQ: res = ua >= ub; break;
                            case CN_AST_BINARY_OP_EQ:         res = ua == ub; break;
                            default:                      res = ua != ub; break;
                        }
                    } 
                    // Case: signed.
                    else {
                        switch (op) {
                            case CN_AST_BINARY_OP_LESS:       res = a <  b; break;
                            case CN_AST_BINARY_OP_LESS_EQ:    res = a <= b; break;
                            case CN_AST_BINARY_OP_GREATER:    res = a >  b; break;
                            case CN_AST_BINARY_OP_GREATER_EQ: res = a >= b; break;
                            case CN_AST_BINARY_OP_EQ:         res = a == b; break;
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
            case CN_AST_BINARY_OP_ADDITION:       res = a + b; break;
            case CN_AST_BINARY_OP_SUBTRACTION:    res = a - b; break;
            case CN_AST_BINARY_OP_MULTIPLICATION: res = a * b; break;
            case CN_AST_BINARY_OP_DIVISION:       res = a / b; break; // Inf or NaN are valid doubles.
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
            case CN_AST_BINARY_OP_ADDITION:       
                res = (int64_t)((uint64_t)a + (uint64_t)b); 
                break;

            case CN_AST_BINARY_OP_SUBTRACTION:    
                res = (int64_t)((uint64_t)a - (uint64_t)b); 
                break;

            case CN_AST_BINARY_OP_MULTIPLICATION: 
                res = (int64_t)((uint64_t)a * (uint64_t)b); 
                break;
 
            case CN_AST_BINARY_OP_DIVISION:
                {
                    if (b == 0) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_INVALID_CONSTANT_EXPRESSION, "Integer division '/' by 0.");
                        return (Cn_Any) {0};
                    }
                    if (is_signed) res = (a == INT64_MIN && b == -1) ? INT64_MIN : (a / b);
                    else           res = (int64_t)((uint64_t)a / (uint64_t)b);
                    break;
                }
 
            case CN_AST_BINARY_OP_MODULO:
                {
                    if (b == 0) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_INVALID_CONSTANT_EXPRESSION, "Integer modulo '\%' by 0.");
                        return (Cn_Any) {0};
                    }
                    if (is_signed) res = (a == INT64_MIN && b == -1) ? 0 : (a % b);
                    else           res = (int64_t)((uint64_t)a % (uint64_t)b);
                    break;
                }
 
            case CN_AST_BINARY_OP_BIT_AND: 
                res = a & b; 
                break;

            case CN_AST_BINARY_OP_BIT_OR:  
                res = a | b; 
                break;

            case CN_AST_BINARY_OP_BIT_XOR: 
                res = a ^ b; 
                break;
 
            case CN_AST_BINARY_OP_LSHIFT:
            case CN_AST_BINARY_OP_RSHIFT: 
                {
                    int64_t width = urtype->size * 8;

                    if (b < 0 || b >= width) return (Cn_Any) {0}; // Undefined behavior shift, not evaluatable.

                    if (op == CN_AST_BINARY_OP_LSHIFT)
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

CNDEF Cn_Any cn__analyze_evaluate_access(Cn_Ast_Node *node, void *buffer) {
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__analyze_evaluate_call(Cn_Ast_Node *node, void *buffer) {
    // Cannot really evaluate runtime function during compile time.
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__analyze_evaluate_unary(Cn_Ast_Node *node, void *buffer) {
    Cn_Ast_Unary_Operator_Kind op = cn_ast_as(Unary, node)->operator;
    Cn_Ast_Node *operand  = cn_ast_as(Unary, node)->expression;
 
    // Not constant operations.
    if (op == CN_AST_UNARY_OP_ADDROF || op == CN_AST_UNARY_OP_DEREF || op == CN_AST_UNARY_OP_INCREMENT || op == CN_AST_UNARY_OP_DECREMENT) return (Cn_Any) {0};

    // Special case, positive operator doesn't really do anything, 
    // so just delegating buffer and evaluation to operand expression.
    if (op == CN_AST_UNARY_OP_POSITIVE) {
        return cn_analyze_evaluate_expression(operand, buffer);
    }

    Cn_Type *rtype = cn_ast_as(Unary, node)->type;
    if (rtype == NULL) return (Cn_Any) {0};
 
    Cn_Any result = { 
        .type = rtype, 
        .data = buffer 
    };

    uint8_t operand_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any operand_value = cn_analyze_evaluate_expression(operand, operand_buffer);

    switch (op) {

        case CN_AST_UNARY_OP_NEGATIVE: 
            {
                if (cn_type_unqualified(rtype)->kind == CN_FLOAT) 
                    cn_any_write_float(result, -cn_any_read_float(operand_value));
                else
                    cn_any_write_int(result, -cn_any_read_int(operand_value));

                return result;
            }

        case CN_AST_UNARY_OP_BIT_NOT: 
            return cn_any_write_int(result, ~cn_any_read_int(operand_value));

        case CN_AST_UNARY_OP_NOT: 
            return cn_any_write_int(result, cn_any_is_zero(operand_value) ? 0 : 1);
        default:
            return (Cn_Any) {0};
    }
}

CNDEF Cn_Any cn__analyze_evaluate_compound(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *target = cn_ast_as(Cast, node)->type;
    if (target == NULL) return (Cn_Any) {0};
 
    Cn_Type *ut = cn_type_unqualified(target);

    switch (ut->kind) {
        case CN_INTEGER:
        case CN_FLOAT:
        case CN_BOOL:
        case CN_POINTER:
            CN_ASSERT(cn_ast_as(Compound, node)->designations.length == 1);
            Cn_Ast_Designation *designation = cn_ast_as(Designation, (cn_ast_as(Compound, node)->designations.ptrs[0]));
            Cn_Ast_Initializer *initializer = designation->initializer;
            CN_ASSERT(cn_ast_as(Initializer, node)->expression != NULL);
            return cn_analyze_evaluate_expression(initializer->expression, buffer);
        
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

CNDEF Cn_Any cn__analyze_evaluate_cast(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *target = cn_ast_as(Cast, node)->type;
    if (target == NULL) return (Cn_Any) {0};
 
    Cn_Type *ut = cn_type_unqualified(target);

    // Cast to void yields no value; cast to pointer has no known address.
    if (ut->kind == CN_VOID || ut->kind == CN_POINTER) return (Cn_Any) {0};
 
    uint8_t operand_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any operand_value = cn_analyze_evaluate_expression(cn_ast_as(Cast, node)->expression, operand_buffer);
    if (operand_value.type == NULL) return (Cn_Any) {0};
 
    return cn_any_convert(operand_value, target, buffer);
}

CNDEF Cn_Any cn__analyze_evaluate_sizeof(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = cn_ast_as(Sizeof, node)->type; // size_t
    if (rtype == NULL) return (Cn_Any) {0};

    Cn_Ast_Node *child = (cn_ast_as(Sizeof, node)->target);

    Cn_Type *operand_type;
    if (child->kind == CN_AST_TYPE_NAME) {
        Cn_Ast_Specifier_Qualifier *sq = (Cn_Ast_Specifier_Qualifier *)cn_ast_upcast((cn_ast_as(Type_Name, child)->specifier_qualifier));
        operand_type    = cn_analyze_to_type(cn_ast_as(Specifier_Qualifier, sq)->qualifiers, cn_ast_as(Specifier_Qualifier, sq)->type_specifier, cn_ast_as(Type_Name, child)->abstract_declarator, 0);
    } else {
        // The operand of sizeof is unevaluated, only its type is needed.
        operand_type = cn_get_expression_type(cn_ast_as(Sizeof, node)->target);
    }

    if (operand_type == NULL) return (Cn_Any) {0};

    operand_type = cn_type_unqualified(operand_type);

    // AI: Generated.
    // Size of opaque is unknown, so it is not a constant.
    if (operand_type->kind == CN_OPAQUE) return (Cn_Any) {0};

    if (!(operand_type->flags & CN_TYPE_COMPLETE) || operand_type->size < 0) return (Cn_Any) {0};      

    Cn_Any result = { 
        .type = rtype, 
        .data = buffer 
    };

    cn_any_write_int(result, operand_type->size);
    return result;
}

CNDEF Cn_Any cn__analyze_evaluate_ternary(Cn_Ast_Node *node, void *buffer) {
    Cn_Type *rtype = cn_ast_as(Ternary, node)->type;
    if (rtype == NULL) return (Cn_Any) {0};

    Cn_Type *urtype = cn_type_unqualified(rtype);

    // Only scalar conditionals evaluated.
    if (!cn_type_is_scalar(urtype) || urtype->kind == CN_POINTER) return (Cn_Any) {0};

    uint8_t cond_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any cond = cn_analyze_evaluate_expression(cn_ast_as(Ternary, node)->condition, cond_buffer);
    if (cond.type == NULL) return (Cn_Any) {0};

    Cn_Ast_Node *chosen = cn_any_is_zero(cond) ? cn_ast_as(Ternary, node)->if_false : cn_ast_as(Ternary, node)->if_true;

    uint8_t value_buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any value = cn_analyze_evaluate_expression(chosen, value_buffer);
    if (value.type == NULL) return (Cn_Any) {0};

    // Convert the chosen branch to the conditional's common result type.
    return cn_any_convert(value, rtype, buffer);
}

CNDEF Cn_Any cn__analyze_evaluate_assignment(Cn_Ast_Node *node, void *buffer) {
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__analyze_evaluate_postfix(Cn_Ast_Node *node, void *buffer) {
    CN_UNUSED(node);
    CN_UNUSED(buffer);
    return (Cn_Any) {0};
}

CNDEF Cn_Any cn__analyze_evaluate_primary(Cn_Ast_Node *node, void *buffer) {
    Cn_Any any = {0};
    any.data = buffer;
    any.type = cn_ast_as(Primary, node)->type;

    Cn_Ast_Node *literal = (cn_ast_as(Primary, node)->literal);
    switch (literal->kind) {
        case CN_AST_INTEGER:
            {
                any = cn_any_write_int(any, cn_ast_as(Integer, literal)->value);
                break;
            }
        case CN_AST_FLOAT:
            {
                any = cn_any_write_float(any, cn_ast_as(Float, literal)->value);
                break;
            }
        case CN_AST_CHAR:
            {
                any = cn_any_write_int(any, cn_ast_as(Char, literal)->value);
                break;
            }
        case CN_AST_STRING:
            {
                return (Cn_Any) {0};
            }
        case CN_AST_IDENTIFIER:
            {   
                Cn_Binding_Idx idx = cn_binding_table_get(cn_ast_as(Identifier, literal)->name, &cn__tu_data->symbol_binding_table);
                if (idx == CN_BINDING_NIL_IDX) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, literal, CN_DC_INVALID_SYMBOL, "Primary expression identifier is not a known symbol.");
                    return (Cn_Any) {0};
                }

                Cn_Binding *binding = cn_binding_get(idx);
                if (binding->kind != CN_BINDING_ENUM_CONSTANT) {
                    return (Cn_Any) {0};
                }

                any = cn_any_write_int(any, binding->b_enum_constant.value);
                break;
            }
        default:
            {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_TOKEN, "Expected identifier, float, integer, char, or string literal in primary expression.");
                return (Cn_Any) {0};
            }
    }

    return any;
}

CNDEF Cn_Any cn_analyze_evaluate_expression(void *expression, void *buffer) {
    if (expression == NULL) return (Cn_Any) {0};

    Cn_Ast_Node *node = expression;

    switch(node->kind) {
        case CN_AST_BINARY: 
            {   
                return cn__analyze_evaluate_binary(node, buffer);
            }
        case CN_AST_ACCESS:
            {
                return cn__analyze_evaluate_access(node, buffer);
            }
        case CN_AST_CALL:
            {
                return cn__analyze_evaluate_call(node, buffer);
            }
        case CN_AST_UNARY:
            {
                return cn__analyze_evaluate_unary(node, buffer);
            }
        case CN_AST_CAST:
            {
                return cn__analyze_evaluate_cast(node, buffer);
            }
        case CN_AST_COMPOUND:
            {
                return cn__analyze_evaluate_compound(node, buffer);
            }
        case CN_AST_SIZEOF:
            {
                return cn__analyze_evaluate_sizeof(node, buffer);
            }
        case CN_AST_TERNARY:
            {   
                return cn__analyze_evaluate_ternary(node, buffer);
            }
        case CN_AST_ASSIGN:
            {
                return cn__analyze_evaluate_assignment(node, buffer);
            }
        case CN_AST_POSTFIX:
            {
                return cn__analyze_evaluate_postfix(node, buffer);
            }
        case CN_AST_PRIMARY:
            {
                return cn__analyze_evaluate_primary(node, buffer);
            }
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, node, CN_DC_EXPECTED_EXPRESSION, "Expected expression to evaluate.");
            break;
    }
    
    return (Cn_Any) {0};
}

/**
 * Helper function that typecheck designator, it returns type that designator access.
 *
 * RETURNS: NULL if error occured.
 */
CNDEF Cn_Type *cn__analyze_typecheck_designator(Cn_Ast_Designator *designator, Cn_Type *type, int64_t *ordinal) {
    
    type = cn_type_unqualified(type);
    // Shouldn't really happen, but still worth to check, if user decides to call this function.
    if (!(type->flags & CN_TYPE_COMPLETE)) {
        Cn_String type_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type, (Cn_String) {0});
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "Designator on incomplete type %.*s.", CN_STR_UNPACK(type_str));
        goto error;
    }

    switch (type->kind) {
        case CN_ARRAY: 
            {
                if (designator->identifier != NULL) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "Identifier designator used on array type.");
                    goto error;
                }

                
                Cn_Type *expression_type = cn_analyze_typecheck_expression(designator->expression);
                if (expression_type == NULL) goto error;
                
                uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
                Cn_Any any = cn_analyze_evaluate_expression(designator->expression, buffer);

                if (cn_any_is_empty(any)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in array designator.");
                    goto error;
                }

                if (any.type->kind != CN_INTEGER) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array designator.");
                    goto error;
                }

                int64_t index = cn_any_read_int(any);

                if (index >= type->array.length) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Out of bound access from constant expression in array designator, at index %ld, for array length %ld.", index, type->array.length);
                    goto error;
                }
                
                if (ordinal != NULL) *ordinal = index;
                
                if (designator->expression_range_end != NULL) {
                    expression_type = cn_analyze_typecheck_expression(designator->expression_range_end);
                    if (expression_type == NULL) goto error;

                    any = cn_analyze_evaluate_expression(designator->expression_range_end, buffer);

                    if (cn_any_is_empty(any)) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid constant expression in array designator.");
                        goto error;
                    }

                    if (any.type->kind != CN_INTEGER) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end, CN_DC_INVALID_CONSTANT_EXPRESSION, "Non integer result from constant expression in array designator.");
                        goto error;
                    }

                    int64_t end_index = cn_any_read_int(any);

                    if (end_index >= type->array.length) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end, CN_DC_INVALID_CONSTANT_EXPRESSION, "Out of bound access from constant expression in array designator, at index %ld, for array length %ld.", end_index, type->array.length);
                        goto error;
                    }

                    if (end_index < index) {
                        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator->expression_range_end, CN_DC_INVALID_CONSTANT_EXPRESSION, "Invalid range in array designator, end index %ld is less then start index %ld.", end_index, index);
                        goto error;
                    }

                    if (ordinal != NULL) *ordinal = end_index;
                }
                
                return type->array.element_type;
            }
        case CN_STRUCT: 
            {
                if (designator->expression != NULL) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "Array designator used on struct type.");
                    goto error;
                }

                Cn_Ast_Identifier *identifier = designator->identifier;
                for (int64_t i = 0; i < type->struct_t.members_length; i++) {
                    if (cn_str_equals(&identifier->name, &type->struct_t.members[i].name)) {
                        if (ordinal != NULL) *ordinal = i;
                        return type->struct_t.members[i].type;
                    }
                }

                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "No member %.*s in struct specified by identifier designator.", CN_STR_UNPACK(identifier->name));
                goto error;
            }
        case CN_UNION: 
            {
                if (designator->expression != NULL) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "Array designator used on union type.");
                    goto error;
                }

                Cn_Ast_Identifier *identifier = designator->identifier;
                for (int64_t i = 0; i < type->union_t.members_length; i++) {
                    if (cn_str_equals(&identifier->name, &type->union_t.members[i].name)) {
                        if (ordinal != NULL) *ordinal = i;
                        return type->union_t.members[i].type;
                    }
                }

                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "No member %.*s in union specified by identifier designator.", CN_STR_UNPACK(identifier->name));
                goto error;
            }
        // AI: Generated.
        // Opaque is trusted to be designated by any designator, designated type is unknown.
        // Array designator expressions are still typechecked, but not evaluated, since bounds are unknown.
        case CN_OPAQUE:
            {
                if (designator->expression != NULL && cn_analyze_typecheck_expression(designator->expression) == NULL) goto error;
                if (designator->expression_range_end != NULL && cn_analyze_typecheck_expression(designator->expression_range_end) == NULL) goto error;

                return type;
            }
        default:
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designator, CN_DC_ILLEGAL_TYPE, "Designator used on non-compound type.");
            goto error;
    }

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF bool cn__analyze_typecheck_designation(Cn_Ast_Designation *designation, Cn_Type *type, int64_t *ordinal) {

    if (designation->designators.length != 0) {
        type = cn__analyze_typecheck_designator(cn_ast_as(Designator, designation->designators.ptrs[0]), type, ordinal);
        // AI: Generated.
        if (type == NULL) goto error;

        for (int64_t i = 1; i < designation->designators.length; i++) {
            type = cn__analyze_typecheck_designator(cn_ast_as(Designator, designation->designators.ptrs[i]), type, NULL);
            // AI: Generated.
            if (type == NULL) goto error;
        }

    } else {
        type = cn_type_unqualified(type);

        // Shouldn't really happen, but still worth to check, if user decides to call this function.
        if (!(type->flags & CN_TYPE_COMPLETE)) {
            Cn_String type_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type, (Cn_String) {0});
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designation, CN_DC_ILLEGAL_TYPE, "Designation on incomplete type %.*s.", CN_STR_UNPACK(type_str));
            goto error;
        }
        
        switch (type->kind) {
            case CN_ARRAY: 
                type = type->array.element_type;
                break;
            case CN_STRUCT:
                if (*ordinal >= type->struct_t.members_length) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designation, CN_DC_ILLEGAL_TYPE, "Out of bounds designation access on struct.");
                    goto error;
                } 
                type = type->struct_t.members[*ordinal].type;
                break;

            case CN_UNION: 
                if (*ordinal >= type->union_t.members_length) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, designation, CN_DC_ILLEGAL_TYPE, "Out of bounds designation access on union.");
                    goto error;
                } 
                type = type->union_t.members[*ordinal].type;
                break;
            default: // Default case is just other complete types.
                break;
        }
    }

    return cn_analyze_typecheck_initializer(designation->initializer, type);

error:
    CN__TRACE_ERROR
    return false;
}

CNDEF bool cn_analyze_typecheck_designations(Cn_Ast_Nodes designations, Cn_Type *type) {
    int64_t ordinal = 0;
    for (int64_t i = 0; i < designations.length; i++) {
        if (!cn__analyze_typecheck_designation(cn_ast_as(Designation, designations.ptrs[i]), type, &ordinal)) goto error;
        ordinal++;
    }

    return true;

error:
    CN__TRACE_ERROR
    return false;
}

CNDEF bool cn_analyze_typecheck_initializer(void *initializer_node, Cn_Type *type) {
    Cn_Ast_Initializer *initializer = initializer_node;

    type = cn_type_unqualified(type);

    if (initializer->expression != NULL) {
        Cn_Type *expression_type = cn_analyze_typecheck_expression(initializer->expression);
        if (expression_type == NULL) goto error;

        // Simple assignment check, same as in assignment expression typechecking.
        if (!cn_type_is_assignable(type, expression_type)) {

            if (!(type->kind == CN_POINTER && cn__analyze_is_null_pointer_constant(initializer->expression))) {
                Cn_String left_str  = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), type, (Cn_String) {0});
                Cn_String right_str = cn_type_stringify(CN_STR_BUFFER_EMPTY(128), expression_type, (Cn_String) {0});
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, initializer->expression, CN_DC_ILLEGAL_TYPE, "Cannot assign %.*s to %.*s in initializer.", CN_STR_UNPACK(right_str), CN_STR_UNPACK(left_str));
                goto error;
            }
        }

        return true;
    }
        
    // Compound initializer case:
    return cn_analyze_typecheck_designations(initializer->designations, type);

error:
    CN__TRACE_ERROR
    return false;
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
 * Helper to format a string.
 */
CNDEF void cn__emit_vformat(Cn_Emitter *e, const char *fmt, va_list args) {
    char stack_buf[256];

    va_list args_copy;
    va_copy(args_copy, args);
    int n = vsnprintf(stack_buf, sizeof stack_buf, fmt, args_copy);
    va_end(args_copy);

    if (n < 0) {
        cn_log(CN_ERROR, "Emitted format string hit formatting error.");
        return; // Formatting error.
    }

    if ((size_t)n < sizeof stack_buf) {
        cn__emit_str(e, (Cn_String){ .data = stack_buf, .length = (size_t)n });
        return;
    }

    cn_log(CN_ERROR, "Emitted format string exceeds 256 bytes. Why? Don't do it.");
}

CNDEF void cn__emit_format(Cn_Emitter *e, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    cn__emit_vformat(e, fmt, args);
    va_end(args);
}

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

CNDEF void cn__emit_code(Cn_Emitter *e, Cn_Ast_Code *node) {
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
CNDEF void cn__emit_storage_specifiers(Cn_Emitter *e, Cn_Ast_Storage_Specifier_Flags storage);
CNDEF void cn__emit_qualifiers(Cn_Emitter *e, Cn_Ast_Qualifier_Flags qualifiers);
CNDEF void cn__emit_function_specifiers(Cn_Emitter *e, Cn_Ast_Function_Specifier_Flags func_spec);
CNDEF void cn__emit_attribute_specifiers(Cn_Emitter *e, Cn_Ast_Nodes specifiers);
#ifdef CN_GNU
CNDEF void cn__emit_attribute_specifiers_suffix(Cn_Emitter *e, Cn_Ast_Nodes specifiers);
#endif
CNDEF void cn__emit_designations(Cn_Emitter *e, Cn_Ast_Nodes designations);

CNDEF void cn__emit_translation_unit(Cn_Emitter *e, Cn_Ast_Translation_Unit *node) {
    for (int64_t i = 0; i < node->external_declarations.length; i++) {
        cn_emit(e, node->external_declarations.ptrs[i]);
    }
}

CNDEF void cn__emit_external_declaration(Cn_Emitter *e, Cn_Ast_External_Declaration *node) {
    if (node->child == NULL) {
        cn__emit_str_lit(e, ";");
        cn__emit_newline(e);
    } else {
        if((node->child)->kind == CN_AST_DECLARATION)
            e->flags |= CN_EMITTER_END_DECLARATION_WITH_SEMICOLON;

        cn_emit(e, node->child);
    }
}

CNDEF void cn__emit_declaration(Cn_Emitter *e, Cn_Ast_Declaration *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn_emit(e, node->declaration_specifiers);

    for (int64_t i = 0; i < node->init_declarators.length; i++) {
        if (i == 0) cn__emit_str_lit(e, " ");
        else cn__emit_str_lit(e, ", ");

        cn_emit(e, node->init_declarators.ptrs[i]);
    }

    if (e->flags & CN_EMITTER_END_DECLARATION_WITH_SEMICOLON) {
        cn__emit_str_lit(e, ";");
        cn__emit_newline(e);
        e->flags &= ~CN_EMITTER_END_DECLARATION_WITH_SEMICOLON;
    }
}

CNDEF void cn__emit_function(Cn_Emitter *e, Cn_Ast_Function *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
    cn_emit(e, node->declaration_specifiers);
    cn__emit_str_lit(e, " ");
    cn_emit(e, node->declarator);
    cn__emit_str_lit(e, " ");
    cn_emit(e, node->block);
}

CNDEF void cn__emit_block(Cn_Emitter *e, Cn_Ast_Block *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

cn__emit_str_lit(e, "{");

    e->indent++;
    cn__emit_newline(e);

    for (int64_t i = 0; i < node->block_items.length; i++) {
        cn_emit(e, node->block_items.ptrs[i]);
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

CNDEF void cn__emit_block_item(Cn_Emitter *e, Cn_Ast_Block_Item *node) {
if ((node->declaration_or_statement)->kind == CN_AST_DECLARATION)
        e->flags |= CN_EMITTER_END_DECLARATION_WITH_SEMICOLON;
    cn_emit(e, node->declaration_or_statement);
}

CNDEF void cn__emit_if_statement(Cn_Emitter *e, Cn_Ast_If *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn__emit_str_lit(e, "if (");
    cn_emit(e, node->condition);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *then_stmt = node->then_statement;
    if (then_stmt->kind == CN_AST_BLOCK) {
        e->flags |= CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK;
        cn_emit(e, node->then_statement);

        if (node->else_statement != NULL) {
            cn__emit_str_lit(e, " ");
        } else {
            cn__emit_newline(e);
        }
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->then_statement);
    }

    if (node->else_statement != NULL) {
        cn__emit_str_lit(e, "else ");
        Cn_Ast_Node *else_stmt = node->else_statement;
        if (else_stmt->kind == CN_AST_BLOCK || else_stmt->kind == CN_AST_IF) {
            cn_emit(e, node->else_statement);
        } else {
            e->indent++;
            cn__emit_newline(e);
            e->indent--;

            cn_emit(e, node->else_statement);
        }
    }
}

CNDEF void cn__emit_switch_statement(Cn_Emitter *e, Cn_Ast_Switch *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn__emit_str_lit(e, "switch (");
    cn_emit(e, node->condition);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *body = node->body;
    if (body->kind == CN_AST_BLOCK) {
        cn_emit(e, node->body);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->body);
    }
}

CNDEF void cn__emit_while_statement(Cn_Emitter *e, Cn_Ast_While *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn__emit_str_lit(e, "while (");
    cn_emit(e, node->condition);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *body = node->body;
    if (body->kind == CN_AST_BLOCK) {
        cn_emit(e, node->body);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->body);
    }
}

CNDEF void cn__emit_do_while(Cn_Emitter *e, Cn_Ast_Do_While *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn__emit_str_lit(e, "do ");

    Cn_Ast_Node *body = node->body;
    if (body->kind == CN_AST_BLOCK) {
        // "while" part has to stay on the same line as the closing brace.
        e->flags |= CN_EMITTER_SKIP_ENDING_NEWLINE_IN_BLOCK;
        cn_emit(e, node->body);

        cn__emit_str_lit(e, " ");
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        // Body ends its own line, so "while" part starts already indented.
        cn_emit(e, node->body);
    }

    cn__emit_str_lit(e, "while (");
    cn_emit(e, node->condition);
    cn__emit_str_lit(e, ");");
    cn__emit_newline(e);
}

CNDEF void cn__emit_for_statement(Cn_Emitter *e, Cn_Ast_For *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn__emit_str_lit(e, "for (");

// Initialization can be a declaration, but its semicolon is emitted here
    // as a part of the for clause, so the declaration flag is left unset.
    cn_emit(e, node->initialization);
    cn__emit_str_lit(e, ";");

    if (node->condition != NULL)
        cn__emit_str_lit(e, " ");

    cn_emit(e, node->condition);
    cn__emit_str_lit(e, ";");

    if (node->update != NULL)
        cn__emit_str_lit(e, " ");

    cn_emit(e, node->update);
    cn__emit_str_lit(e, ") ");

    Cn_Ast_Node *body = node->body;
    if (body->kind == CN_AST_BLOCK) {
        cn_emit(e, node->body);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->body);
    }
}

CNDEF void cn__emit_label(Cn_Emitter *e, Cn_Ast_Label *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

if (node->flags & CN_AST_LABEL_IS_CASE) {
        cn__emit_str_lit(e, "case ");
        cn_emit(e, node->expression);
    }
    else if (node->flags & CN_AST_LABEL_IS_DEFAULT) {
        cn__emit_str_lit(e, "default");
    } else {
        cn_emit(e, node->identifier);
    }

    cn__emit_str_lit(e, ": ");

    if ((node->statement)->kind == CN_AST_BLOCK) {
        cn_emit(e, node->statement);
    } else {
        e->indent++;
        cn__emit_newline(e);
        e->indent--;

        cn_emit(e, node->statement);
    }
}

CNDEF void cn__emit_goto_statement(Cn_Emitter *e, Cn_Ast_Goto *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

cn__emit_str_lit(e, "goto ");
    cn_emit(e, node->identifier);
    cn__emit_str_lit(e, ";");
    cn__emit_newline(e);
}

CNDEF void cn__emit_return_statement(Cn_Emitter *e, Cn_Ast_Return *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

cn__emit_str_lit(e, "return");
    if (node->expression != NULL) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->expression);
    }
    cn__emit_str_lit(e, ";");
    cn__emit_newline(e);
}

CNDEF void cn__emit_break_statement(Cn_Emitter *e, Cn_Ast_Break *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

cn__emit_str_lit(e, "break;");
    cn__emit_newline(e);
}

CNDEF void cn__emit_continue_statement(Cn_Emitter *e, Cn_Ast_Continue *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

cn__emit_str_lit(e, "continue;");
    cn__emit_newline(e);
}

CNDEF void cn__emit_expression_statement(Cn_Emitter *e, Cn_Ast_Expression_Statement *node) {
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

cn_emit(e, node->expression);
    cn__emit_str_lit(e, ";");
    cn__emit_newline(e);
}

CNDEF void cn__emit_unary(Cn_Emitter *e, Cn_Ast_Unary *node) {
    Cn_String op = {0};
    switch (node->operator) {
        case CN_AST_UNARY_OP_INCREMENT: op = CN_STR_LIT("++"); break;
        case CN_AST_UNARY_OP_DECREMENT: op = CN_STR_LIT("--"); break;
        case CN_AST_UNARY_OP_POSITIVE:  op = CN_STR_LIT("+"); break;
        case CN_AST_UNARY_OP_NEGATIVE:  op = CN_STR_LIT("-"); break;
        case CN_AST_UNARY_OP_NOT:       op = CN_STR_LIT("!"); break;
        case CN_AST_UNARY_OP_BIT_NOT:   op = CN_STR_LIT("~"); break;
        case CN_AST_UNARY_OP_DEREF:     op = CN_STR_LIT("*"); break;
        case CN_AST_UNARY_OP_ADDROF:    op = CN_STR_LIT("&"); break;
        default: break;
    }
    cn__emit_str(e, op);
    cn_emit(e, node->expression);
}

CNDEF void cn__emit_postfix(Cn_Emitter *e, Cn_Ast_Postfix *node) {
    cn_emit(e, node->expression);

    switch (node->operator) {
        case CN_AST_POSTFIX_OP_INCREMENT: cn__emit_str_lit(e, "++"); break;
        case CN_AST_POSTFIX_OP_DECREMENT: cn__emit_str_lit(e, "--"); break;
        default: break;
    }
}

CNDEF void cn__emit_binary(Cn_Emitter *e, Cn_Ast_Binary *node) {
    // Special case for array subscript.
    if (node->operator == CN_AST_BINARY_OP_ARRAY_SUB) {
        cn__emit_str_lit(e, "(");
        cn_emit(e, node->left);
        cn__emit_str_lit(e, "[");
        cn_emit(e, node->right);
        cn__emit_str_lit(e, "])");
        return;
    }

    Cn_String op = {0};
    switch (node->operator) {
        case CN_AST_BINARY_OP_MULTIPLICATION: op = CN_STR_LIT(" * "); break;
        case CN_AST_BINARY_OP_DIVISION:       op = CN_STR_LIT(" / "); break;
        case CN_AST_BINARY_OP_MODULO:         op = CN_STR_LIT(" % "); break;
        case CN_AST_BINARY_OP_ADDITION:       op = CN_STR_LIT(" + "); break;
        case CN_AST_BINARY_OP_SUBTRACTION:    op = CN_STR_LIT(" - "); break;
        case CN_AST_BINARY_OP_LSHIFT:         op = CN_STR_LIT(" << "); break;
        case CN_AST_BINARY_OP_RSHIFT:         op = CN_STR_LIT(" >> "); break;
        case CN_AST_BINARY_OP_LESS:           op = CN_STR_LIT(" < "); break;
        case CN_AST_BINARY_OP_LESS_EQ:        op = CN_STR_LIT(" <= "); break;
        case CN_AST_BINARY_OP_GREATER:        op = CN_STR_LIT(" > "); break;
        case CN_AST_BINARY_OP_GREATER_EQ:     op = CN_STR_LIT(" >= "); break;
        case CN_AST_BINARY_OP_EQ:             op = CN_STR_LIT(" == "); break;
        case CN_AST_BINARY_OP_NOT_EQ:         op = CN_STR_LIT(" != "); break;
        case CN_AST_BINARY_OP_BIT_AND:        op = CN_STR_LIT(" & "); break;
        case CN_AST_BINARY_OP_BIT_XOR:        op = CN_STR_LIT(" ^ "); break;
        case CN_AST_BINARY_OP_BIT_OR:         op = CN_STR_LIT(" | "); break;
        case CN_AST_BINARY_OP_AND:            op = CN_STR_LIT(" && "); break;
        case CN_AST_BINARY_OP_OR:             op = CN_STR_LIT(" || "); break;
        case CN_AST_BINARY_OP_COMMA:          op = CN_STR_LIT(", "); break;
        default: break;
    }

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->left);
    cn__emit_str(e, op);
    cn_emit(e, node->right);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_access(Cn_Emitter *e, Cn_Ast_Access *node) {
    cn_emit(e, node->expression);

    if (node->flags & CN_AST_ACCESS_IS_POINTER) {
        cn__emit_str_lit(e, "->");
    } else {
        cn__emit_str_lit(e, ".");
    }

    cn_emit(e, node->member);
}

CNDEF void cn__emit_call(Cn_Emitter *e, Cn_Ast_Call *node) {
    cn_emit(e, node->expression);

    cn__emit_str_lit(e, "(");
    for (int64_t i = 0; i < node->arguments.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->arguments.ptrs[i]);
    }
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_cast(Cn_Emitter *e, Cn_Ast_Cast *node) {
    cn__emit_str_lit(e, "((");
    cn_emit(e, node->type_name);
    cn__emit_str_lit(e, ")");
    cn_emit(e, node->expression);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_compound(Cn_Emitter *e, Cn_Ast_Compound *node) {
    cn__emit_str_lit(e, "(");
    cn_emit(e, node->type_name);
    cn__emit_str_lit(e, ") ");

    cn__emit_str_lit(e, "{ ");
    cn__emit_designations(e, node->designations);
    cn__emit_str_lit(e, "}");
}

CNDEF void cn__emit_sizeof_expression(Cn_Emitter *e, Cn_Ast_Sizeof *node) {
    cn__emit_str_lit(e, "sizeof(");
    cn_emit(e, node->target);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_ternary(Cn_Emitter *e, Cn_Ast_Ternary *node) {
    cn__emit_str_lit(e, "(");
    cn_emit(e, node->condition);
    cn__emit_str_lit(e, " ? ");
    cn_emit(e, node->if_true);
    cn__emit_str_lit(e, " : ");
    cn_emit(e, node->if_false);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_assign(Cn_Emitter *e, Cn_Ast_Assign *node) {
    Cn_String op = {0};
    switch (node->operator) {
        case CN_AST_ASSIGNMENT_OP_ASSIGN:   op = CN_STR_LIT(" = "); break;
        case CN_AST_ASSIGNMENT_OP_MULTIPLY: op = CN_STR_LIT(" *= "); break;
        case CN_AST_ASSIGNMENT_OP_DIVIDE:   op = CN_STR_LIT(" /= "); break;
        case CN_AST_ASSIGNMENT_OP_MODULO:   op = CN_STR_LIT(" %= "); break;
        case CN_AST_ASSIGNMENT_OP_PLUS:     op = CN_STR_LIT(" += "); break;
        case CN_AST_ASSIGNMENT_OP_MINUS:    op = CN_STR_LIT(" -= "); break;
        case CN_AST_ASSIGNMENT_OP_LSHIFT:   op = CN_STR_LIT(" <<= "); break;
        case CN_AST_ASSIGNMENT_OP_RSHIFT:   op = CN_STR_LIT(" >>= "); break;
        case CN_AST_ASSIGNMENT_OP_BIT_AND:  op = CN_STR_LIT(" &= "); break;
        case CN_AST_ASSIGNMENT_OP_BIT_XOR:  op = CN_STR_LIT(" ^= "); break;
        case CN_AST_ASSIGNMENT_OP_BIT_OR:   op = CN_STR_LIT(" |= "); break;
        default: break;
    }

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->left);
    cn__emit_str(e, op);
    cn_emit(e, node->right);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_primary(Cn_Emitter *e, Cn_Ast_Primary *node) {
cn_emit(e, node->literal);
}

CNDEF void cn__emit_initializer(Cn_Emitter *e, Cn_Ast_Initializer *node) {
    if (node->expression != NULL) {
        cn_emit(e, node->expression);
        return;
    }

    cn__emit_str_lit(e, "{ ");
    cn__emit_designations(e, node->designations);
    cn__emit_str_lit(e, "}");
}

CNDEF void cn__emit_designations(Cn_Emitter *e, Cn_Ast_Nodes designations) {
    for (int64_t i = 0; i < designations.length; i++) {
        cn_emit(e, designations.ptrs[i]);

        if (i < designations.length - 1) {
            cn__emit_str_lit(e, ", ");
        } else {
            cn__emit_str_lit(e, " ");
        }
    }
}

CNDEF void cn__emit_designation(Cn_Emitter *e, Cn_Ast_Designation *node) {
    for (int64_t i = 0; i < node->designators.length; i++) {
        cn_emit(e, node->designators.ptrs[i]);

        if (i == node->designators.length - 1) {
            cn__emit_str_lit(e, " = ");
        }
    }

    cn_emit(e, node->initializer);
}

CNDEF void cn__emit_designator(Cn_Emitter *e, Cn_Ast_Designator *node) {
    if (node->identifier != NULL) {
        cn__emit_str_lit(e, ".");
        cn_emit(e, node->identifier);
        return;
    }

    cn__emit_str_lit(e, "[");

    cn_emit(e, node->expression);

    if (node->expression_range_end != NULL) {
        cn__emit_str_lit(e, " ... ");
        cn_emit(e, node->expression_range_end);
    }

    cn__emit_str_lit(e, "]");
}

CNDEF void cn__emit_declarator(Cn_Emitter *e, Cn_Ast_Declarator *node) {
    cn_emit(e, node->pointer);
    cn_emit(e, node->direct_declarator);
}

CNDEF void cn__emit_pointer(Cn_Emitter *e, Cn_Ast_Pointer *node) {
    cn__emit_str_lit(e, "*");
    cn__emit_qualifiers(e, node->qualifiers);

#ifdef CN_GNU
    // Attributes are parsed interleaved with qualifiers, which can be freely reordered, so they go after them.
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
#endif

    cn_emit(e, node->pointer);
}

CNDEF void cn__emit_direct_declarator_grouped(Cn_Emitter *e, Cn_Ast_Direct_Declarator_Grouped *node) {
    cn__emit_str_lit(e, "(");
    cn_emit(e, node->declarator);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_direct_declarator_array(Cn_Emitter *e, Cn_Ast_Direct_Declarator_Array *node) {
    cn_emit(e, node->direct_declarator);
    cn__emit_str_lit(e, "[");
    cn_emit(e, node->expression);
    cn__emit_str_lit(e, "]");
}

CNDEF void cn__emit_direct_declarator_function(Cn_Emitter *e, Cn_Ast_Direct_Declarator_Function *node) {
    cn_emit(e, node->direct_declarator);
    cn__emit_str_lit(e, "(");
    for (int64_t i = 0; i < node->parameter_declarations.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->parameter_declarations.ptrs[i]);
    }
    if (node->flags & CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC) {
        if (node->parameter_declarations.length > 0) cn__emit_str_lit(e, ", ");
        cn__emit_str_lit(e, "...");
    }
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_declaration_specifiers(Cn_Emitter *e, Cn_Ast_Declaration_Specifiers *node) {
    cn__emit_storage_specifiers(e, node->storage_specifiers);
    cn__emit_function_specifiers(e, node->function_specifiers);
    cn__emit_qualifiers(e, node->qualifiers);

#ifdef CN_GNU
    // Attributes are parsed interleaved with other specifiers, which can be freely reordered,
    // so they go right before the type specifier.
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
#endif

    cn_emit(e, node->type_specifier);
}

CNDEF void cn__emit_storage_specifiers(Cn_Emitter *e, Cn_Ast_Storage_Specifier_Flags storage) {
    if (storage & CN_AST_STORAGE_SPECIFIER_TYPEDEF)  cn__emit_str_lit(e, "typedef ");
    if (storage & CN_AST_STORAGE_SPECIFIER_EXTERN)   cn__emit_str_lit(e, "extern ");
    if (storage & CN_AST_STORAGE_SPECIFIER_STATIC)   cn__emit_str_lit(e, "static ");
    if (storage & CN_AST_STORAGE_SPECIFIER_AUTO)     cn__emit_str_lit(e, "auto ");
    if (storage & CN_AST_STORAGE_SPECIFIER_REGISTER) cn__emit_str_lit(e, "register ");
}

CNDEF void cn__emit_qualifiers(Cn_Emitter *e, Cn_Ast_Qualifier_Flags qualifiers) {
    if (qualifiers & CN_AST_TYPE_QUALIFIER_CONST)    cn__emit_str_lit(e, "const ");
    if (qualifiers & CN_AST_TYPE_QUALIFIER_RESTRICT) cn__emit_str_lit(e, "restrict ");
    if (qualifiers & CN_AST_TYPE_QUALIFIER_VOLATILE) cn__emit_str_lit(e, "volatile ");
    // if (qualifiers & CN_AST_TYPE_QUALIFIER_ATOMIC)   cn__emit_str_lit(e, "_Atomic ");
}

CNDEF void cn__emit_function_specifiers(Cn_Emitter *e, Cn_Ast_Function_Specifier_Flags func_spec) {
    if (func_spec & CN_AST_FUNCTION_SPECIFIER_INLINE)   cn__emit_str_lit(e, "inline ");
    // if (func_spec & CN_AST_FUNCTION_SPECIFIER_NORETURN) cn__emit_str_lit(e, "_Noreturn ");
}

CNDEF void cn__emit_type_specifier_primitive(Cn_Emitter *e, Cn_Ast_Type_Specifier_Primitive *node) {
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

CNDEF void cn__emit_type_specifier_typedef(Cn_Emitter *e, Cn_Ast_Type_Specifier_Typedef *node) {
    cn__emit_str(e, node->typedef_name);
}

CNDEF void cn__emit_struct_specifier(Cn_Emitter *e, Cn_Ast_Struct_Specifier *node) {
    cn__emit_str_lit(e, "struct ");

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn_emit(e, node->identifier);

    // Only emit body if there are members.
    if (node->member_declarations.length > 0) {
        cn__emit_str_lit(e, " {");

        e->indent++;
        for (int64_t i = 0; i < node->member_declarations.length; i++) {
            cn_emit(e, node->member_declarations.ptrs[i]);
        }
        e->indent--;

        cn__emit_newline(e);
        cn__emit_str_lit(e, "}");
    }
}

CNDEF void cn__emit_union_specifier(Cn_Emitter *e, Cn_Ast_Union_Specifier *node) {
    cn__emit_str_lit(e, "union ");

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn_emit(e, node->identifier);

    if (node->member_declarations.length > 0) {
        cn__emit_str_lit(e, " {");

        e->indent++;
        for (int64_t i = 0; i < node->member_declarations.length; i++) {
            cn_emit(e, node->member_declarations.ptrs[i]);
        }
        e->indent--;

        cn__emit_newline(e);
        cn__emit_str_lit(e, "}");
    }
}

CNDEF void cn__emit_enum_specifier(Cn_Emitter *e, Cn_Ast_Enum_Specifier *node) {
    cn__emit_str_lit(e, "enum ");

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
cn_emit(e, node->identifier);

    if (node->specifier_qualifier != NULL) {
        cn__emit_str_lit(e, " : ");
        cn_emit(e, node->specifier_qualifier);
    }

    if (node->flags & CN_AST_ENUM_HAS_DEFINITION) {
        cn__emit_str_lit(e, " {");

        e->indent++;
        for (int64_t i = 0; i < node->enumerators.length; i++) {
            cn_emit(e, node->enumerators.ptrs[i]);
        }
        e->indent--;

        cn__emit_newline(e);
        cn__emit_str_lit(e, "}");
    }
}

CNDEF void cn__emit_type_name(Cn_Emitter *e, Cn_Ast_Type_Name *node) {
    cn_emit(e, node->specifier_qualifier);

    if (node->abstract_declarator != NULL) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->abstract_declarator);
    }
}

CNDEF void cn__emit_specifier_qualifier(Cn_Emitter *e, Cn_Ast_Specifier_Qualifier *node) {
#ifdef CN_GNU
    cn__emit_attribute_specifiers(e, node->attribute_specifiers);
#endif
    cn__emit_qualifiers(e, node->qualifiers);
    cn_emit(e, node->type_specifier);
}

CNDEF void cn__emit_identifier(Cn_Emitter *e, Cn_Ast_Identifier *node) {
    cn__emit_str(e, node->name);
}

CNDEF void cn__emit_integer(Cn_Emitter *e, Cn_Ast_Integer *node) {
    cn__emit_format(e, "%ld", node->value);
}

CNDEF void cn__emit_flt(Cn_Emitter *e, Cn_Ast_Float *node) {
    cn__emit_format(e, "%lf", node->value);
}

CNDEF void cn__emit_character(Cn_Emitter *e, Cn_Ast_Char *node) {
    cn__emit_str_lit(e, "'");
    for (int i = node->unit_count - 1; i >= 0; i--) {
        cn__emit_format(e, "\\x%02x", (unsigned)(uint8_t)(node->value >> (i * 8)));
    }
    cn__emit_str_lit(e, "'");
}

CNDEF void cn__emit_string(Cn_Emitter *e, Cn_Ast_String *node) {
    cn__emit_str_lit(e, "\"");
    cn__emit_str(e, node->str);
    cn__emit_str_lit(e, "\"");
}

CNDEF void cn__emit_member_declaration(Cn_Emitter *e, Cn_Ast_Member_Declaration *node) {
    cn__emit_newline(e);

#ifdef CN_GNU
    if (node->flags & CN_AST_GNU_EXTENSION) cn__emit_str_lit(e, "__extension__ ");
#endif

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    cn_emit(e, node->specifier_qualifier);

    for (int64_t i = 0; i < node->member_declarators.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ",");
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->member_declarators.ptrs[i]);
    }
    cn__emit_str_lit(e, ";");
}

CNDEF void cn__emit_member_declarator(Cn_Emitter *e, Cn_Ast_Member_Declarator *node) {
    cn_emit(e, node->declarator);

    if (node->bitfield != NULL) {
        cn__emit_str_lit(e, " : ");
        cn_emit(e, node->bitfield);
    }

#ifdef CN_GNU
    cn__emit_attribute_specifiers_suffix(e, node->attribute_specifiers);
#endif
}


CNDEF void cn__emit_init_declarator(Cn_Emitter *e, Cn_Ast_Init_Declarator *node) {
    cn_emit(e, node->declarator);

#ifdef CN_GNU
    if (node->asm_label != NULL) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->asm_label);
    }
    cn__emit_attribute_specifiers_suffix(e, node->attribute_specifiers);
#endif

    if (node->initializer != NULL) {
        cn__emit_str_lit(e, " = ");
        cn_emit(e, node->initializer);
    }
}

CNDEF void cn__emit_parameter_declaration(Cn_Emitter *e, Cn_Ast_Parameter_Declaration *node) {
    cn_emit(e, node->declaration_specifiers);

    if (node->declarator != NULL) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, node->declarator);
    }
}

CNDEF void cn__emit_enumerator(Cn_Emitter *e, Cn_Ast_Enumerator *node) {
    cn__emit_newline(e);

    cn_emit(e, node->identifier);

    cn__emit_attribute_specifiers(e, node->attribute_specifiers);

    if (node->expression != NULL) {
        cn__emit_str_lit(e, " = ");
        cn_emit(e, node->expression);
    }

    cn__emit_str_lit(e, ",");
}

/**
 * Helper, `true` if attribute specifier emits nothing, which is the case only for empty `[[]]`.
 */
CNDEF bool cn__emit_attribute_specifier_is_empty(Cn_Ast_Node *specifier) {
#ifdef CN_GNU
    if (specifier->flags & CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU) return false;
#endif
    return cn_ast_as(Attribute_Specifier, specifier)->attributes.length == 0;
}

CNDEF void cn__emit_attribute_specifiers(Cn_Emitter *e, Cn_Ast_Nodes specifiers) {
    for (int64_t i = 0; i < specifiers.length; i++) {
        // Empty attribute specifiers emit nothing, so no separator either.
        if (cn__emit_attribute_specifier_is_empty(specifiers.ptrs[i])) continue;

        cn_emit(e, specifiers.ptrs[i]);
        cn__emit_str_lit(e, " ");
    }
}

#ifdef CN_GNU
/**
 * Emits attribute specifiers that follow code, each preceded by a space.
 */
CNDEF void cn__emit_attribute_specifiers_suffix(Cn_Emitter *e, Cn_Ast_Nodes specifiers) {
    for (int64_t i = 0; i < specifiers.length; i++) {
        if (cn__emit_attribute_specifier_is_empty(specifiers.ptrs[i])) continue;

        cn__emit_str_lit(e, " ");
        cn_emit(e, specifiers.ptrs[i]);
    }
}
#endif

CNDEF void cn__emit_attribute_specifier(Cn_Emitter *e, Cn_Ast_Attribute_Specifier *node) {
#ifdef CN_GNU
    if (node->flags & CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU) {
        cn__emit_str_lit(e, "__attribute__((");
        for (int64_t i = 0; i < node->attributes.length; i++) {
            if (i > 0) cn__emit_str_lit(e, ", ");
            cn_emit(e, node->attributes.ptrs[i]);
        }
        cn__emit_str_lit(e, "))");
        return;
    }
#endif

    // Skipping empty attribute specifiers entirely.
    if (node->attributes.length == 0) return;

    cn__emit_str_lit(e, "[[");
    for (int64_t i = 0; i < node->attributes.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->attributes.ptrs[i]);
    }
    cn__emit_str_lit(e, "]]");
}

CNDEF void cn__emit_attribute(Cn_Emitter *e, Cn_Ast_Attribute *node) {
    // Emit vendor prefix if present.
    if (node->vendor_identifier != NULL) {
        cn_emit(e, node->vendor_identifier);
        cn__emit_str_lit(e, "::");
    }

    cn_emit(e, node->identifier);

    // Emit arguments if present.
    if (node->arguments.length > 0) {
        cn__emit_str_lit(e, "(");
        for (int64_t i = 0; i < node->arguments.length; i++) {
            if (i > 0) cn__emit_str_lit(e, ", ");
            cn_emit(e, node->arguments.ptrs[i]);
        }
        cn__emit_str_lit(e, ")");
    }
}


#ifdef CN_GNU
CNDEF void cn__emit_gnu_typeof(Cn_Emitter *e, Cn_Ast_Gnu_Typeof *node) {
    cn__emit_str_lit(e, "__typeof__(");
    cn_emit(e, node->target);
    cn__emit_str_lit(e, ")");
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_qualifiers(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Qualifier_Flags qualifiers) {
    if (qualifiers & CN_AST_GNU_ASM_QUALIFIER_VOLATILE) cn__emit_str_lit(e, "volatile ");
    if (qualifiers & CN_AST_GNU_ASM_QUALIFIER_INLINE)   cn__emit_str_lit(e, "inline ");
    if (qualifiers & CN_AST_GNU_ASM_QUALIFIER_GOTO)     cn__emit_str_lit(e, "goto ");
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_section(Cn_Emitter *e, Cn_Ast_Nodes list) {
    cn__emit_str_lit(e, " :");
    for (int64_t i = 0; i < list.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ",");
        cn__emit_str_lit(e, " ");
        cn_emit(e, list.ptrs[i]);
    }
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_statement(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Statement *node) {
    cn__emit_str_lit(e, "__asm__ ");
    cn__emit_gnu_asm_qualifiers(e, node->qualifiers);

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->template);

    // Basic and extended asm differ semantically (e.g. '%' escaping in template), so
    // an extended asm keeps at least one ':' even when all of its sections are empty.
    // Trailing empty sections are omitted, but a section is always emitted if any later one is not empty.
    int sections = 0;
    if (node->goto_labels.length > 0)   sections = 4;
    else if (node->clobbers.length > 0) sections = 3;
    else if (node->inputs.length > 0)   sections = 2;
    else if (node->outputs.length > 0 || (node->flags & CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED)) sections = 1;

    if (sections >= 1) cn__emit_gnu_asm_section(e, node->outputs);
    if (sections >= 2) cn__emit_gnu_asm_section(e, node->inputs);
    if (sections >= 3) cn__emit_gnu_asm_section(e, node->clobbers);
    if (sections >= 4) cn__emit_gnu_asm_section(e, node->goto_labels);

    cn__emit_str_lit(e, ");");
    cn__emit_newline(e);
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_operand(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Operand *node) {
    if (node->identifier != NULL) {
        cn__emit_str_lit(e, "[");
        cn_emit(e, node->identifier);
        cn__emit_str_lit(e, "] ");
    }

    cn_emit(e, node->string);

    cn__emit_str_lit(e, " (");
    cn_emit(e, node->expression);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_gnu_asm_label(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Label *node) {
    cn__emit_str_lit(e, "__asm__(");
    cn_emit(e, node->string);
    cn__emit_str_lit(e, ")");
}
#endif // CN_GNU

CNDEF void cn_emit(Cn_Emitter *e, void *n) {
    if (n == NULL) return;

    Cn_Ast_Node *node = n;

#ifdef CN_GNU
    // '__extension__' goes right before the node it is attached to, member declaration
    // emits it itself, since it starts with a newline.
    if ((node->flags & CN_AST_GNU_EXTENSION) && node->kind != CN_AST_MEMBER_DECLARATION) {
        cn__emit_str_lit(e, "__extension__ ");
    }
#endif

    void (*emit)(Cn_Emitter *, void *) = cn_ast_info(node->kind)->emit_func;
    emit(e, node);
}


/**
 * ============================================
 * IMPLEMENTATION SECTION: Parse
 * ============================================
 */

CNDEF bool cn_parse_expect(Cn_Lexer *lexer, Cn_Token_Kind kind) {
    if (cn_lexer_expect(lexer, kind)) {
        cn_lexer_next_token(lexer);
        return true;
    }

    Cn_Token token = cn_lexer_peek(lexer, 0);
    cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &token.loc, &token.src, CN_DC_EXPECTED_TOKEN, "Expected '%s' token, but got '%s'.", cn_token_kind_name(kind), cn_token_kind_name(cn_lexer_token(lexer).kind));
    return false;
}

CNDEF bool cn_parse_optional(Cn_Lexer *lexer, Cn_Token_Kind kind) {
    if (cn_lexer_expect(lexer, kind)) {
        cn_lexer_next_token(lexer);
        return true;
    }
    return false;
}

CNDEF Cn_Ast_Translation_Unit *cn_parse_translation_unit(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Translation_Unit node = { .kind = CN_AST_TRANSLATION_UNIT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // Pushing first scope that will act as a global scope.
    cn_scope_stack_push();
    // Inject compiler builtins that appear in preprocessed .i files but are
    // never declared by any source.
    {
        Cn_Type *opaque = cn_analyze_intern_type((Cn_Type *)&CN_TYPE_OPAQUE);

        cn_binding_declare_typedef(CN_STR_LIT("__builtin_va_list"),              NULL, opaque);
    }

    int64_t mark = cn_ast_stack_mark();

    // Executing hooks for the tu start.
    {
        Cn_Lexer l = *lexer;
        Cn_Ast_Node *null;
        for (int64_t i = 0; i < cn_array_list_length(&cn_hooks.tu_start_hooks); i++) {
            null = NULL;

            Cn_Hook_Response response = cn_hooks.tu_start_hooks[i](&null);
            if (response & CN_HOOK_RESPONSE_ERROR) goto error;

            if (null == NULL) continue;

            // Emitting code from received node.
            Cn_String_Builder sb = cn_sb_make(512);
            Cn_Emitter emitter = { 
                .write  = cn_emit_write_sb, 
                .ctx    = &sb 
            };
            cn_emit(&emitter, null);

            Cn_String code = CN_STR(sb.length, cn_chained_arena_alloc(&cn__tu_data->output_arena, sb.length));
            cn_str_copy_to(cn_sb_to_str(&sb), code.data);
            cn_sb_free(&sb);
            
            // Reparsing code that is safely stored in `output_arena`.
            cn_lexer_load_content(&l, code);
            
            while (cn_lexer_token(&l).kind != CN_TOKEN_EOF) {
                Cn_Ast_External_Declaration *next_idx = cn_parse_external_declaration(&l);
                if (next_idx == NULL) goto error;
                cn_ast_stack_push(next_idx);
            }
        }
    }

    while (cn_lexer_token(lexer).kind != CN_TOKEN_EOF) {
        Cn_Ast_External_Declaration *next_idx = cn_parse_external_declaration(lexer);
        if (next_idx == NULL) goto error;
        cn_ast_stack_push(next_idx);
    }

    // Executing hooks for the tu end.
    {
        Cn_Lexer l = *lexer;
        Cn_Ast_Node *null;
        for (int64_t i = 0; i < cn_array_list_length(&cn_hooks.tu_end_hooks); i++) {
            null = NULL;

            Cn_Hook_Response response = cn_hooks.tu_end_hooks[i](&null);
            if (response & CN_HOOK_RESPONSE_ERROR) goto error;

            if (null == NULL) continue;

            // Emitting code from received node.
            Cn_String_Builder sb = cn_sb_make(512);
            Cn_Emitter emitter = { 
                .write  = cn_emit_write_sb, 
                .ctx    = &sb 
            };
            cn_emit(&emitter, null);

            Cn_String code = CN_STR(sb.length, cn_chained_arena_alloc(&cn__tu_data->output_arena, sb.length));
            cn_str_copy_to(cn_sb_to_str(&sb), code.data);
            cn_sb_free(&sb);
            
            // Reparsing code that is safely stored in `output_arena`.
            cn_lexer_load_content(&l, code);
            
            while (cn_lexer_token(&l).kind != CN_TOKEN_EOF) {
                Cn_Ast_External_Declaration *next_idx = cn_parse_external_declaration(&l);
                if (next_idx == NULL) goto error;
                cn_ast_stack_push(next_idx);
            }
        }
    }

    node.external_declarations = cn_ast_stack_finalize(mark);

    Cn_Ast_Translation_Unit *idx = cn_ast_new(node);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_External_Declaration *cn_parse_external_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_External_Declaration node = { .kind = CN_AST_EXTERNAL_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

#ifdef CN_GNU
    if (cn__parse_gnu_extension(lexer)) node.flags |= CN_AST_GNU_EXTENSION;
#endif

    // If stray ';' case, it still can be prefixed.
    if (cn_lexer_token(lexer).kind == CN_TOKEN_SEMICOLON) {
        cn_lexer_next_token(lexer);
        return cn_ast_new(node);
    }

#ifdef CN_GNU
    // File scope asm, only basic asm without qualifiers is allowed, same as GCC.
    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ASM)) {
        Cn_Ast_Gnu_Asm_Statement *statement = cn_parse_gnu_asm_statement(lexer);
        if (statement == NULL) goto error;

        if (statement->qualifiers != 0 || (statement->flags & CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED)) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, statement, CN_DC_UNEXPECTED_TOKEN, "Only basic asm without qualifiers is allowed at file scope.");
            goto error;
        }

        node.child = cn_ast_upcast(statement);
    }
#endif

    if (node.child == NULL) {
        // Saving state because messages could be send on parsed function or declaration ast nodes.
        Cn_Lexer saved_lexer    = *lexer;
        Cn_Tu_Saved saved_state = {0};
        cn__tu_save(&saved_state);

        // Last possible case function definition or declaration.
        Cn_Ast_Node *function_or_declaration;
        function_or_declaration = cn_parse_function_or_declaration(lexer);
        if (function_or_declaration == NULL) goto error;

        CN__EXECUTE_HOOK_AST_PARSED(function_or_declaration, saved_state, saved_lexer, cn_parse_function_or_declaration)

        node.child = function_or_declaration;
    }

    Cn_Ast_External_Declaration *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_function_or_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Location loc = cn_lexer_token(lexer).loc;
    Cn_Source src = cn_lexer_token(lexer).src;

    // Getting attributes [[]].
    bool ok;
    Cn_Ast_Nodes attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    // Getting declration specifiers.
    Cn_Ast_Declaration_Specifiers *declaration_specifiers = cn_parse_declaration_specifiers(lexer);
    if (declaration_specifiers == NULL) goto error;

    // Deciding whether node is function definiton or declaration.
    // There MUST be declarator,
    // which will be either part of init_declarators or just be by itself.
    // It might be completely abstract, meaning no tokens are part of it.
    Cn_Ast_Declarator *declarator = cn_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    // Empty declaration case:
    if (declarator == NULL) {
        
        // Expecting ';' at the end of empty declaration.
        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

        Cn_Ast_Declaration node = {
            .kind = CN_AST_DECLARATION,
            .loc = loc,
            .src = src,
        };

        node.flags |= CN_AST_DECLARATION_IS_EMPTY;
        node.attribute_specifiers = attribute_specifiers;
        node.declaration_specifiers = declaration_specifiers;

        // New types can be made, even in empty declaration.
        ok = cn_analyze_declaration(node.declaration_specifiers);
        if (!ok) goto error;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }
    
    // Non-empty abstract declarator handling:
    if (declarator->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator in declaration.");
        goto error;
    }


    // Now finally we can decide if next token is '{' it is definitely a function definition because, it is part of function definition body, anything else could be part of init_declarator_list or ';'.
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        Cn_Ast_Function node = {
            .kind = CN_AST_FUNCTION,
            .loc = loc,
            .src = src,
        };
        node.attribute_specifiers = attribute_specifiers;
        node.declaration_specifiers = declaration_specifiers;
        node.declarator = declarator;

        // Adding function type and binding.
        Cn_Binding_Idx function_binding_idx;
        ok = cn_analyze_function_definition(
                node.declaration_specifiers,
                node.declarator,
                &function_binding_idx
                );

        if (!ok) goto error;

        // Making new function scope to store parameters + compound statement symbols.
        cn_scope_stack_push();
        cn__tu_data->function_scope_idx = CN_SCOPE_STACK_CURRENT_IDX;

        // Binding non-abstract function parameters as variables from definition.
        if (!cn_binding_declare_function_arguments(function_binding_idx)) goto error;

        node.block = cn_parse_block(lexer, true);
        if (node.block == NULL) goto error;

        cn_scope_stack_pop();

        Cn_Ast_Node *idx = cn_ast_new(node);
        return idx;

    } else {
        Cn_Ast_Declaration node = {
            .kind = CN_AST_DECLARATION,
            .loc = loc,
            .src = src,
        };

        node.attribute_specifiers = attribute_specifiers;
        node.declaration_specifiers = declaration_specifiers;
        node.init_declarators = cn_parse_finish_init_declarators(lexer, declarator, &ok, node.declaration_specifiers);
        if (!ok) goto error;

        // ';' at the end check.
        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

        // // Adding types that are used by declarators.
        // ok = cn_analyze_declaration(node.declaration_specifiers, node.init_declarators);
        // if (!ok) goto error;

        Cn_Ast_Node *idx = cn_ast_new(node);
        return idx;
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_finish_declaration(Cn_Lexer *lexer, Cn_Ast_Nodes attribute_specifiers) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Declaration node = {
        .kind = CN_AST_DECLARATION,
        .loc = cn_lexer_token(lexer).loc,
        .src = cn_lexer_token(lexer).src,
    };

    node.attribute_specifiers = attribute_specifiers;
    
    // Getting declration specifiers.
    Cn_Ast_Declaration_Specifiers *declaration_specifiers = cn_parse_declaration_specifiers(lexer);
    if (declaration_specifiers == NULL) goto error;
    node.declaration_specifiers = declaration_specifiers;

    // If first single declarator is abstract, it is empty declaration case.
    bool ok;
    Cn_Ast_Declarator *declarator = cn_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    
    // Empty declaration case:
    if (declarator == NULL) {
        node.flags |= CN_AST_DECLARATION_IS_EMPTY;

        // New types can be made, even in empty declaration.
        ok = cn_analyze_declaration(node.declaration_specifiers);
        if (!ok) goto error;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }
    
    // Non-empty abstract declarator handling:
    if (declarator->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator in declaration.");
        goto error;
    }
    
    node.init_declarators = cn_parse_finish_init_declarators(lexer, declarator, &ok, node.declaration_specifiers);
    if (!ok) goto error;

    //     // Adding types that are used by declarators.
    //     // ok = cn_analyze_declaration(node.declaration_specifiers, node.init_declarators);
    //     // if (!ok) goto error;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF bool cn_parse_starts_declaration(Cn_Lexer *lexer) {
    return 
            cn_lexer_expect(lexer, CN_TOKEN_STATIC) ||
            cn_lexer_expect(lexer, CN_TOKEN_EXTERN) ||
            cn_lexer_expect(lexer, CN_TOKEN_REGISTER) ||
            cn_lexer_expect(lexer, CN_TOKEN_AUTO) ||
            cn_lexer_expect(lexer, CN_TOKEN_TYPEDEF) ||
#ifdef CN_GNU
            cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE) ||
#endif
            cn_parse_starts_type(lexer)
            ;
}

CNDEF bool cn_parse_starts_type(Cn_Lexer *lexer) {
    if (cn_lexer_token(lexer).kind == CN_TOKEN_IDENTIFIER) {
        Cn_String str = cn_source_to_str(&cn_lexer_token(lexer).src);
        Cn_Binding_Idx *ref = cn_hash_table_get(&cn__tu_data->symbol_binding_table, &str);
        if (ref != NULL && cn__tu_data->binding_list[*ref].kind == CN_BINDING_TYPEDEF) 
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
            cn_lexer_expect(lexer, CN_TOKEN_UNION)
#ifdef CN_GNU
            || cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)
#endif
            ;
}

CNDEF Cn_Ast_Node *cn_parse_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    bool ok;
    Cn_Ast_Nodes attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    return cn_parse_finish_statement(lexer, attribute_specifiers);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_finish_statement(Cn_Lexer *lexer, Cn_Ast_Nodes attribute_specifiers) {
    Cn_Ast_Node *statement = NULL;

#ifdef CN_GNU
    // Block scope asm statement.
    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ASM)) {
        statement = cn_ast_upcast(cn_parse_gnu_asm_statement(lexer));
        if (statement == NULL) goto error;
    }
#endif

    if (statement == NULL) {
        if (cn_lexer_expect(lexer, CN_TOKEN_CASE) || cn_lexer_peek(lexer, 1).kind == CN_TOKEN_COLON) {
            statement = cn_parse_label(lexer);
            if (statement == NULL) goto error;

            cn_ast_as(Label, statement)->attribute_specifiers = attribute_specifiers;
        } else {
            switch (cn_lexer_token(lexer).kind) {
                case CN_TOKEN_CURLY_OPEN:
                    statement = cn_ast_upcast(cn_parse_block(lexer, false));
                    if (statement == NULL) goto error;

                    cn_ast_as(Block, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_IF:
                    statement = cn_parse_if_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(If, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_SWITCH:
                    statement = cn_parse_switch_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Switch, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_WHILE:
                    statement = cn_parse_while_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(While, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_DO:
                    statement = cn_parse_do_while(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Do_While, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_FOR:
                    statement = cn_parse_for_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(For, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_GOTO:
                    statement = cn_parse_goto_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Goto, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_RETURN:
                    statement = cn_parse_return_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Return, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_BREAK:
                    statement = cn_parse_break_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Break, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                case CN_TOKEN_CONTINUE:
                    statement = cn_parse_continue_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Continue, statement)->attribute_specifiers = attribute_specifiers;

                    break;

                default:
                    statement = cn_parse_expression_statement(lexer);
                    if (statement == NULL) goto error;

                    cn_ast_as(Expression_Statement, statement)->attribute_specifiers = attribute_specifiers;

                    break;
            }
        }
    }

    return statement;

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Ast_Block *cn_parse_block(Cn_Lexer *lexer, bool use_current_scope) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Block node = { .kind = CN_AST_BLOCK, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_OPEN)) goto error;

    // Stepping into scope.
    if (!use_current_scope) cn_scope_stack_push();

    int64_t mark = cn_ast_stack_mark();

    while (!cn_parse_optional(lexer, CN_TOKEN_CURLY_CLOSE)) {
        Cn_Ast_Block_Item *block_item = cn_parse_block_item(lexer);
        if (block_item == NULL) goto error;
        cn_ast_stack_push(block_item);
    }

    node.block_items = cn_ast_stack_finalize(mark);

    if (!use_current_scope) cn_scope_stack_pop();

    Cn_Ast_Block *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Block_Item *cn_parse_block_item(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Block_Item node = { .kind = CN_AST_BLOCK_ITEM, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // We assume we deal with declaration first. If not, we will have to reparse a little.
    // Thats the best way I see to deal with declaration vs. statement ambiguity without going
    // worse as of right now.
    Cn_Lexer before_decl_prefix = *lexer;
    Cn_Tu_Saved before_decl_state;
    cn__tu_save(&before_decl_state);


    bool has_declaration_prefix_changes = false;
#ifdef CN_GNU
    has_declaration_prefix_changes = cn__parse_gnu_extension(lexer);
#endif



    // Parsing attribute specifiers here in order to get to the tokens that would 
    // disambiguate delcaration vs statement situation.
    bool ok;
    Cn_Ast_Nodes attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    Cn_Ast_Node *declaration_or_statement;

    if (cn_parse_starts_declaration(lexer)) {
        declaration_or_statement = cn_parse_finish_declaration(lexer, attribute_specifiers);
        if (declaration_or_statement == NULL) goto error;

#ifdef CN_GNU
        if (has_declaration_prefix_changes) declaration_or_statement->flags |= CN_AST_GNU_EXTENSION;
#endif

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    } else {

        if (has_declaration_prefix_changes) {
            // Rollback to before we parsed. But now we know we have statement ahead
            // so we just handle everything correct from now on.
            *lexer = before_decl_prefix;
            cn__tu_rollback(&before_decl_state);
            
            attribute_specifiers = (Cn_Ast_Nodes) {0};
        }

        declaration_or_statement = cn_parse_finish_statement(lexer, attribute_specifiers);
    }

    if (declaration_or_statement == NULL) goto error;
    node.declaration_or_statement = declaration_or_statement;
    
    Cn_Ast_Block_Item *parent = cn_ast_new(node);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_if_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_If node = { .kind = CN_AST_IF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_IF)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'if' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' in if statement.");
    }
    cn_lexer_next_token(lexer);

    node.condition = cn_parse_expression(lexer, -1, 0);
    if (node.condition == NULL) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in if statement.");
    }
    cn_lexer_next_token(lexer);

    node.then_statement = cn_parse_statement(lexer);
    if (node.then_statement == NULL) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_ELSE)) {
        cn_lexer_next_token(lexer);
        node.else_statement = cn_parse_statement(lexer);
        if (node.else_statement == NULL) goto error;
    }

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_switch_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Switch node = { .kind = CN_AST_SWITCH, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_SWITCH)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'switch' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '(' in switch statement.");
    }
    cn_lexer_next_token(lexer);

    node.condition = cn_parse_expression(lexer, -1, 0);
    if (node.condition == NULL) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected ')' in switch statement.");
    }
    cn_lexer_next_token(lexer);

    node.body = cn_parse_statement(lexer);
    if (node.body == NULL) goto error;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_while_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_While node = { .kind = CN_AST_WHILE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_WHILE)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
    if (expression == NULL) goto error;
    node.condition = expression;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Node *statement = cn_parse_statement(lexer);
    if (statement == NULL) goto error;
    node.body = statement;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_do_while(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Do_While node = { .kind = CN_AST_DO_WHILE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_DO)) goto error;

    Cn_Ast_Node *statement = cn_parse_statement(lexer);
    if (statement == NULL) goto error;
    node.body = statement;

    if (!cn_parse_expect(lexer, CN_TOKEN_WHILE)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
    if (expression == NULL) goto error;
    node.condition = expression;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_for_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_For node = { .kind = CN_AST_FOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_FOR)) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    // For statement introduces new scope.
    // for its initialization declaration to live in the scope till for loop dies.
    cn_scope_stack_push();
    
    if (!cn_parse_optional(lexer, CN_TOKEN_SEMICOLON)) {

        // We assume we deal with declaration first. If not, we will have to reparse.
        // Completely same situation as in declaration vs. statement.
        Cn_Lexer before_decl_lexer = *lexer;
        Cn_Tu_Saved before_decl_state;
        cn__tu_save(&before_decl_state);

        bool has_declaration_prefix_changes = false;
#ifdef CN_GNU
        has_declaration_prefix_changes = cn__parse_gnu_extension(lexer);
#endif

        bool ok;
        Cn_Ast_Nodes attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
        if (!ok) goto error;

        Cn_Ast_Node *declaration_or_expression;
        if (cn_parse_starts_declaration(lexer)) {
            declaration_or_expression = cn_parse_finish_declaration(lexer, attribute_specifiers);

#ifdef CN_GNU
            if (declaration_or_expression != NULL && has_declaration_prefix_changes) declaration_or_expression->flags |= CN_AST_GNU_EXTENSION;
#endif

        } else {
            if (has_declaration_prefix_changes) {
                *lexer = before_decl_lexer;
                cn__tu_rollback(&before_decl_state);
                attribute_specifiers = (Cn_Ast_Nodes) {0};
            }

            if (attribute_specifiers.length > 0) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, attribute_specifiers.ptrs[0], CN_DC_ILLEGAL_ATTRIBUTE, "Attribute can't appear in here.");
                goto error;
            }

            declaration_or_expression = cn_parse_expression(lexer, -1, 0);
        }

        if (declaration_or_expression == NULL) goto error;
        node.initialization = declaration_or_expression;

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    }

    if (!cn_parse_optional(lexer, CN_TOKEN_SEMICOLON)) {
        Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
        if (expression == NULL) goto error;
        node.condition = expression;

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    }

    if (!cn_parse_optional(lexer, CN_TOKEN_PARAN_CLOSE)) {
        Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
        if (expression == NULL) goto error;
        node.update = expression;

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    }

    Cn_Ast_Node *statement = cn_parse_statement(lexer);
    if (statement == NULL) goto error;
    node.body = statement;
    
    cn_scope_stack_pop();

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_goto_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Goto node = { .kind = CN_AST_GOTO, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_GOTO)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'goto' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    node.identifier = cn_parse_identifier(lexer);
    if (node.identifier == NULL) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_return_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Return node = { .kind = CN_AST_RETURN, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_RETURN)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'return' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_lexer_expect(lexer, CN_TOKEN_SEMICOLON)) {
        node.expression = cn_parse_expression(lexer, -1, 0);
        if (node.expression == NULL) goto error;

        // Typechecking return expression.
        if (cn_analyze_typecheck_expression(node.expression) == NULL) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_break_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Break node = { .kind = CN_AST_BREAK, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_BREAK)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'break' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_continue_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Continue node = { .kind = CN_AST_CONTINUE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_CONTINUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected 'continue' keyword.");
        goto error;
    }
    cn_lexer_next_token(lexer);

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_label(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Label node = { .kind = CN_AST_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    if (cn_parse_optional(lexer, CN_TOKEN_CASE)) {
        node.flags |= CN_AST_LABEL_IS_CASE;
        Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
        if (expression == NULL) goto error;
        node.expression = expression;

    } else if (cn_parse_optional(lexer, CN_TOKEN_DEFAULT)) {
        node.flags |= CN_AST_LABEL_IS_DEFAULT;
    } else {
        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
        if (identifier == NULL) goto error;
        node.identifier = identifier;

        // Binding new label.
        if (cn_binding_declare_label(cn_ast_as(Identifier, identifier)->name, identifier) == CN_BINDING_NIL_IDX) 
            goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_COLON)) goto error;

    Cn_Ast_Node *statement = cn_parse_statement(lexer);
    if (statement == NULL) goto error;
    node.statement = statement;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_expression_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Expression_Statement node = { .kind = CN_AST_EXPRESSION_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_optional(lexer, CN_TOKEN_SEMICOLON)) {

        node.expression = cn_parse_expression(lexer, -1, 0);
        if (node.expression == NULL) goto error;

        // Typechecking expresion.
        Cn_Type *type = cn_analyze_typecheck_expression(node.expression);
        if (type == NULL) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;
    }

    // TODO: Later replace with a better way to construct ast source lengths.
    node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_arguments(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    while (true) {
        Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (expression == NULL) goto error;
        cn_ast_stack_push(expression);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Node *cn_parse_expression_increasing_precedence(Cn_Lexer *lexer, void * left, int min_precedence, Cn_Parse_Expression_Flags flags) {;
    Cn_Lexer original_state = *lexer;

    // Ternary operator case. TODO: Make macro so its not hardcoded precedence 2.
    if (cn_lexer_expect(lexer, CN_TOKEN_QUESTION)) {
        if (2 < min_precedence) {
            return left;
        }

        Cn_Ast_Ternary node = { .kind = CN_AST_TERNARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);
        
        node.condition = left;

        Cn_Ast_Node *child = cn_parse_expression(lexer, -1, flags);
        if (child == NULL) goto error;

        node.if_true = child;

        if (!cn_parse_expect(lexer, CN_TOKEN_COLON)) goto error;

        child = cn_parse_expression(lexer, -1, flags);
        if (child == NULL) goto error;

        node.if_false = child;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }

    int8_t op_kind;

    // Binary operator case.
    op_kind = cn_ast_is_binary_operator(lexer);
    if (op_kind != CN_AST_BINARY_OP_NONE) {
        int precedence = CN_AST_BINARY_OPERATORS[op_kind].precedence; 

        if (precedence <= min_precedence) {
            return left;
        }

        // Ignoring comma operator if flag is specified.
        if (cn_lexer_expect(lexer, CN_TOKEN_COMMA) && (flags & CN_NO_COMMA_OPERATOR)) {
            return left;
        }
        
        // Function call case.
        if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Ast_Call node = { .kind = CN_AST_CALL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
            cn_lexer_next_token(lexer);

            node.expression = left;
            
            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                bool ok;
                node.arguments = cn_parse_arguments(lexer, &ok);
                if (!ok) goto error;
            }

            if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

            Cn_Ast_Node *parent = cn_ast_new(node);
            return parent;
        } 

        // Access operator cases.
        if (cn_lexer_expect(lexer, CN_TOKEN_ARROW) || cn_lexer_expect(lexer, CN_TOKEN_DOT)) {
            Cn_Ast_Access node = { .kind = CN_AST_ACCESS, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
            cn_lexer_next_token(lexer);
            
            node.expression = left;

            if (op_kind == CN_AST_BINARY_OP_ARROW) node.flags |= CN_AST_ACCESS_IS_POINTER;

            Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
            if (identifier == NULL) goto error;
            node.member = identifier;

            Cn_Ast_Node *parent = cn_ast_new(node);
            return parent;
        }

        // Array subscript case.
        Cn_Ast_Binary node = { .kind = CN_AST_BINARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        if (cn_parse_optional(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
            // nocheck
            Cn_Ast_Node *right = cn_parse_expression(lexer, -1, flags);
            if (right == NULL) goto error;

            if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;

            node.left = left;
            node.operator = CN_AST_BINARY_OP_ARRAY_SUB;
            node.right = right;

            Cn_Ast_Node *parent = cn_ast_new(node);
            return parent;
        } 

        // Regualar binary operator case.
        cn_lexer_next_token(lexer);

        Cn_Ast_Node *right = cn_parse_expression(lexer, precedence, flags);
        if (right == NULL) goto error;

        node.left = left;
        node.operator = op_kind;
        node.right = right;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }

    // Assignment operator case.
    op_kind = cn_ast_is_assignment_operator(lexer);
    if (op_kind != CN_AST_ASSIGNMENT_OP_NONE) {

        if (CN_AST_ASSIGNMENT_OPERATOR_PRECEDENCE < min_precedence) {
            return left;
        }

        Cn_Ast_Assign node = { .kind = CN_AST_ASSIGN, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        Cn_Ast_Node *right = cn_parse_expression(lexer, CN_AST_ASSIGNMENT_OPERATOR_PRECEDENCE, flags);
        if (right == NULL) goto error;

        node.left = left;
        node.operator = op_kind;
        node.right = right;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }

    // Postfix operator case.
    op_kind = cn_ast_is_postfix_operator(lexer);
    if (op_kind != CN_AST_POSTFIX_OP_NONE) {

        if (CN_AST_POSTFIX_OPERATOR_PRECEDENCE <= min_precedence) {
            return left;
        }

        Cn_Ast_Postfix node = { .kind = CN_AST_POSTFIX, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        node.expression = left;
        node.operator = op_kind;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }

    return left;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}
 
CNDEF Cn_Ast_Node *cn_parse_expression(Cn_Lexer *lexer, int min_precedence, Cn_Parse_Expression_Flags flags) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node *left = cn_parse_expression_leaf(lexer, flags);
    if (left == NULL) goto error;

    Cn_Ast_Node *node_ptr;

    while (true) {
        node_ptr = cn_parse_expression_increasing_precedence(lexer, left, min_precedence, flags);
        if (node_ptr == left) break;

        if (node_ptr == NULL) goto error;

        left = node_ptr;
    }

    Cn_Ast_Node *node = left;
    CN_ASSERT(node->kind != CN_AST_UNKNOWN);
    return left;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_expression_leaf(Cn_Lexer *lexer, Cn_Parse_Expression_Flags flags) {
    Cn_Lexer original_state = *lexer;

    // Checking if parenthesized expression, handling cast expression in here too.
    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        cn_lexer_next_token(lexer);
        
        // Cast expression case.
        if (cn_parse_starts_type(lexer)) {
            Cn_Ast_Cast node = { .kind = CN_AST_CAST, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            Cn_Ast_Type_Name *type_name = cn_parse_type_name(lexer);
            if (type_name == NULL) goto error;
            
            if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
            
            // Compound literal case.
            if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
                Cn_Ast_Compound compound = {
                    .kind = CN_AST_COMPOUND,
                    .loc = node.loc,
                    .src = node.src,
                    .type_name = type_name,
                };

                bool ok;
                compound.designations = cn_parse_designations(lexer, &ok);
                if (!ok) goto error;

                return cn_ast_new(compound);
            }

            Cn_Ast_Node *expression = cn_parse_expression(lexer, CN_AST_UNARY_OPERATOR_PRECEDENCE, flags);
            if (expression == NULL) goto error;

            node.type_name = type_name;
            node.expression = expression;

            Cn_Ast_Node *parent = cn_ast_new(node);
            return parent;
        }
        
        // Parenthesized expression case.
        Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
        if (expression == NULL) goto error;
        
        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

        return expression;
    }

    // Handling sizeof expression.
    if (cn_lexer_expect(lexer, CN_TOKEN_SIZEOF)) {
        Cn_Ast_Sizeof node = { .kind = CN_AST_SIZEOF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
        cn_lexer_next_token(lexer);

        if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Lexer peeking_lexer = *lexer;
            cn_lexer_next_token(&peeking_lexer);

            if (cn_parse_starts_type(&peeking_lexer)) {
                // Parsing type_name.
                cn_lexer_next_token(lexer);
                
                Cn_Ast_Type_Name *type_name = cn_parse_type_name(lexer);
                if (type_name == NULL) goto error;

                if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

                node.target = cn_ast_upcast(type_name);

                Cn_Ast_Node *parent = cn_ast_new(node);
                return parent;
            }
        }

        Cn_Ast_Node *expression = cn_parse_expression(lexer, CN_AST_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression == NULL) goto error;

        node.target = expression;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }

    // Handling unary expression.
    Cn_Ast_Unary_Operator_Kind op_kind = cn_ast_is_unary_operator(lexer);
    if (op_kind != CN_AST_UNARY_OP_NONE) {
        Cn_Ast_Unary node = { .kind = CN_AST_UNARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
        cn_lexer_next_token(lexer);

        Cn_Ast_Node *expression = cn_parse_expression(lexer, CN_AST_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression == NULL) goto error;

        node.operator = op_kind;
        node.expression = expression;

        Cn_Ast_Node *parent = cn_ast_new(node);
        return parent;
    }

#ifdef CN_GNU
    // '__extension__' as a unary operator, attached to the expression that follows it.
    if (cn__parse_gnu_extension(lexer)) {
        Cn_Ast_Node *expression = cn_parse_expression(lexer, CN_AST_UNARY_OPERATOR_PRECEDENCE, flags);
        if (expression == NULL) goto error;

        expression->flags |= CN_AST_GNU_EXTENSION;
        return expression;
    }
#endif

    // Primary expression case.
    Cn_Ast_Primary node = { .kind = CN_AST_PRIMARY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    Cn_Ast_Node *literal = NULL;


    switch (cn_lexer_token(lexer).kind) {
        case CN_TOKEN_IDENTIFIER:
            literal = cn_ast_upcast(cn_parse_identifier(lexer));
            break;
        case CN_TOKEN_INTEGER_VALUE:
            literal = cn_ast_upcast(cn_parse_integer(lexer));
            break;
        case CN_TOKEN_FLOAT_VALUE:
            literal = cn_ast_upcast(cn_parse_float(lexer));
            break;
        case CN_TOKEN_CHAR_VALUE:
            literal = cn_ast_upcast(cn_parse_character(lexer));
            break;
        case CN_TOKEN_STRING:
            literal = cn_ast_upcast(cn_parse_string(lexer));
            break;
        default:
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_EXPRESSION, "Expected symbol, integer, float or string token for primary expression.");
            goto error;
    }
    
    if (literal == NULL) goto error;
    node.literal = literal;

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_finish_init_declarators(Cn_Lexer *lexer, void * declarator, bool *ok, void *declaration_specifiers) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    Cn_Ast_Init_Declarator *child = cn_parse_finish_init_declarator(lexer, declarator, declaration_specifiers);

    while (true) {
        if (child == NULL) goto error;
        cn_ast_stack_push(child);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);

        child = cn_parse_init_declarator(lexer, declaration_specifiers);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Nodes cn_parse_init_declarators(Cn_Lexer *lexer, bool *ok, void *declaration_specifiers) {
    Cn_Lexer original_state = *lexer;

    bool decl_ok;
    Cn_Ast_Declarator *declarator = cn_parse_declarator(lexer, &decl_ok);
    if (!decl_ok) goto error;

    if (declarator == NULL || declarator->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator inside init declarator.");
        goto error;
    }

    return cn_parse_finish_init_declarators(lexer, declarator, ok, declaration_specifiers);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Init_Declarator *cn_parse_finish_init_declarator(Cn_Lexer *lexer, Cn_Ast_Declarator *declarator, void *declaration_specifiers) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Init_Declarator node = { .kind = CN_AST_INIT_DECLARATOR, .loc = ((Cn_Ast_Node *)declarator)->loc, .src = ((Cn_Ast_Node *)declarator)->src };

    node.declarator = cn_ast_as(Declarator, declarator);

#ifdef CN_GNU
    // Asm label and attributes, in any order.
    int64_t attribute_mark = cn_ast_stack_mark();
    while (true) {
        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ASM)) {
            if (node.asm_label != NULL) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Duplicate asm label.");
                goto error;
            }

            node.asm_label = cn_parse_gnu_asm_label(lexer);
            if (node.asm_label == NULL) goto error;
            continue;
        }

        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            Cn_Ast_Attribute_Specifier *specifier = cn_parse_gnu_attribute_specifier(lexer);
            if (specifier == NULL) goto error;
            cn_ast_stack_push(specifier);
            continue;
        }

        break;
    }
    node.attribute_specifiers = cn_ast_stack_finalize(attribute_mark);
#endif

    bool ok;

    if (cn_lexer_expect(lexer, CN_TOKEN_ASSIGN)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Initializer *initializer = cn_parse_initializer(lexer);
        if (initializer == NULL) goto error;
        node.initializer = initializer;
    }


    // Creating binding from the finished init declarator.
    ok = cn_analyze_declaration(declaration_specifiers, .declarator = node.declarator, .initializer = node.initializer);
    if (!ok) goto error;

    Cn_Ast_Init_Declarator *parent = cn_ast_new(node);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Init_Declarator *cn_parse_init_declarator(Cn_Lexer *lexer, void *declaration_specifiers) {
    Cn_Lexer original_state = *lexer;

    bool ok;
    Cn_Ast_Declarator *declarator = cn_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    if (declarator == NULL || declarator->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator inside init declarator.");
        goto error;
    }

    return cn_parse_finish_init_declarator(lexer, declarator, declaration_specifiers);
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Initializer *cn_parse_initializer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Initializer node = { .kind = CN_AST_INITIALIZER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        bool ok;
        node.designations = cn_parse_designations(lexer, &ok);   
        if (!ok) goto error;
    } else {
        node.expression = cn_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (node.expression == NULL) goto error;
    }

    Cn_Ast_Initializer *parent = cn_ast_new(node);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_designations(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_OPEN)) goto error;

    int64_t mark = cn_ast_stack_mark();
    
    while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
        Cn_Ast_Designation *designation = cn_parse_designation(lexer);
        if (designation == NULL) goto error;
        cn_ast_stack_push(designation);

        if (!cn_parse_optional(lexer, CN_TOKEN_COMMA)) break;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE)) goto error;

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Designation *cn_parse_designation(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Designation node = { .kind = CN_AST_DESIGNATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_parse_starts_designator(lexer)) {

        bool ok;
        node.designators = cn_parse_designators(lexer, &ok);
        if (!ok) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_ASSIGN)) goto error;
    }

    Cn_Ast_Initializer *initializer = cn_parse_initializer(lexer);
    if (initializer == NULL) goto error;
    node.initializer = initializer;

    Cn_Ast_Designation *parent = cn_ast_new(node);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_designators(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();
    
    while (true) {
        Cn_Ast_Designator *designator = cn_parse_designator(lexer);
        if (designator == NULL) goto error;
        cn_ast_stack_push(designator);

        if (!cn_parse_starts_designator(lexer)) break;
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Designator *cn_parse_designator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Designator node = { .kind = CN_AST_DESIGNATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_parse_optional(lexer, CN_TOKEN_DOT)) {
        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
        if (identifier == NULL) goto error;
        node.identifier = identifier;

    } else {
        if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) goto error;

        Cn_Ast_Node *expression;

        expression = cn_parse_expression(lexer, -1, 0);
        if (expression == NULL) goto error;
        node.expression = expression;

        if (cn_parse_optional(lexer, CN_TOKEN_ELLIPSIS)) {
            expression = cn_parse_expression(lexer, -1, 0);
            if (expression == NULL) goto error;
            node.expression_range_end = expression;
        }

        if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;
    }

    Cn_Ast_Designator *parent = cn_ast_new(node);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF bool cn_parse_starts_designator(Cn_Lexer *lexer) {
    return cn_lexer_expect(lexer, CN_TOKEN_DOT) || cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN);
}

CNDEF Cn_Ast_Declarator *cn_parse_declarator(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Declarator node = { .kind = CN_AST_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Node *child;
    
    // Optional pointer in declarator.
    if (cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        child = cn_ast_upcast(cn_parse_pointer(lexer));

        if (child == NULL) goto error;

        node.pointer = cn_ast_as(Pointer, child);
    }
    
    // Direct declarator parsing.
    bool is_abstract;
    child = cn_parse_direct_declarator(lexer, &is_abstract);

    // Checking if it is abstract declarator, if it is then child allowed to be NIL.
    if (is_abstract) {

        // Completely empty abstract declarator case:
        if (child == NULL && node.pointer == NULL) {
            *ok = true;
            return NULL;
        }

        node.flags |= CN_AST_DECLARATOR_IS_ABSTRACT;

    } else if (child == NULL) goto error;

    node.direct_declarator = child;
    
    Cn_Ast_Declarator *parent = cn_ast_new(node);
    *ok = true;
    return parent;
    
error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Pointer *cn_parse_pointer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Pointer node = { .kind = CN_AST_POINTER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_ASTERISK)) goto error;
    
#ifdef CN_GNU
    int64_t attribute_mark = cn_ast_stack_mark();
#endif

    int ok;
    while (true) {
        ok = cn_parse_try_qualifier(lexer, &node.qualifiers);
        if (ok == 0) continue;
        if (ok == 2) goto error;

#ifdef CN_GNU
        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            Cn_Ast_Attribute_Specifier *specifier = cn_parse_gnu_attribute_specifier(lexer);
            if (specifier == NULL) goto error;
            cn_ast_stack_push(specifier);
            continue;
        }
#endif

        break;
    }

#ifdef CN_GNU
    node.attribute_specifiers = cn_ast_stack_finalize(attribute_mark);
#endif

    if (cn_lexer_expect(lexer, CN_TOKEN_ASTERISK)) {
        Cn_Ast_Node *child;

        child = cn_ast_upcast(cn_parse_pointer(lexer));
        if (child == NULL) goto error;
        node.pointer = cn_ast_as(Pointer, child);
    }
    
    Cn_Ast_Pointer *parent = cn_ast_new(node);
    return parent;
    
error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_direct_declarator(Cn_Lexer *lexer, bool *is_abstract) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Node *direct_declarator;

    // '(' declarator ')' case.
    if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
        Cn_Ast_Direct_Declarator_Grouped node = { .kind = CN_AST_DIRECT_DECLARATOR_GROUPED, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

        cn_lexer_next_token(lexer);

        bool ok;
        Cn_Ast_Declarator *child = cn_parse_declarator(lexer, &ok);
        if (!ok) goto error;

        node.declarator = child;

        // Making above declarator abstract if its child is abstract too.
        if (child == NULL || child->flags & CN_AST_DECLARATOR_IS_ABSTRACT) *is_abstract = true;
        

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

        direct_declarator = cn_ast_new(node);
    }
    // identifier case.
    else if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        *is_abstract = false;
        direct_declarator = cn_ast_upcast(cn_parse_identifier(lexer));
        if (direct_declarator == NULL) goto error;
    }
    // abstract declarator case.
    else {
        *is_abstract = true;
        direct_declarator = NULL;
    }

    // Postfix cases.
    while (true) {
        // direct_declarator '[' expression? ']' case.
        if (cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
            Cn_Ast_Direct_Declarator_Array node = { .kind = CN_AST_DIRECT_DECLARATOR_ARRAY, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            cn_lexer_next_token(lexer);

            if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
                Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
                if (expression == NULL) goto error;
                node.expression = expression;

                // Type checking expression.
                Cn_Type *type = cn_analyze_typecheck_expression(expression);
                if (type == NULL) goto error;
            }

            if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;

            // Extending source loc to highlight whole array '[...]' declarator.
            node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

            node.direct_declarator = direct_declarator;

            direct_declarator = cn_ast_new(node);
            continue;
        }
        // direct_declarator '(' ('void' | parameter_declarations)? ')' case.
        else if (cn_lexer_expect(lexer, CN_TOKEN_PARAN_OPEN)) {
            Cn_Ast_Direct_Declarator_Function node = { .kind = CN_AST_DIRECT_DECLARATOR_FUNCTION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

            cn_lexer_next_token(lexer);

            if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
                // If not '(' 'void' ')' case.
                if (cn_lexer_expect(lexer, CN_TOKEN_VOID) && cn_lexer_peek(lexer, 1).kind == CN_TOKEN_PARAN_CLOSE) {
                    cn_lexer_next_token(lexer);
                } else {
                    bool variadic;
                    bool ok;
                    node.parameter_declarations = cn_parse_parameter_declarations(lexer, &variadic, &ok);
                    if (!ok) goto error;
                    if (variadic) node.flags |= CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC;
                }
            }

            if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

            // Extending source loc to highlight whole function '(...)' declarator.
            node.src.length = cn_source_dist(&node.src, &cn_lexer_token(lexer).src);

            node.direct_declarator = direct_declarator;

            direct_declarator = cn_ast_new(node);
            continue;
        }

        // No postfix case.
        return direct_declarator;
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Identifier *cn_parse_identifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Identifier node = { .kind = CN_AST_IDENTIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected identifier token.");
        goto error;
    }
    
    node.name = cn_source_to_str(&cn_lexer_token(lexer).src);

    cn_lexer_next_token(lexer);

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Integer *cn_parse_integer(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Integer node = { .kind = CN_AST_INTEGER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_INTEGER_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected integer token.");
        goto error;
    }

    bool ok = cn_parse_int_literal(cn_source_to_str(&cn_lexer_token(lexer).src), &node.value);
    if (!ok) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Invalid integer literal.");
        goto error;
    }

    cn_lexer_next_token(lexer);

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Float *cn_parse_float(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Float node = { .kind = CN_AST_FLOAT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_FLOAT_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected or unsupported float token.");
        goto error;
    }

    bool ok = cn_parse_float_literal(cn_source_to_str(&cn_lexer_token(lexer).src), &node.value);
    if (!ok) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Invalid or unsupported float literal.");
        goto error;
    }

    cn_lexer_next_token(lexer);

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Char *cn_parse_character(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Char node = { .kind = CN_AST_CHAR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_CHAR_VALUE)) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected char token.");
        goto error;
    }

    bool ok = cn_parse_char_literal(cn_source_to_str(&cn_lexer_token(lexer).src), &node.value, &node.unit_count);
    if (!ok) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Invalid or unsupported char literal.");
        goto error;
    }

    cn_lexer_next_token(lexer);

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_String *cn_parse_string(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_String node = { .kind = CN_AST_STRING, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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
            // TODO: Parse string here.
            cn_sb_append_str(&sb, str);
        }

        cn_lexer_next_token(lexer);
    } while (cn_lexer_expect(lexer, CN_TOKEN_STRING));

    node.str = cn_sb_to_str(&sb);

    // AI: Generated.
    // Empty string literal "" has nothing to copy, and arena doesn't allow zero sized allocations.
    if (node.str.length > 0) {
        void *d = cn_chained_arena_alloc(&cn__tu_data->permanent_strings_arena, node.str.length);
        cn_str_copy_to(node.str, d);
        node.str.data = d;
    } else {
        node.str = (Cn_String) {0};
    }

    cn_sb_free(&sb);

    return cn_ast_new(node);

error:
    CN__TRACE_ERROR
    cn_sb_free(&sb);
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Type_Specifier_Primitive *cn__parse_primitive_info_to_type_specifier(Cn_Primitive_Type_Info *info, Cn_Location *loc, Cn_Source *src) {
    // Implicit int case - if width or sign specified but no kind.
    if (info->kind == CN_AST_TYPE_NONE) {
        info->kind = CN_AST_TYPE_INT;
    }

    if (info->width != CN_AST_TYPE_WIDTH_NONE && info->kind != CN_AST_TYPE_INT && info->kind != CN_AST_TYPE_DOUBLE) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, loc, src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type width on non 'int' or non 'double' type.");
        goto error;
    }

    if (info->sign != CN_AST_TYPE_SIGN_NONE && info->kind != CN_AST_TYPE_INT && info->kind != CN_AST_TYPE_CHAR) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, loc, src, CN_DC_INVALID_TYPE_SPECIFIER, "Specified type sign on non 'int' or 'char' type.");
        goto error;
    }

    Cn_Ast_Type_Specifier_Primitive ts_node = {
        .kind = CN_AST_TYPE_SPECIFIER_PRIMITIVE,
        .loc = *loc,
        .src = *src,
        .primitive_kind = info->kind,
        .width = info->width,
        .sign = info->sign,
    };

    return cn_ast_new(ts_node);

error:
    CN__TRACE_ERROR
    return NULL;
}

CNDEF Cn_Ast_Declaration_Specifiers *cn_parse_declaration_specifiers(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Declaration_Specifiers node = { .kind = CN_AST_DECLARATION_SPECIFIERS, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // For accumulating primitive type info (int, char, signed, unsigned, long, short, etc.)
    Cn_Primitive_Type_Info primitive_info = {0};
#ifdef CN_GNU
    int64_t attribute_mark = cn_ast_stack_mark();
#endif

    bool at_least_one = false;
    int ok;
    while (true) {
        ok = cn_parse_try_storage_specifier(lexer, &node.storage_specifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        ok = cn_parse_try_qualifier(lexer, &node.qualifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        ok = cn_parse_try_function_specifier(lexer, &node.function_specifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

#ifdef CN_GNU
        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            Cn_Ast_Attribute_Specifier *specifier = cn_parse_gnu_attribute_specifier(lexer);
            if (specifier == NULL) goto error;
            cn_ast_stack_push(specifier);
            at_least_one = true;
            continue;
        }
#endif

        ok = cn_parse_try_type_specifier(lexer, &primitive_info, &node.type_specifier);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        break;
    }

#ifdef CN_GNU
    node.attribute_specifiers = cn_ast_stack_finalize(attribute_mark);
#endif

    // If at least one other declaration specifier present continue, if not error.
    if (!at_least_one) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MISSING_DECLARATION_SPECIFIER, "At least one declaration specifier should be present.");
        goto error;
    }

    // If we have primitive type info, create the node now.
    // Otherwise type_specifier was already set by cn_parse_try_type_specifier.
    if (primitive_info.kind != CN_AST_TYPE_NONE || primitive_info.width != CN_AST_TYPE_WIDTH_NONE || primitive_info.sign != CN_AST_TYPE_SIGN_NONE) {
        node.type_specifier = (Cn_Ast_Node *) cn__parse_primitive_info_to_type_specifier(&primitive_info, &node.loc, &node.src);
    }

    Cn_Ast_Declaration_Specifiers *idx = cn_ast_new(node);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF int cn_parse_try_storage_specifier(Cn_Lexer *lexer, Cn_Ast_Storage_Specifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    switch(cn_lexer_token(lexer).kind) {
        case CN_TOKEN_STATIC:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_AST_STORAGE_SPECIFIER_STATIC;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_EXTERN:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_AST_STORAGE_SPECIFIER_EXTERN;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_REGISTER:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_AST_STORAGE_SPECIFIER_REGISTER;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_AUTO:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_AST_STORAGE_SPECIFIER_AUTO;

                cn_lexer_next_token(lexer);
                return 0;
            }
        case CN_TOKEN_TYPEDEF:
            {
                if (*output != 0) goto error_multiple_storage_specifier;
                *output |= CN_AST_STORAGE_SPECIFIER_TYPEDEF;

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

CNDEF int cn_parse_try_qualifier(Cn_Lexer *lexer, Cn_Ast_Qualifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    switch(cn_lexer_token(lexer).kind) {
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
        // case CN_TOKEN_ATOMIC:
        //     {
        //         // Case where it is atomic specifier is not handled here, if such case appears just skip.
        //         if (cn_lexer_peek(lexer, 1).kind == CN_TOKEN_PARAN_OPEN) return 1;

        //         cn_lexer_next_token(lexer);
        //         *output |= CN_AST_TYPE_QUALIFIER_ATOMIC;
        //         return 0;
        //     }
        default: 
            return 1;
    }

    *lexer = original_state;
    return 2;
}

CNDEF int cn_parse_try_function_specifier(Cn_Lexer *lexer, Cn_Ast_Function_Specifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    switch(cn_lexer_token(lexer).kind) {
        case CN_TOKEN_INLINE:
            {
                cn_lexer_next_token(lexer);
                *output |= CN_AST_FUNCTION_SPECIFIER_INLINE;
                return 0;
            }
        // case CN_TOKEN_NORETURN:
        //     {
        //         cn_lexer_next_token(lexer);
        //         *output |= CN_AST_FUNCTION_SPECIFIER_NORETURN;
        //         return 0;
        //     }
        default: 
            return 1;
    }

    *lexer = original_state;
    return 2;
}

CNDEF int cn_parse_try_type_specifier(Cn_Lexer *lexer, Cn_Primitive_Type_Info *primitive_info, Cn_Ast_Node **type_specifier) {
    Cn_Lexer original_state = *lexer;

    // Checking if token is primitive type kind.
    for (Cn_Ast_Type_Kind kind = CN_AST_TYPE_INT; kind < CN_ARRAY_LENGTH(CN_AST_TYPE_KINDS); kind++) {
        if (cn_lexer_expect(lexer, CN_AST_TYPE_KINDS[kind])) {
            if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier != NULL) {
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
        Cn_Binding_Idx *ref = cn_hash_table_get(&cn__tu_data->symbol_binding_table, &str);

        if (ref != NULL && cn__tu_data->binding_list[*ref].kind == CN_BINDING_TYPEDEF) {
            if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier != NULL) {
                cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
                goto error;
            }

            Cn_Ast_Type_Specifier_Typedef node = {
                .kind = CN_AST_TYPE_SPECIFIER_TYPEDEF,
                .loc = cn_lexer_token(lexer).loc,
                .src = cn_lexer_token(lexer).src,
                .typedef_name = str,
            };

            cn_lexer_next_token(lexer);
            *type_specifier = cn_ast_new(node);
            return 0;
        }
    }

    // Checking if token is struct or union.
    if (cn_lexer_expect(lexer, CN_TOKEN_STRUCT) || cn_lexer_expect(lexer, CN_TOKEN_UNION)) {
        if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier != NULL) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
            goto error;
        }

        Cn_Ast_Node *struct_or_union = cn_parse_struct_or_union_specifier(lexer);
        if (struct_or_union == NULL) goto error;

        *type_specifier = struct_or_union;
        return 0;
    }

    // Checking if token is enum.
    if (cn_lexer_expect(lexer, CN_TOKEN_ENUM)) {
        if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier != NULL) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
            goto error;
        }

        Cn_Ast_Node *enum_node = cn_parse_enum_specifier(lexer);
        if (enum_node == NULL) goto error;

        *type_specifier = enum_node;
        return 0;
    }

#ifdef CN_GNU
    // Checking if token is GNU typeof.
    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)) {
        if (primitive_info->kind != CN_AST_TYPE_NONE || *type_specifier != NULL) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_SPECIFIER, "Only single type specifier kind is allowed.");
            goto error;
        }

        Cn_Ast_Node *typeof_node = cn_ast_upcast(cn_parse_gnu_typeof(lexer));
        if (typeof_node == NULL) goto error;

        *type_specifier = typeof_node;
        return 0;
    }
#endif

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

    // TODO: Not implemented yet.
    // Checking if token(s) atomic_type_specifier.

    return 1;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Type_Name *cn_parse_type_name(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Type_Name node = { .kind = CN_AST_TYPE_NAME, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    Cn_Ast_Specifier_Qualifier *specifier_qualifier = cn_parse_specifier_qualifier(lexer);
    if (specifier_qualifier == NULL) goto error;
    node.specifier_qualifier = specifier_qualifier;

    bool ok;
    Cn_Ast_Declarator *abstract_declarator = cn_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    if (abstract_declarator != NULL) {
        if (!(abstract_declarator->flags & CN_AST_DECLARATOR_IS_ABSTRACT)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_INVALID_TYPE_NAME, "Only abstract declarator is allowed in type name.");
            goto error;
        }
    }

    node.abstract_declarator = abstract_declarator;

    Cn_Ast_Type_Name *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Specifier_Qualifier *cn_parse_specifier_qualifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Specifier_Qualifier node = { .kind = CN_AST_SPECIFIER_QUALIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // For accumulating primitive type info.
    Cn_Primitive_Type_Info primitive_info = {0};

#ifdef CN_GNU
    int64_t attribute_mark = cn_ast_stack_mark();
#endif

    bool at_least_one = false;
    int ok;
    while (true) {
        ok = cn_parse_try_qualifier(lexer, &node.qualifiers);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

#ifdef CN_GNU
        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            Cn_Ast_Attribute_Specifier *specifier = cn_parse_gnu_attribute_specifier(lexer);
            if (specifier == NULL) goto error;
            cn_ast_stack_push(specifier);
            continue;
        }
#endif

        ok = cn_parse_try_type_specifier(lexer, &primitive_info, &node.type_specifier);
        if (ok == 0) {
            at_least_one = true;
            continue;
        }
        if (ok == 2)
            goto error;

        break;
    }

#ifdef CN_GNU
    node.attribute_specifiers = cn_ast_stack_finalize(attribute_mark);
#endif

    // If at least one specifier/qualifier present continue, if not error.
    if (!at_least_one) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_MISSING_DECLARATION_SPECIFIER, "At least one specifier qualifier should be present.");
        goto error;
    }

    // If we have primitive type info, create the node now.
    if (primitive_info.kind != CN_AST_TYPE_NONE || primitive_info.width != CN_AST_TYPE_WIDTH_NONE || primitive_info.sign != CN_AST_TYPE_SIGN_NONE) {
        node.type_specifier = (Cn_Ast_Node *) cn__parse_primitive_info_to_type_specifier(&primitive_info, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src);
    }
    Cn_Ast_Specifier_Qualifier *idx = cn_ast_new(node);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_parameter_declarations(Cn_Lexer *lexer, bool *variadic, bool *ok) {
    Cn_Lexer original_state = *lexer;

    *variadic = false;
    int64_t mark = cn_ast_stack_mark();

    // Pushing new scope, this is known as parameter scope in C.
    // It contains parameters as they are declared. After all parameters are parsed it is popped.
    cn_scope_stack_push();

    while (true) {
        // Checking if '...'.
        if (cn_lexer_expect(lexer, CN_TOKEN_ELLIPSIS)) {
            *variadic = true;
            cn_lexer_next_token(lexer);
            break;
        }

        Cn_Ast_Parameter_Declaration *parameter_declaration = cn_parse_parameter_declaration(lexer);
        if (parameter_declaration == NULL) goto error;

        cn_ast_stack_push(parameter_declaration);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;

        cn_lexer_next_token(lexer);
    }

    // Popping parameter scope.
    cn_scope_stack_pop();

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Parameter_Declaration *cn_parse_parameter_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Parameter_Declaration node = { .kind = CN_AST_PARAMETER_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    node.declaration_specifiers = cn_parse_declaration_specifiers(lexer);
    if (node.declaration_specifiers == NULL) goto error;

    bool ok;

    node.declarator = cn_parse_declarator(lexer, &ok);
    if (!ok) goto error;

    ok = cn_analyze_declaration(node.declaration_specifiers, .declarator = node.declarator, .flags = CN_ANALYZE_APPLY_PARAMETER_ADJUSTMENT);
    if (!ok) goto error;

    Cn_Ast_Parameter_Declaration *idx = cn_ast_new(node);
    return idx;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_struct_or_union_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    // Cn_Ast_Struct_Specifier and Cn_Ast_Union_Specifier have identical layouts,
    // only 'kind' tells them apart, so one local serves for both.
    Cn_Ast_Struct_Specifier node = { .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    switch (cn_lexer_token(lexer).kind) {
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

#ifdef CN_GNU
    node.attribute_specifiers = cn_parse_mixed_attribute_specifiers(lexer, &ok);
#else
    node.attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
#endif
    if (!ok) goto error;

    // Parse tag identifier.
    if (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer); 
        if (identifier == NULL) goto error;
        node.identifier = identifier;
    } else {
        // Generating unique tag name for anonymous struct or union.
        Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);

        if (node.kind == CN_AST_STRUCT_SPECIFIER)
            cn_sb_append_format(&sb, "cn__struct_%lu", cn_ast_counter_next());
        else 
            cn_sb_append_format(&sb, "cn__union_%lu", cn_ast_counter_next());

        Cn_String tag = cn_sb_to_str(&sb);

        void *data = cn_chained_arena_alloc(&cn__tu_data->permanent_strings_arena, tag.length);
        cn_str_copy_to(tag, data);
        tag.data = data;

        Cn_Ast_Node *identifier = cn_ast_new((Cn_Ast_Identifier) {
                    .kind = CN_AST_IDENTIFIER,
                    .loc = cn_lexer_token(lexer).loc,
                    .name = tag,
                });

        node.identifier = cn_ast_as(Identifier, identifier);

        cn_sb_free(&sb);
    }

    // Parsing struct or union body.
    int64_t mark = cn_ast_stack_mark();
    
    if (cn_lexer_expect(lexer, CN_TOKEN_CURLY_OPEN)) {
        cn_lexer_next_token(lexer);

        while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            Cn_Ast_Member_Declaration *member_declaration = cn_parse_member_declaration(lexer);
            if (member_declaration == NULL) goto error;
            cn_ast_stack_push(member_declaration);

        }
        cn_lexer_next_token(lexer);

        node.member_declarations = cn_ast_stack_finalize(mark);

        node.kind == CN_AST_STRUCT_SPECIFIER ? 
            (node.flags |= CN_AST_STRUCT_HAS_DEFINITION) : 
            (node.flags |= CN_AST_UNION_HAS_DEFINITION);
    }

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Member_Declaration *cn_parse_member_declaration(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Member_Declaration node = { .kind = CN_AST_MEMBER_DECLARATION, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    bool ok;

#ifdef CN_GNU
    if (cn__parse_gnu_extension(lexer)) node.flags |= CN_AST_GNU_EXTENSION;
#endif

    node.attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
    if (!ok) goto error;

    Cn_Ast_Specifier_Qualifier *specifier_qualifier = cn_parse_specifier_qualifier(lexer);
    if (specifier_qualifier == NULL) goto error;
    node.specifier_qualifier = specifier_qualifier;

    node.member_declarators = cn_parse_member_declarators(lexer, &ok);
    if (!ok) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Member_Declaration *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_member_declarators(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    while (true) {
        Cn_Ast_Member_Declarator *member_declarator = cn_parse_member_declarator(lexer);
        if (member_declarator == NULL) goto error;
        cn_ast_stack_push(member_declarator);

        if (!cn_lexer_expect(lexer, CN_TOKEN_COMMA)) break;
        cn_lexer_next_token(lexer);
    }
    
    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *ok = false;
    *lexer = original_state;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Member_Declarator *cn_parse_member_declarator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Member_Declarator node = { .kind = CN_AST_MEMBER_DECLARATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
        bool ok;
        Cn_Ast_Declarator *declarator = cn_parse_declarator(lexer, &ok);
        if (!ok) goto error;

        if (declarator != NULL && declarator->flags & CN_AST_DECLARATOR_IS_ABSTRACT) {
            cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, declarator, CN_DC_INVALID_DECLARATOR, "Expected non-abstract declarator inside member declarator.");
            goto error;
        }

        node.declarator = declarator;
    }

    if (cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
        cn_lexer_next_token(lexer);

        Cn_Ast_Node *expression = cn_parse_expression(lexer, 0, CN_NO_COMMA_OPERATOR);
        if (expression == NULL) goto error;
        node.bitfield = expression;
    }

#ifdef CN_GNU
    bool ok_attributes;
    node.attribute_specifiers = cn_parse_gnu_attribute_specifiers(lexer, &ok_attributes);
    if (!ok_attributes) goto error;
#endif

    Cn_Ast_Member_Declarator *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Node *cn_parse_enum_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Enum_Specifier node = { .kind = CN_AST_ENUM_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_ENUM)) goto error;
    
    bool ok;

#ifdef CN_GNU
    node.attribute_specifiers = cn_parse_mixed_attribute_specifiers(lexer, &ok);
#else
    node.attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
#endif
    if (!ok) goto error;

    bool is_enum_definition_optional;
    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        is_enum_definition_optional = true;

        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
        if (identifier == NULL) goto error;
        node.identifier = identifier;
    } else {
         is_enum_definition_optional = false;

         // Generating unique tag name for anonymous enum.
         Cn_String_Builder sb = cn_sb_make(CN_SB_STACK_STORAGE_CAP);

         cn_sb_append_format(&sb, "cn__enum_%lu", cn_ast_counter_next());

         Cn_String tag = cn_sb_to_str(&sb);

         void *data = cn_chained_arena_alloc(&cn__tu_data->permanent_strings_arena, tag.length);
         cn_str_copy_to(tag, data);
         tag.data = data;

         Cn_Ast_Node *identifier = cn_ast_new((Cn_Ast_Identifier) {
                 .kind = CN_AST_IDENTIFIER,
                 .loc = cn_lexer_token(lexer).loc,
                 .name = tag,
                 });

         node.identifier = cn_ast_as(Identifier, identifier);

         cn_sb_free(&sb);
    }

    if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
        Cn_Ast_Specifier_Qualifier *specifier_qualifier = cn_parse_specifier_qualifier(lexer);
        if (specifier_qualifier == NULL) goto error;
        node.specifier_qualifier = specifier_qualifier;
    }


    if (cn_parse_optional(lexer, CN_TOKEN_CURLY_OPEN)) {
        node.flags |= CN_AST_ENUM_HAS_DEFINITION;

        int64_t mark = cn_ast_stack_mark();

        Cn_Ast_Node *enumerator_node;
        while (!cn_lexer_expect(lexer, CN_TOKEN_CURLY_CLOSE)) {
            enumerator_node = cn_ast_upcast(cn_parse_enumerator(lexer));
            if (enumerator_node == NULL) goto error;
            cn_ast_stack_push(enumerator_node);

            if (!cn_parse_optional(lexer, CN_TOKEN_COMMA)) break;
        }

        node.enumerators = cn_ast_stack_finalize(mark);
        
        if (!cn_parse_expect(lexer, CN_TOKEN_CURLY_CLOSE)) goto error;
    } else if (!is_enum_definition_optional) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected enum definition '{' in enum specifier.");
        goto error;
    }

    Cn_Ast_Node *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Enumerator *cn_parse_enumerator(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Enumerator node = { .kind = CN_AST_ENUMERATOR, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (cn_lexer_expect(lexer, CN_TOKEN_IDENTIFIER)) {
        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
        if (identifier == NULL) goto error;
        node.identifier = identifier;
    }

    bool ok;

#ifdef CN_GNU
    node.attribute_specifiers = cn_parse_mixed_attribute_specifiers(lexer, &ok);
#else
    node.attribute_specifiers = cn_parse_attribute_specifiers(lexer, &ok);
#endif
    if (!ok) goto error;

    if (cn_parse_optional(lexer, CN_TOKEN_ASSIGN)) {
        Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, CN_NO_COMMA_OPERATOR);
        if (expression == NULL) goto error;
        node.expression = expression;
    }

    Cn_Ast_Enumerator *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_attribute_specifiers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    while (true) {
        if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN) || cn_lexer_peek(lexer, 1).kind != CN_TOKEN_SQR_BRACES_OPEN) {
            *ok = true;
            return cn_ast_stack_finalize(mark);
        }

        Cn_Ast_Attribute_Specifier *attribute_specifier = cn_parse_attribute_specifier(lexer);
        if (attribute_specifier == NULL) goto error;
        cn_ast_stack_push(attribute_specifier);
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Attribute_Specifier *cn_parse_attribute_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Attribute_Specifier node = { .kind = CN_AST_ATTRIBUTE_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN)) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) {
        bool ok;
        node.attributes = cn_parse_attributes(lexer, &ok); 
        if (!ok) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;

    Cn_Ast_Attribute_Specifier *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_attributes(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    Cn_Ast_Attribute *attribute = cn_parse_attribute(lexer);
    if (attribute == NULL) goto error;
    cn_ast_stack_push(attribute);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        attribute = cn_parse_attribute(lexer);
        if (attribute == NULL) goto error;
        cn_ast_stack_push(attribute);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Attribute *cn_parse_attribute(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Attribute node = { .kind = CN_AST_ATTRIBUTE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    // Extracting vendor identifier.
    if (cn_lexer_peek(lexer, 1).kind == CN_TOKEN_COLON && cn_lexer_peek(lexer, 2).kind == CN_TOKEN_COLON) {
        Cn_Ast_Identifier *vendor_identifier = cn_parse_identifier(lexer);
        if (vendor_identifier == NULL) goto error;
        node.vendor_identifier = vendor_identifier;

        cn_lexer_next_token(lexer);
        cn_lexer_next_token(lexer);
    }

    Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
    if (identifier == NULL) goto error;
    node.identifier = identifier;

    if (cn_parse_optional(lexer, CN_TOKEN_PARAN_OPEN)) {
        bool ok;
        node.arguments = cn_parse_arguments(lexer, &ok);
        if (!ok) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    }

    Cn_Ast_Attribute *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

#ifdef CN_GNU
CNDEF bool cn__parse_gnu_extension(Cn_Lexer *lexer) {
    bool found = false;
    while (cn_parse_optional(lexer, CN_TOKEN_GNU_EXTENSION)) found = true;
    return found;
}

CNDEF Cn_Ast_Gnu_Typeof *cn_parse_gnu_typeof(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;
    Cn_Ast_Gnu_Typeof node = { .kind = CN_AST_GNU_TYPEOF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_TYPEOF)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Node *child;

    if (cn_parse_starts_type(lexer)) {
        child = cn_ast_upcast(cn_parse_type_name(lexer));
    } else {
        child = cn_parse_expression(lexer, 0, 0);

        // Typechecking expression right after parsing.
        if (child != NULL && cn_analyze_typecheck_expression(child) == NULL) goto error;
    }

    if (child == NULL) goto error;
    node.target = child;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Typeof *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_attribute_specifiers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    while (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
        Cn_Ast_Attribute_Specifier *attribute_specifier = cn_parse_gnu_attribute_specifier(lexer);
        if (attribute_specifier == NULL) goto error;
        cn_ast_stack_push(attribute_specifier);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Nodes cn_parse_mixed_attribute_specifiers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    while (true) {
        Cn_Ast_Attribute_Specifier *attribute_specifier;

        if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            attribute_specifier = cn_parse_gnu_attribute_specifier(lexer);
        } else if (cn_lexer_expect(lexer, CN_TOKEN_SQR_BRACES_OPEN) && cn_lexer_peek(lexer, 1).kind == CN_TOKEN_SQR_BRACES_OPEN) {
            attribute_specifier = cn_parse_attribute_specifier(lexer);
        } else {
            break;
        }

        if (attribute_specifier == NULL) goto error;
        cn_ast_stack_push(attribute_specifier);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Attribute_Specifier *cn_parse_gnu_attribute_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Attribute_Specifier node = { .kind = CN_AST_ATTRIBUTE_SPECIFIER, .flags = CN_AST_ATTRIBUTE_SPECIFIER_IS_GNU, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        bool ok;
        node.attributes = cn_parse_attributes(lexer, &ok);
        if (!ok) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Attribute_Specifier *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Gnu_Asm_Statement *cn_parse_gnu_asm_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Statement node = { .kind = CN_AST_GNU_ASM_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ASM)) goto error;

    // Qualifiers, in any order, each at most once.
    while (true) {
        int result = cn_parse_try_gnu_asm_qualifier(lexer, &node.qualifiers);
        if (result == 1) break;
        if (result == 2) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_String *template = cn_parse_string(lexer);
    if (template == NULL) goto error;
    node.template = template;

    // Sections, each one is only allowed after the previous one.
    bool has_goto_labels = false;
    bool ok;

    if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
        node.flags |= CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED;

        node.outputs = cn_parse_gnu_asm_operands(lexer, &ok);
        if (!ok) goto error;

        if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
            node.inputs = cn_parse_gnu_asm_operands(lexer, &ok);
            if (!ok) goto error;

            if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
                node.clobbers = cn_parse_gnu_asm_clobbers(lexer, &ok);
                if (!ok) goto error;

                if (cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
                    if (!(node.qualifiers & CN_AST_GNU_ASM_QUALIFIER_GOTO)) {
                        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Goto labels section is not allowed without 'goto' asm qualifier.");
                        goto error;
                    }
                    cn_lexer_next_token(lexer);

                    node.goto_labels = cn_parse_gnu_asm_goto_labels(lexer, &ok);
                    if (!ok) goto error;
                    has_goto_labels = true;
                }
            }
        }
    }

    if (node.qualifiers & CN_AST_GNU_ASM_QUALIFIER_GOTO) {
        if (!(node.flags & CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &node.loc, &node.src, CN_DC_UNEXPECTED_TOKEN, "'goto' asm qualifier is not allowed in basic asm.");
            goto error;
        }
        if (!has_goto_labels) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected goto labels section in 'asm goto'.");
            goto error;
        }
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Gnu_Asm_Statement *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF int cn_parse_try_gnu_asm_qualifier(Cn_Lexer *lexer, Cn_Ast_Gnu_Asm_Qualifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Qualifier_Flags qualifier;

    switch(cn_lexer_token(lexer).kind) {
        case CN_TOKEN_VOLATILE: qualifier = CN_AST_GNU_ASM_QUALIFIER_VOLATILE; break;
        case CN_TOKEN_INLINE:   qualifier = CN_AST_GNU_ASM_QUALIFIER_INLINE;   break;
        case CN_TOKEN_GOTO:     qualifier = CN_AST_GNU_ASM_QUALIFIER_GOTO;     break;
        default:
            return 1;
    }

    if (*output & qualifier) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Duplicate '%s' asm qualifier.", cn_token_kind_name(cn_lexer_token(lexer).kind));
        goto error;
    }

    *output |= qualifier;
    cn_lexer_next_token(lexer);
    return 0;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_operands(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    // Empty operand list.
    if (cn_lexer_expect(lexer, CN_TOKEN_COLON) || cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        *ok = true;
        return cn_ast_stack_finalize(mark);
    }

    Cn_Ast_Gnu_Asm_Operand *operand = cn_parse_gnu_asm_operand(lexer);
    if (operand == NULL) goto error;
    cn_ast_stack_push(operand);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        operand = cn_parse_gnu_asm_operand(lexer);
        if (operand == NULL) goto error;
        cn_ast_stack_push(operand);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Gnu_Asm_Operand *cn_parse_gnu_asm_operand(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Operand node = { .kind = CN_AST_GNU_ASM_OPERAND, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // Optional symbolic name.
    if (cn_parse_optional(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
        if (identifier == NULL) goto error;
        node.identifier = identifier;

        if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;
    }

    // Constraint.
    Cn_Ast_String *string = cn_parse_string(lexer);
    if (string == NULL) goto error;
    node.string = string;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
    if (expression == NULL) goto error;
    node.expression = expression;

    // Typechecking expression right after parsing.
    if (cn_analyze_typecheck_expression(expression) == NULL) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Asm_Operand *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_clobbers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    // Empty clobber list.
    if (cn_lexer_expect(lexer, CN_TOKEN_COLON) || cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        *ok = true;
        return cn_ast_stack_finalize(mark);
    }

    Cn_Ast_String *clobber = cn_parse_string(lexer);
    if (clobber == NULL) goto error;
    cn_ast_stack_push(clobber);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        clobber = cn_parse_string(lexer);
        if (clobber == NULL) goto error;
        cn_ast_stack_push(clobber);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_goto_labels(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    Cn_Ast_Identifier *label = cn_parse_identifier(lexer);
    if (label == NULL) goto error;
    cn_ast_stack_push(label);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        label = cn_parse_identifier(lexer);
        if (label == NULL) goto error;
        cn_ast_stack_push(label);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Gnu_Asm_Label *cn_parse_gnu_asm_label(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Label node = { .kind = CN_AST_GNU_ASM_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ASM)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_STRING)) {
        Cn_Ast_String *string = cn_parse_string(lexer);
        if (string == NULL) goto error;
        node.string = string;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Asm_Label *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}
#endif // CN_GNU

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

CNDEF bool cn_parse_char_literal(Cn_String str, uint64_t *out, int *unit_count_out) {
    CN_ASSERT(str.length >= 2 && str.data[0] == '\'' && str.data[str.length - 1] == '\'');

    const char *p   = str.data + 1;              // skip opening '
    const char *end = str.data + str.length - 1; // drop closing '

    if (p >= end) return false; // empty character constant: ''

    uint64_t result = 0;
    int unit_count = 0;

    while (p < end) {
        uint8_t bytes[4]; // units produced by this step (max 4 for a UTF-8 encoded \U)
        int nbytes = 0;

        unsigned char c = (unsigned char)*p++;

        if (c == '\n' || c == '\'') {
            return false; // unescaped newline or quote inside the literal
        }

        if (c != '\\') {
            // Source and execution charset are both UTF-8, so source bytes pass
            // through unchanged. A multi-byte character like 'é' becomes 2 units.
            bytes[nbytes++] = c;
        } else {
            if (p == end) return false; // backslash right before the closing quote
            c = (unsigned char)*p++;

            switch (c) {
            case '\'': bytes[nbytes++] = '\''; break;
            case '"':  bytes[nbytes++] = '"';  break;
            case '?':  bytes[nbytes++] = '?';  break;
            case '\\': bytes[nbytes++] = '\\'; break;
            case 'a':  bytes[nbytes++] = '\a'; break;
            case 'b':  bytes[nbytes++] = '\b'; break;
            case 'f':  bytes[nbytes++] = '\f'; break;
            case 'n':  bytes[nbytes++] = '\n'; break;
            case 'r':  bytes[nbytes++] = '\r'; break;
            case 't':  bytes[nbytes++] = '\t'; break;
            case 'v':  bytes[nbytes++] = '\v'; break;
            case 'e':
            case 'E':  bytes[nbytes++] = 0x1B; break; // GNU extension: ESC

            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7': {
                // Octal: 1 to 3 digits, the first one already consumed. Raw unit.
                uint32_t v = (uint32_t)(c - '0');
                for (int i = 1; i < 3 && p < end && *p >= '0' && *p <= '7'; i++) {
                    v = v * 8 + (uint32_t)(*p++ - '0');
                }
                if (v > 0xFF) return false; // e.g. '\777' does not fit in a char
                bytes[nbytes++] = (uint8_t)v;
            } break;

            case 'x': {
                // Hex: one or more digits, no upper limit on count. Raw unit.
                uint32_t v = 0;
                int digits = 0;
                bool overflow = false;
                while (p < end) {
                    char h = *p;
                    uint32_t d;
                    if      (h >= '0' && h <= '9') d = (uint32_t)(h - '0');
                    else if (h >= 'a' && h <= 'f') d = (uint32_t)(h - 'a' + 10);
                    else if (h >= 'A' && h <= 'F') d = (uint32_t)(h - 'A' + 10);
                    else break;
                    p++;
                    digits++;
                    if (!overflow) {
                        v = v * 16 + d;
                        if (v > 0xFF) overflow = true;
                    }
                }
                if (digits == 0) return false; // '\x' with no digits
                if (overflow) return false;    // e.g. '\x100' does not fit in a char
                bytes[nbytes++] = (uint8_t)v;
            } break;

            case 'u':
            case 'U': {
                // Universal character name: exactly 4 or 8 hex digits.
                // Gives a code point, which is then encoded to UTF-8.
                int ndigits = (c == 'u') ? 4 : 8;
                uint32_t cp = 0;
                for (int i = 0; i < ndigits; i++) {
                    if (p == end) return false; // too few digits
                    char h = *p++;
                    uint32_t d;
                    if      (h >= '0' && h <= '9') d = (uint32_t)(h - '0');
                    else if (h >= 'a' && h <= 'f') d = (uint32_t)(h - 'a' + 10);
                    else if (h >= 'A' && h <= 'F') d = (uint32_t)(h - 'A' + 10);
                    else return false; // too few digits
                    cp = cp * 16 + d;
                }

                if (cp > 0x10FFFF) return false;                // not a Unicode code point
                if (cp >= 0xD800 && cp <= 0xDFFF) return false; // surrogate
                if (cp < 0xA0 && cp != 0x24 && cp != 0x40 && cp != 0x60) {
                    return false; // basic chars must not be spelled as UCNs (except $ @ `)
                }

                if (cp < 0x80) {
                    bytes[nbytes++] = (uint8_t)cp;
                } else if (cp < 0x800) {
                    bytes[nbytes++] = (uint8_t)(0xC0 | (cp >> 6));
                    bytes[nbytes++] = (uint8_t)(0x80 | (cp & 0x3F));
                } else if (cp < 0x10000) {
                    bytes[nbytes++] = (uint8_t)(0xE0 | (cp >> 12));
                    bytes[nbytes++] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
                    bytes[nbytes++] = (uint8_t)(0x80 | (cp & 0x3F));
                } else {
                    bytes[nbytes++] = (uint8_t)(0xF0 | (cp >> 18));
                    bytes[nbytes++] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
                    bytes[nbytes++] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
                    bytes[nbytes++] = (uint8_t)(0x80 | (cp & 0x3F));
                }
            } break;

            default:
                return false; // unknown escape (GCC only warns and uses the char itself)
            }
        }

        // Pack units in source order: first unit ends up most significant.
        for (int i = 0; i < nbytes; i++) {
            if (unit_count == 8) return false; // more units than fit in 64 bits
            result = (result << 8) | bytes[i];
            unit_count++;
        }
    }

    *out = result;
    *unit_count_out = unit_count;
    return true;
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Build
 * ============================================
 */

CNDEF Cn_Ast_Node *cn__build_wrap_if_primary(void * idx) {
    if (cn_ast_is_primary(idx)) {
        Cn_Ast_Node *node = idx;
        Cn_Ast_Node *parent = cn_ast_new((Cn_Ast_Primary) { 
                    .kind        = CN_AST_PRIMARY, 
                    .loc         = node->loc, 
                    .src         = node->src, 
                    .literal = (idx), 
                });

        node = idx;
        return parent;
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

CNDEF Cn_Ast_Code *cn__build_format(const char *file, int64_t line, const char *format, ...) {
    Cn_Ast_Code node = {
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
        return NULL;
    }

    void *data = cn_chained_arena_alloc(&cn__tu_data->permanent_strings_arena, needed + 1);
    vsnprintf(data, needed + 1, format, args);
    va_end(args);

    node.text.data = data;
    node.text.length = needed;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Nodes cn_build_list_from(void *ptrs[], int64_t length) {
    int64_t mark = cn_ast_stack_mark();

    for (int64_t i = 0; i < length; i++) {
        cn_ast_stack_push(ptrs[i]);
    }

    return cn_ast_stack_finalize(mark);
}

CNDEF Cn_Ast_Identifier *cn__build_identifier(Cn_String name, Cn_Build_Opt opt) {
    Cn_Ast_Identifier node = {
        .kind = CN_AST_IDENTIFIER,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (opt.alloc) {
        node.name = cn__ast_permanent_save_string(name);
    } else {
        node.name = name;
    }

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Integer *cn__build_integer(uint64_t value, Cn_Build_Opt opt) {
    Cn_Ast_Integer node = {
        .kind = CN_AST_INTEGER,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.value = value;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Float *cn__build_float(double value, Cn_Build_Opt opt) {
    Cn_Ast_Float node = {
        .kind = CN_AST_FLOAT,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.value = value;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_String *cn__build_string(Cn_String str, Cn_Build_Opt opt) {
    Cn_Ast_String node = {
        .kind = CN_AST_STRING,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (opt.alloc) {
        node.str = cn__ast_permanent_save_string(str);
    } else {
        node.str = str;
    }

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Binary *cn__build_binary(Cn_Ast_Binary_Operator_Kind op, void *left, void *right, Cn_Build_Opt opt) {
    Cn_Ast_Binary node = {
        .kind = CN_AST_BINARY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.operator = op;
    node.left = cn__build_wrap_if_primary(left);
    node.right = cn__build_wrap_if_primary(right);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Assign *cn__build_assign(Cn_Ast_Assignment_Operator_Kind op, void *left, void *right, Cn_Build_Opt opt) {
    Cn_Ast_Assign node = {
        .kind = CN_AST_ASSIGN,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.operator = op;
    node.left = cn__build_wrap_if_primary(left);
    node.right = cn__build_wrap_if_primary(right);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Unary *cn__build_unary(Cn_Ast_Unary_Operator_Kind op, void *expression, Cn_Build_Opt opt) {
    Cn_Ast_Unary node = {
        .kind = CN_AST_UNARY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.operator = op;
    node.expression = cn__build_wrap_if_primary(expression);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Call *cn__build_func_call(void *callee, Cn_Ast_Nodes arguments, Cn_Build_Opt opt) {
    Cn_Ast_Call node = {
        .kind = CN_AST_CALL,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.expression = cn__build_wrap_if_primary(callee);

    for (int64_t i = 0; i < arguments.length; i++) {
        arguments.ptrs[i] = cn__build_wrap_if_primary(arguments.ptrs[i]);
    }

    node.arguments = arguments;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Expression_Statement *cn__build_expr_statement(void *expression, Cn_Build_Opt opt) {
    Cn_Ast_Expression_Statement node = {
        .kind = CN_AST_EXPRESSION_STATEMENT,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.expression = cn__build_wrap_if_primary(expression);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Goto *cn__build_goto_statement(Cn_Ast_Identifier *identifier, Cn_Build_Opt opt) {
    Cn_Ast_Goto node = {
        .kind = CN_AST_GOTO,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.identifier = cn_ast_as(Identifier, identifier);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Return *cn__build_return_statement(void *expression, Cn_Build_Opt opt) {
    Cn_Ast_Return node = {
        .kind = CN_AST_RETURN,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.expression = cn__build_wrap_if_primary(expression);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Label *cn__build_label(Cn_Ast_Identifier *identifier, void *statement, Cn_Build_Opt opt) {
    Cn_Ast_Label node = {
        .kind = CN_AST_LABEL,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.identifier = cn_ast_as(Identifier, identifier);
    node.statement = statement;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Block_Item *cn__build_block_item(void *declaration_or_statement, Cn_Build_Opt opt) {
    Cn_Ast_Block_Item node = {
        .kind = CN_AST_BLOCK_ITEM,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.declaration_or_statement = declaration_or_statement;

    return cn_ast_new(node);
}

CNDEF bool cn__build_is_abstract(Cn_Ast_Node *direct_declarator) {
    while (direct_declarator != NULL) {
        Cn_Ast_Node *node = direct_declarator;

        switch (node->kind) {
            case CN_AST_IDENTIFIER:
                return false;
            case CN_AST_DIRECT_DECLARATOR_GROUPED:
                if (cn_ast_as(Direct_Declarator_Grouped, node)->declarator == NULL) return true;
                return ((cn_ast_as(Direct_Declarator_Grouped, node)->declarator)->flags & CN_AST_DECLARATOR_IS_ABSTRACT) != 0;
            case CN_AST_DIRECT_DECLARATOR_ARRAY:
                direct_declarator = cn_ast_as(Direct_Declarator_Array, node)->direct_declarator;
                continue;
            case CN_AST_DIRECT_DECLARATOR_FUNCTION:
                direct_declarator = cn_ast_as(Direct_Declarator_Function, node)->direct_declarator;
                continue;
            default:
                return true;
        }
    }

    return true;
}

CNDEF Cn_Ast_Pointer *cn__build_pointer(Cn_Ast_Qualifier_Flags qualifiers, Cn_Ast_Pointer *pointer, Cn_Build_Opt opt) {
    Cn_Ast_Pointer node = {
        .kind = CN_AST_POINTER,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.qualifiers = qualifiers;
    node.pointer = cn_ast_as(Pointer, pointer);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Direct_Declarator_Grouped *cn__build_direct_declarator_grouped(Cn_Ast_Declarator *declarator, Cn_Build_Opt opt) {
    Cn_Ast_Direct_Declarator_Grouped node = {
        .kind = CN_AST_DIRECT_DECLARATOR_GROUPED,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.declarator = cn_ast_as(Declarator, declarator);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Direct_Declarator_Array *cn__build_direct_declarator_array(void *direct_declarator, void *expression, Cn_Build_Opt opt) {
    Cn_Ast_Direct_Declarator_Array node = {
        .kind = CN_AST_DIRECT_DECLARATOR_ARRAY,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.direct_declarator = direct_declarator;

    if (expression != NULL) {
        node.expression = cn__build_wrap_if_primary(expression);
    }

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Direct_Declarator_Function *cn__build_direct_declarator_function(void *direct_declarator, Cn_Ast_Nodes parameter_declarations, bool variadic, Cn_Build_Opt opt) {
    Cn_Ast_Direct_Declarator_Function node = {
        .kind = CN_AST_DIRECT_DECLARATOR_FUNCTION,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (variadic) node.flags |= CN_AST_DIRECT_DECLARATOR_FUNCTION_IS_VARIADIC;

    node.direct_declarator = direct_declarator;
    node.parameter_declarations = parameter_declarations;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Declarator *cn__build_declarator(Cn_Ast_Pointer *pointer, void *direct_declarator, Cn_Build_Opt opt) {
    Cn_Ast_Declarator node = {
        .kind = CN_AST_DECLARATOR,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (cn__build_is_abstract(direct_declarator)) node.flags |= CN_AST_DECLARATOR_IS_ABSTRACT;

    node.pointer = cn_ast_as(Pointer, pointer);
    node.direct_declarator = direct_declarator;

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Init_Declarator *cn__build_init_declarator(Cn_Ast_Declarator *declarator, Cn_Ast_Initializer *initializer, Cn_Build_Opt opt) {
    Cn_Ast_Init_Declarator node = {
        .kind = CN_AST_INIT_DECLARATOR,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    node.declarator = cn_ast_as(Declarator, declarator);
    node.initializer = cn_ast_as(Initializer, initializer);

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Declaration *cn__build_declaration(Cn_Ast_Declaration_Specifiers *declaration_specifiers, Cn_Ast_Nodes init_declarators, Cn_Build_Opt opt) {
    Cn_Ast_Declaration node = {
        .kind = CN_AST_DECLARATION,
        .flags = CN_AST_SYNTHETIC,
        .loc.file = cn__build_make_location(__FUNCTION__, opt.file, opt.line),
    };

    if (init_declarators.length == 0) node.flags |= CN_AST_DECLARATION_IS_EMPTY;

    node.declaration_specifiers = cn_ast_as(Declaration_Specifiers, declaration_specifiers);
    node.init_declarators = init_declarators;

    return cn_ast_new(node);
}

/**
 * ============================================
 * IMPLEMENTATION SECTION: Translation Unit
 * ============================================
 */

Cn_Tu_Data *cn__tu_data = NULL;

CNDEF void cn__tu_save(Cn_Tu_Saved *state) {
    state->saved_output_length = cn_chained_arena_allocated(&cn__tu_data->output_arena);
    state->saved_ast_length = cn_chained_arena_allocated(&cn__tu_data->ast_arena);
    state->saved_type_length = cn_chained_arena_allocated(&cn__tu_data->type_arena);
    state->saved_type_children_length = cn_chained_arena_allocated(&cn__tu_data->type_children_arena);
    state->saved_binding_length = cn_array_list_length(&cn__tu_data->binding_list);
    state->saved_label_binding_length = cn_array_list_length(&cn__tu_data->label_binding_list);
    state->saved_binding_defined_idx_length = cn_array_list_length(&cn__tu_data->binding_defined_idx_list);
    state->saved_scoped_strings_length = cn_chained_arena_allocated(&cn__tu_data->scoped_strings_arena);
    // checkpoint->saved_permanent_strings_length = cn_chained_arena_allocated(&data->permanent_strings_arena);
    state->saved_scope_length = cn_array_list_length(&cn__tu_data->scope_stack);
    state->saved_function_scope_idx = cn__tu_data->function_scope_idx;
    state->saved_ptr_stack_length = cn_array_list_length(&cn__tu_data->ptr_stack);
    state->saved_counter = cn__tu_data->counter;
}

CNDEF void cn__tu_rollback(Cn_Tu_Saved *state) {
    cn_chained_arena_dealloc(&cn__tu_data->output_arena, cn_chained_arena_allocated(&cn__tu_data->output_arena) - state->saved_output_length);
    cn_chained_arena_dealloc(&cn__tu_data->ast_arena, cn_chained_arena_allocated(&cn__tu_data->ast_arena) - state->saved_ast_length);
    
    // Removing types from hash jset entries.
    int64_t types_count = (cn_chained_arena_allocated(&cn__tu_data->type_arena) - state->saved_type_length) / sizeof(Cn_Type);
    if (types_count > 0) {
        Cn_Chained_Arena *arena = &cn__tu_data->type_arena;
        Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(arena->block);
        Cn_Type *ptr = (Cn_Type *)((uint8_t *)arena->block + header->allocated) - 1;

        while (true) {
            cn_hash_set_remove(&cn__tu_data->type_ptr_set, ptr);
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

        cn_chained_arena_dealloc(&cn__tu_data->type_arena, cn_chained_arena_allocated(&cn__tu_data->type_arena) - state->saved_type_length);
        cn_chained_arena_dealloc(&cn__tu_data->type_children_arena, cn_chained_arena_allocated(&cn__tu_data->type_children_arena) - state->saved_type_children_length);
    }

    // Removing bindings from hash table entries.
    for (int i = state->saved_binding_length; i < cn_array_list_length(&cn__tu_data->binding_list); i++) {
        // Resolve each binding, properly dispose each binding.
        if (cn__tu_data->binding_list[i].kind == CN_BINDING_TAG) {
            if (cn__tu_data->binding_list[i].next_idx == CN_BINDING_NIL_IDX) {
                cn_hash_table_remove(&cn__tu_data->tag_binding_table, &cn__tu_data->binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__tu_data->tag_binding_table, cn__tu_data->binding_list[i].next_idx, &cn__tu_data->binding_list[i].name);
            }
        } else {
            if (cn__tu_data->binding_list[i].next_idx == CN_BINDING_NIL_IDX) {
                cn_hash_table_remove(&cn__tu_data->symbol_binding_table, &cn__tu_data->binding_list[i].name);
            } else {
                cn_hash_table_put(&cn__tu_data->symbol_binding_table, cn__tu_data->binding_list[i].next_idx, &cn__tu_data->binding_list[i].name);
            }
        }
    }
    cn_array_list_pop_multiple(&cn__tu_data->binding_list, cn_array_list_length(&cn__tu_data->binding_list) - state->saved_binding_length);
    
    // Removing label bindings from hash table entries.
    for (int i = state->saved_label_binding_length; i < cn_array_list_length(&cn__tu_data->label_binding_list); i++) {
        // Resolve each binding, properly dispose each binding.
        if (cn__tu_data->label_binding_list[i].next_idx == CN_BINDING_NIL_IDX) {
            cn_hash_table_remove(&cn__tu_data->label_binding_table, &cn__tu_data->label_binding_list[i].name);
        } else {
            cn_hash_table_put(&cn__tu_data->label_binding_table, cn__tu_data->label_binding_list[i].next_idx, &cn__tu_data->label_binding_list[i].name);
        }
    }
    cn_array_list_pop_multiple(&cn__tu_data->label_binding_list, cn_array_list_length(&cn__tu_data->label_binding_list) - state->saved_label_binding_length);

    // Undefining all bindings that contain definition that was made after state was set,
    // but that were declared before checkpint was set.
    for (int64_t i = state->saved_binding_defined_idx_length; i < cn_array_list_length(&cn__tu_data->binding_defined_idx_list); i++) {
        if (cn__tu_data->binding_defined_idx_list[i] < state->saved_binding_length) {
            Cn_Binding *binding = cn_binding_get(cn__tu_data->binding_defined_idx_list[i]);
            
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

    cn_array_list_pop_multiple(&cn__tu_data->binding_defined_idx_list, cn_array_list_length(&cn__tu_data->binding_defined_idx_list) - state->saved_binding_defined_idx_length);
    cn_chained_arena_dealloc(&cn__tu_data->scoped_strings_arena, cn_chained_arena_allocated(&cn__tu_data->scoped_strings_arena) - state->saved_scoped_strings_length);
    // cn_chained_arena_dealloc(&cn__tu_data->permanent_strings_arena, cn_chained_arena_allocated(&cn__tu_data->permanent_strings_arena) - state->saved_permanent_strings_length);
    cn_array_list_pop_multiple(&cn__tu_data->scope_stack, cn_array_list_length(&cn__tu_data->scope_stack) - state->saved_scope_length);
    cn__tu_data->function_scope_idx = state->saved_function_scope_idx;
    cn_array_list_pop_multiple(&cn__tu_data->ptr_stack, cn_array_list_length(&cn__tu_data->ptr_stack) - state->saved_ptr_stack_length);
    cn__tu_data->counter = state->saved_counter;
}

CNDEF Cn_String cn__tu_emit_and_rollback(Cn_Ast_Node *node, Cn_Tu_Saved *state) {
    // Otherwise emitting modified ast and then reparsing it. To send the message again and validate what user did.
    Cn_String_Builder sb = cn_sb_make(512);
    Cn_Emitter emitter = { .write = cn_emit_write_sb, .ctx = &sb };
    cn_emit(&emitter, node);

    // Doing rollback here to simulate reparse from the point before we began anything.
    // we have new code already emmited, now just restoring everything and copyig it into output arena.
    // then just parsing. It is not done before emit, cause emit utilizes ast that might have been using data
    // stored in output arena.
    cn__tu_rollback(state);

    Cn_String code = CN_STR(sb.length, cn_chained_arena_alloc(&cn__tu_data->output_arena, sb.length));
    cn_str_copy_to(cn_sb_to_str(&sb), code.data);
    cn_sb_free(&sb);

    return code;
}

CNDEF bool cn__tu_data_init(Cn_Tu_Data *data) {
    data->ast_arena = cn_chained_arena_make(CN_AST_ARENA_BLOCK_CAP);
    data->output_arena = cn_chained_arena_make(4096);
    data->type_arena = cn_chained_arena_make(CN_AST_TYPE_ARENA_BLOCK_CAP);
    data->type_ptr_set = cn_hash_set_make(Cn_Type *, CN_AST_TYPE_PTR_SET_INITIAL_CAP, (Cn_Hash_Function *)cn_ast_type_ptr_hash, (Cn_Equals_Function *)cn_ast_type_ptr_equals);
    data->type_children_arena = cn_chained_arena_make(CN_AST_TYPE_CHILDREN_ARENA_BLOCK_CAP);

    data->binding_list = cn_array_list_make(Cn_Binding, CN_AST_BINDING_LIST_INITIAL_CAP);
    // Inserting first element as NIL.
    {
        Cn_Binding nil = {0};
        cn_array_list_append(&data->binding_list, nil);
        if (data->binding_list == NULL) return -1;
    }

    data->tag_binding_table = cn_hash_table_make(Cn_String, Cn_Binding_Idx, CN_AST_TAG_BINDING_TABLE_INITIAL_CAP, (Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);
    data->symbol_binding_table = cn_hash_table_make(Cn_String, Cn_Binding_Idx, CN_AST_SYMBOL_BINDING_TABLE_INITIAL_CAP,(Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);
    data->label_binding_list = cn_array_list_make(Cn_Binding, CN_AST_LABEL_BINDING_LIST_INITIAL_CAP);
    data->label_binding_table = cn_hash_table_make(Cn_String, Cn_Binding_Idx, CN_AST_LABEL_BINDING_TABLE_INITIAL_CAP,(Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);
    data->binding_defined_idx_list = cn_array_list_make(Cn_Binding_Idx, 16);
    data->scoped_strings_arena = cn_chained_arena_make(CN_AST_SCOPED_STRINGS_ARENA_BLOCK_CAP);
    data->permanent_strings_arena = cn_chained_arena_make(CN_AST_PERMANENT_STRINGS_ARENA_BLOCK_CAP);
    data->scope_stack = cn_array_list_make(Cn_Scope, CN_AST_SCOPE_STACK_INITIAL_CAP);
    data->ptr_stack = cn_array_list_make(Cn_Ast_Node *, CN_AST_PTR_STACK_INITIAL_CAP);
    data->warning_count = 0;
    data->error_count = 0;

    cn__tu_data = data;

    return 0;
}

CNDEF void cn__tu_data_free(Cn_Tu_Data *data) {
    cn_array_list_free(&data->ptr_stack);
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
    cn_chained_arena_destroy(&data->ast_arena);
    cn_chained_arena_destroy(&data->output_arena);

    *data = (Cn_Tu_Data) {0};
    cn__tu_data = NULL;
}

CNDEF bool cn__tu_init_opt(Cn_Translation_Unit *tu, char *intermidiate_path, Cn_Tu_Make_Opt opt) {
    // TEMPORARY:
    cn_extension_register(0);

    *tu = (Cn_Translation_Unit) {0};

    // Copying flags.
    tu->flags = opt.flags;
    tu->flags &= ~CN__TU_OPT_SOURCE;

    // Setting output path.
    if (!(opt.flags & CN_TU_NO_CODE_OUTPUT)) {
        if (opt.output_path == NULL) {
            tu->output_path = intermidiate_path;
        } else {
            tu->output_path = opt.output_path;
        }

        if (tu->output_path == NULL) {
            cn_log(CN_ERROR, "No output path specified for translation unit, make sure to use CN_TU_NO_CODE_OUTPUT, if you intend to have no output.\n");
            return false;
        }
    }

    // TODO: Pre-load hook.
    

    // Setting path.
    tu->path = intermidiate_path;
    
    // Loading source.
    if (!cn_str_is_empty(opt.source)) {
        tu->flags |= CN__TU_OPT_SOURCE;
        tu->data.source = opt.source;
    } else {
        if (tu->path == NULL) {
            cn_log(CN_ERROR, "No input path specified for translation unit, either provide source in optional, or specify path to input file.\n");
            return false;
        }

        // Reading the whole .i file into memory.
        FILE *file = fopen(tu->path, "rb");
        if (file == NULL) {
            cn_log(CN_ERROR, "Couldn't open the file '%s'.\n", tu->path);
            return false;
        }

        fseek(file, 0, SEEK_END);
        uint64_t size = ftell(file);
        rewind(file);

        void *buffer = CN_REALLOC(NULL, size);
        if (buffer == NULL) {
            cn_log(CN_ERROR, "Memory allocation for string buffer failed while reading the file '%s'.\n", tu->path);
            fclose(file);
            return false;
        }

        if (fread(buffer, 1, size, file) != size) {
            cn_log(CN_ERROR, "Failure reading the file '%s'.\n", tu->path);
            fclose(file);
            free(buffer);
            return false;
        }

        fclose(file);

        tu->data.source = CN_STR((int64_t)size, buffer);
    }

    cn__tu_data_init(&tu->data);

    return tu;
}

// Setting up lexer.
const Cn_Lexer_Blacklist cn__default_blacklist = {
    .length = 2,
    .ttypes = ((Cn_Token_Kind[]){ CN_TOKEN_COMMENT, CN_TOKEN_LINE_MARKER }),
};

CNDEF int cn_tu_process(Cn_Translation_Unit *tu) {
    // Printing source.
    if (tu->flags & CN_TU_PRINT_SOURCE) {
        cn_log(CN_INFO, "Received:\n" CN_ANSI_BRIGHT_BLACK "%.*s" CN_ANSI_RESET, CN_STR_UNPACK(cn__tu_data->source));
    }

    Cn_Lexer lexer = {0};

    // Printing tokens.
    if (tu->flags & CN_TU_PRINT_TOKENS) {
        cn_lexer_init(&lexer, cn__tu_data->source, cn__default_blacklist);
        cn_lexer_log_tokens(&lexer, -1);
    }
    
    // Building AST.
    cn_lexer_init(&lexer, cn__tu_data->source, cn__default_blacklist);


    CN_ASSERT(cn__tu_data->scope_stack != NULL);
    Cn_Ast_Translation_Unit *tu_node = cn_parse_translation_unit(&lexer);
    if (cn__tu_data->error_count > 0) return -1;
    CN_ASSERT(tu_node != NULL);

    // Printing AST.
    if (tu->flags & CN_TU_PRINT_AST) {
        cn_log(CN_INFO, "Parsed:");
        cn_ast_print(tu_node, 0);
        fputc('\n', stderr);
    }

    // Printing type universe.
    if (tu->flags & CN_TU_PRINT_TYPES) {
        cn_tu_log_types(tu);
    }

    // Printing bindings.
    if (tu->flags & CN_TU_PRINT_BINDINGS) {
        cn_tu_log_bindings(tu);
    }

    // Emit full AST back to the same .i file.
    if (!(tu->flags & CN_TU_NO_CODE_OUTPUT)) {
        FILE *out = fopen(tu->path, "w");
        if (out == NULL) {
            cn_log(CN_ERROR, "Failed to open '%s' for writing.", tu->path);
            return -1;
        }

        Cn_Emitter emitter = { .write = &cn_emit_write_file, .ctx = out };
        cn_emit(&emitter, tu_node);

        fclose(out);
    }

    return 0;
}

CNDEF void cn_tu_free(Cn_Translation_Unit *tu) {
    if (!(tu->flags & CN__TU_OPT_SOURCE)) CN_FREE(tu->data.source.data);
    cn__tu_data_free(&tu->data);
    *tu = (Cn_Translation_Unit) {0};
}

CNDEF void cn_tu_log_types(Cn_Translation_Unit *tu) {
    if (CN_INFO >= cn_log_min_level) {
        cn_log(CN_INFO, "Type universe:" CN_ANSI_BLUE);

        void *block = tu->data.type_arena.block;
        Cn_Chained_Arena_Block_Header *h;
        while(true) {
            cn_chained_arena_foreach_in_block(Cn_Type, type, tu->data.type_arena.block) {
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

CNDEF void cn_tu_log_bindings(Cn_Translation_Unit *tu) {
    if (CN_INFO >= cn_log_min_level) {
        // First binding is NIL, so skip index 0.
        cn_log(CN_INFO, "Bindings:" CN_ANSI_BRIGHT_YELLOW);
        for (int i = 1; i < cn_array_list_length(&tu->data.binding_list); i++) {
            switch (tu->data.binding_list[i].kind) {
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
                case CN_BINDING_BUILTIN:
                    fputs("BUILTIN     ", stderr);
                    break;
                default:
                    break;
            }

            fprintf(stderr, "%.*s -> ", CN_STR_UNPACK(tu->data.binding_list[i].name));
            cn_type_print(tu->data.binding_list[i].type);
            fputc('\n', stderr);
        }
        fprintf(stderr, CN_ANSI_RESET"\n");
    }
}

// Defining ast meta info at the very end, so that all
// declarations and emit functions are in the namespace, and can be used.
// Core ast meta info, every entry must match its struct definition above.
static Cn_Ast_Member_Info cn__ast_code_members[] = {
    { "text", offsetof(Cn_Ast_Code, text), CN_AST_MEMBER_INFO_STRING },
};

static Cn_Ast_Member_Info cn__ast_translation_unit_members[] = {
    { "external_declarations", offsetof(Cn_Ast_Translation_Unit, external_declarations), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_external_declaration_members[] = {
    { "child", offsetof(Cn_Ast_External_Declaration, child), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_declaration_members[] = {
    { "attribute_specifiers",   offsetof(Cn_Ast_Declaration, attribute_specifiers),   CN_AST_MEMBER_INFO_NODES },
    { "declaration_specifiers", offsetof(Cn_Ast_Declaration, declaration_specifiers), CN_AST_MEMBER_INFO_NODE },
    { "init_declarators",       offsetof(Cn_Ast_Declaration, init_declarators),       CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_function_members[] = {
    { "attribute_specifiers",   offsetof(Cn_Ast_Function, attribute_specifiers),   CN_AST_MEMBER_INFO_NODES },
    { "declaration_specifiers", offsetof(Cn_Ast_Function, declaration_specifiers), CN_AST_MEMBER_INFO_NODE },
    { "declarator",             offsetof(Cn_Ast_Function, declarator),             CN_AST_MEMBER_INFO_NODE },
    { "block",                  offsetof(Cn_Ast_Function, block),                  CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_block_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Block, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "block_items",          offsetof(Cn_Ast_Block, block_items),          CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_block_item_members[] = {
    { "declaration_or_statement", offsetof(Cn_Ast_Block_Item, declaration_or_statement), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_if_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_If, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "condition",            offsetof(Cn_Ast_If, condition),            CN_AST_MEMBER_INFO_NODE },
    { "then_statement",       offsetof(Cn_Ast_If, then_statement),       CN_AST_MEMBER_INFO_NODE },
    { "else_statement",       offsetof(Cn_Ast_If, else_statement),       CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_switch_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Switch, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "condition",            offsetof(Cn_Ast_Switch, condition),            CN_AST_MEMBER_INFO_NODE },
    { "body",                 offsetof(Cn_Ast_Switch, body),                 CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_while_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_While, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "condition",            offsetof(Cn_Ast_While, condition),            CN_AST_MEMBER_INFO_NODE },
    { "body",                 offsetof(Cn_Ast_While, body),                 CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_do_while_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Do_While, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "condition",            offsetof(Cn_Ast_Do_While, condition),            CN_AST_MEMBER_INFO_NODE },
    { "body",                 offsetof(Cn_Ast_Do_While, body),                 CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_for_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_For, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "initialization",       offsetof(Cn_Ast_For, initialization),       CN_AST_MEMBER_INFO_NODE },
    { "condition",            offsetof(Cn_Ast_For, condition),            CN_AST_MEMBER_INFO_NODE },
    { "update",               offsetof(Cn_Ast_For, update),               CN_AST_MEMBER_INFO_NODE },
    { "body",                 offsetof(Cn_Ast_For, body),                 CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_label_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Label, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "identifier",           offsetof(Cn_Ast_Label, identifier),           CN_AST_MEMBER_INFO_NODE },
    { "expression",           offsetof(Cn_Ast_Label, expression),           CN_AST_MEMBER_INFO_NODE },
    { "statement",            offsetof(Cn_Ast_Label, statement),            CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_goto_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Goto, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "identifier",           offsetof(Cn_Ast_Goto, identifier),           CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_return_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Return, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "expression",           offsetof(Cn_Ast_Return, expression),           CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_break_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Break, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_continue_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Continue, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_expression_statement_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Expression_Statement, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "expression",           offsetof(Cn_Ast_Expression_Statement, expression),           CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_binary_members[] = {
    { "type",     offsetof(Cn_Ast_Binary, type),     CN_AST_MEMBER_INFO_TYPE },
    { "left",     offsetof(Cn_Ast_Binary, left),     CN_AST_MEMBER_INFO_NODE },
    { "operator", offsetof(Cn_Ast_Binary, operator), CN_AST_MEMBER_INFO_INT32 },
    { "right",    offsetof(Cn_Ast_Binary, right),    CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_access_members[] = {
    { "type",       offsetof(Cn_Ast_Access, type),       CN_AST_MEMBER_INFO_TYPE },
    { "expression", offsetof(Cn_Ast_Access, expression), CN_AST_MEMBER_INFO_NODE },
    { "member",     offsetof(Cn_Ast_Access, member),     CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_call_members[] = {
    { "type",       offsetof(Cn_Ast_Call, type),       CN_AST_MEMBER_INFO_TYPE },
    { "expression", offsetof(Cn_Ast_Call, expression), CN_AST_MEMBER_INFO_NODE },
    { "arguments",  offsetof(Cn_Ast_Call, arguments),  CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_unary_members[] = {
    { "type",       offsetof(Cn_Ast_Unary, type),       CN_AST_MEMBER_INFO_TYPE },
    { "operator",   offsetof(Cn_Ast_Unary, operator),   CN_AST_MEMBER_INFO_INT32 },
    { "expression", offsetof(Cn_Ast_Unary, expression), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_cast_members[] = {
    { "type",       offsetof(Cn_Ast_Cast, type),       CN_AST_MEMBER_INFO_TYPE },
    { "type_name",  offsetof(Cn_Ast_Cast, type_name),  CN_AST_MEMBER_INFO_NODE },
    { "expression", offsetof(Cn_Ast_Cast, expression), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_compound_members[] = {
    { "type",         offsetof(Cn_Ast_Compound, type),         CN_AST_MEMBER_INFO_TYPE },
    { "type_name",    offsetof(Cn_Ast_Compound, type_name),    CN_AST_MEMBER_INFO_NODE },
    { "designations", offsetof(Cn_Ast_Compound, designations), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_sizeof_expression_members[] = {
    { "type",   offsetof(Cn_Ast_Sizeof, type),   CN_AST_MEMBER_INFO_TYPE },
    { "target", offsetof(Cn_Ast_Sizeof, target), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_ternary_members[] = {
    { "type",      offsetof(Cn_Ast_Ternary, type),      CN_AST_MEMBER_INFO_TYPE },
    { "condition", offsetof(Cn_Ast_Ternary, condition), CN_AST_MEMBER_INFO_NODE },
    { "if_true",   offsetof(Cn_Ast_Ternary, if_true),   CN_AST_MEMBER_INFO_NODE },
    { "if_false",  offsetof(Cn_Ast_Ternary, if_false),  CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_assign_members[] = {
    { "type",     offsetof(Cn_Ast_Assign, type),     CN_AST_MEMBER_INFO_TYPE },
    { "left",     offsetof(Cn_Ast_Assign, left),     CN_AST_MEMBER_INFO_NODE },
    { "operator", offsetof(Cn_Ast_Assign, operator), CN_AST_MEMBER_INFO_INT32 },
    { "right",    offsetof(Cn_Ast_Assign, right),    CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_postfix_members[] = {
    { "type",       offsetof(Cn_Ast_Postfix, type),       CN_AST_MEMBER_INFO_TYPE },
    { "expression", offsetof(Cn_Ast_Postfix, expression), CN_AST_MEMBER_INFO_NODE },
    { "operator",   offsetof(Cn_Ast_Postfix, operator),   CN_AST_MEMBER_INFO_INT32 },
};

static Cn_Ast_Member_Info cn__ast_primary_members[] = {
    { "type",    offsetof(Cn_Ast_Primary, type),    CN_AST_MEMBER_INFO_TYPE },
    { "literal", offsetof(Cn_Ast_Primary, literal), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_identifier_members[] = {
    { "name", offsetof(Cn_Ast_Identifier, name), CN_AST_MEMBER_INFO_STRING },
};

static Cn_Ast_Member_Info cn__ast_integer_members[] = {
    { "value", offsetof(Cn_Ast_Integer, value), CN_AST_MEMBER_INFO_UINT64 },
};

static Cn_Ast_Member_Info cn__ast_flt_members[] = {
    { "value", offsetof(Cn_Ast_Float, value), CN_AST_MEMBER_INFO_DOUBLE },
};

static Cn_Ast_Member_Info cn__ast_character_members[] = {
    { "value",      offsetof(Cn_Ast_Char, value),      CN_AST_MEMBER_INFO_UINT64 },
    { "unit_count", offsetof(Cn_Ast_Char, unit_count), CN_AST_MEMBER_INFO_INT32 },
};

static Cn_Ast_Member_Info cn__ast_string_members[] = {
    { "str", offsetof(Cn_Ast_String, str), CN_AST_MEMBER_INFO_STRING },
};

static Cn_Ast_Member_Info cn__ast_init_declarator_members[] = {
    { "declarator",               offsetof(Cn_Ast_Init_Declarator, declarator),               CN_AST_MEMBER_INFO_NODE },
#ifdef CN_GNU
    { "asm_label", offsetof(Cn_Ast_Init_Declarator, asm_label), CN_AST_MEMBER_INFO_NODE },
    { "attribute_specifiers", offsetof(Cn_Ast_Init_Declarator, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
#endif
{ "initializer",              offsetof(Cn_Ast_Init_Declarator, initializer),              CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_initializer_members[] = {
    { "expression",   offsetof(Cn_Ast_Initializer, expression),   CN_AST_MEMBER_INFO_NODE },
    { "designations", offsetof(Cn_Ast_Initializer, designations), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_designation_members[] = {
    { "designators", offsetof(Cn_Ast_Designation, designators), CN_AST_MEMBER_INFO_NODES },
    { "initializer", offsetof(Cn_Ast_Designation, initializer), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_designator_members[] = {
    { "identifier",           offsetof(Cn_Ast_Designator, identifier),           CN_AST_MEMBER_INFO_NODE },
    { "expression",           offsetof(Cn_Ast_Designator, expression),           CN_AST_MEMBER_INFO_NODE },
    { "expression_range_end", offsetof(Cn_Ast_Designator, expression_range_end), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_declarator_members[] = {
    { "pointer",           offsetof(Cn_Ast_Declarator, pointer),           CN_AST_MEMBER_INFO_NODE },
    { "direct_declarator", offsetof(Cn_Ast_Declarator, direct_declarator), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_pointer_members[] = {
    { "qualifiers",               offsetof(Cn_Ast_Pointer, qualifiers),               CN_AST_MEMBER_INFO_INT32 },
#ifdef CN_GNU
    { "attribute_specifiers", offsetof(Cn_Ast_Pointer, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
#endif
{ "pointer",                  offsetof(Cn_Ast_Pointer, pointer),                  CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_direct_declarator_grouped_members[] = {
    { "declarator", offsetof(Cn_Ast_Direct_Declarator_Grouped, declarator), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_direct_declarator_array_members[] = {
    { "direct_declarator", offsetof(Cn_Ast_Direct_Declarator_Array, direct_declarator), CN_AST_MEMBER_INFO_NODE },
    { "expression",        offsetof(Cn_Ast_Direct_Declarator_Array, expression),        CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_direct_declarator_function_members[] = {
    { "direct_declarator",      offsetof(Cn_Ast_Direct_Declarator_Function, direct_declarator),      CN_AST_MEMBER_INFO_NODE },
    { "parameter_declarations", offsetof(Cn_Ast_Direct_Declarator_Function, parameter_declarations), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_declaration_specifiers_members[] = {
    { "storage_specifiers",       offsetof(Cn_Ast_Declaration_Specifiers, storage_specifiers),       CN_AST_MEMBER_INFO_INT32 },
    { "qualifiers",               offsetof(Cn_Ast_Declaration_Specifiers, qualifiers),               CN_AST_MEMBER_INFO_INT32 },
    { "function_specifiers",      offsetof(Cn_Ast_Declaration_Specifiers, function_specifiers),      CN_AST_MEMBER_INFO_INT32 },
#ifdef CN_GNU
    { "attribute_specifiers", offsetof(Cn_Ast_Declaration_Specifiers, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
#endif
{ "type_specifier",           offsetof(Cn_Ast_Declaration_Specifiers, type_specifier),           CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_type_specifier_primitive_members[] = {
    { "primitive_kind", offsetof(Cn_Ast_Type_Specifier_Primitive, primitive_kind), CN_AST_MEMBER_INFO_INT32 },
    { "width",          offsetof(Cn_Ast_Type_Specifier_Primitive, width),          CN_AST_MEMBER_INFO_INT32 },
    { "sign",           offsetof(Cn_Ast_Type_Specifier_Primitive, sign),           CN_AST_MEMBER_INFO_INT32 },
};

static Cn_Ast_Member_Info cn__ast_type_specifier_typedef_members[] = {
    { "typedef_name", offsetof(Cn_Ast_Type_Specifier_Typedef, typedef_name), CN_AST_MEMBER_INFO_STRING },
};

static Cn_Ast_Member_Info cn__ast_type_name_members[] = {
    { "specifier_qualifier", offsetof(Cn_Ast_Type_Name, specifier_qualifier), CN_AST_MEMBER_INFO_NODE },
    { "abstract_declarator", offsetof(Cn_Ast_Type_Name, abstract_declarator), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_specifier_qualifier_members[] = {
    { "qualifiers",     offsetof(Cn_Ast_Specifier_Qualifier, qualifiers),     CN_AST_MEMBER_INFO_INT32 },
#ifdef CN_GNU
    { "attribute_specifiers", offsetof(Cn_Ast_Specifier_Qualifier, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
#endif
    { "type_specifier", offsetof(Cn_Ast_Specifier_Qualifier, type_specifier), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_parameter_declaration_members[] = {
    { "declaration_specifiers", offsetof(Cn_Ast_Parameter_Declaration, declaration_specifiers), CN_AST_MEMBER_INFO_NODE },
    { "declarator",             offsetof(Cn_Ast_Parameter_Declaration, declarator),             CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_struct_specifier_members[] = {
    { "attribute_specifiers",     offsetof(Cn_Ast_Struct_Specifier, attribute_specifiers),     CN_AST_MEMBER_INFO_NODES },
{ "identifier",               offsetof(Cn_Ast_Struct_Specifier, identifier),               CN_AST_MEMBER_INFO_NODE },
    { "member_declarations",      offsetof(Cn_Ast_Struct_Specifier, member_declarations),      CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_union_specifier_members[] = {
    { "attribute_specifiers",     offsetof(Cn_Ast_Union_Specifier, attribute_specifiers),     CN_AST_MEMBER_INFO_NODES },
{ "identifier",               offsetof(Cn_Ast_Union_Specifier, identifier),               CN_AST_MEMBER_INFO_NODE },
    { "member_declarations",      offsetof(Cn_Ast_Union_Specifier, member_declarations),      CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_member_declaration_members[] = {
    { "attribute_specifiers", offsetof(Cn_Ast_Member_Declaration, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
    { "specifier_qualifier",  offsetof(Cn_Ast_Member_Declaration, specifier_qualifier),  CN_AST_MEMBER_INFO_NODE },
    { "member_declarators",   offsetof(Cn_Ast_Member_Declaration, member_declarators),   CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_member_declarator_members[] = {
    { "declarator",               offsetof(Cn_Ast_Member_Declarator, declarator),               CN_AST_MEMBER_INFO_NODE },
    { "bitfield",                 offsetof(Cn_Ast_Member_Declarator, bitfield),                 CN_AST_MEMBER_INFO_NODE },
#ifdef CN_GNU
    { "attribute_specifiers", offsetof(Cn_Ast_Member_Declarator, attribute_specifiers), CN_AST_MEMBER_INFO_NODES },
#endif
};

static Cn_Ast_Member_Info cn__ast_enum_specifier_members[] = {
    { "attribute_specifiers",     offsetof(Cn_Ast_Enum_Specifier, attribute_specifiers),     CN_AST_MEMBER_INFO_NODES },
{ "identifier",               offsetof(Cn_Ast_Enum_Specifier, identifier),               CN_AST_MEMBER_INFO_NODE },
    { "specifier_qualifier",      offsetof(Cn_Ast_Enum_Specifier, specifier_qualifier),      CN_AST_MEMBER_INFO_NODE },
    { "enumerators",              offsetof(Cn_Ast_Enum_Specifier, enumerators),              CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_enumerator_members[] = {
    { "identifier",               offsetof(Cn_Ast_Enumerator, identifier),               CN_AST_MEMBER_INFO_NODE },
    { "attribute_specifiers",     offsetof(Cn_Ast_Enumerator, attribute_specifiers),     CN_AST_MEMBER_INFO_NODES },
{ "expression",               offsetof(Cn_Ast_Enumerator, expression),               CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_attribute_specifier_members[] = {
    { "attributes", offsetof(Cn_Ast_Attribute_Specifier, attributes), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_attribute_members[] = {
    { "vendor_identifier", offsetof(Cn_Ast_Attribute, vendor_identifier), CN_AST_MEMBER_INFO_NODE },
    { "identifier",        offsetof(Cn_Ast_Attribute, identifier),        CN_AST_MEMBER_INFO_NODE },
    { "arguments",         offsetof(Cn_Ast_Attribute, arguments),         CN_AST_MEMBER_INFO_NODES },
};

#ifdef CN_GNU
static Cn_Ast_Member_Info cn__ast_gnu_typeof_members[] = {
    { "target", offsetof(Cn_Ast_Gnu_Typeof, target), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_gnu_asm_statement_members[] = {
    { "qualifiers", offsetof(Cn_Ast_Gnu_Asm_Statement, qualifiers), CN_AST_MEMBER_INFO_INT32 },
    { "template", offsetof(Cn_Ast_Gnu_Asm_Statement, template), CN_AST_MEMBER_INFO_NODE },
    { "outputs", offsetof(Cn_Ast_Gnu_Asm_Statement, outputs), CN_AST_MEMBER_INFO_NODES },
    { "inputs", offsetof(Cn_Ast_Gnu_Asm_Statement, inputs), CN_AST_MEMBER_INFO_NODES },
    { "clobbers", offsetof(Cn_Ast_Gnu_Asm_Statement, clobbers), CN_AST_MEMBER_INFO_NODES },
    { "goto_labels", offsetof(Cn_Ast_Gnu_Asm_Statement, goto_labels), CN_AST_MEMBER_INFO_NODES },
};

static Cn_Ast_Member_Info cn__ast_gnu_asm_operand_members[] = {
    { "identifier", offsetof(Cn_Ast_Gnu_Asm_Operand, identifier), CN_AST_MEMBER_INFO_NODE },
    { "string", offsetof(Cn_Ast_Gnu_Asm_Operand, string), CN_AST_MEMBER_INFO_NODE },
    { "expression", offsetof(Cn_Ast_Gnu_Asm_Operand, expression), CN_AST_MEMBER_INFO_NODE },
};

static Cn_Ast_Member_Info cn__ast_gnu_asm_label_members[] = {
    { "string", offsetof(Cn_Ast_Gnu_Asm_Label, string), CN_AST_MEMBER_INFO_NODE },
};
#endif

static Cn_Ast_Info cn__ast_infos[CN_AST__END] = {
    [CN_AST_UNKNOWN] = { .name = "<unknown>", .size = 0, .members = NULL, .members_count = 0 },
    [CN_AST_CODE] = {
        .name = "code", .size = sizeof(Cn_Ast_Code),
        .members = cn__ast_code_members, .members_count = CN_ARRAY_LENGTH(cn__ast_code_members),
        .emit_func = cn__emit_code,
    },
    [CN_AST_TRANSLATION_UNIT] = {
        .name = "translation_unit", .size = sizeof(Cn_Ast_Translation_Unit),
        .members = cn__ast_translation_unit_members, .members_count = CN_ARRAY_LENGTH(cn__ast_translation_unit_members),
        .emit_func = cn__emit_translation_unit,
    },
    [CN_AST_EXTERNAL_DECLARATION] = {
        .name = "external_declaration", .size = sizeof(Cn_Ast_External_Declaration),
        .members = cn__ast_external_declaration_members, .members_count = CN_ARRAY_LENGTH(cn__ast_external_declaration_members),
        .emit_func = cn__emit_external_declaration,
    },
    [CN_AST_DECLARATION] = {
        .name = "declaration", .size = sizeof(Cn_Ast_Declaration),
        .members = cn__ast_declaration_members, .members_count = CN_ARRAY_LENGTH(cn__ast_declaration_members),
        .emit_func = cn__emit_declaration,
    },
    [CN_AST_FUNCTION] = {
        .name = "function", .size = sizeof(Cn_Ast_Function),
        .members = cn__ast_function_members, .members_count = CN_ARRAY_LENGTH(cn__ast_function_members),
        .emit_func = cn__emit_function,
    },
    [CN_AST_BLOCK] = {
        .name = "block", .size = sizeof(Cn_Ast_Block),
        .members = cn__ast_block_members, .members_count = CN_ARRAY_LENGTH(cn__ast_block_members),
        .emit_func = cn__emit_block,
    },
    [CN_AST_BLOCK_ITEM] = {
        .name = "block_item", .size = sizeof(Cn_Ast_Block_Item),
        .members = cn__ast_block_item_members, .members_count = CN_ARRAY_LENGTH(cn__ast_block_item_members),
        .emit_func = cn__emit_block_item,
    },
    [CN_AST_IF] = {
        .name = "if_statement", .size = sizeof(Cn_Ast_If),
        .members = cn__ast_if_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_if_statement_members),
        .emit_func = cn__emit_if_statement,
    },
    [CN_AST_SWITCH] = {
        .name = "switch_statement", .size = sizeof(Cn_Ast_Switch),
        .members = cn__ast_switch_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_switch_statement_members),
        .emit_func = cn__emit_switch_statement,
    },
    [CN_AST_WHILE] = {
        .name = "while_statement", .size = sizeof(Cn_Ast_While),
        .members = cn__ast_while_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_while_statement_members),
        .emit_func = cn__emit_while_statement,
    },
    [CN_AST_DO_WHILE] = {
        .name = "do_while", .size = sizeof(Cn_Ast_Do_While),
        .members = cn__ast_do_while_members, .members_count = CN_ARRAY_LENGTH(cn__ast_do_while_members),
        .emit_func = cn__emit_do_while,
    },
    [CN_AST_FOR] = {
        .name = "for_statement", .size = sizeof(Cn_Ast_For),
        .members = cn__ast_for_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_for_statement_members),
        .emit_func = cn__emit_for_statement,
    },
    [CN_AST_LABEL] = {
        .name = "label", .size = sizeof(Cn_Ast_Label),
        .members = cn__ast_label_members, .members_count = CN_ARRAY_LENGTH(cn__ast_label_members),
        .emit_func = cn__emit_label,
    },
    [CN_AST_GOTO] = {
        .name = "goto_statement", .size = sizeof(Cn_Ast_Goto),
        .members = cn__ast_goto_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_goto_statement_members),
        .emit_func = cn__emit_goto_statement,
    },
    [CN_AST_RETURN] = {
        .name = "return_statement", .size = sizeof(Cn_Ast_Return),
        .members = cn__ast_return_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_return_statement_members),
        .emit_func = cn__emit_return_statement,
    },
    [CN_AST_BREAK] = {
        .name = "break_statement", .size = sizeof(Cn_Ast_Break),
        .members = cn__ast_break_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_break_statement_members),
        .emit_func = cn__emit_break_statement,
    },
    [CN_AST_CONTINUE] = {
        .name = "continue_statement", .size = sizeof(Cn_Ast_Continue),
        .members = cn__ast_continue_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_continue_statement_members),
        .emit_func = cn__emit_continue_statement,
    },
    [CN_AST_EXPRESSION_STATEMENT] = {
        .name = "expression_statement", .size = sizeof(Cn_Ast_Expression_Statement),
        .members = cn__ast_expression_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_expression_statement_members),
        .emit_func = cn__emit_expression_statement,
    },
    [CN_AST_BINARY] = {
        .name = "binary", .size = sizeof(Cn_Ast_Binary),
        .members = cn__ast_binary_members, .members_count = CN_ARRAY_LENGTH(cn__ast_binary_members),
        .emit_func = cn__emit_binary,
    },
    [CN_AST_ACCESS] = {
        .name = "access", .size = sizeof(Cn_Ast_Access),
        .members = cn__ast_access_members, .members_count = CN_ARRAY_LENGTH(cn__ast_access_members),
        .emit_func = cn__emit_access,
    },
    [CN_AST_CALL] = {
        .name = "call", .size = sizeof(Cn_Ast_Call),
        .members = cn__ast_call_members, .members_count = CN_ARRAY_LENGTH(cn__ast_call_members),
        .emit_func = cn__emit_call,
    },
    [CN_AST_UNARY] = {
        .name = "unary", .size = sizeof(Cn_Ast_Unary),
        .members = cn__ast_unary_members, .members_count = CN_ARRAY_LENGTH(cn__ast_unary_members),
        .emit_func = cn__emit_unary,
    },
    [CN_AST_CAST] = {
        .name = "cast", .size = sizeof(Cn_Ast_Cast),
        .members = cn__ast_cast_members, .members_count = CN_ARRAY_LENGTH(cn__ast_cast_members),
        .emit_func = cn__emit_cast,
    },
    [CN_AST_COMPOUND] = {
        .name = "compound", .size = sizeof(Cn_Ast_Compound),
        .members = cn__ast_compound_members, .members_count = CN_ARRAY_LENGTH(cn__ast_compound_members),
        .emit_func = cn__emit_compound,
    },
    [CN_AST_SIZEOF] = {
        .name = "sizeof_expression", .size = sizeof(Cn_Ast_Sizeof),
        .members = cn__ast_sizeof_expression_members, .members_count = CN_ARRAY_LENGTH(cn__ast_sizeof_expression_members),
        .emit_func = cn__emit_sizeof_expression,
    },
    [CN_AST_TERNARY] = {
        .name = "ternary", .size = sizeof(Cn_Ast_Ternary),
        .members = cn__ast_ternary_members, .members_count = CN_ARRAY_LENGTH(cn__ast_ternary_members),
        .emit_func = cn__emit_ternary,
    },
    [CN_AST_ASSIGN] = {
        .name = "assign", .size = sizeof(Cn_Ast_Assign),
        .members = cn__ast_assign_members, .members_count = CN_ARRAY_LENGTH(cn__ast_assign_members),
        .emit_func = cn__emit_assign,
    },
    [CN_AST_POSTFIX] = {
        .name = "postfix", .size = sizeof(Cn_Ast_Postfix),
        .members = cn__ast_postfix_members, .members_count = CN_ARRAY_LENGTH(cn__ast_postfix_members),
        .emit_func = cn__emit_postfix,
    },
    [CN_AST_PRIMARY] = {
        .name = "primary", .size = sizeof(Cn_Ast_Primary),
        .members = cn__ast_primary_members, .members_count = CN_ARRAY_LENGTH(cn__ast_primary_members),
        .emit_func = cn__emit_primary,
    },
    [CN_AST_IDENTIFIER] = {
        .name = "identifier", .size = sizeof(Cn_Ast_Identifier),
        .members = cn__ast_identifier_members, .members_count = CN_ARRAY_LENGTH(cn__ast_identifier_members),
        .emit_func = cn__emit_identifier,
    },
    [CN_AST_INTEGER] = {
        .name = "integer", .size = sizeof(Cn_Ast_Integer),
        .members = cn__ast_integer_members, .members_count = CN_ARRAY_LENGTH(cn__ast_integer_members),
        .emit_func = cn__emit_integer,
    },
    [CN_AST_FLOAT] = {
        .name = "flt", .size = sizeof(Cn_Ast_Float),
        .members = cn__ast_flt_members, .members_count = CN_ARRAY_LENGTH(cn__ast_flt_members),
        .emit_func = cn__emit_flt,
    },
    [CN_AST_CHAR] = {
        .name = "character", .size = sizeof(Cn_Ast_Char),
        .members = cn__ast_character_members, .members_count = CN_ARRAY_LENGTH(cn__ast_character_members),
        .emit_func = cn__emit_character,
    },
    [CN_AST_STRING] = {
        .name = "string", .size = sizeof(Cn_Ast_String),
        .members = cn__ast_string_members, .members_count = CN_ARRAY_LENGTH(cn__ast_string_members),
        .emit_func = cn__emit_string,
    },
    [CN_AST_INIT_DECLARATOR] = {
        .name = "init_declarator", .size = sizeof(Cn_Ast_Init_Declarator),
        .members = cn__ast_init_declarator_members, .members_count = CN_ARRAY_LENGTH(cn__ast_init_declarator_members),
        .emit_func = cn__emit_init_declarator,
    },
    [CN_AST_INITIALIZER] = {
        .name = "initializer", .size = sizeof(Cn_Ast_Initializer),
        .members = cn__ast_initializer_members, .members_count = CN_ARRAY_LENGTH(cn__ast_initializer_members),
        .emit_func = cn__emit_initializer,
    },
    [CN_AST_DESIGNATION] = {
        .name = "designation", .size = sizeof(Cn_Ast_Designation),
        .members = cn__ast_designation_members, .members_count = CN_ARRAY_LENGTH(cn__ast_designation_members),
        .emit_func = cn__emit_designation,
    },
    [CN_AST_DESIGNATOR] = {
        .name = "designator", .size = sizeof(Cn_Ast_Designator),
        .members = cn__ast_designator_members, .members_count = CN_ARRAY_LENGTH(cn__ast_designator_members),
        .emit_func = cn__emit_designator,
    },
    [CN_AST_DECLARATOR] = {
        .name = "declarator", .size = sizeof(Cn_Ast_Declarator),
        .members = cn__ast_declarator_members, .members_count = CN_ARRAY_LENGTH(cn__ast_declarator_members),
        .emit_func = cn__emit_declarator,
    },
    [CN_AST_POINTER] = {
        .name = "pointer", .size = sizeof(Cn_Ast_Pointer),
        .members = cn__ast_pointer_members, .members_count = CN_ARRAY_LENGTH(cn__ast_pointer_members),
        .emit_func = cn__emit_pointer,
    },
    [CN_AST_DIRECT_DECLARATOR_GROUPED] = {
        .name = "direct_declarator_grouped", .size = sizeof(Cn_Ast_Direct_Declarator_Grouped),
        .members = cn__ast_direct_declarator_grouped_members, .members_count = CN_ARRAY_LENGTH(cn__ast_direct_declarator_grouped_members),
        .emit_func = cn__emit_direct_declarator_grouped,
    },
    [CN_AST_DIRECT_DECLARATOR_ARRAY] = {
        .name = "direct_declarator_array", .size = sizeof(Cn_Ast_Direct_Declarator_Array),
        .members = cn__ast_direct_declarator_array_members, .members_count = CN_ARRAY_LENGTH(cn__ast_direct_declarator_array_members),
        .emit_func = cn__emit_direct_declarator_array,
    },
    [CN_AST_DIRECT_DECLARATOR_FUNCTION] = {
        .name = "direct_declarator_function", .size = sizeof(Cn_Ast_Direct_Declarator_Function),
        .members = cn__ast_direct_declarator_function_members, .members_count = CN_ARRAY_LENGTH(cn__ast_direct_declarator_function_members),
        .emit_func = cn__emit_direct_declarator_function,
    },
    [CN_AST_DECLARATION_SPECIFIERS] = {
        .name = "declaration_specifiers", .size = sizeof(Cn_Ast_Declaration_Specifiers),
        .members = cn__ast_declaration_specifiers_members, .members_count = CN_ARRAY_LENGTH(cn__ast_declaration_specifiers_members),
        .emit_func = cn__emit_declaration_specifiers,
    },
    [CN_AST_TYPE_SPECIFIER_PRIMITIVE] = {
        .name = "type_specifier_primitive", .size = sizeof(Cn_Ast_Type_Specifier_Primitive),
        .members = cn__ast_type_specifier_primitive_members, .members_count = CN_ARRAY_LENGTH(cn__ast_type_specifier_primitive_members),
        .emit_func = cn__emit_type_specifier_primitive,
    },
    [CN_AST_TYPE_SPECIFIER_TYPEDEF] = {
        .name = "type_specifier_typedef", .size = sizeof(Cn_Ast_Type_Specifier_Typedef),
        .members = cn__ast_type_specifier_typedef_members, .members_count = CN_ARRAY_LENGTH(cn__ast_type_specifier_typedef_members),
        .emit_func = cn__emit_type_specifier_typedef,
    },
    [CN_AST_TYPE_NAME] = {
        .name = "type_name", .size = sizeof(Cn_Ast_Type_Name),
        .members = cn__ast_type_name_members, .members_count = CN_ARRAY_LENGTH(cn__ast_type_name_members),
        .emit_func = cn__emit_type_name,
    },
    [CN_AST_SPECIFIER_QUALIFIER] = {
        .name = "specifier_qualifier", .size = sizeof(Cn_Ast_Specifier_Qualifier),
        .members = cn__ast_specifier_qualifier_members, .members_count = CN_ARRAY_LENGTH(cn__ast_specifier_qualifier_members),
        .emit_func = cn__emit_specifier_qualifier,
    },
    [CN_AST_PARAMETER_DECLARATION] = {
        .name = "parameter_declaration", .size = sizeof(Cn_Ast_Parameter_Declaration),
        .members = cn__ast_parameter_declaration_members, .members_count = CN_ARRAY_LENGTH(cn__ast_parameter_declaration_members),
        .emit_func = cn__emit_parameter_declaration,
    },
    [CN_AST_STRUCT_SPECIFIER] = {
        .name = "struct_specifier", .size = sizeof(Cn_Ast_Struct_Specifier),
        .members = cn__ast_struct_specifier_members, .members_count = CN_ARRAY_LENGTH(cn__ast_struct_specifier_members),
        .emit_func = cn__emit_struct_specifier,
    },
    [CN_AST_UNION_SPECIFIER] = {
        .name = "union_specifier", .size = sizeof(Cn_Ast_Union_Specifier),
        .members = cn__ast_union_specifier_members, .members_count = CN_ARRAY_LENGTH(cn__ast_union_specifier_members),
        .emit_func = cn__emit_union_specifier,
    },
    [CN_AST_MEMBER_DECLARATION] = {
        .name = "member_declaration", .size = sizeof(Cn_Ast_Member_Declaration),
        .members = cn__ast_member_declaration_members, .members_count = CN_ARRAY_LENGTH(cn__ast_member_declaration_members),
        .emit_func = cn__emit_member_declaration,
    },
    [CN_AST_MEMBER_DECLARATOR] = {
        .name = "member_declarator", .size = sizeof(Cn_Ast_Member_Declarator),
        .members = cn__ast_member_declarator_members, .members_count = CN_ARRAY_LENGTH(cn__ast_member_declarator_members),
        .emit_func = cn__emit_member_declarator,
    },
    [CN_AST_ENUM_SPECIFIER] = {
        .name = "enum_specifier", .size = sizeof(Cn_Ast_Enum_Specifier),
        .members = cn__ast_enum_specifier_members, .members_count = CN_ARRAY_LENGTH(cn__ast_enum_specifier_members),
        .emit_func = cn__emit_enum_specifier,
    },
    [CN_AST_ENUMERATOR] = {
        .name = "enumerator", .size = sizeof(Cn_Ast_Enumerator),
        .members = cn__ast_enumerator_members, .members_count = CN_ARRAY_LENGTH(cn__ast_enumerator_members),
        .emit_func = cn__emit_enumerator,
    },
    [CN_AST_ATTRIBUTE_SPECIFIER] = {
        .name = "attribute_specifier", .size = sizeof(Cn_Ast_Attribute_Specifier),
        .members = cn__ast_attribute_specifier_members, .members_count = CN_ARRAY_LENGTH(cn__ast_attribute_specifier_members),
        .emit_func = cn__emit_attribute_specifier,
    },
    [CN_AST_ATTRIBUTE] = {
        .name = "attribute", .size = sizeof(Cn_Ast_Attribute),
        .members = cn__ast_attribute_members, .members_count = CN_ARRAY_LENGTH(cn__ast_attribute_members),
        .emit_func = cn__emit_attribute,
    },
#ifdef CN_GNU
    [CN_AST_GNU_TYPEOF] = {
        .name = "gnu_typeof", .size = sizeof(Cn_Ast_Gnu_Typeof),
        .members = cn__ast_gnu_typeof_members, .members_count = CN_ARRAY_LENGTH(cn__ast_gnu_typeof_members),
        .emit_func = cn__emit_gnu_typeof,
    },
    [CN_AST_GNU_ASM_STATEMENT] = {
        .name = "gnu_asm_statement", .size = sizeof(Cn_Ast_Gnu_Asm_Statement),
        .members = cn__ast_gnu_asm_statement_members, .members_count = CN_ARRAY_LENGTH(cn__ast_gnu_asm_statement_members),
        .emit_func = cn__emit_gnu_asm_statement,
    },
    [CN_AST_GNU_ASM_OPERAND] = {
        .name = "gnu_asm_operand", .size = sizeof(Cn_Ast_Gnu_Asm_Operand),
        .members = cn__ast_gnu_asm_operand_members, .members_count = CN_ARRAY_LENGTH(cn__ast_gnu_asm_operand_members),
        .emit_func = cn__emit_gnu_asm_operand,
    },
    [CN_AST_GNU_ASM_LABEL] = {
        .name = "gnu_asm_label", .size = sizeof(Cn_Ast_Gnu_Asm_Label),
        .members = cn__ast_gnu_asm_label_members, .members_count = CN_ARRAY_LENGTH(cn__ast_gnu_asm_label_members),
        .emit_func = cn__emit_gnu_asm_label,
    },
#endif
};


#endif // CN_IMPLEMENTATION_GUARD_
#endif // CN_IMPLEMENTATION


/* 
    Revision history:
        0.3.0 (2026-11-27)  Revised whole documentation throughout public API:
                                - All items now contain entries.
                                - All items now contain `SINCE` tag, marking when entry was added.
                            Removed X-Macro usage to declare AST nodes, substituted for more convenient interface.
                            Removed `message_` interface, now it is replaced by `hook_`.
                            Added new `hook_` interface:
                                - `Cn_Hook_Ast_Parsed` for AST rewriting.
                                - `Cn_Hook_Tu_Start` for AST prepending to translation unit.
                                - `Cn_Hook_Tu_End` for AST appending to translation unit.
                                - `Cn_Hook_Analyze_Declarator` for declarator type modification.
                                - `Cn_Hook_Analyze_Layout` for type layout completion.
                                - `cn_hooks` global variable of type `Cn_Hooks` to store all hooks.
                                - `Cn_Extension` for extension functions that register custom hooks.
                                - `cn_extension_register` to register all extensions.
                            Added `CN_GNU` macro flag to enable GNU support in library. All GNU functionality is hidden behind.
                            Revised testing:
                                - Now is split on `parsing` and `exec` tests.
                                - Documentation can be added to each test
                            Added ability to doc tool to scan libaries header and footer.
        0.2.0 (2026-11-11)  Added variable length array support, as well as incomplete array support.
                            Added parameter adjustments in to type conversions from AST.
                            Added basic char parsing, typechecking and evaluation.
                            Added `__builtin_*` auto-detection and handling.
                            Added GNU inline assembly support.
                            Added more GNU `__extension__` parsing in grammar.
                            Fixed `OPAQUE` typechecking bugs, including.
                            Fixed type conversions between pointer and array types.
                            Fixed cast expression precedence when emitting C code.
                            Added `DEVELOPMENT.md` to keep clear development standard.
                            Added `nob doc` command to scan API for documentation and output it into its own web page.
        0.1.1 (2026-08-10)  Fixed how_to/005_defer CN_MESSAGE_RESPONSE_NONE to CN_MESSAGE_RESPONSE_MODIFIED.
        0.1.0 (2026-08-04)  Initial development first release.
*/

/*
    Version conventions:

        Following MAJOR.MINOR.PATCH format described in https://semver.org/ - Semantic Versioning:
            
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
