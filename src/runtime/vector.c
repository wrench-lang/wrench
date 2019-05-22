#include <string.h>
#include "./include/vector.h"

#include <stdio.h>

int wrench_vec_assure_size(char **data, uint32_t *length, uint32_t *capacity, size_t elem_size)
{
    if (*length + 1 <= *capacity) {
        return 0;
    }

    uint32_t new_capacity = 0 == *capacity ? 10 : *capacity << 1;

    void *new;
    new = wrench_allocate(new_capacity * elem_size);

    if (*data) {
        memmove(new, *data, *capacity * elem_size);
        wrench_deallocate(*data);
    }

    *data = new;
    *capacity = new_capacity;

    return 0;
}

