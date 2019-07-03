#define CTEST_MAIN

#include <locale.h>
#include <ctest.h>
#include "../src/runtime/memory.h"
#include "../src/runtime/function.h"


int main(int argc, const char **argv)
{
    memory_init();
    wrench_funtion_init();

    int code = ctest_main(argc, argv);

    wrench_funtion_deinit();
    memory_deinit();

    return code;
}
