#include <stdio.h>
#include <string.h>

#include "queueing/zipfgen.h"
#include "queueing/queue.h"

#include "caches/lru.h"

#include "helpers/log.h"
#include "helpers/random_generators.h"

struct request {
        size_t id;
        size_t content;
        double arrival;
        double exit;
};

#define catalogue_size 10000000
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

#define to_queue_func(queue) queue_t *queue ## _func (__attribute__((unused)) void *request) { return queue; }

#define create_queue(queue) queue_t *queue = NULL; to_queue_func(queue)

#define queue_link(q1,q2) queue_set_output_selection(q1, q2 ## _func)

exponential_generator_func(comp_exp_generator, 1./x_comp)
exponential_generator_func(raw_exp_generator, 1./ s_raw)
exponential_generator_func(proc_exp_generator, 1./ s_proc)

constant_func(nowork_func,0)

create_queue(fog_proc)
create_queue(core_d)
create_queue(cloud_proc)
create_queue(acc_up)
create_queue(acc_d)
create_queue(sink_queue)
create_queue(source_queue)

create_queue(fog_cache_queue)
create_queue(cloud_cache_queue)

lru_filter * lb_filter  = NULL;
lru_filter * fog_cache = NULL;
lru_filter * cloud_cache = NULL;

queue_t *filter_function (void *request)
{
        queue_t * ret = NULL;
        if(lru_update(lb_filter, ((struct request *) request)->content)) {
                ret = fog_cache_queue;
        }
        else {
                ret = cloud_cache_queue;
        }
        return ret;
}

queue_t *fog_cache_function (void *request)
{
        queue_t * ret = NULL;
        if(lru_update(fog_cache, ((struct request *) request)->content)) {
                ret = acc_d;
        }
        else {
                ret = acc_up;
        }
        return ret;
}

queue_t *cloud_cache_function (void *request)
{
        queue_t * ret = NULL;
        if(lru_update(cloud_cache, ((struct request *) request)->content)) {
                ret = core_d;
        }
        else {
                ret = cloud_proc;
        }
        return ret;
}

log_sink *initialize ()
{
        fog_proc = queue_from_mg1ps(mg1ps_alloc(c_compf, comp_exp_generator), "fog_proc");
        core_d = queue_from_mg1ps(mg1ps_alloc(c_core, proc_exp_generator), "core_d");
        cloud_proc = queue_from_mginf(mginf_alloc(c_compc, comp_exp_generator), "cloud_proc");
        acc_up = queue_from_mg1ps(mg1ps_alloc(c_acc, raw_exp_generator), "acc_up");
        acc_d = queue_from_mg1ps(mg1ps_alloc(c_acc, proc_exp_generator), "acc_d");

        source_queue = queue_from_mginf(mginf_alloc(1,nowork_func), "source");
        fog_cache_queue = queue_from_mginf(mginf_alloc(1,nowork_func), "fog_cache");
        cloud_cache_queue = queue_from_mginf(mginf_alloc(1, nowork_func), "cloud_cache");
        log_sink *sink  = log_sink_alloc();
        sink_queue = queue_from_log_sink(sink, "sink");

        //Caches + Filter
        lb_filter  = lru_alloc(2*s_cachef, catalogue_size);
        fog_cache = lru_alloc(s_cachef, catalogue_size);
        cloud_cache = lru_alloc(s_cachef, catalogue_size);

        //Transitions
        queue_set_output_selection(source_queue, filter_function);
        queue_set_output_selection(fog_cache_queue, fog_cache_function);
        queue_set_output_selection(cloud_cache_queue, cloud_cache_function);

        queue_link(acc_up, fog_proc);
        queue_link(fog_proc, acc_d);
        queue_link(acc_d, sink_queue);

        queue_link(cloud_proc, core_d);
        queue_link(core_d, acc_d);

        return sink;
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
        queue_free(sink_queue, 0);

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

        LOG(LOG_INFO, "Initializing queueing network... \n");

        log_sink *sink = initialize();
        LOG(LOG_INFO, "Precomputing popularities...\n");
        zipfgen * arrival_gen = zipfgen_alloc(alpha, catalogue_size, lambda);
       

        LOG(LOG_INFO, "Done, starting simulation\n");

        int number_of_queues = 9;
        queue_t * queues[number_of_queues];
        queues[0] = source_queue;
        queues[1] = fog_cache_queue;
        queues[2] = fog_proc;
        queues[3] = acc_up;
        queues[4] = cloud_cache_queue;
        queues[5] = cloud_proc;
        queues[6] = core_d;
        queues[7] = acc_d;
        queues[8] = sink_queue;


        double current_time = 0;
        struct request requests[number_of_arrivals];
        memset(requests, 0, sizeof(struct request) * number_of_arrivals);

        double queue_next_arrival = 0;
        queue_t  * next_queue = NULL;

        for (size_t i = 0; i<number_of_arrivals; i++) {
                struct request *request = &requests[i];
                request->id = i;
                request->arrival = zipfgen_pop_next_arrival(arrival_gen, &(request->content));

                // Process all queueing events until next arrival
                while((next_queue = queue_net_find_next_exit(number_of_queues, &queues[0], &queue_next_arrival)) &&
                                (queue_next_arrival < (request->arrival-current_time))) {
                        queue_next_arrival = queue_net_make_next_update(number_of_queues, queues);
                        current_time += queue_next_arrival;
                        LOG(LOG_DEBUG, "Updated current time to %f\n", current_time);
                }
                
                queue_net_update_time(number_of_queues, queues, request->arrival - current_time);
                //Add new arrival
                queue_arrival(source_queue, (void *) request);
                current_time = request->arrival;

                queue_next_arrival = 0;
                next_queue = NULL;
                LOG(LOG_INFO, "\rArrival %zu/%zu", i, number_of_arrivals);
        }
        LOG(LOG_INFO, "\n");
        
        //Finish in-flight requests
        while((next_queue = queue_net_find_next_exit(number_of_queues, &queues[0], &queue_next_arrival))) {
                queue_next_arrival = queue_net_make_next_update(number_of_queues, queues);
                current_time += queue_next_arrival;
        }

        size_t log_size = 0;

        struct request *res[number_of_arrivals];
        double arrivals[number_of_arrivals];
        log_size = log_sink_get_log(sink, &arrivals[0],(void **) &res[0]);
        
        for (size_t i = 0; i<log_size; i++) {
                printf("%zu,%zu,%f,%f\n",res[i]->id,res[i]->content,res[i]->arrival,arrivals[i]);
        }

        free_queues_and_caches();
        zipfgen_free(arrival_gen);
        log_sink_free(sink);


        return 0;
}
