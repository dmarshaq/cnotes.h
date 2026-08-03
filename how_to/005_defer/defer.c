#include "cnotes.h"
#include <stdint.h>

// Defer takes in any function definitions, once were modified too, so to keep track which function definitions were deferred.
// We can use helper data structure that simply stores names of the functions that were deferred.
Cn_String *deferred_function_names = NULL;

// Here we just forward declare helper that will be used in the moment.
// It for recurisve walk.
Cn_Message_Response defer_walk_block(Cn_Ast_Block *block, bool block_from_loop_or_switch);

// Deferred stack is array list that will store every deferred statement throughout the deferred function.
Cn_Ast_Node **deferred_stack;

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

    deferred_stack = cn_array_list_make(Cn_Ast_Node *, 4);
    Cn_Message_Response response = defer_walk_block(function->block, false);

    // Prepending `cn__defer_rvalue` and `cn__defer_state` if we know that at least one modification occured.
    if (response != CN_MESSAGE_RESPONSE_NONE) {
        Cn_Ast_Node **items = cn_array_list_make(Cn_Ast_Node *, 16);
        
        cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_block_item(
                cn_build_declaration(
                    cn_ast_copy(function->declaration_specifiers),  // TODO: Fix: Causes error if function specifiers like `inline` are there too.
                    cn_build_list(
                        cn_build_init_declarator(
                            cn_build_declarator(
                                NULL,
                                cn_build_identifier(CN_STR_LIT("cn__defer_rvalue")),
                                ),
                            NULL,
                            )
                        ),
                    ),
                )
            );
        
        cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format("int cn__defer_state = 0;\n"));

        for (int64_t i = 0; i < function->block->block_items.length; i++) {
            cn_array_list_append(&items, function->block->block_items.ptrs[i]);
        }

        function->block->block_items = cn_build_list_from((void **) items, cn_array_list_length(&items));

        cn_array_list_free(&items);
    }

    cn_array_list_free(&deferred_stack);
    
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
Cn_Ast_Block *defer_make_return_replacement(Cn_Ast_Node *return_expression, int64_t deferred_jump_idx) {
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
    Cn_Ast_Block_Item *goto_defer = (Cn_Ast_Block_Item *) cn_build_format("goto cn__defer%ld;\n", deferred_jump_idx);

    // In here we manually build `Cn_Ast_Block` and immediately return it, 
    // composed of two items made above.
    return cn_ast_new((Cn_Ast_Block) {
                .kind = CN_AST_BLOCK,
                .block_items = cn_build_list(rvalue_assignment, state_assignment, goto_defer),
            });
}

Cn_Ast_Block *defer_make_break_replacement(int64_t deferred_jump_idx) {
    Cn_Ast_Block_Item *state_assignment = (Cn_Ast_Block_Item *) cn_build_format("cn__defer_state = 2;\n");
    Cn_Ast_Block_Item *goto_defer       = (Cn_Ast_Block_Item *) cn_build_format("goto cn__defer%ld;\n", deferred_jump_idx);

    return cn_ast_new((Cn_Ast_Block) {
                .kind = CN_AST_BLOCK,
                .block_items = cn_build_list(state_assignment, goto_defer),
            });
}

Cn_Ast_Goto *defer_make_continue_replacement(int64_t deferred_jump_idx) {
    return (Cn_Ast_Goto *) cn_build_format("goto cn__defer%ld;\n", deferred_jump_idx);
}

