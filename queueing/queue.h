#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "queueing/mg1ps.h"

typedef struct queue queue_t;

void queue_arrival (queue_t *queue, void *request, double job_size);
void *queue_pop_next_exit(queue_t *queue);
void queue_update_time (queue_t *queue, double time);
double queue_next_exit (queue_t *queue);
//Pops and forwards the next request according to the output selection
void *queue_fwd_next_request(queue_t *queue);

void queue_set_output_selection (queue_t *queue,
                queue_t *(*output_selection)(void *request, double *job_size));


/* Integrators */
queue_t * queue_from_mg1ps (mg1ps *);

#endif
