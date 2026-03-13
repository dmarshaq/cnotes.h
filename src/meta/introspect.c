#include "meta/introspect.h"

#include "core/str.h"
#include "core/arena.h"
#include "core/structs.h"
#include "core/file.h"
#include "core/typeinfo.h"


#include "meta/lexer.h"
#include "meta/meta.h"
#include "meta/ast.h"

#include <stdio.h>


int introspect_init(Meta_State *state) {
    return 0;
}

int introspect_process(Lexer *lexer) {
    if (str_equals(lexer->token.str, INTROSPECT_METANOTES[0])) {
        String meta_note_str = lexer->token.str;

        meta_replace_with_space(meta_note_str);

        META_LOG_INFO("Introspect process.");
        lexer_next_token(lexer);
        
        if (lexer_expect(lexer, TOKEN_SEMICOLON)) {
            lexer_next_token(lexer);
        }



        /**
         * @TODO: Abstract following code away.
         * Ast interface should logically only accept, string of code.
         * And meta information about where this code is comming from.
         */

        Ast_Node node = ast_parse_declaration(lexer);

        if (node.kind == AST_NODE_DECLARATION) {
            // If declaration contained typedef record respective declarators as new typedefs.
            // @Important: Definition of typedef is a declaration node of typedef.
            if (node.declaration.storage_specifier_flags & STORAGE_SPECIFIER_TYPEDEF) {
                Ast_Node *typedef_declarators = ast_node_list + node.declaration.declarators_index;
                String name = {0};
                for (int i = 0; i < node.declaration.declarators_count; i++) {
                    name = ast_get_declarator_name(typedef_declarators + i);

                    if (hash_table_get(&typedef_definition_table, UNPACK(name)) != NULL) {
                        META_LOG_ERROR("Redefinition of typedef '%.*s'.", UNPACK(name));
                        return -1;
                    }

                    // @Important: ASSUMES that node will be added to the end of the list immideately after this logic ends.
                    hash_table_put(&typedef_definition_table, array_list_length(&ast_node_list), UNPACK(name));
                }
            }
        }

        array_list_append(&ast_node_list, node);
        array_list_append(&ast_root_nodes_index_list, array_list_length(&ast_node_list) - 1);

        // ---------------------




    }

    return 0;
}


static Arena arena_type_info_note;
static Arena arena_type_info_struct_member;
static Arena arena_type_info_function_param;


/**
 * Used to store and translate AST to the Type_Info data that then is baked into auto generated meta file for the user.
 * @Important: Array is used instead of a list because Type_Info's reference each other through pointers therefore using dynamicly growing array is unsafe. Constant memory addresses therefore are required.
 */
#define TYPE_ARRAY_CAPACITY 64

static Type_Info type_array[TYPE_ARRAY_CAPACITY];

static s64 type_array_length;


s64 type_array_add(Type_Info type) {
    if (type_array_length == TYPE_ARRAY_CAPACITY) {
        META_LOG_ERROR("Mas type limit of %d has been reached.", TYPE_ARRAY_CAPACITY);
        return -1;
    }
    type_array[type_array_length] = type;
    return type_array_length++;
}



/**
 * Type table stores indicies to the type info's to quickly access those by their names.
 */
static s64 *type_table; // @Leak.



Type_Info *type_table_get(String name) {
    s64 *index_ptr = hash_table_get(&type_table, UNPACK(name));
    return index_ptr == NULL ? NULL : type_array + *index_ptr;
}


#define NAME_BUFFER_SIZE 128

static String name_buffer           = STR(0, (char[NAME_BUFFER_SIZE]){});
static s64 name_buffer_insert_index = 0;

int name_buffer_add_suffix(String str) {
    if (str.length + name_buffer.length > NAME_BUFFER_SIZE) {
        META_LOG_ERROR("Constructed type name is too big, exceeds %d character buffer limit.", NAME_BUFFER_SIZE);
        return -1;
    }

    memmove(name_buffer.data + name_buffer_insert_index + str.length, name_buffer.data + name_buffer_insert_index, name_buffer.length - name_buffer_insert_index);

    str_copy_to(str, name_buffer.data + name_buffer_insert_index);
    name_buffer_insert_index += str.length;
    name_buffer.length += str.length;
    return 0;
}

int name_buffer_add_suffix_char(char c) {
    if (name_buffer.length == NAME_BUFFER_SIZE) {
        META_LOG_ERROR("Constructed type name is too big, exceeds %d character buffer limit.", NAME_BUFFER_SIZE);
        return -1;
    }

    memmove(name_buffer.data + name_buffer_insert_index + 1, name_buffer.data + name_buffer_insert_index, name_buffer.length - name_buffer_insert_index);

    name_buffer.data[name_buffer_insert_index] = c;
    name_buffer_insert_index++;
    name_buffer.length++;
    return 0;
}

