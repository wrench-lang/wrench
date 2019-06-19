#include <ctest.h>
#include "../src/runtime/include/vector.h"


typedef vec_type(wchar_t) WcharVec;


CTEST(vector, push_pop_last_clear)
{
    WcharVec vec;
    vec_init(&vec);

    wchar_t *hello = L"Hello";
    vec_push_array(&vec, hello, 5);
    ASSERT_EQUAL(5, vec.length);
    ASSERT_WSTR(L"Hello", vec.data);

    ASSERT_EQUAL(L'o', vec_last(&vec));

    ASSERT_EQUAL(L'o', vec_pop(&vec));
    ASSERT_EQUAL(0, vec_push(&vec, L'o'));

    ASSERT_EQUAL(0, vec_push(&vec, L'W'));
    ASSERT_EQUAL(0, vec_push(&vec, L'o'));
    ASSERT_EQUAL(0, vec_push(&vec, L'r'));
    ASSERT_EQUAL(0, vec_push(&vec, L'l'));
    ASSERT_EQUAL(0, vec_push(&vec, L'd'));
    ASSERT_EQUAL(0, vec_push(&vec, L'!'));
    ASSERT_EQUAL(0, vec_push(&vec, L'!'));

    ASSERT_WSTR(L"HelloWorld!!", vec.data);
    ASSERT_EQUAL(12, vec.length);

    ASSERT_EQUAL(L'!', vec_pop(&vec));
    ASSERT_EQUAL(L'!', vec_pop(&vec));
    ASSERT_EQUAL(L'd', vec_pop(&vec));

    while (0 != vec.length) {
        ASSERT_NOT_EQUAL(0, vec_pop(&vec));
    }
    ASSERT_EQUAL(0, vec.length);

    vec_reset(&vec);
    ASSERT_EQUAL(0, vec.length);

    vec_deinit(&vec);
}
