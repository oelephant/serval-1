/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#include <xc.h>

#include <PPS.h>
#include <stdint.h>          /* For uint32_t definition */
#include <stdbool.h>         /* For true/false definition */

#include "uc_pins.h"
#include "user.h"            /* variables/params used by user.c */

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    /* Setup analog functionality and port direction */
    ANSB = 0;   // digital io

    spi_int_toc_dir = 1;    // input
    spi_miso_dir = 1;       // input
    spi_ss_mem_dir = 0; // output
    spi_ss_toc_dir = 0; // output
    spi_ss_lcd_dir = 0; // output

    spi_ss_lcd = 1; // disable
    spi_ss_mem = 1; // disable
    spi_ss_toc = 1; // disable

    TRISDbits.TRISD0 = 1;
    /*
    TRISBbits.TRISB7 = 1;   // spi_int_toc as input
    LATEbits.LATE7 = 1;     // spi_ss_mem disabled
    TRISEbits.TRISE7 = 0;   // spi_ss_mem as output
    TRISDbits.TRISD0 = 1;   // spi_miso as input
*/
    /* Initialize peripherals */
    //iPPSInput(IN_FN_PPS_SDI1, IN_PIN_PPS_RP11);      // remap SDI1 to spi_miso
    iPPSOutput(OUT_PIN_PPS_RP12, OUT_FN_PPS_SDO1);      // remap SDO1 to spi_mosi
    iPPSOutput(OUT_PIN_PPS_RP3, OUT_FN_PPS_SCK1OUT);    // remap SCK1 to spi_sclk
}

