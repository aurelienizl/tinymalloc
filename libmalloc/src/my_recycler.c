#include "my_recycler.h"
#include "tools.h"

#include <stdint.h>
#include <stddef.h>

static int recycler_owns_block(const struct recycler *r, const void *block)
{
    if (!r || !block || !r->chunk || r->block_size == 0 || r->capacity == 0)
        return 0;

    size_t span;
    if (__builtin_mul_overflow(r->capacity, r->block_size, &span))
        return 0;

    uintptr_t base = (uintptr_t)r->chunk;
    uintptr_t p    = (uintptr_t)block;

    // Prevent base + span overflow
    if ((uintptr_t)span > UINTPTR_MAX - base)
        return 0;

    uintptr_t end = base + (uintptr_t)span;

    if (p < base || p >= end)
        return 0;

    // Must be aligned to block boundary
    if (((p - base) % (uintptr_t)r->block_size) != 0)
        return 0;

    return 1;
}

static int freelist_contains(const struct recycler *r, const void *block)
{
    // Defensive scan capped by capacity to avoid infinite loops on corruption.
    const struct free_list *cur = (const struct free_list *)r->free;
    for (size_t i = 0; i < r->capacity && cur != NULL; i++)
    {
        if ((const void *)cur == block)
            return 1;
        cur = cur->next;
    }
    return 0;
}

void recycler_create(struct recycler **recycler_ptr,
                     size_t block_size,
                     size_t total_size,
                     void *start_addr)
{
    if (recycler_ptr == NULL || *recycler_ptr == NULL)
        return;

    struct recycler *r = *recycler_ptr;

    // Basic validation
    if (block_size == 0 || total_size == 0 || start_addr == NULL)
        goto fail;

    if ((block_size % ALIGNMENT) != 0)
        goto fail;

    if (((uintptr_t)start_addr % ALIGNMENT) != 0)
        goto fail;

    r->block_size = block_size;
    r->capacity   = total_size / block_size;
    r->chunk      = start_addr;

    // Critical fix: capacity must be >= 1
    if (r->capacity == 0)
        goto fail;

    // Build free list in-place
    struct free_list *first = (struct free_list *)start_addr;
    struct free_list *cur   = first;

    for (size_t i = 1; i < r->capacity; i++)
    {
        cur->next = (struct free_list *)((char *)cur + block_size);
        cur = cur->next;
    }
    cur->next = NULL;

    r->allocated = 0;
    r->free      = (void *)first;
    return;

fail:
    // Ensure caller can detect failure
    r->block_size = 0;
    r->capacity   = 0;
    r->allocated  = 0;
    r->chunk      = NULL;
    r->free       = NULL;
    *recycler_ptr = NULL;
}

void *recycler_allocate(struct recycler *r)
{
    if (r == NULL || r->free == NULL)
        return NULL;

    // Defensive: prevent overflow of allocated count beyond capacity
    if (r->allocated >= r->capacity)
        return NULL;

    struct free_list *blk = (struct free_list *)r->free;
    r->free = (void *)blk->next;
    r->allocated++;

    return (void *)blk;
}

void recycler_free(struct recycler *r, void *block)
{
    if (r == NULL || block == NULL)
        return;

    // Do not underflow, and avoid exploding on misuse.
    if (r->allocated == 0)
        return;

    // Reject foreign/misaligned pointers (prevents arbitrary writes to "next")
    if (!recycler_owns_block(r, block))
        return;

    // Reject double-free (prevents cycles/corruption)
    if (freelist_contains(r, block))
        return;

    struct free_list *b = (struct free_list *)block;
    b->next = (struct free_list *)r->free;
    r->free = (void *)b;
    r->allocated--;
}