int name_buffer_add_postfix(String str) {
    if (str.length + name_buffer.length > NAME_BUFFER_SIZE) {
        META_LOG_ERROR("Constructed type name is too big, exceeds %d character buffer limit.", NAME_BUFFER_SIZE);
        return -1;
    }

    memmove(name_buffer.data + name_buffer_insert_index + str.length, name_buffer.data + name_buffer_insert_index, name_buffer.length - name_buffer_insert_index);

    str_copy_to(str, name_buffer.data + name_buffer_insert_index);
    name_buffer.length += str.length;
    return 0;
}

int name_buffer_add_postfix_char(char c) {
    if (name_buffer.length == NAME_BUFFER_SIZE) {
        META_LOG_ERROR("Constructed type name is too big, exceeds %d character buffer limit.", NAME_BUFFER_SIZE);
        return -1;
    }

    memmove(name_buffer.data + name_buffer_insert_index + 1, name_buffer.data + name_buffer_insert_index, name_buffer.length - name_buffer_insert_index);

    name_buffer.data[name_buffer_insert_index] = c;
    name_buffer.length++;
    return 0;
}

int name_buffer_append_type_specifier(Qualifier_Flags flags, Type_Specifier *type_specifier) {
    int ok = 0;

    // Appending qualifiers.
    for (int i = 0; i < QUALIFIER_KEYWORDS_LENGTH; i++) {
        if ((1 << i) & flags) {
            ok += name_buffer_add_suffix(QUALIFIER_KEYWORDS[i]);
            ok += name_buffer_add_suffix_char(' ');
        }
    }

    // Appending type specifier.
    switch (type_specifier->kind) {
        case TYPE_IDENTIFIER_INT:
            if (type_specifier->flags & TYPE_MODIFIER_UNSIGNED) {
                ok += name_buffer_add_suffix(MODIFIER_KEYWORDS[FLAG_ORDINAL(TYPE_MODIFIER_UNSIGNED)]);
                ok += name_buffer_add_suffix_char(' ');
            }

            if (type_specifier->flags & TYPE_MODIFIER_SHORT) {
                ok += name_buffer_add_suffix(MODIFIER_KEYWORDS[FLAG_ORDINAL(TYPE_MODIFIER_SHORT)]);
                ok += name_buffer_add_suffix_char(' ');
            } else if (type_specifier->flags & TYPE_MODIFIER_LONG) {
                ok += name_buffer_add_suffix(MODIFIER_KEYWORDS[FLAG_ORDINAL(TYPE_MODIFIER_LONG)]);
                ok += name_buffer_add_suffix_char(' ');
            } else if (type_specifier->flags & TYPE_MODIFIER_LONG_LONG) {
                ok += name_buffer_add_suffix(MODIFIER_KEYWORDS[FLAG_ORDINAL(TYPE_MODIFIER_LONG_LONG)]);
                ok += name_buffer_add_suffix_char(' ');
            }

            ok += name_buffer_add_suffix(INT_STR);
            break;
        case TYPE_IDENTIFIER_CHAR:
            if (type_specifier->flags & TYPE_MODIFIER_UNSIGNED) {
                ok += name_buffer_add_suffix(MODIFIER_KEYWORDS[FLAG_ORDINAL(TYPE_MODIFIER_UNSIGNED)]);
                ok += name_buffer_add_suffix_char(' ');
            }

            ok += name_buffer_add_suffix(CHAR_STR);
            break;
        case TYPE_IDENTIFIER_STRUCT:
            ok += name_buffer_add_suffix(STRUCT_STR);
            ok += name_buffer_add_suffix_char(' ');

            ok += name_buffer_add_suffix(type_specifier->name);
            break;
        case TYPE_IDENTIFIER_ENUM:
            ok += name_buffer_add_suffix(ENUM_STR);
            ok += name_buffer_add_suffix_char(' ');

            ok += name_buffer_add_suffix(type_specifier->name);
            break;
        case TYPE_IDENTIFIER_UNION:
            ok += name_buffer_add_suffix(UNION_STR);
            ok += name_buffer_add_suffix_char(' ');

            ok += name_buffer_add_suffix(type_specifier->name);
            break;
        default:
            ok += name_buffer_add_suffix(type_specifier->name);
            break;
    }

    return ok < 0 ? -1 : 0;
}

int name_buffer_append_pointer_declarator(Qualifier_Flags qualifier_flags) {
    int ok = 0;
    
    ok += name_buffer_add_suffix_char('*');

    // Appending qualifiers.
    for (int i = 0; i < QUALIFIER_KEYWORDS_LENGTH; i++) {
        if ((1 << i) & qualifier_flags) {
            ok += name_buffer_add_suffix_char(' ');
            ok += name_buffer_add_suffix(QUALIFIER_KEYWORDS[i]);
            ok += name_buffer_add_suffix_char(' ');
        }
    }

    return ok < 0 ? -1 : 0;
}

