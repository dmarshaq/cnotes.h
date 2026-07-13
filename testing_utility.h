#ifndef CN_TESTING_UTILITY_H
#define CN_TESTING_UTILITY_H

#include "cnotes.h"
#include <stdio.h>

void cn__json_escape_str(FILE *f, Cn_String s) {
    if (cn_str_is_empty(s)) {
        fputs("null", f);
        return;
    }

    fputc('"', f);
    char *str = s.data;
    int64_t len = s.length;

    for (int64_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)str[i];

        switch (c) {
            // Escape other control chars.
            case '"':  fputs("\\\"", f); break;
            case '\\': fputs("\\\\", f); break;
            case '\b': fputs("\\b", f); break;
            case '\f': fputs("\\f", f); break;
            case '\n': fputs("\\n", f); break;
            case '\r': fputs("\\r", f); break;
            case '\t': fputs("\\t", f); break;

            default:
                if (c < 0x20) {
                    fprintf(f, "\\u%04x", c);
                } else {
                    fputc(c, f);
                }
                break;
        }
    }

    fputc('"', f);
}

void cn__json_escape_cstr(FILE *f, char *str) {
    cn__json_escape_str(f, CN_CSTR(str));
}

CNDEF void cn_test_diagnostic_handler(Cn_Diagnostic_Level level, Cn_Location *loc, Cn_Diagnostic_Code code, Cn_String span, const char *format, va_list args) {
    CN_UNUSED(span);

    if (level < cn_min_diagnostic_level)
        return;

    const char *level_str = NULL;
    switch (level) {
        case CN_DIAGNOSTIC_INFO: level_str = "info";        break;
        case CN_DIAGNOSTIC_WARNING: level_str = "warning";  break;
        case CN_DIAGNOSTIC_ERROR: level_str = "error";      break;
    }

    char *code_str;
    char code_buf[32];

    if (!cn_str_is_empty(CN_DIAGNOSTIC_CODES[code])) {
        snprintf(code_buf, sizeof(code_buf), "%.*s", CN_UNPACK(CN_DIAGNOSTIC_CODES[code]));
        code_str = code_buf;
    } else {
        snprintf(code_buf, sizeof(code_buf), "CN%04d", code);
        code_str = code_buf;
    }

    char message[512];
    int64_t written = vsnprintf(message, sizeof(message), format, args);
    if (written < 0) {
        cn_log(CN_ERROR, "test: Couldn't write diagnostic message to the buffer of %lu size.", sizeof(message));
        exit(1);
    }

    fputs("{", stdout);

    /* level */
    fputs("\"level\":\"", stdout);
    fputs(level_str, stdout);
    fputs("\",", stdout);

    /* code */
    fputs("\"code\":", stdout);
    cn__json_escape_cstr(stdout, code_str);
    fputc(',', stdout);

    /* message */
    fputs("\"message\":", stdout);
    cn__json_escape_cstr(stdout, message);
    fputc(',', stdout);

    /* file */
    fputs("\"file\":", stdout);
    cn__json_escape_str(stdout, loc->file);
    fputc(',', stdout);

    /* numeric fields */
    fprintf(stdout,
        "\"line\":%ld,"
        "\"column\":%ld,",
        loc->line,
        loc->column);

    fputs("}\n", stdout);
}



#endif // CN_TESTING_UTILITY_H


