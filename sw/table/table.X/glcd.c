#include <spi.h>
#include <xc.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include <libpic30.h>   // for delays
#include "glcd.h"
#include "uc_pins.h"
#include "spi_table.h"

// Configures all the registers for the GLCD
void glcd_configRegisters(void){
    spi_open(GRAPHIC);
    glcd_writeConfig(0x06, 0x0100);  // software reset
    glcd_writeConfig(0x04, 0x0000);  // power save config
    glcd_writeConfig(0x10, 0x0000);  // pll setting 0
    glcd_writeConfig(0x12, 0x0007);  // pll setting 1
    glcd_writeConfig(0x14, 0x0013);  // pll setting 2
    glcd_writeConfig(0x16, 0x0001);  // internal clock config
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
    glcd_writeConfig(0x40, 0x0000);  // main layer setting
    glcd_writeConfig(0x42, 0x0000);  // main layer start addr 0
    glcd_writeConfig(0x44, 0x0000);  // main layer start addr 1
    glcd_writeConfig(0x04, 0x0002);  // power save config
}

int glcd_getTouch(void){
    const int DEVICE = TOUCH;
    const uint8_t DUMMY = 0;
    uint8_t pen, x0, x1, y0, y1;
    
    pen = spi_exchange(DEVICE, DUMMY);
    __delay_us(400);
    x0 = spi_exchange(DEVICE, DUMMY);
    __delay_us(400);
    x1 = spi_exchange(DEVICE, DUMMY);
    __delay_us(400);
    y0 = spi_exchange(DEVICE, DUMMY);
    __delay_us(400);
    y1 = spi_exchange(DEVICE, DUMMY);
    __delay_us(400);

    return 1;
}

// Configures reprogrammable pins and values for SPI config
void glcd_init(void){
    glcd_configRegisters();
}

unsigned int glcd_readGraphic(uint32_t addr){
    const int DEVICE = GRAPHIC;
    const uint8_t DUMMY = 0;
    uint16_t result = 0;
    uint8_t a,b,c,d,e;

    spi_ss_lcd = 0;

    // command
    a = spi_exchange(DEVICE, 0b11000000);    // 8-bit read

    // address. VRAM from 0x00000 to 0x5FFFF
    b = spi_exchange(DEVICE, 0x06); // bits 18..16
    c = spi_exchange(DEVICE, 0x08); // bits 15..8
    d = spi_exchange(DEVICE, addr); // bits 7..0

    // data.
    e = spi_exchange(DEVICE, DUMMY);
    result = spi_exchange(DEVICE, DUMMY);   // lower byte
    result |= spi_exchange(DEVICE, DUMMY) << 8; // upper byte

    spi_ss_lcd = 1;
        __delay_ms(3);
    return result;
}

void glcd_writePixel(uint32_t addr, uint8_t r, uint8_t g, uint8_t b){
    const int DEVICE = GRAPHIC;

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b10000000);

    // address. VRAM from 0x00000 to 0x5FFFF
    spi_exchange(DEVICE, addr>>16); // bits 18..16
    spi_exchange(DEVICE, addr>>8); // bits 15..8
    spi_exchange(DEVICE, addr); // bits 7..0

    // data. for 8-bit addressing, write R, G, B bytes
    spi_exchange(DEVICE, r);  // red
    spi_exchange(DEVICE, g);  // green
    spi_exchange(DEVICE, b);  // blue
    spi_exchange(DEVICE, r);  // red
    spi_exchange(DEVICE, g);  // green
    spi_exchange(DEVICE, b);  // blue
    spi_exchange(DEVICE, r);  // red
    spi_exchange(DEVICE, g);  // green
    spi_exchange(DEVICE, b);  // blue

    spi_ss_lcd = 1;
}

// Writes to the registers of the GLCD
void glcd_writeConfig(uint8_t addr_offset, uint16_t val){
    const int DEVICE = GRAPHIC;

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b10000000);

    // address. registers start at 0x60800
    spi_exchange(DEVICE, 0x6); // bits 18..16
    spi_exchange(DEVICE, 0x08); // bits 15..8
    spi_exchange(DEVICE, addr_offset); // bits 7..0

    // data. for 8-bit addressing, register accesses are Little Endian
    spi_exchange(DEVICE, val);    // lower byte
    spi_exchange(DEVICE, val>>8); // upper byte

    spi_ss_lcd = 1;

}
