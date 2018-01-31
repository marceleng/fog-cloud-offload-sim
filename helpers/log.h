#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define LOG_DEBUG 0
#define LOG_INFO  1
#define LOG_WARN  2
#define LOG_ERROR 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO
#endif

#define LOG(log_level,str,...) do {\
        if (log_level >= LOG_LEVEL) { \
                fprintf(stderr, "[%s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__); \
        } } while (0)


#endif
