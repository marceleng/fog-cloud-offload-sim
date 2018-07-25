#include <stdlib.h> //arc4random
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include "random_generators.h"


static int seeded = 0;

static unsigned int _urandom ()
{
        unsigned int randuint;
        FILE *f;

        f = fopen("/dev/random", "r");
        fread(&randuint, sizeof(randuint), 1, f);
        fclose(f);

        return randuint;
}

int safe_rand ()
{
        if (!seeded) {
                srand(_urandom());
                seeded = 1;
        }
        return rand();
}

int irandom(int min, int max)
{
        size_t res = (size_t) safe_rand();
        res *= (max - min);
        res /= RAND_MAX;
        return (int) res;
}

double drandom ()
{
        return safe_rand() / (double) RAND_MAX;
}

double exponential_generator (double lambda)
{
        double r;
        do {
                r = drandom();
        } while (r==1 || r==0);
        return -log(r) / lambda;
}
