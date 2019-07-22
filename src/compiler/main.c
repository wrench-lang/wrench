#include <wrench/wrench.h>
#include "cli.h"
#include "transpiler.h"
#include "compiler.h"

#include <ftw.h>


static void create_build_dir(const char *dir)
{
    struct stat st;

    if (-1 == stat(dir, &st)) {
        mkdir(dir,  0777);
        chmod(dir, 0777);
    }
}

int main(int argc, const char **argv)
{
    wrench_init();

    cli_load(argc, argv);

    create_build_dir("./wrench-build");

    transpile(cli_get_file());
    compile();

    wrench_deinit();
}
