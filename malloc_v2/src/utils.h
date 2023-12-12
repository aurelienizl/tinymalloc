#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

static inline void *ptr_to_ptr(void *ptr)
{
    return ptr;
}

static inline size_t ptr_to_size_t(void *ptr)
{
    return (size_t)ptr;
}

static inline void *size_t_to_ptr(size_t s)
{
    return (void *)s;
}

#endif /* !UTILS_H */
