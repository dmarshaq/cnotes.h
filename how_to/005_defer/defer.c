// This how_to implements `defer` in the simplest shape that is still useful: function scope only.
//
// The rules it plays by are:
//  1) Only a `[[defer]]` written directly in the function's own block is picked up.
//     One written inside a nested block is reported as an error, see `020_block_defer`
//     for the implementation that gives every scope its own defers.
//  2) Deferred statements run in reverse order of registration, right before the function exits.
//  3) Every `return` is rerouted through them, no matter how deep in the function it sits.
//  4) `break` and `continue` are left alone. They never leave the function, so a function scoped
//     defer has nothing to do about them.
//
// Rule 4 is what keeps this file short. The only exit worth caring about is `return`, which means
// all the clean up code can be emitted once, in one place: the end of the function.
//
// So this input:
// ```c
//      int load(int x) {
//          [[defer]] cleanup();
//
//          if (x < 0) return -1;
//          return x;
//      }
// ```
//
// Comes out as:
// ```c
//      int load(int x) {
//          int cn__defer_rvalue;
//
//          if (x < 0) { cn__defer_rvalue = -1; goto cn__defer0; }
//          { cn__defer_rvalue = x; goto cn__defer0; }
//      cn__defer0: cleanup();
//          return cn__defer_rvalue;
//      }
// ```
//
// The returned value is stashed in `cn__defer_rvalue` because the expression has to be evaluated
// where the user wrote the `return`, not after the clean up code had a chance to touch the
// variables it reads.

#include "cnotes.h"
#include <stdint.h>

// Everything we need to remember about a single deferred statement.
//
// `id` names the label this entry gets, and `jumped_to` records whether any `return` was actually
// rerouted here. Entries nobody jumps to don't need a label at all, they just run by falling
// through the clean up chain on the normal path.
typedef struct {
    uint32_t        id;
    bool            jumped_to;
    Cn_Ast_Node *   statement;
} Deferred_Entry;

// The deferred stack is an array list holding every entry registered so far in the function
// currently being processed. A `return` always jumps to whatever sits on top of it at that point,
// which is exactly the set of defers that had run by the time the user's `return` was reached.
Deferred_Entry *deferred_stack;

// Two more pieces of per function state.
//
// `defer_function_returns_void` decides whether a rerouted `return` needs `cn__defer_rvalue`,
// and `defer_rerouted_return` records whether any `return` got rerouted at all, which is what
// tells us at the end if the clean up chain has to finish with a real `return`.
bool defer_function_returns_void;
bool defer_rerouted_return;

// Helper that walks a statement that is not itself deferred, replacing any `return` it finds.
// Sub blocks are walked too, a `return` nested three loops deep still has to run the defers.
void defer_replace_returns(Cn_Ast_Node **statement_ptr);

// Helper that builds the replacement for one `return` statement.
Cn_Ast_Node *defer_make_return_replacement(Deferred_Entry *entry, Cn_Ast_Node *return_expression);

