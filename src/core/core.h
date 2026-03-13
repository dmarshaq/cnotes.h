#ifndef CORE_H
#define CORE_H

#define WINDOWS 1
#define LINUX   2

#ifndef OS
#   if _WIN32
#       define OS WINDOWS
#   endif
#endif



#include "core/type.h"

/**
 * Debug.
 */

#include <stdio.h>
#include <stdbool.h>

extern const char *debug_error_str;
extern const char *debug_warning_str;
extern const char *debug_ok_str;


#define printf_err(format, ...)                 (void)fprintf(stderr, "%s " format, debug_error_str, ##__VA_ARGS__)

#ifdef IGNORE_WARNING

#define printf_warning(format, ...)

#else

#define printf_warning(format, ...)             (void)fprintf(stderr, "%s " format, debug_warning_str, ##__VA_ARGS__)

#endif

#define printf_ok(format, ...)                  (void)fprintf(stderr, "%s " format, debug_ok_str, ##__VA_ARGS__)


#define TODO(message)                           printf_err("%s:%d TODO: %s\n", __FILE__, __LINE__, message); exit(1)



/**
 * Allocators.
 * @Todo: Refactor allocators, avoid standarlized allocator structure. One interface per allocator.
 *
 * Arena allocator.
 */


#include <stdlib.h>

#define KB 1024

typedef struct allocator_header {
    u64 capacity;
    u64 size_filled;
} Allocator_Header;

typedef void *(*Allocate)(Allocator_Header *header, u64 size);
typedef void *(*Zero_Allocate)(Allocator_Header *header, u64 size);
typedef void *(*Re_Allocate)(Allocator_Header *header, void *ptr, u64 size);
typedef void  (*Free)(Allocator_Header *header, void *ptr);

typedef struct allocator {
    Allocator_Header    *ptr;
    Allocate            alc_alloc;
    Zero_Allocate       alc_zero_alloc;
    Re_Allocate         alc_re_alloc;
    Free                alc_free;
} Allocator;

// Std.
extern Allocator std_allocator;

// Allocator interface.
void *allocator_alloc(Allocator *allocator, u64 size);
void *allocator_zero_alloc(Allocator *allocator, u64 size);
void *allocator_re_alloc(Allocator *allocator, void *ptr, u64 size);
void  allocator_free(Allocator *allocator, void *ptr);




/**
 * Time.
 * @Todo: Remove this somewhere else, definitly not part of a core.
 */
typedef struct time_info {
    u32 current_time;
    u32 delta_time_milliseconds;
    float delta_time;

    float delta_time_multi;
    u32 time_slow_factor;

    u32 last_update_time;
    u32 accumilated_time;
    u32 update_step_time;
} Time_Info;


typedef struct time_interpolator {
    float duration;
    float elapsed_t;
} T_Interpolator;

#define ti_make(duration)                   ((T_Interpolator) { duration, 0.0f })

void ti_update(T_Interpolator *interpolator, float delta_time);
float ti_elapsed_percent(T_Interpolator *interpolator);
bool ti_is_complete(T_Interpolator *interpolator);
void ti_reset(T_Interpolator *interpolator);




/**
 * Returns time in nano seconds since program start.
 */
u64 get_time_ns();




#endif
