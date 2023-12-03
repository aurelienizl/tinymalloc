#include "allocator.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "alignment.h"
#include "recycler.h"

static struct blk_meta *bst_minimum(struct blk_meta *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

static void bst_insert(struct blk_meta **root, struct blk_meta *node) {
    struct blk_meta **current = root;
    while (*current != NULL) {
        if (node->key_address < (*current)->key_address) {
            current = &((*current)->left);
        } else {
            current = &((*current)->right);
        }
    }
    *current = node;
    node->left = node->right = NULL;
}

static void bst_delete(struct blk_meta **root, struct blk_meta *node_to_delete) {
    if (node_to_delete == NULL) return;

    // Find node
    struct blk_meta **curr = root;
    while (*curr != NULL && (*curr)->key_address != node_to_delete->key_address) {
        if (node_to_delete->key_address < (*curr)->key_address) {
            curr = &((*curr)->left);
        } else {
            curr = &((*curr)->right);
        }
    }

    if (*curr == NULL) return; // Node not found

    // Node with only one child or no child
    if ((*curr)->left == NULL || (*curr)->right == NULL) {
        struct blk_meta *temp = ((*curr)->left) ? (*curr)->left : (*curr)->right;

        // No child case
        if (temp == NULL) {
            temp = *curr;
            *curr = NULL;
        } else { // One child case
            **curr = *temp; // Copy the contents of the non-empty child
        }
        free(temp);
    } else {
        // Node with two children: Get the inorder successor (smallest in the right subtree)
        struct blk_meta *temp = bst_minimum((*curr)->right);

        // Copy the inorder successor's content to this node
        (*curr)->key_address = temp->key_address;
        (*curr)->size = temp->size;

        // Delete the inorder successor
        bst_delete(&((*curr)->right), temp);
    }
}

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

    ret->size = (len * page_size) - sizeof(struct blk_meta);
    ret->key_address = (size_t)ret; // Use the address as the key.

    bst_insert(&blka->meta, ret);
    return ret;
}

void blka_free(struct blk_meta *blk) {
    munmap(blk, (blk->size + sizeof(struct blk_meta)));
}

void blka_remove(struct blk_allocator *blka, struct blk_meta *blk) {
    // Implement BST delete logic here
    bst_delete(&blka->meta, blk);
}