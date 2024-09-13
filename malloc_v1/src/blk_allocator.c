#include "blk_allocator.h"

#include <sys/mman.h>
#include <unistd.h>

#include "my_recycler.h"
#include "tools.h"

static size_t check_overflow(size_t a, size_t b)
{
    // Check for overflow in the addition
    size_t sum;
    return __builtin_uaddl_overflow(a, b, &sum) ? 0 : sum;
}

void blka_free(struct blk_meta *block)
{
    // If there are no mappings in the specified address range, then munmap()
    // has no effect.
    munmap(block, (block->size + sizeof(struct blk_meta)));
}

struct blk_meta *blka_alloc(struct blk_allocator *allocator, size_t size)
{
    static long system_page_size = 0;
    if (system_page_size == 0)
    {
        system_page_size = sysconf(_SC_PAGESIZE);
        if (system_page_size == -1)
            return NULL; // Failed to get system page size
    }

    size_t aligned_total_size = check_overflow(
        size, size_align(sizeof(struct blk_meta) + sizeof(struct recycler)));

    // Check for overflow in the size calculation
    if (aligned_total_size == 0)
        return NULL;

    // Align the total size to the system page size
    size_t len_aligned_to_page =
        (aligned_total_size + (system_page_size - 1)) & ~(system_page_size - 1);

    // Allocate a new block of memory
    struct blk_meta *new_block =
        mmap(NULL, len_aligned_to_page, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_block == MAP_FAILED)
        return NULL;

    // Update the block metadata and add it to the allocator's linked list
    new_block->size = len_aligned_to_page - sizeof(struct blk_meta);
    new_block->next = allocator->meta;
    new_block->prev = NULL;
    if (allocator->meta)
        allocator->meta->prev = new_block;
    allocator->meta = new_block;

    return new_block;
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
