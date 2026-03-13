#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "core/type.h"

#define ANSI_BLACK      "\x1b[30m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_YELLOW     "\x1b[33m"
#define ANSI_BLUE       "\x1b[34m"
#define ANSI_MAGENTA    "\x1b[35m"
#define ANSI_CYAN       "\x1b[36m"
#define ANSI_WHITE      "\x1b[37m"

#define ANSI_RESET      "\x1b[0m"

typedef enum log_level : u8 {
    LOG_LEVEL_INFO    = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_ERROR   = 2,
} Log_Level;

/**
 * This list of defines dictates what information included in the logging of the program.
 * It is not runtime flags, because logs should run fast.
 * If you don't want some information displayed, just comment out define macros.
 */
#define LOG_INFO_COLOR
#define LOG_INFO_TIME
#define LOG_INFO_THREAD
#define LOG_INFO_FILE
#define LOG_INFO_LINE
#define LOG_INFO_FUNC


#define LOG_INFO(format, ...)       log_print(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)    log_print(LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)      log_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)


/**
 * Sets minimum log level, if log level is below minimum it is not displayed.
 */
void log_set_minimum_level(Log_Level level);

/**
 * Sets log output. If output is not set, all logs default to stderr.
 */
void log_set_output(FILE *stream);

/**
 * Prints and formats log.
 */
void log_print(Log_Level level, const char *file_name, s64 line, const char *function_name, char *format, ...);

/**
 * Prints and formats log, except it takes va_list.
 */
void log_vprint(Log_Level level, const char *file_name, s64 line, const char *function_name, char *format, va_list args);



#endif
