#include <stdint.h>
#include <memory.h>
#include <mem_pool.h>
#include "include/value.h"
#include "include/wrench.h"


struct WrenchValue {
    WrenchType type;
    union {
        WrenchRawInt integer;
    } raw;
};


static FixedMemPool *VALUE_POOL;


void wrench_value_init(void)
{
    MemPoolError err = pool_fixed_init(&VALUE_POOL, sizeof(WrenchValue), 50);

    if (MEM_POOL_ERR_OK != err) {
        wrench_fatal("Unable to initialize FixedPool");
    }
}

void wrench_value_deinit(void)
{
    pool_fixed_destroy(VALUE_POOL);
}

static WrenchValue *allocate_value(void)
{
    void *val;
#ifdef USE_MALLOC
    val = malloc(sizeof(WrenchValue));
#else
    MemPoolError err;

    if (MEM_POOL_ERR_OK != (err = pool_fixed_alloc(VALUE_POOL, &val))) {
        wrench_fatalf("Unable to allocate WrenchValue. Code: %d", err);
    }
#endif
    memset(val, 0, sizeof(WrenchValue));

    return val;
}

void wrenc_value_free(const WrenchValue *val)
{
    if (NULL == val) return;

#ifdef USE_MALLOC
    free((void *) val);
#else
    MemPoolError err;

    if (MEM_POOL_ERR_OK != (err = pool_fixed_free(VALUE_POOL, (void *) val))) {
        wrench_fatalf("Unable to free WrenchValue. Code: %d", err);
    }
#endif
}

WrenchType wrench_valu_typeof(const WrenchValue *value)
{
    return value->type;
}

const WrenchValue *wrench_value_int(WrenchRawInt value)
{
    WrenchValue *val = allocate_value();
    val->raw.integer = value;
    val->type = WRENCH_TYPE_INT;

    return val;
}

WrenchRawInt wrench_value_raw_int(const WrenchValue *val)
{
    if (WRENCH_TYPE_INT != val->type) {
        // TODO raise TypeError
    }

    return val->raw.integer;
}

const WrenchValue *wrench_value_void(void)
{
    WrenchValue *val = allocate_value();
    val->type = WRENCH_TYPE_VOID;

    return val;
}