int name_buffer_append_array_declarator(s64 size, bool include_paranthesis) {
    int ok = 0;
    
    String buf = STR_BUFFER_EMPTY(16);
    ok += name_buffer_add_postfix(str_format(buf, "[%lld]", size));

    // Adds paranthesis.
    if (include_paranthesis) {
        ok += name_buffer_add_suffix_char('(');
        ok += name_buffer_add_postfix_char(')');
    }

    return ok < 0 ? -1 : 0;
}



int name_buffer_append_function_declarator(bool include_paranthesis, String *param_strs, int param_strs_length) {
    int ok = 0;
    
    ok += name_buffer_add_postfix_char(')');
    String delimeter = STR_BUFFER(", ");

    for (int i = param_strs_length - 1; i > 0; i--) {
        ok += name_buffer_add_postfix(param_strs[i]);
        ok += name_buffer_add_postfix(delimeter);
    }
    if (param_strs_length > 0) {
        ok += name_buffer_add_postfix(param_strs[0]);
    }
    ok += name_buffer_add_postfix_char('(');

    // @TODO: Write logic for appending function params.

    // Adds paranthesis.
    if (include_paranthesis) {
        ok += name_buffer_add_suffix_char('(');
        ok += name_buffer_add_postfix_char(')');
    }

    return ok < 0 ? -1 : 0;
}

int name_buffer_append_declarator(Ast_Node *node);

int name_buffer_append_function_param_declaration(Ast_Node *node) {
    // @Important: Clear name buffer.
    name_buffer.length = 0;
    name_buffer_insert_index = 0;

    if (name_buffer_append_type_specifier(node->function_param_declaration.qualifier_flags, &node->function_param_declaration.type_specifier) != 0) {
        return -1;
    }

    return name_buffer_append_declarator(ast_node_list + node->function_param_declaration.declarator_index);
}

/**
 * Recursivly goes through declarators, appending each one to the name buffer.
 */
int name_buffer_append_declarator(Ast_Node *node) {
    String name;

    switch(node->kind) {
        case AST_NODE_POINTER_DECLARATOR: {
            if (name_buffer_append_pointer_declarator(node->pointer_declarator.qualifier_flags) == -1) return -1;

            return name_buffer_append_declarator(ast_node_list + node->pointer_declarator.direct_declarator_index);
        }
        case AST_NODE_ARRAY_DECLARATOR: {
            // @Important: Assumes array declarator has it's length specified.
            s64 array_length = ast_node_list[node->array_declarator.constant_expression_index].integer.value;
            if (name_buffer_append_array_declarator(array_length, ast_node_list[node->array_declarator.direct_declarator_index].kind == AST_NODE_POINTER_DECLARATOR) == -1) return -1;

            return name_buffer_append_declarator(ast_node_list + node->array_declarator.direct_declarator_index);
        }
        case AST_NODE_FUNCTION_DECLARATOR: {
            // Saving name_buffer state, because parsing params will cause buffer to change.
            int name_buffer_insert_index_state = name_buffer_insert_index;
            // TODO: Find a way to store a string and clean it after.
            String name = string_list_force_save(name_buffer);


            // Parsing function params.
            String function_param_strs[node->function_declarator.params_count];

            Ast_Node *function_params = ast_node_list + node->function_declarator.params_index;
            for (int i = 0; i < node->function_declarator.params_count; i++) {
                if (name_buffer_append_function_param_declaration(function_params + i) != 0) {
                    return -1;
                }
                
                // TODO: Find a way to store a string and clean it after.
                function_param_strs[i] = string_list_force_save(name_buffer);
            }


            // Loading saved name_buffer state.
            str_copy_to(name, name_buffer.data);
            name_buffer_insert_index = name_buffer_insert_index_state;
            name_buffer.length = name.length;

            // Completing function typename.
            if (name_buffer_append_function_declarator(ast_node_list[node->function_declarator.direct_declarator_index].kind == AST_NODE_POINTER_DECLARATOR, function_param_strs, node->function_declarator.params_count) == -1) return -1;
        }
        case AST_NODE_IDENTIFIER_DECLARATOR: {
            // Assumes parent is not NULL.
            return 0;
        }
        case AST_NODE_ABSTRACT_DECLARATOR: {
            // Assumes parent is not NULL.
            return 0;
        }
        default: {
            META_LOG_ERROR("Illegal ast node encountered, excpeted declarator ast node.");
            return -1;
        }
    }

}



Type_Info *type_table_parse_ast_node_declaration(Ast_Node *node);

void type_table_parse_ast_node_struct_member_declaration(Ast_Node *node, Type_Info_Struct_Member *members, int *count);


/**
 * Adds base type specified by type_specifier and qualifier flags, to the type table.
 * @Important: It uses specified name as a key in the table and actual name of the type.
 * Returns pointer to the added type info.
 * @Incomplete: Ignoring qualifier flags.
 */
