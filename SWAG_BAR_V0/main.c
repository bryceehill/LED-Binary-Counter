#include <msp430.h> 
#include "serial_handler.h"
#include "main.h"
#include <stdint.h>

//#include "uart.h"


/**
 * main.c
 * 1.0 -
 * 1.1 - RX0
 * 1.2 - TX0
 * 1.3 - SW1
 * 1.4 -
 * 1.5 -
 * 1.6 -
 * 1.7 -
 * 2.0 - L8
 * 2.1 - L9
 * 2.2 -
 * 2.3 -
 * 2.4 -
 * 2.5 -
 * 2.6 -
 * 2.7 -
 * 3.0 - L0
 * 3.1 - L1
 * 3.2 - L2
 * 3.3 - L3
 * 3.4 - L4
 * 3.5 - L5
 * 3.6 - L6
 * 3.7 - L7
 *
 */

char gridArray[5] = {0x7F, 0x04, 0x18, 0x04, 0x7F};
char currChar = ' ';
int isrCount = 0,ledCount = 0, ledState = 1, countDir = 1;
unsigned int swCount = 0, swBeg=0, uartCount = 0;;

int main(void){
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    led_init();
    switch_init();
    timer_ISR_init();
    uart_init(4); // 9600 baud rate = 4 input

    BCSCTL1 = CALBC1_16MHZ;      // Set range
    DCOCTL = CALDCO_16MHZ;      // SMCLK = DCO = 1MHz
    uint8_t c;
    int replyVal=0;
    while(1){
        if(eos_flag){
            replyVal = serial_handler (rx_data_str , 0, rx_flag, tx_data_str);
            if (replyVal)
                uart_write_string(0,replyVal);
            rx_flag = 0;
            eos_flag = 0;
            // Make the timer and count downa nd change the state
            if (ledState<2)
                ledState += 2;
            uartCount = 0;
        }


    }
}



int serial_handler ( char * stringIn, int uartdir,int strLen,char * stringOut){
    // Parse for the initial string and the end string
    volatile int strBeg, strEnd,i,k,byteNum,byteRet,shiftNum,dispShift;
    volatile char tempChar;
    unsigned int dispBits = 0;
    unsigned long convVal = 0, multVal;
    char cmpStr[2],byteOut[5],byteChar[5];
    cmpStr[0]='%';
    cmpStr[1]='*';
    strBeg = strComp(stringIn,cmpStr,strLen+1,2);
    if (strBeg == 0xFFFF)
        return;
    cmpStr[0]=13;
    strEnd = strComp(stringIn,cmpStr,strLen+2,1);
    if (strEnd == 0xFFFF)
        return;
    if (stringIn[strBeg+2] == 'b'){  // binary value entered
        for (k=0;k<(strLen-strBeg-1);k++){
            if (stringIn[strEnd-k-1]=='1')
                dispBits |= (1<<k);


        }
        dispBits = swapMsb(dispBits);
        P3OUT = ((dispBits)>>6)&0xFF;
        P2OUT &=~ (BIT0+BIT1);
        dispBits =((dispBits&0xC000)>>14);
        P2OUT |= dispBits;
        return 0;
    }
    if (stringIn[strBeg+2] == 'o'){  // octal value entered
        for (k=0;k<(strLen-strBeg-4);k++){
            convVal = stringIn[strEnd-1-k] - '0';
            dispBits |= convVal<<(k*3);
        }

        dispBits = swapMsb(dispBits);
        P3OUT = ((dispBits)>>6)&0xFF;
        P2OUT &=~ (BIT0+BIT1);
        dispBits =((dispBits&0xC000)>>14);
        P2OUT |= dispBits;
    }
    if (stringIn[strBeg+2] == 'd'){  // decimal value entered
        multVal = 1;
        for (k=0;k<(strLen-strBeg-4);k++){
            convVal = stringIn[strEnd-1-k] - '0';
            dispBits += convVal*(multVal);
            multVal*=10;
        }

        dispBits = swapMsb(dispBits);
        P3OUT = ((dispBits)>>6)&0xFF;
        P2OUT &=~ (BIT0+BIT1);
        dispBits =((dispBits&0xC000)>>14);
        P2OUT |= dispBits;
        return 0;
    }
    if (stringIn[strBeg+2] == 'x'){  // Hex value entered
        for (k=0;k<(strLen-strBeg-4);k++){
            if (stringIn[strEnd-1-k]<='9'){
                convVal = stringIn[strEnd-1-k] - '0';
            }
            else if(stringIn[strEnd-1-k]<='F'){
                convVal = stringIn[strEnd-1-k] - 'A'+10;
            }
            else if(stringIn[strEnd-1-k]<='4'){
                convVal = stringIn[strEnd-1-k] - 'a'+10;
            }

            dispBits += convVal<<(k*4);
        }

        dispBits = swapMsb(dispBits);
        P3OUT = ((dispBits)>>6)&0xFF;
        P2OUT &=~ (BIT0+BIT1);
        dispBits =((dispBits&0xC000)>>14);
        P2OUT |= dispBits;
    }
    return 0;
}

