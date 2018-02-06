#include <stdio.h>
#include <string.h>

#include "queueing/zipfgen.h"
#include "queueing/queue.h"
#include "queueing/queue_net.h"
#include "queueing/file_logger.h"

#include "caches/lru.h"

#include "helpers/log.h"
#include "helpers/random_generators.h"

#include "queueing/request.h"

#define catalogue_size 1000000UL
#define alpha 1.01

#define x_comp 1e7
#define s_raw 1e6
#define s_proc 1e4
#define delta_app 100 * 1e-3

#define s_cachef_B 1e9
#define c_compf 3. * 1e9
#define c_acc (10. / 8) * 1e9
#define tau_acc 4. * 1e-3

#define c_compc 2e9
#define tau_db 1e-3
#define c_core (1./8) * (1e9)
#define tau_core 40. * 1e-3

#define s_cachef (s_cachef_B/s_proc)

exponential_generator_func(comp_exp_generator, 1./x_comp)
exponential_generator_func(raw_exp_generator, 1./ s_raw)
exponential_generator_func(proc_exp_generator, 1./ s_proc)

constant_func(nowork_func,0)

queue_t *lb_queue = NULL;

queue_t *fog_proc = NULL;
queue_t *core_d = NULL;
queue_t *cloud_proc = NULL;
queue_t *acc_up = NULL;
queue_t *acc_d = NULL;
queue_t *sink_queue = NULL;
queue_t *source_queue = NULL;

queue_t *fog_cache_queue = NULL;
queue_t *cloud_cache_queue = NULL;

lru_filter * lb_filter  = NULL;
lru_filter * fog_cache = NULL;
lru_filter * cloud_cache = NULL;

queue_t *filter_function (request_t *request)
{
        queue_t * ret = NULL;
        if(lru_update(lb_filter, request->content)) {
                ret = fog_cache_queue;
        }
        else {
                ret = cloud_cache_queue;
        }
        return ret;
}

queue_t *fog_cache_function (request_t *request)
{
        queue_t * ret = NULL;
        if(lru_update(fog_cache, request->content)) {
                ret = acc_d;
        }
        else {
                ret = acc_up;
        }
        return ret;
}

queue_t *cloud_cache_function (request_t *request)
{
        queue_t * ret = NULL;
        if(lru_update(cloud_cache, request->content)) {
                ret = core_d;
        }
        else {
                ret = cloud_proc;
        }
        return ret;
}


queue_t *queue_net_transition (queue_t *queue, request_t *req)
{
#define queue_link(queue,q1,q2) if (queue==q1) return q2
#define queue_filter(queue,req,q1,filter) if (queue==q1) return filter(req)

        queue_link(queue,source_queue,lb_queue);
        queue_filter(queue, req, lb_queue, filter_function);
        queue_filter(queue, req, fog_cache_queue, fog_cache_function);
        queue_filter(queue, req, cloud_cache_queue, cloud_cache_function);
        queue_link(queue, acc_up, fog_proc);
        queue_link(queue, fog_proc, acc_d);
        queue_link(queue, acc_d, sink_queue);
        queue_link(queue, cloud_proc, core_d);
        queue_link(queue, core_d, acc_d);
        return NULL;
}


void initialize (double lambda, size_t number_of_arrivals, char *filename)
{
        lb_queue = queue_from_mginf(mginf_alloc(1,nowork_func), "source");
        fog_proc = queue_from_mg1ps(mg1ps_alloc(c_compf, comp_exp_generator), "fog_proc");
        core_d = queue_from_mg1ps(mg1ps_alloc(c_core, proc_exp_generator), "core_d");
        cloud_proc = queue_from_mginf(mginf_alloc(c_compc, comp_exp_generator), "cloud_proc");
        acc_up = queue_from_mg1ps(mg1ps_alloc(c_acc, raw_exp_generator), "acc_up");
        acc_d = queue_from_mg1ps(mg1ps_alloc(c_acc, proc_exp_generator), "acc_d");

        LOG(LOG_INFO, "Precomputing popularities...\n");
        source_queue = queue_from_zipfgen(zipfgen_alloc(alpha, catalogue_size, lambda, number_of_arrivals), "source_gen");
        fog_cache_queue = queue_from_mginf(mginf_alloc(1,nowork_func), "fog_cache");
        cloud_cache_queue = queue_from_mginf(mginf_alloc(1, nowork_func), "cloud_cache");
        sink_queue = queue_from_file_logger(file_logger_alloc(filename),"logger");

        //Caches + Filter
        lb_filter  = lru_alloc(2*s_cachef, catalogue_size);
        fog_cache = lru_alloc(s_cachef, catalogue_size);
        cloud_cache = lru_alloc(s_cachef, catalogue_size);
}

void free_queues_and_caches ()
{
        queue_free(fog_proc,1);
        queue_free(core_d,1);
        queue_free(cloud_proc,1);
        queue_free(acc_up,1);
        queue_free(acc_d,1);

        queue_free(source_queue,1);
        queue_free(fog_cache_queue,1);
        queue_free(cloud_cache_queue,1);
        queue_free(lb_queue,1);
        queue_free(sink_queue, 1);

        lru_free(lb_filter);
        lru_free(fog_cache);
        lru_free(cloud_cache);
};

int main (int argc, char *argv[])
{
        if(argc!=3) {
                LOG(LOG_ERROR, "Usage: ./fog_simulator [lambda] [number of arrivals] \n");
                return -1;
        }

        setvbuf(stdout, NULL, _IONBF, 0);

        double lambda = strtod(argv[1], NULL);
        size_t number_of_arrivals = strtoul(argv[2], NULL, 10);

        char filename[50];
        memset(filename, 0, 50);
        sprintf(filename, "fog_sim_res_%.2f_%zu.csv",lambda, number_of_arrivals);
        LOG(LOG_INFO,"Log file: %s\n", filename);
        LOG(LOG_INFO, "Initializing queueing network... \n");

        initialize(lambda, number_of_arrivals, filename);

        LOG(LOG_INFO, "Done, starting simulation\n");

        int number_of_queues = 10;
        queue_t * queues[number_of_queues];
        queues[0] = lb_queue;
        queues[1] = fog_cache_queue;
        queues[2] = fog_proc;
        queues[3] = acc_up;
        queues[4] = cloud_cache_queue;
        queues[5] = cloud_proc;
        queues[6] = core_d;
        queues[7] = acc_d;
        queues[8] = source_queue;
        queues[9] = sink_queue;

        
        queue_net *qn = queue_net_alloc(number_of_queues, queues, queue_net_transition);

        while(queue_net_make_next_update(qn) >= 0);

        free(qn);
        free_queues_and_caches();

        return 0;
}
