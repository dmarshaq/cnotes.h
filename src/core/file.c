#include "core/file.h"
#include "core/core.h"
#include "core/type.h"
#include "core/str.h"
#include <stdio.h>

/**
 * File utils.
 */

void* read_file_into_buffer(char *file_name, u64 *file_size, Allocator *allocator) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        printf_err("Couldn't open the file '%s'.\n", file_name);
        return NULL;
    }

    (void)fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    rewind(file);

    u8 *buffer = allocator_alloc(allocator, size);
    if (buffer == NULL) {
        printf_err("Memory allocation for string buffer failed while reading the file '%s'.\n", file_name);
        (void)fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, size, file) != size) {
        printf_err("Failure reading the file '%s'.\n", file_name);
        (void)fclose(file);
        free(buffer);
        return NULL;
    }

    (void)fclose(file);

    if (file_size != NULL)
        *file_size = size;

    return buffer;
}

String read_file_into_str(char *file_name, Allocator *allocator) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        printf_err("Couldn't open the file '%s'.\n", file_name);
    }

    (void)fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    String str = STR((s64)file_size, allocator_alloc(allocator, file_size));

    if (str.data == NULL) {
        printf_err("Memory allocation for string buffer failed while reading the file '%s'.\n", file_name);
        (void)fclose(file);
        str.length = 0;
    }

    if (fread(str.data, 1, file_size, file) != file_size) {
        printf_err("Failure reading the file '%s'.\n", file_name);
        (void)fclose(file);
        free(str.data);
        str.data = NULL;
        str.length = 0;
    }

    (void)fclose(file);

    return str;
}


int write_str_to_file(String str, char *file_name) {
    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        printf_err("Couldn't open the file for writing '%s'.\n", file_name);
        return 1;
    }

    s64 written = fwrite(str.data, 1, str.length, file);

    if (written != str.length) {
        printf_err("Failed writing to file '%s', %ld bytes written compared to the expected %ld bytes in the string.\n", file_name, written, str.length);

        (void)fclose(file);

        return 1;
    }
    
    (void)fclose(file);

    return 0;
}


int fwrite_str(String str, FILE *file) {
    s64 written = fwrite(str.data, 1, str.length, file);

    if (written != str.length) {
        printf_err("Failed fwrite, %ld bytes written compared to the expected %ld bytes in the string.\n", written, str.length);

        (void)fclose(file);
        return 1;
    }

    return 0;
}






