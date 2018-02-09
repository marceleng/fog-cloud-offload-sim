#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <stdlib.h>

typedef struct request {
        size_t id;
        size_t content;
        double arrival;
        double latest_timestamp;
        char log[512];
        int buffer_pos;
} request_t;

size_t request_counter;

#endif
