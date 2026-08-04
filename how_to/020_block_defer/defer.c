// IMPORTNAT: This block defer implementation doesn't analyze control-flow of the function, it uses naive stack based walking of the ast,
// replacing any jumps and attaching clean up closures at the end of the scope.
// In the future this how_to will showcase defer using Control-Flow Graph analysis to properly find and resolve any jumps.

#include "cnotes.h"
#include <stdint.h>

// Defer takes in any function definitions, once were modified too, so to keep track which function definitions were deferred.
// We can use helper data structure that simply stores names of the functions that were deferred.
Cn_String *deferred_function_names = NULL;

// Here we just forward declare helper that will be used in the moment.
// And defined below. It will be used for recurisve walk of encountered block statements.
// RETURNS: `true` if modification occured, `false` otherwise.
bool defer_walk_block(Cn_Ast_Block *block);

// Second helper that will be used to explore statements, and potentially replace them if they are:
// `return`, `break` or `continue`.
// RETURNS: `true` if modification occured, `false` otherwise.
bool defer_explore_statement(Cn_Ast_Node **statement_ptr);

// Those flags are set for each entry to store information about early scope exits like, `return`, `break` or `continue`.
// NOTE: Goto is not stored here because `defer` explicitely doesn't handle arbitrary jumps that exit scopes.
// This also means warning is sent to the user if `goto` is encountered in the scoped with deferred statements.
typedef enum : uint32_t {
    DEFERRED_ENTRY_JUMPED_FROM_RETURN       = 0x1,
    DEFERRED_ENTRY_JUMPED_FROM_BREAK        = 0x2,
    DEFERRED_ENTRY_JUMPED_FROM_CONTINUE     = 0x4,
} Deferred_Entry_Flags;

// Struct contains all necessary information to generate proper exit for each deferred entry.
typedef struct {
    uint32_t                id;
    Deferred_Entry_Flags    flags;
    Cn_Ast_Node *           statement;
} Deferred_Entry;

// Deferred stack is array list that will store every deferred entry throughout the deferred function.
// Once `defer` encountered the deferred entry is pushed to the stack.
Deferred_Entry *deferred_stack;

// Those two mark stacks that denote closest break and continue closures.
// For example for `while`, `for`, `do {...} while` and `switch` new marks are pushed.
// Those marks are indicies in deferred_stack or rather points, if anything deferred after these points,
// the `break` and `continue` will unwrap into proper jump statements.
int64_t *closest_break_mark_stack;
int64_t *closest_continue_mark_stack;

// Here we also forward declare three node construction helpers, each one takes inc `Deferred_Entry`
// and potentially other data like `return_expression` node. They all output new statement that will contain
// jump mechanism that will be used to replace code's original `return`, `break` or `continue`.
Cn_Ast_Node *defer_make_return_replacement(Deferred_Entry *entry, Cn_Ast_Node *return_expression);
Cn_Ast_Node *defer_make_break_replacement(Deferred_Entry *entry);
Cn_Ast_Node *defer_make_continue_replacement(Deferred_Entry *entry);

