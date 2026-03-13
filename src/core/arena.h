#ifndef ARENA_H
#define ARENA_H


#include "core/type.h"

typedef struct arena {
    u64 capacity;
    void *allocation;
    void *ptr;
} Arena;

/**
 * Allocates allocation through 'malloc()'.
 * Returns arena's struct.
 */
Arena arena_make(u64 capacity);

/**
 * Allocates specified memory size from the arena.
 * Returns pointer to the memory segment.
 * @Important: It will be invalid once arena is freed.
 */
void *arena_alloc(Arena *arena, u64 size);

/**
 * Tells size of the already allocated arena segment by all 'arena_alloc()' calls.
 */
u64 arena_size(Arena *arena);

/**
 * Clears allocated arena segment.
 * Makes all previous arena_alloc segments be counted as invalid.
 */
void arena_clear(Arena *arena);

/**
 * Completely frees memory occupied by arena.
 */
void arena_free(Arena *arena);



#endif
