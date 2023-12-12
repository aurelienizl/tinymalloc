#include "my_malloc.h"

#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    pthread_mutex_lock(&mutex);
    void *rptr = my_malloc(size);
    pthread_mutex_unlock(&mutex);
    return rptr;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    pthread_mutex_lock(&mutex);
    my_free(ptr);
    pthread_mutex_unlock(&mutex);
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    pthread_mutex_lock(&mutex);
    void *rptr = my_realloc(ptr, size);
    pthread_mutex_unlock(&mutex);
    return rptr;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    pthread_mutex_lock(&mutex);
    void *rptr = my_calloc(nmemb, size);
    pthread_mutex_unlock(&mutex);
    return rptr;
}
