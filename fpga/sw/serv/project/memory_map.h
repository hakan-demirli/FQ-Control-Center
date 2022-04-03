#ifndef MEMORY_MAP
#define MEMORY_MAP
/****************** Timer *******************/
    #define TIMER        *((volatile unsigned int *) 0x80000000)
    
/*************** SHARED_MEM *****************/
    #define SHARED_MEM    ((volatile unsigned int *) 0x40000000)

/***** UART MEMORY MAP ****/
    #define PRESCALAR    *((volatile unsigned int *) 0xC0000000)
    #define RX_DHBR      *((volatile unsigned int *) 0xC0000004)
    #define TX_BUSY      *((volatile unsigned int *) 0xC0000008)
    #define TX_SEND      *((volatile unsigned int *) 0xC000000C)
    #define TX_DATA      *((volatile unsigned int *) 0xC0000010)
    #define RX_READY     *((volatile unsigned int *) 0xC0000014)
    #define RX_DATA      *((volatile unsigned int *) 0xC0000018)

#endif // MEMORY_MAP
