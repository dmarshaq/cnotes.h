#ifndef STRUCTS_H
#define STRUCTS_H

#include "core/core.h"
#include "core/str.h"
#include "core/type.h"

/**
 * Array list.
 */

#define array_list_make(type, capacity, ptr_allocator)              (type *)_array_list_make(sizeof(type), capacity, ptr_allocator)

#define array_list_length(ptr_list)                                 _array_list_length((void *)*ptr_list)
#define array_list_capacity(ptr_list)                               _array_list_capacity((void *)*ptr_list)
#define array_list_item_size(ptr_list)                              _array_list_item_size((void *)*ptr_list)

#define array_list_append(ptr_list, item)                           _array_list_resize_to_fit((void **)(ptr_list), array_list_length(ptr_list) + 1); (*ptr_list)[_array_list_next_index((void **)(ptr_list))] = item
#define array_list_append_multiple(ptr_list, item_arr, count)       _array_list_append_multiple((void **)ptr_list, (void *)item_arr, count)
#define array_list_pop(ptr_list)                                    _array_list_pop((void *)*ptr_list, 1)
#define array_list_pop_multiple(ptr_list, count)                    _array_list_pop((void *)*ptr_list, count)
#define array_list_clear(ptr_list)                                  _array_list_clear((void *)*ptr_list)
#define array_list_unordered_remove(ptr_list, index)                _array_list_unordered_remove((void *)*ptr_list, index)
#define array_list_free(ptr_list)                                   _array_list_free((void **)ptr_list)

typedef struct array_list_header {
    u32 capacity;
    u32 length;
    u32 item_size;
} Array_List_Header;

void *_array_list_make(u32 item_size, u32 initial_capacity, Allocator *allocator);
u32   _array_list_length(void *list);
u32   _array_list_capacity(void *list);
u32   _array_list_item_size(void *list);
void  _array_list_resize_to_fit(void **list, u32 requiered_length);
u32   _array_list_next_index(void **list);
u32   _array_list_append_multiple(void **list, void *items, u32 count);
void  _array_list_pop(void *list, u32 count);
void  _array_list_clear(void *list);
void  _array_list_unordered_remove(void *list, u32 index);
void  _array_list_free(void **list);



/**
 * Looped array.
 */
#define looped_array_make(type, capacity, ptr_allocator)              (type *)_looped_array_make(sizeof(type), capacity, ptr_allocator)

#define looped_array_length(ptr_list)                                 _looped_array_length((void *)*ptr_list)
#define looped_array_capacity(ptr_list)                               _looped_array_capacity((void *)*ptr_list)
#define looped_array_item_size(ptr_list)                              _looped_array_item_size((void *)*ptr_list)
#define looped_array_append(ptr_list, item)                           (*ptr_list)[_looped_array_next_index((void *)(*ptr_list))] = item
#define looped_array_pop(ptr_list)                                    _looped_array_pop((void *)*ptr_list, 1)
#define looped_array_pop_multiple(ptr_list, count)                    _looped_array_pop((void *)*ptr_list, count)
#define looped_array_clear(ptr_list)                                  _looped_array_clear((void *)*ptr_list)
#define looped_array_get(ptr_list, index)                             (*ptr_list)[_looped_array_map_index((void *)(*ptr_list), index)]
// #define looped_array_unordered_remove(ptr_list, index)                _looped_array_unordered_remove((void *)*ptr_list, index)
#define looped_array_free(ptr_list)                                   _looped_array_free((void **)ptr_list)

typedef struct looped_array_header {
    u32 capacity;
    u32 index;
    u32 length;
    u32 item_size;
} Looped_Array_Header;

void *_looped_array_make(u32 item_size, u32 initial_capacity, Allocator *allocator);
u32   _looped_array_length(void *list);
u32   _looped_array_capacity(void *list);
u32   _looped_array_item_size(void *list);
u32   _looped_array_next_index(void *list);
u32   _looped_array_map_index(void *list, u32 index);
void  _looped_array_pop(void *list, u32 count);
void  _looped_array_clear(void *list);
// void  _looped_array_unordered_remove(void *list, u32 index);
void  _looped_array_free(void **list);




/**
 * Hash table.
 */


