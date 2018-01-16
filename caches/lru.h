#ifndef _LRU_SIM_H_
#define _LRU_SIM_H_

#include <stdlib.h>

typedef struct lru_filter lru_filter;

int lru_update(size_t entry, lru_filter *lru);
lru_filter *lru_alloc (size_t lru_size, size_t catalogue_size);
void lru_free(lru_filter *lru);
#endif
