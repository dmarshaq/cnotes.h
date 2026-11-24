/*  cnotes gnu - v0.1.0 - MIT License - https://github.com/dmarshaq/cnotes.h
 
    A header-only library extension of cnotes to be compatible with GNU C.
    
    "What does it mean to extend cnotes?" Simply put cnotes library allows
    not only creation of post meta programming hooks, but also full extension 
    of AST structure, analysis hooks, lexing and code emittion hooks, 
    subparsing hooks. Effectively GNU C support can be implemented as such.
    
    This is done to keep core `cnotes.h` clear from any dialect specifics,
    restricting it close to C99 standard.

    It also means that other dialects like MSVC could be implemented exactly
    in the same way.

    This file demonstrates how such cnotes extensions can be built,
    and also shows how strong core cnotes API can be.

    # Usage

    Include `cnotes.h` first, and then this file. Implementation is enabled by defining
    `CN_GNU_IMPLEMENTATION` in exactly one file, separately from `CN_IMPLEMENTATION`:

    ```
        #define CN_IMPLEMENTATION
        #include "cnotes.h"

        #define CN_GNU_IMPLEMENTATION
        #include "cnotes_gnu.h"
    ```
  
    # API Conventions

    This file follows the same convetions as cnotes, as it extends its functionality.
    However this extension development is separate from `cnotes.h` meaning versioning
    and changelog written here is exclusively for this file.
*/


#ifndef CN_GNU_H_
#define CN_GNU_H_

#define CN_GNU_EXTENSION_ID 1

#include "cnotes.h"


typedef enum {
    CN_AST_ASM_QUALIFIER_VOLATILE   = 0x01,
    CN_AST_ASM_QUALIFIER_INLINE     = 0x02,
    CN_AST_ASM_QUALIFIER_GOTO       = 0x04,
} Cn_Ast_Gnu_Asm_Qualifier_Flags;

typedef enum {
    CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED = 0x100,
} Cn_Ast_Gnu_Flags;

#define CN_AST_GNU_GEN_LIST(X)\
    X(GNU_TYPEOF,                       Gnu_Typeof,                         gnu_typeof)                     \
    X(GNU_ATTRIBUTE_SPECIFIER,          Gnu_Attribute_Specifier,            gnu_attribute_specifier)        \
    X(GNU_ATTRIBUTE,                    Gnu_Attribute,                      gnu_attribute)                  \
    X(GNU_ASM_STATEMENT,                Gnu_Asm_Statement,                  gnu_asm_statement)              \
    X(GNU_ASM_OPERAND,                  Gnu_Asm_Operand,                    gnu_asm_operand)                \
    X(GNU_ASM_LABEL,                    Gnu_Asm_Label,                      gnu_asm_label)                  \
    X(GNU_EXTENSIONS,                   Gnu_Extensions,                     gnu_extensions)


#define CN_AST_GNU_TYPEOF_MEMBERS(F, X)                 \
    F(X, NODE,  Cn_Ast_Node *,      target)             \

#define CN_AST_GNU_ATTRIBUTE_SPECIFIER_MEMBERS(F, X)    \
    F(X, NODES, Cn_Ast_Nodes,       gnu_attributes)     \

#define CN_AST_GNU_ATTRIBUTE_MEMBERS(F, X)              \
    F(X, NODE,  Cn_Ast_Identifier *,    identifier)     \
    F(X, NODES, Cn_Ast_Nodes,           arguments)      \

#define CN_AST_GNU_ASM_STATEMENT_MEMBERS(F, X)                      \
    F(X, INT32, Cn_Ast_Gnu_Asm_Qualifier_Flags,     qualifiers)     \
    F(X, NODE,  Cn_Ast_String *,                    template)       \
    F(X, NODES, Cn_Ast_Nodes,                       outputs)        \
    F(X, NODES, Cn_Ast_Nodes,                       inputs)         \
    F(X, NODES, Cn_Ast_Nodes,                       clobbers)       \
    F(X, NODES, Cn_Ast_Nodes,                       goto_labels)    \

#define CN_AST_GNU_ASM_OPERAND_MEMBERS(F, X)            \
    F(X, NODE,  Cn_Ast_Identifier *,    identifier)     \
    F(X, NODE,  Cn_Ast_String *,        string)         \
    F(X, NODE,  Cn_Ast_Node *,          expression)     \

#define CN_AST_GNU_ASM_LABEL_MEMBERS(F, X)              \
    F(X, NODE,  Cn_Ast_String *,        string)         \

#define CN_AST_GNU_EXTENSIONS_MEMBERS(F, X)             \
    F(X, INT32, int,                    count)          \


CN_AST_DECLARE_KINDS(GNU, Gnu, CN_GNU_EXTENSION_ID, CN_AST_GNU_GEN_LIST)
CN_AST_DECLARE_STRUCTS(CN_AST_GNU_GEN_LIST)
CN_AST_DEFINE_STRUCTS(CN_AST_GNU_GEN_LIST)
CN_AST_INFO_DECLARE_ARRAY(GNU)


/**
 * Parses GNU `typeof` specifier from the current token of `lexer`.
 *
 * ```
 *  gnu_typeof_specifier
 *          : 'typeof' '(' type_name ')'
 *          | 'typeof' '(' expression ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed `typeof` specifier, `NULL` on error.
 */