Cn_Message_Response defer_handler(Cn_Message *message) {
    // Filtering messages down to parsed function definitions.
    //
    // NOTE: Unlike `001_basic_usage` we deliberately do not filter on `CN_MESSAGE_AST_UNMODIFIED`.
    // If another meta program modified the function and the library reparsed it, we still want to
    // see the result, because that other meta program might have generated `[[defer]]` itself.
    // That is precisely how `010_profiling` builds on top of this file.
    if (message->kind != CN_MESSAGE_AST_PARSED)         return CN_MESSAGE_RESPONSE_NONE;
    if (!(message->flags & CN_MESSAGE_AST_FUNCTION))    return CN_MESSAGE_RESPONSE_NONE;

    Cn_Ast_Function *function = cn_ast_as(Function, *((Cn_Message_Ast_Parsed *)message)->node_ptr);

    // Getting the function's type before touching anything, because the shape of a rerouted
    // `return` depends on it: a `void` function has no value to carry across the jump.
    //
    // There are two ways to introspect type. First is to reconstruct and infer type directly from ast nodes.
    // And second is to get function binding, which is basically function name that is binded to the type.
    // Both are valid choices, in here we use the first one since it requires nothing but the ast nodes
    // we already have. We pass all three main ast constructs that make up a C type, those being:
    //  1) Qualifiers: `const`, `volatile`, etc...
    //  2) Type Specifier: `int`, `void`, `struct ...`, `enum ...`, `short int`, etc...
    //  3) Declarator: `* foo(int x, int y)`, etc...
    // Since the node is a function definition the result is always a `Cn_Type_Function`, which the
    // assert below states.
    Cn_Type_Function *type = (Cn_Type_Function *) cn_analyze_to_type(
            function->declaration_specifiers->qualifiers,
            function->declaration_specifiers->type_specifier,
            function->declarator
            );
    CN_ASSERT(type->kind == CN_FUNCTION);

    // Important that we use `cn_type_unqualified` here, technically qualifiers can be added to the
    // type, so we need to strip them before checking kind, otherwise we might get `CN_QUALIFIED`
    // kind wrapping around `CN_VOID`.
    defer_function_returns_void = cn_type_unqualified(type->return_type)->kind == CN_VOID;
    defer_rerouted_return       = false;

    deferred_stack = cn_array_list_make(Deferred_Entry, 4);

    // `items` collects the rebuilt body of the function. Deferred statements are pulled out of it
    // while walking, and put back at the end in reverse order.
    Cn_Ast_Node **items = cn_array_list_make(Cn_Ast_Node *, 16);

    Cn_Ast_Block *block = function->block;
    bool found_defer    = false;

    for (int64_t i = 0; i < block->block_items.length; i++) {
        // Getting block item, and from it also the statement.
        Cn_Ast_Block_Item *item = cn_ast_as(Block_Item, block->block_items.ptrs[i]);
        Cn_Ast_Node *statement  = item->declaration_or_statement;

        if (cn_get_attribute(statement, CN_STR_LIT("defer")) != NULL) {
            // A deferred statement is moved as is, so it only has to be something that survives
            // being written somewhere else. An expression statement and a block cover everything
            // clean up code ever needs, anything else is far more likely to be a mistake.
            if (statement->kind != CN_AST_EXPRESSION_STATEMENT && statement->kind != CN_AST_BLOCK) {
                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, statement, CN_DC_ILLEGAL_ATTRIBUTE, "Expected 'defer' attribute on expression statement or block.");
                continue;
            }

            // Removing the attribute, mostly for clarity, it also saves future iteration of defer meta program to not trigger on same statements, 
            // the statement itself is emitted later.
            cn_ast_remove_attribute(statement, CN_STR_LIT("defer"));

            // Pushing the entry and continuing, note that the statement is NOT appended to `items`
            // here, that is the whole point, it leaves the place it was written in.
            static uint32_t id_counter = 0;
            cn_array_list_append(&deferred_stack, ((Deferred_Entry) {
                        .id         = id_counter++,
                        .jumped_to  = false,
                        .statement  = statement,
                        })
                    );

            found_defer = true;
            continue;
        }

        // Everything else stays where it is, but any `return` hiding inside it, at any depth,
        // has to be rerouted through the defers registered so far.
        defer_replace_returns(&item->declaration_or_statement);

        cn_array_list_append(&items, (Cn_Ast_Node *)item);
    }

    // Nothing was deferred, so the body is untouched and there is nothing to emit.
    if (!found_defer) {
        cn_array_list_free(&items);
        cn_array_list_free(&deferred_stack);
        return CN_MESSAGE_RESPONSE_NONE;
    }

    // Emitting the clean up chain at the end of the function, popping the stack so the statements
    // come out in reverse order of registration.
    //
    // The chain is one straight line of code: jumping into the middle of it runs that entry and
    // every entry below it by simply falling through, which is exactly the unwinding order we want.
    while (cn_array_list_length(&deferred_stack) > 0) {
        Deferred_Entry entry = deferred_stack[cn_array_list_length(&deferred_stack) - 1];
        cn_array_list_pop(&deferred_stack);

        // No `return` ever jumped here, so this entry is only reachable by falling through and
        // does not need a label of its own.
        if (!entry.jumped_to) {
            cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_block_item(entry.statement));
            continue;
        }

        // Otherwise the statement gets wrapped in the label the rerouted `return`s jump to.
        Cn_Ast_Block_Item *label = cn_build_block_item(
                cn_build_label(
                    // This line is self explanatory, but it uses `cn_build_format`
                    // that doesn't produce ast nodes directly like other builders,
                    // instead it inserts a `Cn_Ast_Code` node with the specified formatted text,
                    // when this reaches the reparser it unwraps the text and parses it as usual.
                    // You will see more uses of it later, since it is a very flexible and 
                    // a quick way of generating code.
                    (Cn_Ast_Identifier *) cn_build_format("cn__defer%u", entry.id),
                    entry.statement,
                    ),
                );

        cn_array_list_append(&items, (Cn_Ast_Node *) label);
    }

    // Finally the declaration and the real `return`, both needed only if some `return` was
    // actually rerouted and the function has a value to give back.
    //
    // The trailing `return` needs no condition around it, unlike in `020_block_defer` where clean up
    // code sits in the middle of a function and control has to be told where to go next. Here the
    // chain is the last thing in the function, so anything reaching its end is on its way out.
    if (defer_rerouted_return && !defer_function_returns_void) {
        // In order to declare `cn__defer_rvalue` we can utilize a special library function that
        // directly builds a string out of a type and the declarator name we want at the end.
        // Thats one of the ways to do it, another way would be to copy the ast nodes that denote
        // the return type from the function, but this way is shorter since we already have `Cn_Type`.
        Cn_String buffer    = CN_STR_BUFFER_EMPTY(256);
        Cn_String type_str  = cn_type_stringify(buffer, type->return_type, CN_STR_LIT("cn__defer_rvalue"));

        // Prepending the declaration, so `cn__defer_rvalue` exists before the first jump assigns it.
        cn_array_list_add(&items, 0, (Cn_Ast_Node *) cn_build_format("%.*s;\n", CN_STR_UNPACK(type_str)));

        cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format("return cn__defer_rvalue;\n"));

    }

    // Converting the array list into the block's node list, replacing the original body.
    block->block_items = cn_build_list_from((void **) items, cn_array_list_length(&items));

    cn_array_list_free(&items);
    cn_array_list_free(&deferred_stack);

    return CN_MESSAGE_RESPONSE_MODIFIED;
}

