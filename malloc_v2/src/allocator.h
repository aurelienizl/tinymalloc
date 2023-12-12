#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

struct blk_meta
{
    struct blk_meta *next;
    size_t size;
};

struct blk_allocator
{
    struct blk_meta *meta;
};

void blka_delete(struct blk_allocator *blka);
struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size);
void blka_free(struct blk_meta *blk);
void blka_remove(struct blk_allocator *blka, struct blk_meta *blk);

#endif /* !ALLOCATOR_H */
