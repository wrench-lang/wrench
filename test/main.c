#define CTEST_MAIN

#include <locale.h>
#include <ctest.h>
#include "../src/runtime/memory.h"


int main(int argc, const char **argv)
{
    memory_init();

    setlocale(LC_ALL, "");

    int code = ctest_main(argc, argv);

    memory_deinit();

    return code;
}
