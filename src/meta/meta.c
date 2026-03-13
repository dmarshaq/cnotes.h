#include <stdio.h>

#include "core/type.h"
#include "core/typeinfo.h"
#include "core/str.h"
#include "core/arena.h"
#include "core/file.h"
#include "core/log.h"
#include "core/structs.h"

#include "meta/lexer.h"
#include "meta/meta.h"
#include "meta/ast.h"

#include <stdarg.h>



const char* debug_meta_str = "\033[34m[META]\033[0m";
#define meta_log(format, ...)                 (void)fprintf(stderr, "%s " format, debug_meta_str, ##__VA_ARGS__);




#include "meta/introspect.h"
#include "meta/basic.h"

static Meta_Processor processors[] = {
    (Meta_Processor) { introspect_init, introspect_process, introspect_output, sizeof(INTROSPECT_METANOTES) / sizeof(String), INTROSPECT_METANOTES },
    (Meta_Processor) { basic_init, basic_process, basic_output, sizeof(BASIC_METANOTES) / sizeof(String), BASIC_METANOTES },
};

#define PROCESSORS_COUNT (sizeof(processors) / sizeof(Meta_Processor))


static Meta_State state = {0};



#define META_PRINT_PREFIX ANSI_MAGENTA"[META]"ANSI_RESET" "

void meta_log_error(const char *file_name, s64 line, const char *function_name, char *format, ...) {
    fprintf(stderr, META_PRINT_PREFIX);

    va_list args;
    va_start(args, format);
    if (state.current_file_name != NULL) {
        fprintf(stderr, "%s:%llu ", state.current_file_name, state.lexer.line_num);

        log_vprint(LOG_LEVEL_ERROR, file_name, line, function_name, format, args);
        va_end(args);
        
        lexer_print_snippet_token(&state.lexer);

        return;
    }

    log_vprint(LOG_LEVEL_ERROR, file_name, line, function_name, format, args);
    va_end(args);
}

void meta_log_info(const char *file_name, s64 line, const char *function_name, char *format, ...) {
    fprintf(stderr, META_PRINT_PREFIX);

    va_list args;
    va_start(args, format);
    if (state.current_file_name != NULL) {
        fprintf(stderr, "%s:%llu ", state.current_file_name, state.lexer.line_num);
    }

    log_vprint(LOG_LEVEL_INFO, file_name, line, function_name, format, args);
    va_end(args);
}






void meta_replace_with_space(String metanote_str) {
    for (s64 i = 0; i < metanote_str.length; i++) {
        metanote_str.data[i] = ' ';
    }
}


/**
 * Build files are source files that are copied into 'output_path' directory.
 * They are the files that meta program operates on directly.
 * It ensures original source files are never modified, and allows multiple accesses to edit of the build files. Which is important for the some of the meta processors.
 * @Important: Outputs build_file_name into provided buffer, it must be big enough to hold the name.
 */
int meta_create_build_file(char *file_name, char *output_path, char *build_file_name_buffer) {
    String _content = read_file_into_str(file_name, &std_allocator);

    if (_content.data == NULL) {
        META_LOG_ERROR("Couldn't read file '%s'.", file_name);
        return -1;
    }

    // Write source to build.
    strcpy(build_file_name_buffer, output_path);
    strcat(build_file_name_buffer, "/");
    strcat(build_file_name_buffer, file_name);

#ifdef LOG
    META_LOG_INFO("Producing build file '%s'.", build_file_name_buffer);
#endif

    if (write_str_to_file(_content, build_file_name_buffer) != 0) {
        META_LOG_ERROR("Couldn't create build file '%s'.", build_file_name_buffer);
        return -1;
    }

    allocator_free(&std_allocator, _content.data);

    return 0;
}


/**
 * Meta processing of individual file.
 * It will fwrite generated output to the output file.
 * @Important: output must be opened in "a" mode, in order for this function to properly append generated code.
 */
