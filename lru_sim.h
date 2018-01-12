#ifndef _LRU_SIM_H_
#define _LRU_SIM_H_

#include <stdlib.h>
#include "rbtree.h"

typedef struct lru_filter lru_filter;

int lru_update(size_t entry, lru_filter *lru);
int lru_process_function(rbtree **pqueue, lru_filter *lru, double lambda);
lru_filter *lru_alloc (size_t lru_size, size_t catalogue_size);
void lru_free(lru_filter *lru);
#endif
