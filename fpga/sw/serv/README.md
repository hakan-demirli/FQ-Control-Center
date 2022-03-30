//#pragma GCC optimize("Os,no-unroll-loops,no-peel-loops")
/*
    riscv64-unknown-elf-gcc matmul_bench_default.c utility.c -o matmul_bench_default.elf -nostdlib -march=rv32i -mabi=ilp32 -T ../linkc.ld -Os -ffunction-sections 
    riscv64-unknown-elf-objcopy -O binary matmul_bench_default.elf matmul_bench_default.bin
    python ../py_tools/subservient_makehex.py matmul_bench_default.bin 1024 > matmul_bench_default_subservient.hex
    python ../py_tools/makehex.py matmul_bench_default.bin 256 > matmul_bench_default_servant.hex
    riscv64-unknown-elf-objdump -D matmul_bench_default.elf > elfdump.txt

    

riscv64-unknown-elf-g++ -S sucuk.cpp -o matmul_bench_default.elf -nostdlib -march=rv32i -mabi=ilp32 -T ./linkc.ld -Os -ffunction-sections   
riscv64-unknown-elf-objdump --disassemble-all matmul_bench_default.elf 

*/