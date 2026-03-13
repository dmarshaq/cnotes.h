#ifndef STREAM_H
#define STREAM_H

#include "core/type.h"

typedef struct stream {
    void *head;
    void *tail;
    u64 length;
    void *ptr;
} Stream;

/**
 * Allocates buffer through 'malloc()'
 * Returns stream's struct.
 */
Stream stream_make(u64 capacity);

/**
 * Writes data to the stream, if data size is bigger than stream's capacity, it will be cut.
 * Parameter size should be greater than zero.
 */
void stream_write(Stream *stream, u64 size, void *data);

/**
 * Reads data from the stream into a buffer.
 * Buffer must be big enough to hold the data.
 * @Important: It doens't delete data from the stream.
 */
void stream_read(Stream *stream, void *buffer);

/**
 * Gives information of about stream data size.
 */
u64 stream_tell(Stream *stream);



#endif