int meta_process_file(Meta_Processor *processor, char *file_name, FILE *output_h, char *output_path) {
#ifdef LOG
    META_LOG_INFO("Processing '%s'.", file_name);
#endif

    state.current_file_name = file_name;
    
    String _content = read_file_into_str(state.current_file_name, &std_allocator);

    if (_content.data == NULL) {
        META_LOG_ERROR("Couldn't read file '%s'.", state.current_file_name);
        return -1;
    }
    state.current_file_source = _content;

    
    
    Token next;


    // Triggering specific metanote.
    lexer_init(&state.lexer, _content);

    while(true) {
        lexer_next_token(&state.lexer);
        next = state.lexer.token;

        if (next.type == TOKEN_ZERO) break;


        // Searching for metanotes.
        if (next.type == TOKEN_METANOTE) {

#ifdef LOG
            META_LOG_INFO("Found meta note '%.*s'.", UNPACK(next.str));
#endif

            if (next.str.length <= 1) {
                META_LOG_ERROR("Missing TOKEN_METANOTE name.");
                return -1;
            }



            // Triggering specific metanote.
            for (int s = 0; s < processor->notes_length; s++) {
                if (str_equals(processor->notes[s], next.str)) {
                    Lexer temp = state.lexer;
                    if (processor->process(&state.lexer) != 0) {
                        return -1;
                    }
                    state.lexer = temp;
                    goto metanote_loop_continue;
                }
            }

            // Extra check, replace later with more clever system of collecting meta notes.
            for (int j = 0; j < PROCESSORS_COUNT; j++) {
                for (int s = 0; s < processors[j].notes_length; s++) {
                    if (str_equals(processors[j].notes[s], next.str)) {
                        goto metanote_loop_continue;
                    }
                }
            }

            meta_replace_with_space(next.str);

metanote_loop_continue:
        }
    }


    



    // Write source to back.
#ifdef LOG
    META_LOG_INFO("Write back source file '%s'.", state.current_file_name);
#endif

    if (write_str_to_file(_content, state.current_file_name) != 0) {
        META_LOG_ERROR("Couldn't write to source file '%s'.", state.current_file_name);
        return -1;
    }


    allocator_free(&std_allocator, _content.data);




    return 0;
}













String meta_generated_comment = CSTR(
        "/**\n"
        " * THIS FILE IS AUTO GENERATED BY META PROGRAM\n"
        " *\n"
        " * It will contain auto generated code that then is appended to the main compilation.\n"
        " * Do not modify any code, it will be overwritten.\n"
        " */\n"
        );


/**
 * This function overwrites file specified by file_name.
 * Then opens it in "a" mode and returns pointer to it, ready to accept auto generated code.
 * Needs to closed with fclose after use.
 */
FILE *meta_generate(char *file_name) {
    if (write_str_to_file(meta_generated_comment, file_name) != 0) {
        META_LOG_ERROR("Couldn't overwrite meta generated file '%s'.", file_name);
        return NULL;
    }

    FILE *file = fopen(file_name, "ab");
    if (file == NULL) {
        META_LOG_ERROR("Couldn't open meta generated file for appending '%s'.", file_name);
        return NULL;
    }

    return file;
}









