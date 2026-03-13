#ifndef AST_H
#define AST_H


#include "core/str.h"
#include "meta/lexer.h"
#include "meta/meta.h"

#include <stdio.h>


#define FLAG_ORDINAL(flag) (__builtin_ctz(flag))

#define NULL_AST_NODE_INDEX 0xffffffff

/**
 * This list of strings keeps tracks of actual string saves in arena_strings.
 * It acts like set, saving needed strings only once and then reusing them as needed.
 * @Important: Use array_list_length(&string_list) to get the length of the list.
 */
extern String *string_list;

/**
 * Linearly checks is string already exists, if so reuses existing string rather then saving new.
 */
String string_list_save(String str);

/**
 * Saves string to the string list regardless of whether it is a duplicate or not.
 */
String string_list_force_save(String str);


/**
 * Some of the following flags are mutually exclusive, and cannot be simultaneously be active on a declaration, but they are still grouped as the flags, because some them can be combined together. 
 * Therefore whether the certain combination is legal or not doesn't matter, because parser will make sure its valid, and if it is not for some reason it will result in error at compilation stage ofan actual program. 
 * Be aware that this flags correspond to existence of certain keyword in declaration, for example TYPE_MODIFIER_SIGNED means there is signed keyword, it is intended to detect and report illegal combination of various keywords.
 */
typedef enum qualifier_flags : u8 {
    TYPE_QUALIFIER_CONST           = 0x01,
    TYPE_QUALIFIER_VOLATILE        = 0x02,
    TYPE_QUALIFIER_RESTRICT        = 0x04,
} Qualifier_Flags;

static const String QUALIFIER_KEYWORDS[] = {
    STR_BUFFER("const"),
    STR_BUFFER("volatile"),
    STR_BUFFER("restrict"),
};

#define QUALIFIER_KEYWORDS_LENGTH (sizeof(QUALIFIER_KEYWORDS) / sizeof(String))


typedef enum modifier_flags : u8 {
    TYPE_MODIFIER_SIGNED           = 0x01,
    TYPE_MODIFIER_UNSIGNED         = 0x02,
    TYPE_MODIFIER_SHORT            = 0x04,
    TYPE_MODIFIER_LONG             = 0x08,
    TYPE_MODIFIER_LONG_LONG        = 0x10,
} Modifier_Flags;

static const String MODIFIER_KEYWORDS[] = {
    STR_BUFFER("signed"),
    STR_BUFFER("unsigned"),
    STR_BUFFER("short"),
    STR_BUFFER("long"),
    STR_BUFFER("long long"),
};

#define MODIFIER_KEYWORDS_LENGTH (sizeof(MODIFIER_KEYWORDS) / sizeof(String))


typedef enum storage_specifier_flags : u8 {
    STORAGE_SPECIFIER_STATIC       = 0x01,
    STORAGE_SPECIFIER_EXTERN       = 0x02,
    STORAGE_SPECIFIER_REGISTER     = 0x04,
    STORAGE_SPECIFIER_AUTO         = 0x08,
    STORAGE_SPECIFIER_TYPEDEF      = 0x10,
} Storage_Specifier_Flags;

static const String STORAGE_SPECIFIER_KEYWORDS[] = {
    STR_BUFFER("static"),
    STR_BUFFER("extern"),
    STR_BUFFER("register"),
    STR_BUFFER("auto"),
    STR_BUFFER("typedef"),
};

#define STORAGE_SPECIFIER_KEYWORDS_LENGTH (sizeof(STORAGE_SPECIFIER_KEYWORDS) / sizeof(String))


/**
 * Tries to parse TOKEN_SYMBOL as a storage specifier.
 * Outputs it into supplied output destination.
 * Returns 0 if successful.
 */
int try_parse_storage_specifier(Token symbol, Storage_Specifier_Flags *output);


/**
 * Tries to parse TOKEN_SYMBOL as a qualifier.
 * Outputs it into supplied output destination.
 * Returns 0 if successful.
 */
int try_parse_qualifier(Token symbol, Qualifier_Flags *output);


