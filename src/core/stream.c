#include "core/stream.h"

#include "core/type.h"
#include "core/core.h"
#include "core/log.h"

#include <stdlib.h>
#include <string.h>



Stream stream_make(u64 capacity) {
    void *mem = malloc(capacity);

    if (mem == NULL) {
        LOG_ERROR("Couldn't malloc %lld bytes of memory for the stream.", capacity);
        return (Stream) {0};
    }

    return (Stream) {
        .head = mem,
        .tail = mem + capacity,
        .length = 0,
        .ptr = mem,
    };
 
}

void stream_write(Stream *stream, u64 size, void *data) {
    stream->length += size;
    if (stream->length > stream->tail - stream->head) {
        stream->length = stream->tail - stream->head;
    }
    

    u64 written;
    while(size > 0) {
        written = stream->tail - stream->ptr > size ? size : stream->tail - stream->ptr;
        memcpy(stream->ptr, data, written);

        size -= written;
        data += written;
        stream->ptr += written;

        if (stream->ptr == stream->tail)
            stream->ptr = stream->head;
    }
}

void stream_read(Stream *stream, void *buffer) {
    u64 read = stream->ptr - stream->head > stream->length ? stream->length : stream->ptr - stream->head;

    if (stream->length - read > 0) {
        memcpy(buffer, stream->tail - (stream->length - read), (stream->length - read));
    }

    memcpy(buffer + (stream->length - read), stream->ptr - read, read);
}

u64 stream_tell(Stream *stream) {
    return stream->length;
}
