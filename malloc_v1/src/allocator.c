#include "allocator.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "alignment.h"
#include "recycler.h"

static size_t beware_overflow(size_t a, size_t b)
{
    size_t offset;
    if (__builtin_uaddl_overflow(a, b, &offset))
        return 0;
    return offset;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1)
        return NULL;
    size_t len = beware_overflow(
        size, align(sizeof(struct blk_meta) + sizeof(struct recycler)));
    if (len == 0)
        return NULL;
    len /= page_size;
    if (len * page_size
        < size + align(sizeof(struct blk_meta) + sizeof(struct recycler)))
        len++;
    struct blk_meta *ret = mmap(NULL, len * page_size, PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ret == MAP_FAILED)
        return NULL;
    ret->next = blka->meta;
    ret->size = (len * page_size) - sizeof(struct blk_meta);
    blka->meta = ret;
    return ret;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, (blk->size + sizeof(struct blk_meta)));
}

void blka_remove(struct blk_allocator *blka, struct blk_meta *blk)
{
    if (blka->meta == blk)
    {
        struct blk_meta *tmp = blka->meta;
        blka->meta = blka->meta->next;
        blka_free(tmp);
    }
    else
    {
        struct blk_meta *c = blka->meta;
        while (c->next != blk)
        {
            c = c->next;
        }
        struct blk_meta *tmp = c->next;
        c->next = c->next->next;
        blka_free(tmp);
    }
}
