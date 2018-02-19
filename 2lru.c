#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "caches/double_lru.h"
#include "caches/lru.h"
#include "queueing/zipfgen.h"
#include "queueing/trace_reader.h"



//args catalogue size, lru size, filter size, [traces filename | zipf alpha number_of_queries]
int main (int argc, char *argv[])
{

        if ((argc != 6) || ((strcmp(argv[3], "traces")!=0) && (strcmp(argv[3],"zipf")!=0))) {
                fprintf (stderr, "Usage: ./2lru catalogue_size cache_size [traces alpha trace_file | zipf alpha number_of_queries]\n");
                return -1;
        }

        size_t catalogue_size = strtoul(argv[1], NULL, 10);
        size_t cache_size = strtoul(argv[2], NULL, 10);
        double alpha = strtod(argv[4], NULL);

        lru_filter * lrucache = lru_alloc(cache_size, catalogue_size);
        dlru *dlru_guideline_alpha = dlru_alloc(cache_size, dlru_opt_filter_size_alpha(cache_size, catalogue_size, alpha), catalogue_size);
        dlru *dlru_guideline_noalpha = dlru_alloc(cache_size, dlru_opt_filter_size_noalpha(cache_size, catalogue_size), catalogue_size);
        dlru *dlru_noguideline = dlru_alloc(cache_size, cache_size, catalogue_size);

        size_t hit_lru = 0;
        size_t hit_2lru_ga = 0;
        size_t hit_2lru_gna = 0;
        size_t hit_2lru_ng = 0;
        double nb_arrivals = 0;
        if (strcmp(argv[3],"traces") == 0) {
                char *trace_file = argv[5];
                trace_reader *tr = tr_alloc(trace_file);
                size_t next_content = 0;
                while (tr_pop_next_arrival(tr, &next_content) >= 0)
                {
                        if(lru_update(lrucache, next_content)) {
                                hit_lru++;
                        }
                        if (dlru_update(dlru_guideline_alpha, next_content)) {
                                        hit_2lru_ga++;
                        }
                        if (dlru_update(dlru_guideline_noalpha, next_content)) {
                                        hit_2lru_gna++;
                        }
                        if (dlru_update(dlru_noguideline, next_content)) {
                                        hit_2lru_ng++;
                        }
                        nb_arrivals += 1;
                }
                tr_free(tr);
        }
        else {
                double lambda = 1;
                size_t number_of_queries = strtoul(argv[5], NULL, 10);
                zipfgen *gen = zipfgen_alloc(alpha, catalogue_size, lambda, number_of_queries);
                size_t next_content = 0;
                while (zipfgen_pop_next_arrival(gen, &next_content) >= 0)
                {
                        if(lru_update(lrucache, next_content)) {
                                hit_lru++;
                        }
                        if (dlru_update(dlru_guideline_alpha, next_content)) {
                                        hit_2lru_ga++;
                        }
                        if (dlru_update(dlru_guideline_noalpha, next_content)) {
                                        hit_2lru_gna++;
                        }
                        if (dlru_update(dlru_noguideline, next_content)) {
                                        hit_2lru_ng++;
                        }
                        nb_arrivals += 1;
                }

                zipfgen_free(gen);
        }

        lru_free(lrucache);
        dlru_free (dlru_guideline_alpha);
        dlru_free (dlru_guideline_noalpha);
        dlru_free (dlru_noguideline);

        printf("LRU: %f; 2LRU-a: %f, 2LRU-na: %f, 2LRU-ng: %f\n", hit_lru/nb_arrivals, hit_2lru_ga/nb_arrivals, hit_2lru_gna/nb_arrivals, hit_2lru_ng/nb_arrivals);
        return 0;
}
