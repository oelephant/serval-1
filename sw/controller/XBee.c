#include <xc.h>
#include <stdio.h>
#include "io.h"

#define _XTAL_FREQ 4000000
#define STRLEN 12

volatile unsigned char t;
volatile unsigned char rcindex;
volatile unsigned char rcbuf[STRLEN];

// RC6/PMA5/TX1
// RC7/PMA4/RX1
// RB5/PMA0... DTR
// RC1.. RTS
// RC2 ..CTS

volatile unsigned char data;

void UART_init()
{

    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    // Disable interrups temporarily
    INTCONbits.GIE = 0;

    // Set Baud Rate 9600
    BAUDCONbits.BRG16 = 0;
    TXSTAbits.BRGH = 0;
    BAUDCTLbits.ABDEN = 0;
    SPBRGH = 0;
    SPBRG = 12;


    // Enable Transmitter w/ intterups
    TXSTAbits.SYNC = 0;
    TXSTAbits.TXEN = 1;
    PIE1bits.TXIE = 0;

    // TRMT shows status of TSR, set when TSR is empty
    // TXIF check for TXREG, must delay after loading TXREG
    // load TXREG to transmit

    // Enable Receiver w/ interrupts
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;
    PIE1bits.RCIE = 1;
    PEIE = 1;
    
    // Enable Interrupts
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    // Enable receive
    RCSTAbits.CREN = 1;

    // RCIF set on reception, intterupt  generated
    // read RCREG
    // clear CREN (stays set if error)
}

void XBee_init()
{
    UART_init();
}

void UART_puts(unsigned char *str)
{
    int i = 0;
    while (str[i] != 0) {
        while(!TXSTAbits.TRMT);
        TXREG = str[i++];
    }
}

void XBee_send(unsigned char *str)
{
    UART_puts("\n");
    UART_puts(str);
}

void interrupt ISR()
{

    unsigned char foo;

    // when reception is complete
    if (PIR1bits.RCIF) {
        RCSTAbits.CREN = 0;
        data = RCREG;
        foo = RCREG;
        foo = RCREG;
        if (data == 0xAA) {
            Ethernet_clientMode();
        }
        RCSTAbits.CREN = 1;
        // RCIF cleared when RCREG is read
    }

    // once transmission completes and TXREG is empty
    if (PIR1bits.TXIF) {
        NOP();
        TXSTAbits.TXEN = 0;
    }
}