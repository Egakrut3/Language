#include "Name_table.h"
#include <string.h>

#define FINAL_CODE

size_t const MIN_CAPACITY = 5,
             EXPANSION    = 2;

errno_t Name_table_Ctor(Name_table *const dest) {
    assert(dest); assert(!dest->is_valid);

    CHECK_FUNC(My_calloc, (void **)&dest->keys, MIN_CAPACITY, sizeof(*dest->keys));
    CHECK_FUNC(My_calloc, (void **)&dest->vals, MIN_CAPACITY, sizeof(*dest->vals));

    dest->capacity = MIN_CAPACITY;

    dest->is_valid = true;

    return 0;
}

errno_t Name_table_Dtor(Name_table *const dest) {
    assert(dest); assert(dest->is_valid);

    free(dest->keys);
    free(dest->vals);

    dest->is_valid = false;

    return 0;
}

errno_t Name_table_set(Name_table *const dest, char const *const str, size_t const val) {
    assert(dest);

    size_t any_empty = -1;
    for (size_t i = 0; i < dest->capacity; ++i) {
        if (!dest->keys[i]) {
            any_empty = i;
        }
        else if (!strcmp(dest->keys[i], str)) { return KEY_ALREADY_EXISTS; }
    }

    if (any_empty != (size_t)-1) { dest->keys[any_empty] = str; dest->vals[any_empty] = val; return 0; }

    CHECK_FUNC(My_realloc, (void **)&dest->keys, dest->keys, dest->capacity * EXPANSION * sizeof(*dest->keys));
    CHECK_FUNC(My_realloc, (void **)&dest->vals, dest->vals, dest->capacity * EXPANSION * sizeof(*dest->vals));
    for (size_t i = dest->capacity; i < dest->capacity * EXPANSION; ++i) {
        dest->keys = nullptr;
    }
    dest->keys[dest->capacity] = str;
    dest->vals[dest->capacity] = val;
    dest->capacity *= EXPANSION;

    return 0;
}

errno_t Name_table_get(size_t *const dest, Name_table const *const src, char const *const str) {
    assert(dest); assert(src); assert(str);

    for (size_t i = 0; i < src->capacity; ++i) {
        if (src->keys[i] and !strcmp(src->keys[i], str)) {
            *dest = src->vals[i];

            return 0;
        }
    }

    return KEY_NOT_FOUND;
}

#undef FINAL_CODE
