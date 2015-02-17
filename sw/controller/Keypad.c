/* 
 * File:   Keypad.c
 * Author: Coleby
 *
 * Created on November 4, 2014, 3:36 AM
 */
#include <xc.h>

/*
 * Data bus on port D
 * - D7 ... D4 = Row 1 ... Row 4
 * - D3 ... D0 = Col 1 ... Col 4
 */
#define ROW_COL LATD

/*
 * Keypad's latch LE tied to VCC, only need to enable output to use
 */
#define OE LATEbits.LATE2

/*
 * Debounce constant, how many reads before accepting key press
 */
#define DEBOUNCE_THRESHOLD 1000

/*
 * Class constants
 */
#define COL_1 0xF7
#define COL_2 0xFB
#define COL_3 0xFD
#define COL_4 0xFE

unsigned char Keys_scan(unsigned char col)
{
    unsigned char key;
    OE = 0;
    TRISD = 0x00;
    LATD = col;
    TRISD = 0xFF;
    key = PORTD;
    OE = 1;
    return key;
}

unsigned char Keys_getKey()
{
    unsigned char key;

    key = Keys_scan(COL_1);
    if ((key | 0x0F) != 0xFF) {
        return key;
    }

    key = Keys_scan(COL_2);
    if ((key | 0x0F) != 0xFF) {
        return key;
    }

    key = Keys_scan(COL_3);
    if ((key | 0x0F) != 0xFF) {
        return key;
    }

    key = Keys_scan(COL_4);
    if ((key | 0x0F) != 0xFF) {
        return key;
    }

    return 0x00;
}

unsigned char Keys_debounce()
{
    unsigned char key = 0x00;
    unsigned char old = 0x00;
    int i = 0;

    while (1) {

        old = key;
        while(!(key = Keys_getKey()));

        if (old == key) {
            if (i++ >= 100) {
                while(Keys_getKey());
                return key;
            }
        } else {
            i = 0;
        }
    }

    return 0x00;
}