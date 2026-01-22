.section ".text"

// PL011 UART Base Address (Primary UART for Raspberry Pi 3)
.equ UART_BASE,   0x3F201000
.equ UART_DR,     0x3F201000
.equ UART_FR,     0x3F201018

_start:
    // Print "Hello World\n"
    ldr x0, =hello_msg
    bl uart_puts
    
    // Infinite loop
hang:
    wfe
    b hang

// Send a single character via UART
uart_putc:
    ldr x1, =UART_FR
wait_txready:
    ldr w2, [x1]
    and w2, w2, #0x20         // Check TXFF (transmit FIFO full)
    cbnz w2, wait_txready     // Wait if full
    
    ldr x1, =UART_DR
    str w0, [x1]
    ret

// Send a null-terminated string via UART
uart_puts:
    mov x19, x0               // Save string pointer
    mov x20, x30              // Save return address
next_char:
    ldrb w0, [x19], #1        // Load byte and increment
    cbz w0, puts_done         // If null, we're done
    bl uart_putc              // Print character
    b next_char
puts_done:
    mov x30, x20              // Restore return address
    ret

.section ".data"
hello_msg:
    .asciz "Hello World\n"
