#ifndef MEMORY_MAP
#define MEMORY_MAP
/****************** Timer *******************/
    #define TIMER        *((volatile unsigned int *) 0x80000000)
    
/****************** GPIO ********************/
    #define GPIO_0       *((volatile unsigned int *) 0x40000000)
    #define GPIO_1       *((volatile unsigned int *) 0x40000004)

/***** Matrix Multiplication Accelerator ****/
    #define FMRS 			8
    #define FMCS 			8
    #define SMCS 			8

    #define ADR_CONTROL_BASE 	0xC0000000
    #define ADR_FM_BASE 		0xC0000400
    #define ADR_SM_BASE 		0xC0000800
    #define ADR_TM_BASE 		0xC0000C00
    #define ADR_FINISHED 		0xC0001000

    #define DATA_START          0x01080808
    #define DATA_STOP           0x00080808

    #define ACC_RW(x)   (*(volatile unsigned int *)(x))

    #define FM(r, c) ((volatile int *)ADR_FM_BASE)[(r)*FMCS + (c)]
    #define SM(r, c) ((volatile int *)ADR_SM_BASE)[(r)*SMCS + (c)]
    #define TM(r, c) ((volatile int *)ADR_TM_BASE)[(r)*SMCS + (c)]

#endif // MEMORY_MAP
