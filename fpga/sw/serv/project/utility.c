#include "utility.h"
#include "memory_map.h"

void failed(){
    /* Stop the verilog testbench by setting the first MSB of the GPIO_1*/
    SHARED_MEM[0] = 0b10000000;
    while(1){}
}

void passed(){
    /* Stop the verilog testbench by setting the second MSB of the GPIO_1*/
    SHARED_MEM[0] = 0b00001000;
    while(1){}
}

volatile unsigned int p_time;
void tic(){
    p_time = TIMER;
}

unsigned int toc(){
    return (TIMER - p_time);
}