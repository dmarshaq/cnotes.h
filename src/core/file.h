#ifndef FILE_H
#define FILE_H

#include "core/core.h"
#include "core/type.h"
#include "core/str.h"

/**
 * File utils.
 */


/**
 * Reads contents of the file into the buffer that is preemptivly allocated using allooator.
 * Returns pointer to the buffer and sets buffer size in bytes into the file_size.
 * @Important: Buffer should be freed manually when not used anymore.
 */
void *read_file_into_buffer(char *file_name, u64 *file_size, Allocator *allocator);

/**
 * Reads contents of the file into the String structure that is preemptivly allocated using allocator.
 * Returns String structure with pointer to dynamically allocated memory and size of it.
 * @Important: String should be freed manually when not used anymore.
 */
String read_file_into_str(char *file_name, Allocator *allocator);

/**
 * Writes contents of string into the file specified by the file_name.
 * It will overwrite file if it already exists.
 * Returns 0 on success.
 * Will return any other value and won't write to file if failed.
 */
int write_str_to_file(String str, char *file_name);

/**
 * Perfoms fwrite() of the string contents to the the specified file.
 * Returns 0 on success.
 * Will return any other value and won't write to file if failed.
 */
int fwrite_str(String str, FILE *file);







#endif
