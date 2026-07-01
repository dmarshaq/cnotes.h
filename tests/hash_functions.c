// Test: Hashing functions (cn_hash_*)
#include "../cnotes.h"

int main(void) {
    // Test cn_hash_u64 - consistency
    {
        uint64_t a = cn_hash_u64(12345);
        uint64_t b = cn_hash_u64(12345);
        uint64_t c = cn_hash_u64(54321);

        CN_ASSERT(a == b);
        CN_ASSERT(a != c);
        CN_ASSERT(a != 0);
    }

    // Test cn_hash_u64 - different values produce different hashes
    {
        uint64_t h0 = cn_hash_u64(0);
        uint64_t h1 = cn_hash_u64(1);
        uint64_t h2 = cn_hash_u64(2);
        uint64_t hmax = cn_hash_u64(0xFFFFFFFFFFFFFFFF);

        CN_ASSERT(h0 != h1);
        CN_ASSERT(h1 != h2);
        CN_ASSERT(h0 != hmax);
    }

    // Test cn_hash_ptr
    {
        int x, y;
        uint64_t hx1 = cn_hash_ptr(&x);
        uint64_t hx2 = cn_hash_ptr(&x);
        uint64_t hy = cn_hash_ptr(&y);

        CN_ASSERT(hx1 == hx2);
        CN_ASSERT(hx1 != hy);
    }

    // Test cn_hash_bytes - consistency
    {
        char data[] = "test data";
        uint64_t h1 = cn_hash_bytes(data, sizeof(data) - 1);
        uint64_t h2 = cn_hash_bytes(data, sizeof(data) - 1);

        CN_ASSERT(h1 == h2);
    }

    // Test cn_hash_bytes - different data produces different hashes
    {
        char data1[] = "hello";
        char data2[] = "world";

        uint64_t h1 = cn_hash_bytes(data1, 5);
        uint64_t h2 = cn_hash_bytes(data2, 5);

        CN_ASSERT(h1 != h2);
    }

    // Test cn_hash_bytes - length matters
    {
        char data[] = "hello world";
        uint64_t h1 = cn_hash_bytes(data, 5);
        uint64_t h2 = cn_hash_bytes(data, 11);

        CN_ASSERT(h1 != h2);
    }

    // Test cn_hash_mix
    {
        uint64_t a = 12345;
        uint64_t b = 67890;

        uint64_t mixed1 = cn_hash_mix(a, b);
        uint64_t mixed2 = cn_hash_mix(a, b);
        uint64_t mixed3 = cn_hash_mix(b, a);

        CN_ASSERT(mixed1 == mixed2);
        CN_ASSERT(mixed1 != mixed3);  // Order matters
        CN_ASSERT(mixed1 != a);
        CN_ASSERT(mixed1 != b);
    }

    // Test cn_hash_mix - combining with zero
    {
        uint64_t a = 12345;
        uint64_t mixed = cn_hash_mix(a, 0);

        CN_ASSERT(mixed != a);
        CN_ASSERT(mixed != 0);
    }

    return 0;
}
