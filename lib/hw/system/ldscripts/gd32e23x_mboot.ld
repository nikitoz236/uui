
MEMORY
{
    RAM (xrw)       : ORIGIN = 0x20000000,              LENGTH = 8K
    SIGNATURE (rx)  : ORIGIN = 0x08000000 + 4K,         LENGTH = 16
    FLASH (rx)      : ORIGIN = 0x08000000 + 4K + 16,    LENGTH = 64K - 4K - 16
}

SECTIONS
{
    .section_signature :
    {
        FILL(0)
        KEEP(*(.section_signature))
    } >SIGNATURE

    .section_flag :
    {
        FILL(0)
        KEEP(*(.section_flag))
    } >SIGNATURE
}
