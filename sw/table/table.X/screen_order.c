#include "include.h"

#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_order.h"
#include "wifi.h"

struct Button button_cat1 = {20, 20, C00FFAF, 200, 80, "CAT1", WHITE};
struct Button button_cat2 = {20, 110, C00D7AF, 200, 80, "CAT2", WHITE};
struct Button button_cat3 = {20, 200, C00AFAF, 200, 80, "CAT3", WHITE};
extern struct Button button_page;
extern struct Button button_return;

void screen_order_draw(void){
    screen_drawButton(&button_cat1);
    screen_drawButton(&button_cat2);
    screen_drawButton(&button_cat3);
    screen_drawButton(&button_page);
    screen_drawButton(&button_return);
}

void screen_order_handleTouch(void){
    struct TouchData t;
    struct Button *b;

    t = glcd_getTouch();

    if ((t.pen&1) == 0){
	b = screen_order_identifyTouch(&t);

	if (b == &button_cat1){
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_drawButton(b);
	}
	else if (b == &button_cat2){
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_drawButton(b);
	}
	else if (b == &button_cat3){
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_drawButton(b);
	}
	else if (b == &button_page){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
	    screen_drawButton(b);
            wifi_transmit("page");
	}
        else if (b == &button_return){
	    glcd_putBox(b->x, b->y, WHITE, b->width, b->height);
            screen_change(HOME);
	}
    }
}

struct Button * screen_order_identifyTouch(struct TouchData *t){
    struct Button *b = &button_cat1;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    b = &button_cat2;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    b = &button_cat3;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    b = &button_page;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    b = &button_return;
    if (screen_isWithinBounds(t, b)){
	return b;
    }

    return NULL;
}
