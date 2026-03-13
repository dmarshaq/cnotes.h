#include "core/structs.h"
#include "core/core.h"
#include "core/type.h"
#include "core/mathf.h"
#include "core/str.h"
#include "core/log.h"

#include <limits.h>


#define STRUCTS_DIAGNOSTIC

#include <time.h>
/**
 * Diagnostic.
 */
#ifdef STRUCTS_DIAGNOSTIC

#define MAX_DIAGNOSTICS 32

static Structs_Diagnostic diagnostics[MAX_DIAGNOSTICS];
static s64 diagnostic_count = 0;
static Operation_Flag diagnostic_allowed_flags = ALLOC | RESIZE | ADD | SUBTRACT | CLEAR | FREE;

static bool attach_next = false;
static char *attach_next_name = NULL;
static FILE *attach_next_output = NULL;

void diagnostic_set_allowed_flags(Operation_Flag flags) {
    diagnostic_allowed_flags = flags;
}

void diagnostic_attach(char *attach_name, FILE *stream) {
    attach_next = true;
    attach_next_name = attach_name;
    attach_next_output = stream;
}

// @Important: Right we just assume pointer is 64 bit.
void diagnostic_write(s64 id) {
    if (diagnostics[id].operation & diagnostic_allowed_flags) {
        switch (diagnostics[id].type) {
            case STRUCTS_BUFFER:
                fprintf(diagnostics[id].output, "Buffer,%s,%llu,%llu,0x%016llx,", diagnostics[id].name, diagnostics[id].timestamp, diagnostics[id].size, diagnostics[id].allocation);
                break;
            case STRUCTS_ARRAY_LIST:
                fprintf(diagnostics[id].output, "Array List,%s,%llu,%llu,0x%016llx,%u,%u,%u,", diagnostics[id].name, diagnostics[id].timestamp, diagnostics[id].size, diagnostics[id].allocation, diagnostics[id].t_array_list.capacity, diagnostics[id].t_array_list.length, diagnostics[id].t_array_list.item_size);
                break;
        }

        switch (diagnostics[id].operation) {
            case ALLOC:
                fprintf(diagnostics[id].output, "ALLOC\n");
                break;
            case RESIZE:
                fprintf(diagnostics[id].output, "RESIZE\n");
                break;
            case ADD:
                fprintf(diagnostics[id].output, "ADD\n");
                break;
            case SUBTRACT:
                fprintf(diagnostics[id].output, "SUBTRACT\n");
                break;
            case CLEAR:
                fprintf(diagnostics[id].output, "CLEAR\n");
                break;
            case FREE:
                fprintf(diagnostics[id].output, "FREE\n");
                break;
        }
    }
}

s64 diagnostic_find_id(void *allocation) {
    for (s64 i = 0; i < diagnostic_count; i++) {
        if (diagnostics[i].allocation == allocation) {
            return i;
        }
    }
    return -1;
}

#else

void diagnostic_attach() {
    LOG_ERROR("Structs Diagnostics are not defined. Cannot attach diagnostic.");
}

void diagnostic_set_allowed_flags(Operation_Flag flags) {
    LOG_ERROR("Structs Diagnostics are not defined. Cannot set diagnostic allowed flags.");
}

#endif



/**
 * Data strucutres.
 */

typedef struct buffer_data_struct_header {
    Allocator *allocator;
} Buffer_Data_Struct_Header;

