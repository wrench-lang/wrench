#ifndef WRENCH_VECTOR
#define WRENCH_VECTOR 1


// Based on https://github.com/rxi/vec


#include <stdint.h>
#include "memory.h"


#define vec_type(type) \
    struct { type *data; uint32_t length; uint32_t capacity; }

#define vec_init(vec) \
    ((vec)->data = NULL, (vec)->length = 0, (vec)->capacity = 0)

#define vec_deinit(vec) \
    wrench_free((vec)->data)

#define vec_unpack(vec) \
    (char **)&(vec)->data, &(vec)->length, &(vec)->capacity, sizeof(*(vec)->data)

#define vec_push(vec, val) \
    (vec_assure_size(vec_unpack(vec)) == 0 ?   \
     ((vec)->data[(vec)->length++] = (val)), 0 : \
    -1)

#define vec_push_array(vec, arr, count)                      \
    do {                                                        \
        (vec)->length += (uint32_t) count;                                 \
        if (0 != vec_assure_size(vec_unpack(vec))) break; \
        uint32_t n__ = (count);                                 \
        for (uint32_t i__ = n__; i__ > 0; i__--) {                       \
            (vec)->data[(vec)->length - i__] = (arr)[n__ - i__];      \
        }                                                       \
    } while (0)                                                 \

#define vec_pop(vec) \
    ((vec)->data[0 < (vec)->length ? --(vec)->length : 0])

#define vec_last(vec) \
    ((vec)->data[(vec)->length - 1])

#define vec_reset(vec) \
    ((vec)->length = 0)

#define vec_foreach(vec, var, iter)                                     \
    if  ((vec)->length > 0 )                                            \
    for ( (iter) = 0;                                                   \
        (iter) < (vec)->length && (((var) = (vec)->data[(iter)]), 1);   \
        ++(iter)                                                        \
    )


int vec_assure_size(char **data, uint32_t *length, uint32_t *capacity, size_t elem_size);


#endif
