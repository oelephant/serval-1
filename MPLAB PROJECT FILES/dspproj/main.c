#include <xc.h>
#include <libpic30.h>
#include "config.h"

int main() {
    
    TRISBbits.TRISB7 = 0;

    while(1) {
        LATBbits.LATB7 = ~PORTBbits.RB7;
        __delay32(1500000);
    }
}