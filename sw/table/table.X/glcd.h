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
unsigned int glcd_readGraphic(uint32_t addr);
void glcd_writePixel(uint32_t addr, uint8_t r, uint8_t g, uint8_t b);
void glcd_writeConfig(uint8_t addr_offset, uint16_t val);

#endif	/* GLCD_H */

