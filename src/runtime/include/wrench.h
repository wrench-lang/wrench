#ifndef WRENCH_INTERFACE
#define WRENCH_INTERFACE 1

#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "function.h"
#include "value.h"


#define WRENCH_NEW_LINE "\n"


#define wrench_fatalf(msg, ...)                                     \
    do {                                                            \
        fprintf(stderr, "\x1B[31m"msg"\x1B[0m"WRENCH_NEW_LINE, __VA_ARGS__); wrench_deinit(); exit(0);\
    } while (0)


#define wrench_fatal(msg)                                           \
    do {                                                            \
        fprintf(stderr, "\x1B[31m"msg"\x1B[0m"WRENCH_NEW_LINE); wrench_deinit(); exit(0);             \
    } while (0)


void wrench_init(void);

void wrench_deinit(void);


#endif
