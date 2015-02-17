#include <stdio.h>
#include "config.h"
#include "io.h"

#define _XTAL_FREQ 4000000

void delayMs(int x);

void init()
{
    LCD_init();
    Ethernet_init();
    XBee_init();
}


void main()
{
    unsigned char key = 0x00;
    unsigned char msg[20];

    //OSCCONbits.IRCF = 0x0D; 4MHz?

    init();
    
    while(1) {
        LCD_clear();
        LCD_printStr(" (A) or 0xAA to print  ");

        key = Keys_debounce();
        if (key == 0x7E) {
            XBee_send("(A) pushed");
            XBee_send("    printing...");
            Ethernet_clientMode();
        } else if (key == 0xBE) {
            XBee_send("(B) pushed");
        } else if (key == 0xDE) {
            XBee_send("(C) pushed");
        } else if (key == 0xEE) {
            XBee_send("(D) pushed");
        } else if (key == 0x7D) {
            XBee_send("(3) pushed");
        } else if (key == 0xBD) {
            XBee_send("(6) pushed");
        } else if (key == 0xDD) {
            XBee_send("(9) pushed");
        } else if (key == 0xED) {
            XBee_send("(#) pushed");
        } else if (key == 0x7B) {
            XBee_send("(2) pushed");
        } else if (key == 0xBB) {
            XBee_send("(5) pushed");
        } else if (key == 0xDB) {
            XBee_send("(8) pushed");
        } else if (key == 0xEB) {
            XBee_send("(0) pushed");
        } else if (key == 0x77) {
            XBee_send("(1) pushed");
        } else if (key == 0xB7) {
            XBee_send("(4) pushed");
        } else if (key == 0xD7) {
            XBee_send("(7) pushed");
        } else if (key == 0xE7) {
            XBee_send("(*) pushed");
        }
        
        sprintf(msg, "%X... ", key);
        LCD_printStr(msg);

        delayMs(1000);
    }
}

void delayMs(int x)
{
    int i;

    for (x ;x>0;x--)
    {
        for (i=0;i<=110;i++);
    }
}
