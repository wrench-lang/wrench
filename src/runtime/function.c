#include <wchar.h>
#include <map.h>
#include "include/wrench.h"


typedef struct {
    uint8_t param_num;
    const WrenchValue *PARAM_TYPE_s;
    WrenchType return_type;
    WrenchFunctionPtr ptr;
} Function;

typedef map_t(const Function *) FunctionMap;

static FunctionMap *FUNCTION_MAP;


void wrench_sym_add_function
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
    fn->PARAM_TYPE_s = wrench_allocate(param_num * sizeof(WrenchValue));
    memmove((void *) fn->PARAM_TYPE_s, PARAM_TYPE_s, param_num * sizeof(WrenchValue));

    map_set(FUNCTION_MAP, name, fn);
}

WrenchValue wrench_sym_call_function(const char *name, const WrenchValue *params)
{
    const Function **fn = map_get(FUNCTION_MAP, name);

    if (NULL == fn) {
        wrench_fatalf("Undefined function '%s'", name);
    }

    return (*fn)->ptr(params);
}

