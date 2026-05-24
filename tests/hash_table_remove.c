#include "../cnotes.h"
#include <stdint.h>

// This wrapper is good, because any string copied in here will only exist inside this function on the stack and will be gone after.
void scoped_hash_table_put(int **table, int item, const char *key_str) {
    char key[strlen(key_str) + 1];
    key[0] = '\0';
    strcpy(key, key_str);

    cn_hash_table_put(table, item, sizeof(key) - 1, (uint8_t *)key);
}

int main(void) {
    int *table = cn_hash_table_make(int, 2);

    int items[]        = { 21, 90, 12, 74, -1, 2, -38, -48, 23, -458, 32, 1, 12, 0, 0, 0, -16, 16, 8, 8 };
    const char *keys[] = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu", "nu", "xi", "omicron", "pi", "rho", "sigma", "upsilon", "phi" };

    CN_ASSERT(sizeof(items) / sizeof(items[0]) == sizeof(keys) / sizeof(keys[0]));

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        scoped_hash_table_put(&table, items[i], keys[i]);
    }

    cn_hash_table_print((void **)&table);

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        cn_hash_table_remove(&table, strlen(keys[i]), (uint8_t *)keys[i]);
    }

    cn_hash_table_print((void **)&table);

    return 0;
}
