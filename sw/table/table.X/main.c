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
#include "lut_colors.h"
#include "screen.h"
#include "spi_table.h"
#include "uc_pins.h"
#include "wifi.h"

#include <libpic30.h>   // for delays

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    __delay_ms(100);
    spi_init();
    glcd_init();

    screen_drawBackground();
    screen_drawHome();

    while(1){
        if (spi_int_toc == 1){
	    screen_handleTouch();
        }
	else if (spi_int_wifi == 0){
	    wifi_read();
	}
	else{
	    //wifi_command();
	}
    }
}
