#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdbool.h>
#include <stddef.h>

bool is_nothing_allocated(void);
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t nmemb, size_t size);

#endif /* !MY_MALLOC_H */
