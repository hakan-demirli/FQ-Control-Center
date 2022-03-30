#!/bin/bash
MYS='entry_point.cpp utility.c'

if [[ $1 == 'buildS' ]]
then
    riscv64-unknown-elf-gcc -S $MYS -o entry_point.S -nostdlib -march=rv32i -mabi=ilp32 -T ./linkc.ld -Os -ffunction-sections 
else 
    riscv64-unknown-elf-g++ $MYS -o entry_point.elf -nostdlib -march=rv32i -mabi=ilp32 -T ./linkc.ld -Os -ffunction-sections 
    riscv64-unknown-elf-objcopy -O binary entry_point.elf entry_point.bin
    python3 subservient_makehex.py entry_point.bin 1024 > entry_point.hex
    riscv64-unknown-elf-objdump -D entry_point.elf > entry_point_dump.txt
fi
