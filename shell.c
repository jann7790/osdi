#include "shell.h"
#include "uart.h"
#include "mailbox.h"

// String utility functions
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strlen(const char* s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

// Reboot function
#define PM_PASSWORD 0x5a000000
#define PM_RSTC     0x3F10001c
#define PM_WDOG     0x3F100024

static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int*)reg = val;
}

void reset(int tick) {
    mmio_write(PM_RSTC, PM_PASSWORD | 0x20);
    mmio_write(PM_WDOG, PM_PASSWORD | tick);
}

// Command handlers
void cmd_help(void) {
    uart_puts("Available commands:\n");
    uart_puts("  help     - Show this help message\n");
    uart_puts("  hello    - Print Hello World!\n");
    uart_puts("  hwinfo   - Display hardware information\n");
    uart_puts("  reboot   - Reboot the system\n");
}

void cmd_hello(void) {
    uart_puts("Hello World!\n");
}

void cmd_hwinfo(void) {
    uart_puts("Hardware Information:\n");
    
    // Get board revision
    unsigned int revision = get_board_revision();
    uart_puts("  Board Revision: ");
    uart_hex(revision);
    uart_puts("\n");
    
    // Get ARM memory
    unsigned int base, size;
    get_arm_memory(&base, &size);
    uart_puts("  ARM Memory Base: ");
    uart_hex(base);
    uart_puts("\n");
    uart_puts("  ARM Memory Size: ");
    uart_hex(size);
    uart_puts(" (");
    uart_dec(size / (1024 * 1024));
    uart_puts(" MB)\n");
}

void cmd_reboot(void) {
    uart_puts("Rebooting...\n");
    reset(100);
    while(1);
}

// Shell functions
static char cmd_buffer[MAX_CMD_LEN];
static int cmd_len = 0;

void shell_init(void) {
    cmd_len = 0;
    uart_puts("\n");
    uart_puts("========================================\n");
    uart_puts("  Raspberry Pi 3 Bare Metal Shell\n");
    uart_puts("  Lab 1: Hello World\n");
    uart_puts("========================================\n");
    uart_puts("Type 'help' for available commands.\n\n");
}

void shell_prompt(void) {
    uart_puts("# ");
}

void shell_process_command(void) {
    // Null-terminate the command
    cmd_buffer[cmd_len] = '\0';
    
    if (cmd_len == 0) {
        return;
    }
    
    // Process commands
    if (strcmp(cmd_buffer, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd_buffer, "hello") == 0) {
        cmd_hello();
    } else if (strcmp(cmd_buffer, "hwinfo") == 0) {
        cmd_hwinfo();
    } else if (strcmp(cmd_buffer, "reboot") == 0) {
        cmd_reboot();
    } else {
        uart_puts("Unknown command: ");
        uart_puts(cmd_buffer);
        uart_puts("\n");
        uart_puts("Type 'help' for available commands.\n");
    }
}

void shell_run(void) {
    shell_prompt();
    
    while (1) {
        char c = uart_getc();
        
        // Handle newline/carriage return
        if (c == '\n' || c == '\r') {
            uart_puts("\n");
            shell_process_command();
            cmd_len = 0;
            shell_prompt();
        }
        // Handle backspace
        else if (c == 127 || c == 8) {
            if (cmd_len > 0) {
                cmd_len--;
                uart_puts("\b \b");  // Move back, print space, move back again
            }
        }
        // Handle regular characters
        else if (c >= 32 && c < 127) {
            if (cmd_len < MAX_CMD_LEN - 1) {
                cmd_buffer[cmd_len++] = c;
                uart_putc(c);
            }
        }
    }
}
