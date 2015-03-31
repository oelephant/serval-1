/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_AND_OR
/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#endif

#include <libpic30.h>   // for delays
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */
#include "glcd.h"
#include "spi_table.h"

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    spi_init();
    glcd_init();

    while(1)
    {
        //glcd_writePixel(0x5FFFF, 87, 112, 210);
        //glcd_writePixel(0x4FFFF, 87, 112, 210);
        spi_open(GRAPHIC);
        glcd_readGraphic(1);
        if (spi_int_toc == 1){
            spi_open(TOUCH);
            glcd_getTouch();
        }
    }
}
