#include <locale.h>
#include "memory.h"
#include "function.h"
#include "value.h"


void wrench_init(void)
{
    setlocale(LC_ALL, "");

    memory_init();
    wrench_value_init();
    wrench_funtion_init();
}

void wrench_deinit(void)
{
    wrench_funtion_deinit();
    wrench_value_deinit();
    memory_deinit();
}

