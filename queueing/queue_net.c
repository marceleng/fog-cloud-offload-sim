#include <math.h> //INFINITY

#include "request.h"
#include "queue_net.h"

#include "helpers/log.h"

struct queue_net {
        double time;
        size_t number_of_queues;
        queue_t **queues;
        queue_t *(*transition) (queue_t *exited_queue, request_t *request);
};

queue_net *queue_net_alloc(size_t number_of_queues, queue_t **queues, queue_t *(*transition) (queue_t *, request_t *))
{
        queue_net *ret = (queue_net *) malloc(sizeof(queue_net));

        ret->time = 0;
        ret->number_of_queues = number_of_queues;
        ret->queues = queues;
        ret->transition = transition;
        
        return ret;
}

queue_t * queue_net_find_next_exit(queue_net *qn, double *time)
{
        *time = INFINITY;
        queue_t * ret = NULL;
        for(size_t i=0; i<qn->number_of_queues; i++) {
                double next = queue_next_exit(qn->queues[i]);
                if (next >= 0 && next < *time) {
                        ret = qn->queues[i];
                        *time = next;
                }
        }
        return ret;
}

double queue_net_make_next_update(queue_net *qn)
{
        double time;
        queue_t *orig_queue, *dest_queue;
        request_t * req;
        orig_queue = queue_net_find_next_exit(qn, &time);
        if (orig_queue) {
                queue_net_add_time(qn, time);
                req = queue_pop_next_exit(orig_queue);
                if(req->arrival < 0) { // We're entering the queue
                        req->arrival = qn->time;
                }
                req->latest_timestamp = qn->time;
                dest_queue = qn->transition(orig_queue, req);
                LOG(LOG_INFO, "Moving request %zu from %s to %s at time %.2f\n",
                                req->id, queue_get_name(orig_queue),
                                queue_get_name(dest_queue),
                                qn->time);
                queue_arrival(dest_queue, req);
        }
        else {
                LOG(LOG_INFO,"No more transitions\n");
                time = -1;
        }
        return time;
}

void queue_net_add_time(queue_net *qn, double time)
{
        qn->time += time;
        for (size_t i=0; i<qn->number_of_queues; i++) {
                queue_update_time(qn->queues[i], time);
        }
}

double queue_net_get_time(queue_net *qn)
{
        return qn->time;
}
