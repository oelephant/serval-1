#define FCY 8000000

#include <xc.h>
#include <libpic30.h>
#include "config.h"

#define lcdD0 LATEbits.LATE1
#define lcdD1 LATEbits.LATE2
#define lcdD2 LATEbits.LATE3
#define lcdD3 LATEbits.LATE4
#define lcdD4 LATEbits.LATE5
#define lcdD5 LATEbits.LATE6
#define lcdD6 LATEbits.LATE7
#define lcdD7 LATGbits.LATG6
#define lcdRW LATFbits.LATF1
#define lcdRS LATFbits.LATF0
#define lcdE LATEbits.LATE0

void init(void);
void putLCDData(char c);
void write(char i);
void command(char i);

int main(void) {

    //while(1);

    init();
    write('H');
    write('i');
    write('!');

    while(1);

    return (1);
}

void putLCDData(char c)
{
    lcdD0 = (c & 0x01);
    lcdD1 = (c & 0x02) >> 1;
    lcdD2 = (c & 0x04) >> 2;
    lcdD3 = (c & 0x08) >> 3;
    lcdD4 = (c & 0x10) >> 4;
    lcdD5 = (c & 0x20) >> 5;
    lcdD6 = (c & 0x40) >> 6;
    lcdD7 = (c & 0x80) >> 7;
}

//8-bit Initialization:
/**********************************************************/
void command(char i)
{
    putLCDData(i); //put data on output Port
    lcdRS = 0; //D/I=LOW : send instruction
    lcdRW = 0; //R/W=LOW : Write
    lcdE = 1;
    __delay_us(1); //enable pulse width >= 300ns
    lcdE = 0; //Clock enable: falling edge
}
/**********************************************************/
void write(char i)
{
    putLCDData(i); //put data on output Port
    lcdRS =1; //D/I=LOW : send data
    lcdRW =0; //R/W=LOW : Write
    lcdE = 1;
    __delay_us(1); //enable pulse width >= 300ns
    lcdE = 0; //Clock enable: falling edge
}
/**********************************************************/
void init()
{
    TRISE = 0;
    TRISFbits.TRISF0 = 0;
    TRISFbits.TRISF1 = 0;
    TRISGbits.TRISG6 = 0;

    lcdE = 0;
    __delay_ms(100); //Wait >15 msec after power is applied
    command(0x30); //command 0x30 = Wake up
    __delay_ms(30); //must wait 5ms, busy flag not available
    command(0x30); //command 0x30 = Wake up #2
    __delay_ms(10); //must wait 160us, busy flag not available
    command(0x30); //command 0x30 = Wake up #3
    __delay_ms(10); //must wait 160us, busy flag not available
    command(0x38); //Function set: 8-bit/2-line
    command(0x10); //Set cursor
    command(0x0c); //Display ON; Cursor ON
    command(0x06); //Entry mode set
}
/**********************************************************/