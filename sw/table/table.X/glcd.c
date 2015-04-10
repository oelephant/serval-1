#include <spi.h>
#include <xc.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include <libpic30.h>   // for delays
#include "glcd.h"
#include "uc_pins.h"
#include "spi_table.h"

// Configures all the registers for the GLCD
void glcd_configRegisters(void){
    glcd_writeRegister(0x06, 0x0100);  // software reset
    glcd_writeRegister(0x04, 0x0000);  // power save config
    glcd_writeRegister(0x10, 0x0000);  // pll setting 0
    glcd_writeRegister(0x12, 0x0007);  // pll setting 1
    glcd_writeRegister(0x14, 0x0013);  // pll setting 2
    glcd_writeRegister(0x16, 0x0001);  // internal clock config
    glcd_writeRegister(0x10, 0x0001);  // pll setting 0
    glcd_writeRegister(0x04, 0x0001);  // power save config

    glcd_writeRegister(0x20, 0x004F);  // panel setting misc
    glcd_writeRegister(0x22, 0x0001);  // display setting
    glcd_writeRegister(0x24, 0x0050);  // HDSIP
    glcd_writeRegister(0x26, 0x007F);  // NDSP
    glcd_writeRegister(0x28, 0x01E0);  // VDISP
    glcd_writeRegister(0x2A, 0x002D);  // VNDP
    glcd_writeRegister(0x2C, 0x009E);  // HS pulse width
    glcd_writeRegister(0x2E, 0x00CC);  // HPS
    glcd_writeRegister(0x30, 0x0083);  // VSW
    glcd_writeRegister(0x32, 0x000D);  // VPS
    glcd_writeRegister(0x40, 0x0006);  // main layer setting
    glcd_writeRegister(0x42, 0x0000);  // main layer start addr 0
    glcd_writeRegister(0x44, 0x0000);  // main layer start addr 1
    glcd_writeRegister(0x04, 0x0002);  // power save config

    /*
    glcd_writeRegister(0x06, 0x0100);  // software reset
    glcd_writeRegister(0x04, 0x0000);  // power save config
    glcd_writeRegister(0x10, 0x0000);  // pll setting 0
    glcd_writeRegister(0x12, 0x0007);  // pll setting 1
    glcd_writeRegister(0x14, 0x0013);  // pll setting 2
    glcd_writeRegister(0x16, 0x0001);  // internal clock config
    glcd_writeRegister(0x10, 0x0001);  // pll setting 0
    glcd_writeRegister(0x04, 0x0001);  // power save config
    glcd_writeRegister(0x20, 0x004F);  // panel setting misc
    glcd_writeRegister(0x22, 0x0001);  // display setting
    glcd_writeRegister(0x24, 0x0050);  // HDSIP
    glcd_writeRegister(0x26, 0x007F);  // NDSP
    glcd_writeRegister(0x28, 0x01E0);  // VDISP
    glcd_writeRegister(0x2A, 0x002D);  // VNDP
    glcd_writeRegister(0x2C, 0x0080);  // HS pulse width
    glcd_writeRegister(0x2E, 0x00CC);  // HPS
    glcd_writeRegister(0x30, 0x0080);  // VSW
    glcd_writeRegister(0x32, 0x000D);  // VPS
    glcd_writeRegister(0x40, 0x0006);  // main layer setting
    glcd_writeRegister(0x42, 0x0000);  // main layer start addr 0
    glcd_writeRegister(0x44, 0x0000);  // main layer start addr 1
    glcd_writeRegister(0x04, 0x0002);  // power save config
     */
}

struct TouchData glcd_getTouch(void){
    const int DEVICE = TOUCH;
    const uint8_t DUMMY = 0;
    uint8_t pen, x0, x1, y0, y1;
    uint16_t x, y;
    touchData t;
    
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

    x = x1<<7 | x0;
    y = y1<<7 | y0;

    x = (uint16_t)((double)x / 4095.0 * GLCD_WIDTH);
    y = GLCD_HEIGHT - (uint16_t)((double)y / 4095.0 * GLCD_HEIGHT);

    t.pen = pen;
    t.x = x;
    t.y = y;

    return t;
}

void glcd_init(void){
    int i;
    glcd_configRegisters();
    glcd_initLut1();
}

void glcd_initLut1(){
    const int DEVICE = GRAPHIC;
    uint32_t LUT_Address = 0x60000;
    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b10000000);	// write

    spi_exchange(DEVICE, LUT_Address>>16); // bits 18..16
    spi_exchange(DEVICE, LUT_Address>>8); // bits 15..8
    spi_exchange(DEVICE, LUT_Address); // bits 7..0

    glcd_initLutColor(0x00, 0x00, 0x00);    // BLACK
    glcd_initLutColor(0x80, 0x00, 0x00);    // DARK RED
    glcd_initLutColor(0x00, 0x80, 0x00);    // DARK GREEN
    glcd_initLutColor(0x80, 0x80, 0x00);    // DARK YELLOW
    glcd_initLutColor(0x00, 0x00, 0x80);    // DARK BLUE
    glcd_initLutColor(0x80, 0x00, 0x80);    // DARK MAGENTA
    glcd_initLutColor(0x00, 0x80, 0x80);    // DARK CYAN
    glcd_initLutColor(0xc0, 0xc0, 0xC0);    // GRAY
    glcd_initLutColor(0x80, 0x80, 0x80);    // DARK GRAY
    glcd_initLutColor(0xff, 0x00, 0x00);    // RED
    glcd_initLutColor(0x00, 0xff, 0x00);    // GREEN
    glcd_initLutColor(0xff, 0xff, 0x00);    // YELLOW
    glcd_initLutColor(0x00, 0x00, 0xff);    // BLUE
    glcd_initLutColor(0xff, 0x00, 0xff);    // MAGENTA
    glcd_initLutColor(0x00, 0xff, 0xff);    // CYAN
    glcd_initLutColor(0xff, 0xff, 0xff);    // WHITE

    spi_ss_lcd = 1;
}

