// Notes:
// Max graphic SPI freq: 33 MHz / period 30 ns
// Max touch SPI freq: 900 kHz
#define USE_AND_OR

#include <spi.h>

#include "spi_table.h"
#include "uc_pins.h"

UINT graphic_SPICON1Value;
UINT graphic_SPICON2Value;
UINT graphic_SPISTATValue;
UINT touch_SPICON1Value;
UINT touch_SPICON2Value;
UINT touch_SPISTATValue;

uint8_t spi_exchange(int device, uint8_t dataOut){
    uint8_t result;
    if (device == GRAPHIC){
        SPI1STATbits.SPIROV = 0;
        WriteSPI1(dataOut);
        //while (SPI1_Tx_Buf_Full);  // wait till completion of transmission
        while (!SPI1_Rx_Buf_Full);
        result = ReadSPI1();
    }
    else{
        WriteSPI2(dataOut);
        while (SPI2_Tx_Buf_Full);  // wait till completion of transmission
        while (!SPI2_Rx_Buf_Full);
        result = ReadSPI2();
    }
    return result;
}

void spi_init(void){
    // SPI config for the graphics controller
    graphic_SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_ON | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON | SEC_PRESCAL_2_1 ;   // mode 0
    graphic_SPICON2Value = 0x00;
    graphic_SPISTATValue = SPI_ENABLE;

    // SPI config for the touch controller
    touch_SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_ON | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON | SEC_PRESCAL_2_1 ;    // mode 0
    touch_SPICON2Value = 0x00;
    touch_SPISTATValue = SPI_ENABLE;
}

void spi_open(unsigned int device){
    spi_ss_lcd = 1; // disable
    spi_ss_mem = 1; // disable
    spi_ss_toc = 1; // disable
    if (device == GRAPHIC){
        spi_ss_lcd = 0; // enable
        OpenSPI1(graphic_SPICON1Value,graphic_SPICON2Value,graphic_SPISTATValue);
    }
    else if (device == MEM){
        spi_ss_mem = 0; // enable
    }
    else if (device == TOUCH){
        spi_ss_toc = 0; // enable
        OpenSPI2(touch_SPICON1Value,touch_SPICON2Value,touch_SPISTATValue);
    }
    else if (device == WIFI){

    }
}
