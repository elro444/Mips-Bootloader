#include "serial.h"

#define UART0_BASE (0x1fd003f8) /* 8250 COM1 */
#define UART0_REG(index) ((char *)(UART0_BASE + index))

void init_serial(void)
{
    volatile char *addr;
    *UART0_REG(1) = 0x00;
    *UART0_REG(3) = 0x80;
    *UART0_REG(0) = 0x03;
    *UART0_REG(1) = 0x00;
    *UART0_REG(3) = 0x03;
    *UART0_REG(2) = 0xc7;
    *UART0_REG(4) = 0x0b;
}

static inline int is_transmit_empty(void)
{
    return *UART0_REG(5) & 0x20;
}

static inline void serial_putc(char c)
{
    do {} while (!is_transmit_empty());
    *UART0_REG(0) = c;
}

void serial_out(const char *string)
{
    while (*string != '\0')
    {
        serial_putc(*string);
        string++;
    }
}

static inline int is_receive_empty(void)
{
    return *UART0_REG(5) & 0x1;
}

char serial_getch(void)
{
    do {} while (!is_receive_empty());
    return *UART0_REG(0);
}
