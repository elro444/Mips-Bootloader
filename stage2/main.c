#include "serial.h"
#include "libc.h"

int main(void)
{
    serial_out("Hello from stage 2!!\n");
    while (1);
    return 0;
}