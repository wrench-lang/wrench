#include <ctest.h>
#include "../src/runtime/include/function.h"

#include "../src/runtime/value.h"


static const WrenchValue *hello(const WrenchValue **params)
{
    (void) params;

    return wrench_value_void();
}

CTEST(function, call)
{
    ASSERT_FALSE(wrench_function_exists("hello"));

    WrenchType param_types[1] = {
        WRENCH_TYPE_INT
    };

    wrench_function_add("hello", 1, param_types, WRENCH_TYPE_VOID, hello);

    ASSERT_TRUE(wrench_function_exists("hello"));
    ASSERT_FALSE(wrench_function_exists("unknown"));

    const WrenchValue *params[1];
    params[0] = wrench_value_int(666);

    const WrenchValue *ret = wrench_function_call("hello", params);

    wrenc_value_free(ret);
    wrenc_value_free(params[0]);
}
