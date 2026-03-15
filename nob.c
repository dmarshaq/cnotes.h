#define NOB_IMPLEMENTATION
#include "nob.h"

#define CN_IMPLEMENTATION
#include "cnotes.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "cnotes.h");

    float *list = cn_array_list_make(float, 1);
    
    float data[] = { 1.3f, -24.4f, 0.01f, -3.0f, -7.91f, 3.14f, 0.0f, 0.0f };

    cn_array_list_append_multiple(&list, data, ARRAY_LEN(data));

    printf("[ ");
    for (int i = 0; i < cn_array_list_length(&list) - 1; i++) {
        printf("%2.2f, ", list[i]);
    }
    if (cn_array_list_length(&list) > 0) printf("%2.2f", list[cn_array_list_length(&list) - 1]);
    printf(" ]\n\n");
     

    Cn_String content = CN_CSTR(
            "#include <stdio.h>\n"
            "\n"
            "int main(void) {\n"
            "   printf(\"Hello World!\\n\");\n"
            "}\n"
            );
    cn_log(CN_INFO, "Code:\n%.*s", CN_UNPACK(content));


    Cn_Lexer lexer;
    cn_lexer_init(&lexer, content);

    while (true) {
        cn_lexer_next_token(&lexer);

        if (cn_lexer_expect(&lexer, CN_TOKEN_ZERO)) {
            break;
        }
    }

    Nob_Cmd cmd = {0};
    
    return 0;
}

