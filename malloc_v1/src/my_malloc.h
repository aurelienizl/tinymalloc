#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stddef.h>

/**
 * @brief Allocates a block of memory of a specified size.
 *
 * @param size The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void *my_malloc(size_t size);

/**
 * @brief Frees a block of memory previously allocated with my_malloc or related functions.
 *
 * @param ptr Pointer to the memory block to be freed. If NULL, no action is taken.
 */
void my_free(void *ptr);

/**
 * @brief Reallocates a block of memory to a new size, preserving the existing data.
 *
 * @param ptr Pointer to the memory block previously allocated with my_malloc. If NULL, acts like my_malloc.
 * @param size The new size for the memory block, in bytes.
 * @return A pointer to the reallocated memory block, which may be different from the original ptr, or NULL if the reallocation fails.
 */
void *my_realloc(void *ptr, size_t size);

/**
 * @brief Allocates a block of memory for an array of elements, initializing them to zero.
 *
 * @param nmemb Number of elements in the array.
 * @param size Size of each element in the array, in bytes.
 * @return A pointer to the allocated memory, with all bytes initialized to zero, or NULL if the allocation fails.
 */
void *my_calloc(size_t nmemb, size_t size);

#endif /* !MY_MALLOC_H */
