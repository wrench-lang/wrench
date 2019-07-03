#include <ctest.h>

#include "../src/runtime/value.h"

CTEST(value, integer)
{
    const WrenchValue *val = wrench_value_int(999);

    ASSERT_EQUAL(999, wrench_value_raw_int(val));

    wrenc_value_free(val);
}
