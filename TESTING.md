# Testing Guide for cnotes.h

## Testing Infrastructure

The project uses a **record/test snapshot system** built with `nob.h`:

### Commands

```bash
./nob test                     # Run all tests
./nob test tests/mytest.c      # Run specific test
./nob record tests/mytest.c    # Record expected output for a test
```

### How It Works

1. **Recording**: `./nob record` compiles and runs the test, capturing stdout to a `.stdout.txt` file (golden file)
2. **Testing**: `./nob test` compiles and runs the test, comparing stdout against the recorded `.stdout.txt`
3. **Results**: `SUCCESS`, `BUILD_FAIL`, `RUNTIME_FAIL`, or `UNEXPECTED_OUTPUT`

---

## Writing Tests

### Test Patterns

#### Pattern 1: Pure Assertion (Silent Success)
Use for data structures, pure functions, type system checks.

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
**Golden file**: Empty (assertions pass silently)

#### Pattern 2: Output Capture
Use for debug/print functions, format verification.

```c
#include "../cnotes.h"

int main(void) {
    int *table = cn_hash_table_make(...);
    cn_hash_table_print((void **)&table);  // Output captured
    return 0;
}
```
**Golden file**: Contains expected stdout

#### Pattern 3: Diagnostic/Error Tests
Use for parser errors, warnings, diagnostic messages.

```c
#include "../testing_utility.h"

const Cn_String src = CN_STR_BUFFER(
    "int main(void) {\n"
    "    return 0\n"  // Missing semicolon
    "}\n"
);

int main(void) {
    cn_diagnostic_handler = &cn_test_diagnostic_handler;
    Cn_Translation_Unit tu = cn_tu_make("input.i", .source = src);
    cn_tu_process(&tu, CN_NO_CODE_OUTPUT);
    return 0;
}
```
**Golden file**: JSON diagnostic output

### Important Notes

- Tests are in the `tests/` directory

---

## Current Test Coverage

| Test File | Component | Pattern |
|-----------|-----------|---------|
| `demo.c` | Basic sanity | 1 |
| `array_list_append.c` | Array list operations | 1 |
| `chained_arena.c` | Arena allocator | 1 |
| `hash_table_put.c` | Hash table put/get | 2 |
| `hash_table_remove.c` | Hash table removal | 1/2 |
| `hash_set.c` | Hash set operations | 1/2 |
| `string_builder.c` | String builder | 1 |
| `parse_function_definition.c` | Function definitions | 3 |
| `parse_variable_declaration.c` | Variable declarations, types | 3 |
| `parse_if_else.c` | If/else statements | 3 |
| `parse_expressions.c` | Binary, unary, sizeof, ternary ops | 3 |
| `parse_struct.c` | Struct/union definitions, member access | 3 |
| `string_operations.c` | String functions (`cn_str_*`) | 1 |
| `hash_functions.c` | Hashing functions (`cn_hash_*`) | 1 |
| `literal_parsing.c` | Integer/float literal parsing | 1 |
| `lexer_tokens.c` | Token recognition (keywords, operators, literals) | 1 |
| `lexer_state.c` | Lexer state (peek, expect, navigation) | 1 |
| `type_system.c` | Type creation, predicates, comparison | 1 |

---

## Known Parser Limitations

### Documented (from README TODOs)

| Feature | Status | Notes |
|---------|--------|-------|
| `for` loop | Not implemented | `cn_ast_parse_iteration_statement` is stubbed |
| `while` loop | Not implemented | Same stub |
| `do-while` loop | Not implemented | Same stub |
| Labeled statements | Not implemented | `case`, `default`, goto labels |
| Initializer lists | Not implemented | `{ expr, expr, ... }` |
| Compound literals | Not implemented | `(type){ ... }` |
| K&R function definitions | Not supported | Old-style parameter lists |
| `_Atomic(type)` specifier | Not implemented | Qualifier form works |
| `_Complex` type | Not implemented | Token not in keyword table |
| Hex float literals | Not implemented | `0x1.8p+1` format |
| Variable Length Arrays | Not implemented | Requires runtime size |
| Enum parsing | Incomplete | Tag binding, constants, stringify |

### Discovered Through Testing

| Feature | Error | Notes |
|---------|-------|-------|
| `long double` | "Specified type width on non 'int' type" | Type specifier not fully supported |
| Multi-dimensional arrays | "Expected ';' at the end of declaration" | `int arr[5][10]` fails |
| Binary literals | Returns false | `0b1010` not supported |
| Octal literals | Lexer issue | `0777` not tokenized correctly |
| Char literals | Lexer + Parsing issue | `A` not tokenized at all |
| Address of lvalue | Typechecking issue | Taking address of local variable causes lvalue error diagnostic |

---

## Areas Needing Tests

### Priority 1: Foundation
- [x] String operations (`cn_str_*`)
- [x] Hashing functions (`cn_hash_*`)
- [x] Literal parsing (`cn_parse_int_literal`, `cn_parse_float_literal`)

### Priority 2: Lexer
- [x] Token recognition (keywords, operators)
- [x] Lexer functions (`cn_lexer_init`, `cn_lexer_next_token`, `cn_lexer_peek`)

### Priority 3: Type System
- [x] Type creation/comparison (`cn_type_equals`, `cn_type_make_pointer`)
- [x] Type predicates (`cn_type_is_*`)

### Priority 4: Parser (more coverage)
- [ ] Function calls and arguments
- [ ] Cast expressions
- [ ] Pointer arithmetic
- [ ] More error cases

### Priority 5: AST Building (Meta-programming)
- [ ] `cn_build_*` functions
- [ ] AST modification

---

## Running Tests

```bash
# Build and run all tests
./nob test

# Run specific test
./nob test tests/parse_expressions.c

# Record new test output
./nob record tests/new_test.c

# Full rebuild + test
./nob cn
```
