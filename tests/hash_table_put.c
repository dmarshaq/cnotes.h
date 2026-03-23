#include "../cnotes.h"

// This wrapper is good, because any string copied in here will only exist inside this function on the stack and will be gone after.
void scoped_hash_table_put(int **table, int item, const char *key_str) {
    char key[strlen(key_str) + 1];
    key[0] = '\0';
    strcpy(key, key_str);

    cn_hash_table_put(table, item, sizeof(key) - 1, (uint8_t *)key);
}

int main(void) {
    int *table = cn_hash_table_make(int, 2);

    cn_hash_table_print((void **)&table);

    CN_ASSERT(cn_hash_table_count(&table)     == 0);
    CN_ASSERT(cn_hash_table_capacity(&table)  == 2);
    CN_ASSERT(cn_hash_table_item_size(&table) == sizeof(int));

    {
        Cn_Hash_Table_Header *header = cn_hash_table_header(&table);

        CN_ASSERT(header->hash_func == cn_hashf);

        CN_ASSERT(cn_array_list_length(&header->keys)    == 0);
        CN_ASSERT(cn_array_list_capacity(&header->keys)  >= 2);
        CN_ASSERT(cn_array_list_item_size(&header->keys) == sizeof(uint8_t));
    }

    scoped_hash_table_put(&table, 71, "foo");
    scoped_hash_table_put(&table, -9, "bar");

    cn_hash_table_print((void **)&table);

    CN_ASSERT(cn_hash_table_count(&table)    == 2);
    CN_ASSERT(cn_hash_table_capacity(&table) == 2);

    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("foo") - 1, (uint8_t *)"foo") == 71);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("bar") - 1, (uint8_t *)"bar") == -9);

    scoped_hash_table_put(&table, 0, "az");
    scoped_hash_table_put(&table, 1, "by");
    scoped_hash_table_put(&table, 2, "cx");
    scoped_hash_table_put(&table, 3, "dw");
    scoped_hash_table_put(&table, 4, "ev");
    scoped_hash_table_put(&table, 5, "fu");
    scoped_hash_table_put(&table, 6, "gt");
    scoped_hash_table_put(&table, 7, "hs");
    scoped_hash_table_put(&table, 8, "ir");
    scoped_hash_table_put(&table, 9, "jq");

    cn_hash_table_print((void **)&table);

    CN_ASSERT(cn_hash_table_count(&table)    == 12);
    CN_ASSERT(cn_hash_table_capacity(&table) == 16);

    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("foo") - 1, (uint8_t *)"foo") == 71);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("bar") - 1, (uint8_t *)"bar") == -9);

    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("az") - 1, (uint8_t *)"az") == 0);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("by") - 1, (uint8_t *)"by") == 1);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("cx") - 1, (uint8_t *)"cx") == 2);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("dw") - 1, (uint8_t *)"dw") == 3);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("ev") - 1, (uint8_t *)"ev") == 4);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("fu") - 1, (uint8_t *)"fu") == 5);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("gt") - 1, (uint8_t *)"gt") == 6);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("hs") - 1, (uint8_t *)"hs") == 7);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("ir") - 1, (uint8_t *)"ir") == 8);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("jq") - 1, (uint8_t *)"jq") == 9);

    
    int items[]        = { 21, 90, 12, 74, -1, 2, -38, -48, 23, -458, 32, 1, 12, 0, 0, 0, -16, 16, 8, 8 };
    const char *keys[] = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu", "nu", "xi", "omicron", "pi", "rho", "sigma", "upsilon", "phi" };

    CN_ASSERT(sizeof(items) / sizeof(items[0]) == sizeof(keys) / sizeof(keys[0]));

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        scoped_hash_table_put(&table, items[i], keys[i]);
    }

    cn_hash_table_print((void **)&table);

    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("foo") - 1, (uint8_t *)"foo") == 71);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("bar") - 1, (uint8_t *)"bar") == -9);

    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("az") - 1, (uint8_t *)"az") == 0);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("by") - 1, (uint8_t *)"by") == 1);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("cx") - 1, (uint8_t *)"cx") == 2);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("dw") - 1, (uint8_t *)"dw") == 3);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("ev") - 1, (uint8_t *)"ev") == 4);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("fu") - 1, (uint8_t *)"fu") == 5);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("gt") - 1, (uint8_t *)"gt") == 6);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("hs") - 1, (uint8_t *)"hs") == 7);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("ir") - 1, (uint8_t *)"ir") == 8);
    CN_ASSERT(*(int *)cn_hash_table_get(&table, sizeof("jq") - 1, (uint8_t *)"jq") == 9);

    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        CN_ASSERT(*(int *)cn_hash_table_get(&table, strlen(keys[i]), (uint8_t *)keys[i]) == items[i]);
    }

    CN_ASSERT(cn_hash_table_count(&table)    == 12 + sizeof(items) / sizeof(items[0]));
    CN_ASSERT(cn_hash_table_capacity(&table) < cn_hash_table_count(&table) * 2);

    return 0;
}
