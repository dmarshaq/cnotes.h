#ifndef INTROSPECT_H
#define INTROSPECT_H


#include "core/str.h"
#include "meta/lexer.h"
#include "meta/meta.h"

#include <stdio.h>

int introspect_init(Meta_State *state);

int introspect_process(Lexer *lexer);

int introspect_output(FILE *output);

static String INTROSPECT_METANOTES[] = {
    STR_BUFFER("@Introspect"),
};

/**
 * Takes in Lexer and expects abstract type declaration. Qualifier + Type Specifier + Declarator.
 * For example:
 *
 *  const int *[50]
 *
 * Function fully parses string into ast and then tries to find type info of that type, returns index of such type info.
 */
s64 introspect_type_info_get_parse(Lexer *lexer);



#endif
