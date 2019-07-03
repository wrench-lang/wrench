#ifndef WRENCH_INTERFACE_FUNCTION
#define WRENCH_INTERFACE_FUNCTION 1


#include <wchar.h>
#include <stdint.h>
#include "value.h"


typedef struct {
    uint8_t param_num;
    WrenchType *param_types;
    WrenchType return_type;
} WrenchFunctionInfo;

typedef const WrenchValue *(WrenchFunctionPtr)(const WrenchValue **parameters);


void wrench_function_add(
    const char *name,
    uint8_t param_num,
    const WrenchType *param_types,
    WrenchType return_type,
    WrenchFunctionPtr *ptr
);

WrenchFunctionInfo wrench_function_info(const char *name);

bool wrench_function_exists(const char *name);

const WrenchValue *wrench_function_call(const char *name, const WrenchValue **params);


#endif
