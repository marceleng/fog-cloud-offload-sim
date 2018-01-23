#include <stdlib.h>
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
        return queue->arrival(queue->queue, request, job_size);
}

void *queue_pop_next_exit(queue_t *queue)
{
        return queue->pop_next_exit(queue->queue);
}

void queue_update_time (queue_t *queue, double time)
{
        return queue->update_time(queue->queue, time);
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


/*
 * MG1PS integration
 */

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
