#ifndef LIBC_H
#define LIBC_H

#include "types.h"

int strcmp(const char *a, const char *b);
void strcat(char *a, const char *b);
unsigned strlen(const char *str);
int strchr(const char *str, const char c);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *str, const char *delim, char **saveptr);

static inline void memcpy(void *dst, const void *src, u32 size)
{
    u8 *dst8 = dst;
    const u8 *src8 = src;
    for (u32 i = 0; i < size; ++i) {
        dst8[i] = src8[i];
    }
}

void memmove(void *dst, const void *src, u32 size);
void memset(void *dst, u8 value, u32 size);
int memcmp(const void *a, const void *b, u32 size);

#endif /* LIBC_H */
