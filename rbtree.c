#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "rbtree.h"

#define COLOR_RED 1
#define COLOR_BLACK 2
#define COLOR_UNDEF 0
#define COLOR_DBLACK 3

#define LEFT 1
#define RIGHT 0

#define COLOR(tree) ((tree) ? tree->color : COLOR_BLACK)

static int DEFAULT_POLICY = POLICY_INSERT;

/*
 * BEGIN HELPERS
 */

// Sets child as a child of tree on side left
void _rbtree_update_child(rbtree * tree, rbtree * child, int left);

// Inserts node in tree using binary search. *modified=1 if the tree has been modified
rbtree * _rbtree_bst_insert(rbtree *tree, rbtree *node, int policy, int *modified);

//Family functions
rbtree * _rbtree_gp(rbtree *node);
rbtree* _rbtree_s(rbtree *node, int *side); //Side contains side of node wrt its parent
rbtree* _rbtree_u(rbtree *node);
rbtree* _rbtree_c(rbtree *node, int side); //Returns left or right child depending on side

//Rotations
rbtree* _rbtree_rotate_left(rbtree *tree);
rbtree* _rbtree_rotate_right(rbtree *tree);
rbtree *_rbtree_rotate(rbtree *tree, int side);

void _rbtree_exchange_colors(rbtree *node1, rbtree *node2);
/*
 * END HELPERS
 */

rbtree* _rbtree_gp(rbtree *node)
{
	if (!node->parent)
		return NULL;
	else 
		return node->parent->parent;
}

rbtree* _rbtree_s(rbtree *node, int *side)
{
	rbtree *ret = NULL;
	if (node->parent) {
		if (node == node->parent->lchild){
			*side = LEFT;
			ret = node->parent->rchild;
		}
		else {
			*side = RIGHT;
			ret = node->parent->lchild;
		}
	}
	return ret;
}

rbtree* _rbtree_u(rbtree *node)
{
	rbtree *ret = _rbtree_gp(node);
	if (ret) {
		if (node->parent == ret->lchild)
			ret = ret->rchild;
		else
			ret = ret->lchild;
	}
	return ret;
}

rbtree *_rbtree_c(rbtree *node, int side)
{
	if (node && (side == LEFT)) {
		return node->lchild;
	}
	else if (node && (side == RIGHT)) {
		return node->rchild;
	}
	return NULL;
}

void _rbtree_exchange_colors(rbtree *node1, rbtree *node2)
{
	assert(node1 && node2);
	int color = node1->color;
	node1->color = node2->color;
	node2->color = color;
}

void _rbtree_update_child(rbtree * tree, rbtree *child, int left)
{
	if (left) 
		tree->lchild = child;
	else
		tree->rchild = child;

	if (child)
		child->parent = tree;
}

rbtree * _rbtree_bst_insert(rbtree *tree, rbtree *node, int policy, int *modified)
{
	*modified = 1;
	if (!tree) {
		tree = node;
	}
	else if (node->key == tree->key && policy != POLICY_INSERT) {
#ifdef POLICY_OVERRIDE
		if (policy == POLICY_OVERRIDE) {
			//TODO: figure out what happens with tree->item. Should it be freed? (no)
			tree->item = node->item;
		}
#endif
		*modified = 0;
	}
	else if (node->key < tree->key) {
		_rbtree_update_child(tree, _rbtree_bst_insert(tree->lchild, node, policy, modified), LEFT);
	}
	else {
		_rbtree_update_child(tree, _rbtree_bst_insert(tree->rchild, node, policy, modified), RIGHT);
	}

	return tree;
}

rbtree* rbtree_alloc()
{
	rbtree *node = (rbtree *) malloc(sizeof(rbtree));
	memset(node, 0, sizeof(rbtree));
	return node;
}

