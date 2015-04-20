/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_AND_OR

#include "include.h"

#include "user.h"          /* User funct/params, such as InitApp              */
#include "glcd.h"
#include "lut_colors.h"
#include "menu.h"
#include "screen.h"
#include "spi_table.h"
#include "uc_pins.h"
#include "wifi.h"



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
    menu_init();

    screen_drawBackground();
    screen_draw(HOME, -1);

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
