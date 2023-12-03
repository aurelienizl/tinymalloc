#include "my_malloc.h"

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "alignment.h"
#include "allocator.h"
#include "page_begin.h"
#include "recycler.h"
#include "utils.h"

static struct blk_allocator mem = {
    .meta = NULL,
};

bool is_nothing_allocated(void)
{
    return mem.meta == NULL;
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static struct blk_meta *new_page(size_t block_size)
{
    struct blk_meta *ret = blka_alloc(&mem, block_size);
    if (ret == NULL)
        return NULL;
    struct recycler *recyler_ptr = ptr_to_ptr(ret + 1);

    size_t offset = 0;
    if (block_size < (size_t)sysconf(_SC_PAGESIZE))
    {
        while (offset
               < align(sizeof(struct blk_meta) + sizeof(struct recycler)))
            offset += block_size;
    }
    else
    {
        offset += align(sizeof(struct blk_meta) + sizeof(struct recycler));
    }

    size_t start_point_s = ptr_to_size_t(ret) + offset;

    void *start_point = size_t_to_ptr(start_point_s);
    recycler_create(recyler_ptr, block_size, ret->size - offset, start_point);
    if (recyler_ptr == NULL)
    {
        return NULL;
    }
    return ret;
}

static void *malloc_internal(size_t size, bool *is_new_page)
{
    *is_new_page = false;

    if (size == 0)
        return NULL;

    size = align(size);

    pthread_mutex_lock(&mutex);

    struct blk_meta *current = mem.meta;
    while (current != NULL)
    {
        struct recycler *tmp = ptr_to_ptr(current + 1);
        if (tmp->block_size == size && tmp->free != NULL)
            break;
        current = current->next;
    }
    if (current == NULL)
    {
        current = new_page(size);
        if (current == NULL)
        {
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        *is_new_page = true;
    }
    struct recycler *tmp = ptr_to_ptr(current + 1);
    void *ret = recycler_allocate(tmp);
    
    pthread_mutex_unlock(&mutex);
    return ret;
}

void *my_malloc(size_t size)
{
    bool is_new_page; // dummy variable
    return malloc_internal(size, &is_new_page);
}

void my_free(void *ptr)
{
    if (ptr != NULL)
    {
        pthread_mutex_lock(&mutex);
        struct blk_meta *begin = page_begin(ptr, sysconf(_SC_PAGESIZE));
        struct recycler *recycl = ptr_to_ptr(begin + 1);
        recycler_free(recycl, ptr);
        if (recycl->allocated == 0)
        {
            blka_remove(&mem, begin);
        }
        pthread_mutex_unlock(&mutex);
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
    struct blk_meta *head = page_begin(ptr, sysconf(_SC_PAGESIZE));
    struct recycler *rec = ptr_to_ptr(head + 1);
    if (size <= rec->block_size)
        return ptr;
    void *ret = my_malloc(size);
    if (ret == NULL)
        return NULL;
    memmove(ret, ptr, rec->block_size);
    my_free(ptr);
    return ret;
}

void *my_calloc(size_t nmemb, size_t size)
{
    size_t checked_size;
    if (__builtin_umull_overflow(nmemb, size, &checked_size))
        return NULL;

    bool is_new_page;
    void *ret = malloc_internal(checked_size, &is_new_page);
    if (ret == NULL)
        return NULL;

    if (!is_new_page)
    {
        memset(ret, 0, checked_size);
    }
    else
    {
        memset(ret, 0, sizeof(struct free_list));
    }

    return ret;
}
