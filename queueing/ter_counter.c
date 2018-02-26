#include "ter_counter.h"
#include "request.h"

struct ter_counter {
        double threshold;
        size_t counter;
};

ter_counter *ter_counter_alloc(double threshold)
{
        ter_counter *ret = (ter_counter *) malloc(sizeof(ter_counter));
        ret->counter=0;
        ret->threshold=threshold;
        return ret;
}

void ter_counter_free(ter_counter *counter)
{
        free(counter);
}

void ter_counter_arrival(ter_counter *counter, void *request)
{
        request_t *req = (request_t *) request;
        if ((req->latest_timestamp - req->arrival) > counter->threshold) {
                counter->counter++;
        }

        free(req);
}

size_t ter_counter_get_counter(ter_counter *counter)
{
        return counter->counter;
}
