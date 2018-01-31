#ifndef _LOG_SINK_H_
#define _LOG_SINK_H_

#include <stdlib.h>

typedef struct log_sink log_sink;

log_sink *log_sink_alloc();
void log_sink_free(log_sink *log);

void log_sink_arrival(log_sink *log, void *request);

size_t log_sink_get_log(log_sink *log, double *arrivals, void **requests);

void log_sink_add_time(log_sink *log, double time);
#endif
