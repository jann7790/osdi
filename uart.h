#ifndef UART_H
#define UART_H

// PL011 UART registers
#define UART_BASE       0x3F201000
#define UART_DR         (UART_BASE + 0x00)
#define UART_FR         (UART_BASE + 0x18)

// UART Flag Register bits
#define UART_FR_RXFE    0x10  // Receive FIFO empty
#define UART_FR_TXFF    0x20  // Transmit FIFO full

// Function declarations
void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *s);
void uart_hex(unsigned int num);
void uart_dec(unsigned int num);

#endif
