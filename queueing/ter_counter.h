#ifndef _TER_COUNTER_H_
#define _TER_COUNTER_H_

#include <stdlib.h>

typedef struct ter_counter ter_counter;

ter_counter *ter_counter_alloc(double threshold);
void ter_counter_free(ter_counter *counter);

void ter_counter_arrival(ter_counter *counter, void *request);

size_t ter_counter_get_counter(ter_counter *counter);

#endif
