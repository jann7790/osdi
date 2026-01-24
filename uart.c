#include "uart.h"

// MMIO read/write functions
static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int *)reg = val;
}

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int *)reg;
}

/**
 * uart_putc - Send a character to UART
 * @c: Character to send
 */
void uart_putc(char c) {
    // Wait until transmit FIFO is not full
    while (mmio_read(UART_FR) & UART_FR_TXFF) {
        // Wait
    }
    
    // Write character to data register
    mmio_write(UART_DR, (unsigned int)c);
}

/**
 * uart_getc - Receive a character from UART
 * 
 * Returns: Character received
 */
char uart_getc(void) {
    // Wait until receive FIFO is not empty
    while (mmio_read(UART_FR) & UART_FR_RXFE) {
        // Wait
    }
    
    // Read character from data register
    return (char)(mmio_read(UART_DR) & 0xFF);
}

/**
 * uart_puts - Send a null-terminated string to UART
 * @s: String to send
 */
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

/**
 * uart_hex - Print a number in hexadecimal format
 * @num: Number to print
 */
void uart_hex(unsigned int num) {
    uart_puts("0x");
    
    char hex_chars[] = "0123456789ABCDEF";
    int i;
    int started = 0;
    
    for (i = 28; i >= 0; i -= 4) {
        unsigned int digit = (num >> i) & 0xF;
        if (digit != 0 || started || i == 0) {
            uart_putc(hex_chars[digit]);
            started = 1;
        }
    }
}

/**
 * uart_dec - Print a number in decimal format
 * @num: Number to print
 */
void uart_dec(unsigned int num) {
    if (num == 0) {
        uart_putc('0');
        return;
    }
    
    char buf[10];
    int i = 0;
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Print in reverse order
    while (i > 0) {
        uart_putc(buf[--i]);
    }
}
