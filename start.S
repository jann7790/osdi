.section ".text.boot"

.global _start

_start:
    // Get CPU ID
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, master
    // If CPU ID > 0, hang
hang:
    wfe
    b       hang

master:
    // Set stack pointer
    ldr     x1, =__stack_top
    mov     sp, x1

    // Clear BSS
    ldr     x1, =__bss_start
    ldr     x2, =__bss_end
clear_bss:
    cmp     x1, x2
    b.ge    clear_bss_done
    str     xzr, [x1]
    add     x1, x1, #8
    b       clear_bss

clear_bss_done:
    // Jump to main
    bl      main
    // If main returns, hang
    b       hang
