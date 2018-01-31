#include "mginf.h"
#include "mg1ps.h"
#include "helpers/rbtree.h"

struct mginf {
        double work_rate;
        rbtree *processes;
        double (*distribution)(void);
};

mginf * mginf_alloc (double work_rate, double (*distribution)(void))
{
        mginf *ret = (mginf *) malloc(sizeof(mginf));
        ret->work_rate = work_rate;
        ret->processes = NULL;
        ret->distribution = distribution;
        return ret;
}

void mginf_free (mginf *queue)
{
        rbtree_free(queue->processes);
        free(queue);
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

void mginf_arrival (mginf * queue, void * job)
{
        queue->processes = rbtree_insert(queue->processes, (void *) job, queue->distribution());
}

double mginf_next_process (mginf *queue, void **job)
{
        double ret = -1;
        void *output = rbtree_head(queue->processes, &ret);
        if (output) {
                ret = ret / (queue->work_rate);
                if (job) {
                        *job = output;
                }
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
