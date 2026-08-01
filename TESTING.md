# Testing for cnotes.h

## Testing Infrastructure

The project uses a **record/test snapshot system** built with `nob.h`:

### Commands

```bash
./nob test                      # Run all tests
./nob test tests/mytest.c       # Run specific test
./nob record tests/mytest.c     # Record expected output for a test
                                # If test is meant to run without output, 
                                # empty file can be created as a valid output of the test.
```

### How It Works

1. **Recording**: `./nob record` compiles and runs the test, capturing stdout to a `.stdout.txt` file (golden file)
2. **Testing**: `./nob test` compiles and runs the test, comparing stdout against the recorded `.stdout.txt`
3. **Results**: `SUCCESS`, `BUILD_FAIL`, `RUNTIME_FAIL`, `UNEXPECTED_OUTPUT` or `TIMEOUT`

---

## Writing Tests

### Test Patterns

#### Pattern 1: Pure Assertion (Silent Success)
Use for data structures, pure functions, type system checks.

**Golden file**: Empty (assertions pass silently)

```c
#include "../cnotes.h"

int main(void) {
    int *list = cn_array_list_make(int, 2);
    CN_ASSERT(cn_array_list_length(&list) == 0);
    cn_array_list_append(&list, 42);
    CN_ASSERT(list[0] == 42);
    return 0;
}
```

#### Pattern 2: Empty Output
Used for testing on correct syntaxes, modifications, meaning no diagnostics allowed.

**Golden file**: Empty (assertions pass silently)

```c
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int main(void) {\n"
    "    return 0;\n"
    "}\n"
);

int main(void) {
    // Should pass without error.
    // If ANY diagnostic emmited will fail the test with UNEXPECTED_OUTPUT.
    // cn_test_diagnostic_handler makes sure to output diagnostic in stdout.
    // Such tests don't have to be recorder, but empty `.stdout.txt` must created for such tests.
    // If not error will be emitted about absence of golden file.
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    return 0;
}
```

#### Pattern 3: Captured Output.
Use for print functions that use stdout, format verification.

**Golden file**: Contains expected stdout

```c
#include "../cnotes.h"

int main(void) {
    int *table = cn_hash_table_make(...);
    cn_hash_table_print((void **)&table);  // Output captured
    return 0;
}
```

### Important Notes

- Tests are in the `tests/` directory.

---

## Known tool feature limitation are all stated in README.md.

Everything else should be a fair ground to cover with testing.

---

## Existing testing.

Every test should be listed here, with a brief description of what it covers.
This is done to document and quickly overview what is general coverage.

