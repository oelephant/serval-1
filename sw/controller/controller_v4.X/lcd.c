#include "common.h"
#include <xc.h>
#include <libpic30.h>

#define LCD_D0 LATEbits.LATE1
#define LCD_D1 LATEbits.LATE2
#define LCD_D2 LATEbits.LATE3
#define LCD_D3 LATEbits.LATE4
#define LCD_D4 LATEbits.LATE5
#define LCD_D5 LATEbits.LATE6
#define LCD_D6 LATEbits.LATE7
#define LCD_D7 LATGbits.LATG6
#define LCD_RW LATBbits.LATB0
#define LCD_RS LATFbits.LATF1
#define LCD_E LATEbits.LATE0

void lcdSendCmd(unsigned char cmd);
void lcdInit();
void lcdClear();
void lcdPrintStr(char * str);
void lcdPrintChar(unsigned char c);
void lcdCheckBusy();
unsigned char lcdGetData();
void lcdPutData(char c);

void lcdPutData(char c)
{
    LCD_D0 = (c & 0x01);
    LCD_D1 = (c & 0x02) >> 1;
    LCD_D2 = (c & 0x04) >> 2;
    LCD_D3 = (c & 0x08) >> 3;
    LCD_D4 = (c & 0x10) >> 4;
    LCD_D5 = (c & 0x20) >> 5;
    LCD_D6 = (c & 0x40) >> 6;
    LCD_D7 = (c & 0x80) >> 7;
}

unsigned char lcdGetData()
{
    unsigned char c = 0x00;
    c |= LCD_D0;
    c |= (LCD_D1 << 1);
    c |= (LCD_D2 << 2);
    c |= (LCD_D3 << 3);
    c |= (LCD_D4 << 4);
    c |= (LCD_D5 << 5);
    c |= (LCD_D6 << 6);
    c |= (LCD_D7 << 7);

    return c;
}

void lcdCheckBusy()
{
	unsigned char busy;

	LCD_RS = 0;
	LCD_RW = 1;
	LCD_E = 1;
	lcdPutData(0x00);
	busy = lcdGetData();
	while (busy & 0x80)
	{
		lcdPutData(0x00);
		busy = lcdGetData();
	}

	LCD_E = 0;
}

void lcdPrintChar(unsigned char c)
{
    LCD_RS = 1;
    LCD_RW = 0;
    lcdPutData(c);
    LCD_E = 1;
    __delay_ms(5);
    LCD_E = 0;
}

void lcdPrintStr(char * str)
{
    int i = 0;
    while (str[i] != 0) {
        lcdPrintChar(str[i++]);
    }
}

void lcdClear()
{
    lcdSendCmd(0x01);
}

void lcdInit()
{
    TRISE = 0;
    TRISFbits.TRISF1 = 0;
    TRISBbits.TRISB0 = 0;
    TRISGbits.TRISG6 = 0;

    ANSB = 0;

    lcdPutData(0x00);
    LCD_RS = 0;
    __delay_ms(25);
    lcdSendCmd(0x30); // function set, 8 bit two line 5x10
    __delay_ms(5);
    lcdSendCmd(0x30); // function set, 8 bit two line 5x10
    __delay_ms(15);
    lcdSendCmd(0x30); // display on, cursor on, blink on
    lcdSendCmd(0x38); // display on, cursor on, blink on
    lcdSendCmd(0x0C); // clear
    lcdSendCmd(0x01); // entry mode, increment (don't shift)
    lcdSendCmd(0x06); // entry mode, increment (don't shift)

    lcdSendCmd(0x01);

}

void lcdSendCmd(unsigned char cmd)
{
    LCD_RS = 0;
    LCD_RW = 0;
    lcdPutData(cmd);
    LCD_E = 1;
    __delay_ms(5);
    LCD_E = 0;
};
