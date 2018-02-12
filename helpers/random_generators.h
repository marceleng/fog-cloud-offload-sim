#ifndef _RANDOM_GENERATORS_H_
#define _RANDOM_GENERATORS_H_

#include <limits.h>

#define URAND_MAX UINT_MAX

double exponential_generator (double lambda);
unsigned int urandom ();

//TODO: find a hack for finishing the macro with ';'
#define exponential_generator_func(name, lambda) double name (){\
        return exponential_generator(lambda);\
}


#define constant_func(name,x) double name () {\
        return x;\
}



#endif
