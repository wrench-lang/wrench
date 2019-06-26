#ifndef WRENCH_FUNCTION
#define WRENCH_FUNCTION 1


#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>


typedef enum {
    TYPE_INT,
    TYPE_STRING
} WrenchType;

typedef struct {
    WrenchType type;
} WrenchValue;

typedef struct {
    const uint8_t param_num;
    const WrenchValue *param_types;
    const WrenchType return_type;
} WrenchFunctionInfo;

typedef WrenchValue (WrenchFunctionPtr)(const WrenchValue *parameters);


void wrench_function_add(
    const char *name,
    uint8_t param_num,
    const WrenchValue *param_types,
    WrenchType return_type,
    WrenchFunctionPtr *ptr
);

WrenchFunctionInfo wrench_function_describe(const char *name);

bool wrench_function_exists(const char *name);

WrenchValue wrench_function_call(const char *name, const WrenchValue *params);


#endif