Cn_Message_Response defer_handler(Cn_Message *message) {
    // This message `CN_MESSAGE_TU_START` is sent in the beginning of any translation unit processing.
    // It works well to initialize anything, and even prepend strings to the translation unit.
    // We will use it initialize `deferred_function_names` set.
    if (message->kind == CN_MESSAGE_TU_START) {
        deferred_function_names = cn_hash_set_make(Cn_String, 32, (Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);
        return CN_MESSAGE_RESPONSE_NONE;
    }

    // Likewise `CN_MESSAGE_TU_END` is sent after every external declaration was processed in translation unit.
    // We can use it to free `deferred_function_names` set.
    if (message->kind == CN_MESSAGE_TU_END) {
        cn_hash_set_free(&deferred_function_names);
        return CN_MESSAGE_RESPONSE_NONE;
    }

    if (message->kind != CN_MESSAGE_AST_PARSED)         return CN_MESSAGE_RESPONSE_NONE;
    if (!(message->flags & CN_MESSAGE_AST_FUNCTION))    return CN_MESSAGE_RESPONSE_NONE;

    Cn_Ast_Function *function = cn_ast_as(Function, *((Cn_Message_Ast_Parsed *)message)->node_ptr);


    // Making all stacks that will be used.
    deferred_stack = cn_array_list_make(Deferred_Entry, 4);
    closest_break_mark_stack    = cn_array_list_make(int64_t, 2);
    closest_continue_mark_stack = cn_array_list_make(int64_t, 2);

    // Walking and first block and processing defeer.
    bool modified = defer_walk_block(function->block);

    // Prepending `cn__defer_rvalue` and `cn__defer_state` if we know that at least one modification occured.
    if (modified) {
        Cn_Ast_Node **items = cn_array_list_make(Cn_Ast_Node *, 16);

        // `cn__defer_rvalue` is not declared if return type is void. 
        // As well as we strip any function specifiers from declaration_specifiers node,
        // so it doesn't conflict with regular declaration.
        {
            // There are two ways to introspect type. First is to reconstruct and infer type directly from ast nodes.
            // And second is to get function binding, which is basically function name that is binded to the type.
            // Both are valid choices, in here I will show how to use first method since it requires use of ast nodes.
            // We pass all three main ast constructs that make up C type, those are being:
            //  1) Qualifiers: `const`, `volatile`, etc...
            //  2) Type Specifer: `int`, `void`, `struct ...`, `enum ...`, `short int`, etc...
            //  3) Declarator: `* foo(int x, int y)`, etc...
            // All of that gets contructed into a types, specifically as result we get `Cn_Type_Function` since we know that ast node that we get info from is a function.
            // Below is assert `type->kind == CN_FUNCTION` checks that, because we always expect this to be true on `Cn_Ast_Function` nodes.
            Cn_Type_Function *type = (Cn_Type_Function *) cn_analyze_to_type(
                    function->declaration_specifiers->qualifiers, 
                    function->declaration_specifiers->type_specifier, 
                    function->declarator
                    );
            CN_ASSERT(type->kind == CN_FUNCTION);

            // Now we check that return type is not `CN_VOID`, important that we use `cn_type_unqualified` since technically 
            // qualifiers can be added to the type, so we need strip them before checking kind, 
            // otherwise we might get `CN_QUALIFIED` kind wrapping around `CN_VOID`.
            if (cn_type_unqualified(type->return_type)->kind != CN_VOID) {
                // In order to make declaration using `Cn_Type` struct we can utilize special library method,
                // That directly creates string with type and declarator name that we need at the end.
                // Thats one of the ways to do it, another way would be to copy ast nodes that denote the return type
                // from the function ast nodes, but this way is shorter since we already have `Cn_Type`.
                Cn_String buffer    = CN_STR_BUFFER_EMPTY(256);
                Cn_String type_str  = cn_type_stringify(buffer, type->return_type, CN_STR_LIT("cn__defer_rvalue"));

                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format("%.*s;\n", CN_STR_UNPACK(type_str)));
            }
        }

        // `cn__defer_state` can be simply inserted as a text.
        // We also add `[[maybe_unused]]` attribute so that compiler doesn't spit warning if unused.
        cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format("[[maybe_unused]] int cn__defer_state = 0;\n"));

        for (int64_t i = 0; i < function->block->block_items.length; i++) {
            cn_array_list_append(&items, function->block->block_items.ptrs[i]);
        }

        function->block->block_items = cn_build_list_from((void **) items, cn_array_list_length(&items));

        cn_array_list_free(&items);
    }

    // Freeing stacks.
    cn_array_list_free(&deferred_stack);
    cn_array_list_free(&closest_break_mark_stack);
    cn_array_list_free(&closest_continue_mark_stack);
    
    return CN_MESSAGE_RESPONSE_NONE;
}

