#ifndef SEIRAL_H
#define SEIRAL_H

void init_serial(void);
void serial_out(const char *str);
char serial_getch(void);
void serial_gets(char *buffer, unsigned size);

#endif // SEIRAL_H
