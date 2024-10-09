#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

/**
 * @brief Structure representing metadata for a memory block in a block
 * allocator.
 */
struct blk_meta
{
    struct blk_meta *next; ///< Pointer to the next block in the allocator.
    struct blk_meta *prev; ///< Pointer to the previous block in the allocator.
    size_t size; ///< Size of the memory block.
};

/**
 * @brief Structure representing a block allocator.
 *
 * This structure is used to manage a linked list of blk_meta structures,
 * enabling dynamic memory allocation.
 */
struct blk_allocator
{
    struct blk_meta *meta; ///< Pointer to the first blk_meta in the allocator.
};

/**
 * @brief Allocates a block of memory of a specified size from the block
 * allocator.
 *
 * @param blka Pointer to the block allocator from which to allocate.
 * @param size The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated blk_meta structure, or NULL if the
 * allocation fails.
 */
struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size);

/**
 * @brief Frees a block of memory that was allocated with blka_alloc.
 *
 * @param blk Pointer to the blk_meta structure representing the block to be
 * freed.
 */
void blka_free(struct blk_meta *blk);

/**
 * @brief Removes a block of memory from the block allocator's linked list.
 *
 * This function does not free the memory of the block itself; it only removes
 * the block from the allocator's linked list.
 *
 * @param blka Pointer to the block allocator from which the block is to be
 * removed.
 * @param blk Pointer to the blk_meta structure representing the block to be
 * removed.
 */
void blka_remove(struct blk_allocator *blka, struct blk_meta *blk);

#endif /* !ALLOCATOR_H */
