#include "libc.h"
#include "serial.h"

void memmove(void *dst, const void *src, u32 size)
{
    u8 *dst8 = dst;
    const u8 *src8 = src;
    if (src > dst) {
        for (u32 i = 0; i < size; ++i) {
            dst8[i] = src8[i];
        }
    }
    else {
        for (u32 i = size - 1; i >= 0; --i) {
            dst8[i] = src8[i];
            if (i == 0)
                break;
        }
    }
}

void memset(void *dst, u8 value, u32 size)
{
    u8 *dst8 = dst;
    while (size-- > 0)
        *dst8++ = value;
}

int memcmp(const void *a, const void *b, u32 size)
{
    const u8 *a8 = a;
    const u8 *b8 = b;
    while (size > 0) {
        if (*a8 != *b8)
            return *a8 - *b8;
        ++a8;
        ++b8;
        --size;
    }
    return 0;
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

unsigned strlen(const char *str)
{
    unsigned length = 0;
    while (*str++) {
        ++length;
    }
    return length;
}

int strchr(const char *str, const char c)
{
    int index = 0;
    while (*str) {
        if (*str++ == c)
            return index;
        ++index;
    }
    return -1;
}

char *strstr(const char *haystack, const char *needle)
{
    u32 needle_len = strlen(needle);
    while (*haystack) {

        if (0 == memcmp(haystack, needle, needle_len)) {
    
            return (char *)haystack;
        }
        ++haystack;
    }
    return NULL;
}

char *strtok(char *str, const char *delim, char **saveptr)
{
    unsigned delim_length = strlen(delim);
    if (NULL != str) {
        // First call, start from the beginning
        *saveptr = (char *)str;
    }
    else {
        str = *saveptr;
    }

    if (NULL == str)
        return NULL;

    char *next_delim = strstr(str, delim);
    if (NULL != next_delim) {
        memset(next_delim, 0, delim_length);
        *saveptr = next_delim + delim_length;
    } else {
        *saveptr = NULL;
    }
    
    return str;
}

