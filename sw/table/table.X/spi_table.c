// Notes:
// Max graphic SPI freq: 33 MHz / period 30 ns
// Max touch SPI freq: 900 kHz
#define USE_AND_OR

#include <spi.h>

#include "spi_table.h"
#include "uc_pins.h"

static int currentlyOpen;

UINT graphic_SPICON1Value;
UINT graphic_SPICON2Value;
UINT graphic_SPISTATValue;
UINT touch_SPICON1Value;
UINT touch_SPICON2Value;
UINT touch_SPISTATValue;
UINT wifi_SPICON1Value;
UINT wifi_SPICON2Value;
UINT wifi_SPISTATValue;

char spi_exchange(int device, char dataOut){
    char result;
    if (device == GRAPHIC){
        WriteSPI1(dataOut);
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
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON | PRI_PRESCAL_1_1 | SEC_PRESCAL_1_1 ;
    graphic_SPICON2Value = 0x00;
    graphic_SPISTATValue = SPI_ENABLE;

    // SPI config for the touch controller
    touch_SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_OFF | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON | SEC_PRESCAL_2_1 ;
    touch_SPICON2Value = 0x00;
    touch_SPISTATValue = SPI_ENABLE;

    // SPI config for the XBee wifi
    wifi_SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_OFF | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_LOW | MASTER_ENABLE_ON | PRI_PRESCAL_1_1 | SEC_PRESCAL_1_1 ;
    wifi_SPICON2Value = 0x00;
    wifi_SPISTATValue = SPI_ENABLE;

    currentlyOpen = GRAPHIC;
}

void spi_open(unsigned int device){
    spi_ss_lcd = 1; // disable
    spi_ss_mem = 1; // disable
    spi_ss_toc = 1; // disable
    spi_ss_wifi = 1; // disable
    if (device == GRAPHIC){
        OpenSPI1(graphic_SPICON1Value,graphic_SPICON2Value,graphic_SPISTATValue);
    }
    else if (currentlyOpen != device){
        if (device == TOUCH){
	    CloseSPI2();
	    OpenSPI2(touch_SPICON1Value,touch_SPICON2Value,touch_SPISTATValue);
	    currentlyOpen = TOUCH;
	}
	else if (device == WIFI){
	    CloseSPI2();
	    OpenSPI2(wifi_SPICON1Value,wifi_SPICON2Value,wifi_SPISTATValue);
	    currentlyOpen = WIFI;
	}
    }
}
