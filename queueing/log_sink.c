#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "helpers/log.h"
#include "helpers/rbtree.h"
#include "log_sink.h"

struct log_sink
{
        double time;
        size_t nb_of_objects;
        rbtree *objects;
};

log_sink *log_sink_alloc()
{
        log_sink *ret = (log_sink *) malloc(sizeof(log_sink));
        ret->time = 0;
        ret->nb_of_objects = 0;
        ret->objects = NULL;

        return ret;
}

void log_sink_free(log_sink *log)
{
        if(log->objects) {
                free(log->objects);
        }
        free(log);
}

void log_sink_arrival(log_sink *log, void *request)
{
        LOG(LOG_DEBUG, "Inserting object in log at time %f\n", log->time);
        log->objects = rbtree_insert(log->objects, request, log->time);
        log->nb_of_objects++;
        assert(rbtree_size(log->objects) == log->nb_of_objects);
}

size_t log_sink_get_log(log_sink *log, double *arrivals, void **requests)
{
        assert(rbtree_size(log->objects) == log->nb_of_objects);
        //TODO: debug flattening function, seems to return 0
        return rbtree_flatten(log->objects, requests, arrivals);
}

void log_sink_add_time(log_sink *log, double time)
{
        log->time += time;
        LOG(LOG_DEBUG, "Updated time by %f, new time is now %f\n", time, log->time);
}

size_t log_sink_get_number_of_objects(log_sink *log)
{
        return log->nb_of_objects;
}