bool defer_walk_block(Cn_Ast_Block *block) {
    bool result = false;

    Cn_Ast_Node **items         = cn_array_list_make(Cn_Ast_Node *, 16);
    int64_t deferred_stack_mark = cn_array_list_length(&deferred_stack);
    
    // Iterating over items in the block.
    for (int64_t i = 0; i < block->block_items.length; i++) {
        // Getting block item, and from it also statement.
        Cn_Ast_Block_Item *item = cn_ast_as(Block_Item, block->block_items.ptrs[i]);
        Cn_Ast_Node *statement = item->declaration_or_statement;

        // If statement contains defer, there is no need to know whats inside it if it is a block.
        // We just need to make sure that statement is of valid kind.
        if (cn_get_attribute(statement, CN_STR_LIT("defer")) != NULL) {

            // Validating kind.
            switch (statement->kind) {
                case CN_AST_DECLARATION:
                case CN_AST_EXPRESSION_STATEMENT:
                case CN_AST_BLOCK:
                case CN_AST_FOR:
                case CN_AST_WHILE:
                case CN_AST_IF:
                case CN_AST_SWITCH:
                    break;
                default: 
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, statement, CN_DC_EXPECTED_AST_NODE, "Expected 'defer' attribute on declaration, expression statement, block, for, while, if or switch.");
                    return 0;
            }

            // Remove 'defer' attribute, mostly for clarity.
            cn_ast_remove_attribute(statement, CN_STR_LIT("defer"));

            // Pushing to the stack, and continuing.
            static uint32_t id_counter = 0;
            cn_array_list_append(&deferred_stack, ((Deferred_Entry) { 
                        .id = id_counter++, 
                        .flags = 0, 
                        .statement = statement 
                        })
                    );

            result |= true;
            continue;
        }

        // If statement is not deferred then we either just add it or if it is block we add it and traverse it.
        // Same happens if it is for, while, if, switch, etc... because they all contain blocks.
        // Except we skip those if they don't contain block.
        // This function will also make sure to replace any 
        // return, break or continue with appropriate deferred equivalance if needed.
        result |= defer_explore_statement(&item->declaration_or_statement);

        cn_array_list_append(&items, (Cn_Ast_Node *)item);
    }


    // If we have defers in this block, generate proper labels.
    if (deferred_stack_mark < cn_array_list_length(&deferred_stack)) {
        Deferred_Entry_Flags flags = 0;

        while (deferred_stack_mark < cn_array_list_length(&deferred_stack)) {
            // Pop deferred statement.
            Deferred_Entry deferred = deferred_stack[cn_array_list_length(&deferred_stack) - 1];
            cn_array_list_pop(&deferred_stack);

            // If no flags are set, that means entry was not jumped to form anything. 
            // So we can just emit deferred statement without the label.
            if (deferred.flags == 0) {
                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_block_item(deferred.statement));
                continue;
            }

            // Otherwise we emit label and put deferred statement in there.
            flags |= deferred.flags;

            Cn_Ast_Block_Item *label = cn_build_block_item(
                    cn_build_label(
                        (Cn_Ast_Identifier *) cn_build_format("cn__defer%ld", deferred.id),
                        deferred.statement,
                        ),
                    );

            cn_array_list_append(&items, (Cn_Ast_Node *) label);
        }

        // Making proper exits, adding each depending on the flags.
        if (flags & DEFERRED_ENTRY_JUMPED_FROM_RETURN) {
            if (cn_array_list_length(&deferred_stack) > 0) {
                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                            "if (cn__defer_state == 1) {\n"
                            "    goto cn__defer%ld;\n"
                            "}\n",
                            deferred_stack[cn_array_list_length(&deferred_stack) - 1].id
                            ));
                
            } else {
                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                            "if (cn__defer_state == 1) {\n"
                            "    return cn__defer_rvalue;\n"
                            "}\n"
                            ));
            }
        }
        if (flags & DEFERRED_ENTRY_JUMPED_FROM_BREAK) {
            if (cn_array_list_length(&closest_break_mark_stack) > 0 && 
                    cn_array_list_length(&deferred_stack) > closest_break_mark_stack[cn_array_list_length(&closest_break_mark_stack) - 1]) {
                    cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                                "if (cn__defer_state == 2) {\n"
                                "    goto cn__defer%ld;\n"
                                "}\n",
                                deferred_stack[cn_array_list_length(&deferred_stack) - 1].id
                                ));
            } else {
                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                            "if (cn__defer_state == 2) {\n"
                            "    cn__defer_state = 0;\n"
                            "    break;\n"
                            "}\n"
                            ));
            }
        }
        if (flags & DEFERRED_ENTRY_JUMPED_FROM_CONTINUE) {
            if (cn_array_list_length(&closest_continue_mark_stack) > 0 && 
                    cn_array_list_length(&deferred_stack) > closest_continue_mark_stack[cn_array_list_length(&closest_continue_mark_stack) - 1]) {
                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                            "if (cn__defer_state == 3) {\n"
                            "    goto cn__defer%ld;\n"
                            "}\n",
                            deferred_stack[cn_array_list_length(&deferred_stack) - 1].id
                            ));
            } else {
                cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                            "if (cn__defer_state == 3) {\n"
                            "    cn__defer_state = 0;\n"
                            "    continue;\n"
                            "}\n"
                            ));
            }
        }

        // Finally converting array list to block_items.
        block->block_items = cn_build_list_from((void **)items, cn_array_list_length(&items));

        result |= true;
    }

    cn_array_list_free(&items);

    return result;
}

