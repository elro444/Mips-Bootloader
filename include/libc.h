#ifndef LIBC_H
#define LIBC_H

#include "types.h"

int strcmp(const char *a, const char *b);
void strcat(char *a, const char *b);
unsigned strlen(char *str);

static inline void memcpy(u8 *dst, const u8 *src, u32 size)
{
    for (u32 i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
}

void memmove(u8 *dst, const u8 *src, u32 size);

#endif /* LIBC_H */
