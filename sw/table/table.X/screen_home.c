#include "include.h"

#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_home.h"
#include "wifi.h"

struct Button button_newOrder = {50, 150, C00FF5F, 250, 180, "NEW ORDER", WHITE};
struct Button button_continueOrder = {340, 150, CAF005F, 250, 180, "CONTINUE ORDER", WHITE};
extern struct Button button_page;

void screen_home_draw(void){
    screen_drawButton(&button_page);
    screen_drawButton(&button_newOrder);
    screen_drawButton(&button_continueOrder);
    glcd_putString(50,100,MAGENTA,"\"hadouken! @%&329.\"");
}

void screen_home_handleTouch(void){
    struct TouchData t;
    struct Button *b;

    t = glcd_getTouch();

    if ((t.pen&1) == 0){
	b = screen_home_identifyTouch(&t);

	if (b == &button_newOrder){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
            screen_change(ORDER);
	}
	else if (b == &button_continueOrder){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
            screen_change(ORDER);
	}
	else if (b == &button_page){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
	    screen_drawButton(b);
            wifi_transmit("page");
	}
    }
}

struct Button * screen_home_identifyTouch(struct TouchData *t){
    struct Button *b = &button_newOrder;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    b = &button_continueOrder;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    b = &button_page;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    return NULL;
}
