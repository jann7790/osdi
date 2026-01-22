.section ".text"

// PL011 UART
.equ UART_DR, 0x3F201000
.equ UART_FR, 0x3F201018

_start:
shell_loop:
    ldr x0, =prompt
    bl puts
    
    ldr x0, =buffer
    bl getline
    
    ldr x0, =buffer
    bl run_cmd
    
    b shell_loop

// Read line: x0=buffer
getline:
    mov x19, x0 // buffer
    mov x20, x30 // return address
getline_loop:
    bl getc
    cmp w0, #'\n'
    beq getline_end
    cmp w0, #'\r'
    beq getline_end
    strb w0, [x19], #1 //store one byte to x19, and increase pointer
    bl putc
    b getline_loop
getline_end:
    mov w0, #0
    strb w0, [x19]
    mov w0, #'\n'
    bl putc
    mov x30, x20
    ret

// Run command: x0=cmd string
run_cmd:
    mov x19, x0
    mov x20, x30
    
    ldrb w0, [x19]
    cbz w0, run_end
    
    ldr x1, =str_help
    mov x0, x19
    bl strcmp
    cbz w0, cmd_help
    
    ldr x1, =str_hello
    mov x0, x19
    bl strcmp
    cbz w0, cmd_hello
    
    ldr x1, =str_hwinfo
    mov x0, x19
    bl strcmp
    cbz w0, cmd_hwinfo
    
    b run_end

cmd_help:
    ldr x0, =msg_help
    bl puts
    b run_end

cmd_hello:
    ldr x0, =msg_hello
    bl puts
    b run_end

cmd_hwinfo:
    bl get_board_revision
    b run_end

run_end:
    mov x30, x20
    ret

// String compare: x0=s1, x1=s2, return w0
strcmp:
    mov x2, x0
    mov x3, x1
strcmp_loop:
    ldrb w4, [x2], #1
    ldrb w5, [x3], #1
    cmp w4, w5
    bne strcmp_ne
    cbz w4, strcmp_eq
    b strcmp_loop
strcmp_eq:
    mov w0, #0
    ret
strcmp_ne:
    sub w0, w4, w5
    ret

// Get char: return in w0
getc:
    ldr x1, =UART_FR
getc_wait:
    ldr w2, [x1]
    and w2, w2, #0x10
    cbnz w2, getc_wait // w2 != 0 then wait
    ldr x1, =UART_DR
    ldr w0, [x1]
    and w0, w0, #0xFF // w0 32bits, UART only 1 byte
    ret

// Put char: w0=char
putc:
    ldr x1, =UART_FR
putc_wait:
    ldr w2, [x1]
    and w2, w2, #0x20
    cbnz w2, putc_wait
    ldr x1, =UART_DR
    str w0, [x1]
    ret

// Put string: x0=string
puts:
    mov x19, x0
    mov x21, x30
puts_loop:
    ldrb w0, [x19], #1
    cbz w0, puts_end
    bl putc
    b puts_loop
puts_end:
    mov x30, x21
    ret

