#include "../cnotes.h"

int main(void) {
    Cn_Chained_Arena arena = cn_chained_arena_make(64);

    int *ptr, *v1, *v2, *v3, *v4, *v5, *v6;

    ptr = cn_chained_arena_alloc(&arena, 50);
    ptr[0] = 1;
    v1 = ptr + 0;
    
    ptr[1] = 2;
    v2 = ptr + 1;

    ptr[2] = 3;
    v3 = ptr + 2;

    ptr = cn_chained_arena_alloc(&arena, 20);
    ptr[0] = -1;
    v4 = ptr + 0;

    ptr = cn_chained_arena_alloc(&arena, 20);
    ptr[1] = -2;
    v5 = ptr + 1;

    ptr = cn_chained_arena_alloc(&arena, 64);
    ptr[4] = 100;
    v6 = ptr + 4;

    ptr = cn_chained_arena_alloc(&arena, 10);

    CN_ASSERT(*v1 == 1);
    CN_ASSERT(*v2 == 2);
    CN_ASSERT(*v3 == 3);
    CN_ASSERT(*v4 == -1);
    CN_ASSERT(*v5 == -2);
    CN_ASSERT(*v6 == 100);
    
    cn_chained_arena_dealloc(&arena, 16);

    CN_ASSERT(*v1 == 1);
    CN_ASSERT(*v2 == 2);
    CN_ASSERT(*v3 == 3);
    CN_ASSERT(*v4 == -1);
    CN_ASSERT(*v5 == -2);
    CN_ASSERT(*v6 == 100);

    cn_chained_arena_dealloc(&arena, 58);

    CN_ASSERT(*v1 == 1);
    CN_ASSERT(*v2 == 2);
    CN_ASSERT(*v3 == 3);
    CN_ASSERT(*v4 == -1);
    CN_ASSERT(*v5 == -2);

    cn_chained_arena_dealloc(&arena, 38);

    CN_ASSERT(*v1 == 1);
    CN_ASSERT(*v2 == 2);
    CN_ASSERT(*v3 == 3);

    cn_chained_arena_dealloc(&arena, 48);

    CN_ASSERT(*v1 == 1);

    cn_chained_arena_dealloc(&arena, 9999);

    Cn_Chained_Arena_Block_Header *header = CN_CHAINED_ARENA_BLOCK_HEADER(&arena);
    CN_ASSERT(header->prev == NULL && header->allocated == 0);

    cn_chained_arena_free(&arena);
    
    CN_ASSERT(arena.block == NULL && arena.block_capacity == 0);

    return 0;
}
