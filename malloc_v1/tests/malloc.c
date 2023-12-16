#include <criterion/criterion.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>

#include "../src/my_malloc.h"

TestSuite(my_malloc);

Test(my_malloc, malloc_1)
{
    void *ptr = my_malloc(1);
    cr_assert_not_null(ptr);
    my_free(ptr);
}

Test(my_malloc, random_alloc_01)
{
    clock_t start_time = clock();

    for (long i = 0; i < 100; i++) 
    {
        size_t size = rand() % 1000;
        void *ptr = my_malloc(size);
        memset(ptr, 0, size);
        my_free(ptr);
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("TEST RANDOM 01 : Time taken: %.2f seconds\n", elapsed_time);
}

Test(my_malloc, random_alloc_02)
{
    clock_t start_time = clock();

    void *ptrs[100];

    for (long i = 0; i < 100; i++) 
    {
        size_t size = rand() % 1000;
        void *ptr = my_malloc(size);
        memset(ptr, 0, size);
        ptrs[i] = ptr;
    }

    for (long i = 0; i < 100; i++) 
    {
        my_free(ptrs[i]);
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("TEST RANDOM 02 : Time taken: %.2f seconds\n", elapsed_time);
}

Test(my_malloc, random_alloc_03)
{
    clock_t start_time = clock();

    void *ptrs[1000];

    for (long i = 0; i < 1000; i++) 
    {
        size_t size = rand() % 1000;
        void *ptr = my_malloc(size);
        memset(ptr, 0, size);
        ptrs[i] = ptr;
    }

    for (long i = 0; i < 1000; i++) 
    {
        my_free(ptrs[i]);
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("TEST RANDOM 03 : Time taken: %.2f seconds\n", elapsed_time);
}

Test(my_malloc, random_alloc_04)
{
    clock_t start_time = clock();

    void *ptrs[1000000];

    for (size_t i = 0; i < 1000000; i++) 
    {
        size_t size = rand() % 10000;
        void *ptr = my_malloc(size);
        memset(ptr, 0, size);
        ptrs[i] = ptr;
    }

    for (size_t i = 0; i < 1000000; i++) 
    {
        my_free(ptrs[i]);
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("TEST RANDOM 04 : Time taken: %.2f seconds\n", elapsed_time);
}

Test(my_malloc, segfault_01, .signal = SIGSEGV)
{
    void *ptr = my_malloc(1);
    my_free(ptr);
    memset(ptr, 0, 1);
}

Test(my_malloc, segfault_02, .signal = SIGSEGV)
{
    for (long i = 0; i < 100; i++) 
    {
        size_t size = rand() % 1000;
        void *ptr = my_malloc(size);
        memset(ptr, 0, size);
        my_free(ptr);
    }

    void *ptr = my_malloc(1);
    my_free(ptr);

    memset(ptr, 0, 1);
}

Test(my_malloc, segfault_03, .signal = SIGSEGV)
{
    void *ptrs[100];

    for (long i = 0; i < 100; i++) 
    {
        size_t size = rand() % 1000;
        void *ptr = my_malloc(size);
        ptrs[i] = ptr;
        memset(ptr, 0, size);
    }

    for (long i = 0; i < 100; i++) 
    {
        my_free(ptrs[i]);
    }

    void *ptr = my_malloc(1);
    my_free(ptr);

    memset(ptr, 0, 1);
}

Test(my_malloc, segfault_04, .signal = SIGSEGV)
{
    void *ptrs[1000];

    for (long i = 0; i < 1000; i++) 
    {
        size_t size = rand() % 1000;
        void *ptr = my_malloc(size);
        ptrs[i] = ptr;
        memset(ptr, 0, size);
    }

    for (long i = 0; i < 1000; i++) 
    {
        my_free(ptrs[i]);
    }

    void *ptr = my_malloc(1);
    my_free(ptr);

    memset(ptr, 0, 1);
}
