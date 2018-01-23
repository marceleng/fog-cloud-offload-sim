#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "helpers/rbtree.h"
#include "caches/lru.h"

typedef struct lru_node {
        size_t key;
        struct lru_node* child;
        struct lru_node* parent;
} lru_node;

struct lru_filter {
        uint32_t lru_size; // Size of the LRU filter
        uint32_t nb_objects; // Nb of objects stored in the LRU filter
        lru_node * head; // Head of the filter
        lru_node * tail; // Tail of the filter
        uint32_t map_size; // Catalogue size
        lru_node ** map; // Mapping between content id and filter node
};

lru_filter *lru_alloc(size_t lru_size, size_t catalogue_size)
{
        lru_filter *lru = (lru_filter *) malloc(sizeof(lru_filter));
        lru->lru_size = lru_size;
        lru->nb_objects = 0;
        lru->head = NULL;
        lru->tail = NULL;
        lru->map_size = catalogue_size;
        lru->map = (lru_node **) malloc(sizeof(lru_node *) * catalogue_size);
        memset(lru->map, '\0', sizeof(lru_node *) * catalogue_size);

        return lru;
}

void lru_free(lru_filter * lru)
{
        for (size_t i=0; i<lru->map_size; i++) {
                if (lru->map[i]) {
                        free(lru->map[i]);
                }
        }
        free(lru->map);
        free(lru);
}

static lru_node *_lru_node_alloc(size_t entry)
{
        lru_node *node = (lru_node *) malloc(sizeof(lru_node));
        node->key = entry;
        node->child = NULL;
        node->parent = NULL;
        return node;
}

static lru_node* _lru_remove_node (size_t entry, lru_filter *lru)
{
        lru_node *node=lru->map[entry];
        assert (entry < lru->map_size);
        if(node) {
                if(node->parent) {
                        node->parent->child = node->child;
                }
                else { // It's the head
                        assert(node == lru->head);
                        lru->head = node->child;
                }

                if(node->child) {
                        node->child->parent = node->parent;
                }
                else { // It's the tail
                        assert(node == lru->tail);
                        lru->tail = node->parent;
                }
                lru->map[entry] = NULL;
                lru->nb_objects--;
        }
        return node;
}

__attribute__((unused)) static void _lru_free_head (lru_filter *lru)
{
        lru_node *h = lru->head;
        if (h) {
                _lru_remove_node(h->key, lru);
                free(h);
        }
}

static void _lru_free_tail (lru_filter *lru)
{
        lru_node *t = lru->tail;
        if (t) {
                assert (!t->child);
                _lru_remove_node(t->key, lru);
                assert( !(lru->tail) || !(lru->tail->child));
                assert( !t || t!=lru->tail);
                free(t);
        }
}

static void _lru_insert_head (lru_filter *lru, lru_node *node)
{
        assert(!lru->map[node->key]);

        lru_node *old_head = lru->head;
        if (old_head) {
                old_head->parent = node;
        }
        else { // Filter is empty
                lru->tail = node;
        }
        node->parent = NULL;
        node->child = old_head;
        lru->head = node;

        lru->map[node->key] = node;
        lru->nb_objects++;
}

/*
 * lru_map[i] = NULL if not in LRU, pointer, else pointer to position
 * head: head of LRU cache
 * entry: incoming entry
 * lru_size: size of LRU cache
 * returns 1 if hit, 0 if miss
 */
int lru_update(lru_filter * lru, size_t entry)
{

        int ret = 0;
        lru_node *actual_node = _lru_remove_node(entry, lru);

        if (!actual_node) { // Create one
                actual_node = _lru_node_alloc(entry);
        }
        else { // We're in the filter
                ret = 1;
        }

        _lru_insert_head (lru, actual_node);
        if (lru->nb_objects > lru->lru_size) {
                _lru_free_tail(lru);
        }
        
        return ret;
}

void lru_resize(lru_filter *lru, size_t new_size)
{
        lru->lru_size = new_size;
        while (lru->nb_objects > lru->lru_size)
        {
                _lru_free_tail(lru);
        }
}
