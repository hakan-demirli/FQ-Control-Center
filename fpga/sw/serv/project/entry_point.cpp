#include "utility.h"
#include "memory_map.h"

#define BUFF_SIZE 13
#define MY_SIZE 13

void entry_point () __attribute__((section(".entry_point")));
void setup_uart();
void uart_send_array();
void uart_read_buff();
char i = 0; // uart array send current position, 0 if idle
char ii = 0; // uart buffer read current position, 0 is empty, MY_SIZE is full

const char pass[MY_SIZE] = "0123456789KO";
char buff[BUFF_SIZE] = {0};

char dma_mem[60] __attribute__((section(".dma"))) = {1};

void entry_point () {


    setup_uart();

    while(1){
        
        if(!TX_BUSY && !(i == MY_SIZE)) // keep sending until all sent
            uart_send_array();

        if(RX_READY) // if there is a data read it
            uart_read_buff();
        if((i == MY_SIZE)){
            passed();
        }
    }
}

inline void uart_read_buff(){

    for (int k =0; k<(BUFF_SIZE-1);k++){
        buff[k+1] = buff[k]; // shift all buffer like a fifo
    }
    buff[0] = RX_DATA;
    RX_DHBR = 1; // has to be accessed right after read operation
    // data package structure: "message"+'K'+'O'
    if ((buff[0] == 'O') && (buff[1] == 'K'))
        passed(); //we have a corrrect package
}

inline void uart_send_array(){

    if ((i>=0) && (i < MY_SIZE)){
        TX_DATA = pass[i];
        i++;
    }
}

inline void setup_uart(){
    //100MHz/115200Baudrate = 868. Clock generated by toggling 868/2 = 434. RX use oversampling by x2 434/2 = 217
    PRESCALAR = 217;
}