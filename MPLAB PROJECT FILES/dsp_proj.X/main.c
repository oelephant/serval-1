#define FCY 8000000
#include <libpic30.h>
#include "config.h"
#include "notes.h"


int main(void)
{
    // digital pins
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB12 = 1;

    // analog pins
    initAdcl();

    LATBbits.LATB7 = 0;
    //LATBbits.LATB10 = 1;

    while(1) {
        if (debounce()) {
        }
        AD1CON1bits.SAMP = 1;
        __delay_us(10);
        AD1CON1bits.SAMP = 0;
        while (!AD1CON1bits.DONE);
        blink(ADC1BUF1);
        __delay_ms(10);
    }
    return 0;
}

void blink(int val)
{
    LATBbits.LATB7 = ~PORTBbits.RB7;
    __delay_ms(val);
}

void initAdcl(void)
{
    ANSELA = ANSELB = 0x0000;
    ANSELAbits.ANSA0 = 1;

    AD1CON1 = 0x0000;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x000F;
    AD1CON4 = 0x0000;
    AD1CHS0 = 0x0005;
    AD1CHS123 = 0x0000;
    AD1CSSH = 0x0000;
    AD1CSSL = 0x0000;
    AD1CON1bits.ADON = 1;
    __delay_us(20);
}

int debounce(void)
{
    int valid = 0;
    int pressed = 0;

    while (PORTBbits.RB12 == 1) {
        if (++pressed > 10) {
            valid = 1;
            LATBbits.LATB7 = 1;
            break;
        }
        __delay_ms(1);
    }

    while (PORTBbits.RB12 == 1) {
        test(C7, 100);
        //test(A5, qn);
        //test(G5, qn);
        //test(A5, qn);
    }

    LATBbits.LATB7 = 0;
    return valid;
}

void test(int freq_hz, int duration_ms)
{
    long period;
    long cycles;
    int w;

    period = (1000000 / freq_hz) / 2;
    cycles = duration_ms * (freq_hz / 4000.00);

    for (w = 0; w <= cycles; w++) {
        LATBbits.LATB10 = 1;
        __delay_us(period);
        LATBbits.LATB10 = 0;
        __delay_us(period);
    }

    //LATBbits.LATB10 = 0;
}