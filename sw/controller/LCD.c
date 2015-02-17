/*
 * File:   LCD.c
 * Author: Coleby
 *
 * Created on December 3, 2014, 11:35 PM
 */

#include <xc.h>
#define _XTAL_FREQ 4000000
#define rs LATEbits.LATE1
#define rw LATEbits.LATE0
#define en LATAbits.LATA5
#define data LATD

void LCD_sendCmd(unsigned char cmd);


void LCD_checkBusy()
{
	unsigned char busy;
        TRISD = 0x00;
	rs = 0;
	rw = 1;
	en = 1;
	data = 0x00;
	busy = data;
	while (busy & 0x80)
	{
		data = 0x00;
		busy = data;
	}

	en = 0;
        TRISD = 0xFF;
}

void LCD_printChar(unsigned char c)
{
    TRISD = 0x00;
    rs = 1;
    rw = 0;
    data = c;
    en = 1;
    __delay_ms(5);
    en = 0;
    TRISD = 0xFF;
}

void LCD_printStr(unsigned char * str)
{
    int i = 0;
    while (str[i] != 0) {
        LCD_printChar(str[i++]);
    }
}

void LCD_clear()
{
    LCD_sendCmd(0x01);
}

void LCD_init()
{
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISAbits.TRISA5 = 0;

    data = 0;
    rs = 0;
    __delay_ms(25);
    LCD_sendCmd(0x30); // function set, 8 bit two line 5x10
    __delay_ms(5);
    LCD_sendCmd(0x30); // function set, 8 bit two line 5x10
    __delay_ms(15);
    LCD_sendCmd(0x30); // display on, cursor on, blink on
    LCD_sendCmd(0x38); // display on, cursor on, blink on
    LCD_sendCmd(0x0C); // clear
    LCD_sendCmd(0x01); // entry mode, increment (don't shift)
    LCD_sendCmd(0x06); // entry mode, increment (don't shift)

    LCD_sendCmd(0x01);
}

void LCD_sendCmd(unsigned char cmd)
{
    TRISD = 0x00;
    rs = 0;
    rw = 0;
    data = cmd;
    en = 1;
    __delay_ms(5);
    en = 0;
    TRISD = 0xFF;
}