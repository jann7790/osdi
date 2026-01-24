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
    
    b run_end

cmd_help:
    ldr x0, =msg_help
    bl puts
    b run_end

cmd_hello:
    ldr x0, =msg_hello
    bl puts

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

.section ".data"
prompt:     .asciz "# "
msg_hello:  .asciz "Hello World!\n"
msg_help:   .asciz "help: print all available commands\nhello: print Hello World!\n"
str_help:   .asciz "help"
str_hello:  .asciz "hello"
str_test:  .asciz "aaaaaaaaaaaaaaaaaaaaaaaaaaa"

.section ".bss"
buffer:     .skip 128
