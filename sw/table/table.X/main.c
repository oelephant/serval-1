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
    int temp;
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
        glcd_writePixel(0x3FFFF, 176, 0, 0);
        glcd_writePixel(0x4FFFF, 138, 43, 226);
        //glcd_writePixel(0x4FFFF, 87, 112, 210);
        if (spi_int_toc == 1){
            glcd_getTouch();
        }
    }
}
