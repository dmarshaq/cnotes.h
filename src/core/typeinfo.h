#ifndef TYPE_INFO_H
#define TYPE_INFO_H

/**
 * Type Info's.
 */

#include "core/type.h"
#include "core/str.h"

#include <stdbool.h>

typedef struct type_info Type_Info;

typedef enum : u8 {
    INTEGER,
    FLOAT,
    BOOL,
    POINTER,
    FUNCTION,
    ARRAY,
    VOID,
    STRUCT,
    ENUM,
    UNION,
    TYPEDEF,
    UNKNOWN,
} Type_Info_Kind;

typedef struct {
    bool is_signed;
} Type_Info_Integer;

typedef struct {
    Type_Info *ptr_to;
} Type_Info_Pointer;


typedef struct {
    Type_Info *type;
    String name;
} Type_Info_Function_Param;

typedef struct {
    Type_Info *return_type;

    u32 params_length;
    Type_Info_Function_Param *params;

    String definition_file; // Where the function was defined. Can be a header or c file, depends on where @Introspect is placed.
} Type_Info_Function;


typedef struct {
    Type_Info *type;
    String name;
    u32 offset;
    s32 notes_count;
    String *notes;
} Type_Info_Struct_Member;

typedef struct {
    u32 members_length;
    Type_Info_Struct_Member *members;
    String tag;
} Type_Info_Struct;

typedef struct {
    Type_Info *element_type;
    u64 length;
} Type_Info_Array;

typedef struct {
    String name;
    u64 value;
} Type_Info_Enum_Member;

typedef struct {
    bool is_signed;
    u32 members_length;
    Type_Info_Enum_Member *members;
} Type_Info_Enum;


typedef struct {
    Type_Info *typedef_of;
} Type_Info_Typedef;

struct type_info {
    String name;
    u32 size;
    u32 align;
    
    Type_Info_Kind kind;
    union {
        Type_Info_Integer   t_integer;
        Type_Info_Pointer   t_pointer;
        Type_Info_Function  t_function;
        Type_Info_Struct    t_struct;
        Type_Info_Array     t_array;
        Type_Info_Enum      t_enum;
        Type_Info_Typedef   t_typedef;
    };
};


typedef struct any {
    Type_Info *type;
    void *data;
} Any;

/**
 * 'buffer' should be big enough to hold info about 'any'.
 *  Returns a string that corresponds to the formatted 'any'.
 */
static String format_any(Any any, String buffer) {
    s64 written = 0;
    switch (any.type->kind) {
        case INTEGER:
            if (any.type->t_integer.is_signed) {
                switch(any.type->size) {
                    case 1:
                        written += snprintf(buffer.data, buffer.length, "%d", *(s8 *)any.data);
                        break;
                    case 2:
                        written += snprintf(buffer.data, buffer.length, "%d", *(s16 *)any.data);
                        break;
                    case 4:
                        written += snprintf(buffer.data, buffer.length, "%d", *(s32 *)any.data);
                        break;
                    case 8:
                        written += snprintf(buffer.data, buffer.length, "%lld", *(s64 *)any.data);
                        break;
                }
            }
            else {
                switch(any.type->size) {
                    case 1:
                        written += snprintf(buffer.data, buffer.length, "%u", *(u8 *)any.data);
                        break;
                    case 2:
                        written += snprintf(buffer.data, buffer.length, "%u", *(u16 *)any.data);
                        break;
                    case 4:
                        written += snprintf(buffer.data, buffer.length, "%u", *(u32 *)any.data);
                        break;
                    case 8:
                        written += snprintf(buffer.data, buffer.length, "%llu", *(u64 *)any.data);
                        break;
                }
            }
            break;
        case FLOAT:
            switch(any.type->size) {
                case 4:
                    written += snprintf(buffer.data, buffer.length, "%f", *(float *)any.data);
                    break;
            }
            break;
        case BOOL:
            written += snprintf(buffer.data, buffer.length, "%s", *(bool *)any.data ? "true" : "false");
            break;
        case POINTER:
            written += format_any(any, buffer).length;
            written += snprintf(buffer.data + written, buffer.length - written, "*");
            break;
        case FUNCTION:
            TODO("Function formatting ANY.");
            break;
        case VOID:
            written += snprintf(buffer.data, buffer.length, "void"); // Should not generally happen.
            break;
        case STRUCT:
            TODO("Struct formatting ANY.");
            break;
        case ARRAY:
            TODO("Array formatting ANY.");
            break;
        case ENUM:
            TODO("Enum formatting ANY.");
            break;
        case TYPEDEF:
            break;
        case UNKNOWN:
            written += snprintf(buffer.data, buffer.length, "unknown");
            break;
        default:
            written += snprintf(buffer.data, buffer.length, "undefined");
            break;
    }

    return STR(written, buffer.data);
}





static void print_type_info(Type_Info *type) {
    printf("type: '%.*s', size: %u, align: %u -> ", UNPACK(type->name), type->size, type->align);
    switch (type->kind) {
        case INTEGER:
            printf("INTEGER: size_bits: %d, %s\n", type->size * 8, type->t_integer.is_signed ? "signed" : "unsigned");
            break;
        case FLOAT:
            printf("FLOAT: size_bits: %d\n", type->size * 8);
            break;
        case BOOL:
            printf("BOOL\n");
            break;
        case POINTER:
            printf("POINTER to ");
            print_type_info(type->t_pointer.ptr_to);
            break;
        case FUNCTION:
            printf("FUNCTION\n");
            for (u32 i = 0; i < type->t_function.params_length; i++) {
                printf("    arg[%u] '%.*s': ", i, UNPACK(type->t_function.params[i].name));
                print_type_info(type->t_function.params[i].type);
            }
            printf("\n -> returns: ");
            print_type_info(type->t_function.return_type);
            break;
        case VOID:
            printf("VOID\n");
            break;
        case STRUCT:
            printf("STRUCT tag '%.*s'\n", UNPACK(type->t_struct.tag));
            for (u32 i = 0; i < type->t_struct.members_length; i++) {
                printf("    field[%u] '%.*s' ", i, UNPACK(type->t_struct.members[i].name));
                for (s32 j = 0; j < type->t_struct.members[i].notes_count; j++) {
                    printf("'%.*s' ", UNPACK(type->t_struct.members[i].notes[j]));
                }
                printf(": ");
                print_type_info(type->t_struct.members[i].type);
            }
            break;
        case ARRAY:
            printf("ARRAY of ");
            print_type_info(type->t_array.element_type);
            break;
        case ENUM:
            break;
        case TYPEDEF:
            printf("TYPEDEF of ");
            print_type_info(type->t_typedef.typedef_of);
            break;
        case UNKNOWN:
            printf("UNKNOWN\n");
            break;
        default:
            printf("UNDEFINED TYPE\n");
            break;
    }
}

static Type_Info *get_base_of_typedef(Type_Info *type) {
    while (type->kind == TYPEDEF) {
        type = type->t_typedef.typedef_of;
    }

    return type;
}





#endif
