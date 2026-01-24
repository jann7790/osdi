.section ".text.boot"

.global _start

_start:
    // Get processor ID
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, master
    // Not processor 0, hang
hang:
    wfe
    b       hang

master:
    // Clear BSS section
    ldr     x1, =__bss_start
    ldr     x2, =__bss_end
clear_bss:
    cmp     x1, x2
    beq     clear_bss_done
    str     xzr, [x1], #8
    b       clear_bss

clear_bss_done:
    // Set stack pointer
    ldr     x1, =_start
    mov     sp, x1

    // Branch to main
    bl      main

    // If main returns, hang
    b       hang
