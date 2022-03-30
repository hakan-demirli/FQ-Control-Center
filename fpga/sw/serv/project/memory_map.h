#ifndef MEMORY_MAP
#define MEMORY_MAP
/****************** Timer *******************/
    #define TIMER        *((volatile unsigned int *) 0x80000000)
    
/****************** GPIO ********************/
    #define GPIO_0       *((volatile unsigned int *) 0x40000000)
    #define GPIO_1       *((volatile unsigned int *) 0x40000004)

/***** UART MEMORY MAP ****/
    #define PRESCALAR    *((volatile unsigned int *) 0xC0000000)
    #define UART_CFG     *((volatile unsigned int *) 0xC0000004)
    #define TX_BUSY      *((volatile unsigned int *) 0xC0000008)
    #define TX_SEND      *((volatile unsigned int *) 0xC000000C)
    #define TX_DATA      *((volatile unsigned int *) 0xC0000010)
    #define RX_READY     *((volatile unsigned int *) 0xC0000014)
    #define RX_DATA      *((volatile unsigned int *) 0xC0000018)

#endif // MEMORY_MAP
