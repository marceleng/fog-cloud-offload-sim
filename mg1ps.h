# ifndef _MG1PS_H_
#define _MG1PS_H_

#include <stdlib.h>

typedef struct mg1ps mg1ps;

mg1ps *mg1ps_alloc (double work_rate);
void mg1ps_free(mg1ps * queue);

void mg1ps_arrival (mg1ps *, size_t pid, double job_size);
//Returns process id and stores current remaining time in *time
size_t mg1ps_next_process (mg1ps *, double *time);
//Updates queue by reaching next deadline and returns corresponding pid
size_t mg1ps_reach_next_process (mg1ps *queue);

//Let the queue run for a given amount of time
//WARNING: make sure not to overload the next deadline
void mg1ps_remove_time (mg1ps *, double);
#endif
