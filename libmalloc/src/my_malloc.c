#include "my_malloc.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "blk_allocator.h"
#include "my_recycler.h"
#include "tools.h"

#define MIN_BLOCK_SIZE 16
#define MAX_BUCKET_SIZE 1024
#define BUCKET_COUNT 7

static struct blk_allocator buckets[BUCKET_COUNT + 1];

static void add_block_to_list(struct blk_allocator *alloc, struct blk_meta *block)
{
    block->next = alloc->meta;
    block->prev = NULL;
    if (alloc->meta)
        alloc->meta->prev = block;
    alloc->meta = block;
}

static void remove_block_from_list(struct blk_allocator *alloc, struct blk_meta *block)
{
    if (block->prev)
        block->prev->next = block->next;
    else
        alloc->meta = block->next;

    if (block->next)
        block->next->prev = block->prev;

    block->next = NULL;
    block->prev = NULL;
}

static size_t get_bucket_index(size_t size)
{
    if (size > MAX_BUCKET_SIZE)
        return BUCKET_COUNT;

    if (size <= MIN_BLOCK_SIZE)
        return 0;

    // size assumed aligned to 16
    return (sizeof(size_t) * 8) - (size_t)__builtin_clzl(size - 1) - 4;
}

static size_t get_size_for_index(size_t index)
{
    if (index >= BUCKET_COUNT)
        return 0;
    return (size_t)16 << index;
}

static struct blk_meta *new_page(struct blk_allocator *alloc, size_t block_size)
{
    struct blk_meta *m = blka_alloc(alloc, block_size);
    if (m == NULL)
        return NULL;

    struct recycler *r = (struct recycler *)(m + 1);

    // Offset from mapping base to first allocatable block start
    size_t offset = size_align(sizeof(struct blk_meta) + sizeof(struct recycler));
    if (offset == 0)
    {
        blka_remove(alloc, m);
        return NULL;
    }

    size_t map_len = m->size + sizeof(struct blk_meta);

    // Ensure first block and at least one full block fits in mapping
    if (map_len <= offset || (map_len - offset) < block_size)
    {
        blka_remove(alloc, m);
        return NULL;
    }

    void *start_point = (void *)((char *)m + offset);

    recycler_create(&r, block_size, map_len - offset, start_point);
    if (r == NULL)
    {
        blka_remove(alloc, m);
        return NULL;
    }

    return m;
}

void *my_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    size_t aligned_req = size_align(size);
    // Critical: size_align returns 0 on overflow
    if (aligned_req == 0)
        return NULL;

    size_t bucket_idx = get_bucket_index(aligned_req);
    struct blk_allocator *alloc = &buckets[bucket_idx];

    size_t actual_block_size = (bucket_idx < BUCKET_COUNT)
        ? get_size_for_index(bucket_idx)
        : aligned_req;

    struct blk_meta *m = alloc->meta;
    struct recycler *r = NULL;

    // Search only in bucket list
    while (m != NULL)
    {
        r = (struct recycler *)(m + 1);
        if (r->block_size >= actual_block_size && r->free != NULL)
            break;
        m = m->next;
    }

    if (m == NULL)
    {
        m = new_page(alloc, actual_block_size);
        if (m == NULL)
            return NULL;
        r = (struct recycler *)(m + 1);
    }

    void *p = recycler_allocate(r);

    // If page became full, remove it from list
    if (p != NULL && r->free == NULL)
        remove_block_from_list(alloc, m);

    return p;
}

void my_free(void *ptr)
{
    if (ptr == NULL)
        return;

    size_t ps = tools_page_size();
    if (ps == 0)
        return;

    struct blk_meta *m = page_begin(ptr, ps);
    if (m == NULL)
        return;

    struct recycler *r = (struct recycler *)(m + 1);

    // If it was full (free list empty), it is likely not in any bucket list.
    // Re-add it before freeing so it can be used again.
    if (r->free == NULL)
    {
        size_t idx = get_bucket_index(r->block_size);
        add_block_to_list(&buckets[idx], m);
    }

    recycler_free(r, ptr);

    // If recycler now empty, remove from list and unmap
    if (r->allocated == 0)
    {
        size_t idx = get_bucket_index(r->block_size);
        blka_remove(&buckets[idx], m);
    }
}

void *my_realloc(void *ptr, size_t size)
{
    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    if (ptr == NULL)
        return my_malloc(size);

    size_t ps = tools_page_size();
    if (ps == 0)
        return NULL;

    struct blk_meta *m = page_begin(ptr, ps);
    if (m == NULL)
        return NULL;

    struct recycler *r = (struct recycler *)(m + 1);

    if (size <= r->block_size)
        return ptr;

    void *n = my_malloc(size);
    if (n == NULL)
        return NULL;

    memmove(n, ptr, r->block_size);
    my_free(ptr);
    return n;
}

void *my_calloc(size_t nmemb, size_t size)
{
    if (nmemb != 0 && size > SIZE_MAX / nmemb)
        return NULL;

    size_t total = nmemb * size;
    void *p = my_malloc(total);
    if (p == NULL)
        return NULL;

    memset(p, 0, total);
    return p;
}
