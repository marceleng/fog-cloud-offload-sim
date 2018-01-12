#ifndef _ZIPF_H_
#define _ZIPF_H_

#include <stdlib.h>
#include <stdint.h>

size_t get_number_of_classes (void);
void   set_number_of_classes (size_t);
double get_alpha (void);
void   set_alpha (double);

double get_popularity (size_t);

double poisson (double lambda);

#endif
