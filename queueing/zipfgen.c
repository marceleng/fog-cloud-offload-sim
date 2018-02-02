#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "zipfgen.h"
#include "helpers/random_generators.h"
#include "helpers/rbtree.h"

struct zipfgen {
        double alpha;
        size_t catalogue_size;
        double lambda;
        long double *popularity_dist;
        rbtree *arrivals;
        double norm_factor;
};

static double _zipfgen_gen_arrival (zipfgen *z, double basetime, size_t * key)
{
        double next_arrival = basetime + exponential_generator((z->lambda * z->popularity_dist[*key])/z->norm_factor);
        z->arrivals = rbtree_insert(z->arrivals, (void *) key, next_arrival);
        return next_arrival;
}


/*
 * MEMORY HANDLING
 */

static void _zipfgen_initialize (zipfgen* z)
{
        long double sum=0, pop;
        if(!z || !(z->catalogue_size)) { return; }

        //First let's clean current values
        if (z->popularity_dist) { free(z->popularity_dist); }
        z->popularity_dist = (long double *) malloc(sizeof(long double) * z->catalogue_size);
        
        if(z->arrivals) { rbtree_free(z->arrivals); }
        z->arrivals = NULL;

        for (size_t i=0; i<z->catalogue_size; i++) {
                pop = 1. / pow(i+1, z->alpha);
                sum += pop;
                z->popularity_dist[i] = pop;
        }

        z->norm_factor = sum;

        for (size_t i=0; i<z->catalogue_size; i++) {
                //Normalize popularity
                //z->popularity_dist[i] /= sum;

                //Insert first arrival
                size_t * key = (size_t *) malloc(sizeof(size_t));
                *key = i;
                _zipfgen_gen_arrival(z, 0, key);
        }

        return;
}

zipfgen * zipfgen_alloc (double alpha, size_t catalogue_size, double lambda)
{
        zipfgen *z = (zipfgen *) malloc(sizeof (zipfgen));
        z->alpha = alpha;
        z->catalogue_size = catalogue_size;
        z->lambda = lambda;
        z->popularity_dist = NULL;
        z->arrivals = NULL;
        _zipfgen_initialize(z);
        return z;
}

void zipfgen_free (zipfgen * z)
{
        free(z->popularity_dist);
        // Cleans up next arrivals
        while (z->arrivals != NULL) {
                void * to_free = NULL;
                z->arrivals = rbtree_pop(z->arrivals, NULL, &to_free);
                free(to_free);
        }
        free(z);
}

/*
 * GETTERS AND SETTERS
 */

size_t zipfgen_get_catalogue_size (zipfgen * z)
{
        return z->catalogue_size;
}

void zipfgen_set_catalogue_size(zipfgen *z, size_t cat_size)
{
        if (z->catalogue_size != cat_size) {
                z->catalogue_size = cat_size;
                _zipfgen_initialize(z);
        }
}

double zipfgen_get_alpha (zipfgen * z)
{
        return z->alpha;
}

void zipfgen_set_alpha (zipfgen *z, double a)
{
        if(z->alpha != a) {
                z->alpha = a;
                _zipfgen_initialize(z);
        }
}

double zipfgen_get_popularity (zipfgen *z, size_t k)
{
        if (k<z->catalogue_size) {
                return z->popularity_dist[k] / z->norm_factor;
        }
        return 0;
}

/*
 * GENERATION API
 */

double zipfgen_read_next_arrival (zipfgen * z, size_t * key)
{
        double time = 0;
        if (key) {
                *key = *(size_t *)(rbtree_head(z->arrivals, &time));
        }
        return time;
}

double zipfgen_pop_next_arrival (zipfgen *z, size_t * key)
{
        double time = 0;
        size_t * key_local = NULL;
        z->arrivals = rbtree_pop(z->arrivals, &time, (void  **) &key_local);
        if(key) {
                *key = *key_local;
        }
        // We reinsert the next arrival for that key
        _zipfgen_gen_arrival(z, time, key_local);
        return time;
}
