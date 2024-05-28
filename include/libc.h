#ifndef LIBC_H
#define LIBC_H

#include "types.h"

static inline int strcmp(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b) {
            break;
        }
        ++a;
        ++b;
    }
    return *a - *b;
}

static inline void strcat(char *a, const char *b)
{
    while (*a) {
        ++a;
    }
    while (*b) {
        *a++ = *b++;
    }
    *a = 0;
}

static inline unsigned strlen(char *str)
{
    unsigned length = 0;
    while (*str++) {
        ++length;
    }
    return length;
}

static inline void memcpy(u8 *src, u8 *dst, u32 size)
{
    for (u32 i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
}

#endif /* LIBC_H */
