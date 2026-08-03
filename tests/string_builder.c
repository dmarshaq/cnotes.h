#include "../cnotes.h"
int main(void) {
    // Stack-backed.
    Cn_String_Builder sb = cn_sb_make(16);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(sb.length == 0);

    cn_sb_append_char(&sb, 'h');
    cn_sb_append_char(&sb, 'i');
    CN_ASSERT(sb.length == 2);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(sb.data[0] == 'h');
    CN_ASSERT(sb.data[1] == 'i');

    cn_sb_append_str(&sb, CN_STR_LIT("_world"));
    CN_ASSERT(sb.length == 8);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    Cn_String view = cn_sb_to_str(&sb);
    CN_ASSERT(view.length == 8);
    CN_ASSERT(memcmp(view.data, "hi_world", 8) == 0);

    cn_sb_clear(&sb);
    CN_ASSERT(sb.length == 0);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    // Stack spill to heap.
    cn_sb_append_str(&sb, CN_STR_LIT("abcdefghijklmnopqrstuvwxyz"));
    CN_ASSERT(sb.length == 26);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    cn_sb_append_str(&sb, CN_STR_LIT("ABCDEF"));
    CN_ASSERT(sb.length == 32);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    cn_sb_append_char(&sb, '!'); 
    CN_ASSERT(sb.length == 33);
    CN_ASSERT(sb.capacity > CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(memcmp(sb.data, "abcdefghijklmnopqrstuvwxyzABCDEF!", 33) == 0);

    cn_sb_free(&sb);
    CN_ASSERT(sb.length == 0);
    CN_ASSERT(sb.capacity == 0);
    CN_ASSERT(sb.data == NULL);

    // Heap-backed from the start.
    Cn_String_Builder sb2 = cn_sb_make(64);
    CN_ASSERT(sb2.capacity == 64);
    CN_ASSERT(sb2.length == 0);
    CN_ASSERT(sb2.capacity > CN_SB_STACK_STORAGE_CAP);
    cn_sb_free(&sb2);

    // Append format
    Cn_String_Builder sb3 = cn_sb_make(16);
    cn_sb_append_format(&sb3, "int [%lld]", (int64_t)30);
    CN_ASSERT(sb3.length == 8);
    CN_ASSERT(memcmp(sb3.data, "int [30]", 8) == 0);
    cn_sb_free(&sb3);

    // Reverse.
    Cn_String_Builder sb4 = cn_sb_make(16);
    cn_sb_append_str(&sb4, CN_STR_LIT("**tni"));
    cn_sb_reverse(&sb4);
    CN_ASSERT(sb4.length == 5);
    CN_ASSERT(memcmp(sb4.data, "int**", 5) == 0);
    cn_sb_free(&sb4);

    // Prepend onto an empty builder, then in front of existing contents.
    Cn_String_Builder sb5 = cn_sb_make(16);
    cn_sb_prepend_str(&sb5, CN_STR_LIT("[4]"));
    CN_ASSERT(sb5.length == 3);
    CN_ASSERT(memcmp(sb5.data, "[4]", 3) == 0);

    cn_sb_prepend_str(&sb5, CN_STR_LIT("[3]"));
    CN_ASSERT(sb5.length == 6);
    CN_ASSERT(memcmp(sb5.data, "[3][4]", 6) == 0);

    // Mixing with append keeps both ends intact.
    cn_sb_append_str(&sb5, CN_STR_LIT("(void)"));
    cn_sb_prepend_str(&sb5, CN_STR_LIT("*"));
    CN_ASSERT(sb5.length == 13);
    CN_ASSERT(memcmp(sb5.data, "*[3][4](void)", 13) == 0);

    // Prepending an empty string is a no-op.
    cn_sb_prepend_str(&sb5, CN_STR(0, NULL));
    CN_ASSERT(sb5.length == 13);
    CN_ASSERT(memcmp(sb5.data, "*[3][4](void)", 13) == 0);
    cn_sb_free(&sb5);

    // Prepend spilling stack backed storage over to the heap, contents preserved.
    Cn_String_Builder sb6 = cn_sb_make(CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(sb6.capacity == CN_SB_STACK_STORAGE_CAP);
    cn_sb_append_char(&sb6, 'z');

    char filler[CN_SB_STACK_STORAGE_CAP];
    memset(filler, 'a', sizeof(filler));
    cn_sb_prepend_str(&sb6, CN_STR(sizeof(filler), filler));

    CN_ASSERT(sb6.capacity > CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(sb6.length == CN_SB_STACK_STORAGE_CAP + 1);
    CN_ASSERT(memcmp(sb6.data, filler, sizeof(filler)) == 0);
    CN_ASSERT(sb6.data[CN_SB_STACK_STORAGE_CAP] == 'z');
    cn_sb_free(&sb6);

    return 0;
}
