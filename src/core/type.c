#include "core/type.h"

#include <stdbool.h>
#include <string.h>

bool is_little_endian() {
    u16 x = 0x0001;     // two-byte value.
    u8 *p = (u8 *)&x;   // look at lowest-addressed byte.
    return p[0] == 1;   // if first byte is 1 → little-endian.
}



void mem_copy_int(void *dest, void *src, u8 dest_size, u8 src_size, u8 filler) {
    u8 min = src_size < dest_size ? src_size : dest_size;

    if (is_little_endian()) {
        memcpy(dest, src, min);
        memset(dest + min, filler, dest_size - min);
    }
    else {
        memcpy(dest + dest_size - min, src + src_size - min, min);
        memset(dest, filler, dest_size - min);
    }
}
