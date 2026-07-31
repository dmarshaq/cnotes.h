#include "../cnotes.h"

Cn_Pool pool = {0};

int main(void) {
    pool = cn_pool_make(8, 16, 24, 4096);

    CN_ASSERT(pool.blocks_length == 4);
    CN_ASSERT(pool.blocks[0]->capacity == 8);
    CN_ASSERT(pool.blocks[3]->capacity == 4096);
    CN_ASSERT(CN_POOL_BLOCK_SLOTS(pool.blocks[0]) == 512);
    CN_ASSERT(CN_POOL_BLOCK_SLOTS(pool.blocks[3]) == 1);

    // Allocation is taken from the smallest size class that fits it.
    uint8_t *a = cn_pool_alloc(&pool, 1);
    uint8_t *b = cn_pool_alloc(&pool, 8);
    uint8_t *c = cn_pool_alloc(&pool, 9);
    uint8_t *d = cn_pool_alloc(&pool, 16);

    CN_ASSERT(a == pool.blocks[0]->mem + 0);
    CN_ASSERT(b == pool.blocks[0]->mem + 8);
    CN_ASSERT(c == pool.blocks[1]->mem + 0);
    CN_ASSERT(d == pool.blocks[1]->mem + 16);

    *a = 11;
    *b = 22;
    *c = 33;
    *d = 44;

    // Freed slot is reused, neighbouring slots are untouched.
    cn_pool_free(&pool, a);
    CN_ASSERT(pool.blocks[0]->flags[0] == 0x02);
    CN_ASSERT(cn_pool_alloc(&pool, 4) == a);
    CN_ASSERT(*b == 22);

    size_t slot;
    CN_ASSERT(cn_pool_allocation_info(&pool, d, &slot) == pool.blocks[1] && slot == 1);
    CN_ASSERT(cn_pool_allocation_info(&pool, d + 1, &slot) == NULL); // Points inside of the slot.
    CN_ASSERT(cn_pool_allocation_info(&pool, &pool, &slot) == NULL); // Points outside of the pool.

    // Realloc that still fits the slot keeps the memory in place.
    CN_ASSERT(cn_pool_realloc(&pool, c, 16) == c);
    CN_ASSERT(*c == 33);

    // Realloc that outgrows the slot moves the memory to a bigger size class and frees the slot.
    uint8_t *moved = cn_pool_realloc(&pool, c, 100);
    CN_ASSERT(moved == pool.blocks[3]->mem);
    CN_ASSERT(*moved == 33);
    CN_ASSERT(cn_pool_allocation_info(&pool, c, &slot) == NULL);
    CN_ASSERT(cn_pool_alloc(&pool, 16) == c);

    // Realloc of NULL is an allocation.
    CN_ASSERT(cn_pool_realloc(&pool, NULL, 8) != NULL);

    // Capacity 24 divides its block into 170 slots, so the last flag byte is partially used.
    CN_ASSERT(CN_POOL_BLOCK_SLOTS(pool.blocks[2]) == 170);

    uint8_t *last = NULL;
    for (int i = 0; i < 170; i++) {
        last = cn_pool_alloc(&pool, 17);
        CN_ASSERT(last == pool.blocks[2]->mem + i * 24);
    }

    CN_ASSERT(pool.blocks[2]->flags[20] == 0xFF);
    CN_ASSERT(pool.blocks[2]->flags[21] == 0x03);
    CN_ASSERT(pool.blocks[2]->next == NULL);

    // Full size class chains one more block of the same capacity.
    uint8_t *chained = cn_pool_alloc(&pool, 24);
    CN_ASSERT(pool.blocks[2]->next != NULL);
    CN_ASSERT(pool.blocks[2]->next->capacity == 24);
    CN_ASSERT(chained == pool.blocks[2]->next->mem);
    CN_ASSERT(pool.blocks[2]->flags[21] == 0x03);

    // Memory past the last slot of the block belongs to no slot.
    CN_ASSERT(cn_pool_allocation_info(&pool, pool.blocks[2]->mem + 170 * 24, &slot) == NULL);
    CN_ASSERT(cn_pool_allocation_info(&pool, last, &slot) == pool.blocks[2] && slot == 169);

    // Hole in the first block is refilled before the chained block is used.
    cn_pool_free(&pool, last);
    CN_ASSERT(cn_pool_alloc(&pool, 24) == last);

    // Freeing walks the whole chain of the size class.
    cn_pool_free(&pool, chained);
    CN_ASSERT(pool.blocks[2]->next->flags[0] == 0x00);

    cn_pool_free_all(&pool);

    for (size_t i = 0; i < pool.blocks_length; i++) {
        for (Cn_Pool_Block *block = pool.blocks[i]; block != NULL; block = block->next) {
            for (size_t f = 0; f < sizeof(block->flags); f++) {
                CN_ASSERT(block->flags[f] == 0);
            }
        }
    }

    CN_ASSERT(cn_pool_alloc(&pool, 8) == pool.blocks[0]->mem);

    cn_pool_destroy(&pool);

    CN_ASSERT(pool.blocks == NULL && pool.blocks_length == 0);

    return 0;
}
