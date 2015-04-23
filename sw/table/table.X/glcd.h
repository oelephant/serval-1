/* 
 * File:   glcd.h
 * Author: Tae
 *
 * Created on March 13, 2015, 3:55 PM
 */

#ifndef GLCD_H
#define	GLCD_H

#include "include.h"

struct TouchData {char pen; int x; int y;};

#define spi_int_toc PORTBbits.RB7
#define GLCD_WIDTH 640
#define GLCD_HEIGHT 480

void glcd_configRegisters(void);
int glcd_configTouch(void);
struct TouchData glcd_getTouch(void);
void glcd_init(void);
void glcd_initLut1();
void glcd_initLutColor(char r, char g, char b);
void glcd_putBox(int x, int y, char color, int width, int height);
char glcd_putChar(int x, int y, char color, char c);
void glcd_putPixel(int x, int y, char color, long length);
void glcd_putString(int x, int y, char color, char *c);
int glcd_readLut1(char offset, char length);
int glcd_readRegister(char offset);
int glcd_readRegisters(void);
int glcd_readVram(long addr, char length);
char glcd_touchExchange(char value);
void glcd_writeRegister(char offset, int val);
void glcd_writeVram(long addr, char lut_offset, long length);
#endif	/* GLCD_H */

