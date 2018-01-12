#include "zipf.h"
#include <math.h>
#include <stdint.h>

#define STEP 500.

static size_t number_of_classes=0;
static double *popularity=NULL;
static double alpha=1.;

static void initialize_zipf ()
{
        double sum, pop;
        if(!number_of_classes) { return; }
        if (popularity) { free(popularity); }
        popularity = (double *) malloc(sizeof(double) * number_of_classes);

        for (int i=0; i<number_of_classes; i++) {
                pop = 1. / pow(i+1, alpha);
                sum += pop;
                popularity[i] = pop;
        }

        for (int i=0; i<number_of_classes; i++) {
                popularity[i] /= sum;
        }

        return;
}

size_t get_number_of_classes (void)
{
        return number_of_classes;
}

void set_number_of_classes(size_t nb_of_classes)
{
        if (number_of_classes != nb_of_classes) {
                number_of_classes = nb_of_classes;
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
        if (k<number_of_classes) {
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
