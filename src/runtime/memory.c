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
#ifdef USE_MALLOC
    ptr = malloc(size);
#else
    MemPoolError err;

    if (MEM_POOL_ERR_OK != (err = pool_variable_alloc(COMMON_POOL, size, &ptr))) {
        wrench_fatalf("Unable to allocate memory. Code: %d", err);
    }
#endif
    memset(ptr, 0, size);

    return ptr;
}

void *wrench_reallocate(void *old, size_t size)
{
#ifdef USE_MALLOC
    return realloc(old, size);
#else
    void *new = wrench_allocate(size);

    if (NULL != old) {
        memmove(new, old, size);
        wrench_free(old);
    }

    return new;
#endif
}

void wrench_free(void *ptr)
{
    if (NULL == ptr) return;

#ifdef USE_MALLOC
    free(ptr);
#else
    MemPoolError err;

    if (MEM_POOL_ERR_OK != (err = pool_variable_free(COMMON_POOL, ptr))) {
        wrench_fatalf("Unable to free memory. Code: %d", err);
    }
#endif
}
