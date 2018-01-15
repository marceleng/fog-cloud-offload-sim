#include "zipf.h"
#include <math.h>
#include <stdint.h>

#define STEP 500.

static size_t catalogue_size=0;
static double *popularity=NULL;
static double alpha=1.;

typedef struct zipfgen {
        double alpha;
        size_t catalogue_size;
        double *popularity_dist;
} zipf_generator;

static void initialize_zipf ()
{
        double sum=0, pop;
        if(!catalogue_size) { return; }
        if (popularity) { free(popularity); }
        popularity = (double *) malloc(sizeof(double) * catalogue_size);

        for (size_t i=0; i<catalogue_size; i++) {
                pop = 1. / pow(i+1, alpha);
                sum += pop;
                popularity[i] = pop;
        }

        for (size_t i=0; i<catalogue_size; i++) {
                popularity[i] /= sum;
        }

        return;
}

size_t get_catalogue_size (void)
{
        return catalogue_size;
}

void set_catalogue_size(size_t cat_size)
{
        if (catalogue_size != cat_size) {
                catalogue_size = cat_size;
                initialize_zipf();
        }
}

double get_alpha (void)
{
        return alpha;
}

void set_alpha (double a)
{
        if(alpha != a) {
                alpha = a;
                initialize_zipf();
        }
}

double get_popularity (size_t k)
{
        if (k<catalogue_size) {
                return popularity[k];
        }
        return 0;
}

double poisson (double lambda)
{
        double r;
        do {
                r = arc4random() / ((double) UINT32_MAX);
        } while (r==1 || r==0);
        return -log(r) / lambda;
}


