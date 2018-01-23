#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mg1ps.h"
#include "mginf.h"

typedef struct queue queue_t;

/* Interactions with a single queue */
void queue_arrival (queue_t *queue, void *request, double job_size);
void *queue_pop_next_exit(queue_t *queue);
void queue_update_time (queue_t *queue, double time);
void *queue_fwd_next_request(queue_t *queue);
double queue_next_exit (queue_t *queue);

void queue_set_output_selection (queue_t *queue,
                queue_t *(*output_selection)(void *request, double *job_size));

/* Interactions with a set of queues */
queue_t * queue_net_find_next_exit (int number_of_queues, queue_t **queues, double * time);
double queue_net_make_next_update(int number_of_queues, queue_t ** queues);
void queue_net_update_time(int number_of_queues, queue_t **queues, double time);


/* Integrators */
queue_t * queue_from_mg1ps (mg1ps *);
queue_t * queue_from_mginf (mginf *);

#endif
