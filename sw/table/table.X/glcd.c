/*  glcd.c
    created by Ellen Fluehr
 */

#include <xc.h>

#include "font.h"
#include "glcd.h"
#include <math.h>
#include "uc_pins.h"
#include "spi_table.h"
#include <string.h>

extern const char Tahoma19x21[];

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  configures all the registers for the GLCD
 */
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
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  configures the touch controller
 */
int glcd_configTouch(void){
    const char DUMMY = 0;
    char startAddress;
    char reply[5];

    spi_open(TOUCH);
    spi_ss_toc = 0;	// enable

    // get register start address
    glcd_touchExchange(0x55);	// header
    glcd_touchExchange(1);	// number of bytes after this
    glcd_touchExchange(0x22);	// command ID (register start address req)
    //while (!spi_int_toc);
    reply[0] = glcd_touchExchange(DUMMY);	// header
    reply[1] = glcd_touchExchange(DUMMY);	// number of bytes to follow
    reply[2] = glcd_touchExchange(DUMMY);	// success status
    reply[3] = glcd_touchExchange(DUMMY);
    if (!(reply[0] == 0x55 && reply[1] == 0x03 && reply[2] == 0x00 && reply[3] == 0x22)){
	return 1;
    }
    startAddress = glcd_touchExchange(DUMMY);	// start address

    // write to register
    glcd_touchExchange(0x55);	// header
    glcd_touchExchange(0x04 + 1);	// number of bytes after this
    glcd_touchExchange(0x21);	// command ID (register write)
    glcd_touchExchange(0x00);	// address high byte
    glcd_touchExchange(startAddress+0x0C);	// address low byte
    glcd_touchExchange(0x00);	// data high
    glcd_touchExchange(0x51);	// data (pen down = 1; pen up = 0)
    //while (!spi_int_toc);
    reply[0] = glcd_touchExchange(DUMMY);
    reply[1] = glcd_touchExchange(DUMMY);
    reply[2] = glcd_touchExchange(DUMMY);
    reply[3] = glcd_touchExchange(DUMMY);

    spi_ss_toc = 1;	// disable

    return 1;
}

/* parameters
 *  none
 * return
 *  touch data
 * purpose
 *  gets and parses a packet from the touch screen controller
 */
struct TouchData glcd_getTouch(void){
    static char lastPen;
    const char DUMMY = 0;
    char pen, x0, x1, y0, y1;
    int x, y;
    struct TouchData t;

    spi_open(TOUCH);
    spi_ss_toc = 0;	// enable

    pen = glcd_touchExchange(DUMMY);
    x0 = glcd_touchExchange(DUMMY);
    x1 = glcd_touchExchange(DUMMY);
    y0 = glcd_touchExchange(DUMMY);
    y1 = glcd_touchExchange(DUMMY);

    spi_ss_toc = 1;	// disable

    x = x1<<7 | x0;
    y = y1<<7 | y0;

    x = (int)((double)x / 4095.0 * GLCD_WIDTH);
    y = GLCD_HEIGHT - (int)((double)y / 4095.0 * GLCD_HEIGHT);

    if ((lastPen&1) == 0 && (pen&1) == 0){
	t.pen = 1;
    }
    else{
	t.pen = pen;
    }
    t.x = x;
    t.y = y;
    lastPen = t.pen;

