#include "../cnotes.h"
#include <unistd.h>

void hash_set_put(Cn_String **set, const char *value) {
    Cn_String str = CN_CSTR(value);
    (void)cn_hash_set_put(set, str);
}

bool hash_set_contains(Cn_String **set, const char *value) {
    Cn_String str = CN_CSTR(value);
    return cn_hash_set_contains(set, str);
}

void hash_set_remove(Cn_String **set, const char *value) {
    Cn_String str = CN_CSTR(value);
    return cn_hash_set_remove(set, str);
}

int main(void) {
    Cn_String *set = cn_hash_set_make(Cn_String, 3, (Cn_Hash_Function *)cn_str_hash, (Cn_Equals_Function *)cn_str_equals);

    CN_ASSERT(set != NULL);

    CN_ASSERT(((Cn_Hash_Set_Header *)((uint8_t *)(set - 1) - sizeof(Cn_Hash_Set_Header)))->count == 0);
    
    hash_set_put(&set, "foo");
    hash_set_put(&set, "bar");

    CN_ASSERT(cn_hash_set_header(&set)->count == 2);

    CN_ASSERT(hash_set_contains(&set, "a") == false);
    CN_ASSERT(hash_set_contains(&set, "b") == false);

    CN_ASSERT(hash_set_contains(&set, "foo") == true);
    CN_ASSERT(hash_set_contains(&set, "bar") == true);

    hash_set_remove(&set, "foo");
    hash_set_remove(&set, "bar");

    CN_ASSERT(cn_hash_set_header(&set)->count == 0);

    CN_ASSERT(hash_set_contains(&set, "foo") == false);
    CN_ASSERT(hash_set_contains(&set, "bar") == false);

    hash_set_put(&set, "foo");
    hash_set_put(&set, "bar");

    CN_ASSERT(cn_hash_set_header(&set)->count == 2);

    CN_ASSERT(hash_set_contains(&set, "foo") == true);
    CN_ASSERT(hash_set_contains(&set, "bar") == true);

    hash_set_put(&set, "aa");
    hash_set_put(&set, "bb");
    hash_set_put(&set, "cc");
    hash_set_put(&set, "dd");

    CN_ASSERT(cn_hash_set_header(&set)->count == 6);

    CN_ASSERT(hash_set_contains(&set, "foo") == true);
    CN_ASSERT(hash_set_contains(&set, "bar") == true);
    CN_ASSERT(hash_set_contains(&set, "aa") == true);
    CN_ASSERT(hash_set_contains(&set, "bb") == true);
    CN_ASSERT(hash_set_contains(&set, "cc") == true);
    CN_ASSERT(hash_set_contains(&set, "dd") == true);

    return 0;
}
