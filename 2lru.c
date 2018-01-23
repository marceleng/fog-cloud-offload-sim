#include <stdio.h>
#include <assert.h>
#include "caches/lru.h"
#include "queueing/zipfgen.h"

#define printr(...) fprintf(stderr, __VA_ARGS__)

void lru_test (double alpha, double lambda, size_t catalogue_size, size_t lru_size, size_t number_of_queries)
{
        lru_filter * cache = lru_alloc(lru_size, catalogue_size);

        zipfgen *generator = zipfgen_alloc(alpha, catalogue_size, lambda);

        //printr("Running iterations\n");
        size_t count = 0;
        for (size_t i=0; i<number_of_queries; i++) {
                size_t catalogue_key = 0;
                double time_key = 0;
                time_key = zipfgen_pop_next_arrival(generator, &catalogue_key);
                int hit = 0;
                
                //printf("%zu ", catalogue_key);
                if (lru_update(cache, catalogue_key)) {
                        hit = 1;
                }
                //printf ("%d\n", hit);
                count += hit;
        }

        printf("(%f, %zu, %zu, %zu): %f\n", lambda, catalogue_size, lru_size, number_of_queries, (double) count / (double) number_of_queries);


}

void lru2_test (double alpha, double lambda, size_t catalogue_size, size_t lru_size,
                size_t filter_size, size_t number_of_queries)
{
        lru_filter * filter = lru_alloc(filter_size, catalogue_size);
        lru_filter * cache = lru_alloc(lru_size, catalogue_size);

        zipfgen *generator = zipfgen_alloc(alpha, catalogue_size, lambda);

        //printr("Running iterations\n");
        size_t count = 0;
        for (size_t i=0; i<number_of_queries; i++) {
                size_t catalogue_key = 0;
                double time_key = 0;
                time_key = zipfgen_pop_next_arrival(generator, &catalogue_key);
                int hit = 0;
                
                //printf("%zu ", catalogue_key);
                if (lru_update(filter, catalogue_key) && lru_update(cache, catalogue_key)) {
                        hit = 1;
                }
                //printf ("%d\n", hit);
                count += hit;
        }

        printf("(%f, %zu, %zu, %zu, %zu): %f\n", lambda, catalogue_size, lru_size, filter_size, number_of_queries, (double) count / (double) number_of_queries);

}
//args: alpha, catalogue size, lru size, filter size, nb of queries
int main (int argc, char *argv[])
{

        if (argc != 6) {
                printr ("Usage: ./2lru_sim alpha lambda catalogue_size lru_size filter_size number_of_queries\n");
                return -1;
        }

        double alpha = strtod(argv[1], NULL);
        double lambda = 1;
        size_t catalogue_size = strtoul(argv[2], NULL, 10);
        size_t lru_size = strtoul(argv[3], NULL, 10);
        size_t filter_size = strtoul(argv[4], NULL, 10);
        size_t number_of_queries = strtoul(argv[5], NULL, 10);

        //lru_test(lambda, catalogue_size, lru_size, number_of_queries);
        lru2_test(alpha, lambda, catalogue_size, lru_size, filter_size, number_of_queries);
        return 0;
}