    return t;
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  initializes the glcd
 */
void glcd_init(void){
    //glcd_configTouch();
    spi_open(GRAPHIC);
    glcd_configRegisters();
    __delay_ms(100);
    glcd_initLut1();
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  initializes the glcd's lookup table
 */
void glcd_initLut1(){
    const int DEVICE = GRAPHIC;
    long LUT_Address = 0x60000;
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
    glcd_initLutColor(0xc0, 0xc0, 0xc0);    // GRAY
    glcd_initLutColor(0x80, 0x80, 0x80);    // DARK GRAY
    glcd_initLutColor(0xff, 0x00, 0x00);    // RED
    glcd_initLutColor(0x00, 0xff, 0x00);    // GREEN
    glcd_initLutColor(0xff, 0xff, 0x00);    // YELLOW
    glcd_initLutColor(0x00, 0x00, 0xff);    // BLUE
    glcd_initLutColor(0xff, 0x00, 0xff);    // MAGENTA
    glcd_initLutColor(0x00, 0xff, 0xff);    // CYAN
    glcd_initLutColor(0xff, 0xff, 0xff);    // WHITE

    // blue/green
    glcd_initLutColor(0x00, 0x00, 0x00);
    glcd_initLutColor(0x00, 0x00, 0x5f);
    glcd_initLutColor(0x00, 0x00, 0x87);
    glcd_initLutColor(0x00, 0x00, 0xaf);
    glcd_initLutColor(0x00, 0x00, 0xd7);
    glcd_initLutColor(0x00, 0x00, 0xff);
    glcd_initLutColor(0x00, 0x5f, 0x00);
    glcd_initLutColor(0x00, 0x5f, 0x5f);
    glcd_initLutColor(0x00, 0x5f, 0x87);
    glcd_initLutColor(0x00, 0x5f, 0xaf);
    glcd_initLutColor(0x00, 0x5f, 0xd7);
    glcd_initLutColor(0x00, 0x5f, 0xff);
    glcd_initLutColor(0x00, 0x87, 0x00);
    glcd_initLutColor(0x00, 0x87, 0x5f);
    glcd_initLutColor(0x00, 0x87, 0x87);
    glcd_initLutColor(0x00, 0x87, 0xaf);
    glcd_initLutColor(0x00, 0x87, 0xd7);
    glcd_initLutColor(0x00, 0x87, 0xff);
    glcd_initLutColor(0x00, 0xaf, 0x00);
    glcd_initLutColor(0x00, 0xaf, 0x5f);
    glcd_initLutColor(0x00, 0xaf, 0x87);
    glcd_initLutColor(0x00, 0xaf, 0xaf);
    glcd_initLutColor(0x00, 0xaf, 0xd7);
    glcd_initLutColor(0x00, 0xaf, 0xff);
    glcd_initLutColor(0x00, 0xd7, 0x00);
    glcd_initLutColor(0x00, 0xd7, 0x5f);
    glcd_initLutColor(0x00, 0xd7, 0x87);
    glcd_initLutColor(0x00, 0xd7, 0xaf);
    glcd_initLutColor(0x00, 0xd7, 0xd7);
    glcd_initLutColor(0x00, 0xd7, 0xff);
    glcd_initLutColor(0x00, 0xff, 0x00);
    glcd_initLutColor(0x00, 0xff, 0x5f);
    glcd_initLutColor(0x00, 0xff, 0x87);
    glcd_initLutColor(0x00, 0xff, 0xaf);
    glcd_initLutColor(0x00, 0xff, 0xd7);
    glcd_initLutColor(0x00, 0xff, 0xff);

    glcd_initLutColor(0x5f, 0x00, 0x00);
    glcd_initLutColor(0x5f, 0x00, 0x5f);
    glcd_initLutColor(0x5f, 0x00, 0x87);
    glcd_initLutColor(0x5f, 0x00, 0xaf);
    glcd_initLutColor(0x5f, 0x00, 0xd7);
    glcd_initLutColor(0x5f, 0x00, 0xf7);
    glcd_initLutColor(0x5f, 0x5f, 0x00);
    glcd_initLutColor(0x5f, 0x5f, 0x5f);
    glcd_initLutColor(0x5f, 0x5f, 0x87);
    glcd_initLutColor(0x5f, 0x5f, 0xaf);
    glcd_initLutColor(0x5f, 0x5f, 0xd7);
    glcd_initLutColor(0x5f, 0x5f, 0xf7);
    glcd_initLutColor(0x5f, 0x87, 0x00);
    glcd_initLutColor(0x5f, 0x87, 0x5f);
    glcd_initLutColor(0x5f, 0x87, 0x87);
    glcd_initLutColor(0x5f, 0x87, 0xaf);
    glcd_initLutColor(0x5f, 0x87, 0xd7);
    glcd_initLutColor(0x5f, 0x87, 0xf7);
    glcd_initLutColor(0x5f, 0xaf, 0x00);
    glcd_initLutColor(0x5f, 0xaf, 0x5f);
    glcd_initLutColor(0x5f, 0xaf, 0x87);
    glcd_initLutColor(0x5f, 0xaf, 0xaf);
    glcd_initLutColor(0x5f, 0xaf, 0xd7);
    glcd_initLutColor(0x5f, 0xaf, 0xf7);
    glcd_initLutColor(0x5f, 0xd7, 0x00);
    glcd_initLutColor(0x5f, 0xd7, 0x5f);
    glcd_initLutColor(0x5f, 0xd7, 0x87);
    glcd_initLutColor(0x5f, 0xd7, 0xaf);
    glcd_initLutColor(0x5f, 0xd7, 0xd7);
    glcd_initLutColor(0x5f, 0xd7, 0xf7);
    glcd_initLutColor(0x5f, 0xff, 0x00);
    glcd_initLutColor(0x5f, 0xff, 0x5f);
    glcd_initLutColor(0x5f, 0xff, 0x87);
    glcd_initLutColor(0x5f, 0xff, 0xaf);
    glcd_initLutColor(0x5f, 0xff, 0xd7);
    glcd_initLutColor(0x5f, 0xff, 0xf7);

    glcd_initLutColor(0x87, 0x00, 0x00);
    glcd_initLutColor(0x87, 0x00, 0x5f);
    glcd_initLutColor(0x87, 0x00, 0x87);
    glcd_initLutColor(0x87, 0x00, 0xaf);
    glcd_initLutColor(0x87, 0x00, 0xd7);
    glcd_initLutColor(0x87, 0x00, 0xff);
    glcd_initLutColor(0x87, 0x5f, 0x00);
    glcd_initLutColor(0x87, 0x5f, 0x5f);
    glcd_initLutColor(0x87, 0x5f, 0x87);
    glcd_initLutColor(0x87, 0x5f, 0xaf);
    glcd_initLutColor(0x87, 0x5f, 0xd7);
    glcd_initLutColor(0x87, 0x5f, 0xff);
    glcd_initLutColor(0x87, 0x87, 0x00);
    glcd_initLutColor(0x87, 0x87, 0x5f);
    glcd_initLutColor(0x87, 0x87, 0x87);
    glcd_initLutColor(0x87, 0x87, 0xaf);
    glcd_initLutColor(0x87, 0x87, 0xd7);
    glcd_initLutColor(0x87, 0x87, 0xff);
    glcd_initLutColor(0x87, 0xaf, 0x00);
    glcd_initLutColor(0x87, 0xaf, 0x5f);
    glcd_initLutColor(0x87, 0xaf, 0x87);
    glcd_initLutColor(0x87, 0xaf, 0xaf);
    glcd_initLutColor(0x87, 0xaf, 0xd7);
    glcd_initLutColor(0x87, 0xaf, 0xff);
    glcd_initLutColor(0x87, 0xd7, 0x00);
    glcd_initLutColor(0x87, 0xd7, 0x5f);
    glcd_initLutColor(0x87, 0xd7, 0x87);
    glcd_initLutColor(0x87, 0xd7, 0xaf);
    glcd_initLutColor(0x87, 0xd7, 0xd7);
    glcd_initLutColor(0x87, 0xd7, 0xff);
    glcd_initLutColor(0x87, 0xff, 0x00);
    glcd_initLutColor(0x87, 0xff, 0x5f);
    glcd_initLutColor(0x87, 0xff, 0x87);
    glcd_initLutColor(0x87, 0xff, 0xaf);
    glcd_initLutColor(0x87, 0xff, 0xd7);
    glcd_initLutColor(0x87, 0xff, 0xff);

    glcd_initLutColor(0xaf, 0x00, 0x00);
    glcd_initLutColor(0xaf, 0x00, 0x5f);
    glcd_initLutColor(0xaf, 0x00, 0x87);
    glcd_initLutColor(0xaf, 0x00, 0xaf);
    glcd_initLutColor(0xaf, 0x00, 0xd7);
    glcd_initLutColor(0xaf, 0x00, 0xf7);
    glcd_initLutColor(0xaf, 0x5f, 0x00);
    glcd_initLutColor(0xaf, 0x5f, 0x5f);
    glcd_initLutColor(0xaf, 0x5f, 0x87);
    glcd_initLutColor(0xaf, 0x5f, 0xaf);
    glcd_initLutColor(0xaf, 0x5f, 0xd7);
    glcd_initLutColor(0xaf, 0x5f, 0xf7);
    glcd_initLutColor(0xaf, 0x87, 0x00);
    glcd_initLutColor(0xaf, 0x87, 0x5f);
    glcd_initLutColor(0xaf, 0x87, 0x87);
    glcd_initLutColor(0xaf, 0x87, 0xaf);
    glcd_initLutColor(0xaf, 0x87, 0xd7);
    glcd_initLutColor(0xaf, 0x87, 0xf7);
    glcd_initLutColor(0xaf, 0xaf, 0x00);
    glcd_initLutColor(0xaf, 0xaf, 0x5f);
    glcd_initLutColor(0xaf, 0xaf, 0x87);
    glcd_initLutColor(0xaf, 0xaf, 0xaf);
    glcd_initLutColor(0xaf, 0xaf, 0xd7);
    glcd_initLutColor(0xaf, 0xaf, 0xf7);
    glcd_initLutColor(0xaf, 0xd7, 0x00);
    glcd_initLutColor(0xaf, 0xd7, 0x5f);
    glcd_initLutColor(0xaf, 0xd7, 0x87);
    glcd_initLutColor(0xaf, 0xd7, 0xaf);
    glcd_initLutColor(0xaf, 0xd7, 0xd7);
    glcd_initLutColor(0xaf, 0xd7, 0xf7);
    glcd_initLutColor(0xaf, 0xff, 0x00);
    glcd_initLutColor(0xaf, 0xff, 0x5f);
    glcd_initLutColor(0xaf, 0xff, 0x87);
    glcd_initLutColor(0xaf, 0xff, 0xaf);
    glcd_initLutColor(0xaf, 0xff, 0xd7);
    glcd_initLutColor(0xaf, 0xff, 0xf7);

    glcd_initLutColor(0xd7, 0x00, 0x00);
    glcd_initLutColor(0xd7, 0x00, 0x5f);
    glcd_initLutColor(0xd7, 0x00, 0x87);
    glcd_initLutColor(0xd7, 0x00, 0xaf);
    glcd_initLutColor(0xd7, 0x00, 0xd7);
    glcd_initLutColor(0xd7, 0x00, 0xff);
    glcd_initLutColor(0xd7, 0x5f, 0x00);
    glcd_initLutColor(0xd7, 0x5f, 0x5f);
    glcd_initLutColor(0xd7, 0x5f, 0x87);
    glcd_initLutColor(0xd7, 0x5f, 0xaf);
    glcd_initLutColor(0xd7, 0x5f, 0xd7);
    glcd_initLutColor(0xd7, 0x5f, 0xff);
    glcd_initLutColor(0xd7, 0x87, 0x00);
    glcd_initLutColor(0xd7, 0x87, 0x5f);
    glcd_initLutColor(0xd7, 0x87, 0x87);
    glcd_initLutColor(0xd7, 0x87, 0xaf);
    glcd_initLutColor(0xd7, 0x87, 0xd7);
    glcd_initLutColor(0xd7, 0x87, 0xff);
    glcd_initLutColor(0xd7, 0xaf, 0x00);
    glcd_initLutColor(0xd7, 0xaf, 0x5f);
    glcd_initLutColor(0xd7, 0xaf, 0x87);
    glcd_initLutColor(0xd7, 0xaf, 0xaf);
    glcd_initLutColor(0xd7, 0xaf, 0xd7);
    glcd_initLutColor(0xd7, 0xaf, 0xff);
    glcd_initLutColor(0xd7, 0xd7, 0x00);
    glcd_initLutColor(0xd7, 0xd7, 0x5f);
    glcd_initLutColor(0xd7, 0xd7, 0x87);
    glcd_initLutColor(0xd7, 0xd7, 0xaf);
    glcd_initLutColor(0xd7, 0xd7, 0xd7);
    glcd_initLutColor(0xd7, 0xd7, 0xff);
    glcd_initLutColor(0xd7, 0xff, 0x00);
    glcd_initLutColor(0xd7, 0xff, 0x5f);
    glcd_initLutColor(0xd7, 0xff, 0x87);
    glcd_initLutColor(0xd7, 0xff, 0xaf);
    glcd_initLutColor(0xd7, 0xff, 0xd7);
    glcd_initLutColor(0xd7, 0xff, 0xff);

    glcd_initLutColor(0xff, 0x00, 0x00);
    glcd_initLutColor(0xff, 0x00, 0x5f);
    glcd_initLutColor(0xff, 0x00, 0x87);
    glcd_initLutColor(0xff, 0x00, 0xaf);
    glcd_initLutColor(0xff, 0x00, 0xd7);
    glcd_initLutColor(0xff, 0x00, 0xf7);
    glcd_initLutColor(0xff, 0x5f, 0x00);
    glcd_initLutColor(0xff, 0x5f, 0x5f);
    glcd_initLutColor(0xff, 0x5f, 0x87);
    glcd_initLutColor(0xff, 0x5f, 0xaf);
    glcd_initLutColor(0xff, 0x5f, 0xd7);
    glcd_initLutColor(0xff, 0x5f, 0xf7);
    glcd_initLutColor(0xff, 0x87, 0x00);
    glcd_initLutColor(0xff, 0x87, 0x5f);
    glcd_initLutColor(0xff, 0x87, 0x87);
    glcd_initLutColor(0xff, 0x87, 0xaf);
    glcd_initLutColor(0xff, 0x87, 0xd7);
    glcd_initLutColor(0xff, 0x87, 0xf7);
    glcd_initLutColor(0xff, 0xaf, 0x00);
    glcd_initLutColor(0xff, 0xaf, 0x5f);
    glcd_initLutColor(0xff, 0xaf, 0x87);
    glcd_initLutColor(0xff, 0xaf, 0xaf);
    glcd_initLutColor(0xff, 0xaf, 0xd7);
    glcd_initLutColor(0xff, 0xaf, 0xf7);
    glcd_initLutColor(0xff, 0xd7, 0x00);
    glcd_initLutColor(0xff, 0xd7, 0x5f);
    glcd_initLutColor(0xff, 0xd7, 0x87);
    glcd_initLutColor(0xff, 0xd7, 0xaf);
    glcd_initLutColor(0xff, 0xd7, 0xd7);
    glcd_initLutColor(0xff, 0xd7, 0xf7);
    glcd_initLutColor(0xff, 0xff, 0x00);
    glcd_initLutColor(0xff, 0xff, 0x5f);
    glcd_initLutColor(0xff, 0xff, 0x87);
    glcd_initLutColor(0xff, 0xff, 0xaf);
    glcd_initLutColor(0xff, 0xff, 0xd7);
    glcd_initLutColor(0xff, 0xff, 0xf7);
    spi_ss_lcd = 1;
}

/* parameters
 *  r: the red value of the color
 *  g: the green value of the color
 *  b: the blue value of the color
 * return
 *  none
 * purpose
 *  initializes a color in the glcd's lookup table
 */
void glcd_initLutColor(char r, char g, char b){
    const int DEVICE = GRAPHIC;
    spi_exchange(DEVICE, b);	// blue
    spi_exchange(DEVICE, g);	// green
    spi_exchange(DEVICE, r);	// red
    spi_exchange(DEVICE, 0x00);	// null
}

/* parameters
 *  x: the x coordination of the upper left corner
 *  y: the y coordinate of the upper left corner
 *  color: the fill color of the box
 *  width: the width of the box
 *  height: the height of the box
 * return
 *  none
 * purpose
 *  draws a box of the specified size and position on the glcd
 */
void glcd_putBox(int x, int y, char color, int width, int height){
    int i;
    for (i = y; i < y+height; i++){
	glcd_putPixel(x, i, color, width);
    }
}

/* parameters
 *  x: the x coordination of the lower left corner
 *  y: the y coordinate of the lower left corner
 *  color: the color of the text
 *  c: the ascii character to draw
 * return
 *  how many horizontal pixels the character takes up
 * purpose
 *  draws a character on the glcd
 */
char glcd_putChar(int x, int y, char color, char c){
    char fontWidth = 19; // each character starts at index at 3*fontWidth + 1
    char fontHeight = 21;
    char currentByte,byte, width;
    int i,j,k;
    int startIndex = (c - 32)*(1+fontWidth*ceil((float)fontHeight/8));	// font starts at ascii 32
    k = startIndex + 1;
    width = Tahoma19x21[k - 1];
    for (i = 0; i<fontWidth; i++){
	for (j=0; j<fontHeight; ){
	    currentByte = Tahoma19x21[k];
	    for (byte = 0; byte < 8; byte++){
		if (((currentByte>>byte)&1) == 1){
		    glcd_putPixel(x+i, y+j, color, 1);
		}
		j++;
	    }
	    k++;
	}
    }
    return width;
}

/* parameters
 *  x: the start x coordinate
 *  y: the start y coordinate
 *  color: the pixel(s) color
 *  length: how many consecutive pixels to draw
 *  height: the height of the box
 * return
 *  none
 * purpose
 *  draws a pixel on the glcd
 */
void glcd_putPixel(int x, int y, char color, long length){
    glcd_writeVram((long)GLCD_WIDTH*y + x, color, length);
}

/* parameters
 *  x: the x coordination of the lower left corner
 *  y: the y coordinate of the lower left corner
 *  color: the color of the text
 *  c: the ascii characters to draw
 * return
 *  none
 * purpose
 *  draws characters on the glcd
 */
void glcd_putString(int x, int y, char color, char *c){
    char width;
    unsigned int i;
    int totalWidth = 0;
    for (i = 0; i < strlen(c); i++){
	if (totalWidth > 400 && c[i-1] == ' '){
	    x -= totalWidth;
	    totalWidth = 0;
	    y += 23;
	}
	width = glcd_putChar(x, y, color, c[i]);
	x += width+2;
	totalWidth += width+2;
    }
}

/* parameters
 *  offset: the number of the color to look up
 *  length: how many consecutive bytes to read
 * return
 *  none
 * purpose
 *  read entries from the glcd's lookup table
 */
int glcd_readLut1(char offset, char length){
    const int DEVICE = GRAPHIC;
    const char DUMMY = 0;
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

/* parameters
 *  length: how many consecutive bytes to read
 * return
 *  none
 * purpose
 *  read an entry from the touch controller's registers
 */
int glcd_readRegister(char offset){
    const int DEVICE = GRAPHIC;
    const char DUMMY = 0;
    int result = 0;

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

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  read entries from the touch controller's registers
 */
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

/* parameters
 *  addr: the address to start reading at
 *  length: how many consecutive bytes to read
 * return
 *  none
 * purpose
 *  read entries from the touch controller's vram
 */
int glcd_readVram(long addr, char length){
    const int DEVICE = GRAPHIC;
    const char DUMMY = 0;
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

/* parameters
 *  value: the value to write to the touch controller
 * return
 *  the value received from the touch controller
 * purpose
 *  performs an spi exchange with the touch controller
 */
char glcd_touchExchange(char value){
    char result = spi_exchange(TOUCH, value);
    __delay_us(400);
    return result;
}

/* parameters
 *  offset: the offset of the register to write to
 *  val: what to write
 * return
 *  none
 * purpose
 *  writes to a register of the touch controller
 */
void glcd_writeRegister(char offset, int val){
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

/* parameters
 *  addr: the address of the vram to write to
 *  lut_offset: the offset of the lookup table entry that should be written to the vram
 *  length: how many consecutive bytes to write to
 * return
 *  none
 * purpose
 *  writes to the vram of the touch controller
 */
void glcd_writeVram(long addr, char lut_offset, long length){
    const int DEVICE = GRAPHIC;
    long i;

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