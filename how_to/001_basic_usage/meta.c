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

Cn_Result msg_handler(Cn_Message_Kind kind, void *message) {
    switch (kind) {
        case CN_MESSAGE_PARSED_FUNCTION:
            {   
                Cn_Message_Parsed_Function *msg = message;
                Cn_Ast_Block *block = msg->node->block;

                Cn_Ast_Node **items = cn_array_list_make(Cn_Ast_Node *, 4);

                Cn_Ast_Identifier *ident;
                cn_get_declarator_info(msg->node->declarator, (Cn_Ast_Node **)&ident);

                CN_ASSERT(ident->kind == CN_AST_IDENTIFIER);

                cn_array_list_append(&items, cn_ast_node(cn_build_format("printf(\">>> hello from: '%.*s'\\n\");\n",
                            CN_STR_UNPACK(ident->name)))
                        );

                for (int64_t i = 0; i < block->block_items.length; i++) {
                    cn_array_list_append(&items, block->block_items.ptrs[i]);
                }

                Cn_Ast_List new = {
                    .ptrs = items,
                    .length = cn_array_list_length(&items),
                };

                block->block_items = new;

                return CN_RESULT_MODIFIED_NO_REPEAT;
            }
        default: 
            break;
    }

    return CN_RESULT_NONE;
}
