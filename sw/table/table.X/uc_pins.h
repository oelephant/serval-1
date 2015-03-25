/*
 * File:   uc_pins.h
 * Author: Tae
 *
 * Created on March 25, 2015, 3:11 PM
 */

#ifndef UC_PINS_H
#define	UC_PINS_H

#define spi_int_toc_dir TRISBbits.TRISB7
#define spi_int_toc PORTBbits.RB7

#define spi_ss_lcd_dir TRISDbits.TRISD9
#define spi_ss_lcd LATDbits.LATD9

#define spi_ss_mem_dir TRISEbits.TRISE7
#define spi_ss_mem LATEbits.LATE7

#define spi_ss_toc_dir TRISBbits.TRISB6
#define spi_ss_toc LATBbits.LATB6

#define spi_miso_dir TRISDbits.TRISD0

#endif	/* UC_PINS_H */

