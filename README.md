# cnotes.h — Single-Header C Pre-Processor Library

**A header-only library for building C meta-programs, static analysis tools, and source transformation pipelines — operating directly on pre-processed `.i` files.**

![License](https://img.shields.io/badge/License-MIT-blue)
![C](https://img.shields.io/badge/C-blue)

---

## Overview

`cnotes.h` is a self-contained, single-header C library that parses pre-processed C translation units (`.i` files), builds a typed Abstract Syntax Tree, resolves symbols and bindings, and evaluates constant expressions — all within a single `#include`. It is designed for authors of refactoring tools, linters, code generators, and any program that needs to inspect or transform C source at compile time.

The library is influenced by the design philosophy of [`stb`](https://github.com/nothings/stb) and [`nob`](https://github.com/tsoding/nob.h): no build system, no dependencies, just one file.

---

## Features

- **Full C lexer** with look-ahead/look-behind ring buffer, keyword recognition, comment skipping, and GCC line-marker parsing
- **Recursive-descent C parser** producing a typed, intrusive AST stored in a flat array — no heap fragmentation, pointer-stable indices
- **Type inference and type checking** for all expression forms: binary, unary, cast, ternary, assignment, postfix, `sizeof`, member access, and function calls
- **Symbol and tag binding system** with proper scope shadowing, redeclaration/redefinition detection, and typedef/function/variable/enum-constant kinds
- **Compile-time constant expression evaluator** supporting integer and floating-point arithmetic, bitwise ops, shifts, logical short-circuit, ternary, sizeof, and casts
- **Struct and union layout computation** — member offsets, alignment padding, and size propagation
- **Chained arena allocator** for all AST and type data — pointer-stable, O(1) alloc, scope-pop dealloc
- **Generic array list, hash table, and hash set** — header-embedded, type-safe via macros, with automatic resizing
- **String builder** with stack-small-buffer optimisation
- **Diagnostic system** with source-snippet rendering, ANSI colour output, and pluggable handlers
- **Translation unit API** — open a `.i` file, process it to a typed AST, inspect or modify it, free everything cleanly
- **GCC extension support**: `__attribute__`, `__typeof__`, `__asm__`, `__extension__`, `__restrict`, `__inline`, etc.

---

## Quick Start

### 1. Obtain the header

Copy `cnotes.h` into your project. No other files are required.

### 2. Pre-process your source file

`cnotes.h` operates on **pre-processed** `.i` files — macros must already be expanded. Use your compiler's pre-processor:

```sh
# GCC / Clang
gcc -E -o my_file.i my_file.c

# MSVC
cl /P my_file.c
```

### 3. Use the library

In **exactly one** translation unit define `CN_IMPLEMENTATION` before the include. Every other file that uses the API just includes the header normally.

```c
// main.c  — the one file that holds the implementation
#define CN_IMPLEMENTATION
#include "cnotes.h"

int main(void) {
    // Allocate AST data storage
    Cn_Ast_Data ast_data = {0};
    cn_ast_init(&ast_data);

    // Open and process a pre-processed file
    Cn_Translation_Unit tu = cn_tu_make("my_file.i");
    if (cn_tu_process(&tu, CN_PRINT_AST | CN_PRINT_BINDINGS) != 0) {
        fprintf(stderr, "Processing failed with %lld error(s)\n",
                ast_data.error_count);
        cn_tu_free(&tu);
        return 1;
    }

    cn_tu_free(&tu);
    return 0;
}
```

---

## Customisation

All allocator calls, assertions, and ANSI colour output can be overridden before the `#include`:

```c
#define CN_REALLOC  my_realloc
#define CN_FREE     my_free
#define CN_ASSERT   my_assert
#define CN_ANSI_NO_COLOR   // disable colour escape codes
#define CN_IMPLEMENTATION
#include "cnotes.h"
```

---

## Supported C Subset

`cnotes.h` handles the output of a standard C pre-processor (`gcc -E`) including:

- All primitive types: `char`, `short`, `int`, `long`, `long long`, `float`, `double`, `_Bool`, `void`, `unsigned`/`signed` variants
- Pointers, arrays, function types, structs, unions (enums are partially supported)
- `typedef`, `extern`, `static`, `register`, `auto`, `inline`, `_Noreturn`
- `const`, `volatile`, `restrict`, `_Atomic` qualifiers
- All C operators at correct precedence and associativity
- `sizeof` for both types and expressions
- GCC extensions: `__attribute__`, `__typeof__`, `__asm__` labels, `__extension__`, `__restrict`, `__inline`
- Compound statements, `if`/`else`, `switch`, `return`, `goto`, `break`, `continue`
- Variadic functions (`...`)
- Adjacent string literal concatenation

---

## Roadmap / TODO
 
Items below are either marked `CN_TODO` in the source, stubbed out with an empty `goto error`, or explicitly listed as incomplete in comments. They are grouped by the subsystem they belong to.
 
---
 
### Parser — missing statement forms
 
- **`for` loop** — `cn_ast_parse_iteration_statement` is stubbed; the `CN_AST_NODE_ITERATION_STATEMENT` node kind exists but the parser immediately returns `NIL`. Full `for (init; cond; post)` parsing with a new scope for the init-declaration is needed.
- **`while` and `do`/`while` loops** — same stub; both are routed through `cn_ast_parse_iteration_statement` with no implementation.
- **Labeled statements** — `cn_ast_parse_labeled_statement` is a stub. Needed for `case`, `default`, and named `goto` targets. The `CN_AST_NODE_LABELED_STATEMENT` node kind is declared but unused.
- **`switch` body** — `switch` is parsed as a selection statement but `case`/`default` labels inside its body rely on the missing labeled-statement support.
- **`goto` target resolution** — identifiers after `goto` are parsed and stored but never validated against declared labels in the same function.
- **`asm` definition at file scope** — `cn_ast_parse_asm_definition` immediately calls `CN_TODO`. Top-level `asm("...")` blocks are skipped entirely.

---
 
### Parser — missing expression / declaration forms
 
- **Initialiser lists** — `cn_ast_parse_initializer` currently only handles a single expression; the `{ expr, expr, ... }` brace-initialiser form for arrays and structs is noted as `TODO` in the source.
- **Compound literals** — `(type){ ... }` is detected as a cast expression but the brace body is not parsed; the compound-literal path is left as a `TODO` comment inside `cn_ast_parse_expression_leaf`.
- **K&R-style function definitions** — the old-style parameter declaration list that appears between the declarator and the function body is explicitly excluded from `cn_ast_parse_function_definition_or_declaration`.
- **`_Atomic(type)` specifier** — the atomic *qualifier* (`_Atomic int`) is handled, but the atomic *specifier* form `_Atomic(int)` (with parentheses) is explicitly skipped with a `return 1` in `cn_ast_try_parse_qualifier` and noted as incomplete.
- **`_Complex` type** — the token `_Complex` is not in the keyword table; the specifier is listed as `INCOMPLETE` in `cn_ast_try_parse_type_specifier`.
- **Hex float literals** — `cn_parse_float_literal` documents that hex floats (`0x1.8p+1`) are not handled and will return `false`.

---
 
### Type system — enums
 
- **`enum` specifier parsing** — `cn_ast_parse_struct_or_union_specifier` handles only `struct` and `union`; enum parsing is not routed here and the `CN_TOKEN_ENUM` branch in `cn_ast_starts_type` is present but no parser function exists for it.
- **`enum` tag binding** — `cn_ast_tag_binding_declare` calls `CN_TODO("Enum binding.")` for the `CN_ENUM` kind in both the new-declaration and shadow-declaration paths.
- **`enum` constant bindings** — `Cn_Ast_Binding_Enum_Constant` is declared but its body is empty (`// TODO: Binding enum constant.`) and no declare function exists yet.
- **`enum` stringify** — `cn__type_stringify` calls `CN_TODO("enum stringify.")` for the `CN_ENUM` case.
- **`cn_type_hash` / `cn_type_equals` for enums** — both functions have empty `case CN_ENUM: break;` bodies, so enum types are not properly deduplicated in the type set.

---
 
### Type system — unions in type checking
 
- **`cn__ast_access_expression_typecheck`** — member access on union types is not yet allowed; the check `struct_type->kind != CN_STRUCT` rejects unions with "Member access on non-struct type." The comment reads `// TODO: Allow CN_UNION once union types are implemented.`
- **`cn_type_hash` / `cn_type_equals` for unions** — same as enums; the `case CN_UNION: break;` bodies are empty placeholders.

---
 
### Source modification pipeline
 
- **`CN_REMOVE` modification kind** — `cn_tu_process` processes `CN_INSERT` modifications but calls `CN_TODO("Implement remove modification.")` for `CN_REMOVE`. The `Cn_Modification_Remove` struct and enum value are declared and ready.
- **Modification overlap resolution** — a `// TODO: Account for overlaps, and resolve them.` comment sits at the top of the modification-application loop; overlapping or out-of-order patches are not detected.

---
 
### Binding and symbol table
 
- **`cn_ast_binding_table_put` / `cn_ast_binding_table_get` documentation** — both functions are marked `// TODO: Write documentation.` in the header.
- **`cn__ast_add_type_if_not` exposure** — the function is declared `CNDEF` and has a `// TODO: Expose to the user and document.` comment; it is currently internal-only.

---
 
### AST printing
 
- **`CN_AST_NODE_GNU_ATTRIBUTE_SPECIFIER` node in `cn_ast_print`** is not given a named case; it falls through to the default `printf("\n")` branch, so attribute specifiers print no detail.
- **`Cn_Ast_Node_Iteration_Statement`** — the union member is commented out in `Cn_Ast_Node` (`// Cn_Ast_Node_Iteration_Statement iteration_statement;`), pending the iteration-statement implementation.
- **`Cn_Ast_Node_Asm_Definition`** — similarly commented out (`// Cn_Ast_Node_Asm_Definition asm_definition;`).
- **`Cn_Ast_Node_Labeled_Statement`** — similarly commented out (`// Cn_Ast_Node_Labeled_Statement labeled_statement;`).

---
 
### VLA (Variable Length Arrays)
 
- Array declarators currently require a compile-time-constant expression. The comment in `cn_ast_to_type` reads `// TODO: Handle VLA.`; if `cn_ast_expression_evaluate` returns an empty `Cn_Any`, the declarator is rejected with `CN_DC_INVALID_CONSTANT_EXPRESSION` instead of accepting a runtime-sized array.

---
 
### `Cn_Message` / post-processing hooks
 
- The `Cn_Message` / `Cn_Message_Handler` API (`cn_message_handler`) is declared and the handler pointer is exported, but no messages are ever enqueued or fired during `cn_tu_process`. The comment in the `cn_tu_process` docs says "messages are enqueued … and by the end of the process they are triggered" — this mechanism is not yet wired up.

---

## License

MIT — see the `LICENSE` file or the header of `cnotes.h`.
