#include "uart.h"
#include "mailbox.h"

// String comparison function
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Command: help
void cmd_help(void) {
    uart_puts("help        : print all available commands\n");
    uart_puts("hello       : print Hello World!\n");
    uart_puts("hwinfo      : print hardware information\n");
    uart_puts("reboot      : reboot the device\n");
}

// Command: hello
void cmd_hello(void) {
    uart_puts("Hello World!\n");
}

// Command: hwinfo
void cmd_hwinfo(void) {
    unsigned int revision, base, size;
    
    // Get board revision
    revision = get_board_revision();
    uart_puts("Board revision: ");
    uart_hex(revision);
    uart_puts("\n");
    
    // Get ARM memory
    get_arm_memory(&base, &size);
    uart_puts("ARM memory base address: ");
    uart_hex(base);
    uart_puts("\n");
    uart_puts("ARM memory size: ");
    uart_hex(size);
    uart_puts(" (");
    uart_dec(size / 1024 / 1024);
    uart_puts(" MB)\n");
}

// Command: reboot
void cmd_reboot(void) {
    uart_puts("Rebooting...\n");
    
    // PM_PASSWORD and addresses for watchdog reset
    #define PM_PASSWORD 0x5a000000
    #define PM_RSTC     0x3F10001c
    #define PM_WDOG     0x3F100024
    
    volatile unsigned int *pm_rstc = (unsigned int *)PM_RSTC;
    volatile unsigned int *pm_wdog = (unsigned int *)PM_WDOG;
    
    // Full reset
    *pm_rstc = PM_PASSWORD | 0x20;
    *pm_wdog = PM_PASSWORD | 10; // 10 ticks
    
    // Wait for reset
    while (1) {
        // Hang
    }
}

// Process command
void run_command(char *cmd) {
    // Skip empty commands
    if (cmd[0] == '\0') {
        return;
    }
    
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "hello") == 0) {
        cmd_hello();
    } else if (strcmp(cmd, "hwinfo") == 0) {
        cmd_hwinfo();
    } else if (strcmp(cmd, "reboot") == 0) {
        cmd_reboot();
    } else {
        uart_puts("Unknown command: ");
        uart_puts(cmd);
        uart_puts("\n");
    }
}

// Read line from UART
void read_line(char *buffer, int max_len) {
    int i = 0;
    char c;
    
    while (i < max_len - 1) {
        c = uart_getc();
        
        // Handle backspace
        if (c == 127 || c == 8) {
            if (i > 0) {
                i--;
                uart_puts("\b \b"); // Erase character on screen
            }
            continue;
        }
        
        // Echo character
        uart_putc(c);
        
        // Handle newline
        if (c == '\n' || c == '\r') {
            uart_putc('\n');
            break;
        }
        
        buffer[i++] = c;
    }
    
    buffer[i] = '\0';
}

// Main entry point from assembly
void main(void) {
    char buffer[128];
    
    uart_puts("\n");
    uart_puts("=================================\n");
    uart_puts("  Raspberry Pi 3 Simple Shell\n");
    uart_puts("=================================\n");
    uart_puts("\n");
    
    // Main shell loop
    while (1) {
        uart_puts("# ");
        read_line(buffer, sizeof(buffer));
        run_command(buffer);
    }
}
