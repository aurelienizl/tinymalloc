#ifndef RECYCLER_H
#define RECYCLER_H

#include <stddef.h>

struct recycler
{
    size_t block_size;
    size_t allocated;
    size_t capacity; // number of blocks in the chunk
    void *chunk; // memory chunk containing all blocks
    void *free; // address of the first free block of the free list
};

struct free_list
{
    struct free_list *next; // next free block
};

void recycler_create(struct recycler *ret, size_t block_size, size_t total_size,
                     void *start_point);
void recycler_destroy(struct recycler *r);
void *recycler_allocate(struct recycler *r);
void recycler_free(struct recycler *r, void *block);

#endif /* !RECYCLER_H */
