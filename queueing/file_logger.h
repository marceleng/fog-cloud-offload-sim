#ifndef _FILE_LOGGER_H_
#define _FILE_LOGGER_H_

#include "request.h"

typedef struct file_logger file_logger;

file_logger *file_logger_alloc(char *filename);
void file_logger_free(file_logger *log);

void file_logger_dump(file_logger *log);

void file_logger_arrival(file_logger *log, request_t *req);
#endif
