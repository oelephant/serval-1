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
    uint16_t x;
    uint8_t i = 0;
    touchData t;
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    spi_init();
    spi_open(GRAPHIC);
    spi_open(TOUCH);
    glcd_init();

    glcd_writeVram(0, 0x0, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
    glcd_writeVram((uint32_t)GLCD_WIDTH*50, 0x4, GLCD_WIDTH);
    glcd_writeVram((uint32_t)GLCD_WIDTH*100, 0x2, GLCD_WIDTH);
    glcd_writeVram((uint32_t)GLCD_WIDTH*300, 0x3, GLCD_WIDTH);
 

    while(1)
    {
	//glcd_readLut1(0, 0xFF);
	//glcd_readVram(0, 0xFF);
        if (spi_int_toc == 1){
            t = glcd_getTouch();
	    glcd_writeVram((uint32_t)GLCD_WIDTH*t.y + t.x, i, 5);
	    i++;
	    i %= 5;
        }
    }
}