CNDEF Cn_Ast_Gnu_Typeof *cn_parse_gnu_typeof(Cn_Lexer *lexer);

/**
 * Parses GNU attribute specifier sequence from the current token of `lexer`.
 *
 * ```
 *  gnu_attribute_specifiers
 *          : gnu_attribute_specifier*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of GNU attribute specifiers.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_attribute_specifiers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `__extension__` keywords from the current token of `lexer`.
 *
 * ```
 *  gnu_extensions
 *          : '__extension__'+
 *          ;
 * ```
 *
 * RETURNS: Parsed keywords, `NULL` on error.
 *
 * NOTE: Keyword only disables pedantic diagnostics in GCC, so repeated keywords are collapsed
 * into a single node that counts them. It is attached as an extension to the node that follows.
 *
 * NOTE: Allowed only at the start of an external declaration, block item declaration, `for`
 * initialization declaration, member declaration, and as a unary operator in expressions.
 * Never after attribute specifiers, same as GCC.
 *
 * AI: Generated.
 */
CNDEF Cn_Ast_Gnu_Extensions *cn_parse_gnu_extensions(Cn_Lexer *lexer);

/**
 * Parses GNU attribute specifier from the current token of `lexer`.
 *
 * ```
 *  gnu_attribute_specifier
 *          : '__attribute__' '(' '(' gnu_attribute_list? ')' ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU attribute specifier, `NULL` on error.
 */
CNDEF Cn_Ast_Gnu_Attribute_Specifier *cn_parse_gnu_attribute_specifier(Cn_Lexer *lexer);

/**
 * Parses GNU attribute list from the current token of `lexer`.
 *
 * ```
 *  gnu_attributes
 *          : gnu_attribute (',' gnu_attribute)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of GNU attributes.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_attributes(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU attribute from the current token of `lexer`.
 *
 * ```
 *  gnu_attribute
 *          : identifier ( '(' arguments ')' )?
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU attribute, `NULL` on error.
 */
CNDEF Cn_Ast_Gnu_Attribute *cn_parse_gnu_attribute(Cn_Lexer *lexer);

/**
 * Parses GNU `asm` statement from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_statement
 *          : 'asm' gnu_asm_qualifier* '(' string gnu_asm_sections? ')' ';'
 *          ;
 *
 *  gnu_asm_sections
 *          : ':' gnu_asm_operands? ( ':' gnu_asm_operands? ( ':' gnu_asm_clobbers? ( ':' gnu_asm_goto_labels )? )? )?
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU `asm` statement, `NULL` on error.
 *
 * NOTE: Without `gnu_asm_sections` the statement is basic asm, otherwise it is extended asm.
 * First operand list holds outputs, second holds inputs. Lists that are not present are empty.
 *
 * NOTE: Constraints not expressed by the grammar, reported as errors:
 *  - each qualifier may appear at most once, in any order,
 *  - `goto` qualifier is not allowed in basic asm,
 *  - `goto` qualifier requires the goto labels section, with at least one label,
 *  - goto labels section is not allowed without `goto` qualifier.
 *
 * NOTE: Also used for file-scope asm, which is a basic asm statement appearing as an external
 * declaration. Callers at file scope should reject extended asm and any qualifiers, GCC and Clang
 * reject them too.
 *
 * AI: Generated.
 */
CNDEF Cn_Ast_Gnu_Asm_Statement *cn_parse_gnu_asm_statement(Cn_Lexer *lexer);

/**
 * Tries to parse GNU `asm` qualifier from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_qualifier
 *          : 'volatile'
 *          | 'inline'
 *          | 'goto'
 *          ;
 * ```
 *
 * RETURNS: `0` on success, `1` if token wasn't accepted, `2` on error.
 *
 * OUTPUTS: Parsed qualifier, added to `output`.
 *
 * NOTE: Returns `2` if the qualifier is already set in `output`.
 *
 * AI: Generated.
 */
CNDEF int cn_parse_try_gnu_asm_qualifier(Cn_Lexer *lexer, Cn_Ast_Gnu_Asm_Qualifier_Flags *output);

/**
 * Parses GNU `asm` operand list from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_operands
 *          : gnu_asm_operand (',' gnu_asm_operand)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of GNU `asm` operands.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * NOTE: Returns an empty list without consuming tokens if current token is `:` or `)`,
 * since operand lists may be empty.
 *
 * AI: Generated.
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_operands(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `asm` operand from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_operand
 *          : ('[' identifier ']')? string '(' expression ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU `asm` operand, `NULL` on error.
 *
 * NOTE: `string` is the constraint, e.g. `"=r"`, `"+m"`, `"i"`. Identifier is the optional
 * symbolic name referenced from the template as `%[name]`.
 *
 * AI: Generated.
 */
CNDEF Cn_Ast_Gnu_Asm_Operand *cn_parse_gnu_asm_operand(Cn_Lexer *lexer);

/**
 * Parses GNU `asm` clobber list from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_clobbers
 *          : string (',' string)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of clobber strings.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * NOTE: Returns an empty list without consuming tokens if current token is `:` or `)`.
 *
 * AI: Generated.
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_clobbers(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `asm goto` label list from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_goto_labels
 *          : identifier (',' identifier)*
 *          ;
 * ```
 *
 * RETURNS: Parsed list of label identifiers.
 *
 * OUTPUTS: `true` on success, `false` on error, written to `ok`.
 *
 * AI: Generated.
 */
CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_goto_labels(Cn_Lexer *lexer, bool *ok);

/**
 * Parses GNU `asm` label from the current token of `lexer`.
 *
 * ```
 *  gnu_asm_label
 *          : 'asm' '(' string ')'
 *          ;
 * ```
 *
 * RETURNS: Parsed GNU `asm` label, `NULL` on error.
 */
CNDEF Cn_Ast_Gnu_Asm_Label *cn_parse_gnu_asm_label(Cn_Lexer *lexer);

/**
 * GNU extension, registers every hook and ast node kind of GNU C.
 *
 * ```
 *  cn_extension_register(&cn_gnu_extension, ...);
 * ```
 *
 * RETURNS: `true` on success, `false` on error.
 */
CNDEF bool cn_gnu_extension(Cn_Hooks *hooks, Cn_Ast_Info_Array ast_info_table[256]);

#endif // CN_GNU_H_



#ifdef CN_IMPLEMENTATION
#ifndef CN_GNU_IMPLEMENTATION_GUARD_
#define CN_GNU_IMPLEMENTATION_GUARD_

CNDEF Cn_Ast_Gnu_Typeof *cn_parse_gnu_typeof(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;
    Cn_Ast_Gnu_Typeof node = { .id = CN_AST_GNU_TYPEOF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_TYPEOF)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Node *child;

    if (cn_parse_starts_type(lexer)) {
        child = cn_ast_node(cn_parse_type_name(lexer));
    } else {
        child = cn_parse_expression(lexer, 0, 0);

        // Typechecking expression right after parsing.
        if (cn_analyze_typecheck_expression(child) == NULL) goto error;
    }

    if (child == NULL) goto error;
    node.target = child;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Typeof *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Gnu_Extensions *cn_parse_gnu_extensions(Cn_Lexer *lexer) {
    Cn_Ast_Gnu_Extensions node = { .id = CN_AST_GNU_EXTENSIONS, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    while (cn_parse_optional(lexer, CN_TOKEN_GNU_EXTENSION)) node.count++;

    if (node.count == 0) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected '__extension__' keyword.");
        CN__TRACE_ERROR
        return NULL;
    }

    return cn_ast_new(node);
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_attribute_specifiers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    while (true) {
        if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
            *ok = true;
            return cn_ast_stack_finalize(mark);
        }

        Cn_Ast_Gnu_Attribute_Specifier *attribute_specifier = cn_parse_gnu_attribute_specifier(lexer);
        if (attribute_specifier == NULL) goto error;
        cn_ast_stack_push(attribute_specifier);
    }

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Gnu_Attribute_Specifier *cn_parse_gnu_attribute_specifier(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Attribute_Specifier node = { .id = CN_AST_GNU_ATTRIBUTE_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    if (!cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        bool ok;
        node.gnu_attributes = cn_parse_gnu_attributes(lexer, &ok); 
        if (!ok) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Attribute_Specifier *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_attributes(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    Cn_Ast_Gnu_Attribute *gnu_attribute = cn_parse_gnu_attribute(lexer);
    if (gnu_attribute == NULL) goto error;
    cn_ast_stack_push(gnu_attribute);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        gnu_attribute = cn_parse_gnu_attribute(lexer);
        if (gnu_attribute == NULL) goto error;
        cn_ast_stack_push(gnu_attribute);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Gnu_Attribute *cn_parse_gnu_attribute(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Attribute node = { .id = CN_AST_GNU_ATTRIBUTE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
    Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
    if (identifier == NULL) goto error;
    node.identifier = identifier;

    if (cn_parse_optional(lexer, CN_TOKEN_PARAN_OPEN)) {
        bool ok;
        node.arguments = cn_parse_arguments(lexer, &ok);
        if (!ok) goto error;

        if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    }

    Cn_Ast_Gnu_Attribute *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Gnu_Asm_Statement *cn_parse_gnu_asm_statement(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Statement node = { .id = CN_AST_GNU_ASM_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ASM)) goto error;

    // Qualifiers, in any order, each at most once.
    while (true) {
        int result = cn_parse_try_gnu_asm_qualifier(lexer, &node.qualifiers);
        if (result == 1) break;
        if (result == 2) goto error;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_String *template = cn_parse_string(lexer);
    if (template == NULL) goto error;
    node.template = template;

    // Sections, each one is only allowed after the previous one.
    bool has_goto_labels = false;
    bool ok;

    if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
        node.flags |= CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED;

        node.outputs = cn_parse_gnu_asm_operands(lexer, &ok);
        if (!ok) goto error;

        if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
            node.inputs = cn_parse_gnu_asm_operands(lexer, &ok);
            if (!ok) goto error;

            if (cn_parse_optional(lexer, CN_TOKEN_COLON)) {
                node.clobbers = cn_parse_gnu_asm_clobbers(lexer, &ok);
                if (!ok) goto error;

                if (cn_lexer_expect(lexer, CN_TOKEN_COLON)) {
                    if (!(node.qualifiers & CN_AST_ASM_QUALIFIER_GOTO)) {
                        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Goto labels section is not allowed without 'goto' asm qualifier.");
                        goto error;
                    }
                    cn_lexer_next_token(lexer);

                    node.goto_labels = cn_parse_gnu_asm_goto_labels(lexer, &ok);
                    if (!ok) goto error;
                    has_goto_labels = true;
                }
            }
        }
    }

    if (node.qualifiers & CN_AST_ASM_QUALIFIER_GOTO) {
        if (!(node.flags & CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED)) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &node.loc, &node.src, CN_DC_UNEXPECTED_TOKEN, "'goto' asm qualifier is not allowed in basic asm.");
            goto error;
        }
        if (!has_goto_labels) {
            cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_EXPECTED_TOKEN, "Expected goto labels section in 'asm goto'.");
            goto error;
        }
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_SEMICOLON)) goto error;

    Cn_Ast_Gnu_Asm_Statement *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF int cn_parse_try_gnu_asm_qualifier(Cn_Lexer *lexer, Cn_Ast_Gnu_Asm_Qualifier_Flags *output) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Qualifier_Flags qualifier;

    switch(cn_lexer_token(lexer).type) {
        case CN_TOKEN_VOLATILE: qualifier = CN_AST_ASM_QUALIFIER_VOLATILE; break;
        case CN_TOKEN_INLINE:   qualifier = CN_AST_ASM_QUALIFIER_INLINE;   break;
        case CN_TOKEN_GOTO:     qualifier = CN_AST_ASM_QUALIFIER_GOTO;     break;
        default:
            return 1;
    }

    if (*output & qualifier) {
        cn_diagnostic_src(CN_DIAGNOSTIC_ERROR, &cn_lexer_token(lexer).loc, &cn_lexer_token(lexer).src, CN_DC_UNEXPECTED_TOKEN, "Duplicate '%s' asm qualifier.", cn_token_kind_name(cn_lexer_token(lexer).type));
        goto error;
    }

    *output |= qualifier;
    cn_lexer_next_token(lexer);
    return 0;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return 2;
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_operands(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    // Empty operand list.
    if (cn_lexer_expect(lexer, CN_TOKEN_COLON) || cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        *ok = true;
        return cn_ast_stack_finalize(mark);
    }

    Cn_Ast_Gnu_Asm_Operand *operand = cn_parse_gnu_asm_operand(lexer);
    if (operand == NULL) goto error;
    cn_ast_stack_push(operand);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        operand = cn_parse_gnu_asm_operand(lexer);
        if (operand == NULL) goto error;
        cn_ast_stack_push(operand);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Gnu_Asm_Operand *cn_parse_gnu_asm_operand(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Operand node = { .id = CN_AST_GNU_ASM_OPERAND, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    // Optional symbolic name.
    if (cn_parse_optional(lexer, CN_TOKEN_SQR_BRACES_OPEN)) {
        Cn_Ast_Identifier *identifier = cn_parse_identifier(lexer);
        if (identifier == NULL) goto error;
        node.identifier = identifier;

        if (!cn_parse_expect(lexer, CN_TOKEN_SQR_BRACES_CLOSE)) goto error;
    }

    // Constraint.
    Cn_Ast_String *string = cn_parse_string(lexer);
    if (string == NULL) goto error;
    node.string = string;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    Cn_Ast_Node *expression = cn_parse_expression(lexer, -1, 0);
    if (expression == NULL) goto error;
    node.expression = expression;

    // Typechecking expression right after parsing.
    if (cn_analyze_typecheck_expression(expression) == NULL) goto error;

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Asm_Operand *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_clobbers(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    // Empty clobber list.
    if (cn_lexer_expect(lexer, CN_TOKEN_COLON) || cn_lexer_expect(lexer, CN_TOKEN_PARAN_CLOSE)) {
        *ok = true;
        return cn_ast_stack_finalize(mark);
    }

    Cn_Ast_String *clobber = cn_parse_string(lexer);
    if (clobber == NULL) goto error;
    cn_ast_stack_push(clobber);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        clobber = cn_parse_string(lexer);
        if (clobber == NULL) goto error;
        cn_ast_stack_push(clobber);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Nodes cn_parse_gnu_asm_goto_labels(Cn_Lexer *lexer, bool *ok) {
    Cn_Lexer original_state = *lexer;

    int64_t mark = cn_ast_stack_mark();

    Cn_Ast_Identifier *label = cn_parse_identifier(lexer);
    if (label == NULL) goto error;
    cn_ast_stack_push(label);

    while (cn_lexer_expect(lexer, CN_TOKEN_COMMA)) {
        cn_lexer_next_token(lexer);

        label = cn_parse_identifier(lexer);
        if (label == NULL) goto error;
        cn_ast_stack_push(label);
    }

    *ok = true;
    return cn_ast_stack_finalize(mark);

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    *ok = false;
    return (Cn_Ast_Nodes) {0};
}

CNDEF Cn_Ast_Gnu_Asm_Label *cn_parse_gnu_asm_label(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;

    Cn_Ast_Gnu_Asm_Label node = { .id = CN_AST_GNU_ASM_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

    if (!cn_parse_expect(lexer, CN_TOKEN_GNU_ASM)) goto error;
    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_OPEN)) goto error;

    if (cn_lexer_expect(lexer, CN_TOKEN_STRING)) {
        Cn_Ast_String *string = cn_parse_string(lexer);
        if (string == NULL) goto error;
        node.string = string;
    }

    if (!cn_parse_expect(lexer, CN_TOKEN_PARAN_CLOSE)) goto error;

    Cn_Ast_Gnu_Asm_Label *parent = cn_ast_new(node);
    return parent;

error:
    CN__TRACE_ERROR
    *lexer = original_state;
    return NULL;
}

/**
 * Type specifier analyze hook, resolves `typeof(...)` to the type of its type name or expression operand.
 */
CNDEF Cn_Hook_Response cn__gnu_hook_analyze_type_specifier(Cn_Ast_Node *type_specifier, Cn_Type **type_out) {
    if (type_specifier->id != CN_AST_GNU_TYPEOF) return CN_HOOK_RESPONSE_NONE;

    Cn_Ast_Gnu_Typeof *typeof_spec = cn_ast_as(Gnu_Typeof, type_specifier);
    Cn_Type *result;
 
    if (typeof_spec->target->id == CN_AST_TYPE_NAME) {
        Cn_Ast_Type_Name *tn = cn_ast_as(Type_Name, typeof_spec->target);
        Cn_Ast_Specifier_Qualifier *sq = tn->specifier_qualifier;
        result = cn_analyze_to_type(sq->qualifiers, sq->type_specifier, tn->abstract_declarator, 0);
    } else {
        // Expression operand was already typechecked when it was parsed.
        result = cn_get_expression_type(typeof_spec->target);
    }
 
    if (result == NULL) {
        CN__TRACE_ERROR
        return CN_HOOK_RESPONSE_ERROR;
    }

    *type_out = result;
    return CN_HOOK_RESPONSE_NONE;
}

CNDEF void cn__emit_gnu_typeof(Cn_Emitter *e, Cn_Ast_Gnu_Typeof *node) {
    cn__emit_str_lit(e, "__typeof__(");
    cn_emit(e, node->target);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_gnu_attribute_specifiers(Cn_Emitter *e, Cn_Ast_Nodes specifiers) {
    for (int64_t i = 0; i < specifiers.length; i++) {
        cn__emit_str_lit(e, " ");
        cn_emit(e, specifiers.ptrs[i]);
    }
}

CNDEF void cn__emit_gnu_attribute_specifier(Cn_Emitter *e, Cn_Ast_Gnu_Attribute_Specifier *node) {
    cn__emit_str_lit(e, "__attribute__((");
    for (int64_t i = 0; i < node->gnu_attributes.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ", ");
        cn_emit(e, node->gnu_attributes.ptrs[i]);
    }
    cn__emit_str_lit(e, "))");
}

CNDEF void cn__emit_gnu_attribute(Cn_Emitter *e, Cn_Ast_Gnu_Attribute *node) {
    // Emit attribute identifier.
    cn_emit(e, node->identifier);

    // Emit arguments if present.
    if (node->arguments.length > 0) {
        cn__emit_str_lit(e, "(");
        for (int64_t i = 0; i < node->arguments.length; i++) {
            if (i > 0) cn__emit_str_lit(e, ", ");
            cn_emit(e, node->arguments.ptrs[i]);
        }
        cn__emit_str_lit(e, ")");
    }
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_qualifiers(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Qualifier_Flags qualifiers) {
    if (qualifiers & CN_AST_ASM_QUALIFIER_VOLATILE) cn__emit_str_lit(e, "volatile ");
    if (qualifiers & CN_AST_ASM_QUALIFIER_INLINE)   cn__emit_str_lit(e, "inline ");
    if (qualifiers & CN_AST_ASM_QUALIFIER_GOTO)     cn__emit_str_lit(e, "goto ");
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_section(Cn_Emitter *e, Cn_Ast_Nodes list) {
    cn__emit_str_lit(e, " :");
    for (int64_t i = 0; i < list.length; i++) {
        if (i > 0) cn__emit_str_lit(e, ",");
        cn__emit_str_lit(e, " ");
        cn_emit(e, list.ptrs[i]);
    }
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_statement(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Statement *node) {
    cn__emit_str_lit(e, "__asm__ ");
    cn__emit_gnu_asm_qualifiers(e, node->qualifiers);

    cn__emit_str_lit(e, "(");
    cn_emit(e, node->template);

    // Basic and extended asm differ semantically (e.g. '%' escaping in template), so
    // an extended asm keeps at least one ':' even when all of its sections are empty.
    // Trailing empty sections are omitted, but a section is always emitted if any later one is not empty.
    int sections = 0;
    if (node->goto_labels.length > 0)   sections = 4;
    else if (node->clobbers.length > 0) sections = 3;
    else if (node->inputs.length > 0)   sections = 2;
    else if (node->outputs.length > 0 || (node->flags & CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED)) sections = 1;

    if (sections >= 1) cn__emit_gnu_asm_section(e, node->outputs);
    if (sections >= 2) cn__emit_gnu_asm_section(e, node->inputs);
    if (sections >= 3) cn__emit_gnu_asm_section(e, node->clobbers);
    if (sections >= 4) cn__emit_gnu_asm_section(e, node->goto_labels);

    cn__emit_str_lit(e, ");");
    cn__emit_newline(e);
}

// AI: Generated.
CNDEF void cn__emit_gnu_asm_operand(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Operand *node) {
    if (node->identifier != NULL) {
        cn__emit_str_lit(e, "[");
        cn_emit(e, node->identifier);
        cn__emit_str_lit(e, "] ");
    }

    cn_emit(e, node->string);

    cn__emit_str_lit(e, " (");
    cn_emit(e, node->expression);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_gnu_asm_label(Cn_Emitter *e, Cn_Ast_Gnu_Asm_Label *node) {
    cn__emit_str_lit(e, "__asm__(");
    cn_emit(e, node->string);
    cn__emit_str_lit(e, ")");
}

CNDEF void cn__emit_gnu_extensions(Cn_Emitter *e, Cn_Ast_Gnu_Extensions *node) {
    for (int i = 0; i < node->count; i++) {
        if (i > 0) cn__emit_str_lit(e, " ");
        cn__emit_str_lit(e, "__extension__");
    }
}

/**
 * Pushes `node` as a single node extension of the slot currently being parsed.
 */
CNDEF void cn__gnu_push_node(void *node) {
    cn_ast_ext_stack_push(((Cn_Ast_Extension) { .kind = CN_AST_EXTENSION_NODE, .node = cn_ast_node(node) }));
}

/**
 * Parses GNU attribute specifiers if present, and pushes them as a list extension
 * of the slot currently being parsed.
 */
CNDEF Cn_Hook_Response cn__gnu_parse_attribute_specifiers_extension(Cn_Lexer *lexer) {
    if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) return CN_HOOK_RESPONSE_NONE;

    bool ok;
    Cn_Ast_Nodes specifiers = cn_parse_gnu_attribute_specifiers(lexer, &ok);
    if (!ok) return CN_HOOK_RESPONSE_ERROR;

    cn_ast_ext_stack_push(((Cn_Ast_Extension) { .kind = CN_AST_EXTENSION_LIST, .list = specifiers }));
    return CN_HOOK_RESPONSE_NONE;
}

/**
 * Single subparsing hook of GNU extension, dispatches to the GNU parsing function
 * that is allowed to appear in the grammar `slot` being parsed.
 *
 * Nothing is parsed if the current token doesn't start a GNU construct of that slot.
 */
CNDEF Cn_Hook_Response cn__gnu_hook_parse(Cn_Ast_Slot_Id slot, Cn_Ast_Node **null_ptr, Cn_Lexer *lexer) {
    switch (slot) {
        // '__extension__' keywords, attached to whatever follows them.
        case CN_AST_SLOT_EXTERNAL_DECLARATION_PREFIX:
        case CN_AST_SLOT_DECLARATION_PREFIX:
        case CN_AST_SLOT_MEMBER_DECLARATION_PREFIX:
        case CN_AST_SLOT_EXPRESSION_PREFIX:
            {
                if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_EXTENSION)) return CN_HOOK_RESPONSE_NONE;

                Cn_Ast_Gnu_Extensions *extensions = cn_parse_gnu_extensions(lexer);
                if (extensions == NULL) return CN_HOOK_RESPONSE_ERROR;

                cn__gnu_push_node(extensions);
                return CN_HOOK_RESPONSE_NONE;
            }

        // File-scope asm, only basic asm without qualifiers is allowed, same as GCC.
        case CN_AST_SLOT_EXTERNAL_DECLARATION:
            {
                if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ASM)) return CN_HOOK_RESPONSE_NONE;

                Cn_Ast_Gnu_Asm_Statement *statement = cn_parse_gnu_asm_statement(lexer);
                if (statement == NULL) return CN_HOOK_RESPONSE_ERROR;

                if (statement->qualifiers != 0 || (statement->flags & CN_AST_GNU_ASM_STATEMENT_IS_EXTENDED)) {
                    cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, statement, CN_DC_UNEXPECTED_TOKEN, "Only basic asm without qualifiers is allowed at file scope.");
                    return CN_HOOK_RESPONSE_ERROR;
                }

                *null_ptr = cn_ast_node(statement);
                return CN_HOOK_RESPONSE_NONE;
            }

        // Block scope asm statement.
        case CN_AST_SLOT_STATEMENT:
            {
                if (!cn_lexer_expect(lexer, CN_TOKEN_GNU_ASM)) return CN_HOOK_RESPONSE_NONE;

                Cn_Ast_Gnu_Asm_Statement *statement = cn_parse_gnu_asm_statement(lexer);
                if (statement == NULL) return CN_HOOK_RESPONSE_ERROR;

                *null_ptr = cn_ast_node(statement);
                return CN_HOOK_RESPONSE_NONE;
            }

        // 'typeof' substitutes type specifier, otherwise attributes in specifier qualifier lists.
        case CN_AST_SLOT_TYPE_SPECIFIER:
            {
                if (cn_lexer_expect(lexer, CN_TOKEN_GNU_TYPEOF)) {
                    Cn_Ast_Gnu_Typeof *gnu_typeof = cn_parse_gnu_typeof(lexer);
                    if (gnu_typeof == NULL) return CN_HOOK_RESPONSE_ERROR;

                    *null_ptr = cn_ast_node(gnu_typeof);
                    return CN_HOOK_RESPONSE_NONE;
                }

                return cn__gnu_parse_attribute_specifiers_extension(lexer);
            }

        // Attributes.
        case CN_AST_SLOT_DECLARATION_SPECIFIERS:
        case CN_AST_SLOT_POINTER:
        case CN_AST_SLOT_TAG:
        case CN_AST_SLOT_MEMBER_DECLARATOR:
        case CN_AST_SLOT_ENUMERATOR:
            return cn__gnu_parse_attribute_specifiers_extension(lexer);

        // Asm label and attributes, in any order.
        case CN_AST_SLOT_AFTER_DECLARATOR:
            {
                while (true) {
                    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ASM)) {
                        Cn_Ast_Gnu_Asm_Label *label = cn_parse_gnu_asm_label(lexer);
                        if (label == NULL) return CN_HOOK_RESPONSE_ERROR;

                        cn__gnu_push_node(label);
                        continue;
                    }

                    if (cn_lexer_expect(lexer, CN_TOKEN_GNU_ATTRIBUTE)) {
                        if (cn__gnu_parse_attribute_specifiers_extension(lexer) != CN_HOOK_RESPONSE_NONE) return CN_HOOK_RESPONSE_ERROR;
                        continue;
                    }

                    return CN_HOOK_RESPONSE_NONE;
                }
            }

        default:
            return CN_HOOK_RESPONSE_NONE;
    }
}

/**
 * Alignment used by `aligned` attribute without argument, the biggest alignment of the target.
 */
#define CN_GNU_BIGGEST_ALIGNMENT 16

/**
 * Finds GNU attribute `name` among GNU attribute specifiers attached to `node` at `slot`.
 * Both `name` and `__name__` spellings are matched.
 *
 * RETURNS: Found attribute, `NULL` if there is none.
 */
CNDEF Cn_Ast_Gnu_Attribute *cn__gnu_find_attribute(Cn_Ast_Node *node, Cn_Ast_Slot_Id slot, Cn_String name) {
    for (int64_t i = 0; i < node->extensions.length; i++) {
        Cn_Ast_Extension *ext = &node->extensions.exts[i];
        if (ext->slot != slot || ext->kind != CN_AST_EXTENSION_LIST) continue;

        for (int64_t j = 0; j < ext->list.length; j++) {
            if (ext->list.ptrs[j]->id != CN_AST_GNU_ATTRIBUTE_SPECIFIER) continue;
            Cn_Ast_Nodes attributes = cn_ast_as(Gnu_Attribute_Specifier, ext->list.ptrs[j])->gnu_attributes;

            for (int64_t k = 0; k < attributes.length; k++) {
                Cn_Ast_Gnu_Attribute *attribute = cn_ast_as(Gnu_Attribute, attributes.ptrs[k]);
                Cn_String attribute_name = attribute->identifier->name;

                // Stripping `__` from both sides, for `__name__` spelling.
                if (attribute_name.length > 4 && cn_str_starts_with(attribute_name, CN_STR_LIT("__")) && cn_str_ends_with(attribute_name, CN_STR_LIT("__"))) {
                    attribute_name = cn_str_substring(attribute_name, 2, attribute_name.length - 2);
                }

                if (cn_str_is(attribute_name, name)) return attribute;
            }
        }
    }

    return NULL;
}

/**
 * Evaluates alignment requested by `aligned` attribute, `aligned` without argument requests
 * `CN_GNU_BIGGEST_ALIGNMENT`.
 *
 * RETURNS: Requested alignment, `-1` on error.
 */
CNDEF int64_t cn__gnu_attribute_alignment(Cn_Ast_Gnu_Attribute *attribute) {
    if (attribute->arguments.length == 0) return CN_GNU_BIGGEST_ALIGNMENT;

    if (attribute->arguments.length != 1) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, attribute, CN_DC_ILLEGAL_ATTRIBUTE, "Attribute 'aligned' takes at most one argument.");
        return -1;
    }

    Cn_Ast_Node *expression = attribute->arguments.ptrs[0];

    // Attribute arguments are not typechecked when parsed, since their meaning depends on the attribute.
    if (cn_analyze_typecheck_expression(expression) == NULL) return -1;

    uint8_t buffer[CN_TYPE_SCALAR_MAX_SIZE];
    Cn_Any value = cn_analyze_evaluate_expression(expression, buffer);
    if (cn_any_is_empty(value) || cn_type_unqualified(value.type)->kind != CN_INTEGER) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Argument of attribute 'aligned' must be an integer constant expression.");
        return -1;
    }

    int64_t alignment = cn_any_read_int(value);
    if (alignment <= 0 || (alignment & (alignment - 1)) != 0) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, expression, CN_DC_INVALID_CONSTANT_EXPRESSION, "Argument of attribute 'aligned' must be a power of 2, got %lld.", (long long)alignment);
        return -1;
    }

    return alignment;
}

