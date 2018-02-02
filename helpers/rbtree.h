#ifndef _BST_H_
#define _BST_H_

#include <stdlib.h>

//Inserts an additional item with same key
#define POLICY_INSERT 0
// Override any previous item with the same key if it exists
//#define POLICY_OVERRIDE 1
// Does not insert if item with same key found
#define POLICY_IGNORE -1

typedef double rbtree_key_t;

struct rbtree {
	void *item;
	rbtree_key_t key;

	int color;

	struct rbtree *lchild;
	struct rbtree *rchild;
	struct rbtree *parent;
};

typedef struct rbtree rbtree;

rbtree* rbtree_alloc();
void rbtree_free(rbtree* tree);

void rbtree_flatprint(rbtree* tree);
size_t rbtree_flatten(rbtree *x, void **item_buffer, rbtree_key_t *key_buffer);

rbtree* rbtree_insert(rbtree *tree, void *item, rbtree_key_t key);
rbtree* rbtree_insert_policy(rbtree *tree, void *item, rbtree_key_t key, int policy);
int rbtree_verify(rbtree *tree);
void *rbtree_search(rbtree *tree, rbtree_key_t key);

rbtree* rbtree_remove(rbtree *tree, rbtree_key_t key, void **item);

rbtree* rbtree_pop(rbtree *tree, rbtree_key_t* key, void** item);

void* rbtree_head(rbtree *tree, rbtree_key_t *key);

size_t rbtree_size(rbtree *tree);

// WARNING: func must be increasing so as to keep the tree a BST
void rbtree_apply_func_key(rbtree *tree, rbtree_key_t (*func)(rbtree_key_t, void *, void *), void *args);
void rbtree_apply_func_item(rbtree *tree, void* (*func)(rbtree_key_t, void*, void *), void *args);

int rbtree_get_default_policy();
void rbtree_set_default_policy(int policy);
#endif
