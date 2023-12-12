#ifndef MMA_TOOLS_H
#define MMA_TOOLS_H

#include <stddef.h>

/**
 * @brief Defines the system page size using sysconf.
 */
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

/**
 * @brief Defines the alignment size.
 */
#define ALIGNMENT 16 // 16 bytes

/**
 * @brief Calculates the beginning of the page on which a given pointer resides.
 *
 * This function is used to find the start of the memory page that contains the provided pointer.
 *
 * @param ptr Pointer for which the page beginning is to be found.
 * @param page_size The size of a page, typically obtained from a system call or constant.
 * @return A pointer to the beginning of the page, or NULL if the calculation fails.
 */
void *page_begin(void *ptr, size_t page_size);

/**
 * @brief Aligns a given size to the nearest multiple of a predefined alignment size.
 *
 * This function is used to ensure memory sizes are aligned to a specific boundary, which can be
 * critical for performance on some architectures.
 *
 * @param size The size to be aligned.
 * @return The aligned size, which is equal to or greater than the original size.
 */
size_t size_align(size_t size);

#endif // MMA_TOOLS_H
