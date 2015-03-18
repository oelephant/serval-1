// Notes:
// Max graphic SPI freq: 33 MHz / period 30 ns
// Max touch SPI 900 kHz
#define USE_AND_OR

#include <PPS.h>
#include <spi.h>
#include <xc.h>
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include <libpic30.h>   // for delays
#include "glcd.h"

#define spi_ss_toc LATBbits.LATB6
#define spi_ss_lcd LATDbits.LATD9

UINT graphic_SPICON1Value;
UINT graphic_SPICON2Value;
UINT graphic_SPISTATValue;
UINT touch_SPICON1Value;
UINT touch_SPICON2Value;
UINT touch_SPISTATValue;

// Closes SPI for the GLCD
void glcd_close(void){
    CloseSPI1();
    spi_ss_toc = 1; // disable
    spi_ss_lcd = 1; // disable
}

// Configures all the registers for the GLCD
void glcd_config_registers(void){
    OpenSPI1(graphic_SPICON1Value,graphic_SPICON2Value,graphic_SPISTATValue );
    glcd_writeConfig(0x06, 0x0100);  // software reset
    glcd_writeConfig(0x04, 0x0000);  // power save config
    glcd_writeConfig(0x10, 0x0000);  // pll setting 0
    glcd_writeConfig(0x12, 0x000B);  // pll setting 1
    glcd_writeConfig(0x14, 0x0013);  // pll setting 2
    glcd_writeConfig(0x16, 0x0000);  // internal clock config
    glcd_writeConfig(0x10, 0x0001);  // pll setting 0
    glcd_writeConfig(0x04, 0x0001);  // power save config
    glcd_writeConfig(0x20, 0x004F);  // panel setting misc
    glcd_writeConfig(0x22, 0x0001);  // display setting
    glcd_writeConfig(0x24, 0x0050);  // HDSIP
    glcd_writeConfig(0x26, 0x007F);  // NDSP
    glcd_writeConfig(0x28, 0x01E0);  // VDISP
    glcd_writeConfig(0x2A, 0x002D);  // VNDP
    glcd_writeConfig(0x2C, 0x00A6);  // HS pulse width
    glcd_writeConfig(0x2E, 0x00CC);  // HPS
    glcd_writeConfig(0x30, 0x0083);  // VSW
    glcd_writeConfig(0x32, 0x000D);  // VPS
    glcd_writeConfig(0x40, 0x0006);  // main layer setting
    glcd_writeConfig(0x42, 0x0000);  // main layer start addr 0
    glcd_writeConfig(0x44, 0x0000);  // main layer start addr 1
    glcd_writeConfig(0x04, 0x0002);  // power save config
}

void glcd_getTouch(void){

}

// Configures reprogrammable pins and values for SPI config
void glcd_init(void){
    TRISBbits.TRISB6 = 0;   // spi_ss_toc as output
    TRISDbits.TRISD9 = 0;   // spi_ss_lcd as output
    spi_ss_toc = 1; // disable
    spi_ss_lcd = 1; // disable

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

    // Remap pins
    iPPSInput(IN_FN_PPS_SDI1, IN_PIN_PPS_RP11);      // remap SDI1 to spi_miso
    iPPSOutput(OUT_PIN_PPS_RP12, OUT_FN_PPS_SDO1);      // remap SDO1 to spi_mosi
    iPPSOutput(OUT_PIN_PPS_RP3, OUT_FN_PPS_SCK1OUT);    // remap SCK1 to spi_sclk
}

// Opens SPI for graphic controller
void glcd_openGraphic(){
    CloseSPI1();    // close anything else that might be open
    spi_ss_toc = 1; // disable
    spi_ss_lcd = 1; // disable
    OpenSPI1(graphic_SPICON1Value,graphic_SPICON2Value,graphic_SPISTATValue );
}

// Opens SPI for touch controller
void glcd_openTouch(){
    CloseSPI1();    // close anything else that might be open
    spi_ss_toc = 1; // disable
    spi_ss_lcd = 1; // disable
    OpenSPI1(touch_SPICON1Value,touch_SPICON2Value,touch_SPISTATValue );
}

unsigned int glcd_readTouch(void){
    unsigned int a,b,c,d,e;
    //unsigned int *packet;
    if (SPI1_Rx_Buf_Full){
        b = SPI1BUF;
    }
    spi_ss_toc = 0;
    WriteSPI1(0x00);   // send dummy byte
    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission

//    WriteSPI1(0x55);   // send dummy byte
//    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
//    __delay_us(400);
//    WriteSPI1(0x01);   // send dummy byte
//    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
//    __delay_us(400);
//    WriteSPI1(0x10);   // send dummy byte
//    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
//    __delay_us(400);
//    WriteSPI1(0xFF);   // send dummy byte
//    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
//    __delay_us(400);
//    WriteSPI1(0xFF);   // send dummy byte
//    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
//    __delay_us(400);

    while (! SPI1_Rx_Buf_Full);
    a = ReadSPI1();
    spi_ss_toc = 1;
//    b = ReadSPI1();
//    c = ReadSPI1();
//    d = ReadSPI1();
//    e = ReadSPI1();
    //getsSPI1(5, packet, 1000);
    return 1;
}

// Writes a byte to the GLCD with SPI
void glcd_writeGraphic(uint8_t val){
    spi_ss_lcd = 0; // enable
    WriteSPI1((unsigned int)val); //write data to buffer
    while(SPI1_Tx_Buf_Full);  //wait till completion of transmission
    spi_ss_lcd = 1; // disable
}

void glcd_writePixel(uint32_t addr, uint8_t r, uint8_t g, uint8_t b){
    glcd_writeGraphic(0b10000000);

    // address. VRAM from 0x00000 to 0x5FFFF
    glcd_writeGraphic(addr>>16); // bits 18..16
    glcd_writeGraphic(addr>>8); // bits 15..8
    glcd_writeGraphic(addr); // bits 7..0

    // data. for 8-bit addressing, write R, G, B bytes
    glcd_writeGraphic(r);  // red
    glcd_writeGraphic(g);  // green
    glcd_writeGraphic(b);  // blue
}

// Writes to the registers of the GLCD
void glcd_writeConfig(uint8_t addr_offset, uint16_t val){
    // command
    glcd_writeGraphic(0b10000000);

    // address. registers start at 0x60800
    glcd_writeGraphic(0x6); // bits 18..16
    glcd_writeGraphic(0x08); // bits 15..8
    glcd_writeGraphic(addr_offset); // bits 7..0

    // data. for 8-bit addressing, register accesses are Little Endian
    glcd_writeGraphic(0x0F && val);    // lower byte
    glcd_writeGraphic(val>>8); // upper byte

}
