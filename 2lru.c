#include <stdio.h>
#include <assert.h>
#include "zipf.h"
#include "lru_sim.h"
#include "rbtree.h"

#define printr(...) fprintf(stderr, __VA_ARGS__)

rbtree* initialize_arrivals (rbtree * queue, size_t catalogue_size, double lambda)
{
        for (size_t i=0; i<catalogue_size; i++) {
                double next = poisson(lambda * get_popularity(i));
                queue = rbtree_insert(queue, (void *) i, next);
        }
        return queue;
}

void initialize_zipf (double alpha, size_t catalogue_size)
{
        //printr("Initializing\n");
        set_alpha(alpha);
        set_number_of_classes(catalogue_size);
}

void lru_test (double lambda, size_t catalogue_size, size_t lru_size, size_t number_of_queries)
{
        lru_filter * cache = lru_alloc(lru_size, catalogue_size);

        rbtree *arrivals = rbtree_alloc();
        arrivals = initialize_arrivals(arrivals, catalogue_size, lambda);

        //printr("Running iterations\n");
        size_t count = 0;
        for (size_t i=0; i<number_of_queries; i++) {
                size_t catalogue_key = 0;
                double time_key = 0;
                arrivals = rbtree_pop(arrivals, &time_key, (void **) &catalogue_key);
                double lambdak = get_popularity(catalogue_key) * lambda;
                double next_time = poisson(lambdak) + time_key;
                int hit = 0;
                
                //printf("%zu ", catalogue_key);
                if (lru_update(catalogue_key, cache)) {
                        hit = 1;
                }
                //printf ("%d\n", hit);
                arrivals = rbtree_insert(arrivals, (void *) catalogue_key, next_time);
                count += hit;
        }

        printf("(%f, %zu, %zu, %zu): %f\n", lambda, catalogue_size, lru_size, number_of_queries, (double) count / (double) number_of_queries);


}

void lru2_test (double lambda, size_t catalogue_size, size_t lru_size, size_t filter_size, size_t number_of_queries)
{
        lru_filter * filter = lru_alloc(filter_size, catalogue_size);
        lru_filter * cache = lru_alloc(lru_size, catalogue_size);

        rbtree *arrivals = rbtree_alloc();
        arrivals = initialize_arrivals(arrivals, catalogue_size, lambda);

        //printr("Running iterations\n");
        size_t count = 0;
        for (size_t i=0; i<number_of_queries; i++) {
                size_t catalogue_key = 0;
                double time_key = 0;
                arrivals = rbtree_pop(arrivals, &time_key, (void **) &catalogue_key);
                double lambdak = get_popularity(catalogue_key) * lambda;
                double next_time = poisson(lambdak) + time_key;
                int hit = 0;
                
                //printf("%zu ", catalogue_key);
                if (lru_update(catalogue_key, filter) && lru_update(catalogue_key, cache)) {
                        hit = 1;
                }
                //printf ("%d\n", hit);
                arrivals = rbtree_insert(arrivals, (void *) catalogue_key, next_time);
                count += hit;
        }

        printf("(%f, %zu, %zu, %zu, %zu): %f\n", lambda, catalogue_size, lru_size, filter_size, number_of_queries, (double) count / (double) number_of_queries);

}
//args: alpha, catalogue size, lru size, filter size, nb of queries
int main (int argc, char *argv[])
{

        assert(argc == 6);
        (void) argc;

        double alpha = strtod(argv[1], NULL);
        double lambda = 1;
        size_t catalogue_size = strtoul(argv[2], NULL, 10);
        size_t lru_size = strtoul(argv[3], NULL, 10);
        size_t filter_size = strtoul(argv[4], NULL, 10);
        size_t number_of_queries = strtoul(argv[5], NULL, 10);

        initialize_zipf(alpha, catalogue_size);

        //lru_test(lambda, catalogue_size, lru_size, number_of_queries);
        lru2_test(lambda, catalogue_size, lru_size, filter_size, number_of_queries);
        return 0;
}
