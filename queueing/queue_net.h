#ifndef _QUEUE_NET_H_
#define _QUEUE_NET_H_

#include "queue.h"

typedef struct queue_net queue_net;

queue_net *queue_net_alloc(size_t number_of_queues, queue_t **queues, queue_t *(*transition) (queue_t *, request_t *));

queue_t * queue_net_find_next_exit (queue_net *qn, double *time);
//Returns -1 if nothing is being done
double queue_net_make_next_update(queue_net * qn);
void queue_net_add_time(queue_net *qn, double time);

double queue_net_get_time(queue_net *qn);

#endif
