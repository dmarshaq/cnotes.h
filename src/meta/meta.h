#ifndef META_H
#define META_H

#include "core/str.h"
#include "meta/lexer.h"

#include <stdio.h>

typedef struct meta_state {
    Lexer lexer;
    char *current_file_name;
    String current_file_source;
} Meta_State;

/**
 * Meta processor is struct desgined to inject more functionality into existing processor.
 * It allows to specifiy pointers to the functions that will be called automatically.
 * As well as the meta notes themselves, that will trigger the functions once found by the processor.
 * Order is used to indicate when the meta processor is executed.
 * For example: If order is 1, the meta processor will only be executed after all meta processors with order 0 are executed.
 */
typedef struct meta_processor {
    int (*init)(Meta_State *);
    int (*process)(Lexer *);
    int (*output)(FILE *);
    int notes_length;
    String *notes;
} Meta_Processor;



#define META_LOG_ERROR(format, ...)       meta_log_error(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define META_LOG_INFO(format, ...)       meta_log_info(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)


/**
 * Will print error message and place in current file where error occured.
 * Example:
 *
 *  [ERRO] Expected TOKEN ... but got ... .
 *      
 *  46 | typedef struct {
 *     |         ^~~~~~
 */
void meta_log_error(const char *file_name, s64 line, const char *function_name, char *format, ...);

/**
 * Will print info message and line number + current file where it occured.
 * Example:
 *
 *  [INFO] Found TOKEN ... .
 */
void meta_log_info(const char *file_name, s64 line, const char *function_name, char *format, ...);



void meta_replace_with_space(String metanote_str);



#define LOG

int meta_process(int count, char **files, char *output_path);

#endif
