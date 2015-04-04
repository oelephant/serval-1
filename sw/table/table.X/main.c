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
    uint32_t i = 0;
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    spi_init();
    glcd_init();

    spi_open(TOUCH);
    while(1)
    {
        //temp = glcd_readGraphic(0x2C);
        glcd_writePixel(i, 176, 0, 0);
        glcd_writePixel(i + 1, 138, 43, 226);
        i += 2;
        if (i > 0x5FFFF){
            i = 0;
        }
        //glcd_writePixel(0x4FFFF, 87, 112, 210);
        if (spi_int_toc == 1){
            glcd_getTouch();
        }
    }
}
