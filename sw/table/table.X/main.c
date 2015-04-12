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
#include "spi_table.h"
#include "uc_pins.h"
#include "wifi.h"

#include <libpic30.h>   // for delays

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{
    int z = 0;
    touchData t;
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    __delay_ms(100);
    spi_init();
    glcd_init();

    glcd_writeVram(0, WHITE, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
    glcd_putBox(50, 50, GRAY, 200, 100);

    while(1)
    {
        if (spi_int_toc == 1){
            t = glcd_getTouch();
	    if (t.x > 50 && t.x < 250 && t.y > 50 && t.y < 150){
		if (z==0){
		    glcd_putBox(50, 50, MAGENTA, 200, 100);
		    z = 1;
		}
		if ((t.pen&1) == 0){
		    glcd_writeVram(0, WHITE, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
		    glcd_putBox(50, 50, GRAY, 200, 100);
		}
	    }
	    else{
		if (z==1){
		    glcd_putBox(50, 50, GRAY, 200, 100);
		    z = 0;
		}
		glcd_putPixel(t.x, t.y, MAGENTA, 3);
	    }
        }
	else if (spi_int_wifi == 0){
	    wifi_read();
	}
	else{
	    //wifi_command();
	}
    }
}
