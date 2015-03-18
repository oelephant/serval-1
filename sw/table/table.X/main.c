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
#include <libpic30.h>   // for delays

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{
    //unsigned int * packet;
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    glcd_init();
    glcd_config_registers();
    ANSBbits.ANSB7 = 0;     // spi_int_toc as digital io
    TRISBbits.TRISB7 = 1;   // spi_int_toc as input
    TRISDbits.TRISD0 = 1;   // spi_miso as input

    //glcd_openGraphic();
    glcd_openTouch();
    while(1)
    {
        //glcd_writePixel(0x5FFFF, 87, 112, 210);
        //glcd_writePixel(0x4FFFF, 87, 112, 210);
        //glcd_readTouch();
        if (spi_int_toc == 1){
           glcd_readTouch();
            //glcd_openGraphic();
        }
    }
}