// Get board revision via mailbox
// Uses: x19, x20, x21
get_board_revision:
    mov x21, x30  // Save return address in x21 (cmd_hwinfo doesn't use this)
    
    // Build mailbox message in mailbox_buffer
    ldr x19, =mailbox_buffer
    
    // Message size (8 words = 32 bytes)
    mov w0, #32
    str w0, [x19, #0]
    
    // Request code
    ldr w0, =MBOX_REQUEST
    str w0, [x19, #4]
    
    // Tag: Get board revision
    ldr w0, =TAG_GET_BOARD_REV
    str w0, [x19, #8]
    
    // Value buffer size
    mov w0, #4
    str w0, [x19, #12]
    
    // Request/response code
    mov w0, #0
    str w0, [x19, #16]
    
    // Value buffer (space for response)
    str w0, [x19, #20]
    
    // End tag
    str w0, [x19, #24]
    
    // Padding for 16-byte alignment
    str w0, [x19, #28]
    
    // Call mailbox
    mov x0, x19
    bl mailbox_call
    
    // Check result (w0 = 0 on success, 1 on failure)
    cbnz w0, get_board_rev_fail
    
    // Success - print message
    ldr x0, =msg_board_rev
    bl puts
    
    // Get revision value
    ldr w20, [x19, #20]  // Load board revision into w20
    
    // Print hex prefix
    ldr x0, =msg_hex_prefix
    bl puts
    
    // Print the revision in hex (simplified - just print the value)
    mov w0, w20
    bl print_simple_hex
    
    ldr x0, =msg_newline
    bl puts
    
    b get_board_rev_end
    
get_board_rev_fail:
    ldr x0, =msg_mbox_fail
    bl puts
    
get_board_rev_end:
    mov x30, x21
    ret

// Print a simple hex value: w0 = 32-bit value to print
// Uses: x19, x20, x21, x22
print_simple_hex:
    mov x22, x30  // Save return address
    mov w20, w0   // Save value
    mov x21, #8   // 8 hex digits
    
print_hex_loop:
    sub x21, x21, #1
    
    // Extract nibble - shift by (x21 * 4)
    lsl w19, w21, #2  // Multiply x21 by 4 using left shift
    lsr w0, w20, w19
    and w0, w0, #0xF
    
    // Convert to ASCII
    cmp w0, #10
    blt print_hex_digit
    add w0, w0, #('a' - 10)
    b print_hex_char
    
print_hex_digit:
    add w0, w0, #'0'
    
print_hex_char:
    bl putc
    
    cbnz x21, print_hex_loop
    
    mov x30, x22
    ret

// Mailbox call: x0=mailbox buffer address
// Returns: 0 on success, 1 on failure
mailbox_call:
    mov x19, x0
    mov x20, x30
    
    // Get mailbox buffer address (upper 28 bits) and combine with channel (8)
    mov x1, x19
    and x1, x1, #0xFFFFFFF0  // Clear lower 4 bits
    orr x1, x1, #8           // Set channel to 8
    
    // Wait for mailbox to be not full
    ldr x2, =MBOX_STATUS
mbox_full_wait:
    ldr w3, [x2]
    and w3, w3, #MBOX_FULL
    cbnz w3, mbox_full_wait
    
    // Write to mailbox
    ldr x2, =MBOX_WRITE
    str w1, [x2]
    
    // Wait for response
mbox_response_wait:
    // Wait for mailbox to be not empty
    ldr x2, =MBOX_STATUS
mbox_empty_wait:
    ldr w3, [x2]
    and w3, w3, #MBOX_EMPTY
    cbnz w3, mbox_empty_wait
    
    // Read response
    ldr x2, =MBOX_READ
    ldr w3, [x2]
    
    // Check if it's our message
    cmp w3, w1
    bne mbox_response_wait
    
    // Check if request succeeded
    ldr w0, [x19, #4]
    ldr w2, =MBOX_RESPONSE_SUCCESS
    cmp w0, w2
    beq mbox_success
    
    mov w0, #1  // Failed
    b mbox_end
    
mbox_success:
    mov w0, #0  // Success
    
mbox_end:
    mov x30, x20
    ret

// Mailbox registers
.equ MMIO_BASE,           0x3F000000
.equ MBOX_BASE,           MMIO_BASE + 0xB880
.equ MBOX_READ,           MBOX_BASE + 0x00
.equ MBOX_STATUS,         MBOX_BASE + 0x18
.equ MBOX_WRITE,          MBOX_BASE + 0x20

// Mailbox status flags
.equ MBOX_FULL,           0x80000000
.equ MBOX_EMPTY,          0x40000000

// Mailbox codes
.equ MBOX_REQUEST,        0x00000000
.equ MBOX_RESPONSE_SUCCESS, 0x80000000

// Tags
.equ TAG_GET_BOARD_REV,   0x00010002

.section ".data"
prompt:         .asciz "# "
msg_hello:      .asciz "Hello World!\n"
msg_help:       .asciz "help: print all available commands\nhello: print Hello World!\nhwinfo: show hardware info\n"
msg_board_rev:  .asciz "Board Revision: "
msg_mbox_fail:  .asciz "Mailbox call failed\n"
msg_hex_prefix: .asciz "0x"
msg_newline:    .asciz "\n"
str_help:       .asciz "help"
str_hello:      .asciz "hello"
str_hwinfo:     .asciz "hwinfo"

.section ".bss"
.align 4
buffer:         .skip 128
mailbox_buffer: .skip 128
