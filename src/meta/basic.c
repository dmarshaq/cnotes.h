#include "meta/basic.h"

#include <stdio.h>

#include "core/type.h"
#include "core/typeinfo.h"
#include "core/str.h"
#include "core/arena.h"
#include "core/file.h"
#include "core/log.h"
#include "core/structs.h"

#include "meta/lexer.h"
#include "meta/introspect.h"
#include "meta/meta.h"


int basic_init(Meta_State *state) {
    return 0;
}

int basic_process(Lexer *lexer){
    if (str_equals(lexer->token.str, BASIC_METANOTES[0])) {
        String meta_note_str = lexer->token.str;


        lexer_next_token(lexer);

        if (!lexer_expect(lexer, TOKEN_PARAN_OPEN)) {
            return -1;
        }
        lexer_next_token(lexer);

        s64 idx = introspect_type_info_get_parse(lexer);

        if (!lexer_expect(lexer, TOKEN_PARAN_CLOSE)) {
            return -1;
        }
        lexer_next_token(lexer);

        meta_note_str.length = lexer->token.str.data - meta_note_str.data;

        if (idx == -1) {
            meta_replace_with_space(meta_note_str);
            return -1;
        }
    
        // Replace with reference to type table.
        String reference = str_format(meta_note_str, "MTT(%lld)", idx);
        meta_note_str = str_eat_chars(meta_note_str, reference.length);
        meta_replace_with_space(meta_note_str);
    }

    return 0;
}

int basic_output(FILE *output) {
    return 0;
}


