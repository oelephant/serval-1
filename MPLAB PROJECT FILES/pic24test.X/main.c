#define FCY 500000UL

#include <xc.h>
#include <libpic30.h>
#include "config.h"

int main() {
    TRISBbits.TRISB7 = 0;

    while(1) {
        LATBbits.LATB7 = ~LATBbits.LATB7;
        __delay_ms(1000);
    }
}