void *buffer_data_struct_make(u32 size, u32 header_size, Allocator *allocator) {
    void *data = allocator_alloc(allocator, size + header_size + sizeof(Buffer_Data_Struct_Header));
    ((Buffer_Data_Struct_Header *)data)->allocator = allocator;

    if (data == NULL) {
        LOG_ERROR("Couldn't allocate memory of size: %llu bytes, for the buffer data structure.", size + header_size + sizeof(Buffer_Data_Struct_Header));
        return NULL;
    }


#ifdef STRUCTS_DIAGNOSTIC
    if (attach_next) {
        if (attach_next_name == NULL) {
            LOG_ERROR("Structs Diagnostic: couldn't attach, no name specified.");
        }
        else if (diagnostic_count >= MAX_DIAGNOSTICS) {
            LOG_ERROR("Structs Diagnostic: cannot attach diagnostic, max limit of %d has been reached.", MAX_DIAGNOSTICS);
        } else {
            if (attach_next_output == NULL) {
                attach_next_output = stdout;
            }

            diagnostics[diagnostic_count] = (Structs_Diagnostic) {
                .output = attach_next_output,
                    .type = STRUCTS_BUFFER,
                    .name = attach_next_name,
                    .timestamp = get_time_ns(),
                    .size = size + header_size + sizeof(Buffer_Data_Struct_Header),
                    .allocation = data,
                    .operation = ALLOC,
            };

            diagnostic_write(diagnostic_count);


            attach_next = false;
            attach_next_name = NULL;
            attach_next_output = NULL;
            diagnostic_count++;

            LOG_INFO("Structs Diagnostic: succesfully attached to allocated buffer.");
        }
    }
#endif


    return data + header_size + sizeof(Buffer_Data_Struct_Header);
}

void *buffer_data_struct_resize(void *data, u32 new_size, u32 header_size) {
    Allocator *allocator = ((Buffer_Data_Struct_Header *)(data - header_size - sizeof(Buffer_Data_Struct_Header)))->allocator;

#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(data - header_size - sizeof(Buffer_Data_Struct_Header)));
#endif


    data = allocator_re_alloc(allocator, data - header_size - sizeof(Buffer_Data_Struct_Header), new_size + header_size + sizeof(Buffer_Data_Struct_Header));

    if (data == NULL) {
        LOG_ERROR("Couldn't reallocate memory of size: %llu bytes, for the buffer data structure.", new_size + header_size + sizeof(Buffer_Data_Struct_Header));
        return NULL;
    }

#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_BUFFER) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].size = new_size + header_size + sizeof(Buffer_Data_Struct_Header);
        diagnostics[diagnostic_id].allocation = data;
        diagnostics[diagnostic_id].operation = RESIZE;

        diagnostic_write(diagnostic_id);
    }
#endif


    return data + header_size + sizeof(Buffer_Data_Struct_Header);
}

void buffer_data_struct_free(void *data, u32 header_size) {

#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(data - header_size - sizeof(Buffer_Data_Struct_Header)));
#endif

    Allocator *allocator = ((Buffer_Data_Struct_Header *)(data - header_size - sizeof(Buffer_Data_Struct_Header)))->allocator;
    allocator_free(allocator, data - header_size - sizeof(Buffer_Data_Struct_Header));

#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_BUFFER) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].size = 0;
        diagnostics[diagnostic_id].allocation = NULL;
        diagnostics[diagnostic_id].operation = FREE;

        diagnostic_write(diagnostic_id);
    }
#endif

}

/**
 * Array list.
 */

void *_array_list_make(u32 item_size, u32 capacity, Allocator *allocator) {

#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = -1;
    if (attach_next) {
        if (attach_next_name == NULL) {
            LOG_ERROR("Structs Diagnostic: couldn't attach, no name specified.");
        }
        else if (diagnostic_count >= MAX_DIAGNOSTICS) {
            LOG_ERROR("Structs Diagnostic: cannot attach diagnostic, max limit of %d has been reached.", MAX_DIAGNOSTICS);
        } else {
            if (attach_next_output == NULL) {
                attach_next_output = stdout;
            }

            diagnostics[diagnostic_count] = (Structs_Diagnostic) {
                    .output = attach_next_output,
                    .type = STRUCTS_ARRAY_LIST,
                    .name = attach_next_name,
                    .timestamp = get_time_ns(),
            };

            diagnostic_id = diagnostic_count;

            attach_next = false;
            attach_next_name = NULL;
            attach_next_output = NULL;
            diagnostic_count++;

        }
    }
#endif

    Array_List_Header *ptr = buffer_data_struct_make(item_size * capacity, sizeof(Array_List_Header), allocator) - sizeof(Array_List_Header);

    if (ptr == NULL) {
        LOG_ERROR("Couldn't allocate more memory of size: %lld bytes, for the array list.", item_size * capacity + sizeof(Array_List_Header));
        return NULL;
    }

    ptr->capacity = capacity;
    ptr->item_size = item_size;
    ptr->length = 0;
    

#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1) {
        diagnostics[diagnostic_id].size = item_size * capacity + sizeof(Array_List_Header) + sizeof(Buffer_Data_Struct_Header),
        diagnostics[diagnostic_id].allocation = (void *)ptr - sizeof(Buffer_Data_Struct_Header),
        diagnostics[diagnostic_id].t_array_list.capacity = ptr->capacity,
        diagnostics[diagnostic_id].t_array_list.length = ptr->length,
        diagnostics[diagnostic_id].t_array_list.item_size = ptr->item_size,
        diagnostics[diagnostic_id].operation = ALLOC;

        diagnostic_write(diagnostic_id);

        LOG_INFO("Structs Diagnostic: succesfully attached to allocated array list.");
    }
