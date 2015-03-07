// Notes:
// check max SPI freq for glcd ctrl

#include <PPS.h>
#include <spi.h>
#include <xc.h>
#include "glcd.h"

#define bit_8_write 0x80
#define spi_ss_lcd LATDbits.LATD9

UINT SPICON1Value;
UINT SPICON2Value;
UINT SPISTATValue;

void glcd_close(void){
    CloseSPI1();
    spi_ss_lcd = 1; // disable
}

void glcd_config_registers(void){
    OpenSPI1(SPICON1Value,SPICON2Value,SPISTATValue );
    glcd_write_config(0x06, 0x0100);  // software reset
    glcd_write_config(0x04, 0x0000);  // power save config
    glcd_write_config(0x10, 0x0000);  // pll setting 0
    glcd_write_config(0x12, 0x000B);  // pll setting 1
    glcd_write_config(0x14, 0x0013);  // pll setting 2
    glcd_write_config(0x16, 0x0000);  // internal clock config
    glcd_write_config(0x10, 0x0001);  // pll setting 0
    glcd_write_config(0x04, 0x0001);  // power save config
    glcd_write_config(0x20, 0x004F);  // panel setting misc
    glcd_write_config(0x22, 0x0001);  // display setting
    glcd_write_config(0x24, 0x0050);  // HDSIP
    glcd_write_config(0x26, 0x007F);  // NDSP
    glcd_write_config(0x28, 0x01E0);  // VDISP
    glcd_write_config(0x2A, 0x002D);  // VNDP
    glcd_write_config(0x2C, 0x00A6);  // HS pulse width
    glcd_write_config(0x2E, 0x00CC);  // HPS
    glcd_write_config(0x30, 0x0083);  // VSW
    glcd_write_config(0x32, 0x000D);  // VPS
    glcd_write_config(0x40, 0x0006);  // main layer setting
    glcd_write_config(0x42, 0x0000);  // main layer start addr 0
    glcd_write_config(0x44, 0x0000);  // main layer start addr 1
    glcd_write_config(0x04, 0x0002);  // power save config
}

void glcd_init(void){
    TRISDbits.TRISD9 = 0;   // spi_ss_lcd as output

    SPICON1Value = DISABLE_SCK_PIN | DISABLE_SDO_PIN | SPI_MODE8_ON
            | SPI_SMP_OFF | SPI_CKE_OFF | SLAVE_ENABLE_OFF
            | CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON;
    SPICON2Value = 0x00;
    SPISTATValue = SPI_ENABLE;
    iPPSOutput(OUT_PIN_PPS_RP12, OUT_FN_PPS_SDO1);      // remap SDO1
    iPPSOutput(OUT_PIN_PPS_RP3, OUT_FN_PPS_SCK1OUT);    // remap SCK1
}

void glcd_open(){
    OpenSPI1(SPICON1Value,SPICON2Value,SPISTATValue );
    spi_ss_lcd = 0; // enable
}

void glcd_write(uint8_t val){
    WriteSPI1((unsigned int)val); //write data to buffer
    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
}

void glcd_write_config(uint8_t addr_offset, uint16_t val){
    // command
    glcd_write(0b10000000);

    // address. registers start at 0x60800
    glcd_write(0x6); // bits 18..16
    glcd_write(0x08); // bits 15..8
    glcd_write(addr_offset); // bits 7..0

    // data. for 8-bit addressing, register accesses are Little Endian
    glcd_write(0x0F && val);    // lower byte
    glcd_write(val>>8); // upper byte

}
