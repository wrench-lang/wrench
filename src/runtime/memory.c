#include <mem_pool.h>
#include <memory.h>
#include "include/wrench.h"


static VariableMemPool *COMMON_POOL;


void memory_init(void)
{
    pool_variable_init(&COMMON_POOL, 1000, 5);
}

void memory_deinit(void)
{
    pool_variable_destroy(COMMON_POOL);
}

void *wrench_allocate(size_t size)
{
    void *ptr;
#ifndef USE_MALLOC
    MemPoolError err;

    if (MEM_POOL_ERR_OK != (err = pool_variable_alloc(COMMON_POOL, size, &ptr))) {
        wrench_fatalf("Unable to allocate memory. Code: %d", err);
    }
#else
    ptr = malloc(size);
    memset(ptr, 0, size);
#endif

    return ptr;
}

void wrench_deallocate(void *ptr)
{
    if (NULL == ptr) return;

#ifndef USE_MALLOC
    MemPoolError err;

    if (MEM_POOL_ERR_OK != (err = pool_variable_free(COMMON_POOL, ptr))) {
        wrench_fatalf("Unable to free memory. Code: %d", err);
    }
#else
    free(ptr);
#endif
}
