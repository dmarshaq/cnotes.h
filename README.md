# cnotes.h — Single Header C Meta Programming Library

**A header-only library for building C meta programs, static analysis tools, and source transformation pipelines — operating directly on pre-processed `.i` files.**

![License](https://img.shields.io/badge/License-MIT-blue)
![C](https://img.shields.io/badge/C-blue)

---

## Overview

`cnotes.h` is a self-contained, single-header C library that parses pre-processed C translation units (`.i` files), builds a typed Abstract Syntax Tree, resolves symbols and bindings, and evaluates constant expressions — all within a single `#include`. It is designed for authors who need code generation and introspection capabilities without too much complexity and overhead. Aiming to give practical tools for various meta programming techniques that utilize transformation and introspection of C source at compile time.

The library is influenced by the design philosophy of [`stb`](https://github.com/nothings/stb) and [`nob`](https://github.com/tsoding/nob.h): easy integration with no dependencies, no giant build systems, just one file.

---

## Features & Scope

`cnotes.h` end goal is to provide integration with major C compilers like, `GCC`, `Clang` and `MSVC` as well as be able to understand their various syntactical features and details. And be compatible with `C99` standard.

**As of right now:** The library keeps up only with `GCC` and some of the `GNU` extension respectively, it is **NOT** fully completed to satisfy outlined goals *yet...* That is due to sheer complexity and alterations different compilers and their extensions introduce, which will be supported as library progresses.

Following is the list of features that are still not implemented, but will be in the future.

 -  **VLA, variable length arrays support.**
 -  **Struct and Union empty member declarations.**
 -  **ASM definitions.**
 -  **K&R-style function definitions.**
 -  **_Atomic specifiers.**
 -  **_Complex types.**
 -  **HEX float literals.**
 -  **FAM, Flexible Array Member.**
 -  **MSVC Specific compiler extensions.**

Note that this list might NOT include every feature that was not implemented, and can be added to as they are discovered or suggested.

---

## Quick Start

The library was designed for the specific pipeline in order to provide most effective and powerful introspection and code modification. Basically looking like this:
```
Source Files                    *.c
|
|       Compiler Pre-Processor.
|
Translation Units               *.i
|
|       cnotes.h processing:
|       Messaging loop where meta programs can be hooked.
|       Abstract Syntax Tree modifications and introspections occur here.
|
Modified Translation Units      *.i
|
|       Compiler.
|
Target Binary                   
```
Manual example usage is outlined below.

### 1. Obtain the header

Copy `cnotes.h` into your project. No other files are required.

### 2. Pre-process your source file

`cnotes.h` operates on **pre-processed** `.i` files — macros must already be expanded in order for the library to parse C files correctly. Use your compiler's pre-processor:

```sh
# GCC
# Pre-Processing my_main.c
gcc -E -o my_main.i my_main.c
```

### 3. Use the library

In **exactly one** translation unit define `CN_IMPLEMENTATION` before the include. Every other file that uses the API just includes the header normally. 

You want to have two `.c` files, one for meta program code *that defines and uses cnotes.h* and main file that  meta program will process.

```c
// meta.c — the one file that holds the implementation of cnotes.h
// sets everything up and perfoms operations on the main.c file.
#define CN_IMPLEMENTATION
#include "cnotes.h"

int main(void) {
    // Allocate AST data storage.
    Cn_Ast_Data ast_data = {0};
    cn_ast_init(&ast_data);

    // Open and process a pre-processed file.
    Cn_Translation_Unit tu = cn_tu_make("my_file.i");
    if (cn_tu_process(&tu, CN_PRINT_AST | CN_PRINT_BINDINGS) != 0) {
        fprintf(stderr, "Processing failed with %lld error(s)\n", ast_data.error_count);
        cn_tu_free(&tu);
        return 1;
    }

    cn_tu_free(&tu);
    return 0;
}
```

### 4. Processing file and compiling

```sh
# GCC
# Compiling meta program
gcc -o meta meta.c
# Running meta program
./meta
# Compiling meta-processed my_main.i
gcc -o my_main my_main.i
```
 
---

## License

MIT — see the `LICENSE` file or the header of `cnotes.h`.
