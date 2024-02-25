#ifndef LZMA_H
#define LZMA_H

#define   R_OK                           0
#define   R_ERR_MEMORY_RUNOUT            1
#define   R_ERR_UNSUPPORTED              2
#define   R_ERR_OUTPUT_OVERFLOW          3
#define   R_ERR_INPUT_OVERFLOW           4
#define   R_ERR_DATA                     5
#define   R_ERR_OUTPUT_LEN_MISMATCH      6

int lzma_deflate(char *dst, const char *data, unsigned data_size);
int lzma_inflate(char *dst, const char *data, unsigned data_size);

#endif /* LZMA_H */
