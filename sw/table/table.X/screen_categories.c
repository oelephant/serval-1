#include "include.h"

#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_categories.h"
#include "screen_items.h"
#include "wifi.h"

struct Button button_cat1 = {115, 110, C00FFAF, 200, 80, "ENTREES", WHITE};
struct Button button_cat2 = {325, 110, C00D7AF, 200, 80, "APPETIZERS", WHITE};
struct Button button_cat3 = {115, 200, C00AFAF, 200, 80, "DRINKS", WHITE};
struct Button button_cat4 = {325, 200, C00FFAF, 200, 80, "DESSERTS", WHITE};
extern struct Button button_viewCheck;
extern struct Button button_page;
extern struct Button button_return;

void screen_categories_clear(void){
    struct Button *b = &button_cat1;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_cat2;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_cat3;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_cat4;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_viewCheck;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_return;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
}

void screen_categories_draw(void){
    screen_drawButton(&button_cat1);
    screen_drawButton(&button_cat2);
    screen_drawButton(&button_cat3);
    screen_drawButton(&button_cat4);
    screen_drawButton(&button_viewCheck);
    screen_drawButton(&button_return);
}

void screen_categories_handleTouch(void){
    struct TouchData t;
    struct Button *b;

    t = glcd_getTouch();

    if ((t.pen&1) == 0){
	if (screen_isWithinBounds(&t, &button_cat1)){
	    b = &button_cat1;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_draw(ITEMS, ENTREE);
	}
	else if (screen_isWithinBounds(&t, &button_cat2)){
	    b = &button_cat2;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_draw(ITEMS, APPETIZER);
	}
	else if (screen_isWithinBounds(&t, &button_cat3)){
	    b = &button_cat3;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_draw(ITEMS, DRINK);
	}
	else if (screen_isWithinBounds(&t, &button_cat4)){
	    b = &button_cat4;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_draw(ITEMS, DESSERT);
	}
	else if (screen_isWithinBounds(&t, &button_viewCheck)){
	    b = &button_viewCheck;
	    glcd_putBox(b->x, b->y, C005FFF, b->width, b->height);
	    screen_drawButton(b);
	    screen_draw(CHECK, -1);
	}
	else if (screen_isWithinBounds(&t, &button_page)){
	    b = &button_page;
	    glcd_putBox(b->x, b->y, CD75F00, b->width, b->height);
	    screen_drawButton(b);
            wifi_transmit("page");
	}
        else if (screen_isWithinBounds(&t, &button_return)){
	    b = &button_return;
	    glcd_putBox(b->x, b->y, CAF5FFF, b->width, b->height);
            screen_draw(HOME, -1);
	}
    }
}