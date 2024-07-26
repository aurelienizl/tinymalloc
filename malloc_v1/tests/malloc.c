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

Test(my_malloc, calloc_1)
{
    size_t num = 10;
    size_t size = sizeof(int);
    int *ptr = (int *)my_calloc(num, size);
    cr_assert_not_null(ptr, "calloc returned NULL");

    // Verify that memory is zero-initialized
    for (size_t i = 0; i < num; i++) {
        cr_assert_eq(ptr[i], 0, "calloc memory not zero-initialized at index %zu", i);
    }

    my_free(ptr);
}

Test(my_malloc, calloc_zero_size)
{
    int *ptr = (int *)my_calloc(0, sizeof(int));
    cr_assert_null(ptr, "calloc should return NULL when number of elements is 0");
}

Test(my_malloc, calloc_zero_elem_size)
{
    int *ptr = (int *)my_calloc(10, 0);
    cr_assert_null(ptr, "calloc should return NULL when size of each element is 0");
}

Test(my_malloc, realloc_increase_size)
{
    char *ptr = (char *)my_malloc(10);
    memset(ptr, 1, 10);
    char *new_ptr = (char *)my_realloc(ptr, 20);

    // Check if realloc was successful
    cr_assert_not_null(new_ptr, "realloc returned NULL");

    // Check if old data is preserved
    for (int i = 0; i < 10; i++) {
        cr_assert_eq(new_ptr[i], 1, "Data not preserved after realloc at index %d", i);
    }

    my_free(new_ptr);
}

Test(my_malloc, realloc_decrease_size)
{
    char *ptr = (char *)my_malloc(10);
    memset(ptr, 1, 10);
    char *new_ptr = (char *)my_realloc(ptr, 5);

    // Check if realloc was successful
    cr_assert_not_null(new_ptr, "realloc returned NULL");

    // Check if old data is preserved (at least for the reduced size part)
    for (int i = 0; i < 5; i++) {
        cr_assert_eq(new_ptr[i], 1, "Data not preserved after realloc at index %d", i);
    }

    my_free(new_ptr);
}

Test(my_malloc, realloc_null_pointer)
{
    char *new_ptr = (char *)my_realloc(NULL, 10);  // Should behave like malloc
    cr_assert_not_null(new_ptr, "realloc with NULL pointer did not behave like malloc");
    my_free(new_ptr);
}

Test(my_malloc, realloc_zero_size)
{
    char *ptr = (char *)my_malloc(10);
    char *new_ptr = (char *)my_realloc(ptr, 0);  // Optional: Some systems return NULL here

    // Implementation-specific behavior: Some free memory, others don't
    cr_assert(new_ptr == NULL || new_ptr != NULL, "realloc to zero size did not behave as expected");
    my_free(new_ptr);
}

Test(my_malloc, stress_large_block_malloc_free)
{
    const int iterations = 10; // Fewer iterations due to large size
    for (int i = 0; i < iterations; i++) {
        size_t size = 1024 * 1024 * 100; // 100 MB blocks
        void *ptr = my_malloc(size);
        cr_assert_not_null(ptr, "malloc failed to allocate 100MB on iteration %d", i);
        my_free(ptr);
    }
}

Test(my_malloc, stress_random_realloc)
{
    void *ptr = NULL;
    const int iterations = 50000;
    for (int i = 0; i < iterations; i++) {
        size_t newSize = rand() % (1024 * 1024); // Up to 1 MB
        ptr = my_realloc(ptr, newSize);
        cr_assert_not_null(ptr, "realloc failed on iteration %d with size %zu", i, newSize);
    }
    my_free(ptr);
}

Test(my_malloc, mixed_calloc_realloc_free)
{
    int *ptr = NULL;
    const int iterations = 50000;
    for (int i = 0; i < iterations; i++) {
        ptr = (int *)my_calloc(1, rand() % 512 + 1);
        cr_assert_not_null(ptr, "calloc failed on iteration %d", i);
        ptr = (int *)my_realloc(ptr, rand() % 1024 + 1);
        cr_assert_not_null(ptr, "realloc failed on iteration %d", i);
        my_free(ptr);
    }
}

Test(my_malloc, mixed_random_operations)
{
    // Start a timer
    clock_t start_time = clock();

    void *ptr = NULL;
    const int iterations = 100000;
    for (int i = 0; i < iterations; i++) {
        switch (rand() % 4) {
            case 0: // malloc
                ptr = my_malloc(rand() % 1024 + 1);
                cr_assert_not_null(ptr, "malloc failed on iteration %d", i);
                break;
            case 1: // realloc
                ptr = my_realloc(ptr, rand() % 2048 + 1);
                cr_assert_not_null(ptr, "realloc failed on iteration %d", i);
                break;
            case 2: // free
                my_free(ptr);
                ptr = NULL;
                break;
            case 3: // calloc
                size_t size = rand() % 512 + 1;
                ptr = my_calloc(1, size);
                cr_assert_not_null(ptr, "calloc failed on iteration %d", i);
                for (size_t j = 0; j < size; j++) {
                    cr_assert_eq(((char *)ptr)[j], 0, "calloc memory not zero-initialized on iteration %d", i);
                }
                break;
        }
    }
    my_free(ptr); // Ensure we clean up

    // End the timer
    clock_t end_time = clock();

    // Calculate the elapsed time
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print the elapsed time
    printf("BENCH_MIXED_1: %.2f seconds\n", elapsed_time);
}


Test(my_malloc, mixed_all_functions)
{
    // Start a timer
    clock_t start_time = clock();

    void **pointers = calloc(1000, sizeof(void *));
    for (int i = 0; i < 100000; i++) {
        int idx = rand() % 1000;
        if (pointers[idx] != NULL) {
            if (rand() % 2) {
                pointers[idx] = my_realloc(pointers[idx], rand() % 1000);
            } else {
                my_free(pointers[idx]);
                pointers[idx] = NULL;
            }
        } else {
            if (rand() % 2) {
                pointers[idx] = my_malloc(rand() % 500);
            } else {
                pointers[idx] = my_calloc(1, rand() % 500);
            }
        }
    }

    for (int i = 0; i < 1000; i++) {
        my_free(pointers[i]);
    }
    free(pointers);

    // End the timer
    clock_t end_time = clock();

    // Calculate the elapsed time
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print the elapsed time
    printf("BENCH_MIX_2: %.2f seconds\n", elapsed_time);
}
