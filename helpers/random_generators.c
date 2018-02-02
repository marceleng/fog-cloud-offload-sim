#include <stdlib.h> //arc4random
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include "random_generators.h"


unsigned int urandom ()
{
        unsigned int randuint;
        FILE *f;

        f = fopen("/dev/random", "r");
        fread(&randuint, sizeof(randuint), 1, f);
        fclose(f);

        return randuint;
}

__attribute__((unused)) static int seeded = 0;

double exponential_generator (double lambda)
{
        double r;
        do {
#ifdef __APPLE__
                r = arc4random() / ((double) UINT32_MAX);
#else
                if (!seeded) {
                        srand(urandom());
                        seeded = 1;
                }
                r = rand () / ((double) RAND_MAX);
#endif
        } while (r==1 || r==0);
        return -log(r) / lambda;
}
