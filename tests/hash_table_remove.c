#include "../cnotes.h"

void hash_table_put(int **table, int value, const char *key) {
    Cn_String str = CN_CSTR(key);
    cn_hash_table_put(table, value, &str);
}

void hash_table_remove(int **table, const char *key) {
    Cn_String str = CN_CSTR(key);
    cn_hash_table_remove(table, &str);
}

int main(void) {
    int *table = cn_hash_table_make(Cn_String, int, 2, (Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);

    int items[]        = { 21, 90, 12, 74, -1, 2, -38, -48, 23, -458, 32, 1, 12, 0, 0, 0, -16, 16, 8, 8 };
    const char *keys[] = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu", "nu", "xi", "omicron", "pi", "rho", "sigma", "upsilon", "phi" };

    CN_ASSERT(sizeof(items) / sizeof(items[0]) == sizeof(keys) / sizeof(keys[0]));

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        hash_table_put(&table, items[i], keys[i]);
    }

    cn_hash_table_print((void **)&table);

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        hash_table_remove(&table, keys[i]);
    }

    cn_hash_table_print((void **)&table);

    return 0;
}