Type_Info *type_table_parse_type_specifier(Qualifier_Flags flags, Type_Specifier *type_specifier) {
    name_buffer_append_type_specifier(flags, type_specifier);

    Type_Info *basetype = type_table_get(name_buffer);
    if (basetype != NULL) {
        return basetype;
    }

    Type_Info temp;

    String name = string_list_force_save(name_buffer);
    switch (type_specifier->kind) {
        case TYPE_IDENTIFIER_INT:
            temp.kind = INTEGER;
            temp.name = name;
            
            if (type_specifier->flags & TYPE_MODIFIER_SHORT) {
                temp.size = sizeof(short);
                temp.align = sizeof(short);
            } else if (type_specifier->flags & TYPE_MODIFIER_LONG) {
                temp.size = sizeof(long);
                temp.align = sizeof(long);
            } else if (type_specifier->flags & TYPE_MODIFIER_LONG_LONG) {
                temp.size = sizeof(long long);
                temp.align = sizeof(long long);
            } else {
                temp.size = sizeof(int);
                temp.align = sizeof(int);
            }

            temp.t_integer.is_signed = !(type_specifier->flags & TYPE_MODIFIER_UNSIGNED);
            break;
        case TYPE_IDENTIFIER_CHAR:
            temp.kind = INTEGER;
            temp.name = name;
            temp.size = sizeof(char);
            temp.align = sizeof(char);

            temp.t_integer.is_signed = !(type_specifier->flags & TYPE_MODIFIER_UNSIGNED);
            break;
        case TYPE_IDENTIFIER_FLOAT:
            temp.kind = FLOAT;
            temp.name = name;
            temp.size = sizeof(float);
            temp.align = sizeof(float);
            break;
        case TYPE_IDENTIFIER_DOUBLE:
            temp.kind = FLOAT;
            temp.name = name;
            temp.size = sizeof(double);
            temp.align = sizeof(double);
            break;
        case TYPE_IDENTIFIER_BOOL:
            temp.kind = BOOL;
            temp.name = name;
            temp.size = sizeof(bool);
            temp.align = sizeof(bool);
            break;
        case TYPE_IDENTIFIER_VOID:
            temp.kind = VOID;
            temp.name = name;
            temp.size = 0;
            temp.align = 0;
            break;
        case TYPE_IDENTIFIER_TYPEDEF:
            // Should never happen.
            return NULL;
            break;
        case TYPE_IDENTIFIER_STRUCT: {
            temp.kind = STRUCT;
            temp.name = name; // This stores full name: struct + 'tag'.

            
            // Looking for definition.
            u32 *definition_index_ptr = hash_table_get(&tag_definition_table, UNPACK(type_specifier->name));
            if (definition_index_ptr == NULL) {
                META_LOG_ERROR("Struct '%.*s' is declared, but not defined.", UNPACK(temp.name));
                return NULL;
            }

            // Saving name_buffer state, because parsing params will cause buffer to change.
            int name_buffer_insert_index_state = name_buffer_insert_index;


            Ast_Node *definition_node = ast_node_list + *definition_index_ptr;
            Ast_Node *members = ast_node_list + definition_node->struct_definition.members_index;
            int type_info_member_count = 0;
            
            // Before doing any parsing prealloc space needed for definition, because otherwise nested structs will be recursivly allocated extra in-between memory and cause invalid layout of produced Type_Info_Struct_Member's
            for (s32 i = 0; i < definition_node->struct_definition.members_count; i++) {
                type_info_member_count += members[i].struct_member_declaration.declarators_count;
            }

            Type_Info_Struct_Member *type_info_members = arena_alloc(&arena_type_info_struct_member, sizeof(Type_Info_Struct_Member) * type_info_member_count);

            temp.t_struct.members_length = type_info_member_count;
            temp.t_struct.members = type_info_members;
            temp.t_struct.tag = type_specifier->name;

            for (s32 i = 0; i < definition_node->struct_definition.members_count; i++) {
                int count = 0;

                // Technically this line will cause illegal memory access (overflow) if arena size is exceeded. But following arena_alloc should be able to detect it and error.
                type_table_parse_ast_node_struct_member_declaration(members + i, type_info_members, &count);
                type_info_members += count;
            }
            

            // Loading saved name_buffer state.
            str_copy_to(temp.name, name_buffer.data);
            name_buffer_insert_index = name_buffer_insert_index_state;
            name_buffer.length = temp.name.length;
            
            

            // Size and offsets calculations of struct
            u32 offset = 0;
            u32 max_align = 0;
            for (u32 i = 0; i < temp.t_struct.members_length; i++) {
                // Edge case: 0 aligned member.
                if (temp.t_struct.members[i].type->align == 0)
                    continue;

                // Setting offset to be next alligned offset;
                offset = (offset + temp.t_struct.members[i].type->align - 1) / temp.t_struct.members[i].type->align * temp.t_struct.members[i].type->align;
                temp.t_struct.members[i].offset = offset;
                offset += temp.t_struct.members[i].type->size;

                // Comparing with max align.
                if (max_align < temp.t_struct.members[i].type->align) {
                    max_align = temp.t_struct.members[i].type->align;
                }
            }

            temp.size = offset;
            temp.align = max_align;

            break;
        }
        case TYPE_IDENTIFIER_ENUM:
            temp.kind = ENUM;
            TODO("Enum type info conversion.");
            break;
        case TYPE_IDENTIFIER_UNION:
            temp.kind = UNION;
            TODO("Union type info conversion.");
            break;
        default:
            META_LOG_ERROR("Illegal type specifier encountered when building type info.");
            return NULL;
    }

    // print_type_info(&type);

    int index = type_array_add(temp);
    if (index == -1) {
        return NULL;
    }
    basetype = type_array + index;
    hash_table_put(&type_table, basetype - type_array, UNPACK(name));
    return basetype;
}


