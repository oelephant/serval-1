/* 
 * File:   io.h
 * Author: Coleby
 *
 * Created on December 3, 2014, 11:39 PM
 */

#ifndef IO_H
#define	IO_H

#define KEY_0 0xEB
#define KEY_1 0x77
#define KEY_2 0x7B
#define KEY_3 0x7D
#define KEY_4 0xB7
#define KEY_5 0xBB
#define KEY_6 0xBD
#define KEY_7 0xD7
#define KEY_8 0xDB
#define KEY_9 0xDD
#define KEY_A 0x7E
#define KEY_B 0xBE
#define KEY_C 0xDE
#define KEY_D 0xEE
#define KEY_STAR 0xE7
#define KEY_POUND 0xED

unsigned char getKey();
unsigned char debounce();

void Keys_clearLCD();
void LCD_init();
void LCD_printStr(unsigned char * str);

void Ethernet();
void Ethernet_clientMode();
void Ethernet_ipRaw();
void Ethernet_init();

void XBee_init();
void XBee_send(unsigned char *str);

#endif	/* IO_H */

