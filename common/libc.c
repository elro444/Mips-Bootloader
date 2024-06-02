#include "libc.h"


void memmove(u8 *dst, const u8 *src, u32 size)
{
    if (src > dst) {
        for (u32 i = 0; i < size; ++i) {
            dst[i] = src[i];
        }
    }
    else {
        for (u32 i = size - 1; i >= 0; --i) {
            dst[i] = src[i];
            if (i == 0)
                break;
        }
    }
}

int strcmp(const char *a, const char *b)
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

void strcat(char *a, const char *b)
{
    while (*a) {
        ++a;
    }
    while (*b) {
        *a++ = *b++;
    }
    *a = 0;
}

unsigned strlen(char *str)
{
    unsigned length = 0;
    while (*str++) {
        ++length;
    }
    return length;
}