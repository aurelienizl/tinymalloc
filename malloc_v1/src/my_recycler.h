#ifndef RECYCLER_H
#define RECYCLER_H

#include <stddef.h>

/**
 * @brief Structure representing a memory recycler.
 *        Manages memory allocation and deallocation within a fixed-size block.
 */
struct recycler
{
    size_t block_size; ///< Size of each memory block within the recycler.
    size_t allocated;  ///< Number of blocks currently allocated.
    size_t capacity;   ///< Total number of blocks that can be allocated.
    void *chunk;       ///< Pointer to the start of the memory managed by the recycler.
    void *free;        ///< Pointer to the list of free blocks.
};

/**
 * @brief Structure representing a node in the free list of the recycler.
 */
struct free_list
{
    struct free_list *next; ///< Pointer to the next node in the free list.
};

/**
 * @brief Initializes a recycler structure.
 *
 * @param ret Pointer to a pointer to the recycler structure to initialize.
 * @param block_size Size of each block to be managed by the recycler.
 * @param total_size Total size of the memory to be managed by the recycler.
 * @param start_point Pointer to the start of the memory to be managed.
 */
void recycler_create(struct recycler **ret, size_t block_size, size_t total_size,
                     void *start_point);

/**
 * @brief Allocates a block of memory from the recycler.
 *
 * @param r Pointer to the recycler from which to allocate.
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
void *recycler_allocate(struct recycler *r);

/**
 * @brief Frees a block of memory back to the recycler.
 *
 * @param r Pointer to the recycler to which the block will be freed.
 * @param block Pointer to the block of memory to be freed.
 */
void recycler_free(struct recycler *r, void *block);

#endif /* !RECYCLER_H */
