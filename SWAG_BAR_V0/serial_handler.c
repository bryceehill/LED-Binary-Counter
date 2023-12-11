/*
 * uart_control.c
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */
#include  "msp430.h"
#include "serial_handler.h"
//#define uart_max 100
#define i2c_max  12

unsigned char tx_data_str[uart_max], rx_data_str[uart_max], rx_flag=0, dec_str[7], eos_flag=0;
char dec_char[6];
int tx_ptr,e_tx_ptr;




void uart_init(int br){
    volatile int temp=0;
    //Set baud rate to 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
    // use index of 0 1 2 3... corresponding to the rates above
    volatile unsigned int brvec[]={0x55, 0x15, 0x0B, 0x05, 0x83, 0x41, 0xA1, 0x16, 139, 69, 35, 17};
    volatile unsigned int hrvec[]={0xD0,0x34,0x1A,0x0D,6,3,1,1,0,0,0,0};
    BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
    DCOCTL = CALDCO_16MHZ;
    P1SEL |= (BIT1+BIT2);                             // P3.4,5 = USCI_A0 TXD/RXD
    P1SEL2 |= (BIT1+BIT2);
    //	UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = brvec[br];
    UCA0BR1 = hrvec[br];
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    __bis_SR_register(GIE);       			// interrupts enabled
}

void uart_write_string(int vals, int vale){
    int i;									// writes a string from global variable tx_data_str.  vals is starting pointer and vale is the ending value
    for(i=vals;i<vale;i++){
        while (!(IFG2&UCA0TXIFG));
        UCA0TXBUF=tx_data_str[i];
    }
    while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF='\n';
    while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF='\r';
}

void uart_write_fast_string(int vals, int vale){
    tx_ptr=vals;								// writes a string from global variable tx_data_str.  vals is starting pointer and vale is the ending value
    e_tx_ptr=vale;								//  Uses interrupts to send out bytes
    UCA0TXBUF=tx_data_str[tx_ptr];
    IE2 |= UCA0TXIE;

}


#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
    if (IE2&UCA0TXIE){									//portion of uart_write_fast_string
        tx_ptr++;
        if (tx_ptr<e_tx_ptr)
            UCA0TXBUF=tx_data_str[tx_ptr];
        else if (tx_ptr==e_tx_ptr){
            UCA0TXBUF='\n';
        }
        else{
            UCA0TXBUF='\r';
            IE2 &=~ UCA0TXIE;
        }
    }
    else{



    }



}

//  Place data in RX-buffer and set flag
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    volatile char temp;
    if(IFG2 & UCA0RXIFG){							// Receive data on UART
        rx_data_str[rx_flag]=UCA0RXBUF;				// data is stored in rx_data_str
        temp=rx_data_str[rx_flag];
        while (!(IFG2&UCA0TXIFG));
        //	UCA0TXBUF=temp;
        if (rx_data_str[rx_flag]=='\r')				// new line or carriage return set eos_flag global variable
            eos_flag=1;
        if (rx_data_str[rx_flag]=='\n')
            eos_flag=1;
        rx_flag++;
        if (rx_flag>uart_max){							// maximum of characters starts at the beginning again
            rx_flag=1;
        }
    }
}

char uart_get_char(int num){
    return rx_data_str[num];
}
void uart_set_char(char tx_data,int num){
    tx_data_str[num]=tx_data;
}


void conv_hex_dec(int val){
    volatile int temp,prev;
    unsigned int divider=10000;
    volatile int n=1,z=0,neg=0;
    dec_str[0]='0';
    if (val<0){
        neg=1;
        val=val*(-1);
        dec_str[0]='-';
    }
    prev=0;
    for(n=1;n<6;n++){
        temp=(val-prev)/divider;


        dec_str[n]=temp+0x30;
        prev=prev+(temp*divider);


        divider=divider/10;
    }

}

void unsigned_conv_hex_dec(int val){
    volatile unsigned int temp,prev;
    unsigned int divider=10000;
    volatile unsigned int n=1,z=0,neg=0;
    dec_str[0]='0';
    prev=0;
    for(n=1;n<6;n++){
        temp=(val-prev)/divider;


        dec_str[n]=temp+0x30;
        prev=prev+(temp*divider);


        divider=divider/10;
    }

}

int conv_dec_hex ( void ){
    volatile int num,k,temp;
    num=0;
    for (k=1;k<6;k++){
        num*=10;
        temp=dec_char[k];
        if (dec_char[k]>0x39)
            return 0x7FFF;
        if (dec_char[k]<0x30)
            return 0x7fff;
        num+=(dec_char[k]-0x30);
    }
    if (dec_char[0]=='-')
        num=num*(-1);
    return num;
}