#endif


    // @Important: Because ptr is of type "Array_List_Header *", compiler will automatically translate "ptr + 1" to "(void*)(ptr) + sizeof(Array_List_Header)".

    return ptr + 1;
}

u32 _array_list_length(void *list) {
    return ((Array_List_Header *)(list - sizeof(Array_List_Header)))->length;
}

u32 _array_list_capacity(void *list) {
    return ((Array_List_Header *)(list - sizeof(Array_List_Header)))->capacity;
}

u32 _array_list_item_size(void *list) {
    return ((Array_List_Header *)(list - sizeof(Array_List_Header)))->item_size;
}

void _array_list_free(void **list) {

#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(*list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header)));
#endif

    buffer_data_struct_free(*list, sizeof(Array_List_Header));
    *list = NULL;


#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_ARRAY_LIST) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].size = 0;
        diagnostics[diagnostic_id].allocation = NULL;
        diagnostics[diagnostic_id].t_array_list.capacity = 0;
        diagnostics[diagnostic_id].t_array_list.length = 0;
        diagnostics[diagnostic_id].t_array_list.item_size = 0;
        diagnostics[diagnostic_id].operation = FREE;

        diagnostic_write(diagnostic_id);
    }
#endif
}

void _array_list_resize_to_fit(void **list, u32 requiered_length) {
    Array_List_Header *header = *list - sizeof(Array_List_Header);



    if (requiered_length > header->capacity) {
        u32 capacity_multiplier = (u32)powf(2.0f, (float)((u32)(log2f((float)requiered_length / (float)header->capacity)) + 1));


#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(*list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header)));
#endif

        
        *list = buffer_data_struct_resize(*list, header->capacity * capacity_multiplier * header->item_size, sizeof(Array_List_Header));
        header = *list - sizeof(Array_List_Header); // @Important: Resizing perfomed above changes the pointer to the list, so it is neccessary to reassign header ptr again, otherwise segfault occure.
        header->capacity *= capacity_multiplier; // @Important: Using "buffer_data_struct_resize" will not update capacity in the header, because this function is only designed to only resize the whole data structure, there for it is needed to manually set capacity to the right value, which was intended.

#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_ARRAY_LIST) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].size = header->capacity * header->item_size + sizeof(Buffer_Data_Struct_Header) + sizeof(Array_List_Header);
        diagnostics[diagnostic_id].allocation = (void *)(*list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header));
        diagnostics[diagnostic_id].t_array_list.capacity = header->capacity;
        diagnostics[diagnostic_id].operation = RESIZE;

        diagnostic_write(diagnostic_id);
    }
#endif
    }

    if (header->capacity * header->item_size < requiered_length * header->item_size) {
        LOG_ERROR("List capacity size is less than requiered length size after being resized, capacity size: %d, size of requiered length: %d.", header->capacity * header->item_size, requiered_length * header->item_size);
    }


}

u32 _array_list_next_index(void **list) {
#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(*list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header)));
#endif

    Array_List_Header *header = *list - sizeof(Array_List_Header);
    header->length += 1;

#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_ARRAY_LIST) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].t_array_list.length = header->length;
        diagnostics[diagnostic_id].operation = ADD;

        diagnostic_write(diagnostic_id);
    }
