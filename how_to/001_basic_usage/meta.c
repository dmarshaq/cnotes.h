// This how_to goes over a very basic meta program that you can write with cnotes.h
//
// This file is your meta program fully written in C. It includes cnotes.h and it is built separately from your main program.
// This meta program is designed to receive your C files that are stripped from any macros. They will be refered to a lot as `.i` files
// which stands for intermediate files.
//
// As stated before, the meta program acts separately, this is achievable through build system of your project.
// For example this how_to uses Makefile to build everything in one command. And if you look at its output or inside it, you will see: 
//  1) First it pre-processes `src/*.c` files into `build/*.i` files using `gcc -E` command.
//  2) Then it compiles `meta.c` file (the file we are currently in) into `meta` executable. 
//  3) After that executes `meta` with `build/*.i` files as its arguments.
//  4) Finishes build by compiling all `build/*.i` files into `build/*.o` and then into a final executable `main`.
//
// So meta program gets to run in step 3, any modifications made are then sent to step 4 where final compilation occurs.
//
// Starting things in this file we need to include `cnotes.h` library, since it is stb-style library we need to define its implementation somewhere.
// This is done by defining CN_IMPLEMENTATION macro. So that when `#include "cnotes.h"` expands it will paste implementation right after the header portion of the library.
// That mechanism gives you power to decide where you want library's implementation to go.
#define CN_IMPLEMENTATION
#include "cnotes.h"

// In here we forward declare function of type Cn_Message_Handler, it is a special function that will intercept various libraries messages.
// For example if something got parsed or certain library state was reached. It will be called as a callback by a library.
Cn_Message_Response msg_handler(Cn_Message *message);

// This is entry point for the meta program. In here we do a simple loop where we process each `.i` one by one.
int main(int argc, char **argv) {
    while (true) {
        // Getting next argument. On the first iteration it skips first argument, 
        // because it is be program's name.
        argc--;
        argv++;
        if (argc == 0) break; // Break if no arguments left.

        // Making translation unit.
        // It is a struct used to contain state and settings for each of the `.i` files parsed.
        // We initialize it simply by passing path to one of the `.i` files.
        // By defualt `.i` file will be overidden with the generated C code at the end of processing.
        Cn_Translation_Unit tu = {0};
        if (!cn_tu_init(&tu, *argv)) return 1;

        // Setting message handler.
        // This is where we specify that library should send all messages to our msg_hanlder that we forward declared above.
        // By default cn_message_handler is NULL, and if it remains NULL no messages are sent at all.
        cn_message_handler = msg_handler;

        // Processing translation unit.
        // In this tiny call cn_tu_process all of the complex parsing, analysis, messages occur.
        // It will return non-zero value if error occured. 
        // If this happens the number of diagnostic errors can access in `tu.data.error_count`.
        if (cn_tu_process(&tu) != 0) {
            fprintf(stderr, "Processing failed with %ld error(s)\n", tu.data.error_count);
            cn_tu_free(&tu);
            return 1;
        }

        // Freeing everything.
        // This operation completely wipes out all allocations used by translation unit and library during parsing.
        cn_tu_free(&tu);
    }

    return 0;
}

// In this function we will do a simple introspection operation.
// We will get message that contains information about every function from `src/main.c` and then log function's name.
Cn_Message_Response msg_handler(Cn_Message *message) {
    // First thing is to filter messages, so we only get messages for parsed functions that were not modified before.
    if (message->kind != CN_MESSAGE_AST_PARSED)             return CN_MESSAGE_RESPONSE_NONE;
    if (!(message->flags & CN_MESSAGE_AST_FUNCTION))        return CN_MESSAGE_RESPONSE_NONE;
    if (!(message->flags & CN_MESSAGE_AST_UNMODIFIED))      return CN_MESSAGE_RESPONSE_NONE;
    
    // Getting function ast (abstract syntax tree) node.
    // A little bit verbouse, but basically first we cast the message to Cn_Message_Ast_Parsed, according to its kind.
    // Then we get it's field `node_ptr` of type `Cn_Ast_Node **`, which is a pointer to an actual variable that references node.
    //
    // NOTE: The reason why message passes `Cn_Ast_Node **` and not `Cn_Ast_Node *`, 
    // is because in some cases you can set underlying variable to point to NULL, 
    // effectivly deleting that specific ast branch completely.
    //
    // Or which more likerly, you can set it to point to a different `Cn_Ast_Node`, 
    // for example to substitude given node completely for a different function.
    //
    // Either way, to simply get a workable function node we just can dereference that `Cn_Ast_Node **` and cast it to `Cn_Ast_Function *`, 
    // in here casting is done through simple `cn_ast_as(T, p)` macro.
    Cn_Ast_Function *function = cn_ast_as(Function, *((Cn_Message_Ast_Parsed *)message)->node_ptr);

    // To know where the node is coming from we can use its Cn_Location struct.
    // In here we will only proceed with the function if it comes from `src/hello.c`.
    if (cn_str_is(function->loc.file, CN_STR_LIT("src/hello.c"))) {
        
        // Now lets introspect function's name.
        // To do so we will use cn_get_declarator_info(...), it is a general function for getting info about declarators.
        // Since it is a function it will return identifier at the end which represents function name.
        Cn_Ast_Identifier *identifer;
        cn_get_declarator_info(function->declarator, (Cn_Ast_Node **) &identifer);

        // Lets log the identifer and then since we didn't do any modifications, we can exist with CN_MESSAGE_RESPONSE_NONE.
        cn_log(CN_INFO, "Introspected '%.*s' function, from 'src/hello.c'.", CN_STR_UNPACK(identifer->name));
    }
    
    // If you want to, uncomment following code to see logs of all functions (function definitions) from all `.i` files.
    /*
    Cn_Ast_Identifier *identifer;
    cn_get_declarator_info(function->declarator, (Cn_Ast_Node **) &identifer);
    cn_log(CN_INFO, "Function '%.*s', from '%.*s'.", CN_STR_UNPACK(identifer->name), CN_STR_UNPACK(function->loc.file));
    */

    return CN_MESSAGE_RESPONSE_NONE;
}