void convNibble(char * stringIn,char * byteOut, int num2Conv){
    // blindly converts two nibbles embedded in two bytes to a byte raw format
    int k;
    //    volatile char C1, C2;
    //    volatile int tempVal=0;
    for(k = 0;k<num2Conv;k++){
        //        C1=stringIn[k*2];
        //        C2 = stringIn[k*2+1];

        //        tempVal = (stringIn[k*2]-0x30)*10 + (stringIn[k*2+1]-0x30);
        byteOut[k] = (stringIn[k*2]-0x30)*10 + (stringIn[k*2+1]-0x30);
    }
}

void num2Char2Digit(int valIn,char * stringIn){
    //convert to a 2 digit char string.  Only 2 digits
    int tempVal=0;
    //    if (valIn<10){
    //        stringIn[0]='0';
    //        stringIn[1]=0x30+valIn;
    //    }
    tempVal = valIn/10;
    stringIn[0]=tempVal+0x30;
    valIn -= 10*tempVal;
    stringIn[1] = valIn+0x30;

}

int twoCharConv (char * stringIn){
    // simply and blindly creates a two digit int from two digit char
    return (stringIn[0]-0x30)*10+(stringIn[1]-0x30);
}

int strComp (char * stringIn, char * strPattern, int stringLen, int patternLen){
    // returns a value of where in the string the first occurance happens
    int i,k,diffVal;
    for (k=0;k<stringLen-patternLen;k++){
        diffVal = 0;
        for (i = 0;i<patternLen; i++){
            if (stringIn[k+i] != strPattern[i])
                diffVal++;
        }
        if (diffVal == 0)
            return k;
    }
    return 0xFFFF;  // no pattern detected

}





void led_init(void){
    P2DIR |= (BIT0+BIT1);   // Annodes as outputs  (Active Low)
    P2OUT |= (BIT0+BIT1);        // Annodes all off
    // PWM pin on (for brightness) On
    P3DIR |= (BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7);  // Cathodes all as outputs
    P3OUT |= (BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7);  // Cathodes all low (Active high)

}

void timer_ISR_init(void){
    TA1CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TA1CCR0 = 0;
    TA1CTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

    _BIS_SR( GIE);                 // Enter LPM0 w/ interrupt
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    if (P1IFG & BIT3){                   // RX has ensued
        if (P1IES & BIT3){
            swBeg = swCount;
        }
        else{
            if ((swCount-swBeg)>10000){
                //long press
                countDir*=-1;
                ledState = 1;
            }
            else {  // short press
                ledState ^= 1;
            }
        }
        P1IES ^= (BIT3);
        P1IFG &=~ BIT3;
    }
}
// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1 (void)
{
    char outstr[] = "Decimal Value:     ";
    char tempstr[7];
    int tempVal, k;
    isrCount++;
    swCount++;
    if (ledState >1){
    uartCount++;
    if (uartCount>60000)
        ledState-=2;
    }

    if (ledState==1){
        if (isrCount>4000){
            ledCount+=countDir;
            tempVal = swapMsb(ledCount);
            P3OUT = (tempVal>>6)&0xFF;
            P2OUT &=~ (BIT0+BIT1);
            tempVal =((tempVal&0xC000)>>14);
            P2OUT |= tempVal;
            isrCount=0;
            num2str(ledCount&(0x03FF), tempstr);
            outstr[15]=tempstr[2];
            outstr[16]=tempstr[3];
            outstr[17]=tempstr[4];
            outstr[18]=tempstr[5];
            for (k = 0;k<20;k++){
                tx_data_str[k]=outstr[k];
            }
            uart_write_fast_string(0, 19);
        }
    }

    TA1CCR0 +=1000;                            // Add Offset to CCR0
}

int swapMsb(int valin){
    int k;
    long outVal = 0;
    for(k=0;k<16;k++){
        if (valin&(1<<k))
            outVal |= (0x8000>>k);

    }
    return outVal;
}


void switch_init(void){
    P1DIR &=~ (BIT3);  // INputs for switches
    P1OUT |= (BIT3);   // Pull-UP for switches
    P1REN |= (BIT3);   // Pulling resistors for switches
    P1IE |=(BIT3);   // Hardware interrupt P1 switches
    P1IES |=(BIT3);   // High to low edge trigger
    P1IFG &= ~ (BIT3);
    _BIS_SR( GIE);                 // Global interrupt enable
}


void num2str(int val, char * outStr){
    volatile int temp,prev;
    unsigned int divider=10000;
    volatile int n=1,z=0,neg=0;
    outStr[0]='0';
    if (val<0){
        neg=1;
        val=val*(-1);
        outStr[0]='-';
    }
    prev=0;
    for(n=1;n<6;n++){
        temp=(val-prev)/divider;


        outStr[n]=temp+0x30;
        prev=prev+(temp*divider);


        divider=divider/10;
    }

}
