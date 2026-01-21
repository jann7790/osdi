#include "uart.h"

// Mini UART registers
#define MMIO_BASE       0x3F000000
#define AUX_ENABLES     (MMIO_BASE + 0x00215004)
#define AUX_MU_IO_REG   (MMIO_BASE + 0x00215040)
#define AUX_MU_IER_REG  (MMIO_BASE + 0x00215044)
#define AUX_MU_IIR_REG  (MMIO_BASE + 0x00215048)
#define AUX_MU_LCR_REG  (MMIO_BASE + 0x0021504C)
#define AUX_MU_MCR_REG  (MMIO_BASE + 0x00215050)
#define AUX_MU_LSR_REG  (MMIO_BASE + 0x00215054)
#define AUX_MU_MSR_REG  (MMIO_BASE + 0x00215058)
#define AUX_MU_SCRATCH  (MMIO_BASE + 0x0021505C)
#define AUX_MU_CNTL_REG (MMIO_BASE + 0x00215060)
#define AUX_MU_STAT_REG (MMIO_BASE + 0x00215064)
#define AUX_MU_BAUD_REG (MMIO_BASE + 0x00215068)

// GPIO registers
#define GPFSEL1         (MMIO_BASE + 0x00200004)
#define GPPUD           (MMIO_BASE + 0x00200094)
#define GPPUDCLK0       (MMIO_BASE + 0x00200098)

// Helper functions
static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int*)reg = val;
}

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int*)reg;
}

static inline void delay(int count) {
    for (int i = 0; i < count; i++) {
        asm volatile("nop");
    }
}

void uart_init(void) {
    // Enable mini UART
    mmio_write(AUX_ENABLES, 1);
    
    // Disable TX/RX during configuration
    mmio_write(AUX_MU_CNTL_REG, 0);
    
    // Disable interrupts
    mmio_write(AUX_MU_IER_REG, 0);
    
    // Set 8-bit mode
    mmio_write(AUX_MU_LCR_REG, 3);
    
    // Set RTS high
    mmio_write(AUX_MU_MCR_REG, 0);
    
    // Clear FIFOs
    mmio_write(AUX_MU_IIR_REG, 0xC6);
    
    // Set baud rate to 115200
    // Baud rate = system_clock / (8 * (baud_reg + 1))
    // For 250MHz clock: 250000000 / (8 * 115200) ≈ 270
    mmio_write(AUX_MU_BAUD_REG, 270);
    
    // Configure GPIO pins 14 and 15
    unsigned int selector = mmio_read(GPFSEL1);
    selector &= ~(7 << 12);  // Clear GPIO 14
    selector |= 2 << 12;     // Set alt5 for GPIO 14
    selector &= ~(7 << 15);  // Clear GPIO 15
    selector |= 2 << 15;     // Set alt5 for GPIO 15
    mmio_write(GPFSEL1, selector);
    
    // Disable pull up/down for pins 14 and 15
    mmio_write(GPPUD, 0);
    delay(150);
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    mmio_write(GPPUDCLK0, 0);
    
    // Enable TX and RX
    mmio_write(AUX_MU_CNTL_REG, 3);
}

char uart_getc(void) {
    // Wait for data
    while (!(mmio_read(AUX_MU_LSR_REG) & 0x01));
    return mmio_read(AUX_MU_IO_REG) & 0xFF;
}

void uart_putc(char c) {
    // Wait for space in transmit FIFO
    while (!(mmio_read(AUX_MU_LSR_REG) & 0x20));
    mmio_write(AUX_MU_IO_REG, c);
}

void uart_puts(const char* str) {
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
        }
        uart_putc(*str++);
    }
}

void uart_hex(unsigned int val) {
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        int digit = (val >> i) & 0xF;
        if (digit < 10) {
            uart_putc('0' + digit);
        } else {
            uart_putc('A' + digit - 10);
        }
    }
}

void uart_dec(unsigned int val) {
    if (val == 0) {
        uart_putc('0');
        return;
    }
    
    char buffer[10];
    int i = 0;
    while (val > 0) {
        buffer[i++] = '0' + (val % 10);
        val /= 10;
    }
    
    // Print in reverse
    while (i > 0) {
        uart_putc(buffer[--i]);
    }
}
