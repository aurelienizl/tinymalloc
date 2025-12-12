#include <stddef.h>
#include <stdatomic.h>

#include "my_malloc.h"

// Non-allocating, re-entrant lock (per-thread depth)
static atomic_flag g_lock = ATOMIC_FLAG_INIT;
static __thread unsigned g_depth = 0;

#if defined(__x86_64__) || defined(__i386__)
static inline void cpu_relax(void) { __asm__ __volatile__("pause" ::: "memory"); }
#else
static inline void cpu_relax(void) { /* no-op */ }
#endif

static inline void hook_lock(void)
{
    if (g_depth++ != 0)
        return; // recursive entry on same thread

    while (atomic_flag_test_and_set_explicit(&g_lock, memory_order_acquire))
        cpu_relax();
}

static inline void hook_unlock(void)
{
    if (--g_depth != 0)
        return;

    atomic_flag_clear_explicit(&g_lock, memory_order_release);
}

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    hook_lock();
    void *p = my_malloc(size);
    hook_unlock();
    return p;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    hook_lock();
    my_free(ptr);
    hook_unlock();
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    hook_lock();
    void *p = my_realloc(ptr, size);
    hook_unlock();
    return p;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    hook_lock();
    void *p = my_calloc(nmemb, size);
    hook_unlock();
    return p;
}
