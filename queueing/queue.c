#include <stdlib.h>
#include <math.h> //INFINITY
#include "queue.h"

struct queue {
        void * queue;
        void (*arrival) (void * queue, void * request, double job_size);
        void *(*pop_next_exit) (void * queue);
        void (*update_time) (void * queue, double time);
        double (*next_exit) (void * queue);
        struct queue *(*output_queue) (void * request, double *job_size);
};

void queue_arrival (queue_t *queue, void *request, double job_size)
{
        queue->arrival(queue->queue, request, job_size);
}

void *queue_pop_next_exit(queue_t *queue)
{
        return queue->pop_next_exit(queue->queue);
}

void queue_update_time (queue_t *queue, double time)
{
        queue->update_time(queue->queue, time);
}

double queue_next_exit (queue_t *queue)
{
        return queue->next_exit(queue->queue);
}

//Pops and forwards the next request according to the output selection
void *queue_fwd_next_request(queue_t *queue)
{
        void * ret = queue->pop_next_exit(queue->queue);
        double job_size = 0; //TODO: how to get the job size?
        queue_t * next = queue->output_queue(ret, &job_size);

        queue_arrival(next, ret, job_size);
        
        return ret;
}

void queue_set_output_selection (queue_t* queue,
                queue_t *(*output_selection)(void *request, double *job_size))
{
        queue->output_queue = output_selection;
}


queue_t * queue_net_find_next_exit(int number_of_queues, queue_t **queues, double *time)
{
        *time = INFINITY;
        queue_t * ret = NULL;
        for(int i=0; i<number_of_queues; i++) {
                double next = queue_next_exit(queues[i]);
                if (next < *time) {
                        ret = queues[i];
                        *time = next;
                }
        }
        return ret;
}

double queue_net_make_next_update(int number_of_queues, queue_t **queues)
{
        double time = INFINITY;
        queue_t *next_queue = queue_net_find_next_exit(number_of_queues, queues, &time);
        if (next_queue) {
                queue_net_update_time(number_of_queues, queues, time);
                queue_fwd_next_request(next_queue);
        }
        return time;
}

void queue_net_update_time(int number_of_queues, queue_t **queues, double time)
{
        for (int i=0; i<number_of_queues; i++) {
                queue_update_time(queues[i], time);
        }
}

/*
 * MGINF integration
 */
static void _queue_mginf_arrival(void * queue, void * request, double job_size)
{
        mginf_arrival((mginf *) queue, request, job_size);
}

static void * _queue_mginf_pop_next_exit(void * queue)
{
        return mginf_reach_next_process((mginf *) queue);
}

static void _queue_mginf_update_time ( void* queue, double time)
{
        mginf_remove_time((mginf *) queue, time);
}

static double _queue_minf_next_exit (void * queue)
{
        return mginf_next_process((mginf *) queue, (void **) NULL);
}

queue_t * queue_from_mginf (mginf * queue)
{
        queue_t * ret = (queue_t *) sizeof(queue_t);

        ret->queue = queue;
        ret->arrival = _queue_mginf_arrival;
        ret->pop_next_exit = _queue_mginf_pop_next_exit;
        ret->update_time = _queue_mginf_update_time;
        ret->next_exit = _queue_minf_next_exit;
        ret->output_queue = NULL;

        return ret;
}


static void _queue_mg1ps_arrival(void * queue, void * request, double job_size)
{
        mg1ps_arrival((mg1ps *) queue, request, job_size);
}

static void * _queue_mg1ps_pop_next_exit(void * queue)
{
        return mg1ps_reach_next_process((mg1ps *) queue);
}

static void _queue_mg1ps_update_time ( void* queue, double time)
{
        mg1ps_remove_time((mg1ps *) queue, time);
}

static double _queue_m1ps_next_exit (void * queue)
{
        return mg1ps_next_process((mg1ps *) queue, (void **) NULL);
}

queue_t * queue_from_mg1ps (mg1ps * queue)
{
        queue_t * ret = (queue_t *) sizeof(queue_t);

        ret->queue = queue;
        ret->arrival = _queue_mg1ps_arrival;
        ret->pop_next_exit = _queue_mg1ps_pop_next_exit;
        ret->update_time = _queue_mg1ps_update_time;
        ret->next_exit = _queue_m1ps_next_exit;
        ret->output_queue = NULL;

        return ret;
}

/*
 * LOGGING SINK
 */
queue_t *queue_logging_sink_alloc (size_t log_size)
{
        queue_t *ret = (queue_t *) malloc(sizeof(queue_t));
        ret->queue = (void *)

        return ret;
}
