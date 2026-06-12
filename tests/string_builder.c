#include "../cnotes.h"
int main(void) {
    // Stack-backed.
    Cn_String_Builder sb = CN_SB_MAKE(16);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(sb.length == 0);

    cn_sb_append_char(&sb, 'h');
    cn_sb_append_char(&sb, 'i');
    CN_ASSERT(sb.length == 2);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);
    CN_ASSERT(sb.data[0] == 'h');
    CN_ASSERT(sb.data[1] == 'i');

    cn_sb_append_str(&sb, CN_CSTR("_world"));
    CN_ASSERT(sb.length == 8);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    Cn_String view = cn_sb_to_str(&sb);
    CN_ASSERT(view.length == 8);
    CN_ASSERT(memcmp(view.data, "hi_world", 8) == 0);

    cn_sb_clear(&sb);
    CN_ASSERT(sb.length == 0);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    // Stack spill to heap.
    cn_sb_append_str(&sb, CN_CSTR("abcdefghijklmnopqrstuvwxyz"));
    CN_ASSERT(sb.length == 26);
    CN_ASSERT(sb.capacity == CN_SB_STACK_STORAGE_CAP);

    cn_sb_append_str(&sb, CN_CSTR("ABCDEF"));
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
    Cn_String_Builder sb2 = CN_SB_MAKE(64);
    CN_ASSERT(sb2.capacity == 64);
    CN_ASSERT(sb2.length == 0);
    CN_ASSERT(sb2.capacity > CN_SB_STACK_STORAGE_CAP);
    cn_sb_free(&sb2);

    // Append format
    Cn_String_Builder sb3 = CN_SB_MAKE(16);
    cn_sb_append_format(&sb3, "int [%lld]", (int64_t)30);
    CN_ASSERT(sb3.length == 8);
    CN_ASSERT(memcmp(sb3.data, "int [30]", 8) == 0);
    cn_sb_free(&sb3);

    // Reverse.
    Cn_String_Builder sb4 = CN_SB_MAKE(16);
    cn_sb_append_str(&sb4, CN_CSTR("**tni"));
    cn_sb_reverse(&sb4);
    CN_ASSERT(sb4.length == 5);
    CN_ASSERT(memcmp(sb4.data, "int**", 5) == 0);
    cn_sb_free(&sb4);

    return 0;
}
