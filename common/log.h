#include <stdio.h>
#include <stdlib.h>

#ifndef GB_EMU_LOG_H
#define GB_EMU_LOG_H

//TODO: figure out whether ##__VA_ARGS__ is stdc
//just so logging is a bit more easy and configurable
#define log_error(pattern, ...) fprintf(stderr, "[error %s:%d] " pattern "\n",__FILE__,__LINE__, ##__VA_ARGS__)
#define log_fatal(pattern, ...) fprintf(stderr, "[fatal %s:%d] " pattern "\n",__FILE__,__LINE__, ##__VA_ARGS__);exit(1)
#define log_debug(pattern, ...)  fprintf(stdout, "[debug] " pattern "\n", ##__VA_ARGS__)
#define log_info(pattern, ...)  fprintf(stdout, "[info] " pattern "\n", ##__VA_ARGS__)

#endif //GB_EMU_LOG_H
