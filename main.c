#include "serial.h"

int main(void) {
    char buf[] = "Got *X*";
    serial_out("Hello, serials!\n");
    buf[5] = serial_getch();
    serial_out(buf);
    return 0;
}

