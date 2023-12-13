#include "tools.h"

size_t size_align_(size_t size)
{
    // Align to 16 bytes
    const size_t align_base = sizeof(long double);
    const size_t mask = align_base - 1;

    if (size > (size_t)(-1) - mask)
    {
        return 0;
    }

    return (size + mask) & ~mask;
}

size_t size_align(size_t size)
{
    if (size < 16)
    {
        return 16;
    }
    // Align to the closed power of two
    if (size > (size_t)(-1) / 2 + 1)
    {
        return 0;
    }

    size_t mask = 1;
    while (mask < size)
    {
        mask <<= 1;
    }

    return mask;
}

void *page_begin(void *ptr, size_t page_size)
{
    char *tmp = (char *)ptr;
    size_t mask = page_size - 1;

    if ((page_size & mask) != 0)
    {
        return NULL;
    }

    return (void *)(tmp - ((size_t)tmp & mask));
}
