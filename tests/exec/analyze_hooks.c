// Test: Analyze hooks of custom extension, order of producer hooks, core fallback and error propagation.
#include "../../cnotes.h"

#define TEST_EXTENSION_ID 2

// Custom ast nodes: `my_int` type specifier and `my_val` / `my_error` primary expressions.
#define CN_AST_TEST_GEN_LIST(X)                                                         \
    X(TEST_TYPE,                        Test_Type,                          test_type)  \
    X(TEST_VALUE,                       Test_Value,                         test_value)

#define CN_AST_TEST_TYPE_MEMBERS(F, X)              \
    F(X, INT32,  int,           size)               \

#define CN_AST_TEST_VALUE_MEMBERS(F, X)             \
    F(X, TYPE,   Cn_Type *,     type)               \
    F(X, UINT64, uint64_t,      value)              \

CN_AST_DECLARE_KINDS(TEST, Test, TEST_EXTENSION_ID, CN_AST_TEST_GEN_LIST)
CN_AST_DECLARE_STRUCTS(CN_AST_TEST_GEN_LIST)
CN_AST_DEFINE_STRUCTS(CN_AST_TEST_GEN_LIST)

void cn__emit_test_type(Cn_Emitter *e, Cn_Ast_Test_Type *node) {
    CN_UNUSED(node);
    e->write(CN_STR_LIT("my_int"), e->ctx);
}

void cn__emit_test_value(Cn_Emitter *e, Cn_Ast_Test_Value *node) {
    CN_UNUSED(node);
    e->write(CN_STR_LIT("my_val"), e->ctx);
}

CN_AST_INFO_DEFINE_ARRAY(TEST, TEST_EXTENSION_ID, CN_AST_TEST_GEN_LIST);

bool is_identifier(Cn_Lexer *lexer, Cn_String name) {
    Cn_Token token = cn_lexer_token(lexer);
    return token.type == CN_TOKEN_IDENTIFIER && cn_str_is(cn_source_to_str(&token.src), name);
}

Cn_Hook_Response parse_hook(Cn_Ast_Slot_Id slot, Cn_Ast_Node **null_ptr, Cn_Lexer *lexer) {
    Cn_Token token = cn_lexer_token(lexer);

    if (slot == CN_AST_SLOT_TYPE_SPECIFIER && is_identifier(lexer, CN_STR_LIT("my_int"))) {
        cn_lexer_next_token(lexer);
        *null_ptr = cn_ast_new((Cn_Ast_Test_Type) { .id = CN_AST_TEST_TYPE, .loc = token.loc, .src = token.src, .size = 2 });
        return CN_HOOK_RESPONSE_NONE;
    }

    if (slot == CN_AST_SLOT_PRIMARY && (is_identifier(lexer, CN_STR_LIT("my_val")) || is_identifier(lexer, CN_STR_LIT("my_error")))) {
        uint64_t value = is_identifier(lexer, CN_STR_LIT("my_val")) ? 7 : 0;
        cn_lexer_next_token(lexer);
        *null_ptr = cn_ast_new((Cn_Ast_Test_Value) { .id = CN_AST_TEST_VALUE, .loc = token.loc, .src = token.src, .value = value });
        return CN_HOOK_RESPONSE_NONE;
    }

    return CN_HOOK_RESPONSE_NONE;
}

int first_type_specifier_calls = 0;
int second_type_specifier_calls = 0;
int layout_calls = 0;
int expression_calls = 0;
int evaluate_calls = 0;

// First type specifier hook produces 2 byte integer for `my_int`.
Cn_Hook_Response first_type_specifier_hook(Cn_Ast_Node *type_specifier, Cn_Type **type_out) {
    first_type_specifier_calls++;
    if (type_specifier->id != CN_AST_TEST_TYPE) return CN_HOOK_RESPONSE_NONE;

    int size = cn_ast_as(Test_Type, type_specifier)->size;
    Cn_Type type = { .integer = { .kind = CN_INTEGER, .flags = CN_TYPE_COMPLETE, .size = size, .align = size, .is_signed = true } };
    *type_out = cn_type_intern(&type);
    return CN_HOOK_RESPONSE_NONE;
}

// Second type specifier hook must never be reached, since the first one always produces output.
Cn_Hook_Response second_type_specifier_hook(Cn_Ast_Node *type_specifier, Cn_Type **type_out) {
    CN_UNUSED(type_specifier);
    second_type_specifier_calls++;
    *type_out = cn_type_intern((Cn_Type *)&CN_TYPE_DOUBLE);
    return CN_HOOK_RESPONSE_NONE;
}

