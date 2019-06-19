#include <stdio.h>
#include <string.h>
#include <wrench/wrench.h>
#include "generator.h"


static uint32_t ENTITY_ID = 0;

static const char *TEMPLATE_ENTRY_FN_CALL = "wrench_sym_call_function(\"%ls\", NULL);";


static char *call_to_string(const FunctionCall *call)
{
    size_t size = strlen(TEMPLATE_ENTRY_FN_CALL) + (wcslen(call->name) * sizeof(wchar_t));
    char *call_str = wrench_allocate(size);

    sprintf(call_str, TEMPLATE_ENTRY_FN_CALL, call->name);

    return call_str;
}

void generate_file(const ParsedFile *parsed, const char *target)
{
    FILE *file = fopen(target, "w+");

    if (!file) wrench_fatalf("Unable to write file '%s'", target);

    FunctionCall fn_call;
    uint32_t i = 0;
    char *call_str;

    fprintf(file, "%s", "#include <wrench/wrench.h>\n");;
    fprintf(file, "void entry_%d(void) {\n", ENTITY_ID++);

    vec_foreach(parsed->fn_calls, fn_call, i) {
        call_str = call_to_string(&fn_call);
        fprintf(file, "%s", call_str);
        wrench_free(call_str);
    };

    fprintf(file, "%s", "\n}\n");

    fclose(file);
}