// Expects statements to not have defer, this function is a helper to properly handler block statements that are attached to while, for, if, switch, etc...
Cn_Message_Response defer_explore_statement(Cn_Ast_Node **statement_ptr, bool block_contains_deferred, bool block_from_loop_or_switch) {
    Cn_Ast_Node *statement = *statement_ptr;
    if (statement != NULL) {
        switch (statement->kind) {
            case CN_AST_BLOCK:
                return defer_walk_block((Cn_Ast_Block *)statement, block_from_loop_or_switch);

            case CN_AST_FOR:
                return defer_explore_statement(&cn_ast_as(For, statement)->body, block_contains_deferred, true);

            case CN_AST_WHILE:
                return defer_explore_statement(&cn_ast_as(While, statement)->body, block_contains_deferred, true);

            case CN_AST_IF:
                {
                    Cn_Message_Response result;

                    result = defer_explore_statement(&cn_ast_as(If, statement)->then_statement, block_contains_deferred, false);
                    result = defer_explore_statement(&cn_ast_as(If, statement)->else_statement, block_contains_deferred, false);

                    return result;
                }

            case CN_AST_SWITCH:
                return defer_explore_statement(&cn_ast_as(Switch, statement)->body, block_contains_deferred, true);

            case CN_AST_LABEL:
                return defer_explore_statement(&cn_ast_as(Label, statement)->statement, block_contains_deferred, false);
                break;

            case CN_AST_RETURN:
                if (cn_array_list_length(&deferred_stack) > 0) {
                    *statement_ptr = (Cn_Ast_Node *) defer_make_return_replacement(
                            cn_ast_as(Return, statement)->expression, 
                            cn_array_list_length(&deferred_stack) - 1
                            );

                    return CN_MESSAGE_RESPONSE_MODIFIED;
                }

                return CN_MESSAGE_RESPONSE_NONE;

            case CN_AST_BREAK:
                // If `block_contains_deferred` is true it is implied `cn_array_list_length(&deferred_stack) > 0` is true.
                if (block_contains_deferred) {
                    *statement_ptr = (Cn_Ast_Node *) defer_make_break_replacement(
                            cn_array_list_length(&deferred_stack) - 1
                            );

                    return CN_MESSAGE_RESPONSE_MODIFIED;
                }

                return CN_MESSAGE_RESPONSE_NONE;

            case CN_AST_CONTINUE:
                if (block_contains_deferred) {
                    *statement_ptr = (Cn_Ast_Node *) defer_make_continue_replacement(
                            cn_array_list_length(&deferred_stack) - 1
                            );

                    return CN_MESSAGE_RESPONSE_MODIFIED;
                }

                return CN_MESSAGE_RESPONSE_NONE;

            case CN_AST_GOTO:
                if (block_contains_deferred) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_WARNING, statement, CN_DC_EXPECTED_AST_NODE, "Expected other nodes but 'goto' in the block with deferred statements, use of this 'goto' can lead to undefined behavior.");
                }
                break;

            default: 
                break;
        }
    }

    return CN_MESSAGE_RESPONSE_NONE;
}

Cn_Message_Response defer_walk_block(Cn_Ast_Block *block, bool block_from_loop_or_switch) {
    Cn_Message_Response result = CN_MESSAGE_RESPONSE_NONE;

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
            cn_array_list_append(&deferred_stack, statement);

            result = CN_MESSAGE_RESPONSE_MODIFIED;
            continue;
        }

        // If statement is not deferred then we either just add it or if it is block we add it and traverse it.
        // Same happens if it is for, while, if, switch, etc... because they all contain blocks.
        // Except we skip those if they don't contain block.
        // This function will also make sure to replace any 
        // return, break or continue with appropriate deferred equivalance if needed.
        result = defer_explore_statement(&item->declaration_or_statement, deferred_stack_mark < cn_array_list_length(&deferred_stack), false);

        cn_array_list_append(&items, (Cn_Ast_Node *)item);
    }


    // If we have defers in this block, generate proper labels.
    if (deferred_stack_mark < cn_array_list_length(&deferred_stack)) {
        while (deferred_stack_mark < cn_array_list_length(&deferred_stack)) {
            // Pop deferred statement.
            Cn_Ast_Node *deferred = deferred_stack[cn_array_list_length(&deferred_stack) - 1];
            cn_array_list_pop(&deferred_stack);

            Cn_Ast_Block_Item *label = cn_build_block_item(
                    cn_build_label(
                        (Cn_Ast_Identifier *) cn_build_format("cn__defer%ld", cn_array_list_length(&deferred_stack)),
                        deferred,
                        ),
                    );

            cn_array_list_append(&items, (Cn_Ast_Node *) label);
        }

        // Making proper exits depending on the `cn__defer_state`.

        if (block_from_loop_or_switch) {
            cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                        "if (cn__defer_state == 1) {\n"
                        "    return cn__defer_rvalue;\n"
                        "} else if (cn__defer_state == 2) {\n"
                        "    cn__defer_state = 0;\n"
                        "    break;\n"
                        "}\n"
                        ));
        } else {
            cn_array_list_append(&items, (Cn_Ast_Node *) cn_build_format(
                        "return cn__defer_rvalue;\n"
                        ));
        }

        // Finally converting array list to block_items.
        block->block_items = cn_build_list_from((void **)items, cn_array_list_length(&items));
    }

    cn_array_list_free(&items);

    return result;
}

