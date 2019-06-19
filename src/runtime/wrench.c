#include <locale.h>
#include "memory.h"


void wrench_init(void)
{
    setlocale(LC_ALL, "");

    memory_init();
}

void wrench_deinit(void)
{
    memory_deinit();
}

