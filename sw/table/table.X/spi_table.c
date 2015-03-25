// Notes:
// Max graphic SPI freq: 33 MHz / period 30 ns
// Max touch SPI freq: 900 kHz
#define USE_AND_OR

#include <spi.h>

#include "spi_table.h"
#include "uc_pins.h"

enum mode{GRAPHIC, MEM, TOUCH};

UINT graphic_SPICON1Value;
UINT graphic_SPICON2Value;
UINT graphic_SPISTATValue;
UINT touch_SPICON1Value;
UINT touch_SPICON2Value;
UINT touch_SPISTATValue;

uint8_t spi_exchange(uint8_t dataOut){
    WriteSPI1(dataOut);
    while (SPI1_Tx_Buf_Full);  // wait till completion of transmission
    while (!SPI1_Rx_Buf_Full);
    return ReadSPI1();
}

void spi_init(void){
    // SPI config for the graphics controller
    graphic_SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_OFF | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON;
    graphic_SPICON2Value = 0x00;
    graphic_SPISTATValue = SPI_ENABLE;

    // SPI config for the touch controller
    touch_SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_OFF | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON | SEC_PRESCAL_2_1 ;
    touch_SPICON2Value = 0x00;
    touch_SPISTATValue = SPI_ENABLE;
}

void spi_open(unsigned int mode){
    spi_ss_lcd = 1; // disable
    spi_ss_mem = 1; // disable
    spi_ss_toc = 1; // disable
    if (mode == GRAPHIC){
        spi_ss_lcd = 0; // enable
        OpenSPI1(graphic_SPICON1Value,graphic_SPICON2Value,graphic_SPISTATValue);
    }
    else if (mode == MEM){
        spi_ss_mem = 0; // enable
    }
    else if (mode == TOUCH){
        spi_ss_toc = 0; // enable
        OpenSPI1(touch_SPICON1Value,touch_SPICON2Value,touch_SPISTATValue);
    }
}
