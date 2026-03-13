#include "core/core.h"
#include "core/type.h"
#include "core/mathf.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


/**
 * Debug.
 */

const char* debug_error_str = "\033[31m[ERROR]\033[0m";
const char* debug_warning_str = "\033[33m[WARNING]\033[0m";
const char* debug_ok_str = "\033[32m[OK]\033[0m";


/**
 * Allocator.
 */

void allocator_destroy(Allocator *allocator) {
    free(allocator->ptr);
    allocator->ptr = NULL;
    allocator->alc_alloc = NULL;
    allocator->alc_free = NULL;
}

// Std.
void *std_malloc(Allocator_Header *header, u64 size) {
    return malloc(size);
}

void *std_calloc(Allocator_Header *header, u64 size) {
    return calloc(1, size);
}

void *std_realloc(Allocator_Header *header, void *ptr, u64 size) {
    return realloc(ptr, size);
}

void std_free(Allocator_Header *header, void *ptr) {
    free(ptr);
}

Allocator std_allocator = (Allocator) {
    .ptr = NULL,
    .alc_alloc = std_malloc,
    .alc_zero_alloc = std_calloc,
    .alc_re_alloc = std_realloc,
    .alc_free = std_free,
};


// Allocator interface.
void *allocator_alloc(Allocator *allocator, u64 size) {
    if (allocator->alc_alloc == NULL) {
        printf_err("Allocator couldn't allocate memory since allocataion function is not defined for the allocator.\n");
        return NULL;
    }

    return allocator->alc_alloc(allocator->ptr, size);
}

void *allocator_zero_alloc(Allocator *allocator, u64 size) {
    if (allocator->alc_zero_alloc == NULL) {
        printf_err("Allocator couldn't zero allocate memory since zero allocataion function is not defined for the allocator.\n");
        return NULL;
    }

    return allocator->alc_zero_alloc(allocator->ptr, size);
}

void *allocator_re_alloc(Allocator *allocator, void *ptr, u64 size) {
    if (allocator->alc_re_alloc == NULL) {
        printf_err("Allocator couldn't reallocate memory since reallocataion function is not defined for the allocator.\n");
        return NULL;
    }

    return allocator->alc_re_alloc(allocator->ptr, ptr, size);
}


void allocator_free(Allocator *allocator, void *ptr) {
    if (allocator->alc_free == NULL) {
        printf_err("Allocator couldn't free memory since free function is not defined for the allocator.\n");
        return;
    }

    return allocator->alc_free(allocator->ptr, ptr);
}



/**
 * Time
 */

void ti_update(T_Interpolator *interpolator, float delta_time) {
    interpolator->elapsed_t += delta_time;

    if (interpolator->elapsed_t > interpolator->duration) {
        // Later there should different cases of how end of interpolation should be handled.
        interpolator->elapsed_t = interpolator->duration;
    }
}

float ti_elapsed_percent(T_Interpolator *interpolator) {
    return interpolator->elapsed_t / interpolator->duration;
}

bool ti_is_complete(T_Interpolator *interpolator) {
    return fequal(interpolator->duration, interpolator->elapsed_t);
}

void ti_reset(T_Interpolator *interpolator) {
    interpolator->elapsed_t = 0;
}



#ifdef _WIN32
    #include <windows.h>
#elif defined(__APPLE__) || defined(__MACH__) || defined(__linux__) || defined(__unix__)
    #include <time.h>
#else
    #error "No high-resolution timer available for this platform"
#endif

// Store program start time.
static u64 start_time_ns = 0;

u64 get_time_ns() {
#if defined(_WIN32)
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER counter;

    if (freq.QuadPart == 0) {
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&counter);
        start_time_ns = (u64)((counter.QuadPart * 1000000000ULL) / freq.QuadPart);
        return 0;
    }

    QueryPerformanceCounter(&counter);
    u64 now_ns = (u64)((counter.QuadPart * 1000000000ULL) / freq.QuadPart);
    return now_ns - start_time_ns;

#elif defined(__APPLE__) || defined(__MACH__) || defined(__linux__) || defined(__unix__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t now_ns = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;

    if (start_time_ns == 0) {
        start_time_ns = now_ns;
        return 0;
    }
    return now_ns - start_time_ns;

#endif
}



