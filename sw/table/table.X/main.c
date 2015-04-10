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

#include <libpic30.h>   // for delays

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{
    int z = 0;
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

    glcd_writeVram(0, WHITE, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
    glcd_putBox(50, 50, GRAY, 200, 100);

    while(1)
    {
	//glcd_readVram(0, 0xFF);
        if (spi_int_toc == 1){
            t = glcd_getTouch();
	    if (t.x > 50 && t.x < 250 && t.y > 50 && t.y < 150){
		if (z==0){
		    glcd_putBox(50, 50, DARK_GRAY, 200, 100);
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
	    //if ((t.pen & 1) == 0){
		//glcd_writeVram((uint32_t)GLCD_WIDTH*i + 200, 0, 1);
		//glcd_writeVram((uint32_t)GLCD_WIDTH*t.y + t.x, 3, 3);
		//i++;
		//i %= 5;
	    //}

//	    if ((t.pen & 1) == 1){
//		if (z == 0){
//		    glcd_writeVram(0, 0x1, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*500, 0x3, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*400, 0x4, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*300, 0x2, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*200, 0x2, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*100, 0x3, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*50, 0x4, GLCD_WIDTH*10);
//		}
//		else{
//		    glcd_writeVram(0, 0x0, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*500, 0x1, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*400, 0x2, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*300, 0x3, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*200, 0x4, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*100, 0x5, GLCD_WIDTH*10);
//		    glcd_writeVram((uint32_t)GLCD_WIDTH*50, 0x6, GLCD_WIDTH*10);
//		}
//
//		z = ~z;
//	    }
        }
    }
}
