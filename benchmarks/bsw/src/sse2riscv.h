#ifndef SSE2RISCV_H
#define SSE2RISCV_H

#ifdef __riscv

#if defined(__GNUC__) || defined(__clang__)

#pragma push_macro("FORCE_INLINE")
#pragma push_macro("ALIGN_STRUCT")
#define FORCE_INLINE static inline __attribute__((always_inline))
#define ALIGN_STRUCT(x) __attribute__((aligned(x)))

#else

#error "Macro name collisions may happens with unknown compiler"
#ifdef FORCE_INLINE
#undef FORCE_INLINE
#endif
#define FORCE_INLINE static inline
#ifndef ALIGN_STRUCT
#define ALIGN_STRUCT(x) __declspec(align(x))
#endif

#endif

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

FORCE_INLINE void* _mm_malloc(size_t size, size_t align)
{
    // TODO: 256 -> alignment.
    return aligned_alloc(256, size);
}

FORCE_INLINE void _mm_free(void *ptr)
{
    free(ptr);
}

FORCE_INLINE uint64_t __rdtsc()
{
    uint64_t virtual_timer_value;
    asm volatile ("rdcycle %0 ;\n":"=r" (virtual_timer_value) ::);
    //virtual_timer_value = (unsigned long) time(0);
    return virtual_timer_value;
}

#if defined(__GNUC__) || defined(__clang__)
#pragma pop_macro("ALIGN_STRUCT")
#pragma pop_macro("FORCE_INLINE")
#endif

#endif

#endif