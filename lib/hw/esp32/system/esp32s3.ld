MEMORY
{
  D_SRAM(rwx) : ORIGIN = 0x3FC88000, LENGTH = 480K
  I_SRAM(rw)  : ORIGIN = 0x40370000, LENGTH = 448K
}



_eram = ORIGIN(D_SRAM) + LENGTH(D_SRAM);
ENTRY(_reset)

SECTIONS {
  /* .vectors  : { KEEP(*(.vectors))   } > I_SRAM */
  .text     : { *(.text*) *(.text)  } > I_SRAM

  .data : {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    *(.rodata)
    *(.rodata*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    . = ALIGN(4);
    _edata = .;
  } > D_SRAM

  .bss : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > D_SRAM

  /*
  /DISCARD/ : { *(.debug) *(.debug*) *(.xtensa.*) *(.comment) }
  */
}
