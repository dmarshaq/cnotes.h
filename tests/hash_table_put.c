#include "../cnotes.h"
#include <unistd.h>

void hash_table_put(int **table, int value, const char *key) {
    Cn_String str = CN_STR(strlen(key), key);
    cn_hash_table_put(table, value, &str);
}

int *hash_table_get(int **table, const char *key) {
    Cn_String str = CN_STR(strlen(key), key);
    int *result = cn_hash_table_get(table, &str);
    CN_ASSERT(result != NULL);
    return result;
}

int main(void) {
    int *table = cn_hash_table_make(Cn_String, int, 3, (Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);

    cn_hash_table_print(stdout, (void **)&table);

    CN_ASSERT(cn_hash_table_count(&table)     == 0);

    hash_table_put(&table, 0xABC, "foo");
    hash_table_put(&table, -9, "bar");

    cn_hash_table_print(stdout, (void **)&table);

    CN_ASSERT(cn_hash_table_count(&table)    == 2);

    CN_ASSERT(*(int *)hash_table_get(&table, "foo") == 0xABC);
    CN_ASSERT(*(int *)hash_table_get(&table, "bar") == -9);

    hash_table_put(&table, 0, "az");
    hash_table_put(&table, 1, "by");
    hash_table_put(&table, 2, "cx");
    hash_table_put(&table, 3, "dw");
    hash_table_put(&table, 4, "ev");
    hash_table_put(&table, 5, "fu");
    hash_table_put(&table, 6, "gt");
    hash_table_put(&table, 7, "hs");
    hash_table_put(&table, 8, "ir");
    hash_table_put(&table, 9, "jq");

    cn_hash_table_print(stdout, (void **)&table);

    CN_ASSERT(cn_hash_table_count(&table)    == 12);

    CN_ASSERT(*(int *)hash_table_get(&table, "foo") == 0xABC);
    CN_ASSERT(*(int *)hash_table_get(&table, "bar") == -9);

    CN_ASSERT(*(int *)hash_table_get(&table, "az") == 0);
    CN_ASSERT(*(int *)hash_table_get(&table, "by") == 1);
    CN_ASSERT(*(int *)hash_table_get(&table, "cx") == 2);
    CN_ASSERT(*(int *)hash_table_get(&table, "dw") == 3);
    CN_ASSERT(*(int *)hash_table_get(&table, "ev") == 4);
    CN_ASSERT(*(int *)hash_table_get(&table, "fu") == 5);
    CN_ASSERT(*(int *)hash_table_get(&table, "gt") == 6);
    CN_ASSERT(*(int *)hash_table_get(&table, "hs") == 7);
    CN_ASSERT(*(int *)hash_table_get(&table, "ir") == 8);
    CN_ASSERT(*(int *)hash_table_get(&table, "jq") == 9);
    
    int items[]        = { 21, 90, 12, 74, -1, 2, -38, -48, 23, -458, 32, 1, 12, 0, 0, 0, -16, 16, 8, 8 };
    const char *keys[] = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu", "nu", "xi", "omicron", "pi", "rho", "sigma", "upsilon", "phi" };

    CN_ASSERT(sizeof(items) / sizeof(items[0]) == sizeof(keys) / sizeof(keys[0]));

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        hash_table_put(&table, items[i], keys[i]);
    }

    cn_hash_table_print(stdout, (void **)&table);

    CN_ASSERT(*(int *)hash_table_get(&table, "foo") == 0xABC);
    CN_ASSERT(*(int *)hash_table_get(&table, "bar") == -9);

    CN_ASSERT(*(int *)hash_table_get(&table, "az") == 0);
    CN_ASSERT(*(int *)hash_table_get(&table, "by") == 1);
    CN_ASSERT(*(int *)hash_table_get(&table, "cx") == 2);
    CN_ASSERT(*(int *)hash_table_get(&table, "dw") == 3);
    CN_ASSERT(*(int *)hash_table_get(&table, "ev") == 4);
    CN_ASSERT(*(int *)hash_table_get(&table, "fu") == 5);
    CN_ASSERT(*(int *)hash_table_get(&table, "gt") == 6);
    CN_ASSERT(*(int *)hash_table_get(&table, "hs") == 7);
    CN_ASSERT(*(int *)hash_table_get(&table, "ir") == 8);
    CN_ASSERT(*(int *)hash_table_get(&table, "jq") == 9);

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        CN_ASSERT(*(int *)hash_table_get(&table, keys[i]) == items[i]);
    }

    CN_ASSERT(cn_hash_table_count(&table)    == 12 + sizeof(items) / sizeof(items[0]));

    return 0;
}
