#include "stdio.h"
#include "stdlib.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lzma/lzma.h"


void read_from_file(const char *filename, char **buf, unsigned *outsize)
{
    int fd = open(filename, O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    *buf = malloc(size);
    read(fd, *buf, size);
    close(fd);
    *outsize = size;
}


void write_to_file(const char *filename, const char *buf, const unsigned size)
{
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    write(fd, buf, size);
    close(fd);
}


int main()
{
    char *input = NULL;
    unsigned input_size = 0;
    char *output = NULL;

    read_from_file("input.bin", &input, &input_size);
    write_to_file("output.bin", input, input_size);
    free(input);

    return 0;
}