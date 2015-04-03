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
    //spi_miso_dir = 1;       // input
    spi_ss_mem_dir = 0; // output
    spi_ss_toc_dir = 0; // output
    spi_ss_lcd_dir = 0; // output

    spi_ss_lcd = 1; // disable
    spi_ss_mem = 1; // disable
    spi_ss_toc = 1; // disable

    TRISDbits.TRISD0 = 1;
    TRISCbits.TRISC14 = 1;
    CM3CON = 0;

    /* Initialize peripherals */
    /* SPI1 for graphic controller */
    iPPSInput(IN_FN_PPS_SDI1, IN_PIN_PPS_RP16);      // remap SDI1 to spi1_miso
    iPPSOutput(OUT_PIN_PPS_RP12, OUT_FN_PPS_SDO1);      // remap SDO1 to spi1_mosi
    iPPSOutput(OUT_PIN_PPS_RP3, OUT_FN_PPS_SCK1OUT);    // remap SCK1 to spi1_sclk
    /* SPI2 for WiFi, touch controller */
    iPPSInput(IN_FN_PPS_SDI2, IN_PIN_PPS_RP20);      // remap SDI2 to spi2_miso
    iPPSOutput(OUT_PIN_PPS_RP25, OUT_FN_PPS_SDO2);      // remap SDO2 to spi2_mosi
    iPPSOutput(OUT_PIN_PPS_RP22, OUT_FN_PPS_SCK2OUT);    // remap SCK2 to spi2_sclk
}

