/* 
 * File:   glcd.h
 * Author: Tae
 *
 * Created on March 13, 2015, 3:55 PM
 */

#ifndef GLCD_H
#define	GLCD_H

#include <stdint.h>

#define spi_int_toc PORTBbits.RB7

void glcd_configRegisters(void);
int glcd_getTouch(void);
void glcd_init(void);
int glcd_readLut1(uint8_t offset, uint8_t length);
uint16_t glcd_readRegister(uint8_t offset);
int glcd_readRegisters(void);
int glcd_readVram(uint32_t addr, uint8_t length);
void glcd_writeLut1(uint8_t offset, uint8_t r, uint8_t g, uint8_t b);
void glcd_writeRegister(uint8_t offset, uint16_t val);
void glcd_writePixel(uint32_t addr, uint8_t r, uint8_t g, uint8_t b);
void glcd_writeVram(uint32_t addr, uint8_t lut_offset, uint32_t length);

#endif	/* GLCD_H */

