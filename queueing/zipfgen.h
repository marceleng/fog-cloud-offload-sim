#ifndef _ZIPFGEN_H_
#define _ZIPFGEN_H_

#include <stdlib.h>

typedef struct zipfgen zipfgen;

zipfgen * zipfgen_alloc(double alpha, size_t catalogue_size, double lambda, size_t number_of_arrivals);
void zipfgen_free(zipfgen * generator);

size_t zipfgen_get_catalogue_size(zipfgen * generator);
void zipfgen_set_catalogue_size(zipfgen *, size_t);

double zipfgen_get_alpha(zipfgen *);
void zipfgen_set_alpha(zipfgen *, double);

double zipfgen_get_popularity (zipfgen *, size_t);

void zipfgen_add_time(zipfgen *z, double t);

double zipfgen_read_next_arrival (zipfgen * z, size_t * key);
double zipfgen_pop_next_arrival (zipfgen *z, size_t * key);

#endif