// Essentially meta program is compiled into a file and immediately executed as a preprocces.
int meta_process(int count, char **files, char *output_path) {
    printf("\n\n");

    
#ifdef LOG
    META_LOG_INFO("Files passed to meta program: ");
    for (int i = 0; i < count; i++) {
        printf("'%s' ", files[i]);
    }
    printf("\n");
#endif

    if (ast_init() != 0) {
        META_LOG_ERROR("Couldn't init parser.");
        return -1;
    }

    for (int i = 0; i < PROCESSORS_COUNT; i++) {
        if (processors[i].init(&state) != 0) {
            META_LOG_ERROR("Couldn't init meta processor.");
            return -1;
        }
    }


    char *meta_generated_file_name = "src/meta_generated.h";
    char meta_generated_full_path[strlen(output_path) + 1 + strlen(meta_generated_file_name) + 1];
    strcpy(meta_generated_full_path, output_path);
    strcat(meta_generated_full_path, "/");
    strcat(meta_generated_full_path, meta_generated_file_name);

    // Preparing meta generated files.
    FILE *meta_generated_h = meta_generate(meta_generated_full_path);

    if (meta_generated_h == NULL) {
        return -1;
    }
    
    
    // Header defines
    fwrite_str(STR_BUFFER("#ifndef META_GENERATED_H\n#define META_GENERATED_H\n\n"), meta_generated_h);



    /**
     * Creating build files.
     * Find a way to not do so many loops.
     */
    u64 capacity = 0;
    int output_path_length = strlen(output_path);
    for (int i = 0; i < count; i++) {
        capacity += strlen(files[i]);
    }
    capacity += (output_path_length + 2) * count; // + 2 to account for '/' and '\0'.

    char *build_file_names[count];
    char *build_file_names_mem = malloc(capacity); // @Leak.
    
    u64 offset = 0;
    for (int i = 0; i < count; i++) {
        build_file_names[i] = build_file_names_mem + offset;
        if (meta_create_build_file(files[i], output_path, build_file_names[i]) != 0) {
            fclose(meta_generated_h);
            return -1;
        }
        offset += strlen(build_file_names[i]) + 1;
    }


    /**
     * Processing each file.
     */
    for (int j = 0; j < PROCESSORS_COUNT; j++) {
        for (int i = 0; i < count; i++) {
            if (meta_process_file(&processors[j], build_file_names[i], meta_generated_h, output_path) != 0) {
                fclose(meta_generated_h);
                return -1;
            }
        }
        if (processors[j].output(meta_generated_h) != 0) {
            META_LOG_ERROR("Couldn't output from meta processor.");
            return -1;
        }
    }




    // /**
    //  * Calculating type sizes.
    //  */
    // if (type_table_calculate_sizes() != 0) {
    //     return 1;
    // }
    // 

    // /**
    //  * Generating Introspect Meta data.
    //  */

    // // Include
    // fwrite_str(STR_BUFFER("#include \"core/typeinfo.h\"\n"), meta_generated_h);
    // fwrite_str(STR_BUFFER("#include \"game/command.h\"\n\n"), meta_generated_h);

    // // Tool defines.
    // fwrite_str(STR_BUFFER("#define META_TYPE(type) META_TYPE_##type\n\n"), meta_generated_h);

    // fwrite_str(STR_BUFFER("#define TYPE_OF(type) (&META_TYPE_TABLE[META_TYPE(type)])\n\n"), meta_generated_h);
    // // First generating Meta_Type enum.
    // fwrite_str(STR_BUFFER("typedef enum meta_type {\n"), meta_generated_h);
    // for (u32 i = 0; i < hash_table_capacity(&type_table); i++) {
    //     Hash_Table_Slot *slot = _hash_table_get_slot((void **)&type_table, i);
    //     if (slot->state == SLOT_OCCUPIED) {
    //         fprintf(meta_generated_h, "    META_TYPE(%.*s),\n", UNPACK(slot->key));
    //     }
    // }
    // fwrite_str(STR_BUFFER("} Meta_Type;\n\n"), meta_generated_h);

    // // Forward declaring META_TYPE_TABLE
    // fwrite_str(STR_BUFFER("static Type_Info META_TYPE_TABLE[];\n\n"), meta_generated_h);

    // // Generating META_TYPE_FUNCTION_ARGS[]
    // fwrite_str(STR_BUFFER("static Type_Info_Function_Argument META_TYPE_FUNCTION_ARGS[] = {\n"), meta_generated_h);
    // for (u32 i = 0; i < arena_size(&arena_type_info_function_argument) / sizeof(Type_Info_Function_Argument); i++) {
    //     Type_Info_Function_Argument *arg = ((Type_Info_Function_Argument *)arena_type_info_function_argument.allocation) + i;
    //     String typename = type_table_get_typename(arg->type);
    //     fprintf(meta_generated_h, "    { TYPE_OF(%.*s), STR_BUFFER(\"%.*s\")", UNPACK(typename), UNPACK(arg->name));

    //     fwrite_str(STR_BUFFER(" },\n"), meta_generated_h);
    // }
    // fwrite_str(STR_BUFFER("};\n\n"), meta_generated_h);


    // // Generating META_TYPE_STRUCT_MEMBERS[]
    // fwrite_str(STR_BUFFER("static Type_Info_Struct_Member META_TYPE_STRUCT_MEMBERS[] = {\n"), meta_generated_h);
    // for (u32 i = 0; i < arena_size(&arena_type_info_struct_member) / sizeof(Type_Info_Struct_Member); i++) {
    //     Type_Info_Struct_Member *member = ((Type_Info_Struct_Member *)arena_type_info_struct_member.allocation) + i;
    //     String typename = type_table_get_typename(member->type);
    //     fprintf(meta_generated_h, "    { TYPE_OF(%.*s), STR_BUFFER(\"%.*s\"), %u", UNPACK(typename), UNPACK(member->name), member->offset);

    //     fwrite_str(STR_BUFFER(" },\n"), meta_generated_h);
    // }
    // fwrite_str(STR_BUFFER("};\n\n"), meta_generated_h);


    // // Generating META_TYPE_ENUM_MEMBERS[]
    // fwrite_str(STR_BUFFER("static Type_Info_Enum_Member META_TYPE_ENUM_MEMBERS[] = {\n"), meta_generated_h);
    // for (u32 i = 0; i < arena_size(&arena_type_info_enum_member) / sizeof(Type_Info_Enum_Member); i++) {
    //     Type_Info_Enum_Member *member = ((Type_Info_Enum_Member *)arena_type_info_enum_member.allocation) + i;
    //     fprintf(meta_generated_h, "    { STR_BUFFER(\"%.*s\"), %llu", UNPACK(member->name), member->value);

    //     fwrite_str(STR_BUFFER(" },\n"), meta_generated_h);
    // }
    // fwrite_str(STR_BUFFER("};\n\n"), meta_generated_h);


    // // Now generating type table itself
    // fwrite_str(STR_BUFFER("static Type_Info META_TYPE_TABLE[] = {\n"), meta_generated_h);
    // for (u32 i = 0; i < hash_table_capacity(&type_table); i++) {
    //     Hash_Table_Slot *slot = _hash_table_get_slot((void **)&type_table, i);
    //     Type_Info *item;
    //     if (slot->state == SLOT_OCCUPIED) {
    //         item = type_table[i];


    //         fprintf(meta_generated_h, "    [META_TYPE(%.*s)] = (Type_Info) { ", UNPACK(slot->key));
    //         
    //         // Switching between type groups.
    //         switch(item->type) {
    //             case INTEGER:
    //                 fprintf(meta_generated_h, "INTEGER, STR_BUFFER(\"%.*s\"), %u, %u, .t_integer = { %d, %d }", UNPACK(item->name), item->size, item->align, item->t_integer.size_bits, item->t_integer.is_signed);
    //                 break;
    //             case FLOAT:
    //                 fprintf(meta_generated_h, "FLOAT, STR_BUFFER(\"%.*s\"), %u, %u, .t_float = { %d }", UNPACK(item->name), item->size, item->align, item->t_float.size_bits);
    //                 break;
    //             case BOOL:
    //                 fprintf(meta_generated_h, "BOOL, STR_BUFFER(\"%.*s\"), %u, %u", UNPACK(item->name), item->size, item->align);
    //                 break;
    //             case POINTER:
    //                 /**
    //                  * What this line assumes is that every pointer typename is 'basename' + '_ptr' at the end, so by cutting off '_ptr' we get typename of the base type.
    //                  */
    //                 String ptr_to_typename = slot->key;
    //                 ptr_to_typename.length -= TYPE_PTR_POSTFIX.length;
    //                
    //                 fprintf(meta_generated_h, "POINTER, STR_BUFFER(\"%.*s\"), %u, %u, .t_pointer = { TYPE_OF(%.*s) }", UNPACK(item->name), item->size, item->align, UNPACK(ptr_to_typename));
    //                 break;
    //             case FUNCTION:
    //                 String return_typename = type_table_get_typename(item->t_function.return_type);

    //                 u64 function_arg_index = item->t_function.arguments - (Type_Info_Function_Argument *)arena_type_info_function_argument.allocation; 
    //                 
    //                 fprintf(meta_generated_h, "FUNCTION, STR_BUFFER(\"%.*s\"), %u, %u, .t_function = { TYPE_OF(%.*s), %u, &META_TYPE_FUNCTION_ARGS[%llu], STR_BUFFER(\"%.*s\") }", UNPACK(item->name), item->size, item->align, UNPACK(return_typename), item->t_function.arguments_length, function_arg_index, UNPACK(item->t_function.definition_file));
    //                 break;
    //             case VOID:
    //                 fprintf(meta_generated_h, "VOID, STR_BUFFER(\"%.*s\"), %u, %u", UNPACK(item->name), item->size, item->align);
    //                 break;
    //             case STRUCT:
    //                 u64 struct_member_index = item->t_struct.members - (Type_Info_Struct_Member *)arena_type_info_struct_member.allocation; 
    //                 
    //                 fprintf(meta_generated_h, "STRUCT, STR_BUFFER(\"%.*s\"), %u, %u, .t_struct = { %u, &META_TYPE_STRUCT_MEMBERS[%llu] }", UNPACK(item->name), item->size, item->align, item->t_struct.members_length, struct_member_index);
    //                 break;
    //             case ARRAY:
    //                 TODO("Array meta generation.");
    //                 fwrite_str(STR_BUFFER("ARRAY, .t_array = { "), meta_generated_h);
    //                 // ...
    //                 fwrite_str(STR_BUFFER(" }"), meta_generated_h);
    //                 break;
    //             case ENUM:
    //                 u64 enum_member_index = item->t_enum.members - (Type_Info_Enum_Member *)arena_type_info_enum_member.allocation; 
    //                 
    //                 fprintf(meta_generated_h, "ENUM, STR_BUFFER(\"%.*s\"), %u, %u, .t_enum = { %d, %u, &META_TYPE_ENUM_MEMBERS[%llu] }", UNPACK(item->name), item->size, item->align, item->t_enum.is_signed, item->t_enum.members_length, enum_member_index);
    //                 break;
    //             case TYPEDEF:
    //                 String typedef_of_typename = type_table_get_typename(item->t_typedef.typedef_of);
    //                 fprintf(meta_generated_h, "TYPEDEF, STR_BUFFER(\"%.*s\"), %u, %u, .t_typedef = { TYPE_OF(%.*s) }", UNPACK(item->name), item->size, item->align, UNPACK(typedef_of_typename));
    //                 break;
    //             case UNKNOWN:
    //                 fprintf(meta_generated_h, "UNKNOWN, STR_BUFFER(\"%.*s\"), %u, %u", UNPACK(item->name), item->size, item->align);
    //                 break;
    //         }
    //        
    //         

    //         fwrite_str(STR_BUFFER(" },\n"), meta_generated_h);
    //     }
    // }
    // fwrite_str(STR_BUFFER("};\n"), meta_generated_h);





    /**
     * Generating registered functions code.
     */

    // fprintf(meta_generated_h, "\n\n");

    // // H files included.
    // for (u32 i = 0; i < array_list_length(&registered_functions_headers); i++) {
    //     fprintf(meta_generated_h, "#include \"%.*s\"\n", UNPACK(registered_functions_headers[i]));
    // }

    // // H file output.
    // for (u32 i = 0; i < array_list_length(&registered_functions); i++) {
    //     Type_Info *item = registered_functions[i];


    //     fprintf(meta_generated_h, "\nstatic void COMMAND_PREFIX(%.*s)(Any *args, u32 args_length) {\n", UNPACK(item->name));
    //     

    //     Type_Info *return_type = get_base_of_typedef(item->t_function.return_type);
    //     if (return_type->type != VOID) {
    //         fprintf(meta_generated_h, "    %.*s _rvalue = %.*s(", UNPACK(return_type->name), UNPACK(item->name));
    //     } 
    //     else {
    //         fprintf(meta_generated_h, "    %.*s(", UNPACK(item->name));
    //     }

    //     for (u32 j = 0; j < item->t_function.arguments_length; j++) {
    //         fprintf(meta_generated_h, "*(%.*s*)args[%u].data", UNPACK(item->t_function.arguments[j].type->name), j);

    //         if (j + 1 < item->t_function.arguments_length)
    //             fprintf(meta_generated_h, ", ");
    //     }
    //     fprintf(meta_generated_h, ");\n\n");

    //     
    //     if (return_type->type != VOID) {
    //         fprintf(meta_generated_h, "    args[0].type = TYPE_OF(%.*s);\n", UNPACK(return_type->name));
    //         fprintf(meta_generated_h, "    *(%.*s*)args[0].data = _rvalue;\n", UNPACK(return_type->name));
    //     }

    //     fprintf(meta_generated_h, "}\n");
    // }

    // fprintf(meta_generated_h, "\nstatic void register_all_commands() {\n");
    // for (u32 i = 0; i < array_list_length(&registered_functions); i++) {
    //     Type_Info *item = registered_functions[i];

    //     fprintf(meta_generated_h, "    command_register(TYPE_OF(%.*s), COMMAND_PREFIX(%.*s));\n", UNPACK(item->name), UNPACK(item->name));


    // }
    // fprintf(meta_generated_h, "}\n\n");









    // Ending header file.
    fwrite_str(STR_BUFFER("#endif\n"), meta_generated_h);




    fclose(meta_generated_h);

    // hash_table_print((void **)&type_table);

    printf("\n\n");

    return 0;
}
