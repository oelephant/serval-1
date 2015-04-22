// Ref: http://www.geeksforgeeks.org/convert-floating-point-number-string/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#include <xc.h>

#include <PPS.h>
#include <stdint.h>          /* For uint32_t definition */
#include <stdbool.h>         /* For true/false definition */
#include <stdio.h>
#include <math.h>

#include "uc_pins.h"
#include "user.h"            /* variables/params used by user.c */

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
    /* Setup analog functionality and port direction */
    ANSB = 0; // digital io

    spi_int_toc_dir = 1; // input
    spi_int_wifi_dir = 1;   // input
    spi_ss_mem_dir = 0; // output
    spi_ss_toc_dir = 0; // output
    spi_ss_lcd_dir = 0; // output
    spi_ss_wifi_dir = 0; // output

    spi_ss_lcd = 1; // disable
    spi_ss_mem = 1; // disable
    spi_ss_toc = 1; // disable
    spi_ss_wifi = 1; // disable

    TRISDbits.TRISD0 = 1;
    TRISCbits.TRISC14 = 1;
    CM3CON = 0;

    /* Initialize peripherals */
    /* SPI1 for graphic controller */
    iPPSInput(IN_FN_PPS_SDI1, IN_PIN_PPS_RP16); // remap SDI1 to spi1_miso
    iPPSOutput(OUT_PIN_PPS_RP12, OUT_FN_PPS_SDO1); // remap SDO1 to spi1_mosi
    iPPSOutput(OUT_PIN_PPS_RP3, OUT_FN_PPS_SCK1OUT); // remap SCK1 to spi1_sclk
    /* SPI2 for WiFi, touch controller */
    iPPSInput(IN_FN_PPS_SDI2, IN_PIN_PPS_RP20); // remap SDI2 to spi2_miso
    iPPSOutput(OUT_PIN_PPS_RP25, OUT_FN_PPS_SDO2); // remap SDO2 to spi2_mosi
    iPPSOutput(OUT_PIN_PPS_RP22, OUT_FN_PPS_SCK2OUT); // remap SCK2 to spi2_sclk
}

// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}