void rbtree_free(rbtree *tree)
{
	if (tree->lchild)
		rbtree_free(tree->lchild);
	else if (tree->rchild)
		rbtree_free(tree->rchild);
	free(tree->item);
	free(tree);
}

void rbtree_set_default_policy(int policy)
{
	assert(policy == POLICY_IGNORE || policy==POLICY_INSERT
#ifdef POLICY_OVERRIDE
			|| policy==POLICY_OVERRIDE
#endif
			);

	DEFAULT_POLICY = policy;
}

int rbtree_get_default_policy()
{
	return DEFAULT_POLICY;
}

size_t rbtree_size(rbtree *tree)
{
	if (!tree)
		return 0;
	else
		return 1+rbtree_size(tree->lchild)+rbtree_size(tree->rchild);
}


rbtree* _rbtree_rotate_left(rbtree *tree)
{
	rbtree *old_root = tree->parent;

	assert(old_root);
	assert(tree == old_root->rchild);

	_rbtree_update_child(old_root, tree->lchild, RIGHT);
	if (old_root->parent) {
		int side = (old_root == old_root->parent->lchild) ? LEFT : RIGHT;
		_rbtree_update_child(old_root->parent, tree, side);
	}
	else { /* Old root has not parent -> It's the root */
		tree->parent = NULL;
	}
	_rbtree_update_child(tree, old_root, LEFT);

	return tree;
}

rbtree* _rbtree_rotate_right(rbtree *tree)
{

	rbtree *old_root = tree->parent;

	assert(old_root);
       	assert(tree == old_root->lchild);

	if (old_root->parent) {
		int side = (old_root == old_root->parent->lchild) ? LEFT : RIGHT;
		_rbtree_update_child(old_root->parent, tree, side);
	}
	else { /* Old root has not parent -> It's the root */
		tree->parent = NULL;
	}
	_rbtree_update_child(old_root, tree->rchild, LEFT);
	_rbtree_update_child(tree, old_root, RIGHT);

	return tree;
}

rbtree *_rbtree_rotate(rbtree *tree, int side)
{
	rbtree * ret = NULL;
	if (side == RIGHT) {
		ret = _rbtree_rotate_right(tree);
	}
	else {
		ret = _rbtree_rotate_left(tree);
	}
	return ret;
}

rbtree* rbtree_insert(rbtree *tree, void *item, key_t key)
{
	rbtree *ret;
	if (!tree) {
		ret = rbtree_alloc();
		ret->key = key;
		ret->item = item;
		ret->color = COLOR_BLACK; //root is black
	}
	else
		ret  = rbtree_insert_policy(tree, item, key, DEFAULT_POLICY);
	return ret;
}

rbtree* rbtree_insert_policy(rbtree *tree, void *item, key_t key, int policy)
{
	rbtree *node = rbtree_alloc();
	node->key = key;
	node->item = item;
	node->color = COLOR_RED;

	int modified;

	tree = _rbtree_bst_insert(tree, node, policy, &modified);

	if (!modified) {
		free(node);
	}
	else {
		while (node->parent && node->parent->color == COLOR_RED) {
			rbtree *u = _rbtree_u(node), *gp = _rbtree_gp(node), *p = node->parent;
			if(u && u->color==COLOR_RED) {//parent and uncle are red-> switch them to black
				node->parent->color = COLOR_BLACK;
				u->color = COLOR_BLACK;
				gp->color = COLOR_RED;
				node = gp;
			}
			else {
				if(p==gp->lchild) {
					if (node==p->rchild) {
						_rbtree_rotate_left(node);	
						node = node->lchild;
						p = node->parent;
					}
					_rbtree_rotate_right(p);
					node->color = COLOR_RED;
					p->color = COLOR_BLACK;
					p->rchild->color = COLOR_RED;
				}
				else { /* p==gp->rchild */
					if(node==p->lchild) {
						_rbtree_rotate_right(node);
						node = node->rchild;
						p = node->parent;
					}
					_rbtree_rotate_left(p);
					node->color = COLOR_RED;
					p->color = COLOR_BLACK;
					p->lchild->color = COLOR_RED;
				}
			}
		}

		while (tree->parent)
			tree = tree->parent;

		tree->color = COLOR_BLACK; //Root must be black
	}
	return tree;
}

