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
                Cn_Ast_Idx block_idx = cn_ast_get_as_node(msg->node_idx)->function.block_idx;
                
                Cn_Ast_Idx *items = cn_array_list_make(Cn_Ast_Idx, 4);

                Cn_Ast_Idx ident_idx;
                cn_get_declarator_info(
                        cn_ast_get_as_node(msg->node_idx)->function.declarator_idx,
                        &ident_idx
                        );

                CN_ASSERT(cn_ast_get_as_node(ident_idx)->kind == CN_AST_IDENTIFIER);

                cn_array_list_append(&items, cn_build_format("printf(\">>> hello from: '%.*s'\\n\");\n", 
                            CN_UNPACK(cn_ast_get_as_node(ident_idx)->identifier.name))
                        );
                
                for (int64_t i = 0; i < cn_ast_get_as_node(block_idx)->block.block_items.length; i++) {
                    cn_array_list_append(&items, cn_ast_get_as_node(block_idx)->block.block_items.idxs[i]);
                }

                Cn_Ast_List new = {
                    .idxs = items,
                    .length = cn_array_list_length(&items),
                };

                cn_replace_list(&cn_ast_get_as_node(block_idx)->block.block_items, new);

                return CN_RESULT_MODIFIED_NO_REPEAT;
            }
        default: 
            break;
    }

    return CN_RESULT_NONE;
}
