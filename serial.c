#include "serial.h"

#define UART0_BASE 0x1fd003f8 /* 8250 COM1 */

void init_serial(void)
{
    volatile char *addr;
    addr = (volatile char *)(UART0_BASE + 1);
    *addr = 0x00;
    addr = (volatile char *)(UART0_BASE + 3);
    *addr = 0x80;
    addr = (volatile char *)(UART0_BASE + 0);
    *addr = 0x03;
    addr = (volatile char *)(UART0_BASE + 1);
    *addr = 0x00;
    addr = (volatile char *)(UART0_BASE + 3);
    *addr = 0x03;
    addr = (volatile char *)(UART0_BASE + 2);
    *addr = 0xc7;
    addr = (volatile char *)(UART0_BASE + 4);
    *addr = 0x0b;
}

static int is_transmit_empty(void)
{
    volatile char *addr = (volatile char *)(UART0_BASE + 5);
    return *addr & 0x20;
}

static void serial_putc(char a)
{
    while (is_transmit_empty() == 0)
        ;
    volatile char *addr = (volatile char *)UART0_BASE;
    *addr = a;
}

void serial_out(const char *s)
{
    while (*s != '\0')
    {
        serial_putc(*s);
        s++;
    }
}
