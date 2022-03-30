OUTPUT_ARCH( "riscv" )
SECTIONS
{
    . = 0;
    .entry_point : {*(.entry_point*);}
    .text :{*(.text*);}
    .data : { *(.data) }
}