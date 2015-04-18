/* 
 * File:   glcd.h
 * Author: Tae
 *
 * Created on March 13, 2015, 3:55 PM
 */

#ifndef GLCD_H
#define	GLCD_H

#include "include.h"

struct TouchData {uint8_t pen; uint16_t x; uint16_t y;};

#define spi_int_toc PORTBbits.RB7
#define GLCD_WIDTH 640
#define GLCD_HEIGHT 480

void glcd_configRegisters(void);
struct TouchData glcd_getTouch(void);
void glcd_init(void);
void glcd_initLut1();
void glcd_initLutColor(uint8_t r, uint8_t g, uint8_t b);
void glcd_putBox(uint16_t x, uint16_t y, uint8_t color, uint16_t width, uint16_t height);
uint8_t glcd_putChar(uint16_t x, uint16_t y, uint8_t color, char c);
void glcd_putPixel(uint16_t x, uint16_t y, uint8_t color, uint32_t length);
void glcd_putString(uint16_t x, uint16_t y, uint8_t color, char *c);
int glcd_readLut1(uint8_t offset, uint8_t length);
uint16_t glcd_readRegister(uint8_t offset);
int glcd_readRegisters(void);
int glcd_readVram(uint32_t addr, uint8_t length);
void glcd_writeRegister(uint8_t offset, uint16_t val);
void glcd_writeVram(uint32_t addr, uint8_t lut_offset, uint32_t length);
#endif	/* GLCD_H */

