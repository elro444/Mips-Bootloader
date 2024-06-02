#ifndef TYPES_H
#define TYPES_H


typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;


#ifndef NO_LIBC_TYPES

#define NULL ((void*)0)
typedef u32 size_t;

#define _rev32(x) ( \
    ((x) << 24) | \
    ((x & 0x0000ff00) << 8) | \
    ((x & 0x00ff0000) >> 8) | \
    ((x) >> 24) \
)
#define _rev16(x) ( \
    ((x) << 8) | \
    ((x) >> 8)\
)

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define htonl(x) (x)
#define htons(x) (x)
#define ntohl(x) htonl(x)
#define ntohs(x) htons(x)
#define le_to_host32(x) htonl(x)
#define le_to_host16(x) htons(x)
#define be_to_host32(x) (x)
#define be_to_host16(x) (x)
#else
#define htonl(x) _rev32(x)
#define htons(x) _rev16(x)
#define ntohl(x) htonl(x)
#define ntohs(x) htons(x)
#define le_to_host32(x) (x)
#define le_to_host16(x) (x)
#define be_to_host32(x) ntohl(x)
#define be_to_host16(x) ntohs(x)
#endif

#endif /* NO_LIBC_TYPES */

#endif /* TYPES_H */