void _rbtree_flatten(rbtree *x, void **item_buffer, key_t *key_buffer, size_t *count)
{
	if (x) {
		_rbtree_flatten(x->lchild, item_buffer, key_buffer, count);
		key_buffer[*count] = x->key;
		item_buffer[(*count)++] = x->item;
		_rbtree_flatten(x->rchild, item_buffer, key_buffer, count);
	}
}

size_t rbtree_flatten(rbtree *x, void **item_buffer, key_t *key_buffer)
{
	size_t count = 0;
	_rbtree_flatten(x, item_buffer, key_buffer, &count);
	return count;
}

void rbtree_flatprint(rbtree *x)
{
	if(x) {
		rbtree_flatprint(x->lchild);
		printf("%f ", x->key);
		rbtree_flatprint(x->rchild);
	}
}

rbtree* _rbtree_search(rbtree *tree, key_t key) {
	if (!tree) {
		return NULL;
	}
	else if (tree->key == key) {
		return tree;
	}
	else if (tree->key > key && tree->lchild) {
		return _rbtree_search(tree->lchild, key);
	}
	else if (tree->key <= key && tree->rchild) {
		return _rbtree_search(tree->rchild, key);
	}
	return NULL;
}

void *rbtree_search(rbtree *tree, key_t key) {
	rbtree *ret = _rbtree_search(tree, key);
	return ((ret) ? ret->item : NULL);
}

int rbtree_verify(rbtree *tree) {
	if (!tree) {
		return 1;
	}

	if (tree->color == COLOR_RED){
		if(COLOR(tree->lchild) == COLOR_RED) {
			printf("Left child of node %f is also red\n", tree->key);
			return 0;
		}
		if(COLOR(tree->rchild) == COLOR_RED) {
			printf("Right child of node %f is also red\n", tree->key);
			return 0;
		}
	}

	int nb_black_l = rbtree_verify(tree->lchild);
	int nb_black_r = rbtree_verify(tree->rchild);

	if (nb_black_l <= 0 || nb_black_r <= 0) {
		return 0;
	}

	if (nb_black_l != nb_black_r) {
		printf("Black violation at node %f\n", tree->key);
		return 0;
	}

	return ((tree->color == COLOR_RED) ? nb_black_l : nb_black_l + 1);
}

rbtree *_rbtree_find_successor (rbtree *tree)
{
	if (!tree->rchild) {
		return tree;
	}
	rbtree *cur_node = tree->rchild;
	while (cur_node->lchild) {
		cur_node = cur_node->lchild;
	}
	return cur_node;
}