| Test | Covers |
| --- | --- |
| `array_list_append.c` | `cn_array_list_make` / `_item_size` / `_capacity` / `_length`, single `_append` with growth doubling (2 → 4 → 32), `_append_multiple` of a 20 element block. |
| `chained_arena.c` | `cn_chained_arena_make/_alloc/_dealloc/_free`. Allocations larger than block capacity, chaining across multiple blocks, previously handed out pointers staying valid, partial and over-sized deallocation unwinding back to an empty head block, teardown state. |
| `pool.c` | `cn_pool_make/_alloc/_realloc/_free/_free_all/_allocation_info/_destroy`. Size class selection by tightest fit, slot addresses inside a block, freed slots being reused, realloc staying in place versus moving to a bigger class, a capacity whose slot count only partially fills its last flag byte, chaining a second block when a size class fills up, rejection of interior, out of pool and already freed pointers, teardown state. |
| `hash_functions.c` | `cn_hash_u64`, `cn_hash_ptr`, `cn_hash_bytes`, `cn_hash_mix`. Determinism, distinctness for different inputs, byte length participating in the hash, order dependence of mixing. |
| `hash_set.c` | `cn_hash_set_make/_put/_contains/_remove` with `Cn_String` keys. Header `count` bookkeeping via `cn_hash_set_header` and raw header pointer arithmetic, membership of absent keys, re-insertion after removal, growth past initial capacity. |
| `hash_table_put.c` | `cn_hash_table_make/_put/_get/_count` with `Cn_String` keys and `int` values, growing from capacity 3 to 32 entries. Captured `cn_hash_table_print` output after each stage pins bucket layout and rehashing. |
| `hash_table_remove.c` | `cn_hash_table_remove` of every key of a 20 entry table. Captured `cn_hash_table_print` before and after pins slot state of a fully emptied table. |
| `string_builder.c` | `cn_sb_make/_append_char/_append_str/_append_format/_to_str/_clear/_reverse/_free`. Stack backed storage (`CN_SB_STACK_STORAGE_CAP`), spill from stack to heap on overflow with content preserved, heap backed construction from the start, post-free state. |
| `string_operations.c` | `Cn_String` API: `_equals`, `_is_empty`, `_substring`, `_find_left/_right`, `_find_char_left/_right`, `_find_whitespace_left`, `_is_symbol`, `_is_int`, `_parse_int`, `_count_chars`, `_hash`, `_eat_chars`, `_get_chars`, `_eat_spaces`, `_format`, including empty string and not-found cases. |
| `type_system.c` | `cn_type_equals`, `cn_type_make_pointer` (incl. pointer-to-pointer), `cn_type_make_qualified` (`const` / `volatile`), `cn_type_is_constant`, `cn_type_unqualified`, `cn_type_is_arithmetic`, `cn_type_is_scalar`, `cn_type_hash`, `cn_type_is_compatible`, `cn_type_greatest_arithmetic_rank`. |
| `lexer_tokens.c` | Token recognition: type keywords, storage class and qualifier keywords, control flow keywords, `struct`/`union`/`enum`/`sizeof`, identifiers, integer/float/string literals, punctuation, arithmetic, comparison, logical and bitwise, shift, all compound assignment operators, `++`/`--`/`->`/`?`/`...`. Octal literal lexing left as a commented-out TODO. |
| `lexer_state.c` | Lexer navigation: `cn_lexer_peek` not consuming, `cn_lexer_expect` on matching and mismatching types, `cn_lexer_next_token` to EOF and staying at EOF, empty input, whitespace/newline/tab skipping, peeking past EOF, token streams of a dense expression and a function declaration. |
| `literal_parsing.c` | `cn_parse_int_literal` for decimal, hex (`0x`/`0X`), octal, and `u`/`U`/`l`/`L`/`ul`/`ULL` suffixes. `cn_parse_float_literal` for plain decimals, scientific notation (`e`/`E`, negative exponents), `f`/`F`/`l`/`L` suffixes, and the `.5` / `5.` edge forms. Binary literals left as a commented-out TODO. |
| `parse_variable_declaration.c` | Global, `static`, `extern` and `const` declarations; `char`/`short`/`long`/`long long`/`unsigned`/`signed`, float and double; pointers, pointer-to-pointer, pointer-to-const, const-pointer; 1D and 2D arrays; block scope locals including `register`. |
| `parse_function_definition.c` | Function definition with parameters, `void` parameter list, empty body, `static inline` specifiers. |
| `parse_function_calls.c` | Calls with multiple arguments, single argument, no arguments, and nested calls as arguments. |
| `parse_expressions.c` | All binary arithmetic/bitwise/shift operators, all comparison operators, logical `&&`/`\|\|`/`!`, unary `-`/`+`/`~`/`!`, `sizeof` on type and expression, ternary operator, every compound assignment operator. |
| `parse_precedence.c` | Precedence and associativity: `*` vs `+`, parenthesised override, `+`/`-` left associativity, `<<` vs `+`, `&` vs `\|`, `&&` vs `\|\|`, nested and chained ternaries. |
| `parse_pointers.c` | Address-of and dereference, double pointers and double dereference as assignment target, pointer arithmetic (`p + 1`, `p - 1`, `*(arr + 2)`), array subscript. |
| `parse_increment.c` | Prefix and postfix `++`/`--` as statements and as sub-expressions of an assignment, on both integers and pointers. |
| `parse_cast.c` | Casts between arithmetic types, casts to and from pointer types (`(void *)&i`), dereferencing a cast pointer (`*(int *)ptr`). Char values written as integers since char literals are not supported yet. |
| `parse_sizeof.c` | `sizeof` on primitive types, pointer types, single and double pointers, declared variables, arrays, struct variables, and `struct` type names. |
| `parse_struct.c` | `struct` definitions, structs nested by value, `union` definitions, self referencing structs via pointer, a global of struct type, member access via `.` and `->`. |
| `parse_compound.c` | Compound literals: scalar (`(int){42}`, `(float){3.14}`), array (`(int[3]){1, 2, 3}`) with subscripting, struct with designated initializers, address of a compound literal followed by `->` access. |
| `parse_initializer.c` | Scalar initializers, array initializers mixing positional and `[0] =` designators, compound literal initializer, struct initializers mixing `.field =` designations with positional values through a `typedef struct`. |
| `parse_if_else.c` | `if`, `if`/`else`, `else if` chains, nested `if` inside `if`. |
| `parse_comma.c` | Comma operator in an expression statement and inside parentheses as a return value, multi declarator declaration. |
| `parse_switch.c` | `switch` with `case` and `default` labels, stacked labels, fallthrough, `default` before `case`, empty body, non-block body, declaration inside the switch body, nested switches, expression conditions, and constant expression case labels (`sizeof`, `1 << 4`, negative). |
| `parse_loops.c` | `while`, `do while` and `for` in block, non-block and empty body forms; complex and bare conditions; `for` with expression init, declaration init, multiple declarators, comma clauses, each clause omitted and `for (;;)`; nested and mixed loops; declarations scoped to loop bodies. |
| `parse_break_continue.c` | `break` and `continue` in `while`, `do while` and `for` loops, `break` in `switch`, both inside non-block loop bodies and `if`/`else` branches, nested loops, and nested plain blocks inside a loop. |
| `parse_goto_labels.c` | `goto` with forward and backward jumps, multiple labels in one function, labels on empty statements, blocks and loops, chained labels, labels inside nested blocks, jumping out of nested loops, and the same label name reused in a different function. |
| `parse_enum.c` | `enum` definitions with implicit, explicit and mixed enumerator values; values from constant expressions referencing earlier enumerators, `sizeof` and negatives; trailing comma; single enumerator; anonymous enums; named and anonymous `typedef enum`; fixed underlying type (`enum E : unsigned int`) in both definition and use; enums as globals, locals, parameters, return types and struct members; block scoped enum definitions; enum constants in initializers, array sizes, conditions and expressions; `sizeof` of enum types and constants. |
| `parse_attributes.c` | `[[...]]` specifiers on external declarations, function definitions and declarations, block scoped declarations, and every statement kind (expression, empty, block, `if`, `switch`, `while`, `do while`, `for` including its init declaration, `goto`, `break`, `continue`, `return`, labels); empty `[[]]`, multiple specifiers, multiple attributes per specifier, attribute arguments, string arguments, and `vendor::name` namespaced attributes; on `enum` specifiers and enumerators. GNU `__attribute__((...))` leading and trailing on declarations, on function definitions, `struct`/`union` specifiers and members, `enum` specifiers and enumerators, locals, with arguments and string arguments; both styles combined on one declaration. |
| `reparse_function_definition.c` | reparse path, entered by reporting every `CN_MESSAGE_PARSED_FUNCTION` as modified, which loads the checkpoint and reparses the function definition instead of parsing it. Covers rebuilding of types and bindings over declaration specifiers, declarators, init declarators, initializers with designators, `struct`/`union`/`enum` specifiers, typedefs, `[[...]]` and GNU attribute specifiers, and over every statement kind, including labels, `goto`, nested blocks with shadowing, block scoped types, `static` locals, recursion, and a definition preceded by its own prototype. |

---

## Running Tests

```bash
# Build and run all tests
./nob test

# Run specific test
./nob test tests/parse_expressions.c

# Record new test output
# If needed, if not just create create empty new file
./nob record tests/new_test.c
# Or
touch tests/new_test.stdout.txt

# Full rebuild + test, without running demo processing
./nob clean
./nob lib
./nob test
```
