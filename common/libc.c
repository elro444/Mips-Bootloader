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

void memset(u8 *dst, u8 value, u32 size)
{
    while (size-- > 0)
        *dst++ = value;
}

int memcmp(const u8 *a, const u8 *b, u32 size)
{
    while (size > 0) {
        if (*a != *b)
            return *a - *b;
        ++a;
        ++b;
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

unsigned strlen(char *str)
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
        if (0 == memcmp(haystack, needle, needle_len))
            return haystack;
    }
    return NULL;
}

char *strtok(const char *str, const char *delim, char **saveptr)
{
    unsigned delim_length = strlen(delim);
    if (NULL != str) {
        // First call, start from the beginning
        *saveptr = str;
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

