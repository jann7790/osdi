SECTIONS
{
  . = 0x80000;
  .text : {
    *(.text.boot)
    *(.text)
  }
  .rodata : {
    *(.rodata)
  }
  .data : {
    *(.data)
  }
  . = ALIGN(16);
  __bss_start = .;
  .bss : {
    *(.bss)
  }
  __bss_end = .;
  . = ALIGN(16);
  . += 0x4000; /* 16KB stack */
  __stack_top = .;
}
