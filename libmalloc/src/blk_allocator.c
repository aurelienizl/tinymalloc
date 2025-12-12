#include "blk_allocator.h"

#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>

#include "my_recycler.h"
#include "tools.h"

static size_t check_overflow_add(size_t a, size_t b)
{
    size_t sum;
#if __SIZEOF_SIZE_T__ == 8
    if (__builtin_uaddl_overflow(a, b, &sum)) return 0;
#else
    if (__builtin_uadd_overflow(a, b, &sum)) return 0;
#endif
    return sum;
}

void blka_free(struct blk_meta *block)
{
    // If there are no mappings in the specified address range, then munmap()
    // has no effect.
    munmap(block, (block->size + sizeof(struct blk_meta)));
}

struct blk_meta *blka_alloc(struct blk_allocator *allocator, size_t size)
{
    if (allocator == NULL)
        return NULL;

    size_t ps = tools_page_size();
    if (ps == 0)
        return NULL;

    size_t hdr = size_align(sizeof(struct blk_meta) + sizeof(struct recycler));
    if (hdr == 0)
        return NULL;

    size_t total = check_overflow_add(size, hdr);
    if (total == 0)
        return NULL;

    // Round up to page size without overflow: total + (ps-1)
    if (total > SIZE_MAX - (ps - 1))
        return NULL;

    size_t map_len = (total + (ps - 1)) & ~(ps - 1);
    if (map_len <= sizeof(struct blk_meta))
        return NULL;

    struct blk_meta *m = mmap(NULL, map_len, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (m == MAP_FAILED)
        return NULL;

    m->size = map_len - sizeof(struct blk_meta);
    m->prev = NULL;
    m->next = allocator->meta;
    if (allocator->meta)
        allocator->meta->prev = m;
    allocator->meta = m;

    return m;
}

void blka_remove(struct blk_allocator *allocator, struct blk_meta *block)
{
    if (allocator == NULL || block == NULL)
        return;

    if (block->prev)
        block->prev->next = block->next;
    else
        allocator->meta = block->next;

    if (block->next)
        block->next->prev = block->prev;

    blka_free(block);
}