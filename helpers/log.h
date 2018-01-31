#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define LOG(str,...) fprintf(stderr, "[%s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__)

#endif
