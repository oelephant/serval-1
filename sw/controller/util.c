#include <xc.h>

#define _XTAL_FREQ 4000000

unsigned int Utils_charsToInt(unsigned char c0, unsigned char c1)
{
    unsigned int newInt = c0;
    newInt <<= 8;
    return newInt |= c1;
}

void Utils_intToChars(unsigned int i, unsigned char *c0, unsigned char *c1)
{
    unsigned char newChar = i >> 8;
    *c0 = newChar;
    newChar = i & 0x00FF;
    *c1 = newChar;
}

void Utils_longToChars(\
    unsigned long l, unsigned char *c0, unsigned char *c1,\
        unsigned char *c2, unsigned char *c3)
{
    unsigned char newChar = l >> 24;
    *c0 = newChar;
    newChar = (0x0F00 & l) >> 16;
    *c1 = newChar;
    newChar = (0x00F0 & l) >> 8;
    *c2 = newChar;
    newChar = (0x000F & l);
    *c3 = newChar;
}