#endif

    return header->length - 1;
}

u32 _array_list_append_multiple(void **list, void *items, u32 count) {
#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(*list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header)));
#endif

    Array_List_Header *header = *list - sizeof(Array_List_Header);
    
    u32 requiered_length = header->length + count;

    _array_list_resize_to_fit(list, requiered_length);
    header = *list - sizeof(Array_List_Header); // @Important: Resizing perfomed above might change the pointer to the list, so it is neccessary to reassign header ptr again, otherwise segfault occure.

    (void)memcpy(*list + header->length * header->item_size, items, header->item_size * count);
    header->length += count;


#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_ARRAY_LIST) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].t_array_list.length = header->length;
        diagnostics[diagnostic_id].operation = ADD;

        diagnostic_write(diagnostic_id);
    }
#endif

    return header->length - count;
}

void _array_list_pop(void *list, u32 count) {

#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header)));
#endif

    ((Array_List_Header *)(list - sizeof(Array_List_Header)))->length -= count;

#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_ARRAY_LIST) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].t_array_list.length = ((Array_List_Header *)(list - sizeof(Array_List_Header)))->length;
        diagnostics[diagnostic_id].operation = SUBTRACT;

        diagnostic_write(diagnostic_id);
    }
#endif
}

void _array_list_clear(void *list) {
#ifdef STRUCTS_DIAGNOSTIC
    s64 diagnostic_id = diagnostic_find_id((void *)(list - sizeof(Array_List_Header) - sizeof(Buffer_Data_Struct_Header)));
#endif
    ((Array_List_Header *)(list - sizeof(Array_List_Header)))->length = 0;
#ifdef STRUCTS_DIAGNOSTIC
    if (diagnostic_id != -1 && diagnostics[diagnostic_id].type == STRUCTS_ARRAY_LIST) {
        diagnostics[diagnostic_id].timestamp = get_time_ns();
        diagnostics[diagnostic_id].t_array_list.length = 0;
        diagnostics[diagnostic_id].operation = CLEAR;

        diagnostic_write(diagnostic_id);
    }
#endif
}

void _array_list_unordered_remove(void *list, u32 index) {
    Array_List_Header *header = list - sizeof(Array_List_Header);
    memcpy(list + index * header->item_size, list + (header->length - 1) * header->item_size, header->item_size);
    _array_list_pop(list, 1);
}




/**
 * Looped Array.
 */

void *_looped_array_make(u32 item_size, u32 capacity, Allocator *allocator) {
    Looped_Array_Header *ptr = buffer_data_struct_make(item_size * capacity, sizeof(Looped_Array_Header), allocator) - sizeof(Looped_Array_Header);

    if (ptr == NULL) {
        printf_err("Couldn't allocate more memory of size: %lld bytes, for the looped array.\n", item_size * capacity + sizeof(Looped_Array_Header));
        return NULL;
    }

    ptr->capacity = capacity;
    ptr->item_size = item_size;
    ptr->length = 0;
    ptr->index = 0;
    
    // @Important: Because ptr is of type "Looped_Array_Header *", compiler will automatically translate "ptr + 1" to "(void*)(ptr) + sizeof(Looped_Array_Header)".
    return ptr + 1;
}

u32 _looped_array_length(void *list) {
    return ((Looped_Array_Header *)(list - sizeof(Looped_Array_Header)))->length;
}

u32 _looped_array_index(void *list) {
    return ((Looped_Array_Header *)(list - sizeof(Looped_Array_Header)))->index;
}

u32 _looped_array_capacity(void *list) {
    return ((Looped_Array_Header *)(list - sizeof(Looped_Array_Header)))->capacity;
}

u32 _looped_array_item_size(void *list) {
    return ((Looped_Array_Header *)(list - sizeof(Looped_Array_Header)))->item_size;
}

void _looped_array_free(void **list) {
    buffer_data_struct_free(*list, sizeof(Looped_Array_Header));
    *list = NULL;
}

