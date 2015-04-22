#include "include.h"

#include "check.h"
#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_home.h"
#include "wifi.h"
#include "menu.h"

struct Button button_newOrder = {65, 150, C00FF5F, 250, 180, "NEW ORDER", WHITE};
struct Button button_continueOrder = {325, 150, CAF005F, 250, 180, "CONTINUE ORDER", WHITE};
extern struct Button button_page;
extern struct Check check;

void screen_home_clear(void){
    struct Button *b = &button_newOrder;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_continueOrder;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
}

void screen_home_draw(void){
    screen_drawButton(&button_page);
    screen_drawButton(&button_newOrder);
    screen_drawButton(&button_continueOrder);
}

void screen_home_handleTouch(void){
    struct TouchData t;
    struct Button *b;

    t = glcd_getTouch();

    if ((t.pen&1) == 0){
	if (screen_isWithinBounds(&t, &button_newOrder)){
	    b = &button_newOrder;
	    glcd_putBox(b->x, b->y, C5FAF5F, b->width, b->height);
	    check_reset();
            screen_draw(CATEGORIES, -1);
	}
	else if (screen_isWithinBounds(&t, &button_continueOrder)){
	    b = &button_continueOrder;
	    glcd_putBox(b->x, b->y, C5F005F, b->width, b->height);
            screen_draw(CATEGORIES, -1);
	}
	else if (screen_isWithinBounds(&t, &button_page)){
	    b = &button_page;
	    glcd_putBox(b->x, b->y, CD75F00, b->width, b->height);
	    screen_drawButton(b);
            wifi_transmit("page");
	}
    }
}
