#ifndef LZMA_H
#define LZMA_H

#include "types.h"

int lzma_deflate(const u8 *data, u32 data_size, u8 *dst, u32 *dst_size);
int lzma_inflate(const u8 *data, u32 data_size, u8 *dst, u32 *dst_size);

#endif /* LZMA_H */
