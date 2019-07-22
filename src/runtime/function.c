#include <wchar.h>
#include <map.h>
#include "include/wrench.h"


typedef struct {
    WrenchFunctionInfo info;
    WrenchFunctionPtr *ptr;
} Function;

typedef map_t(const Function *) FunctionMap;


static FunctionMap FUNCTION_MAP;

void wrench_funtion_init(void)
{
    map_init(&FUNCTION_MAP);
}

void wrench_funtion_deinit(void)
{
    map_deinit(&FUNCTION_MAP);
}

void wrench_function_add
(
    const char *name,
    uint8_t param_num,
    const WrenchType *param_types,
    WrenchType return_type,
    WrenchFunctionPtr ptr
)
{
    if (map_get(&FUNCTION_MAP, name)) {
        wrench_fatalf("Function '%s' already defined", name);
    }

    Function *fn = wrench_allocate(sizeof(Function));
    fn->info.return_type = return_type;
    fn->info.param_num = param_num;
    fn->ptr = ptr;
    fn->info.param_types = wrench_allocate(param_num * sizeof(WrenchType));
    memmove((void *) fn->info.param_types, param_types, param_num * sizeof(WrenchType));

    map_set(&FUNCTION_MAP, name, fn);
}

bool wrench_function_exists(const char *name)
{
    return NULL != map_get(&FUNCTION_MAP, name);
}

static const Function *get_function(const char *name)
{
    const Function **fn = map_get(&FUNCTION_MAP, name);

    if (NULL == fn) {
        wrench_fatalf("Undefined function '%s'", name);
    }

    return *fn;
}

WrenchFunctionInfo wrench_function_info(const char *name)
{
    return get_function(name)->info;
}

const WrenchValue *wrench_function_call(const char *name, const WrenchValue **params)
{
    const Function *fn = get_function(name);
    const WrenchValue *ret = fn->ptr(params);

    /** Needed for Extension functions written in C because its impossible to tell if the return type is correct at compile time */
    if (wrench_valu_typeof(ret) != fn->info.return_type) {
        // TODO throw exception
    }

    return ret;
}

