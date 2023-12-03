#include "alignment.h"

size_t align(size_t size)
{
    return ((size - 1) & (~(sizeof(long double) - 1))) + sizeof(long double);
}