Type_Info *type_table_parse_ast_node_declarator(Ast_Node *node, Type_Info *parent) {
    Type_Info *type = NULL;
    Type_Info temp;
    String name;
    int name_buffer_insert_index_state;

    switch(node->kind) {
        case AST_NODE_POINTER_DECLARATOR: {
            // Assumes parent is not NULL.
            if (name_buffer_append_pointer_declarator(node->pointer_declarator.qualifier_flags) == -1) return NULL;

            // printf("=> '%.*s'\n", UNPACK(name_buffer));


            type = type_table_get(name_buffer);
            if (type == NULL) {
                name = string_list_force_save(name_buffer);

                temp = (Type_Info) {0};

                temp.kind = POINTER;
                temp.name = name;
                temp.size = sizeof(void *);
                temp.align = sizeof(void *);
                
                temp.t_pointer.ptr_to = parent;

                int index = type_array_add(temp);
                if (index == -1) {
                    return NULL;
                }
                type = type_array + index;
                hash_table_put(&type_table, type - type_array, UNPACK(name));
            }
            

            return type_table_parse_ast_node_declarator(ast_node_list + node->pointer_declarator.direct_declarator_index, type);
        }
        case AST_NODE_ARRAY_DECLARATOR: {

            // Assumes parent is not NULL.
            // @Important: Assumes array declarator has it's length specified.
            s64 array_length = ast_node_list[node->array_declarator.constant_expression_index].integer.value;
            if (name_buffer_append_array_declarator(array_length, ast_node_list[node->array_declarator.direct_declarator_index].kind == AST_NODE_POINTER_DECLARATOR) == -1) return NULL;

            // printf("=> '%.*s'\n", UNPACK(name_buffer));

            type = type_table_get(name_buffer);
            if (type == NULL) {
                name = string_list_force_save(name_buffer);

                temp = (Type_Info) {0};

                temp.kind = ARRAY;
                temp.name = name;
                temp.size = parent->size * (u32)array_length;
                temp.align = parent->align;
                
                temp.t_array.element_type = parent;
                temp.t_array.length = array_length;

                int index = type_array_add(temp);
                if (index == -1) {
                    return NULL;
                }
                type = type_array + index;
                hash_table_put(&type_table, type - type_array, UNPACK(name));
            }


            return type_table_parse_ast_node_declarator(ast_node_list + node->array_declarator.direct_declarator_index, type);
        }
        case AST_NODE_FUNCTION_DECLARATOR: {
            // Assumes parent is not NULL.
            
            // Saving name_buffer state, because parsing params will cause buffer to change.
            name_buffer_insert_index_state = name_buffer_insert_index;


            // Parsing function params.
            String function_param_strs[node->function_declarator.params_count];
            Type_Info *function_param_type_infos[node->function_declarator.params_count];

            Ast_Node *function_params = ast_node_list + node->function_declarator.params_index;
            for (int i = 0; i < node->function_declarator.params_count; i++) {
                function_param_type_infos[i] = type_table_parse_ast_node_declaration(function_params + i);
                function_param_strs[i] = function_param_type_infos[i]->name;
            }


            // Loading saved name_buffer state.
            str_copy_to(parent->name, name_buffer.data);
            name_buffer_insert_index = name_buffer_insert_index_state;
            name_buffer.length = parent->name.length;


            // Completing function typename.
            // @Inccomplete: Split function declarator if it is not a function pointer.
            if (name_buffer_append_function_declarator(ast_node_list[node->function_declarator.direct_declarator_index].kind == AST_NODE_POINTER_DECLARATOR, function_param_strs, node->function_declarator.params_count) == -1) return NULL;
            // printf("=> '%.*s'\n", UNPACK(name_buffer));

            type = type_table_get(name_buffer);
            if (type == NULL) {
                name = string_list_force_save(name_buffer);

                temp = (Type_Info) {0};

                temp.kind = FUNCTION;
                temp.name = name;
                temp.size = 0;
                temp.align = 0;
                
                temp.t_function.return_type = parent;
                temp.t_function.params_length = node->function_declarator.params_count;

                temp.t_function.params = arena_alloc(&arena_type_info_function_param, sizeof(Type_Info_Function_Param) * node->function_declarator.params_count);
                for (int i = 0; i < node->function_declarator.params_count; i++) {
                    temp.t_function.params[i] = (Type_Info_Function_Param) {
                        .type = function_param_type_infos[i],
                        .name = ast_get_declarator_name(ast_node_list + function_params[i].function_param_declaration.declarator_index),
                    };
                }

                // @TODO: Get access to meta info and assign temp.t_function.definition_file.

                int index = type_array_add(temp);
                if (index == -1) {
                    return NULL;
                }
                type = type_array + index;
                hash_table_put(&type_table, type - type_array, UNPACK(name));
            }


            return type_table_parse_ast_node_declarator(ast_node_list + node->function_declarator.direct_declarator_index, type);
        }
        case AST_NODE_IDENTIFIER_DECLARATOR: {
            // Assumes parent is not NULL.
            return parent;
        }
        case AST_NODE_ABSTRACT_DECLARATOR: {
            // Assumes parent is not NULL.
            return parent;
        }
        default: {
            META_LOG_ERROR("Illegal ast node encountered, excpeted declarator ast node.");
            return NULL;
        }
    }
}

