#ifndef STRING_H
#define STRING_H

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



#endif /* STRING_H */
