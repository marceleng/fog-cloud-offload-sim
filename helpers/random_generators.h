#ifndef _RANDOM_GENERATORS_H_
#define _RANDOM_GENERATORS_H_

double exponential_generator (double lambda);

//TODO: find a hack for finishing the macro with ';'
#define exponential_generator_func(name, lambda) double name (){\
        return exponential_generator(lambda);\
}


#define constant_func(name,x) double name () {\
        return x;\
}



#endif