/**
 * Parses Ast_Node_Struct_Member_Declaration into type info struct members.
 * Uses passed pointer parameters as outputs.
 * @Important: node->kind is assumed to be AST_NODE_STRUCT_MEMBER_DECLARATION.
 */
void type_table_parse_ast_node_struct_member_declaration(Ast_Node *node, Type_Info_Struct_Member *members, int *count) {
    // @Important: Clear name buffer.
    name_buffer.length = 0;
    name_buffer_insert_index = 0;

    // Base type present.
    Type_Info *basetype = type_table_parse_type_specifier(node->struct_member_declaration.qualifier_flags, &node->struct_member_declaration.type_specifier);

    // printf("=> '%.*s'\n", UNPACK(name_buffer));
    
    // Saving name_buffer state, because parsing params will cause buffer to change.
    int name_buffer_insert_index_state = name_buffer_insert_index;


    Ast_Node *notes = ast_node_list + node->struct_member_declaration.notes_index;
    String *type_info_notes = arena_alloc(&arena_type_info_note, sizeof(String *) * node->struct_member_declaration.notes_count);

    // Getting notes.
    for (s32 i = 0; i < node->struct_member_declaration.notes_count; i++) {
        type_info_notes[i] = notes[i].note.name;
    }

    Ast_Node *declarators = ast_node_list + node->struct_member_declaration.declarators_index;
    Type_Info *declarator_type;
    String declarator_name;
    for (s32 i = 0; i < node->struct_member_declaration.declarators_count; i++) {

        declarator_type = type_table_parse_ast_node_declarator(declarators + i, basetype);
        
        declarator_name = ast_get_declarator_name(declarators + i);

        // Offset is ignored because it is calculated once every Type_Info_Struct_Member is known.
        members[i] = (Type_Info_Struct_Member) {
            .type = declarator_type,
            .name = declarator_name,
            .notes_count = node->struct_member_declaration.notes_count,
            .notes = type_info_notes,
        };



        // Loading saved name_buffer state.
        str_copy_to(basetype->name, name_buffer.data);
        name_buffer_insert_index = name_buffer_insert_index_state;
        name_buffer.length = basetype->name.length;
    }
    
    *count = node->struct_member_declaration.declarators_count;
}

Type_Info *type_table_parse_ast_node_declaration(Ast_Node *node) {
    // @Important: Clear name buffer.
    name_buffer.length = 0;
    name_buffer_insert_index = 0;

    Type_Info *type = NULL;
    Type_Info temp;
    String name;
    int name_buffer_insert_index_state;

    switch (node->kind) {
        case AST_NODE_DECLARATION:
            // Base type present.
            type = type_table_parse_type_specifier(node->declaration.qualifier_flags, &node->declaration.type_specifier);

            // printf("=> '%.*s'\n", UNPACK(name_buffer));

            // Saving name_buffer state, because parsing params will cause buffer to change.
            name_buffer_insert_index_state = name_buffer_insert_index;
            
            Ast_Node *declarators = ast_node_list + node->declaration.declarators_index;
            Type_Info *declarator_type;
            for (s32 i = 0; i < node->declaration.declarators_count; i++) {
                // Loading saved name_buffer state.
                str_copy_to(type->name, name_buffer.data);
                name_buffer_insert_index = name_buffer_insert_index_state;
                name_buffer.length = type->name.length;

                declarator_type = type_table_parse_ast_node_declarator(declarators + i, type);
                if (node->declaration.storage_specifier_flags & STORAGE_SPECIFIER_TYPEDEF) {
                    name = ast_get_declarator_name(declarators + i);

                    // printf("=> '%.*s'\n", UNPACK(name));

                    if (str_empty(name)) {
                        META_LOG_ERROR("Cannot construct type info of unnamed typedef.");
                        return NULL;
                    }
                    if (type_table_get(name) != NULL) {
                        META_LOG_ERROR("Cannot construct type info for typedef, type '%.*s' already exists.", UNPACK(name));
                        return NULL;
                    }

                    temp = (Type_Info) {0};

                    temp.kind = TYPEDEF;
                    temp.name = name;
                    temp.size = declarator_type->size;
                    temp.align = declarator_type->align;

                    temp.t_typedef.typedef_of = declarator_type;

                    int index = type_array_add(temp);
                    if (index == -1) {
                        return NULL;
                    }
                    hash_table_put(&type_table, index, UNPACK(name));
                }
            }


            
            // Default declaration doesn't need to return anything usefull.
            return NULL;
        case AST_NODE_FUNCTION_PARAM_DECLARATION:
            // Base type present.
            type = type_table_parse_type_specifier(node->function_param_declaration.qualifier_flags, &node->function_param_declaration.type_specifier);

            // printf("=> '%.*s'\n", UNPACK(name_buffer));
            return type_table_parse_ast_node_declarator(ast_node_list + node->function_param_declaration.declarator_index, type);

    }
}



