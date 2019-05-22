#ifndef WRENCH_INTERFACE_MEMORY
#define WRENCH_INTERFACE_MEMORY 1


#include <stddef.h>


void *wrench_allocate(size_t size);

void wrench_deallocate(void *ptr);


#endif
