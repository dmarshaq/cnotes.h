#define NOB_IMPLEMENTATION
#include "nob.h"

#define CN_IMPLEMENTATION
#include "cnotes.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "cnotes.h");

    CN_String content = CN_CSTR(
            "#include <stdio.h>\n"
            "\n"
            "int main(void) {\n"
            "   printf(\"Hello World!\\n\");\n"
            "}\n"
            );
    printf("Code:\n%.*s", CN_UNPACK(content));

    CN_Lexer lexer;
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

