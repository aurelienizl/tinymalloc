#include <string.h>
#include <unistd.h>

#include "blk_allocator.h"
#include "my_recycler.h"
#include "tools.h"
#include "my_malloc.h"


static struct blk_allocator blk_alloc = {
    .meta = NULL,
};

static struct blk_meta *new_page(size_t block_size) {
    // Allocate a new block of memory
    struct blk_meta *block_meta = blka_alloc(&blk_alloc, block_size);
    if (block_meta == NULL) {
        return NULL; // Allocation failed
    }

    // Setup the recycler structure immediately after the block meta
    struct recycler *recycler_ptr = (struct recycler *)(block_meta + 1);

    // Calculate the offset to ensure proper alignment
    size_t offset;
    size_t alignment_requirement = size_align(sizeof(struct blk_meta) + sizeof(struct recycler));
    if (block_size < (size_t)PAGE_SIZE) {
        // For small block sizes, increment offset until it meets the alignment requirement
        offset = 0;
        while (offset < alignment_requirement) {
            offset += block_size;
        }
    } else {
        // For larger block sizes, size_align directly
        offset = alignment_requirement;
    }

    // Calculate the starting point for the recycler's memory blocks
    void *start_point = (void *)((char *)block_meta + offset);

    // Initialize the recycler at the calculated starting point
    recycler_create(&recycler_ptr, block_size, block_meta->size - offset, start_point);

    // Check if the recycler was successfully created
    if (recycler_ptr == NULL) {
        return NULL; // Failed to create recycler
    }

    return block_meta;
}

void *my_malloc(size_t size) {
    // Return NULL for zero size allocation request
    if (size == 0) {
        return NULL;
    }

    // Align the requested size
    size = size_align(size);

    struct blk_meta *current_block = blk_alloc.meta;
    struct recycler *recycler_ptr = NULL;

    // Search for a suitable block
    while (current_block != NULL) {
        recycler_ptr = (struct recycler *)(current_block + 1);
        if (recycler_ptr->block_size == size && recycler_ptr->free != NULL) {
            break; // Suitable block found
        }
        current_block = current_block->next;
    }

    // If no suitable block found, create a new page
    if (current_block == NULL) {
        current_block = new_page(size);
        if (current_block == NULL) {
            return NULL; // Page creation failed
        }
        recycler_ptr = (struct recycler *)(current_block + 1);
    }

    // Allocate block from recycler
    void *allocated_block = recycler_allocate(recycler_ptr);
    
    return allocated_block;
}

void my_free(void *ptr) {
    // Only proceed if ptr is not NULL
    if (ptr != NULL) {

        // Find the beginning of the page containing ptr
        struct blk_meta *page_start = page_begin(ptr, PAGE_SIZE);
        if (page_start == NULL) {
            return; // Handle error if page_begin fails
        }

        // Get the recycler associated with this memory block
        struct recycler *recycler_ptr = (struct recycler *)(page_start + 1);

        // Free the block using the recycler
        recycler_free(recycler_ptr, ptr);

        // If all blocks in the recycler are free, remove the page
        if (recycler_ptr->allocated == 0) {
            blka_remove(&blk_alloc, page_start);
        }
    }
}

void *my_realloc(void *ptr, size_t size) {
    // Free the block if the new size is zero
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    // Allocate a new block if ptr is NULL
    if (ptr == NULL) {
        return my_malloc(size);
    }

    // Find the blk_meta and recycler for the current memory block
    struct blk_meta *block_meta = page_begin(ptr, PAGE_SIZE);
    struct recycler *recycler_ptr = (struct recycler *)(block_meta + 1);

    // Return the same block if the new size is smaller or equal to the current block size
    if (size <= recycler_ptr->block_size) {
        return ptr;
    }

    // Allocate a new block and copy the old data
    void *new_block = my_malloc(size);
    if (new_block == NULL) {
        return NULL;
    }
    memmove(new_block, ptr, recycler_ptr->block_size);

    // Free the old block
    my_free(ptr);

    return new_block;
}

void *my_calloc(size_t nmemb, size_t size) {
    // Calculate the total size and check for overflow
    size_t total_size;
    if (__builtin_umull_overflow(nmemb, size, &total_size)) {
        return NULL; // Return NULL if overflow occurs
    }

    // Allocate memory using my_malloc
    void *new_block = my_malloc(total_size);
    if (new_block == NULL) {
        return NULL; // Allocation failed
    }

    // Initialize the allocated memory to zero
    memset(new_block, 0, total_size);

    return new_block;
}
