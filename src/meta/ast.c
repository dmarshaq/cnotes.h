#include "meta/ast.h"

#include "core/str.h"
#include "core/arena.h"
#include "core/structs.h"
#include "core/file.h"
#include "core/typeinfo.h"


#include "meta/lexer.h"
#include "meta/meta.h"

#include <stdio.h>



/**
 * This arena is just used to save any string data, because source string of the file is freed when current file is closed and next one is opened for processing.
 */
static Arena arena_strings; // @Leak.

String *string_list; // @Leak.

String string_list_save(String str) {
    for (u32 i = 0; i < array_list_length(&string_list); i++) {
        if (str_equals(str, string_list[i])) {
            return string_list[i];
        }
    }
    
    array_list_append(&string_list, ((String) {
        .data = str_copy_to(str, arena_alloc(&arena_strings, str.length)),
        .length = str.length
    }));
    return string_list[array_list_length(&string_list) - 1];
}

String string_list_force_save(String str) {
    array_list_append(&string_list, ((String) {
        .data = str_copy_to(str, arena_alloc(&arena_strings, str.length)),
        .length = str.length
    }));
    return string_list[array_list_length(&string_list) - 1];
}




int try_parse_storage_specifier(Token symbol, Storage_Specifier_Flags *output) {
    for (int i = 0; i < STORAGE_SPECIFIER_KEYWORDS_LENGTH; i++) {
        if (str_equals(symbol.str, STORAGE_SPECIFIER_KEYWORDS[i])) {
            *output |= (1 << i);
            return 0;
        }
    }

    return -1;
}

int try_parse_qualifier(Token symbol, Qualifier_Flags *output) {
    for (int i = 0; i < QUALIFIER_KEYWORDS_LENGTH; i++) {
        if (str_equals(symbol.str, QUALIFIER_KEYWORDS[i])) {
            *output |= (1 << i);
            return 0;
        }
    }

    return -1;
}

