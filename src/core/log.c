#include "core/log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <pthread.h>
#endif


static Log_Level log_minimum_level  = 0;
static FILE *log_output             = NULL;

void log_set_minimum_level(Log_Level level) {
    log_minimum_level = level;
}

void log_set_output(FILE *stream) {
    log_output = stream;
}

void log_print(Log_Level level, const char *file_name, s64 line, const char *function_name, char *format, ...) {
    va_list args;
    va_start(args, format);
    log_vprint(level, file_name, line, function_name, format, args);
    va_end(args);

//     va_list args;
//     va_start(args, format);

//     // Print log level.
//     switch(level) {
//         case LOG_LEVEL_INFO:
// #ifdef LOG_INFO_COLOR
//             fprintf(log_output, ANSI_BLUE"[INFO]"ANSI_RESET);
// #else
//             fprintf(log_output, "[INFO]");
// #endif
//             break;
//         case LOG_LEVEL_WARNING:
// #ifdef LOG_INFO_COLOR
//             fprintf(log_output, ANSI_YELLOW"[WARN]"ANSI_RESET);
// #else
//             fprintf(log_output, "[WARN]");
// #endif
//             break;
//         case LOG_LEVEL_ERROR:
// #ifdef LOG_INFO_COLOR
//             fprintf(log_output, ANSI_RED"[ERRO]"ANSI_RESET);
// #else
//             fprintf(log_output, "[ERRO]");
// #endif
//             break;
//     }
// 
// #ifdef LOG_INFO_TIME
//     struct timespec ts;
//     clock_gettime(CLOCK_REALTIME, &ts);
// 
//     struct tm tm;
// #if defined(_WIN32)
//     localtime_s(&tm, &ts.tv_sec);
// #else
//     localtime_r(&ts.tv_sec, &tm);
// #endif
// 
//     fprintf(log_output, " %04d-%02d-%02d %02d:%02d:%02d.%03ld",
//             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000000);
// #endif
// 
// #ifdef LOG_INFO_THREAD
// 
// #if defined(_WIN32)
//     fprintf(log_output, " [TID %lu]", GetCurrentThreadId());
// #else
//     fprintf(log_output, " [TID %lu]", (unsigned long)pthread_self());
// #endif
// 
// #endif
// 
// 
// #ifdef LOG_INFO_FILE
//     fprintf(log_output, " %s", file_name);
// #endif
// 
// #ifdef LOG_INFO_LINE
//     fprintf(log_output, ":%ld", line);
// #endif
// 
// #ifdef LOG_INFO_FUNC
//     fprintf(log_output, " %s():", function_name);
// #endif
// 
// 
//     fprintf(log_output, " ");
// 
//     vfprintf(log_output, format, args);
// 
//     fprintf(log_output, "\n");
// 
//     fflush(log_output); // Ensure it's written.
//     va_end(args);

}



void log_vprint(Log_Level level, const char *file_name, s64 line, const char *function_name, char *format, va_list args) {
    if (level < log_minimum_level)
        return;

    if (log_output == NULL)
        log_output = stderr;

    // Print log level.
    switch(level) {
        case LOG_LEVEL_INFO:
#ifdef LOG_INFO_COLOR
            fprintf(log_output, ANSI_BLUE"[INFO]"ANSI_RESET);
#else
            fprintf(log_output, "[INFO]");
#endif
            break;
        case LOG_LEVEL_WARNING:
#ifdef LOG_INFO_COLOR
            fprintf(log_output, ANSI_YELLOW"[WARN]"ANSI_RESET);
#else
            fprintf(log_output, "[WARN]");
#endif
            break;
        case LOG_LEVEL_ERROR:
#ifdef LOG_INFO_COLOR
            fprintf(log_output, ANSI_RED"[ERRO]"ANSI_RESET);
#else
            fprintf(log_output, "[ERRO]");
#endif
            break;
    }

#ifdef LOG_INFO_TIME
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    struct tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &ts.tv_sec);
#else
    localtime_r(&ts.tv_sec, &tm);
#endif

    fprintf(log_output, " %04d-%02d-%02d %02d:%02d:%02d.%03ld",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000000);
#endif

#ifdef LOG_INFO_THREAD

#if defined(_WIN32)
    fprintf(log_output, " [TID %lu]", GetCurrentThreadId());
#else
    fprintf(log_output, " [TID %lu]", (unsigned long)pthread_self());
#endif

#endif


#ifdef LOG_INFO_FILE
    fprintf(log_output, " %s", file_name);
#endif

#ifdef LOG_INFO_LINE
    fprintf(log_output, ":%ld", line);
#endif

#ifdef LOG_INFO_FUNC
    fprintf(log_output, " %s():", function_name);
#endif


    fprintf(log_output, " ");

    vfprintf(log_output, format, args);

    fprintf(log_output, "\n");

    fflush(log_output); // Ensure it's written.
}
