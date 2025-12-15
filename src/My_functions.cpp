#include "My_functions.h"
#include <stdarg.h>
#include <string.h>

#define FINAL_CODE

errno_t My_calloc(void **const dest, size_t const num, size_t const size) {
    assert(dest);

    if (num == 0) {
        *dest = nullptr;
        return 0;
    }

    void *const ptr = calloc(num, size);
    if (!ptr) {
        return errno;
    }

    *dest = ptr;
    return 0;
}

errno_t My_realloc(void **const dest, void *const prev_ptr, size_t const size) {
    assert(dest); assert(prev_ptr);

    void *const ptr = realloc(prev_ptr, size);
    if (!ptr) {
        return errno;
    }

    *dest = ptr;
    return 0;
}

errno_t My_strdup(char **const dest, char const *const src) {
    assert(dest); assert(src);

    char *const ptr = strdup(src);
    if (!ptr) {
        return errno;
    }

    *dest = ptr;
    return 0;
}

errno_t My_fread(void *__restrict const buffer, size_t const size, size_t const num,
                 FILE *__restrict const stream) {
    assert(buffer); assert(stream);

    if (fread(buffer, size, num, stream) != num) {
        return ferror(stream);
    }

    return 0;
}

errno_t My_fwrite(void const *__restrict const buffer, size_t const size, size_t const num,
                  FILE *__restrict const stream) {
    assert(buffer); assert(stream);

    if (fwrite(buffer, size, num, stream) != num) {
        fflush(stderr);
        return ferror(stream);
    }

    return 0;
}

#undef FINAL_CODE

errno_t My_sscanf_s(size_t const count, char const *__restrict const buffer,
                    char const *__restrict const format, ...) {
    assert(buffer); assert(format);

    va_list args = nullptr;
    va_start(args, format);
    #define FINAL_CODE  \
    va_end(args);

    if (vsscanf_s(buffer, format, args) != (ssize_t)count) {
        CLEAR_RESOURCES();
        return ferror(stdin);
    }

    CLEAR_RESOURCES();
    return 0;

    #undef FINAL_CODE
}

errno_t My_scanf_s(size_t const count, char const *__restrict const format, ...) {
    assert(format);

    va_list args = nullptr;
    va_start(args, format);
    #define FINAL_CODE  \
    va_end(args);

    if (vscanf_s(format, args) != (ssize_t)count) {
        CLEAR_RESOURCES();
        return ferror(stdin);
    }

    CLEAR_RESOURCES();
    return 0;

    #undef FINAL_CODE
}
