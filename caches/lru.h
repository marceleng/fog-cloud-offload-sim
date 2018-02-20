#ifndef _LRU_SIM_H_
#define _LRU_SIM_H_

#include <stdlib.h>

typedef struct lru_filter lru_filter;

lru_filter *lru_alloc (size_t lru_size, size_t catalogue_size);
void lru_free(lru_filter *lru);

int lru_update(lru_filter *lru, size_t entry);
int lru_contains(lru_filter *lru, size_t entry);
void lru_resize(lru_filter *lru, size_t new_size);
#endif