typedef enum type_identifier_kind : u8 {
    TYPE_IDENTIFIER_INT = 0,
    TYPE_IDENTIFIER_CHAR,
    TYPE_IDENTIFIER_FLOAT,
    TYPE_IDENTIFIER_DOUBLE,
    TYPE_IDENTIFIER_BOOL,
    TYPE_IDENTIFIER_VOID,
    TYPE_IDENTIFIER_TYPEDEF,
    TYPE_IDENTIFIER_STRUCT,
    TYPE_IDENTIFIER_ENUM,
    TYPE_IDENTIFIER_UNION,
} Type_Identifier_Kind;

static const String INT_STR         = STR_BUFFER("int");
static const String CHAR_STR        = STR_BUFFER("char");
static const String FLOAT_STR       = STR_BUFFER("float");
static const String DOUBLE_STR      = STR_BUFFER("double");
static const String BOOL_STR        = STR_BUFFER("bool");
static const String VOID_STR        = STR_BUFFER("void");
static const String STRUCT_STR      = STR_BUFFER("struct");
static const String ENUM_STR        = STR_BUFFER("enum");
static const String UNION_STR       = STR_BUFFER("union");

/**
 * Type Specifiers are like base types, that can both be user defined struct ..., enum ..., union ..., and even typedef. Or built in types like int, float, double, long, long long, void, short char, unsigned, signed char, etc...
 */
typedef struct type_specifier {
    Type_Identifier_Kind kind;
    Modifier_Flags flags;
    String name;
    u32 definition_index;
} Type_Specifier;

/**
 * Tries to parse next token(s) as a type specifier.
 * Outputs it into supplied output destination.
 * Returns 0 if successful.
 */
int try_parse_type_specifier(Lexer *lexer, Type_Specifier *output);


typedef struct ast_node Ast_Node;

typedef enum ast_node_kind : u8 {
    AST_NODE_UNKNOWN                = 0,
    AST_NODE_STRUCT_DEFINITION,
    AST_NODE_STRUCT_MEMBER_DECLARATION,
    AST_NODE_FUNCTION_PARAM_DECLARATION,
    AST_NODE_DECLARATION,
    AST_NODE_POINTER_DECLARATOR,
    AST_NODE_FUNCTION_DECLARATOR,
    AST_NODE_ARRAY_DECLARATOR,
    AST_NODE_IDENTIFIER_DECLARATOR,
    AST_NODE_ABSTRACT_DECLARATOR,
    AST_NODE_INTEGER,
    AST_NODE_FLOAT,
    AST_NODE_NOTE,
} Ast_Node_Kind;


typedef struct {
    s32 members_count;
    u32 members_index;
} Ast_Node_Struct_Definition;

typedef struct {
    Qualifier_Flags qualifier_flags;
    Type_Specifier type_specifier;
    s32 declarators_count;
    u32 declarators_index;
    s32 notes_count;
    u32 notes_index;
} Ast_Node_Struct_Member_Declaration;

typedef struct {
    Qualifier_Flags qualifier_flags;
    Type_Specifier type_specifier;
    u32 declarator_index;
    // s32 notes_count;
    // u32 notes_index;
} Ast_Node_Function_Param_Declaration;

typedef struct {
    Storage_Specifier_Flags storage_specifier_flags;
    Qualifier_Flags qualifier_flags;
    Type_Specifier type_specifier;
    s32 declarators_count;
    u32 declarators_index;
} Ast_Node_Declaration;

typedef struct {
    String name;
} Ast_Node_Identifier_Declarator;

typedef struct {
    Qualifier_Flags qualifier_flags;
    u32 direct_declarator_index;
} Ast_Node_Pointer_Declarator;

typedef struct {
    u32 direct_declarator_index;
    s32 params_count;
    u32 params_index;
} Ast_Node_Function_Declarator;

typedef struct {
    u32 constant_expression_index;
    u32 direct_declarator_index;
} Ast_Node_Array_Declarator;

typedef struct {
    s64 value;
} Ast_Node_Integer;

