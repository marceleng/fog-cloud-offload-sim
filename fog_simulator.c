#include <stdio.h>
#include <string.h>

#include "queueing/zipfgen.h"
#include "queueing/mg1ps.h"
#include "queueing/queue.h"
#include "queueing/mginf.h"

#include "caches/lru.h"

#define printr(...) fprintf(stderr, __VA_ARGS__)

struct request {
        size_t id;
        size_t content;
        double arrival;
        double exit;
};

int main (int argc, char *argv[])
{
        if(argc!=2) {
                printr("Usage: \n");
                return -1;
        }

        double lambda = strtod(argv[1], NULL);
        size_t number_of_arrivals = strtoul(argv[2], NULL, 10);

        size_t catalogue_size = 10000000;
        double alpha = 1.01;

        double x_comp = 1e7;
        double s_raw = 1e6;
        double s_proc = 1e4;
        double delta_app = 100 * 1e-3;

        double s_cachef_B = 1e9;
        double c_compf = 3. * 1e9;
        double c_acc = (10. / 8) * 1e9;
        double tau_acc = 4. * 1e-3;

        double c_compc = 2e9;
        double tau_db = 1e-3;
        double c_core = (1./8) * (1e9);
        double tau_core = 40. * 1e-3;

        double s_cachef = s_cachef_B / s_proc;


        zipfgen * arrival_gen = zipfgen_alloc(alpha, catalogue_size, lambda);
        queue_t * fog_proc = queue_from_mg1ps(mg1ps_alloc(c_compf));
        queue_t * core_d = queue_from_mg1ps(mg1ps_alloc(c_core));
        queue_t * cloud_proc = queue_from_mginf(mginf_alloc(c_compc));
        queue_t * acc_up = queue_from_mg1ps(mg1ps_alloc(c_acc));
        queue_t * acc_d = queue_from_mg1ps(mg1ps_alloc(c_acc));

        lru_filter * lb_filter  = lru_alloc(2*s_cachef, catalogue_size);
        lru_filter * fog_cache = lru_alloc(s_cachef, catalogue_size);
        lru_filter * cloud_cache = lru_alloc(s_cachef, catalogue_size);

        //TODO: use lrus to do filtering
        
        int number_of_queues = 5;
        queue_t * queues[number_of_queues];
        queues[0] = fog_proc;
        queues[1] = core_d;
        queues[2] = cloud_proc;
        queues[3] = acc_up;
        queues[4] = acc_d;


        double current_time = 0;
        struct request *requests = (struct request *) malloc(sizeof(struct request) * number_of_arrivals);
        memset(requests, 0, sizeof(struct request) * number_of_arrivals);

        for (size_t i = 0; i<number_of_arrivals; i++) {
                struct request *request = &requests[i];
                request->id = i;
                request->arrival = zipfgen_pop_next_arrival(arrival_gen, &(request->content));
                double queue_next_arrival = 0;
                queue_t  * next_queue = NULL;

                // Process all queueing events until next arrival
                while((next_queue = queue_net_find_next_exit(number_of_queues, &queues[0], &queue_next_arrival)) &&
                                (queue_next_arrival < (request->arrival-current_time))) {
                        queue_next_arrival = queue_net_make_next_update(number_of_queues, queues);
                        current_time += queue_next_arrival;
                }
                
                //Add new arrival
                if (lru_update(lb_filter, request->content)) {
                        //TODO: x_comp
                        queue_arrival(fog_proc, (void *) request, x_comp);
                }
                else {
                        //TODO: x_comp
                        queue_arrival(cloud_proc, (void *) request, x_comp);
                }
        }

        return 0;
}