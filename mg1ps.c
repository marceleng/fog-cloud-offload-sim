#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "rbtree.h"

typedef struct process {
        size_t id;
        double total_work; // Total time for executing the process
} process_t;

typedef struct mg1ps {
        double work_rate;
        size_t nb_processes;
        rbtree *processes; // Processes sorted by remaining amount of WORK
} mg1ps;

mg1ps * mg1ps_alloc (double work_rate)
{
        mg1ps *ret = (mg1ps *) malloc(sizeof(mg1ps));
        ret->work_rate = work_rate;
        ret->nb_processes = 0;
        ret->processes = rbtree_alloc();
        return ret;
}

void mg1ps_free (mg1ps *queue)
{
        rbtree_free(queue->processes);
        free(queue);
}

double multiply_key (double current_time, __attribute__((unused)) void *item, void *coef) {
        return current_time * *(double *) coef;
}

double substract_key (double current_time, __attribute__((unused)) void *item, void *coef) {
        return current_time - *(double *) coef;
}

void _mg1ps_add_process (mg1ps * queue, process_t *process)
{
        size_t new_nb_processes = queue->nb_processes+1;

        //Insert new process
        queue->processes = rbtree_insert(queue->processes, (void *) process, process->total_work);

        //Update process counter
        queue->nb_processes = new_nb_processes;
}

void _mg1ps_remove_work (mg1ps * queue, double work)
{ 
        rbtree_apply_func_key(queue->processes, &substract_key, &work);
}

void mg1ps_remove_time (mg1ps * queue, double time)
{
        double removed_work = time * queue->work_rate / queue->nb_processes;
        _mg1ps_remove_work(queue, removed_work);
}

void mg1ps_arrival (mg1ps * queue, size_t pid, double job_size)
{
        process_t *process = (process_t *) malloc(sizeof(process_t));
        process->id = pid;
        process->total_work = job_size;

        _mg1ps_add_process (queue, process);
}

process_t * _mg1ps_next_process(mg1ps *queue, double *work)
{
        return (process_t *) rbtree_head(queue->processes, work);
}

size_t mg1ps_next_process (mg1ps *queue, double *time)
{
        process_t * process = _mg1ps_next_process(queue, time);
        size_t ret = 0;
        if (process) {
                *time = *time / (queue->work_rate / queue->nb_processes);
                ret = process->id;
        }
        return ret;
}

size_t mg1ps_reach_next_process(mg1ps *queue)
{
        size_t ret = 0;
        double work=0;
        process_t * process = NULL;
        
        queue->processes = rbtree_pop(queue->processes, &work, (void **) &process);

        _mg1ps_remove_work(queue, work);

        ret = process->id;

        free(process);

        return ret;
}
