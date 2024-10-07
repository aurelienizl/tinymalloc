#include "my_recycler.h"

#include "tools.h"

void recycler_create(struct recycler **recycler_ptr, size_t block_size,
                     size_t total_size, void *start_addr)
{
    // Validate input parameters
    if (block_size % ALIGNMENT != 0 || block_size == 0 || total_size == 0
        || recycler_ptr == NULL || *recycler_ptr == NULL)
    {
        // Invalid parameters, cannot initialize recycler
        if (recycler_ptr != NULL)
        {
            *recycler_ptr = NULL;
        }
        return;
    }

    struct recycler *recycler = *recycler_ptr;

    // Initialize recycler structure
    recycler->block_size = block_size;
    recycler->capacity = total_size / block_size;
    recycler->chunk = start_addr;

    // Setup the free list using a linked list structure
    struct free_list *current_block = (struct free_list *)start_addr;
    for (size_t i = 1; i < recycler->capacity; i++)
    {
        // Calculate the address of the next block and link it
        current_block->next =
            (struct free_list *)((char *)current_block + block_size);
        current_block = current_block->next;
    }
    // Mark the end of the free list
    current_block->next = NULL;

    // Initialize the allocation and free pointers
    recycler->allocated = 0;
    recycler->free = recycler->chunk;
}

void *recycler_allocate(struct recycler *recycler)
{
    // Check for null pointers
    if (recycler == NULL || recycler->free == NULL)
    {
        return NULL; // No recycler or no free blocks available
    }

    // Allocate a block from the recycler's free list
    struct free_list *allocated_block = recycler->free;
    recycler->free =
        allocated_block->next; // Move the free pointer to the next free block
    recycler->allocated++; // Increment the allocated block count

    return (void *)allocated_block; // Return the allocated block
}

void recycler_free(struct recycler *recycler, void *block)
{
    // Check for null pointers
    if (recycler != NULL && block != NULL)
    {
        struct free_list *block_to_free = (struct free_list *)block;

        // Add the block back to the recycler's free list
        block_to_free->next = recycler->free;
        recycler->free = block_to_free;

        // Decrement the allocated block count
        recycler->allocated--;
    }
}
