#include <math.h>
#include <stdio.h>

#include "double_lru.h"
#include "lru.h"

struct double_lru {
        lru_filter *filter;
        lru_filter *cache;
};

dlru *dlru_alloc (size_t cache_size, size_t filter_size, size_t catalogue_size)
{
        dlru *ret = (dlru *) malloc(sizeof(dlru));
        ret->filter = lru_alloc(filter_size, catalogue_size);
        ret->cache = lru_alloc(cache_size, catalogue_size);
        return ret;
}

void dlru_free (dlru *lru)
{
        lru_free(lru->filter);
        lru_free(lru->cache);
        free(lru);
}

int dlru_update (dlru *lru, size_t entry)
{
        int ret = 0;
        if(lru_update(lru->filter, entry)) {
                ret = lru_update(lru->cache, entry);
        }
        else {
                ret = lru_contains(lru->cache, entry);
        }
        return ret;
}

void dlru_resize_filter(dlru *lru, size_t new_size)
{
        lru_resize(lru->filter, new_size);
}

void dlru_resize_cache(dlru *lru, size_t new_size)
{
        lru_resize(lru->cache, new_size);
}

#define B 1.938
#define C 0.342
#define D 0.422
#define DEFAULT_ALPHA 1.0

size_t dlru_opt_filter_size_alpha (size_t cache_size, size_t catalogue_size, double alpha)
{
        double delta2 = (double) cache_size / (double) catalogue_size;
        double delta1 = B * pow(delta2, C*alpha + D);
        return (size_t) (delta1 * catalogue_size);
}

size_t dlru_opt_filter_size_noalpha (size_t cache_size, size_t catalogue_size)
{
        return dlru_opt_filter_size_alpha(cache_size, catalogue_size, DEFAULT_ALPHA);
}
