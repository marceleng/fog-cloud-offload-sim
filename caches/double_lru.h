#ifndef _DOUBLE_LRU_H_
#define _DOUBLE_LRU_H_

#include <stdlib.h>

typedef struct double_lru dlru;

dlru *dlru_alloc (size_t cache_size, size_t filter_size, size_t catalogue_size);
void dlru_free (dlru *lru);

int dlru_update (dlru *lru, size_t entry);

void dlru_resize_filter(dlru *lru, size_t new_size);
void dlru_resize_cache(dlru *lru, size_t new_size);

size_t dlru_opt_filter_size_alpha (size_t cache_size, size_t catalogue_size, double alpha);
size_t dlru_opt_filter_size_noalpha (size_t cache_size, size_t catalogue_size);

#endif