// Expects statements to not have defer, this function is a helper to properly handler block statements that are attached to while, for, if, switch, etc...
bool defer_explore_statement(Cn_Ast_Node **statement_ptr) {
    Cn_Ast_Node *statement = *statement_ptr;
    if (statement != NULL) {
        switch (statement->kind) {
            case CN_AST_BLOCK:
                return defer_walk_block((Cn_Ast_Block *)statement);

            case CN_AST_FOR:
                cn_array_list_append(&closest_break_mark_stack, cn_array_list_length(&deferred_stack));
                cn_array_list_append(&closest_continue_mark_stack, cn_array_list_length(&deferred_stack));
                return defer_explore_statement(&cn_ast_as(For, statement)->body);

            case CN_AST_WHILE:
                cn_array_list_append(&closest_break_mark_stack, cn_array_list_length(&deferred_stack));
                cn_array_list_append(&closest_continue_mark_stack, cn_array_list_length(&deferred_stack));
                return defer_explore_statement(&cn_ast_as(While, statement)->body);

            case CN_AST_DO_WHILE:
                cn_array_list_append(&closest_break_mark_stack, cn_array_list_length(&deferred_stack));
                cn_array_list_append(&closest_continue_mark_stack, cn_array_list_length(&deferred_stack));
                return defer_explore_statement(&cn_ast_as(Do_While, statement)->body);

            case CN_AST_IF:
                {
                    bool result = false;

                    result |= defer_explore_statement(&cn_ast_as(If, statement)->then_statement);
                    result |= defer_explore_statement(&cn_ast_as(If, statement)->else_statement);

                    return result;
                }

            case CN_AST_SWITCH:
                cn_array_list_append(&closest_break_mark_stack, cn_array_list_length(&deferred_stack));
                return defer_explore_statement(&cn_ast_as(Switch, statement)->body);

            case CN_AST_LABEL:
                return defer_explore_statement(&cn_ast_as(Label, statement)->statement);

            case CN_AST_RETURN:
                if (cn_array_list_length(&deferred_stack) > 0) {
                    *statement_ptr = (Cn_Ast_Node *) defer_make_return_replacement(
                            deferred_stack + cn_array_list_length(&deferred_stack) - 1,
                            cn_ast_as(Return, statement)->expression
                            );

                    return true;
                }

                return false;

            case CN_AST_BREAK:
                if (cn_array_list_length(&closest_break_mark_stack) > 0) {
                    if (cn_array_list_length(&deferred_stack) > closest_break_mark_stack[cn_array_list_length(&closest_break_mark_stack) - 1]) {
                        *statement_ptr = (Cn_Ast_Node *) defer_make_break_replacement(
                                deferred_stack + cn_array_list_length(&deferred_stack) - 1
                                );

                        return true;
                    }
                }

                return false;

            case CN_AST_CONTINUE:
                if (cn_array_list_length(&closest_continue_mark_stack) > 0) {
                    if (cn_array_list_length(&deferred_stack) > closest_continue_mark_stack[cn_array_list_length(&closest_continue_mark_stack) - 1]) {
                        *statement_ptr = (Cn_Ast_Node *) defer_make_continue_replacement(
                                deferred_stack + cn_array_list_length(&deferred_stack) - 1
                                );

                        return true;
                    }
                }

                return false;

            case CN_AST_GOTO:
                if (cn_array_list_length(&deferred_stack) > 0) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_WARNING, statement, CN_DC_EXPECTED_AST_NODE, "Expected other statements but 'goto' in the function scope with defer, use of this 'goto' can lead to undefined behavioar, if it jumps outside of the block with deferred items.");
                }
                break;

            default: 
                break;
        }
    }

    return CN_MESSAGE_RESPONSE_NONE;
}