// Walks one statement of the function looking for jumps that need rerouting.
//
// Everything here is a plain recursive descent, the only reason a statement is interesting is that
// it either is a `return`, or is something that can contain one.
void defer_replace_returns(Cn_Ast_Node **statement_ptr) {
    Cn_Ast_Node *statement = *statement_ptr;
    if (statement == NULL) return;

    // Any `[[defer]]` that reaches this function is nested somewhere inside the function scope,
    // the walk in `defer_handler` handles the function scope itself and never passes those here.
    if (cn_get_attribute(statement, CN_STR_LIT("defer")) != NULL) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, statement, CN_DC_ILLEGAL_ATTRIBUTE, "Expected 'defer' attribute in the function scope, this defer implementation only defers to the end of the function.");
        return;
    }

    switch (statement->kind) {
        case CN_AST_BLOCK:
            {
                Cn_Ast_Block *block = cn_ast_as(Block, statement);

                for (int64_t i = 0; i < block->block_items.length; i++) {
                    Cn_Ast_Block_Item *item = cn_ast_as(Block_Item, block->block_items.ptrs[i]);

                    defer_replace_returns(&item->declaration_or_statement);
                }
            }
            break;

        // The statements below are walked purely to reach the blocks they hold.
        case CN_AST_IF:
            defer_replace_returns(&cn_ast_as(If, statement)->then_statement);
            defer_replace_returns(&cn_ast_as(If, statement)->else_statement);
            break;

        case CN_AST_FOR:        defer_replace_returns(&cn_ast_as(For, statement)->body);        break;
        case CN_AST_WHILE:      defer_replace_returns(&cn_ast_as(While, statement)->body);      break;
        case CN_AST_DO_WHILE:   defer_replace_returns(&cn_ast_as(Do_While, statement)->body);   break;
        case CN_AST_SWITCH:     defer_replace_returns(&cn_ast_as(Switch, statement)->body);     break;
        case CN_AST_LABEL:      defer_replace_returns(&cn_ast_as(Label, statement)->statement); break;

        // The one jump that actually leaves the function.
        //
        // NOTE: `CN_AST_BREAK` and `CN_AST_CONTINUE` are not listed at all. They can only move
        // control around inside the function, so the function's defers are none of their business.
        case CN_AST_RETURN:
            // With an empty stack there is nothing registered yet, which means every defer in this
            // function is written below this `return` and this exit runs none of them.
            if (cn_array_list_length(&deferred_stack) > 0) {
                // Replacing the node by writing through `statement_ptr`, this is why the walk
                // passes `Cn_Ast_Node **` around instead of plain `Cn_Ast_Node *`.
                *statement_ptr = defer_make_return_replacement(
                        &deferred_stack[cn_array_list_length(&deferred_stack) - 1],
                        cn_ast_as(Return, statement)->expression
                        );
            }
            break;

        // `goto` is the one exit `defer` explicitly doesn't handle, it can jump anywhere including
        // straight out of the function's clean up chain, so all we can do is warn about it.
        case CN_AST_GOTO:
            if (cn_array_list_length(&deferred_stack) > 0) {
                cn_diagnostic_node(CN_DIAGNOSTIC_WARNING, statement, CN_DC_EXPECTED_AST_NODE, "Expected other statements but 'goto' in the function with defer, use of this 'goto' can lead to undefined behavioar, if it jumps over the deferred statements.");
            }
            break;

        default:
            break;
    }
}