/**
 * Layout analyze hook, applies `packed` and `aligned` attributes of the tag specifier and of
 * struct or union members. Leaves type to default layout, if none of them are present.
 */
CNDEF Cn_Hook_Response cn__gnu_hook_analyze_layout(Cn_Ast_Node *tag_specifier, Cn_Type *type) {
    Cn_Ast_Gnu_Attribute *packed  = cn__gnu_find_attribute(tag_specifier, CN_AST_SLOT_TAG, CN_STR_LIT("packed"));
    Cn_Ast_Gnu_Attribute *aligned = cn__gnu_find_attribute(tag_specifier, CN_AST_SLOT_TAG, CN_STR_LIT("aligned"));

    Cn_Layout_Opt opt = {0};

    if (aligned != NULL) {
        opt.min_align = cn__gnu_attribute_alignment(aligned);
        if (opt.min_align < 0) return CN_HOOK_RESPONSE_ERROR;
    }

    if (type->kind == CN_ENUM) {
        if (packed == NULL && aligned == NULL) return CN_HOOK_RESPONSE_NONE;

        cn_analyze_layout_default(type, opt);

        // Packed enum shrinks to the smallest integer that holds every enumerator value.
        if (packed != NULL) {
            int64_t size = 1;
            for (int64_t i = 0; i < type->enum_t.members_length; i++) {
                int64_t v = type->enum_t.members[i].value;
                while (size < 8 && (v < -(INT64_C(1) << (size * 8 - 1)) || v >= (INT64_C(1) << (size * 8)))) size *= 2;
            }

            type->size = size;
            type->align = size > opt.min_align ? size : opt.min_align;
        }

        return CN_HOOK_RESPONSE_NONE;
    }

    // Struct and union, both specifiers share the same layout.
    Cn_Ast_Struct_Specifier *specifier = cn_ast_as(Struct_Specifier, tag_specifier);
    int64_t members_length = type->kind == CN_STRUCT ? type->struct_t.members_length : type->union_t.members_length;

    int64_t *member_aligns = CN_REALLOC(NULL, sizeof(int64_t) * (members_length > 0 ? members_length : 1));
    bool any_member_attribute = false;

    // Member `i` comes from `i`-th member declarator, in declaration order.
    int64_t i = 0;
    for (int64_t m = 0; m < specifier->member_declarations.length; m++) {
        Cn_Ast_Member_Declaration *md = cn_ast_as(Member_Declaration, specifier->member_declarations.ptrs[m]);

        for (int64_t d = 0; d < md->member_declarators.length; d++, i++) {
            Cn_Ast_Node *declarator = md->member_declarators.ptrs[d];
            Cn_Type *member_type = type->kind == CN_STRUCT ? type->struct_t.members[i].type : type->union_t.members[i].type;

            Cn_Ast_Gnu_Attribute *member_packed  = cn__gnu_find_attribute(declarator, CN_AST_SLOT_MEMBER_DECLARATOR, CN_STR_LIT("packed"));
            Cn_Ast_Gnu_Attribute *member_aligned = cn__gnu_find_attribute(declarator, CN_AST_SLOT_MEMBER_DECLARATOR, CN_STR_LIT("aligned"));

            // Packed member, or member of packed type, drops to alignment of 1.
            // Aligned member raises its alignment, but never lowers it unless packed.
            int64_t member_align = (packed != NULL || member_packed != NULL) ? 1 : member_type->align;

            if (member_aligned != NULL) {
                int64_t requested = cn__gnu_attribute_alignment(member_aligned);
                if (requested < 0) {
                    CN_FREE(member_aligns);
                    return CN_HOOK_RESPONSE_ERROR;
                }
                if (requested > member_align) member_align = requested;
            }

            if (member_packed != NULL || member_aligned != NULL) any_member_attribute = true;
            member_aligns[i] = member_align;
        }
    }

    if (packed == NULL && aligned == NULL && !any_member_attribute) {
        CN_FREE(member_aligns);
        return CN_HOOK_RESPONSE_NONE;
    }

    opt.member_aligns = member_aligns;
    cn_analyze_layout_default(type, opt);

    CN_FREE(member_aligns);
    return CN_HOOK_RESPONSE_NONE;
}

