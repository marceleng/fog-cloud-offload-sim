# ifndef _MGINF_H_
#define _MGINF_H_

typedef struct mginf mginf;

mginf *mginf_alloc (double work_rate);
void mginf_free(mginf * queue);

void mginf_arrival (mginf *, void * job, double job_size);
//Returns process id and stores current remaining time in *time
double mginf_next_process (mginf *, void **job);
//Updates queue by reaching next deadline and returns corresponding pid
void * mginf_reach_next_process (mginf *queue);

//Let the queue run for a given amount of time
//WARNING: make sure not to overload the next deadline
void mginf_remove_time (mginf *, double);
#endif
