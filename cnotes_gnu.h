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

#include "cnotes.h"

#endif // CN_GNU_H_


