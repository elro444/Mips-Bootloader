#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lzma/lzma.h"


void read_from_file(const char *filename, u8 **buf, u32 *outsize)
{
    int fd = open(filename, O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    *buf = malloc(size);
    read(fd, *buf, size);
    close(fd);
    *outsize = size;
}


void write_to_file(const char *filename, const u8 *buf, const u32 size)
{
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    write(fd, buf, size);
    close(fd);
}


int main()
{
    u8 *input = NULL;
    u8 *output = NULL;
    u32 input_size = 0;

    read_from_file("input.bin", &input, &input_size);
    u32 output_size = input_size * 30;
    output = malloc(output_size);
    int ret = lzma_inflate(input, input_size, output, &output_size);
    printf("lzma_inflate: %d\n", ret);
    write_to_file("output.bin", output, output_size);
    free(input);
    free(output);

    return 0;
}