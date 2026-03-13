#ifndef BASIC_H
#define BASIC_H


#include "core/str.h"
#include "meta/lexer.h"
#include "meta/meta.h"

#include <stdio.h>

int basic_init(Meta_State *state);

int basic_process(Lexer *lexer);

int basic_output(FILE *output);

static String BASIC_METANOTES[] = {
    STR_BUFFER("@Typeof"),
};




#endif
