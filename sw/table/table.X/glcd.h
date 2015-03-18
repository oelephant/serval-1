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

void glcd_close(void);
void glcd_config_registers(void);
void glcd_getTouch(void);
void glcd_init(void);
void glcd_openGraphic();
void glcd_openTouch();
unsigned int glcd_readTouch(void);
void glcd_writeGraphic(uint8_t val);
void glcd_writePixel(uint32_t addr, uint8_t r, uint8_t g, uint8_t b);
void glcd_writeConfig(uint8_t addr_offset, uint16_t val);

#endif	/* GLCD_H */

