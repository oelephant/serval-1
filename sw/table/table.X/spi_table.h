/* 
 * File:   spi_table.h
 * Author: Tae
 *
 * Created on March 25, 2015, 3:49 PM
 */

#ifndef SPI_TABLE_H
#define	SPI_TABLE_H

#include <stdint.h>

uint8_t spi_exchange(uint8_t dataOut);
void spi_init(void);
void spi_open(unsigned int mode);

#endif	/* SPI_TABLE_H */

