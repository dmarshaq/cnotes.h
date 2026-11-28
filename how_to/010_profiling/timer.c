// This how_to is about composing meta programs, so it is worth saying up front what is new here
// and what is not.
//
// `defer.c` is not new. It is the exact same file as the one in `005_defer`, symlinked, not
// copied. The only new meta program is this one, and all it does is turn:
// ```c
//      [[time]] int sum_to(int n) {
//          ...
//      }
// ```
//
// Into:
// ```c
//      int sum_to(int n) {
//          clock_t cn__timer_start = clock();
//          [[defer]] {
//              clock_t cn__timer_end = clock();
//              printf(...);
//          }
//          ...
//      }
// ```
//
// Note what it generates: not the timing code in its final shape, but a `[[defer]]` for somebody
// else to deal with. This meta program never thinks about where the function returns, how many
// exits it has, or what its return type is. `defer.c` already knows all of that, so `[[time]]`
// gets to be about timing and nothing else.
//
// The handoff works because of `ast_is_modified`. Setting it makes the library emit the modified
// function back to text and parse it again, so once `defer_hook` is called the `[[defer]]` written
// above is a real attribute on a real ast node, and `defer_hook` picks it up like any other defer
// the user could have written by hand.

#include "cnotes.h"

Cn_Hook_Response timer_hook(Cn_Ast_Node **node_ptr, bool *ast_is_modified, bool first);

// Extension that adds `timer_hook` to the library, it is registered in `meta.c`.
bool timer_extension(Cn_Hooks *hooks) {
    cn_array_list_append(&hooks->ast_parsed_hooks, timer_hook);
    return true;
}

Cn_Hook_Response timer_hook(Cn_Ast_Node **node_ptr, bool *ast_is_modified, bool first) {
    (void) first;

    if ((*node_ptr)->kind != CN_AST_FUNCTION) return CN_HOOK_RESPONSE_NONE;

    Cn_Ast_Function *function = cn_ast_as(Function, *node_ptr);
    Cn_Ast_Nodes attr_specs = cn_get_attribute_specifiers(function);

    // `[[time]]` sits on the function itself, so unlike `defer` this hook can look at the function's own
    // attribute specifiers and skip every function that carries no attributes at all, without walking anything.
    if (function->attribute_specifiers.length == 0) return CN_HOOK_RESPONSE_NONE;

    if (cn_get_attribute(attr_specs, CN_STR_LIT("time")) == NULL) return CN_HOOK_RESPONSE_NONE;

    // IMPORTANT: Removing the attribute.
    //
    // Reporting a modification below sends this very function through the hook again, so if
    // `[[time]]` were left in place we would generate another timer, report another modification,
    // and never stop. Any hook that sets `ast_is_modified` has to consume what made it do so.
    cn_ast_remove_attribute(attr_specs, CN_STR_LIT("time"));

    // Getting the function's name to print, `cn_get_declarator_info` walks the declarator down to
    // the identifier it declares, which for a function definition is the function name.
    Cn_Ast_Identifier *identifier;
    cn_get_declarator_info(function->declarator, (Cn_Ast_Node **) &identifier);

    // Generating the timer as plain text.
    //
    // IMPORTANT: `1000000.0` is `CLOCKS_PER_SEC` written out by hand, and it has to be.
    // A meta program runs after the pre-processor, so the `.i` file it works on has no macros left
    // in it, and any macro name this code generates would reach the compiler undefined. The same
    // goes for anything else that only exists before pre-processing.
    //
    // NOTE: The generated code is written where the user's body starts, which makes it the first
    // defer registered in the function, and defers unwind in reverse order. So the timer stops
    // after every other defer in the function has run, which is exactly what you want out of a
    // measurement: it covers the clean up too.
    Cn_Ast_Code *timer = cn_build_format(
            "clock_t cn__timer_start = clock();\n"
            "[[defer]] {\n"
            "    clock_t cn__timer_end = clock();\n"
            "    printf(\"[time] %%s: %%.3f ms\\n\", \"%.*s\", (double)(cn__timer_end - cn__timer_start) * 1000.0 / 1000000.0);\n"
            "}\n",
            CN_STR_UNPACK(identifier->name)
            );

    // Prepending it to the function's body.
    Cn_Ast_Block *block = function->block;
    Cn_Ast_Node **items = cn_array_list_make(Cn_Ast_Node *, block->block_items.length + 1);

    cn_array_list_append(&items, (Cn_Ast_Node *) timer);

    for (int64_t i = 0; i < block->block_items.length; i++) {
        cn_array_list_append(&items, block->block_items.ptrs[i]);
    }

    block->block_items = cn_build_list_from((void **) items, cn_array_list_length(&items));

    cn_array_list_free(&items);

    // Reporting the modification, which is what gets the generated text parsed into ast nodes and
    // handed to the following hooks, `defer_hook` included.
    *ast_is_modified = true;
    return CN_HOOK_RESPONSE_NONE;
}
