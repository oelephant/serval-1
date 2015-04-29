/*  main.c
    created by Ellen Fluehr
 */

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
#include <timer.h>
#include "uc_pins.h"
#include "wifi.h"

extern char wifi_resultPacket[0xff];
extern int wifi_resultLength;

void initTestOrder(void);
void testOrder(void);
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int main(void)
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

void initTestOrder(void){
   ConfigIntTimer23(T23_INT_ON|T23_INT_PRIOR_1);/*Enable Interrupt*/
   OpenTimer23(T23_ON,0x10000000);
   while (1){   }
   CloseTimer23();

}

void testOrder(void){
    wifi_reqID();
    while (spi_int_wifi == 1);
    wifi_read();
    wifi_sendOrder();
}


void __attribute__ ((interrupt,no_auto_psv)) _T23Interrupt (void)
{
    testOrder();
    T23_Clear_Intr_Status_Bit;
}
