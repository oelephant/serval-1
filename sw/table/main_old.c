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
//#include "glcd.h"
#include <PPS.h>
#include <spi.h>
#include <libpic30.h>   // for delays

void spi_write(unsigned int v);
/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
UINT SPICON1Value;
UINT SPICON2Value;
UINT SPISTATValue;

int16_t main(void)
{

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    //glcd_init();
    //glcd_config_registers();

    //glcd_open();

    // slave select
    TRISCbits.TRISC13 = 0;   // output
    LATCbits.LATC13 = 0;

//    while(1)
//    {
//        LATCbits.LATC13 = ~PORTCbits.RC13; // enable
//        //LATCbits.LATC13 = 1; // enable
//    }

    // mosi
    TRISDbits.TRISD3 = 0;   // output
    LATDbits.LATD3 = 0;

    // miso
    TRISDbits.TRISD2 = 0;   // output
    LATDbits.LATD2 = 0;

    // sclk
    TRISDbits.TRISD1 = 0;   // output
    LATDbits.LATD1 = 0;

    iPPSInput(IN_FN_PPS_SDI1, IN_PIN_PPS_RP23);      // remap SDI1
    iPPSOutput(OUT_PIN_PPS_RP22, OUT_FN_PPS_SDO1);      // remap SDO1
    iPPSOutput(OUT_PIN_PPS_RP24, OUT_FN_PPS_SCK1OUT);    // remap SCK1
    SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
        | SPI_SMP_OFF | SPI_CKE_OFF | SLAVE_ENABLE_OFF
        | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON;
    SPICON2Value = 0x00;
    SPISTATValue = SPI_ENABLE;

    OpenSPI1(SPICON1Value,SPICON2Value,SPISTATValue );

//    while(1){
////        LATCbits.LATC13 = 0;
////        LATDbits.LATD3 = 0;
////        LATDbits.LATD1 = 0;
////        LATCbits.LATC13 = 1;
////        LATDbits.LATD3 = 1;
////        LATDbits.LATD1 = 1;
//        spi_write(0xaf);
//    }

    while(1)
    {

    spi_write(0xf0);    // write
    spi_write(0x00);
    spi_write(0x0f);
    spi_write(0xc9);    

    spi_write(0xf0);    // write
    spi_write(0x00);
    spi_write(0x10);
    spi_write(0xa8);

    spi_write(0xf0);    // write
    spi_write(0x00);
    spi_write(0x11);
    spi_write(0x01);

    spi_write(0xf0);    // write
    spi_write(0x00);
    spi_write(0x12);
    spi_write(0xc8);
        __delay_ms(50);
    }

    spi_write(0x0f);    // read
    spi_write(0x00);
    spi_write(0x0f);
    spi_write(0x00);

    spi_write(0x0f);    // read
    spi_write(0x00);
    spi_write(0x10);
    spi_write(0x00);

    spi_write(0x0f);    // read
    spi_write(0x00);
    spi_write(0x11);
    spi_write(0x00);

    spi_write(0x0f);    // read
    spi_write(0x00);
    spi_write(0x12);
    spi_write(0x00);

    while(1)
    {
//        glcd_write('a');
    }
}

void spi_write(unsigned int v){
    LATCbits.LATC13 = 0; // enable
    WriteSPI1((unsigned int)v); //write data to buffer
    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
    LATCbits.LATC13 = 1; // disable
}
