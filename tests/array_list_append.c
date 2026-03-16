#define CN_IMPLEMENTATION
#include "../cnotes.h"


int main(void) {

    int *list = cn_array_list_make(int, 2);

    CN_ASSERT(cn_array_list_item_size(&list) == sizeof(int));
    CN_ASSERT(cn_array_list_capacity(&list) == 2);
    CN_ASSERT(cn_array_list_length(&list) == 0);

    cn_array_list_append(&list, 58);
    cn_array_list_append(&list, -2);

    CN_ASSERT(list[0] == 58);
    CN_ASSERT(list[1] == -2);

    CN_ASSERT(cn_array_list_capacity(&list) == 2);
    CN_ASSERT(cn_array_list_length(&list) == 2);

    
    cn_array_list_append(&list, -10);

    CN_ASSERT(list[0] == 58);
    CN_ASSERT(list[1] == -2);
    CN_ASSERT(list[2] == -10);

    CN_ASSERT(cn_array_list_capacity(&list) == 4);
    CN_ASSERT(cn_array_list_length(&list) == 3);

    int items[] = { 21, 90, 12, 74, -1, 2, -38, -48, 23, -458, 32, 1, 12, 0, 0, 0, -16, 16, 8, 8 };
    cn_array_list_append_multiple(&list, items, sizeof(items) / sizeof(items[0]));

    CN_ASSERT(list[0] == 58);
    CN_ASSERT(list[1] == -2);
    CN_ASSERT(list[2] == -10);

    CN_ASSERT(cn_array_list_capacity(&list) == 32);
    CN_ASSERT(cn_array_list_length(&list) == 23);

    for (int64_t i = 3; i < cn_array_list_length(&list); i++) {
        CN_ASSERT(list[i] == items[i - 3]);
    }

    return 0;
}