int try_parse_type_specifier(Lexer *lexer, Type_Specifier *output) {
    Lexer original_state = *lexer;

    if (!lexer_expect(lexer, TOKEN_SYMBOL)) {
        goto backtrack;
    }

    for (int i = 0; i < MODIFIER_KEYWORDS_LENGTH; i++) {
        if (str_equals(lexer->token.str, MODIFIER_KEYWORDS[i])) {
            u8 flag = (1 << i);
            switch(flag) {
                case TYPE_MODIFIER_SIGNED:
                    if (output->flags & TYPE_MODIFIER_UNSIGNED) {
                        META_LOG_ERROR("Cannot have signed and unsigned qualifiers be set together.");
                        goto backtrack;
                    }
                    output->flags |= TYPE_MODIFIER_SIGNED;
                    break;
                case TYPE_MODIFIER_UNSIGNED:
                    if (output->flags & TYPE_MODIFIER_SIGNED) {
                        META_LOG_ERROR("Cannot have signed and unsigned qualifiers be set together.");
                        goto backtrack;
                    }
                    output->flags |= TYPE_MODIFIER_UNSIGNED;
                    break;
                case TYPE_MODIFIER_SHORT:
                    if (output->flags & (TYPE_MODIFIER_LONG | TYPE_MODIFIER_LONG_LONG)) {
                        META_LOG_ERROR("Cannot have short and long modifiers be set together.");
                        goto backtrack;
                    }
                    output->flags |= TYPE_MODIFIER_SHORT;
                    break;
                case TYPE_MODIFIER_LONG:
                    if (output->flags & TYPE_MODIFIER_LONG_LONG) {
                        META_LOG_ERROR("Cannot have long long and long modifiers be set together.");
                        goto backtrack;
                    }

                    if (output->flags & TYPE_MODIFIER_SHORT) {
                        META_LOG_ERROR("Cannot have short and long modifiers be set together.");
                        goto backtrack;
                    }

                    if (output->flags & TYPE_MODIFIER_LONG) {
                        output->flags |= TYPE_MODIFIER_LONG_LONG;
                        output->flags &= ~TYPE_MODIFIER_LONG;
                    } else {
                        output->flags |= TYPE_MODIFIER_LONG;
                    }
                    break;
                default:
                    output->flags |= flag;
                    break;
            }

            lexer_next_token(lexer);
            return 0;
        }

    }

    // @Incomplete: Replace this with separate logic for different primitive types. Because C syntax sometimes sucks.
    // For right now it is only int.
    if (str_equals(lexer->token.str, INT_STR)) {
        output->kind = TYPE_IDENTIFIER_INT;
        output->name = INT_STR;
        lexer_next_token(lexer);
        return 0;
    }

    if (str_equals(lexer->token.str, CHAR_STR)) {
        output->kind = TYPE_IDENTIFIER_CHAR;
        output->name = CHAR_STR;
        lexer_next_token(lexer);
        return 0;
    }

    if (str_equals(lexer->token.str, FLOAT_STR)) {
        output->kind = TYPE_IDENTIFIER_FLOAT;
        output->name = FLOAT_STR;
        lexer_next_token(lexer);
        return 0;
    }

    if (str_equals(lexer->token.str, DOUBLE_STR)) {
        output->kind = TYPE_IDENTIFIER_DOUBLE;
        output->name = DOUBLE_STR;
        lexer_next_token(lexer);
        return 0;
    }

    if (str_equals(lexer->token.str, BOOL_STR)) {
        output->kind = TYPE_IDENTIFIER_BOOL;
        output->name = BOOL_STR;
        lexer_next_token(lexer);
        return 0;
    }

    if (str_equals(lexer->token.str, VOID_STR)) {
        output->kind = TYPE_IDENTIFIER_VOID;
        output->name = VOID_STR;
        lexer_next_token(lexer);
        return 0;
    }

    if (str_equals(lexer->token.str, STRUCT_STR)) {
        if (output->flags != 0) {
            META_LOG_ERROR("Modifier flags are not allowed in struct type specifier.");
            goto backtrack;
        }

        output->kind = TYPE_IDENTIFIER_STRUCT;
        lexer_next_token(lexer);

        // Optional: expecting tag.
        if (lexer_expect(lexer, TOKEN_SYMBOL)) {
            output->name = string_list_save(lexer->token.str);
            lexer_next_token(lexer);
        }

        // Expecting definition.
        if (lexer_expect(lexer, TOKEN_CURLY_OPEN)) {
            Ast_Node struct_definition = ast_parse_struct_definition(lexer);


            array_list_append(&ast_node_list, struct_definition);
            output->definition_index = array_list_length(&ast_node_list) - 1;
            
            // If there is tag, tie this tag to the struct definition index.
            if (!str_empty(output->name)) {

                // But if there is already non null index, error.
                u32 *index_ptr = hash_table_get(&tag_definition_table, UNPACK(output->name));
                if (index_ptr != NULL && *index_ptr != NULL_AST_NODE_INDEX) {
                    META_LOG_ERROR("Redefinition of struct '%.*s'.", UNPACK(output->name));
                    goto backtrack;
                }

                hash_table_put(&tag_definition_table, output->definition_index, UNPACK(output->name));
            }

            return 0;
        }

        // If not expecting to have at least a tag name.
        if (!str_empty(output->name)) {

            // Only forward declaring tag if it is not in the table yet, and since here there was no definition, just putting NULL_AST_NODE_INDEX.
            if (hash_table_get(&tag_definition_table, UNPACK(output->name)) == NULL) {
                hash_table_put(&tag_definition_table, NULL_AST_NODE_INDEX, UNPACK(output->name));
            }

            return 0;
        }

        META_LOG_ERROR("Couldn't parse struct type specifier.");
        goto backtrack;
    }

    if (str_equals(lexer->token.str, ENUM_STR)) {
        TODO("Enum type identifier.");

        return 0;
    }

    if (str_equals(lexer->token.str, UNION_STR)) {
        TODO("Union type identifier.");

        return 0;
    }


    if (!str_empty(output->name) ) {
        if (hash_table_get(&typedef_definition_table, UNPACK(lexer->token.str)) != NULL) {
            META_LOG_ERROR("More than one typename specified in type specifier.");
        }
        // Already have a typename and following symbol is not known type.
        goto backtrack;
    }

    // If specified token that is assumed to be a type is not a valid typename, error.
    if (hash_table_get(&typedef_definition_table, UNPACK(lexer->token.str)) == NULL) {
        META_LOG_ERROR("Unknown type: '%.*s'.", UNPACK(lexer->token.str));
        goto backtrack;
    }

    output->kind = TYPE_IDENTIFIER_TYPEDEF;
    output->name = string_list_save(lexer->token.str);
    lexer_next_token(lexer);
    return 0;
    

backtrack:
    *lexer = original_state;
    return -1;
}



/**
 * Following arena simply stores member's nodes till all members are parsed.
 * And then we simply take these nodes from arena and move them into an ast tree.
 */
static Arena arena_ast_node_struct_member_declaration; // @Leak.

/**
 * Following arena simply stores param nodes till all parameters are parsed.
 * And then we simply take these nodes from arena and move them into an ast tree.
 */
static Arena arena_ast_node_function_param_declaration; // @Leak.

/**
 * Following arena simply stores declarators that a multiply defined for a signle declarator. Yeah this wording is bad.
 * For example: 
 *
 *          int a, *b, c[30];
 *
 */