s64 introspect_type_info_get_parse(Lexer *lexer) {
    u32 ast_node_list_original_length = array_list_length(&ast_node_list);

    Lexer original_state = *lexer;

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


    Ast_Node declarator = ast_parse_declarator(lexer);
    if (declarator.kind == AST_NODE_UNKNOWN) {
        goto backtrack;
    }

    array_list_append(&ast_node_list, declarator);
    node.declaration.declarators_index = array_list_length(&ast_node_list) - 1;

    // Parsing the type.

    // @Important: Clear name buffer.
    name_buffer.length = 0;
    name_buffer_insert_index = 0;

    if (name_buffer_append_type_specifier(node.declaration.qualifier_flags, &node.declaration.type_specifier) != 0) {
        goto backtrack;
    }

    if (name_buffer_append_declarator(ast_node_list + node.declaration.declarators_index) != 0) {
        goto backtrack;
    }

    s64 *type_idx_ptr = hash_table_get(&type_table, UNPACK(name_buffer));
    if (type_idx_ptr == NULL) {
        goto backtrack;
    }


    array_list_pop_multiple(&ast_node_list, array_list_length(&ast_node_list) - ast_node_list_original_length);

    return *type_idx_ptr;
    
backtrack:
    
    array_list_pop_multiple(&ast_node_list, array_list_length(&ast_node_list) - ast_node_list_original_length);
    *lexer = original_state;
    return -1;
}