void glcd_initLutColor(uint8_t r, uint8_t g, uint8_t b){
    const int DEVICE = GRAPHIC;
    spi_exchange(DEVICE, b);	// blue
    spi_exchange(DEVICE, g);	// green
    spi_exchange(DEVICE, r);	// red
    spi_exchange(DEVICE, 0x00);	// null
}

void glcd_putBox(uint16_t x, uint16_t y, uint8_t color, uint16_t width, uint16_t height){
    uint16_t i;
    for (i = y; i < y+height; i++){
	glcd_putPixel(x, i, color, width);
    }
}

void glcd_putPixel(uint16_t x, uint16_t y, uint8_t color, uint32_t length){
    glcd_writeVram((uint32_t)GLCD_WIDTH*y + x, color, length);
}

int glcd_readLut1(uint8_t offset, uint8_t length){
    const int DEVICE = GRAPHIC;
    const uint8_t DUMMY = 0;
    int i;
    int result[0xFF];

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b11000000);    // 8-bit read

    // address. registers start at 0x60800
    spi_exchange(DEVICE, 0x06); // bits 18..16
    spi_exchange(DEVICE, 0x00); // bits 15..8
    spi_exchange(DEVICE, offset); // bits 7..0

    // data. returns as dummy-b-g-r
    for (i = 0; i < length; i++){
	result[i] = spi_exchange(DEVICE, DUMMY);
    }

    spi_ss_lcd = 1;
    return 1;
}

uint16_t glcd_readRegister(uint8_t offset){
    const int DEVICE = GRAPHIC;
    const uint8_t DUMMY = 0;
    uint16_t result = 0;

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b11000000);    // 8-bit read

    // address. registers start at 0x60800
    spi_exchange(DEVICE, 0x6); // bits 18..16
    spi_exchange(DEVICE, 0x08); // bits 15..8
    spi_exchange(DEVICE, offset); // bits 7..0

    // data.
    spi_exchange(DEVICE, DUMMY);
    result = spi_exchange(DEVICE, DUMMY);   // lower byte
    result |= spi_exchange(DEVICE, DUMMY) << 8; // upper byte

    spi_ss_lcd = 1;
    return result;
}

int glcd_readRegisters(void){
    int r4,r6,r10,r12,r14,r16,r20,r22,r24,r26,r28,r2a,r2c,r2e,r30,r32,r40,r42,r44;
    r4 = glcd_readRegister(0x04);
    r6 = glcd_readRegister(0x06);
    r10 = glcd_readRegister(0x10);
    r12 = glcd_readRegister(0x12);
    r14 = glcd_readRegister(0x14);
    r16 = glcd_readRegister(0x16);
    r20 = glcd_readRegister(0x20);
    r22 = glcd_readRegister(0x22);
    r24 = glcd_readRegister(0x24);
    r26 = glcd_readRegister(0x26);
    r28 = glcd_readRegister(0x28);
    r2a = glcd_readRegister(0x2A);
    r2c = glcd_readRegister(0x2C);
    r2e = glcd_readRegister(0x2E);
    r30 = glcd_readRegister(0x30);
    r32 = glcd_readRegister(0x32);
    r40 = glcd_readRegister(0x40);
    r42 = glcd_readRegister(0x42);
    r44 = glcd_readRegister(0x44);

    return 1;
}

int glcd_readVram(uint32_t addr, uint8_t length){
    const int DEVICE = GRAPHIC;
    const uint8_t DUMMY = 0;
    int i;

    int res[0xFF];

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b11000000);    // 8-bit read

    // address. VRAM from 0x00000 to 0x5FFFF
    spi_exchange(DEVICE, addr>>16); // bits 18..16
    spi_exchange(DEVICE, addr>>8); // bits 15..8
    spi_exchange(DEVICE, addr); // bits 7..0

    // data.
    for (i = 0; i < length; i++){
	res[i] = spi_exchange(DEVICE, DUMMY);
    }

    spi_ss_lcd = 1;
    return 1;
}

// Writes to the registers of the GLCD
void glcd_writeRegister(uint8_t offset, uint16_t val){
    const int DEVICE = GRAPHIC;

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b10000000);

    // address. registers start at 0x60800
    spi_exchange(DEVICE, 0x6); // bits 18..16
    spi_exchange(DEVICE, 0x08); // bits 15..8
    spi_exchange(DEVICE, offset); // bits 7..0

    // data. for 8-bit addressing, register accesses are Little Endian
    spi_exchange(DEVICE, val);    // lower byte
    spi_exchange(DEVICE, val>>8); // upper byte

    spi_ss_lcd = 1;

}

void glcd_writeVram(uint32_t addr, uint8_t lut_offset, uint32_t length){
    const int DEVICE = GRAPHIC;
    uint32_t i;

    spi_ss_lcd = 0;

    // command
    spi_exchange(DEVICE, 0b10000000);	// write

    // address. VRAM from 0x00000 to 0x5FFFF
    spi_exchange(DEVICE, addr>>16); // bits 18..16
    spi_exchange(DEVICE, addr>>8); // bits 15..8
    spi_exchange(DEVICE, addr); // bits 7..0

    // data. for 8 bpp LUT, write LUT address offset
    for (i = 0; i < length; i++){
	if (addr + i > 0x5FFFF){
	    break;
	}
	spi_exchange(DEVICE, lut_offset);
    }

    spi_ss_lcd = 1;
}