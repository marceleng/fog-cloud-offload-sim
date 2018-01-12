#include <stdio.h>
#include <stdlib.h>
#include "lru_sim.h"
#include "zipf.h"

int old_main ()
{
        rbtree *pq = rbtree_alloc();
        size_t catalogue_size = 10000000;
        size_t lru_size = 10000;
        double lambda = .001;

        lru_filter *lru = lru_alloc(lru_size, catalogue_size);

        set_alpha(0.9);
        set_number_of_classes(catalogue_size);

        for (size_t i=0; i<catalogue_size; i++) {
                double next = poisson(lambda * get_popularity(i));

                pq = rbtree_insert(pq, (void *) i, next);
        }

        size_t count=0;
        size_t nb_tries = 100000000;
        for(size_t i=0; i<nb_tries; i++) {
                if(lru_process_function(&pq, lru, lambda)) {
                        count++;
                }
        }
        printf("Hit rate: %f\n", (double) count / (double) nb_tries);

        return 0;
}
