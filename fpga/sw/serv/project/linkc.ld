OUTPUT_ARCH( "riscv" )
MEMORY {
   r_mem        (RX)    : ORIGIN = 0x00000000, LENGTH = 0x000007AF
   dma_mem (RX)    : ORIGIN = 0x000007C0, LENGTH = 0x00000050
}
SECTIONS
{
    . = 0;
    
    .entry_point :
    {
        . = ALIGN(4);
        KEEP(*(.entry_point));
        . = ALIGN(4);
    } > r_mem
    
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.text));
        . = ALIGN(4);
    } > r_mem

    .data :
    {
        . = ALIGN(4);
        KEEP(*(.data));
        . = ALIGN(4);
    } > r_mem

    .bss :
    {
        . = ALIGN(4);
        KEEP(*(.bss));
        . = ALIGN(4);
    } > r_mem

    .sbss :
    {
        . = ALIGN(4);
        KEEP(*(.sbss));
        . = ALIGN(4);
    } > r_mem

    .dma :
    {
        . = ALIGN(4);
        KEEP(*(.dma));
        . = ALIGN(4);
    } > dma_mem
}