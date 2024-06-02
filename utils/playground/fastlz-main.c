#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "types.h"
#include "fastlz.h"

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            printf("[%s:%d] Condition '%s' failed!\n", __func__, __LINE__, #cond); \
            exit(1); \
        } \
    } while (0);

void read_from_file(const char *filename, u8 **buf, u32 *outsize)
{
    int fd = open(filename, O_RDONLY);
    ASSERT(fd >= 0);
    int size = lseek(fd, 0, SEEK_END);
    ASSERT(size >= 0);
    lseek(fd, 0, SEEK_SET);
    *buf = malloc(size);
    ASSERT(NULL != buf);
    read(fd, *buf, size);
    close(fd);
    *outsize = size;
}


void write_to_file(const char *filename, const u8 *buf, const u32 size)
{
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT(fd >= 0);
    write(fd, buf, size);
    close(fd);
}


int main(int argc, char **argv)
{
    ASSERT(argc == 3);
    char *infile = argv[1];
    char *outfile = argv[2];

    u8 *input = NULL;
    u8 *output = NULL;
    u32 input_size = 0;

    read_from_file(infile, &input, &input_size);
    u32 output_size = input_size * 5;
    output = malloc(output_size);
    ASSERT(NULL != output);
    // int ret = fastlz_compress(input, input_size, output, output_size);
    int ret = fastlz_compress_level(2, input, input_size, output);
    fprintf(stderr, "Output length: %d\n", ret);
    write_to_file(outfile, output, ret);
    free(input);
    free(output);

    return 0;
}