#include "serial.h"

#include <stdarg.h>
#include "types.h"

#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"

#define UART0_BASE (0x1fd003f8) /* 8250 COM1 */
#define UART0_REG(index) ((char *)(UART0_BASE + index))

static inline int is_transmit_empty(void)
{
    return *UART0_REG(5) & 0x20;
}

void serial_putc(char c)
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

void gets(char *buffer, unsigned size)
{
    unsigned received = 0;
    while (received < size) {
        char c = serial_getch();
        if (c == '\n' || c == '\r') {
            serial_putc('\n');
            break;
        }
        serial_putc(c);
        if (c == '\b') {
            serial_putc('X');
        }
        buffer[received++] = c;
    }
    // Add a terminating NULL
    if (received >= size) {
        received = size - 1;
    }
    buffer[received] = 0;
}

void puts(const char *str)
{
    serial_out(str);
    serial_putc('\n');
}

void putc_wrapper(int c, void *unused)
{
    serial_putc((char)c);
}

int printf(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    int const rv = npf_vpprintf(putc_wrapper, NULL, format, val);
    va_end(val);
    return rv;
}