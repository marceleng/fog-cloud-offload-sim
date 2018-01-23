#include "mginf.h"
#include "helpers/rbtree.h"

struct mginf {
        double work_rate;
        rbtree *processes;
};

mginf * mginf_alloc (double work_rate)
{
        mginf *ret = (mginf *) malloc(sizeof(mginf));
        ret->work_rate = work_rate;
        ret->processes = NULL;
        return ret;
}

void mginf_free (mginf *queue)
{
        rbtree_free(queue->processes);
        free(queue);
}

double substract_key (double current_time, __attribute__((unused)) void *item, void *coef) {
        return current_time - *(double *) coef;
}
static void _mginf_remove_work (mginf * queue, double work)
{ 
        rbtree_apply_func_key(queue->processes, &substract_key, &work);
}

void mginf_remove_time (mginf * queue, double time)
{
        double removed_work = time * queue->work_rate;
        _mginf_remove_work(queue, removed_work);
}

void mginf_arrival (mginf * queue, void * job, double job_size)
{
        queue->processes = rbtree_insert(queue->processes, (void *) job, job_size);
}

double mginf_next_process (mginf *queue, void **job)
{
        double ret = -1;
        *job = rbtree_head(queue->processes, &ret);
        if (*job) {
                ret = ret / (queue->work_rate);
        }
        return ret;
}

void * mginf_reach_next_process(mginf *queue)
{
        void * ret = NULL;
        double work=0;
        
        queue->processes = rbtree_pop(queue->processes, &work, &ret);

        _mginf_remove_work(queue, work);

        return ret;
}
