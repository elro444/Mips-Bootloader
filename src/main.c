#include "serial.h"

int main(void) {
    char buf[30] = {0};
    while (1) {
        serial_out("What is your name?\n");
        serial_gets(buf, sizeof(buf));
        serial_out("Hello ");
        serial_out(buf);
        serial_out("!\n");
    }
    return 0;
}

