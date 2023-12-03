#include <criterion/criterion.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// to get a reproducible test. Remove macro and uncomment include to get
// random tests
#include <time.h>
//#define time(...) (42)

#include "../src/my_malloc.h"

#define IS_PTR_ALIGNED(PTR) cr_assert_eq((((unsigned long)(PTR)) & 15), 0)

Test(malloc_free, malloc_free_0, .timeout = 1)
{
    void *tmp = my_malloc(0);
    cr_expect_eq(tmp, NULL);
    free(tmp);
}

Test(malloc_free, segfault, .signal = SIGSEGV, .timeout = 1)
{
    char *tmp = my_malloc(1);
    IS_PTR_ALIGNED(tmp);
    my_free(tmp);
    tmp[0] = 'a';
}

Test(malloc_free, easy, .timeout = 2)
{
    char *str = "Hello World!";
    char *tmp = my_malloc(sizeof(char) * (strlen(str) + 1));
    cr_expect_neq(tmp, NULL);
    IS_PTR_ALIGNED(tmp);
    strcpy(tmp, str);
    cr_expect_eq(strcmp(str, tmp), 0);
    my_free(tmp);
}

Test(malloc_free, malloc_huge_seqfault, .signal = SIGSEGV, .timeout = 2)
{
    char *tmp = my_malloc(42000);
    IS_PTR_ALIGNED(tmp);
    my_free(tmp);
    tmp[5000] = 'a';
}

Test(malloc_free, malloc_huge, .timeout = 2)
{
    char *tmp = my_malloc(42000);
    IS_PTR_ALIGNED(tmp);
    tmp[5000] = 'a';
    my_free(tmp);
}

Test(malloc_free, malloc_free_serie, .timeout = 2)
{
#define TEST (4)
    char *arr[TEST];
    for (size_t i = 0; i < TEST; i++)
    {
        arr[i] = my_malloc(42);
        IS_PTR_ALIGNED(arr[i]);
        arr[i][40] = 'a';
    }
    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);
#undef TEST
}

Test(malloc_free, malloc_free_serie_a_lot, .timeout = 2)
{
#define TEST (420)
    char *arr[TEST];
    for (size_t i = 0; i < TEST; i++)
    {
        arr[i] = my_malloc(42);
        arr[i][40] = 'a';
    }
    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);
#undef TEST
}

Test(malloc_free, malloc_free_serie_random, .timeout = 2)
{
    srand(time(NULL));

#define TEST (10)
    char *arr[TEST] = { 0 };
    for (size_t i = 0; i < TEST; i++)
    {
        size_t s = rand() % 70;
        arr[i] = my_malloc(s);
    }
    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);
#undef TEST
}

#include <assert.h>
#include <stdio.h>
Test(malloc_free, malloc_free_real)
{
    // Bad seed: 1701474286
    size_t seed = time(NULL);
    //printf("Seed: %ld\n", seed);
    //size_t seed = 1701474286;
    srand(seed);
#define TEST (42000)
#define MIN_STEP (100)
#define MAX_STEP_OFFSET (150)
#define MAX_STEP_BACK (91)
#define MAX_SIZE (400)

    char *arr[TEST] = { NULL };
    size_t k = 0;
    size_t j = 0;
    size_t step = MIN_STEP + rand() % MAX_STEP_OFFSET;

    for (size_t i = 0; i < TEST; i++, k++, j++)
    {
        size_t s =  rand() % MAX_SIZE;
        arr[j] = my_malloc(s);
        IS_PTR_ALIGNED(arr[j]);
        if (s != 0)
        {
            cr_assert_neq(arr[j], NULL);
            arr[j][s - 1] = 'a';
        }
        if (k == step)
        {
            size_t l = k - rand() % MAX_STEP_BACK;
            while (k > l)
            {
                my_free(i[arr]);
                j[arr] = NULL;
                k--;
                j--;
            }
            step = MIN_STEP + rand() % MAX_STEP_OFFSET;
        }
    }

    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);

#undef TEST
#undef MIN_STEP
#undef MAX_STEP_OFFSET
#undef MAX_SIZE
#undef STEP_BACK
}

Test(malloc_free, malloc_free_serie_a_lot_random, .timeout = 3)
{
    srand(time(NULL));

#define TEST (42000)
    char *arr[TEST] = { 0 };
    for (size_t i = 0; i < TEST; i++)
    {
        size_t s = rand() % 400;
        arr[i] = my_malloc(s);
        if (s != 0)
            arr[i][s - 1] = 'a';
    }
    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);
#undef TEST
}

Test(malloc_free, calloc_increment, .timeout = 2)
{
#define TEST (20)
    char *tmp = calloc(TEST, 1);
    char *arr[TEST] = { 0 };
    for (size_t i = 0; i < TEST; i++)
    {
        arr[i] = my_calloc(i, 1);
        cr_expect_eq(memcmp(arr[i], tmp, i), 0);
    }

    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);

    free(tmp);
#undef TEST
}

Test(calloc_free, calloc_increment_a_lot, .timeout = 2)
{
#define TEST (2000)
    char *tmp = calloc(TEST, 1);
    char *arr[TEST] = { 0 };
    for (size_t i = 0; i < TEST; i++)
    {
        arr[i] = my_calloc(i, 1);
        cr_expect_eq(memcmp(arr[i], tmp, i), 0);
    }

    for (size_t i = 0; i < TEST; i++)
        my_free(arr[i]);
    free(tmp);
#undef TEST
}

Test(realloc, easy_peasy, .timeout = 1)
{
    char *tmp = NULL;
    tmp = my_realloc(tmp, 42);
    tmp[41] = 0;
    my_free(tmp);
}

Test(realloc, easy, .signal = SIGSEGV, .timeout = 1)
{
    char *tmp = my_malloc(42);
    tmp = my_realloc(tmp, 0);
    tmp[0] = 'a';
}

Test(bonus, realloc_extend_reduce_bitbucket, .timeout = 1)//, .disabled = true)
{
    char *tmp = my_calloc(33,1);
    void *tmp2 = my_realloc(tmp, 40);
    cr_expect_eq(tmp, tmp2);
    my_free(tmp2);
}

Test(bonus, realloc_reduce_no_move, .timeout = 1)//, .disabled = true)
{
    char *tmp = my_calloc(33,1);
    void *tmp2 = my_realloc(tmp, 10);
    cr_expect_eq(tmp, tmp2);
    my_free(tmp2);
}