// void _looped_array_resize_to_fit(void **list, u32 requiered_length) {
//     Looped_Array_Header *header = *list - sizeof(Looped_Array_Header);
// 
//     if (requiered_length > header->capacity) {
//         u32 capacity_multiplier = (u32)powf(2.0f, (float)((u32)(log2f((float)requiered_length / (float)header->capacity)) + 1));
// 
//         
//         *list = buffer_data_struct_resize(*list, header->capacity * capacity_multiplier * header->item_size, sizeof(Looped_Array_Header));
//         header = *list - sizeof(Looped_Array_Header); // @Important: Resizing perfomed above changes the pointer to the list, so it is neccessary to reassign header ptr again, otherwise segfault occure.
//         header->capacity *= capacity_multiplier; // @Important: Using "buffer_data_struct_resize" will not update capacity in the header, because this function is only designed to only resize the whole data structure, there for it is needed to manually set capacity to the right value, which was intended.
//     }
// 
//     if (header->capacity * header->item_size < requiered_length * header->item_size) {
//         printf_err("List capacity size is less than requiered length size after being resized, capacity size: %d, size of requiered length: %d.\n", header->capacity * header->item_size, requiered_length * header->item_size);
//     }
// }

u32 _looped_array_next_index(void *list) {
    Looped_Array_Header *header = list - sizeof(Looped_Array_Header);
    u32 next_index = header->index;
    header->index = (header->index + 1) % header->capacity;
    header->length = header->length < header->capacity ? header->length + 1 : header->capacity;
    return next_index;
}

u32 _looped_array_map_index(void *list, u32 index) {
    Looped_Array_Header *header = list - sizeof(Looped_Array_Header);

    return (header->index - (header->length - index) + header->capacity) % header->capacity;
}

void _looped_array_pop(void *list, u32 count) {
    Looped_Array_Header *header = list - sizeof(Looped_Array_Header);
    header->length -= count;
    header->index = (header->index - count + header->capacity) % header->capacity;
}

void _looped_array_clear(void *list) {
    ((Looped_Array_Header *)(list - sizeof(Looped_Array_Header)))->length = 0;
    ((Looped_Array_Header *)(list - sizeof(Looped_Array_Header)))->index = 0;
}

// @Refactor: Wrong implementation.
void _looped_array_unordered_remove(void *list, u32 index) {
    Looped_Array_Header *header = list - sizeof(Looped_Array_Header);
    memcpy(list + index * header->item_size, list + (header->length - 1) * header->item_size, header->item_size);
    _looped_array_pop(list, 1);
}




/**
 * Hash Table. 
 */

Hash_Table_Slot *_hash_table_get_slot(void **table, u32 index) {
    return *table + hash_table_header(table)->capacity * hash_table_header(table)->item_size + index * sizeof(Hash_Table_Slot);
}

/**
 * Internal function.
 * Returns index of the corresponding key by calculating hash.
 * @Careful: Doesn't perfom any slot checks.
 */
u32 hash_table_hash_index_of(void **table, void *key, u32 key_size) {
    Hash_Table_Header *header = hash_table_header(table);
    return header->hash_func(key_size, key) % header->capacity;
}

/**
 * Internal function.
 * Sets all occupied slots to depricated.
 */
void hash_table_depricate_slots(void **table) {
    u32 cap = hash_table_capacity(table);
    Hash_Table_Slot *slot = NULL;
    for (u32 i = 0; i < cap; i++) {
        slot = _hash_table_get_slot(table, i);
        if (slot->state == SLOT_OCCUPIED) {
            slot->state = SLOT_DEPRICATED;
        }
    }
}

/**
 * @Internal function.
 */
void hash_table_print_slot(void *item, u32 item_size, Hash_Table_Slot *slot) {
    
    // Print the item in hex based on item_size.
    printf("Item: 0x");
    for (u32 i = 0; i < item_size; i++) {
        (void)printf("%02x", *((u8 *)item + i));  // Print each byte of the item.
    }

    // Print the state and key_size as hex.
    (void)printf(" | State: 0x%02x | Key Size: 0x%08x | Key Ptr: 0x%16p -> ", slot->state, slot->key.length, slot->key.data);

    if (slot->state == SLOT_OCCUPIED) {
        
        // Print key itself
        (void)printf("%.*s", slot->key.length, slot->key.data);
    }
    (void)printf("\n");
}

