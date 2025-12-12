#include "tools.h"

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

size_t global_page_size; // cache; 0 means uninitialized

static int is_power_of_two(size_t x)
{
    return x != 0 && (x & (x - 1)) == 0;
}

size_t tools_page_size(void)
{
    // Lock-free lazy init (no pthread dependency, avoids constructor ordering issues)
    size_t ps = __atomic_load_n(&global_page_size, __ATOMIC_ACQUIRE);
    if (ps != 0)
        return ps;

    long tmp = sysconf(_SC_PAGESIZE);
    if (tmp <= 0)
        return 0;

    ps = (size_t)tmp;
    if (!is_power_of_two(ps))
        return 0;

    __atomic_store_n(&global_page_size, ps, __ATOMIC_RELEASE);
    return ps;
}

size_t size_align(size_t size)
{
    // Align to ALIGNMENT (16 bytes) — keep consistent with recycler/block expectations
    const size_t mask = (size_t)ALIGNMENT - 1;

    if (size > SIZE_MAX - mask)
        return 0;

    return (size + mask) & ~mask;
}

void *page_begin(void *ptr, size_t page_size)
{
    if (ptr == NULL)
        return NULL;

    if (page_size == 0 || !is_power_of_two(page_size))
        return NULL;

    uintptr_t p = (uintptr_t)ptr;
    return (void *)(p & ~(uintptr_t)(page_size - 1));
}
