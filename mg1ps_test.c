#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mg1ps.h"
#include "zipf.h"

int main (int argc, char *argv[])
{
        assert(argc == 5); //lambda, mu, job_size (must be <mu), number of arrivals
        (void) argc;

        double lambda = strtod(argv[1], NULL);
        double mu = strtod(argv[2], NULL);
        double job_size = strtod(argv[3], NULL);
        size_t nb_arrivals = strtoul(argv[4], NULL, 10);

        mg1ps * queue = mg1ps_alloc(mu);

        double res[nb_arrivals];
        memset(&res[0], '\0', sizeof(res));

        double next_arrival = poisson(lambda);
        double current_time = 0;
        size_t next_exit = 0;
        double next_process_end = 0;

        for (size_t i=1; i<nb_arrivals+1; i++) {
                while ((next_exit = mg1ps_next_process(queue, &next_process_end))) {
                        if (next_process_end > (next_arrival-current_time)) {
                                break;
                        }
                        //TODO: remove process and update stats
                        mg1ps_reach_next_process(queue);
                        current_time += next_process_end;
                        res[next_exit-1] = current_time - res[next_exit-1];
                        
                }
                //TODO add new process with pid i
                mg1ps_remove_time(queue, lambda-current_time);
                mg1ps_arrival(queue, i, job_size);
                res[i-1] = current_time;
                next_arrival = poisson(lambda) + current_time;
        }

        while ((next_exit = mg1ps_next_process(queue, &next_process_end))) {
                mg1ps_reach_next_process(queue);
                current_time += next_process_end;
                res[next_exit-1] = current_time - res[next_exit-1];
        }

        double sum = 0;
        for (size_t i=0; i<nb_arrivals; i++) {
                sum += res[i];
        }
        sum /= nb_arrivals;
        printf("Average queueing time: %f\n", sum);

        return 0;
}
