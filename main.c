#include "uart.h"
#include "shell.h"

void main(void) {
    // Initialize UART
    uart_init();
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    // Should never reach here
    while (1);
}
