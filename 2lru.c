#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "caches/lru.h"
#include "queueing/zipfgen.h"
#include "queueing/trace_reader.h"



//args catalogue size, lru size, filter size, [traces filename | zipf alpha number_of_queries]
int main (int argc, char *argv[])
{

        if (!(argc == 6 && strcmp(argv[4], "traces")==0) && !(argc==7 && strcmp(argv[4],"zipf")==0)) {
                fprintf (stderr, "Usage: ./2lru catalogue_size lru_size filter_size [traces trace_file | zipf alpha number_of_queries]\n");
                return -1;
        }

        size_t catalogue_size = strtoul(argv[1], NULL, 10);
        size_t lru_size = strtoul(argv[2], NULL, 10);
        size_t filter_size = strtoul(argv[3], NULL, 10);

        lru_filter * filter = lru_alloc(filter_size, catalogue_size);
        lru_filter * dlrucache = lru_alloc(lru_size, catalogue_size);
        lru_filter * lrucache = lru_alloc(lru_size, catalogue_size);

        size_t hit_lru = 0;
        size_t hit_2lru = 0;
        double nb_arrivals = 0;
        if (strcmp(argv[4],"traces") == 0) {
                char *trace_file = argv[5];
                trace_reader *tr = tr_alloc(trace_file);
                size_t next_content = 0;
                while (tr_pop_next_arrival(tr, &next_content) >= 0)
                {
                        if(lru_update(lrucache, next_content)) {
                                hit_lru++;
                        }
                        if(lru_update(filter, next_content) && lru_update(dlrucache, next_content)) {
                                hit_2lru++;
                        }
                        nb_arrivals += 1;
                }
                tr_free(tr);
        }
        else {
                double alpha = strtod(argv[5], NULL);
                double lambda = 1;
                size_t number_of_queries = strtoul(argv[6], NULL, 10);
                zipfgen *gen = zipfgen_alloc(alpha, catalogue_size, lambda, number_of_queries);
                size_t next_content = 0;
                while (zipfgen_pop_next_arrival(gen, &next_content) >= 0)
                {
                        if(lru_update(lrucache, next_content)) {
                                hit_lru++;
                        }
                        if(lru_update(filter, next_content) && lru_update(dlrucache, next_content)) {
                                hit_2lru++;
                        }
                        nb_arrivals += 1;
                }

                zipfgen_free(gen);
        }

        lru_free(filter);
        lru_free(dlrucache);
        lru_free(lrucache);

        printf("LRU: %f; 2LRU: %f\n", hit_lru/nb_arrivals, hit_2lru/nb_arrivals);
        return 0;
}
