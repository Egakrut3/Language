#ifndef NAME_TABLE_H
#define NAME_TABLE_H

#include "Common.h"

struct Name_table {
    char const **keys;
    size_t     *vals;

    size_t     capacity;

    bool       is_valid;
};

errno_t Name_table_Ctor(Name_table *dest);

errno_t Name_table_Dtor(Name_table *dest);

#define KEY_ALREADY_EXISTS 2'000
errno_t Name_table_set(Name_table *dest, char const *str, size_t val);
#define KEY_NOT_FOUND      2'001
errno_t Name_table_get(size_t *dest, Name_table const *src, char const *str);

#endif /* NAME_TABLE_H */
