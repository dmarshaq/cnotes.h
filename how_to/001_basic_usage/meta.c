#define CN_IMPLEMENTATION
#include "cnotes.h"

int msg_handler(Cn_Message_Kind kind, void *message);

int main(int argc, char **argv) {

    while (true) {
        argc--;
        argv++;
        if (argc == 0) break;

        // Allocate AST data storage.
        Cn_Ast_Data ast_data = {0};
        cn_ast_init(&ast_data);

        // Processing single translation unit.
        Cn_Translation_Unit tu = cn_tu_make(*argv);

        // Setting message handler.
        cn_message_handler = msg_handler;

        if (cn_tu_process(&tu, 0) != 0) {
            fprintf(stderr, "Processing failed with %ld error(s)\n", ast_data.error_count);
            cn_tu_free(&tu);
            return 1;
        }

        cn_tu_free(&tu);
    }

    return 0;
}

const Cn_String cn__defer_rvalue = CN_STR_BUFFER("cn__defer_rvalue");

int msg_handler(Cn_Message_Kind kind, void *message) {
    switch (kind) {
        case CN_MESSAGE_PARSED_FUNCTION:
            {   
                int modified = 0;

                Cn_Ast_Idx *deffered_stack = cn_array_list_make(Cn_Ast_Idx, 4);

                Cn_Message_Parsed_Function *m = message;

                Cn_Ast_Idx block_idx = cn_ast_get_as_node(m->node_idx)->function.block_idx;
                
                // Using libraries idx stack to construct new block items list.
                int64_t mark = cn_ast_idx_stack_mark();

                for (int64_t i = 0; i < cn_ast_get_as_node(block_idx)->block.block_items.length; i++) {
                    Cn_Ast_Idx item_idx = cn_ast_get_as_node(block_idx)->block.block_items.idxs[i];

                    Cn_Ast_Idx attribute_idx = cn_get_attribute(cn_ast_get_as_node(item_idx)->block_item.declaration_or_statement_idx, CN_STR_LIT("defer"));
                    if (attribute_idx != CN_AST_NIL_IDX) {
                        
                        switch (cn_ast_get_as_node(cn_ast_get_as_node(item_idx)->block_item.declaration_or_statement_idx)->kind) {
                            case CN_AST_DECLARATION:
                            case CN_AST_EXPRESSION_STATEMENT:
                            case CN_AST_BLOCK:
                                break;
                            default: 
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, cn_ast_get_as_node(item_idx)->block_item.declaration_or_statement_idx, CN_DC_EXPECTED_AST_NODE, "Expected 'defer' attribute on declaration, expression statement or block.");
                                return 0;
                        }

                        // Remove `defer` attribute.
                        cn_remove_attribute(attribute_idx);

                        // Pushing to the stack idx, and continuing.
                        cn_array_list_append(&deffered_stack, cn_ast_get_as_node(item_idx)->block_item.declaration_or_statement_idx);

                        modified = 1;
                    } 
                    else if (cn_ast_get_as_node(cn_ast_get_as_node(item_idx)->block_item.declaration_or_statement_idx)->kind == CN_AST_RETURN && cn_array_list_length(&deffered_stack) > 0) {
                        // Replacing return with rvalue set and goto statement.
                        Cn_Ast_Idx rvalue_set = cn_build_block_item(cn_build_expr_statement(
                                cn_build_assign(CN_ASSIGNMENT_OP_ASSIGN, 
                                    cn_build_identifier(cn__defer_rvalue),
                                    cn_ast_get_as_node(cn_ast_get_as_node(item_idx)->block_item.declaration_or_statement_idx)->return_statement.expression_idx,
                                    ),
                                ));

                        Cn_String str = CN_STR_BUFFER_EMPTY(128);
                        str = cn_str_format(str, "cn__defer%ld", cn_array_list_length(&deffered_stack) - 1);

                        Cn_Ast_Idx goto_stmt = cn_build_block_item(cn_build_goto_statement(cn_build_identifier(str, .alloc = true)));

                        cn_ast_idx_stack_push(rvalue_set);
                        cn_ast_idx_stack_push(goto_stmt);
                    } 
                    else {
                        cn_ast_idx_stack_push(cn_ast_get_as_node(block_idx)->block.block_items.idxs[i]);
                    }
                }

                if (modified == 1) {
                    Cn_Ast_Idx rvalue_decl = cn_build_block_item(
                                cn_build_declaration(
                                    cn_copy(cn_ast_get_as_node(m->node_idx)->function.declaration_specifiers_idx),
                                    cn_build_list(
                                        cn_build_init_declarator(
                                            cn_build_declarator(
                                                CN_AST_NIL_IDX,
                                                cn_build_identifier(cn__defer_rvalue),
                                                ),
                                            CN_AST_NIL_IDX,
                                            )
                                        ),
                                ),
                            );

                    cn_ast_idx_stack_prepend(rvalue_decl, mark);

                    while (cn_array_list_length(&deffered_stack) > 0) {
                        Cn_Ast_Idx idx = deffered_stack[cn_array_list_length(&deffered_stack) - 1];
                        cn_array_list_pop(&deffered_stack);

                        Cn_String str = CN_STR_BUFFER_EMPTY(128);
                        str = cn_str_format(str, "cn__defer%ld", cn_array_list_length(&deffered_stack));
                        Cn_Ast_Idx label = cn_build_block_item(
                                    cn_build_label(
                                            cn_build_identifier(str, .alloc = true),
                                            idx,
                                        ),
                                );

                        cn_ast_idx_stack_push(label);
                    }

                    Cn_Ast_Idx return_stmt = cn_build_block_item(
                                    cn_build_return_statement(
                                            cn_build_identifier(cn__defer_rvalue),
                                        ),
                                );

                    cn_ast_idx_stack_push(return_stmt);

                    cn_replace_list(&cn_ast_get_as_node(block_idx)->block.block_items, cn_ast_idx_stack_finalize(mark));
                } else {
                    cn_ast_idx_stack_discard(mark);
                }

                cn_array_list_free(&deffered_stack);

                return modified;
            }
        default: 
            return 0;
    }

    return 0;
}
