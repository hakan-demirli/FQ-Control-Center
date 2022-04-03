#!/bin/bash
MYS='entry_point.cpp utility.c'

if [[ $1 == 'buildS' ]]
then
    riscv64-unknown-elf-gcc -S $MYS -o entry_point.S -nostdlib -march=rv32i -mabi=ilp32 -T ./linkc.ld -Os -ffunction-sections 
else 
    riscv64-unknown-elf-g++ $MYS -o entry_point.elf -nostdlib -march=rv32i -mabi=ilp32 -T ./linkc.ld -O0 -ffunction-sections 
    riscv64-unknown-elf-objcopy -O binary entry_point.elf entry_point.bin
    riscv64-unknown-elf-objdump -D entry_point.elf > entry_point_dump.txt
    python3 subservient_makehex.py entry_point.bin 2048 > entry_point.hex
    #cp ./entry_point.hex /mnt/second/STORAGE/projects/repositories/innovate_fpga_contest/vm_shared_folder/vmware_code/qt/fpga_control_center/fpga/hw/quartus/ip/serv_uart/modelsim
fi
