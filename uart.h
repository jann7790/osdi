#ifndef UART_H
#define UART_H

void uart_init(void);
char uart_getc(void);
void uart_putc(char c);
void uart_puts(const char* str);
void uart_hex(unsigned int val);
void uart_dec(unsigned int val);

#endif
