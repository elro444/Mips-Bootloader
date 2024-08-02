#ifndef LIBC_H
#define LIBC_H

#include "types.h"

int strcmp(const char *a, const char *b);
void strcat(char *a, const char *b);
unsigned strlen(char *str);
int strchr(const char *str, const char c);
char *strstr(const char *haystack, const char *needle);
char *strtok(const char *str, const char *delim, char **saveptr);

static inline void memcpy(u8 *dst, const u8 *src, u32 size)
{
    for (u32 i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
}

void memmove(u8 *dst, const u8 *src, u32 size);
void memset(u8 *dst, u8 value, u32 size);
int memcmp(const u8 *a, const u8 *b, u32 size);

#endif /* LIBC_H */
