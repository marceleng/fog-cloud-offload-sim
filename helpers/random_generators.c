#include "random_generators.h"

double exponential_generator (double lambda)
{
        double r;
        do {
                r = arc4random() / ((double) UINT32_MAX);
        } while (r==1 || r==0);
        return -log(r) / lambda;
}
