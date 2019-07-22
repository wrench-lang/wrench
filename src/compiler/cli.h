#ifndef WRENCH_CLI
#define WRENCH_CLI 1


#include <stdbool.h>


void cli_load(int argc, const char **argv);

const char *cli_get_cc(void);

const char *cli_get_build_dir(void);

const char *cli_get_file(void);

bool cli_is_debug(void);


#endif
