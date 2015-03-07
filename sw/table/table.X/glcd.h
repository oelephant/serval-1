/* 
 * File:   glcd.h
 * Author: Tae
 *
 * Created on March 13, 2015, 3:55 PM
 */

#ifndef GLCD_H
#define	GLCD_H

#include <stdint.h>

void glcd_config_registers(void);
void glcd_init(void);
void glcd_open();
void glcd_write(uint8_t val);
void glcd_write_config(uint8_t addr_offset, uint16_t val);

#endif	/* GLCD_H */