/**
 * Internal function.
 * @Recursion: Recursivly readresses slots if they are depricated.
 * Returns true, if it succesfully readressed a slot.
 */
bool hash_table_readress(void **table, u32 index) {
    Hash_Table_Header *header = hash_table_header(table);
    Hash_Table_Slot *target_slot = _hash_table_get_slot(table, index);

    u32 new_index = hash_table_hash_index_of(table, target_slot->key.data, target_slot->key.length);
    target_slot->state = SLOT_EMPTY;

    Hash_Table_Slot *slot = NULL;
    for (u32 i = 0; i < header->capacity; i++) {
        slot = _hash_table_get_slot(table, (new_index + i) % header->capacity);
        if (slot->state == SLOT_EMPTY) {
            // Copy data to a new slot.
            slot->state = SLOT_OCCUPIED;

            slot->key.length = target_slot->key.length;
            slot->key.data = target_slot->key.data;

            memmove(*table + ((new_index + i) % header->capacity) * header->item_size, *table + index * header->item_size, header->item_size);

            return true;
        }
        else if (slot->state == SLOT_DEPRICATED && hash_table_readress(table, (new_index + i) % header->capacity)) {

            // @Important: There is an additional is SLOT_OCCUPIED check, because hash_table_readress can possbily readress slot to the same index as it was before, therefore additional check is needed.
            if (slot->state != SLOT_OCCUPIED) {
                // Copy data to a new slot.
                slot->state = SLOT_OCCUPIED;

                slot->key.length = target_slot->key.length;
                slot->key.data = target_slot->key.data;

                memmove(*table + ((new_index + i) % header->capacity) * header->item_size, *table + index * header->item_size, header->item_size);

                return true;
            }
        }
    }

    
    printf_err("Couldn't find new free hash slot when readressing.\n");
    return false;

}

void *_hash_table_make(u32 item_size, u32 capacity, Allocator *allocator) {
    u32 table_size = (sizeof(Hash_Table_Slot) + item_size) * capacity;
    void *buffer = buffer_data_struct_make(table_size, sizeof(Hash_Table_Header), allocator);

    if (buffer == NULL) {
        printf_err("Couldn't allocate more memory of size: %lld bytes, for the hash table.\n", item_size * capacity + sizeof(Hash_Table_Header));
        return NULL;
    }

    u8 *keys = array_list_make(u8, capacity * 8, allocator);    

    if (keys == NULL) {
        printf_err("Couldn't allocate more memory of size: %u bytes, for the keys for the hash table.\n", item_size * capacity * 8);
        buffer_data_struct_free(buffer, sizeof(Hash_Table_Header));
        return NULL;
    }

    Hash_Table_Header *header = buffer - sizeof(Hash_Table_Header);
    header->capacity = capacity;
    header->item_size = item_size;
    header->count = 0;
    header->hash_func = hashf;
    header->keys = keys;

    // Set slots to SLOT_EMPTY.
    for (u32 i = 0; i < header->capacity; i++) {
        _hash_table_get_slot(&buffer, i)->state = SLOT_EMPTY;
    }
    
    // @Important: Because header is of type "Hash_Table_Header *", compiler will automatically translate "header + 1" to "(void *)(header) + sizeof(Hash_Table_Header)".
    return header + 1;
}

u32 _hash_table_count(void *table) {
    return ((Hash_Table_Header *)(table - sizeof(Hash_Table_Header)))->count;
}

u32 _hash_table_capacity(void *table) {
    return ((Hash_Table_Header *)(table - sizeof(Hash_Table_Header)))->capacity;
}

u32 _hash_table_item_size(void *table) {
    return ((Hash_Table_Header *)(table - sizeof(Hash_Table_Header)))->item_size;
}

