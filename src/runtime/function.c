#include <wchar.h>
#include <map.h>
#include "include/wrench.h"


typedef struct {
    uint8_t param_num;
    const WrenchValue *param_types;
    WrenchType return_type;
    WrenchFunctionPtr *ptr;
} Function;

typedef map_t(const Function *) FunctionMap;

static FunctionMap *FUNCTION_MAP;


void wrench_function_add
(
    const char *name,
    uint8_t param_num,
    const WrenchValue *PARAM_TYPE_s,
    WrenchType return_type,
    WrenchFunctionPtr ptr
)
{
    if (map_get(FUNCTION_MAP, name)) {
        wrench_fatalf("Function '%s' already defined", name);
    }

    Function *fn = wrench_allocate(sizeof(Function));
    fn->return_type = return_type;
    fn->param_num = param_num;
    fn->ptr = ptr;
    fn->param_types = wrench_allocate(param_num * sizeof(WrenchValue));
    memmove((void *) fn->param_types, PARAM_TYPE_s, param_num * sizeof(WrenchValue));

    map_set(FUNCTION_MAP, name, fn);
}

bool wrench_function_exists(const char *name)
{
    return NULL != map_get(FUNCTION_MAP, name);
}

static const Function *get_function(const char *name)
{
    const Function **fn = map_get(FUNCTION_MAP, name);

    if (NULL == fn) {
        wrench_fatalf("Undefined function '%s'", name);
    }

    return *fn;
}

WrenchFunctionInfo wrench_function_describe(const char *name)
{
    const Function *fn = get_function(name);

    return (WrenchFunctionInfo) {
        .param_num = fn->param_num,
        .param_types = fn->param_types,
        .return_type = fn->return_type
    };
}

WrenchValue wrench_function_call(const char *name, const WrenchValue *params)
{
    return get_function(name)->ptr(params);
}

