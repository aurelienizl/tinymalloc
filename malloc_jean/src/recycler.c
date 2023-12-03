#include "recycler.h"

#include <stdlib.h>

#include "utils.h"

void recycler_create(struct recycler *ret, size_t block_size, size_t total_size,
                     void *start_point)
{
    if (block_size % sizeof(long double) != 0 || block_size == 0 || total_size == 0)
    {
        ret = NULL;
        return;
    }

    ret->block_size = block_size;
    ret->capacity = total_size / block_size;

    ret->chunk = start_point;

    struct free_list *c = ret->chunk;
    for (size_t i = 1; i < ret->capacity; i++)
    {
        c->next = c + block_size / sizeof(struct free_list);
        c = c->next;
    }

    c->next = NULL;

    ret->allocated = 0;
    ret->free = ret->chunk;
}

void recycler_destroy(struct recycler *r)
{
    if (r != NULL)
    {
        free(r->chunk);
        free(r);
    }
}

void *recycler_allocate(struct recycler *r)
{
    if (r == NULL || r->free == NULL)
        return NULL;
    struct free_list *r_free_list = r->free;
    if (r_free_list == NULL)
        return NULL;
    void *ret = r->free;
    struct free_list *tmp = r->free;
    r->free = tmp->next;
    r->allocated++;
    return ret;
}

void recycler_free(struct recycler *r, void *block)
{
    if (r != NULL && block != NULL)
    {
        struct free_list *tmp = block;
        tmp->next = r->free;
        r->free = tmp;
        r->allocated--;
    }
}
