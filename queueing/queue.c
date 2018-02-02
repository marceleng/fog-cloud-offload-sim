#include <stdio.h>
#include <stdlib.h>
#include <math.h> //INFINITY
#include <string.h>

#include "helpers/log.h"

#include "queue.h"

struct queue {
        char * name;
        void * queue;
        void (*arrival) (void * queue, void * request);
        void *(*pop_next_exit) (void * queue);
        void (*update_time) (void * queue, double time);
        double (*next_exit) (void * queue);
        struct queue *(*output_queue) (void * request);
        void (*free) (void * queue);
};

static queue_t *_queue_alloc(char * name)
{
        queue_t * ret = (queue_t *) malloc(sizeof(queue_t));
        memset(ret, 0, sizeof(queue_t));
        ret->name = (char *) malloc(sizeof(char)*(strlen(name)+1));
        memset(ret->name, 0, sizeof(char)*(strlen(name)+1));
        strcpy(ret->name, name);
        return ret;
}

void queue_free (queue_t * queue, int free_queue)
{
        free(queue->name);
        if (free_queue && (queue->free)) {
                queue->free(queue->queue);
        }
        free(queue);
}

void queue_arrival (queue_t *queue, void *request)
{
        queue->arrival(queue->queue, request);
}

void *queue_pop_next_exit(queue_t *queue)
{
        void *ret = NULL;
        if (queue->pop_next_exit) {
                ret = queue->pop_next_exit(queue->queue);
        }
        return ret;
}

void queue_update_time (queue_t *queue, double time)
{
        if(queue->update_time) {
                queue->update_time(queue->queue, time);
        }
}

double queue_next_exit (queue_t *queue)
{
        double ret = INFINITY;
        if (queue->next_exit) {
                ret = queue->next_exit(queue->queue);
        }
        return ret;
}

//Pops and forwards the next request according to the output selection
void *queue_fwd_next_request(queue_t *queue)
{
        void * ret = queue->pop_next_exit(queue->queue);

        if (queue->output_queue) {
                queue_t * next = queue->output_queue(ret);

                LOG(LOG_DEBUG,"Forwarding request from %s to %s\n", queue->name, next->name);

                queue_arrival(next, ret);
        }
        
        return ret;
}

void queue_set_output_selection (queue_t* queue,
                queue_t *(*output_selection)(void *request))
{
        queue->output_queue = output_selection;
}


queue_t * queue_net_find_next_exit(int number_of_queues, queue_t **queues, double *time)
{
        *time = INFINITY;
        queue_t * ret = NULL;
        for(int i=0; i<number_of_queues; i++) {
                double next = queue_next_exit(queues[i]);
                if (next >= 0 && next < *time) {
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
static void _queue_mginf_arrival(void * queue, void * request)
{
        mginf_arrival((mginf *) queue, request);
}

static void * _queue_mginf_pop_next_exit(void * queue)
{
        return mginf_reach_next_process((mginf *) queue);
}

static void _queue_mginf_update_time ( void* queue, double time)
{
        mginf_remove_time((mginf *) queue, time);
}

static double _queue_mginf_next_exit (void * queue)
{
        return mginf_next_process((mginf *) queue, (void **) NULL);
}

static void _queue_mginf_free (void * queue)
{
        mginf_free((mginf *) queue);
}

queue_t * queue_from_mginf (mginf * queue, char *name)
{
        queue_t *ret = _queue_alloc(name);

        ret->queue = queue;
        ret->arrival = _queue_mginf_arrival;
        ret->pop_next_exit = _queue_mginf_pop_next_exit;
        ret->update_time = _queue_mginf_update_time;
        ret->next_exit = _queue_mginf_next_exit;
        ret->output_queue = NULL;
        ret->free = _queue_mginf_free;

        return ret;
}


static void _queue_mg1ps_arrival(void * queue, void * request)
{
        mg1ps_arrival((mg1ps *) queue, request);
}

static void * _queue_mg1ps_pop_next_exit(void * queue)
{
        return mg1ps_reach_next_process((mg1ps *) queue);
}

static void _queue_mg1ps_update_time ( void* queue, double time)
{
        mg1ps_remove_time((mg1ps *) queue, time);
}

static double _queue_mg1ps_next_exit (void * queue)
{
        return mg1ps_next_process((mg1ps *) queue, (void **) NULL);
}

static void _queue_mg1ps_free (void * queue)
{
        mg1ps_free((mg1ps *) queue);
}

queue_t * queue_from_mg1ps (mg1ps * queue, char *name)
{
        queue_t *ret = _queue_alloc(name);

        ret->queue = queue;
        ret->arrival = _queue_mg1ps_arrival;
        ret->pop_next_exit = _queue_mg1ps_pop_next_exit;
        ret->update_time = _queue_mg1ps_update_time;
        ret->next_exit = _queue_mg1ps_next_exit;
        ret->output_queue = NULL;
        ret->free = _queue_mg1ps_free;

        return ret;
}

/*
 * LOGGING SINK
 */
static void _queue_log_sink_arrival(void *queue, void *request)
{
        log_sink_arrival((log_sink *) queue, request);
}

static void _queue_log_sink_update_time (void *queue, double time)
{
        log_sink_add_time((log_sink *) queue, time);
}

static void _queue_log_sink_free(void *queue)
{
        log_sink_free((log_sink *) queue);
}

queue_t * queue_from_log_sink (log_sink *log, char *name)
{
        queue_t *ret = _queue_alloc(name);
        ret->queue = log;
        ret->arrival = _queue_log_sink_arrival;
        ret->pop_next_exit = NULL;
        ret->update_time = _queue_log_sink_update_time;
        ret->next_exit = NULL;
        ret->output_queue = NULL;
        ret->free = _queue_log_sink_free;

        return ret;
}
