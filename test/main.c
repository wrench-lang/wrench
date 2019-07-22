#define CTEST_MAIN

#include <locale.h>
#include <ctest.h>
#include "../src/runtime/include/wrench.h"


int main(int argc, const char **argv)
{
    wrench_init();

    int code = ctest_main(argc, argv);

    wrench_deinit();

    return code;
}
