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

    # API Conventions

    This file follows the same convetions as cnotes, as it extends its functionality.
    However this extension development is separate from `cnotes.h` meaning versioning
    and changelog written here is exclusively for this file.
*/


#ifndef CN_GNU_H_
#define CN_GNU_H_

#define CN_GNU_EXTENSION_ID 1

#include "cnotes.h"


typedef enum : uint8_t {
    CN_AST_ASM_QUALIFIER_VOLATILE   = 0x01,
    CN_AST_ASM_QUALIFIER_INLINE     = 0x02,
    CN_AST_ASM_QUALIFIER_GOTO       = 0x04,
} Cn_Ast_Gnu_Asm_Qualifier_Flags;

#define CN_AST_GNU_GEN_LIST(X)\
    X(GNU_TYPEOF,                       Gnu_Typeof,                         gnu_typeof)                     \
    X(GNU_ATTRIBUTE_SPECIFIER,          Gnu_Attribute_Specifier,            gnu_attribute_specifier)        \
    X(GNU_ATTRIBUTE,                    Gnu_Attribute,                      gnu_attribute)                  \
    X(GNU_ASM_STATEMENT,                Gnu_Asm_Statement,                  gnu_asm_statement)              \
    X(GNU_ASM_OPERAND,                  Gnu_Asm_Operand,                    gnu_asm_operand)                \
    X(GNU_ASM_LABEL,                    Gnu_Asm_Label,                      gnu_asm_label)

typedef enum : uint32_t {
    CN_AST_GNU_UNKNOWN = 0x01000000,
#define X(K, T, m) CN_AST_##K,
    CN_AST_GNU_GEN_LIST(X)
#undef X
} Cn_Ast_Gnu_Kind;

#define X(K, T, m) typedef struct cn_ast_##m Cn_Ast_##T;
    CN_AST_GNU_GEN_LIST(X)
#undef X

struct cn_ast_gnu_typeof { CN_AST_BASE;
    Cn_Ast_Node *target; // Could be expression or type name.
};

struct cn_ast_gnu_attribute_specifier { CN_AST_BASE;
    Cn_Ast_Nodes gnu_attributes; // List of Cn_Ast_Gnu_Attribute.
};

struct cn_ast_gnu_attribute { CN_AST_BASE;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_Nodes       arguments;
};

struct cn_ast_gnu_asm_statement { CN_AST_BASE;
    Cn_Ast_Gnu_Asm_Qualifier_Flags qualifiers;
    Cn_Ast_String *template;
    Cn_Ast_Nodes   outputs;
    Cn_Ast_Nodes   inputs;
    Cn_Ast_Nodes   clobbers;
    Cn_Ast_Nodes   goto_labels;
};

struct cn_ast_gnu_asm_operand { CN_AST_BASE;
    Cn_Ast_Identifier *identifier;
    Cn_Ast_String     *string;
    Cn_Ast_Node       *expression;
};

struct cn_ast_gnu_asm_label { CN_AST_BASE;
    Cn_Ast_String *string;
};

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
 *          : '__extension__'*
 *          ;
 * ```
 *
 * RETURNS: `true` if at least one keyword was parsed, `false` otherwise.
 *
 * NOTE: Keyword only disables pedantic diagnostics in GCC, so repeated keywords are collapsed
 * into a single `CN_AST_GNU_EXTENSION` flag, set by the caller on the node that follows.
 *
 * NOTE: Allowed only at the start of an external declaration, block item declaration, `for`
 * initialization declaration, member declaration, and as a unary operator in expressions.
 * Never after attribute specifiers, same as GCC.
 *
 * AI: Generated.
 */
CNDEF bool cn_parse_gnu_extensions(Cn_Lexer *lexer);

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

#endif // CN_GNU_H_





#ifdef CN_IMPLEMENTATION

CNDEF Cn_Ast_Gnu_Typeof *cn_parse_gnu_typeof(Cn_Lexer *lexer) {
    Cn_Lexer original_state = *lexer;
    Cn_Ast_Gnu_Typeof node = { .kind = CN_AST_GNU_TYPEOF, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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

CNDEF bool cn_parse_gnu_extensions(Cn_Lexer *lexer) {
    bool parsed = false;
    while (cn_parse_optional(lexer, CN_TOKEN_GNU_EXTENSION)) parsed = true;
    return parsed;
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

    Cn_Ast_Gnu_Attribute_Specifier node = { .kind = CN_AST_GNU_ATTRIBUTE_SPECIFIER, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
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

    Cn_Ast_Gnu_Attribute node = { .kind = CN_AST_GNU_ATTRIBUTE, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };
    
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

    Cn_Ast_Gnu_Asm_Statement node = { .kind = CN_AST_GNU_ASM_STATEMENT, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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

    Cn_Ast_Gnu_Asm_Operand node = { .kind = CN_AST_GNU_ASM_OPERAND, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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

    Cn_Ast_Gnu_Asm_Label node = { .kind = CN_AST_GNU_ASM_LABEL, .loc = cn_lexer_token(lexer).loc, .src = cn_lexer_token(lexer).src };

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
 * Resolves typeof(...) to the type of its typename or expression operand.
 *
 * RETURNS: The resolved type, or NULL on error.
 */
CNDEF Cn_Type *cn__analyze_type_from_gnu_typeof(Cn_Ast_Node *ts) {
    Cn_Ast_Gnu_Typeof *typeof_spec = cn_ast_as(Gnu_Typeof, ts);
    Cn_Ast_Node *typeof_child = typeof_spec->target;
    Cn_Type *result;
 
    if (typeof_child->kind == CN_AST_TYPE_NAME) {
        Cn_Ast_Type_Name *tn = cn_ast_as(Type_Name, typeof_spec->target);
        Cn_Ast_Specifier_Qualifier *sq = tn->specifier_qualifier;
        result = cn_analyze_to_type(sq->qualifiers, sq->type_specifier, tn->abstract_declarator, 0);
    } else {
        result = cn_get_expression_type(typeof_spec->target);
    }
 
    if (result == NULL) goto error;
    return result;
 
error:
    CN__TRACE_ERROR
    return NULL;
}

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