rbtree *_rbtree_remove(rbtree *node, void **item)
{
	assert(node);
	//Ref: https://www.geeksforgeeks.org/red-black-tree-set-3-delete-2/
	rbtree *ret = node;
	rbtree *child;
	if(item) { *item = node->item; }
	
	//We place ourself in the situation where the node only has a right child
	if (node->lchild && node->rchild) {
		rbtree *succ = _rbtree_find_successor(node);
		node->key = succ->key;
		node->item = succ-> item;
		node = succ;
	}

	child = (node->rchild) ? node->rchild : node->lchild;

	//Its parent
	rbtree *parent = node->parent;
	//Its sibling
	int side;
	rbtree *sibling = _rbtree_s(node, &side);

	if (node->parent) { //First we remove the node from the graph
		_rbtree_update_child(node->parent, child, side);
	}
	else { //No parent means we're at the root
		if (child) { child->parent = NULL; }
		ret = child;
	}
	if ((COLOR(node) == COLOR_RED) || (COLOR(child) == COLOR_RED)) {
		/*
		 * If one of the two nodes is red, we move the child up
		 * and set it Black without loss of the red/black property
		 */

		if (child) { //we make sure the new node is black
			child->color = COLOR_BLACK;
		}
	}

	else { /* Both nodes are black, this is the annoying case */
		/*
		 * If both nodes were black, we tag the new node as double black
		 * Then we rebalance the graph
		 */
		//The double-black node
		rbtree *cur_node = child;
		int balanced = 0;

		while((parent) && (!balanced)) {
			
			/* If sibling is red, we rotate so that sibling becomes black */
			if (COLOR(sibling) == COLOR_RED) {
				_rbtree_rotate(sibling, side);
				_rbtree_exchange_colors(sibling, parent);
				sibling = _rbtree_c(parent, !side);
			} /* EndIf COLOR(sibling) == COLOR_RED */

			/* Now COLOR(sibling) == COLOR_BLACK */

			// First let's look for a red nephew
			if (COLOR(_rbtree_c(sibling,!side)) == COLOR_RED) {
				/* Left-left or Right-right */
				_rbtree_c(sibling,!side)->color = COLOR_BLACK;
				_rbtree_rotate(sibling, side);
				_rbtree_exchange_colors(parent, sibling);
				balanced = 1;
			}
			else if (COLOR(_rbtree_c(sibling, side)) == COLOR_RED) {
				/*Left-right or Right-left */
				rbtree *nephew = _rbtree_c(sibling, side);
				nephew->color = COLOR_BLACK;
				_rbtree_rotate(nephew, !side);
				_rbtree_rotate(nephew, side);
				_rbtree_exchange_colors(parent, nephew);
				balanced = 1;
			}
			else { /* Siblings and nephews are black */
				sibling->color = COLOR_RED;
				if(parent->color == COLOR_BLACK) { /* Parent becomes double black */
					cur_node = parent;
				}
				else { /* parent becomes single Black, I'm done */
					parent->color = COLOR_BLACK;
					balanced = 1;
				}
			}

			parent = (balanced) ? parent : cur_node->parent;
			sibling = (balanced) ? sibling : _rbtree_s(cur_node, &side);
		} /* EndWhile(parent) */
		if (!parent) { ret = cur_node; }
		else { ret = parent; }

	} /* EndIf */

	while(ret && ret->parent) {
		ret = ret->parent;
	}

	free(node); //Finally we destroy the node

	return ret;
}

rbtree* rbtree_remove(rbtree *tree, key_t key, void **item)
{
	//First let's search the node
	rbtree *node = _rbtree_search(tree, key);
	if (node) {
		return _rbtree_remove(node, item);
	}
	return tree;
}

rbtree *rbtree_pop(rbtree *tree, key_t *key, void **item)
{
	rbtree *node = tree;
	if (!node) { return NULL; }
	while (node->lchild) { node = node->lchild; }
	
	if(key) { *key = node->key; }
	
	return _rbtree_remove(node, item);
}

void* rbtree_head(rbtree *tree, key_t *key)
{
	rbtree *node = tree;
	if (!node) { return NULL; }
	while (node->lchild) { node = node->lchild; }
        
        if(key) { *key = node->key; }
        return node->item;
	
}
void rbtree_apply_func_key(rbtree *tree, key_t (*func)(key_t, void*, void *), void *args)
{
        if(tree) {
                tree->key = func(tree->key, tree->item, args);
                rbtree_apply_func_key(tree->lchild, func, args);
                rbtree_apply_func_key(tree->rchild, func, args);
        }
        return;
}

void rbtree_apply_func_item(rbtree *tree, void * (*func)(key_t, void*, void *), void *args)
{
        if(tree) {
                tree->item = func(tree->key, tree->item, args);
                rbtree_apply_func_item(tree->lchild, func, args);
                rbtree_apply_func_item(tree->rchild, func, args);
        }
        return;
}
