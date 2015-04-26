#include "include.h"

#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_numberpad.h"
#include "wifi.h"

struct Button button_n1 = {180, 110, BTN_GRAD1_BG, 80, 80, "1", WHITE};
struct Button button_n2 = {270, 110, BTN_GRAD1_BG, 80, 80, "2", WHITE};
struct Button button_n3 = {360, 110, BTN_GRAD1_BG, 80, 80, "3", WHITE};
struct Button button_n4 = {180, 200, BTN_GRAD2_BG, 80, 80, "4", WHITE};
struct Button button_n5 = {270, 200, BTN_GRAD2_BG, 80, 80, "5", WHITE};
struct Button button_n6 = {360, 200, BTN_GRAD2_BG, 80, 80, "6", WHITE};
struct Button button_n7 = {180, 290, BTN_GRAD3_BG, 80, 80, "7", WHITE};
struct Button button_n8 = {270, 290, BTN_GRAD3_BG, 80, 80, "8", WHITE};
struct Button button_n9 = {360, 290, BTN_GRAD3_BG, 80, 80, "9", WHITE};
extern struct Button button_submit;
extern struct Button button_return;

static char code[5];
static int currentIndex = 0;

void screen_numberpad_clear(void){
    struct Button *b = &button_n1;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n2;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n3;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n4;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n5;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n6;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n7;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n8;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_n9;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_return;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_submit;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    glcd_putChar(100+20*0, 40, WHITE, code[0]);
    glcd_putChar(100+20*1, 40, WHITE, code[1]);
    glcd_putChar(100+20*2, 40, WHITE, code[2]);
    glcd_putChar(100+20*3, 40, WHITE, code[3]);
    currentIndex = 0;
}
void screen_numberpad_draw(void){
    screen_drawButton(&button_n1);
    screen_drawButton(&button_n2);
    screen_drawButton(&button_n3);
    screen_drawButton(&button_n4);
    screen_drawButton(&button_n5);
    screen_drawButton(&button_n6);
    screen_drawButton(&button_n7);
    screen_drawButton(&button_n8);
    screen_drawButton(&button_n9);
    screen_drawButton(&button_return);
    screen_drawButton(&button_submit);
}
void screen_numberpad_handleTouch(struct TouchData t){
    struct Button *b;
    bool isWithinNumber = false;

    if (screen_isWithinBounds(&t, &button_n1)){
	b = &button_n1;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '1';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n2)){
	b = &button_n2;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '2';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n3)){
	b = &button_n3;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '3';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n4)){
	b = &button_n4;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '4';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n5)){
	b = &button_n5;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '5';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n6)){
	b = &button_n6;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '6';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n7)){
	b = &button_n7;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '7';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n8)){
	b = &button_n8;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '8';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_n9)){
	b = &button_n9;
	glcd_putBox(b->x, b->y, BTN_GRAD_BG2, b->width, b->height);
	code[currentIndex] = '9';
	isWithinNumber = true;
    }
    else if (screen_isWithinBounds(&t, &button_return)){
	b = &button_return;
	glcd_putBox(b->x, b->y, CAF5FFF, b->width, b->height);
	screen_draw(HOME, -1);
	return;
    }
    else if (screen_isWithinBounds(&t, &button_submit)){
	b = &button_submit;
	glcd_putBox(b->x, b->y, CAF5FFF, b->width, b->height);
    }

    screen_drawButton(b);
    if (isWithinNumber == true){
	if (currentIndex > 3){
	    glcd_putChar(100+20*0, 40, WHITE, code[0]);
	    glcd_putChar(100+20*1, 40, WHITE, code[1]);
	    glcd_putChar(100+20*2, 40, WHITE, code[2]);
	    glcd_putChar(100+20*3, 40, WHITE, code[3]);
	    currentIndex = 0;
	    code[0] = code[4];
	}
	glcd_putChar(100+20*currentIndex, 40, MAGENTA, code[currentIndex]);
	currentIndex++;
    }
}
