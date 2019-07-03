#ifndef WRENCH_INTERFACE_VALUE
#define WRENCH_INTERFACE_VALUE 1


#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>


typedef intmax_t WrenchRawInt;


typedef enum {
    WRENCH_TYPE_INT,
    WRENCH_TYPE_STRING,
    WRENCH_TYPE_VOID
} WrenchType;

typedef struct WrenchValue WrenchValue;

const WrenchValue *wrench_value_int(WrenchRawInt value);

WrenchRawInt wrench_value_raw_int(const WrenchValue *val);

const WrenchValue *wrench_value_void(void);

WrenchType wrench_valu_typeof(const WrenchValue *value);


#endif
