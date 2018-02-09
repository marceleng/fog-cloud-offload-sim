#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h> //INFINITY
#include <string.h>

#include "helpers/log.h"

#include "request.h"
#include "queue.h"

struct queue {
        char * name;
        void * queue;
        void (*arrival) (void * queue, request_t * request);
        request_t *(*pop_next_exit) (void * queue);
        void (*update_time) (void * queue, double time);
        double (*next_exit) (void * queue);
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

char *queue_get_name (queue_t *queue)
{
        return queue->name;
}

void queue_arrival (queue_t *queue, request_t *request)
{
        queue->arrival(queue->queue, request);
}

request_t *queue_pop_next_exit(queue_t *queue)
{
        request_t *ret = NULL;
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


/*
 * MGINF integration
 */
static void _queue_mginf_arrival(void * queue, request_t * request)
{
        mginf_arrival((mginf *) queue, (void *) request);
}

static request_t * _queue_mginf_pop_next_exit(void * queue)
{
        return (request_t *) mginf_reach_next_process((mginf *) queue);
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
        ret->free = _queue_mginf_free;

        return ret;
}


static void _queue_mg1ps_arrival(void * queue, request_t * request)
{
        mg1ps_arrival((mg1ps *) queue, (void *) request);
}

static request_t * _queue_mg1ps_pop_next_exit(void * queue)
{
        return (request_t *) mg1ps_reach_next_process((mg1ps *) queue);
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
        ret->free = _queue_mg1ps_free;

        return ret;
}

/*
 * LOGGING SINK
 */
static void _queue_log_sink_arrival(void *queue, request_t *request)
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
        ret->free = _queue_log_sink_free;

        return ret;
}

/*
 * ZIPFGEN ALLOCATOR
 */
static request_t *_queue_zipfgen_pop_next_exit (void * queue)
{
        request_t *request = NULL;
        size_t key;
        zipfgen_pop_next_arrival((zipfgen *) queue, &key);
        if (key != SIZE_MAX) {

                request = (request_t *) malloc(sizeof(request_t));
        
                memset(request, 0, sizeof(request_t));

                request->id = request_counter++;
                request->arrival = -1;
                request->content = key;
        }
        return request;
}

static double _queue_zipfgen_next_exit (void * queue)
{
        return zipfgen_read_next_arrival((zipfgen *) queue, NULL);
}

static void _queue_zipfgen_add_time(void *queue, double time)
{
        zipfgen_add_time((zipfgen *) queue, time);
}

static void _queue_zipfgen_free(void *queue)
{
        zipfgen_free((zipfgen *) queue);
}

queue_t * queue_from_zipfgen (zipfgen *z, char *name)
{
        queue_t *ret = _queue_alloc(name);
        ret->queue = z;
        ret->arrival = NULL;
        ret->pop_next_exit = _queue_zipfgen_pop_next_exit;
        ret->update_time = _queue_zipfgen_add_time;
        ret->next_exit = _queue_zipfgen_next_exit;
        ret->free = _queue_zipfgen_free;

        return ret;
}


/*
 * FILE LOGGER
 */

static void _queue_file_logger_arrival(void *queue, request_t *request)
{
        file_logger_arrival((file_logger *) queue, request);
}

static void _queue_file_logger_free(void *queue)
{
        file_logger_free((file_logger *) queue);
}

queue_t * queue_from_file_logger (file_logger *log, char *name)
{
        queue_t *ret = _queue_alloc(name);
        ret->queue = log;
        ret->arrival = _queue_file_logger_arrival;
        ret->pop_next_exit = NULL;
        ret->update_time = NULL;
        ret->next_exit = NULL;
        ret->free = _queue_file_logger_free;

        return ret;
}