typedef struct {
    float value;
} Ast_Node_Float;

typedef struct {
    String name;
} Ast_Node_Note;


typedef struct ast_node {
    Ast_Node_Kind kind;

    union {
        Ast_Node_Struct_Definition struct_definition;
        Ast_Node_Struct_Member_Declaration struct_member_declaration;
        Ast_Node_Function_Param_Declaration function_param_declaration;
        Ast_Node_Declaration declaration;
        Ast_Node_Pointer_Declarator pointer_declarator;
        Ast_Node_Function_Declarator function_declarator;
        Ast_Node_Array_Declarator array_declarator;
        Ast_Node_Identifier_Declarator identifier_declarator;
        Ast_Node_Integer integer;
        Ast_Node_Float floating_point;
        Ast_Node_Note note;
    };
} Ast_Node;


/**
 * Stores all nodes in growing array list.
 * @Important: Access elements by indicies, so there are no unsafe situations occuring.
 */
extern Ast_Node *ast_node_list;

/**
 * Contains indicies to the root ast nodes (usually declarations).
 */
extern u32 *ast_root_nodes_index_list;

/**
 * Tag table uses tag names of struct, enum, union to store index of such definition.
 * @Important: If tag is declared but not defined, and it is not in the table yet it will be stored without definition, meaning index will be equal to NULL_AST_NODE_INDEX.
 */
extern u32 *tag_definition_table;

/**
 * Typedef table acts similar to the tag table except. If typedef is declared but not yet defined it will error rather than storing typedef. Indicies stored here will always be valid.
 * Primitives are not included here. Only typedef.
 */
extern u32 *typedef_definition_table;

/**
 * @Temporary: For right now parser will not parse any complex constat expressions. 
 * Only one token and accept if it either integer or float.
 */
Ast_Node ast_parse_constant_expression(Lexer *lexer);

/**
 * Expects only one token accepted 'TOKEN_METANOTE', if not, returns AST_NODE_UNKNOWN.
 */
Ast_Node ast_parse_note(Lexer *lexer);


Ast_Node ast_parse_function_param_declaration(Lexer *lexer);

/**
 * Recursivly parse these kind of syntax: **a[10] where 'a' is identifier returned to the very top.
 * If end leaf doesn't contain identifier it recursivly returns empty string, meaning we parsed Abstract Declarator, for example: *[10].
 * Can be a case when you have a sizeof like:
 *
 *      sizeof(int *[10]) 
 *
 *
 *  declarator:
 *      pointer_opt direct_declarator
 *
 *  direct_declarator:
 *      identifier
 *      ( declarator )
 *      direct_declarator [ constant_expression_opt ]
 *      direct_declarator ( parameter_type_list_opt )*
 */
Ast_Node ast_parse_declarator(Lexer *lexer);

/**
 *  direct_declarator:
 *      identifier
 *      ( declarator )
 *      direct_declarator [ constant_expression_opt ]
 *      direct_declarator ( parameter_type_list_opt )*
 */
Ast_Node ast_parse_direct_declarator(Lexer *lexer);

/**
 *  direct_declarator (postfix):
 *      direct_declarator [ constant_expression_opt ]
 *      direct_declarator ( parameter_type_list_opt )*
 */
Ast_Node ast_parse_direct_declarator_postfix(Lexer *lexer, Ast_Node child);

Ast_Node ast_parse_struct_definition(Lexer *lexer);

/**
 * Basically declaration is a very broad abstraction and it is a valid declaration if it begins with any of the declaration specifiers like: StorageSpecifier, TypeQualifier, TypeSpecifier.
 * Most of the parsing is dealing with declarations properly.
 */
Ast_Node ast_parse_declaration(Lexer *lexer);

/**
 * Recursivly prints passed ast node to stdout.
 */
void ast_print(Ast_Node *node, int depth);

/**
 * Recursivly walks down declarator ast branch and returns declarator name if any.
 */
String ast_get_declarator_name(Ast_Node *declarator);

/**
 * Inits ast functionality, called once before parsing begins.
 */
int ast_init();


#endif
