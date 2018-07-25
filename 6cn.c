#include <string.h>
#include <stdio.h>

#include "caches/lru.h"
#include "queueing/zipfgen.h"

//args: catalogue size, alpha, cache size, filter size
int main (int argc, char *argv[]) {

        if (argc != 5) {
                fprintf (stderr, "Usage: ./ats_lru catalogue_size alpha cache_size filter_size\n");
                return -1;
        }

        setbuf(stdout, NULL);

        size_t catalogue_size = strtoul(argv[1], NULL, 10);
        double alpha = strtod(argv[2], NULL);
        size_t cache_size = (size_t) (strtod(argv[3], NULL) * catalogue_size);
        size_t filter_size = (size_t) (strtod(argv[4], NULL) * catalogue_size);

        size_t nb_arrivals = 10000000000;

        printf("Initialization...");
        zipfgen *source = zipfgen_alloc(alpha, catalogue_size, 1, nb_arrivals);
        lru_filter *cache = lru_alloc(cache_size, catalogue_size);
        lru_filter *filter = lru_alloc(filter_size, catalogue_size);
        printf(" Done\n");

        size_t next_content = 0;
        size_t counter=0;
        size_t hit_counter=0;

        printf("Step 1/%zu", nb_arrivals);

        while(zipfgen_pop_next_arrival(source, &next_content) >= 0)
        {
                if (++counter % 10000 == 0) {
                        printf("\rStep %zu/%zu", counter, nb_arrivals);
                }
                if (lru_update(filter, next_content)) {
                        hit_counter += lru_update(cache, next_content);
                }
        }
        printf("\n");

        printf("%f: %f\n", alpha, ((double) hit_counter) / (double) catalogue_size);

        /*
        size_t nb_objects_filter = 0;
        size_t nb_objects_cache = 0;

        size_t *filter_objs = lru_toarr(filter, &nb_objects_filter);
        size_t *cache_objs = lru_toarr(cache, &nb_objects_cache);

        lru_free(cache);
        lru_free(filter);
        zipfgen_free(source);
        printf("Done with the simulation, now exporting results\n");

        char filename[64];
        sprintf(filename, "filter_%zu_%f_%zu_%zu", catalogue_size, alpha, cache_size, filter_size);

        FILE * ffilter = fopen(filename,"w");
        for (size_t i=0; i<nb_objects_filter; i++) {
                fprintf(ffilter, "%zu\n", filter_objs[i]);
        }
        fclose(ffilter);

        sprintf(filename, "cache_%zu_%f_%zu_%zu", catalogue_size, alpha, cache_size, filter_size);
        FILE *fcache = fopen(filename, "w");
        for (size_t i=0; i<nb_objects_cache; i++) {
                fprintf(fcache, "%zu\n", cache_objs[i]);
        }
        fclose(fcache);

        free(filter_objs);
        free(cache_objs);
        */
}
