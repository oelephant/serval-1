/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_AND_OR
/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#endif

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */
#include "glcd.h"
#include "spi_table.h"
#include "uc_pins.h"

#include <libpic30.h>   // for delays

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{
    uint32_t i;
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    spi_init();
    spi_open(GRAPHIC);
    spi_open(TOUCH);
    glcd_init();


    for (i = 0; i < 0x1FF; i++){
	glcd_writeVram(i, i, 1);
    }
    while(1)
    {
	glcd_readLut1(0, 0xFF);
	glcd_readVram(0, 0xFF);
        if (spi_int_toc == 1){
            glcd_getTouch();
        }
    }
}
