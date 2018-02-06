#include <stdio.h>
#include <string.h>

#include "file_logger.h"
#include "request.h"

#define BUFFER_SIZE 1000
#define LINE_FORMAT "%zu,%zu,%f,%f\n"

#define MAX_LINE_SIZE 50

struct file_logger {
        request_t *requests[BUFFER_SIZE];
        size_t buffer_occupancy;
        FILE *f;
};

file_logger *file_logger_alloc(char *filename)
{
        file_logger *ret = (file_logger *) malloc(sizeof(file_logger));
        memset(ret, 0, sizeof(file_logger));
        ret->f = fopen(filename, "w");
        return ret;
}

void file_logger_free(file_logger *l)
{
        if(l->buffer_occupancy) {
                file_logger_dump(l);
        }
        fclose(l->f);
        free(l);
}

void file_logger_dump(file_logger *log)
{
        size_t buffer_size = BUFFER_SIZE*MAX_LINE_SIZE;
        char buffer[buffer_size];
        memset(&buffer, 0, buffer_size);
        size_t start = 0;
        for (size_t i=0; i<log->buffer_occupancy; i++) {
                request_t *req = log->requests[i];
                start += sprintf(&(buffer[start]), LINE_FORMAT, req->id, req->content, req->arrival, req->latest_timestamp);
                free(req);
                log->requests[i] = NULL;
        }

        fputs(buffer, log->f);

        log->buffer_occupancy = 0;
}

void file_logger_arrival(file_logger *log, request_t *req)
{
        log->requests[log->buffer_occupancy++] = req;
        if(log->buffer_occupancy == BUFFER_SIZE) {
                file_logger_dump(log);
        }
}