static Arena arena_ast_node_declarators; // @Leak.

/**
 * Following arena allows to parse multiple notes together as an array of ast nodes.
 * For example:
 *      ... @Abc @Cde @Fgh
 */
static Arena arena_ast_node_note; // @Leak.
                                         

Ast_Node *ast_node_list; // @Leak.

u32 *ast_root_nodes_index_list; // @Leak. 

u32 *tag_definition_table; // @Leak.

u32 *typedef_definition_table; // @Leak.


int ast_init() {
    arena_strings                                       = arena_make(2048);
    string_list                                         = array_list_make(String, 32, &std_allocator);


    arena_ast_node_declarators                          = arena_make(2048);
    arena_ast_node_struct_member_declaration            = arena_make(2048);
    arena_ast_node_function_param_declaration           = arena_make(2048);
    arena_ast_node_note                                 = arena_make(2048);

    ast_node_list                                       = array_list_make(Ast_Node, 64, &std_allocator);
    ast_root_nodes_index_list                           = array_list_make(u32, 16, &std_allocator);

    tag_definition_table                                = hash_table_make(u32, 16, &std_allocator);
    typedef_definition_table                            = hash_table_make(u32, 16, &std_allocator);

    return 0;
}


Ast_Node ast_parse_constant_expression(Lexer *lexer) {
    Lexer original_state = *lexer;

    if (lexer_expect(lexer, TOKEN_INTEGER)) {
        Ast_Node node = {
            .kind = AST_NODE_INTEGER,
            .integer.value = str_parse_int(lexer->token.str),
        };

        lexer_next_token(lexer);
        return node;
    }

    if (lexer_expect(lexer, TOKEN_FLOAT)) {
        Ast_Node node = {
            .kind = AST_NODE_FLOAT,
            .floating_point.value = str_parse_float(lexer->token.str),
        };

        lexer_next_token(lexer);
        return node;
    }


    META_LOG_ERROR("Couldn't parse constant expression, only supports integers or floats.");

backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_note(Lexer *lexer) {
    Lexer original_state = *lexer;

    if (!lexer_expect(lexer, TOKEN_METANOTE)) {
        META_LOG_ERROR("Expected @note.");
        goto backtrack;
    }

    Ast_Node node = {
        .kind = AST_NODE_NOTE,
    };

    node.note.name = string_list_save(lexer->token.str);

    lexer_next_token(lexer);

    return node;
    
backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_function_param_declaration(Lexer *lexer) {
    Lexer original_state = *lexer;

    Ast_Node node = {
        .kind = AST_NODE_FUNCTION_PARAM_DECLARATION,
        .function_param_declaration.type_specifier.definition_index = NULL_AST_NODE_INDEX,
    };


    // Getting declaration specifiers.
    while (true) {
        if (try_parse_qualifier(lexer->token, &node.struct_member_declaration.qualifier_flags) == 0) {
            lexer_next_token(lexer);
            continue;
        }

        // @Important: Type specifier comes always last, because it is the only parsing that can accept arbirtary unknown symbol as type name.
        if (try_parse_type_specifier(lexer, &node.struct_member_declaration.type_specifier) == 0) {
            continue;
        }

        break;
    }

    // Just in case, checking if user tried to insert illegal definition into function param type specifier.
    if (node.function_param_declaration.type_specifier.definition_index != NULL_AST_NODE_INDEX) {
        META_LOG_ERROR("Type specifier definitions are not allowed in function param declaration.");
        goto backtrack;
    }


    // We can move forward if we have at least one type_specifier present.
    // To check that we need to have either an actual typename or a modifier flag present like (unsigned or long).
    // For example if there is no typename like int, double, float, but there is unsigned, short or long then we can proceed.
    if (str_empty(node.function_param_declaration.type_specifier.name) && node.function_param_declaration.type_specifier.flags == 0) {
        META_LOG_ERROR("No type specifier was found in function param declaration.");
        goto backtrack;
    }


    Ast_Node declarator = ast_parse_declarator(lexer);
    if (declarator.kind == AST_NODE_UNKNOWN) {
        return node;
    }

    array_list_append(&ast_node_list, declarator);
    node.function_param_declaration.declarator_index = array_list_length(&ast_node_list) - 1;

    return node;
    
backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}


Ast_Node ast_parse_struct_member_declaration(Lexer *lexer) {
    Lexer original_state = *lexer;

    Ast_Node node = {
        .kind = AST_NODE_STRUCT_MEMBER_DECLARATION,
        .struct_member_declaration.type_specifier.definition_index = NULL_AST_NODE_INDEX,
    };


    // Getting declaration specifiers.
    while (true) {
        if (try_parse_qualifier(lexer->token, &node.struct_member_declaration.qualifier_flags) == 0) {
            lexer_next_token(lexer);
            continue;
        }

        // @Important: Type specifier comes always last, because it is the only parsing that can accept arbirtary unknown symbol as type name.
        if (try_parse_type_specifier(lexer, &node.struct_member_declaration.type_specifier) == 0) {
            continue;
        }

        break;
    }


    // We can move forward if we have at least one type_specifier present.
    // To check that we need to have either an actual typename or a modifier flag present like (unsigned or long).
    // For example if there is no typename like int, double, float, but there is unsigned, short or long then we can proceed. @TODO: Make exception for anonymous structs as well, or add a convention for silently naming them.
    if (str_empty(node.struct_member_declaration.type_specifier.name) && node.struct_member_declaration.type_specifier.flags == 0) {
        META_LOG_ERROR("No type specifier was found in struct member declaration.");
        goto backtrack;
    }


    // @Important: Not neccessary anymore, since ast_parse_declarator will return signle abstract declarator without a name if there are no valid symbol that can be used for declarators.
    // // Edge case: Early return if no declarators are present. Usually what happens most of the time in structs.
    // if (lexer_expect(lexer, TOKEN_SEMICOLON)) {
    //     lexer_next_token(lexer);
    //     return node;
    // }


    // Parsing declarators, comma separated.
    Arena arena_state;
    arena_state = arena_ast_node_declarators;
    while (true) {
        
        Ast_Node declarator = ast_parse_declarator(lexer);
        if (declarator.kind == AST_NODE_UNKNOWN) {
            // META_LOG_ERROR("Couldn't parse declarator.");
            printf("Backtrack?\n");
            goto backtrack;
        }

        node.struct_member_declaration.declarators_count++;
        Ast_Node *next = arena_alloc(&arena_ast_node_declarators, sizeof(Ast_Node));
    
        *next = declarator;

        if (lexer->token.type == TOKEN_COMMA) {
            lexer_next_token(lexer);
            continue;
        }

        break;
    }


    // Copying every parsed declarator to the actual ast node list.
    Ast_Node *parsed_declarators = arena_state.ptr;
    for (s32 i = 0; i < node.struct_member_declaration.declarators_count; i++) {
        array_list_append(&ast_node_list, parsed_declarators[i]);
    }

    node.struct_member_declaration.declarators_index = array_list_length(&ast_node_list) - node.struct_member_declaration.declarators_count;

    // @Important: Clearing declarator arena after.
    arena_ast_node_declarators = arena_state;



    // Expecting optional notes.
    arena_state = arena_ast_node_note;
    while (lexer_expect(lexer, TOKEN_METANOTE)) {
        Ast_Node note = ast_parse_note(lexer);

        if (note.kind == AST_NODE_UNKNOWN) {
            printf("Backtrack? x2\n");
            goto backtrack;
        }

        node.struct_member_declaration.notes_count++;
        Ast_Node *next = arena_alloc(&arena_ast_node_note, sizeof(Ast_Node));
        
        *next = note;
    }

    // Copying every parsed note to the actual ast node list.
    Ast_Node *parsed_notes = arena_state.ptr;
    for (s32 i = 0; i < node.struct_member_declaration.notes_count; i++) {
        array_list_append(&ast_node_list, parsed_notes[i]);
    }

    node.struct_member_declaration.notes_index = array_list_length(&ast_node_list) - node.struct_member_declaration.notes_count;

    // @Important: Clearing note arena after.
    arena_ast_node_note = arena_state;


    if (!lexer_expect(lexer, TOKEN_SEMICOLON)) {
        META_LOG_ERROR("Expected ';' at the end of struct member declaration.");
        goto backtrack;
    }

    lexer_next_token(lexer);

    return node;
    
backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_struct_definition(Lexer *lexer) {
    Lexer original_state = *lexer;
    
    if (!lexer_expect(lexer, TOKEN_CURLY_OPEN)) {
        goto backtrack;
    }

    lexer_next_token(lexer);

    Ast_Node node = {
        .kind = AST_NODE_STRUCT_DEFINITION,
    };

    // Parsing struct member declarations, semicolon separated.
    // Saving arena state.
    Arena arena_state;
    arena_state = arena_ast_node_struct_member_declaration;
    while (true) {
        
        Ast_Node struct_member_declaration = ast_parse_struct_member_declaration(lexer);

        if (struct_member_declaration.kind == AST_NODE_UNKNOWN) {
            META_LOG_ERROR("Couldn't parse struct member declaration.");
            goto backtrack;
        }

        node.struct_definition.members_count++;
        Ast_Node *next = arena_alloc(&arena_ast_node_struct_member_declaration, sizeof(Ast_Node));
    
        *next = struct_member_declaration;

        if (lexer_expect(lexer, TOKEN_CURLY_CLOSE)) {
            lexer_next_token(lexer);
            break;
        }
    }


    // Copying every parsed declarator to the actual ast node list.
    Ast_Node *parsed_struct_member_declarations = arena_state.ptr;
    for (s32 i = 0; i < node.struct_definition.members_count; i++) {
        array_list_append(&ast_node_list, parsed_struct_member_declarations[i]);
    }

    node.struct_definition.members_index = array_list_length(&ast_node_list) - node.struct_definition.members_count;

    // @Important: Clearing struct mebmer declarations arena after.
    arena_ast_node_struct_member_declaration = arena_state;

    return node;

backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_declarator(Lexer *lexer) {
    Lexer original_state = *lexer;
    
    if (lexer_expect(lexer, TOKEN_ASTERISK)) {
        Ast_Node node = {
            .kind = AST_NODE_POINTER_DECLARATOR,
        };

        lexer_next_token(lexer);

        // Getting qualifiers.
        while (true) {
            if (try_parse_qualifier(lexer->token, &node.pointer_declarator.qualifier_flags) == 0) {
                lexer_next_token(lexer);
                continue;
            }
            break;
        }


        Ast_Node child_declarator = ast_parse_declarator(lexer);
        if (child_declarator.kind == AST_NODE_UNKNOWN) {
            return node;
        }

        array_list_append(&ast_node_list, child_declarator);
        node.pointer_declarator.direct_declarator_index = array_list_length(&ast_node_list) - 1;

        return node;
    }

    // Parsing direct declarator.
    return ast_parse_direct_declarator(lexer);

backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_direct_declarator_postfix(Lexer *lexer, Ast_Node child) {
    Lexer original_state = *lexer;

    Ast_Node node = {0}; 

    if (lexer_expect(lexer, TOKEN_SQR_BRACES_OPEN)) {
        lexer_next_token(lexer);
        node.kind = AST_NODE_ARRAY_DECLARATOR;

        // TODO: Parse constant_expression_opt.
        Ast_Node constant_expression = ast_parse_constant_expression(lexer);
        if (constant_expression.kind != AST_NODE_INTEGER) {
            META_LOG_ERROR("Expected single integer constant expression for array length.");
            goto backtrack;
        }
        array_list_append(&ast_node_list, constant_expression);
        node.array_declarator.constant_expression_index = array_list_length(&ast_node_list) - 1;


        if (!lexer_expect(lexer, TOKEN_SQR_BRACES_CLOSE)) {
            META_LOG_ERROR("Expected ']'");
            goto backtrack;
        }
        lexer_next_token(lexer);

        array_list_append(&ast_node_list, child);
        node.array_declarator.direct_declarator_index = array_list_length(&ast_node_list) - 1;
    }
    else if (lexer_expect(lexer, TOKEN_PARAN_OPEN)) {
        lexer_next_token(lexer);
        node.kind = AST_NODE_FUNCTION_DECLARATOR;
    

        if (str_equals(lexer->token.str, VOID_STR)) {
            lexer_next_token(lexer);
            if (!lexer_expect(lexer, TOKEN_PARAN_CLOSE)) {
                META_LOG_ERROR("Expected ')'");
                goto backtrack;
            }
            lexer_next_token(lexer);
        } else if (lexer_expect(lexer, TOKEN_PARAN_CLOSE)) {
            lexer_next_token(lexer);
        } else {
            // Parsing function params declarations, semicolon separated.
            // Saving arena state.
            Arena arena_state = arena_ast_node_function_param_declaration;
            while (true) {
                Ast_Node function_param_declaration = ast_parse_function_param_declaration(lexer);
                if (function_param_declaration.kind == AST_NODE_UNKNOWN) {
                    META_LOG_ERROR("Couldn't parse function param declaration.");
                    goto backtrack;
                }

                node.function_declarator.params_count++;
                Ast_Node *next = arena_alloc(&arena_ast_node_function_param_declaration, sizeof(Ast_Node));

                *next = function_param_declaration;

                if (lexer_expect(lexer, TOKEN_COMMA)) {
                    lexer_next_token(lexer);
                    continue;
                }
                
                if (!lexer_expect(lexer, TOKEN_PARAN_CLOSE)) {
                    META_LOG_ERROR("Expected ')'");
                    goto backtrack;
                }

                lexer_next_token(lexer);
                break;
            }


            // Copying every parsed declarator to the actual ast node list.
            Ast_Node *parsed_function_param_declarations = arena_state.ptr;
            for (s32 i = 0; i < node.function_declarator.params_count; i++) {
                array_list_append(&ast_node_list, parsed_function_param_declarations[i]);
            }

            node.function_declarator.params_index = array_list_length(&ast_node_list) - node.function_declarator.params_count;

            // @Important: Clearing function param declarations arena after.
            arena_ast_node_function_param_declaration = arena_state;
        }

        array_list_append(&ast_node_list, child);
        node.function_declarator.direct_declarator_index = array_list_length(&ast_node_list) - 1;
    }
    else {
        // Nothing was parsed returning child.
        return child;
    }

    return ast_parse_direct_declarator_postfix(lexer, node);

backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_direct_declarator(Lexer *lexer) {
    Lexer original_state = *lexer;

    Ast_Node node = {0};

    if (lexer_expect(lexer, TOKEN_SYMBOL)) {
        // @TODO: Check if valid symbol is used for identifier.
        node.kind = AST_NODE_IDENTIFIER_DECLARATOR;
        node.identifier_declarator.name = string_list_save(lexer->token.str);

        lexer_next_token(lexer);
    }
    else if (lexer_expect(lexer, TOKEN_PARAN_OPEN)) {
        lexer_next_token(lexer);
        node = ast_parse_declarator(lexer);

        if (node.kind == AST_NODE_UNKNOWN) {
            goto backtrack;
        }

        if (!lexer_expect(lexer, TOKEN_PARAN_CLOSE)) {
            META_LOG_ERROR("Expected ')'");
            goto backtrack;
        }
        lexer_next_token(lexer);
    }
    else {
        // If there are no declarator, then make it abstract.
        node.kind = AST_NODE_ABSTRACT_DECLARATOR;
    }

    // Looking for postfix.
    return ast_parse_direct_declarator_postfix(lexer, node);

backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}

Ast_Node ast_parse_declaration(Lexer *lexer) {
    Lexer original_state = *lexer;
    // Declaration -> StorageSpecifier? Qualifiers? TypeSpecifier Declarator*


    Ast_Node node = {
        .kind = AST_NODE_DECLARATION,
        .declaration.type_specifier.definition_index = NULL_AST_NODE_INDEX,
    };

    


    // Getting declaration specifiers.
    while (true) {
        if (try_parse_qualifier(lexer->token, &node.declaration.qualifier_flags) == 0) {
            lexer_next_token(lexer);
            continue;
        }

        // Try to get StorageSpecifier, there is only one storage specifier per declaration allowed.
        if (node.declaration.storage_specifier_flags == 0 && try_parse_storage_specifier(lexer->token, &node.declaration.storage_specifier_flags) == 0) {
            lexer_next_token(lexer);
            continue;
        }

        // @Important: Type specifier comes always last, because it is the only parsing that can accept arbirtary unknown symbol as type name.
        if (try_parse_type_specifier(lexer, &node.declaration.type_specifier) == 0) {
            continue;
        }

        break;
    }


    // We can move forward if we have at least one type_specifier present.
    // To check that we need to have either an actual typename or a modifier flag present like (unsigned or long).
    if (node.declaration.type_specifier.name.length == 0 && node.declaration.type_specifier.flags == 0) {
        META_LOG_ERROR("No type specifier was found in declaration.");
        goto backtrack;
    }


    // Edge case: Early return if no declarators are present.
    if (lexer_expect(lexer, TOKEN_SEMICOLON)) {
        lexer_next_token(lexer);
        return node;
    }


    // Optional if no semicolon: Parsing declarators, comma separated.
    Arena arena_state = arena_ast_node_declarators;
    while (true) {
        
        Ast_Node declarator = ast_parse_declarator(lexer);
        if (declarator.kind == AST_NODE_UNKNOWN) {
            // META_LOG_ERROR("Couldn't parse declarator.");
            goto backtrack;
        }

        node.declaration.declarators_count++;
        Ast_Node *next = arena_alloc(&arena_ast_node_declarators, sizeof(Ast_Node));
    
        *next = declarator;

        if (lexer->token.type == TOKEN_COMMA) {
            lexer_next_token(lexer);
            continue;
        }

        break;
    }


    // Copying every parsed declarator to the actual ast node list.
    Ast_Node *parsed_declarators = arena_state.ptr;
    for (s32 i = 0; i < node.declaration.declarators_count; i++) {
        array_list_append(&ast_node_list, parsed_declarators[i]);
    }

    node.declaration.declarators_index = array_list_length(&ast_node_list) - node.declaration.declarators_count;

    // @Important: Clearing declarator arena after.
    arena_ast_node_declarators = arena_state;



    // In case of a function just stop declaration once function definition starts.
    if (lexer_expect(lexer, TOKEN_CURLY_OPEN)) {
        return node;
    }


    if (!lexer_expect(lexer, TOKEN_SEMICOLON)) {
        META_LOG_ERROR("Expected ';' at the end of declaration.");
        goto backtrack;
    }

    lexer_next_token(lexer);



    return node;

backtrack:
    *lexer = original_state;
    return (Ast_Node) {0};
}







void ast_print_ident(int depth) {
    for (int i = 0; i < depth; i++)
        printf("\t");
}

void ast_print_type_specifier(Type_Specifier *type_specifier, int depth) {
    ast_print_ident(depth);
    printf("| Type Specifier:\n");
    switch(type_specifier->kind) {
        case TYPE_IDENTIFIER_INT:
            ast_print_ident(depth);
            printf("\t| Kind: INT\n");
            break;
        case TYPE_IDENTIFIER_CHAR:
            ast_print_ident(depth);
            printf("\t| Kind: CHAR\n");
            break;
        case TYPE_IDENTIFIER_FLOAT:
            ast_print_ident(depth);
            printf("\t| Kind: FLOAT\n");
            break;
        case TYPE_IDENTIFIER_DOUBLE:
            ast_print_ident(depth);
            printf("\t| Kind: DOUBLE\n");
            break;
        case TYPE_IDENTIFIER_BOOL:
            ast_print_ident(depth);
            printf("\t| Kind: BOOL\n");
            break;
        case TYPE_IDENTIFIER_VOID:
            ast_print_ident(depth);
            printf("\t| Kind: VOID\n");
            break;
        case TYPE_IDENTIFIER_TYPEDEF:
            ast_print_ident(depth);
            printf("\t| Kind: TYPEDEF\n");
            break;
        case TYPE_IDENTIFIER_STRUCT:
            ast_print_ident(depth);
            printf("\t| Kind: STRUCT\n");
            break;
        case TYPE_IDENTIFIER_ENUM:
            ast_print_ident(depth);
            printf("\t| Kind: ENUM\n");
            break;
        case TYPE_IDENTIFIER_UNION:
            ast_print_ident(depth);
            printf("\t| Kind: UNION\n");
            break;
    }

    for (int i = 0; i < MODIFIER_KEYWORDS_LENGTH; i++) {
        if (type_specifier->flags & (1 << i)) {
            ast_print_ident(depth);
            printf("\t| Modifier: '%.*s'\n", UNPACK(MODIFIER_KEYWORDS[i]));
        }
    }

    ast_print_ident(depth);
    printf("\t| Name: '%.*s'\n", UNPACK(type_specifier->name));


    if (type_specifier->definition_index != NULL_AST_NODE_INDEX) {
        ast_print_ident(depth);
        printf("\t| Definition:\n");

        ast_print(ast_node_list + type_specifier->definition_index, depth + 2);
    }
}

void ast_print(Ast_Node *node, int depth) {
    Ast_Node *declarators;
    Ast_Node *members;
    Ast_Node *params;
    switch(node->kind) {
        case AST_NODE_POINTER_DECLARATOR:
            ast_print_ident(depth);
            printf("AST_NODE_POINTER_DECLARATOR\n");

            ast_print(ast_node_list + node->pointer_declarator.direct_declarator_index, depth + 1);
            break;
        case AST_NODE_ARRAY_DECLARATOR:
            ast_print_ident(depth);
            printf("AST_NODE_ARRAY_DECLARATOR\n");

            ast_print_ident(depth);
            printf("| Lenght: %d\n", ast_node_list[node->array_declarator.constant_expression_index].integer.value);

            ast_print(ast_node_list + node->array_declarator.direct_declarator_index, depth + 1);
            break;
        case AST_NODE_FUNCTION_DECLARATOR:
            ast_print_ident(depth);
            printf("AST_NODE_FUNCTION_DECLARATOR\n");

            ast_print_ident(depth);
            printf("| %d Params:\n", node->function_declarator.params_count);
            params = ast_node_list + node->function_declarator.params_index;
            for (s32 i = 0; i < node->function_declarator.params_count; i++) {
                ast_print(params + i, depth + 1);
            }

            ast_print_ident(depth);
            printf("| Direct Declarator:\n");
            ast_print(ast_node_list + node->function_declarator.direct_declarator_index, depth + 1);
            break;
        case AST_NODE_IDENTIFIER_DECLARATOR:
            ast_print_ident(depth);
            printf("AST_NODE_IDENTIFIER_DECLARATOR\n");

            ast_print_ident(depth);
            printf("| Identifier: '%.*s'\n", UNPACK(node->identifier_declarator.name));
            break;
        case AST_NODE_ABSTRACT_DECLARATOR:
            ast_print_ident(depth);
            printf("AST_NODE_ABSTRACT_DECLARATOR\n");
            break;
        case AST_NODE_DECLARATION:
            ast_print_ident(depth);
            printf("AST_NODE_DECLARATION\n");
            for (int i = 0; i < STORAGE_SPECIFIER_KEYWORDS_LENGTH; i++) {
                if (node->declaration.storage_specifier_flags & (1 << i)) {
                    ast_print_ident(depth);
                    printf("| Storage Specifier: '%.*s'\n", UNPACK(STORAGE_SPECIFIER_KEYWORDS[i]));
                }
            }
            for (int i = 0; i < QUALIFIER_KEYWORDS_LENGTH; i++) {
                if (node->declaration.qualifier_flags & (1 << i)) {
                    ast_print_ident(depth);
                    printf("| Qualifier: '%.*s'\n", UNPACK(QUALIFIER_KEYWORDS[i]));
                }
            }

            ast_print_type_specifier(&node->declaration.type_specifier, depth);

            ast_print_ident(depth);
            printf("| %d Declarators:\n", node->declaration.declarators_count);
            declarators = ast_node_list + node->declaration.declarators_index;
            for (s32 i = 0; i < node->declaration.declarators_count; i++) {
                ast_print(declarators + i, depth + 1);
            }
            break;
        case AST_NODE_STRUCT_DEFINITION:
            ast_print_ident(depth);
            printf("AST_NODE_STRUCT_DEFINITION\n");

            ast_print_ident(depth);
            printf("| %d Members:\n", node->struct_definition.members_count);
            members = ast_node_list + node->struct_definition.members_index;
            for (s32 i = 0; i < node->struct_definition.members_count; i++) {
                ast_print(members + i, depth + 1);
            }
            break;
        case AST_NODE_STRUCT_MEMBER_DECLARATION:
            ast_print_ident(depth);
            printf("AST_NODE_STRUCT_MEMBER_DECLARATION\n");

            for (int i = 0; i < QUALIFIER_KEYWORDS_LENGTH; i++) {
                if (node->struct_member_declaration.qualifier_flags & (1 << i)) {
                    ast_print_ident(depth);
                    printf("| Qualifier: '%.*s'\n", UNPACK(QUALIFIER_KEYWORDS[i]));
                }
            }

            ast_print_type_specifier(&node->struct_member_declaration.type_specifier, depth);

            ast_print_ident(depth);
            printf("| %d Declarators:\n", node->struct_member_declaration.declarators_count);
            declarators = ast_node_list + node->struct_member_declaration.declarators_index;
            for (s32 i = 0; i < node->struct_member_declaration.declarators_count; i++) {
                ast_print(declarators + i, depth + 1);
            }
            break;
        case AST_NODE_FUNCTION_PARAM_DECLARATION:
            ast_print_ident(depth);
            printf("AST_NODE_FUNCTION_PARAM_DECLARATION\n");

            for (int i = 0; i < QUALIFIER_KEYWORDS_LENGTH; i++) {
                if (node->function_param_declaration.qualifier_flags & (1 << i)) {
                    ast_print_ident(depth);
                    printf("| Qualifier: '%.*s'\n", UNPACK(QUALIFIER_KEYWORDS[i]));
                }
            }

            ast_print_type_specifier(&node->function_param_declaration.type_specifier, depth);

            ast_print(ast_node_list + node->function_param_declaration.declarator_index, depth + 1);
            break;
        case AST_NODE_UNKNOWN:
            ast_print_ident(depth);
            printf("AST_NODE_UNKNOWN\n");
            break;
    }
}

// @TODO: Rewrite as a loop.
String ast_get_declarator_name(Ast_Node *declarator) {
    switch(declarator->kind) {
        case AST_NODE_POINTER_DECLARATOR:
            return ast_get_declarator_name(ast_node_list + declarator->pointer_declarator.direct_declarator_index);
            break;
        case AST_NODE_ARRAY_DECLARATOR:
            return ast_get_declarator_name(ast_node_list + declarator->array_declarator.direct_declarator_index);
            break;
        case AST_NODE_FUNCTION_DECLARATOR:
            return ast_get_declarator_name(ast_node_list + declarator->function_declarator.direct_declarator_index);
            break;
        case AST_NODE_IDENTIFIER_DECLARATOR:
            return declarator->identifier_declarator.name;
            break;
        case AST_NODE_ABSTRACT_DECLARATOR:
            return (String) {0};
            break;
        default:
            META_LOG_ERROR("Couldn't find declarator name.");
            return (String) {0};
            break;
    }
}