// The following function simply builds replacement for every return statement defer needs to change, producing something like:
// ```c
//      {
//          cn__defer_rvalue = <return_expression>;
//          cn__defer_state = 1;
//          goto cn__defer<deferred_jump_idx>;
//      }
// ```
Cn_Ast_Node *defer_make_return_replacement(Deferred_Entry *entry, Cn_Ast_Node *return_expression) {
    // Following sequence of builders makes something like this:
    // ```c
    //      cn__defer_rvalue = <return_expression>;
    // ```
    Cn_Ast_Block_Item *rvalue_assignment = cn_build_block_item(
            cn_build_expr_statement(
                cn_build_assign(
                    CN_AST_ASSIGNMENT_OP_ASSIGN,
                    cn_build_identifier(CN_STR_LIT("cn__defer_rvalue")),
                    return_expression,
                    ),
                )
            );

    Cn_Ast_Block_Item *state_assignment = (Cn_Ast_Block_Item *) cn_build_format("cn__defer_state = 1;\n");

    // This line is self explanatory, but it uses `cn_build_format` 
    // that doesn't produce ast nodes directly like other builders,
    // instead it inserts `Cn_Ast_Code` node with specified formatted text, 
    // when this reaches reparser it unwraps the text and parses as usual.
    Cn_Ast_Block_Item *goto_defer = (Cn_Ast_Block_Item *) cn_build_format("goto cn__defer%ld;\n", entry->id);

    // Marking entry as used by used by `return`.
    entry->flags |= DEFERRED_ENTRY_JUMPED_FROM_RETURN;

    // In here we manually build `Cn_Ast_Block` and immediately return it, 
    // composed of two items made above.
    return cn_ast_new((Cn_Ast_Block) {
                .kind = CN_AST_BLOCK,
                .block_items = cn_build_list(rvalue_assignment, state_assignment, goto_defer),
            });
}

Cn_Ast_Node *defer_make_break_replacement(Deferred_Entry *entry) {
    Cn_Ast_Block_Item *state_assignment = (Cn_Ast_Block_Item *) cn_build_format("cn__defer_state = 2;\n");
    Cn_Ast_Block_Item *goto_defer       = (Cn_Ast_Block_Item *) cn_build_format("goto cn__defer%ld;\n", entry->id);

    entry->flags |= DEFERRED_ENTRY_JUMPED_FROM_BREAK;

    return cn_ast_new((Cn_Ast_Block) {
                .kind = CN_AST_BLOCK,
                .block_items = cn_build_list(state_assignment, goto_defer),
            });
}

Cn_Ast_Node *defer_make_continue_replacement(Deferred_Entry *entry) {
    Cn_Ast_Block_Item *state_assignment = (Cn_Ast_Block_Item *) cn_build_format("cn__defer_state = 3;\n");
    Cn_Ast_Block_Item *goto_defer       = (Cn_Ast_Block_Item *) cn_build_format("goto cn__defer%ld;\n", entry->id);

    entry->flags |= DEFERRED_ENTRY_JUMPED_FROM_CONTINUE;

    return cn_ast_new((Cn_Ast_Block) {
                .kind = CN_AST_BLOCK,
                .block_items = cn_build_list(state_assignment, goto_defer),
            });
}