// Builds the replacement for a `return` that has defers to run first, producing something like:
// ```c
//      {
//          cn__defer_rvalue = <return_expression>;
//          goto cn__defer<entry->id>;
//      }
// ```
Cn_Ast_Node *defer_make_return_replacement(Deferred_Entry *entry, Cn_Ast_Node *return_expression) {
    // Marking the entry, so `defer_handler` knows it has to give this one a label,
    // and that the clean up chain has to end with a real `return`.
    entry->jumped_to        = true;
    defer_rerouted_return   = true;

    Cn_Ast_Block_Item *goto_defer = (Cn_Ast_Block_Item *) cn_build_format("goto cn__defer%u;\n", entry->id);

    // `return;` carries nothing across the jump, so the jump is all there is to build.
    if (return_expression == NULL) {
        return cn_ast_new((Cn_Ast_Block) {
                    .kind = CN_AST_BLOCK,
                    .block_items = cn_build_list(goto_defer),
                });
    }

    // Otherwise the expression has to be evaluated right here, before any deferred statement runs,
    // since clean up code is free to change or free whatever the expression reads.
    //
    // In a `void` function there is nothing to keep it in, `return f();` with a `void` `f` is only
    // reachable for its side effects, so the call is emitted on its own.
    Cn_Ast_Node *expression = defer_function_returns_void
        ? return_expression
        : (Cn_Ast_Node *) cn_build_assign(
                CN_AST_ASSIGNMENT_OP_ASSIGN,
                cn_build_identifier(CN_STR_LIT("cn__defer_rvalue")),
                return_expression,
                );

    Cn_Ast_Block_Item *rvalue_assignment = cn_build_block_item(cn_build_expr_statement(expression));

    // In here we manually build `Cn_Ast_Block` and immediately return it,
    // composed of the two items made above.
    return cn_ast_new((Cn_Ast_Block) {
                .kind = CN_AST_BLOCK,
                .block_items = cn_build_list(rvalue_assignment, goto_defer),
            });
}
