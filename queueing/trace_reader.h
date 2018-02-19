#ifndef _TRACE_READER_H_
#define _TRACE_READER_H_

#include <stdlib.h>

typedef struct trace_reader trace_reader;

trace_reader *tr_alloc(char *filename);
void tr_free(trace_reader *tr);

double tr_read_next_arrival(trace_reader *tr, size_t *key);
double tr_pop_next_arrival(trace_reader *tr, size_t *key);
#endif