// Layout hook aligns only `struct Big` to 32, every other tag is left to core default layout.
Cn_Hook_Response layout_hook(Cn_Ast_Node *tag_specifier, Cn_Type *type) {
    layout_calls++;
    if (type->kind != CN_STRUCT) return CN_HOOK_RESPONSE_NONE;
    if (!cn_str_is(cn_ast_as(Struct_Specifier, tag_specifier)->identifier->name, CN_STR_LIT("Big"))) return CN_HOOK_RESPONSE_NONE;

    cn_analyze_layout_default(type, (Cn_Layout_Opt) { .min_align = 32 });
    return CN_HOOK_RESPONSE_NONE;
}

Cn_Hook_Response expression_hook(Cn_Ast_Node *expression, Cn_Type **type_out) {
    expression_calls++;
    if (expression->id != CN_AST_TEST_VALUE) return CN_HOOK_RESPONSE_NONE;

    // `my_error` reports diagnostic and aborts analysis.
    if (cn_ast_as(Test_Value, expression)->value == 0) {
        cn_diagnostic_node(CN_DIAGNOSTIC_ERROR, expression, CN_DC_ILLEGAL_TYPE, "my_error can't be typechecked.");
        return CN_HOOK_RESPONSE_ERROR;
    }

    *type_out = cn_type_intern((Cn_Type *)&CN_TYPE_INT);
    return CN_HOOK_RESPONSE_NONE;
}

Cn_Hook_Response evaluate_hook(Cn_Ast_Node *expression, void *buffer, Cn_Any *out) {
    evaluate_calls++;
    if (expression->id != CN_AST_TEST_VALUE) return CN_HOOK_RESPONSE_NONE;

    *out = cn_any_write_int((Cn_Any) { .type = cn_get_expression_type(expression), .data = buffer }, cn_ast_as(Test_Value, expression)->value);
    return CN_HOOK_RESPONSE_NONE;
}

bool test_extension(Cn_Hooks *hooks, Cn_Ast_Info_Array ast_info_table[256]) {
    CN_AST_INFO_TABLE_REGISTER(ast_info_table, TEST_EXTENSION_ID, TEST);

    Cn_Hook_Parse *parse = &parse_hook;
    cn_array_list_append(&hooks->parse_hooks, parse);

    Cn_Hook_Analyze_Type_Specifier *first = &first_type_specifier_hook, *second = &second_type_specifier_hook;
    cn_array_list_append(&hooks->analyze_type_specifier_hooks, first);
    cn_array_list_append(&hooks->analyze_type_specifier_hooks, second);

    Cn_Hook_Analyze_Layout *layout = &layout_hook;
    cn_array_list_append(&hooks->analyze_layout_hooks, layout);

    Cn_Hook_Analyze_Expression *expression = &expression_hook;
    cn_array_list_append(&hooks->analyze_expression_hooks, expression);

    Cn_Hook_Analyze_Evaluate *evaluate = &evaluate_hook;
    cn_array_list_append(&hooks->analyze_evaluate_hooks, evaluate);

    return true;
}

// Every check below is an array of length 0 if it fails, which is an analysis error.
const Cn_String src = CN_STR_BUFFER(
    "my_int small;\n"
    "int check_type[sizeof(small) == 2 ? 1 : 0];\n"
    "struct Big { char c; };\n"
    "struct Small { int a; char b; };\n"
    "enum Tag { TAG_A };\n"
    "int check_big[sizeof(struct Big) == 32 ? 1 : 0];\n"
    "int check_small[sizeof(struct Small) == 8 ? 1 : 0];\n"
    "int value = my_val + 1;\n"
    "int check_value[my_val == 7 ? 1 : 0];\n"
    "struct Member { my_int m; char c; };\n"
    "int check_member[sizeof(struct Member) == 4 ? 1 : 0];\n"
);

const Cn_String error_src = CN_STR_BUFFER(
    "int bad = my_error;\n"
);

int main(void) {
    cn_extension_register(test_extension);

    Cn_Translation_Unit tu = {0};
    CN_ASSERT(cn_tu_init(&tu, NULL, .source = src, .flags = CN_TU_NO_CODE_OUTPUT));
    CN_ASSERT(cn_tu_process(&tu) == 0);
    cn_tu_free(&tu);

    // `my_int` used twice, first hook produced output both times, second hook was never called.
    CN_ASSERT(first_type_specifier_calls == 2);
    CN_ASSERT(second_type_specifier_calls == 0);

    // Called for every completed tag, including the ones left to default layout.
    CN_ASSERT(layout_calls == 4);

    // Hooks are called only for custom expressions, `my_val` appears twice.
    CN_ASSERT(expression_calls == 2);
    CN_ASSERT(evaluate_calls >= 1);

    // Hook error aborts processing.
    cn_diagnostic_handler = &cn_null_diagnostic_handler;
    CN_ASSERT(cn_tu_init(&tu, NULL, .source = error_src, .flags = CN_TU_NO_CODE_OUTPUT));
    CN_ASSERT(cn_tu_process(&tu) != 0);
    CN_ASSERT(tu.data.error_count == 1);
    cn_tu_free(&tu);

    return 0;
}