CN_AST_INFO_DEFINE_ARRAY(GNU, CN_GNU_EXTENSION_ID, CN_AST_GNU_GEN_LIST);

CNDEF bool cn_gnu_extension(Cn_Hooks *hooks, Cn_Ast_Info_Array ast_info_table[256]) {

    // Registring gnu ast meta info.
    CN_AST_INFO_TABLE_REGISTER(ast_info_table, CN_GNU_EXTENSION_ID, GNU);

    // Registring single parsing hook, it dispatches by slot itself.
    Cn_Hook_Parse *parse_hook = &cn__gnu_hook_parse;
    cn_array_list_append(&hooks->parse_hooks, parse_hook);

    // Registring analyze hooks.
    Cn_Hook_Analyze_Type_Specifier *type_specifier_hook = &cn__gnu_hook_analyze_type_specifier;
    cn_array_list_append(&hooks->analyze_type_specifier_hooks, type_specifier_hook);

    Cn_Hook_Analyze_Layout *layout_hook = &cn__gnu_hook_analyze_layout;
    cn_array_list_append(&hooks->analyze_layout_hooks, layout_hook);

    return true;
}

#endif // CN_GNU_IMPLEMENTATION_GUARD_
#endif // CN_IMPLEMENTATION


/* 
    Revision history:
*/

/*
    Version conventions:

        Following MAJOR.MINOR.PATCH format described in https://semver.org/ - Semantic Versioning:
            
      - Comments modification don't update the version.
      - Major version is incremented when any backward incompatible changes are made to the public API.
      - Minor version is incremented when backward compatible changes are made to the public API.
      - Patch version is incremented if only backward compatible bug fixes are made without changing public API.
      - Major version '0.y.z' is for intial development. Anything may change at any time. 
        Public API is not stable.
*/

/*
    MIT License
    
    Copyright (c) 2026 Daniil Yarmalovich
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
