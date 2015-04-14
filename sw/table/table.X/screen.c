#include <stddef.h>	// definition of NULL

#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"

struct Button button_newOrder = {50, 150, GREEN, 250, 180};
struct Button button_continueOrder = {340, 150, MAGENTA, 250, 180};
struct Button button_page = {300, 380, RED, 300, 80};

static int currentScreen = HOME;

void screen_drawBackground(void){
    glcd_writeVram(0, WHITE, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
}

void screen_drawButton(struct Button *b){
    glcd_putBox(b->x, b->y, b->color, b->width, b->height);
}

void screen_drawHome(void){
    screen_drawButton(&button_newOrder);
    screen_drawButton(&button_continueOrder);
    screen_drawButton(&button_page);
}

void screen_handleTouch(void){
    struct TouchData t;
    struct Button *b;

    t = glcd_getTouch();

    if ((t.pen&1) == 0){
	b = screen_identifyTouch(&t);

	if (b == &button_newOrder){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
	    screen_drawButton(&button_newOrder);
	}
	else if (b == &button_continueOrder){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
	    screen_drawButton(&button_continueOrder);
	}
	else if (b == &button_page){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
	    screen_drawButton(&button_page);
	}
    }
    /*
    if (t.x > 50 && t.x < 250 && t.y > 50 && t.y < 150){
	if (z==0){
	    glcd_putBox(50, 50, MAGENTA, 200, 100);
	    z = 1;
	}
	if ((t.pen&1) == 0){
	    glcd_writeVram(0, WHITE, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
	    glcd_putBox(50, 50, GRAY, 200, 100);
	}
    }
    else{
	if (z==1){
	    glcd_putBox(50, 50, GRAY, 200, 100);
	    z = 0;
	}
	glcd_putPixel(t.x, t.y, MAGENTA, 3);
    }*/
}

struct Button * screen_identifyTouch(struct TouchData *t){
    struct Button *b = &button_newOrder;
    if (t->x > b->x && t->x < b->x+b->width
	    && t->y > b->y && t->y < b->y+b->height){
	return b;
    }

    b = &button_continueOrder;
    if (t->x > b->x && t->x < b->x+b->width
	    && t->y > b->y && t->y < b->y+b->height){
	return b;
    }

    b = &button_page;
    if (t->x > b->x && t->x < b->x+b->width
	    && t->y > b->y && t->y < b->y+b->height){
	return b;
    }

    return NULL;
}
