#include "log_sink.h"

struct log_sink
{
        size_t log_size;
        size_t nb_of_objects;
        void **objects;
};

log_sink *log_sink_alloc(size_t log_size)
{
        log_sink *ret = (log_sink *) malloc(sizeof(log_sink));
        ret->log_size = log_size;
        ret->nb_of_objects = 0;
        ret->objects = (void **) malloc(sizeof(void *)*log_size);

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
        if(log->nb_of_objects < log->log_size)
        {
                log->objects[(log->nb_of_objects)++] = request;
        }
}
