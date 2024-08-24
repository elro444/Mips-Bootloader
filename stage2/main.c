#include "serial.h"
#include "libc.h"
#include "commands.h"


int main(void)
{
    char buffer[1024] = {0};
    printf("Hello from stage 2!!\n");
    while (true) {
        printf("> ");
        gets(buffer, sizeof(buffer));
        run_command(buffer);
    }
    while (1);
    return 0;
}