void _hash_table_free(void **table) {
    array_list_free(&(hash_table_header(table)->keys));
    buffer_data_struct_free(*table, sizeof(Hash_Table_Header));
    *table = NULL;
}

void _hash_table_resize_to_fit(void **table, u32 requiered_length) {
    Hash_Table_Header *header = hash_table_header(table);

    if (requiered_length > header->capacity) {
        // Before resizing, depricate slots.
        hash_table_depricate_slots(table);

        u32 capacity_multiplier = (u32)powf(2.0f, (float)((u32)(log2f((float)requiered_length / (float)header->capacity)) + 1));

        *table = buffer_data_struct_resize(*table, header->capacity * capacity_multiplier * (header->item_size + sizeof(Hash_Table_Slot)), sizeof(Hash_Table_Header));
        header = hash_table_header(table); // @Important: Resizing perfomed above changes the pointer to the table, so it is neccessary to reassign header ptr again, otherwise segfault occure.
        header->capacity *= capacity_multiplier; // @Important: Using "buffer_data_struct_resize" will not update capacity in the header, because this function is only designed to only resize the whole data structure, there for it is needed to manually set capacity to the right value, which was intended.
        
        /**
         * After resize, all data is copied and buffer is expanded to the right.
         * But due to the nature of hash table structure the slot area of the buffer would not be properly shifter in the resulting array.
         * For example diagrams (NOT TO SCALE):
         *
         *      BEFORE:
         *                  
         *                  |-----------------------------Buffer-Capacity-4-------------------------------|
         *                  |                                                                             |
         *                  |------Array-of-Data-------|-----------------Array-of-Slots-------------------|
         *      Indicies:   | 0     1     2     3      | 0           1           2           3            |
         *      Data:       | [1111][1111][1111][____] | [10][0x2342][01][0x2344][11][0x2348][__][______] |
         *                  ^
         *                  |
         *                  *table
         *
         *
         *      AFTER RESIZE (BAD):
         *                  
         *                  |-------------------------------------------------------------------Buffer-Capacity-8-----------------------------------------------------------------------|
         *                  |                                                                                                                                                           |
         *                  |------Array-of-Data-------|-----------------Array-of-Slots-------------------|--------------------------|--------------------------------------------------|
         *      Indicies:   | 0     1     2     3      | 0           1           2           3            |                          |                                                  |
         *      Data:       | [1111][1111][1111][____] | [10][0x2342][01][0x2344][11][0x2348][__][______] | [____][____][____][____] | [__][______][__][______][__][______][__][______] |
         *                  ^
         *                  |
         *                  *table
         *
         *
         *      WHAT IS EXPECTED / NEEDED (GOOD):
         *                  
         *                  |----------------------------------------------------------------Buffer-Capacity-8--------------------------------------------------------------------|
         *                  |                                                                                                                                                     |
         *                  |------------------Array-of-Data-------------------|------------------------------------------Array-of-Slots------------------------------------------|
         *      Indicies:   | 0     1     2     3     4     5     6     7      | 0           1           2           3           4           5           6           7            |
         *      Data:       | [1111][1111][1111][____][____][____][____][____] | [10][0x2342][01][0x2344][11][0x2348][__][______][__][______][__][______][__][______][__][______] |
         *                  ^
         *                  |
         *                  *table
         *
         * To achieve this good layout, it is needed to shift array of slots to the right.
         * Should be done with memmove cause, destination and source can overlap.
         */

        memmove(*table + header->capacity * header->item_size, *table + (header->capacity / capacity_multiplier) * header->item_size, (header->capacity / capacity_multiplier) * sizeof(Hash_Table_Slot));

        // Set new slots to SLOT_EMPTY.
        for (u32 i = header->capacity / capacity_multiplier; i < header->capacity; i++) {
            _hash_table_get_slot(table, i)->state = SLOT_EMPTY;
        }

        // Readress slots after resizing.
        for (u32 i = 0; i < header->capacity; i++) {
            if (_hash_table_get_slot(table, i)->state == SLOT_DEPRICATED) {
                (void)hash_table_readress(table, i);
            }
        }
    }

    if (header->capacity * (header->item_size + sizeof(Hash_Table_Slot)) < requiered_length * (header->item_size + sizeof(Hash_Table_Slot))) {
        printf_err("Table capacity size is less than requiered length size after being resized, capacity size: %llu, size of requiered length: %llu.\n", header->capacity * (header->item_size + sizeof(Hash_Table_Slot)), requiered_length * (header->item_size + sizeof(Hash_Table_Slot)));
    }
}

