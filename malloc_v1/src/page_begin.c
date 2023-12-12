#include "page_begin.h"

void *page_begin(void *ptr, size_t page_size)
{
    char *tmp = ptr;
    return (tmp - ((size_t)tmp & (page_size - 1)));
}
