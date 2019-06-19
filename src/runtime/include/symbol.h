#ifndef WRENCH_SYMBOL
#define WRENCH_SYMBOL 1


#include <wchar.h>
#include <stdint.h>


typedef uint32_t WrenchType;

typedef struct {
    WrenchType type;
} WrenchValue;

typedef WrenchValue (*WrenchFunctionPtr)(const WrenchValue *parameters);


void wrench_sym_add_function(
    const char *name,
    uint8_t param_num,
    const WrenchValue *param_types,
    WrenchType return_type,
    WrenchFunctionPtr ptr
);

WrenchValue wrench_sym_call_function(const char *name, const WrenchValue *params);


#endif