int introspect_output(FILE *output) {
    arena_type_info_note            = arena_make(2048);
    arena_type_info_struct_member   = arena_make(2048);
    arena_type_info_function_param  = arena_make(2048);

    type_table          = hash_table_make(s64, TYPE_ARRAY_CAPACITY, &std_allocator);
    type_array_length   = 0;
    
    

    
    Ast_Node *node;
    for (u32 i = 0; i  < array_list_length(&ast_root_nodes_index_list); i++) {
        node = ast_node_list + ast_root_nodes_index_list[i];

        

        if (node->kind == AST_NODE_DECLARATION) {
            type_table_parse_ast_node_declaration(node);
        }


        printf("\n---------------------------------------------------------------------------\n");
        ast_print(node, 0);
        printf("\n");

    }
    // printf("\n---------------------------------------------------------------------------\nStrings:\n");
    // for (u32 i = 0; i < array_list_length(&string_list); i++) {
    //     printf("'%.*s'  ", UNPACK(string_list[i]));
    // }
    // printf("\n");

    
    // hash_table_print((void **)&type_table);
    // for (int i = 0; i < type_array_length; i++) { print_type_info(type_array + i); }
    // hash_table_print((void **)&tag_definition_table);
    // hash_table_print((void **)&typedef_definition_table);


    // @Typeof(i);  // @Typeof(i) -> MTT(43890) -> META_TYPE_TABLE[43890]
                    // Annotation -> Macro      -> Code
    
    //@Typeof(MyStruct);


    /**
     * Generating Introspect Meta data.
     */

    // Include
    fwrite_str(STR_BUFFER("#include \"core/typeinfo.h\"\n"), output);

    // Tool defines.
    fwrite_str(STR_BUFFER("#define MTT(i) (META_TYPE_TABLE + (i))\n\n"), output);

    // Forward declaring META_TYPE_TABLE
    fwrite_str(STR_BUFFER("static Type_Info META_TYPE_TABLE[];\n\n"), output);

    // Generating META_TYPE_FUNCTION_ARGS[]
    fwrite_str(STR_BUFFER("static Type_Info_Function_Param META_TYPE_FUNCTION_PARAMS[] = {\n"), output);
    for (u32 i = 0; i < arena_size(&arena_type_info_function_param) / sizeof(Type_Info_Function_Param); i++) {
         Type_Info_Function_Param *param = ((Type_Info_Function_Param *)arena_type_info_function_param.allocation) + i;
         s64 *idx_ptr = hash_table_get(&type_table, UNPACK(param->type->name));
         fprintf(output, "    { MTT(%lld), STR_BUFFER(\"%.*s\") },\n", *idx_ptr, UNPACK(param->name));

    }
    fwrite_str(STR_BUFFER("};\n\n"), output);


    // Generating META_TYPE_STRUCT_MEMBERS[]
    fwrite_str(STR_BUFFER("static Type_Info_Struct_Member META_TYPE_STRUCT_MEMBERS[] = {\n"), output);
    for (u32 i = 0; i < arena_size(&arena_type_info_struct_member) / sizeof(Type_Info_Struct_Member); i++) {
         Type_Info_Struct_Member *member = ((Type_Info_Struct_Member *)arena_type_info_struct_member.allocation) + i;

         s64 *idx_ptr = hash_table_get(&type_table, UNPACK(member->type->name));
         fprintf(output, "    { MTT(%lld), STR_BUFFER(\"%.*s\"), %u, %d, ", *idx_ptr, UNPACK(member->name), member->offset, member->notes_count);

         fprintf(output, "((String[]){ ");
         for (s32 j = 0; j < member->notes_count; j++) {
             fprintf(output, "STR_BUFFER(\"%.*s\"), ", UNPACK(member->notes[j]));
         }
         fprintf(output, "}) },\n");

    }
    fwrite_str(STR_BUFFER("};\n\n"), output);


    // Generating META_TYPE_ENUM_MEMBERS[]
    fwrite_str(STR_BUFFER("static Type_Info_Enum_Member META_TYPE_ENUM_MEMBERS[] = {\n"), output);
    // for (u32 i = 0; i < arena_size(&arena_type_info_enum_member) / sizeof(Type_Info_Enum_Member); i++) {
    //     Type_Info_Enum_Member *member = ((Type_Info_Enum_Member *)arena_type_info_enum_member.allocation) + i;
    //     fprintf(output, "    { STR_BUFFER(\"%.*s\"), %llu", UNPACK(member->name), member->value);

    //     fwrite_str(STR_BUFFER(" },\n"), output);
    // }
    fwrite_str(STR_BUFFER("};\n\n"), output);


    // Now generating type table itself
    fwrite_str(STR_BUFFER("static Type_Info META_TYPE_TABLE[] = {\n"), output);
    for (s64 i = 0; i < type_array_length; i++) {
        fprintf(output, "    { STR_BUFFER(\"%.*s\"), %u, %u, ", UNPACK(type_array[i].name), type_array[i].size, type_array[i].align);
        switch(type_array[i].kind) {
            case INTEGER: 
                {
                    fprintf(output, "INTEGER, .t_integer = { %d }", type_array[i].t_integer.is_signed);
                    break;
                }
            case FLOAT:
                {
                    fprintf(output, "FLOAT");
                    break;
                }
            case BOOL:
                {
                    fprintf(output, "BOOL");
                    break;
                }
            case POINTER:
                {
                    s64 *ptr_to_idx_ptr = hash_table_get(&type_table, UNPACK(type_array[i].t_pointer.ptr_to->name));
                    fprintf(output, "POINTER, .t_pointer = { MTT(%lld) }", *ptr_to_idx_ptr);
                    break;
                }
            case FUNCTION:
                {
                    s64 *return_type_idx_ptr = hash_table_get(&type_table, UNPACK(type_array[i].t_function.return_type->name));
                    fprintf(output, "FUNCTION, .t_function = { MTT(%lld), %u, &META_TYPE_FUNCTION_PARAMS[%lld] }", *return_type_idx_ptr, type_array[i].t_function.params_length, type_array[i].t_function.params - (Type_Info_Function_Param *)arena_type_info_function_param.allocation);

                    break;
                }
            case ARRAY:
                {
                    s64 *element_type_idx_ptr = hash_table_get(&type_table, UNPACK(type_array[i].t_array.element_type->name));
                    fprintf(output, "ARRAY, .t_array = { MTT(%lld), %llu }", *element_type_idx_ptr, type_array[i].t_array.length);
                    break;
                }
            case VOID:
                {
                    fprintf(output, "VOID");
                    break;
                }
            case STRUCT:
                {
                    fprintf(output, "STRUCT, .t_struct = { %u, &META_TYPE_STRUCT_MEMBERS[%lld], STR_BUFFER(\"%.*s\") }", type_array[i].t_struct.members_length, type_array[i].t_struct.members - (Type_Info_Struct_Member *)arena_type_info_struct_member.allocation, UNPACK(type_array[i].t_struct.tag));
                    break;
                }
            case ENUM:
                {
                    TODO("Enum type info output.");
                    break;
                }
            case UNION:
                {
                    TODO("Union type info output.");
                    break;
                }
            case TYPEDEF:
                {
                    s64 *typedef_of_idx_ptr = hash_table_get(&type_table, UNPACK(type_array[i].t_typedef.typedef_of->name));
                    fprintf(output, "TYPEDEF, .t_typedef = { MTT(%lld) }", *typedef_of_idx_ptr);
                    break;
                }
        }
        fwrite_str(STR_BUFFER(" },\n"), output);
    }
    fwrite_str(STR_BUFFER("};\n"), output);

    return 0;
}