u32 _hash_table_push_key(void **table, u32 key_size, void *key) {
    u32 index = hash_table_hash_index_of(table, key, key_size);
    Hash_Table_Header *header = hash_table_header(table);

    Hash_Table_Slot *slot = NULL;
    for (u32 i = 0; i < header->capacity; i++) {
        slot = _hash_table_get_slot(table, (index + i) % header->capacity);
        if (slot->state == SLOT_EMPTY) {
            // Write all data to hash table by corresponding index.
            slot->state = SLOT_OCCUPIED;

            slot->key.length = key_size;
            
            /**
             * @Important: "array_list_append_multiple" is not inlined because it might change the "header->keys" value when resizing array list.
             * So it is neccessary to call this macro like function in a separate line, otherwise undefined behaviour will occure.
             */
            u32 key_index = array_list_append_multiple(&(header->keys), key, key_size);
            slot->key.data = header->keys + key_index;

            header->count++;
           
            return (index + i) % header->capacity;
        }
        else if (slot->key.length == key_size && !memcmp(slot->key.data, key, key_size)) {
            // Key already exists, return index of that slot.
            return (index + i) % header->capacity;
        }
    }

    
    printf_err("Couldn't find free hash table slot for the new key.\n");
    return UINT_MAX;
}

void *_hash_table_get(void **table, u32 key_size, void *key) {
    u32 index = hash_table_hash_index_of(table, key, key_size);
    Hash_Table_Header *header = hash_table_header(table);

    Hash_Table_Slot *slot = NULL;
    for (u32 i = 0; i < header->capacity; i++) {
        slot = _hash_table_get_slot(table, (index + i) % header->capacity);
        if (slot->state == SLOT_EMPTY) {
            return NULL;
        }
        else if (slot->key.length == key_size && !memcmp(slot->key.data, key, key_size)) {
            // Right key is found, return.
            return *table + ((index + i) % header->capacity) * header->item_size;
        }
    }

    return NULL;
}

void _hash_table_remove(void **table, u32 key_size, void *key) {
    u32 index = hash_table_hash_index_of(table, key, key_size);
    Hash_Table_Header *header = hash_table_header(table);

    Hash_Table_Slot *slot = NULL;
    for (u32 i = 0; i < header->capacity; i++) {
        slot = _hash_table_get_slot(table, (index + i) % header->capacity);
        if (slot->state == SLOT_EMPTY) {
            return;
        }
        else if (slot->key.length == key_size && !memcmp(slot->key.data, key, key_size)) {
            // Right key is found.
            // @Incomplete: Doesn't delete key.
            header->count--;
            slot->state = SLOT_EMPTY;
            return;
        }
    }
}


u32 hashf(u32 key_size, void *key) {
    if (key == NULL || key_size == 0) {
        printf_err("Couldn't hash a NULL or 0 sized key.\n");
        return 0;
    }
    if (key_size < 2) {
        return *(u8 *)(key);
    }
    
    u32 hash = 0;
    u8 *hash_ptr = (u8 *)&(hash);
    hash_ptr[0] = *((u8 *)(key) + 0);
    hash_ptr[1] = *((u8 *)(key) + 1);
    hash_ptr[2] = *((u8 *)(key) + key_size - 1);
    hash_ptr[3] = *((u8 *)(key) + key_size - 2);

    return hash;
}

void hash_table_print(void **table) {
    Hash_Table_Header *header = hash_table_header(table);

    printf("\n--------\tHash Table\t--------\n");
    for (u32 i = 0; i < header->capacity; i++) {
        hash_table_print_slot(*table + i * header->item_size, header->item_size, _hash_table_get_slot(table, i));
    }
}


