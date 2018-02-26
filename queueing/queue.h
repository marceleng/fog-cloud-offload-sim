#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>

#include "mg1ps.h"
#include "mginf.h"
#include "log_sink.h"
#include "zipfgen.h"
#include "file_logger.h"
#include "ter_counter.h"

#include "request.h"

typedef struct queue queue_t;

void queue_free (queue_t * queue, int free_underlying_struct);

/* Interactions with a single queue */
void queue_arrival (queue_t *queue, request_t *request);
request_t *queue_pop_next_exit(queue_t *queue);
void queue_update_time (queue_t *queue, double time);
double queue_next_exit (queue_t *queue);

char * queue_get_name(queue_t *queue);

/* Integrators */
queue_t * queue_from_mg1ps (mg1ps *, char *);
queue_t * queue_from_mginf (mginf *, char *);
queue_t * queue_from_log_sink(log_sink *, char *);
queue_t * queue_from_zipfgen (zipfgen *z, char *name);
queue_t * queue_from_file_logger (file_logger *log, char *name);
queue_t * queue_from_ter_counter (ter_counter *log, char *name);

#endif