#define hash_table_make(type, capacity, allocator_ptr)              (type *)_hash_table_make(sizeof(type), capacity, allocator_ptr) 

#define hash_table_count(ptr_list)                                  _hash_table_count((void *)*ptr_list)
#define hash_table_capacity(ptr_list)                               _hash_table_capacity((void *)*ptr_list)
#define hash_table_item_size(ptr_list)                              _hash_table_item_size((void *)*ptr_list)

#define hash_table_put(ptr_table, item, ...)          _hash_table_resize_to_fit((void **)(ptr_table), hash_table_count(ptr_table) + 1); (*ptr_table)[_hash_table_push_key((void **)(ptr_table), __VA_ARGS__)] = item
#define hash_table_get(ptr_table, ...)                _hash_table_get((void **)(ptr_table), __VA_ARGS__)
#define hash_table_remove(ptr_table, ...)             _hash_table_remove((void **)(ptr_table), __VA_ARGS__) 
#define hash_table_free(ptr_table)                                  _hash_table_free((void **)(ptr_table))

typedef u32 (*Hashfunc)(u32, void *);

typedef struct hash_table_header {
    u32 capacity;
    u32 count;
    u32 item_size;
    Hashfunc hash_func;
    u8 *keys;
} Hash_Table_Header;


#define hash_table_header(table_ptr)        ((Hash_Table_Header *)(*(table_ptr) - sizeof(Hash_Table_Header)))

typedef enum hash_table_slot_state : u8 {
    SLOT_EMPTY      = 0x00,
    SLOT_OCCUPIED   = 0x01,
    SLOT_DEPRICATED = 0x02,
} Hash_Table_Slot_State;

typedef struct hash_table_slot {
    Hash_Table_Slot_State state;
    String key;
} Hash_Table_Slot;


void *_hash_table_make(u32 item_size, u32 initial_capacity, Allocator *allocator);
u32   _hash_table_count(void *table);
u32   _hash_table_capacity(void *table);
u32   _hash_table_item_size(void *table);
void  _hash_table_resize_to_fit(void **table, u32 requiered_length);
u32   _hash_table_push_key(void **table, u32 key_size, void *key);
void *_hash_table_get(void **table, u32 key_size, void *key);
void  _hash_table_remove(void **table, u32 key_size, void *key);
void  _hash_table_free(void **table);
Hash_Table_Slot *_hash_table_get_slot(void **table, u32 index);

u32 hashf(u32 key_size, void *key);

void hash_table_print(void **table);






// Diagnostic

typedef enum structs_type : u8 {
    STRUCTS_BUFFER,
    STRUCTS_ARRAY_LIST,
    STRUCTS_LOOPED_ARRAY,
    STRUCTS_HASH_TABLE,
} Structs_Type;


typedef enum operation_flag : u8 {
    ALLOC    = 0x01,
    RESIZE   = 0x02,
    ADD      = 0x04,
    SUBTRACT = 0x08,
    CLEAR    = 0x10,
    FREE     = 0x20,
} Operation_Flag;


typedef struct structs_diagnostic_buffer {
    
} Structs_Diagnostic_Buffer;


typedef struct structs_diagnostic_array_list {
    u32 capacity;
    u32 length;
    u32 item_size;
} Structs_Diagnostic_Array_List;

typedef struct structs_diagnostic_looped_array {

} Structs_Diagnostic_Looped_Array;

typedef struct structs_diagnostic_hash_table {

} Structs_Diagnostic_Hash_Table;


typedef struct structs_diagnostic {
    FILE *output;   // Not included in diagnostic data.
    
    Structs_Type type;
    char *name;
    u64 timestamp;
    u64 size;
    void *allocation;
    Operation_Flag operation;
    union {
        Structs_Diagnostic_Buffer t_buffer;
        Structs_Diagnostic_Array_List t_array_list;
        Structs_Diagnostic_Looped_Array t_looped_array;
        Structs_Diagnostic_Hash_Table t_hash_table;
    };
} Structs_Diagnostic;


/**
 * Will automatically attach diagnostic to view next allocated memory structure from this header.
 * Will output diagnostic everytime struct is modified into stream.
 */
void diagnostic_attach(char *attach_name, FILE *stream);

void diagnostic_set_allowed_flags(Operation_Flag flags);



#endif
