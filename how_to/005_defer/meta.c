#define CN_IMPLEMENTATION
#include "cnotes.h"

Cn_Result msg_handler(Cn_Message_Kind kind, void *message);

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

Cn_Result msg_handler(Cn_Message_Kind kind, void *message) {
    switch (kind) {
        case CN_MESSAGE_PARSED_FUNCTION:
            {   
                int modified = CN_RESULT_NONE;

                Cn_Ast_Node **deffered_stack = cn_array_list_make(Cn_Ast_Node *, 4);

                Cn_Message_Parsed_Function *m = message;

                Cn_Ast_Block *block = m->node->block;

                // Using libraries ptr stack to construct new block items list.
                int64_t mark = cn_ast_stack_mark();

                for (int64_t i = 0; i < block->block_items.length; i++) {
                    Cn_Ast_Block_Item *item = cn_ast_as(Block_Item, block->block_items.ptrs[i]);
                    Cn_Ast_Node *statement = item->declaration_or_statement;

                    if (cn_get_attribute(statement, CN_STR_LIT("defer")) != NULL) {

                        switch (statement->kind) {
                            case CN_AST_DECLARATION:
                            case CN_AST_EXPRESSION_STATEMENT:
                            case CN_AST_BLOCK:
                                break;
                            default: 
                                cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, statement, CN_DC_EXPECTED_AST_NODE, "Expected 'defer' attribute on declaration, expression statement or block.");
                                return 0;
                        }

                        // Remove 'defer' attribute, so the deferred statement is not
                        // picked up again when the modified function is re-sent.
                        cn_remove_attribute(statement, CN_STR_LIT("defer"));

                        // Pushing to the stack, and continuing.
                        cn_array_list_append(&deffered_stack, statement);

                        modified = CN_RESULT_MODIFIED;
                    } 
                    else if (statement->kind == CN_AST_RETURN && cn_array_list_length(&deffered_stack) > 0) {
                        // Replacing return with rvalue set and goto statement.
                        Cn_Ast_Block_Item *rvalue_set = cn_build_block_item(cn_build_expr_statement(
                                cn_build_assign(CN_ASSIGNMENT_OP_ASSIGN, 
                                    cn_build_identifier(cn__defer_rvalue),
                                    cn_ast_as(Return, statement)->expression,
                                    ),
                                ));

                        Cn_String str = CN_STR_BUFFER_EMPTY(128);
                        str = cn_str_format(str, "cn__defer%ld", cn_array_list_length(&deffered_stack) - 1);

                        Cn_Ast_Block_Item *goto_stmt = cn_build_block_item(cn_build_goto_statement(cn_build_identifier(str, .alloc = true)));

                        cn_ast_stack_push(rvalue_set);
                        cn_ast_stack_push(goto_stmt);
                    } 
                    else {
                        cn_ast_stack_push(block->block_items.ptrs[i]);
                    }
                }

                if (modified == CN_RESULT_MODIFIED) {
                    Cn_Ast_Block_Item *rvalue_decl = cn_build_block_item(
                                cn_build_declaration(
                                    cn_copy(m->node->declaration_specifiers),
                                    cn_build_list(
                                        cn_build_init_declarator(
                                            cn_build_declarator(
                                                NULL,
                                                cn_build_identifier(cn__defer_rvalue),
                                                ),
                                            NULL,
                                            )
                                        ),
                                ),
                            );

                    cn_ast_stack_prepend(rvalue_decl, mark);

                    while (cn_array_list_length(&deffered_stack) > 0) {
                        Cn_Ast_Node *deferred = deffered_stack[cn_array_list_length(&deffered_stack) - 1];
                        cn_array_list_pop(&deffered_stack);

                        Cn_String str = CN_STR_BUFFER_EMPTY(128);
                        str = cn_str_format(str, "cn__defer%ld", cn_array_list_length(&deffered_stack));
                        Cn_Ast_Block_Item *label = cn_build_block_item(
                                    cn_build_label(
                                            cn_build_identifier(str, .alloc = true),
                                            deferred,
                                        ),
                                );

                        cn_ast_stack_push(label);
                    }

                    Cn_Ast_Block_Item *return_stmt = cn_build_block_item(
                                    cn_build_return_statement(
                                            cn_build_identifier(cn__defer_rvalue),
                                        ),
                                );

                    cn_ast_stack_push(return_stmt);

                    block->block_items = cn_ast_stack_finalize(mark);
                } else {
                    cn_ast_stack_discard(mark);
                }

                cn_array_list_free(&deffered_stack);

                return modified;
            }
        default: 
            break;
    }

    return CN_RESULT_NONE;
}
