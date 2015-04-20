// TODO: actual prices
// TODO: remove blank items
// TODO: allow wrapping descriptions

#include "include.h"

#include "check.h"
#include "glcd.h"
#include "lut_colors.h"
#include "menu.h"
#include "object.h"
#include "screen.h"
#include "screen_items.h"
#include "wifi.h"

static uint8_t currentItemIndex;
static bool upEnabled;
static bool downEnabled;
static struct Menu *menu;
extern struct Check check;

struct Button button_item1 = {10, 10, C00FFAF, 500, 110, "", WHITE};
struct Button button_item2 = {10, 130, C00D7AF, 500, 110, "", WHITE};
struct Button button_item3 = {10, 250, C00AFAF, 500, 110, "", WHITE};
struct Button button_viewCheck = {410, 390, C0087FF, 220, 80, "REVIEW CHECK", WHITE};
extern struct Button button_up;
extern struct Button button_down;
extern struct Button button_page;
extern struct Button button_return;

void screen_items_clear(void){
    struct Button *b;
    b = &button_item1;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_item2;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_item3;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_up;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_down;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_page;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_viewCheck;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_return;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
}

void screen_items_clearEntries(void){
    screen_items_drawEntry(&button_item1, &menu->foods[currentItemIndex], C00FFAF);
    screen_items_drawEntry(&button_item2, &menu->foods[currentItemIndex+1], C00D7AF);
    screen_items_drawEntry(&button_item3, &menu->foods[currentItemIndex+2], C00AFAF);
}

void screen_items_draw(int category){
    if (category != -1){
	menu = menu_getRoot(category);
    }
    currentItemIndex = 0;
    screen_drawButton(&button_item1);
    screen_drawButton(&button_item2);
    screen_drawButton(&button_item3);
    screen_items_drawEntries();
    screen_drawButton(&button_page);
    screen_drawButton(&button_viewCheck);
    screen_drawButton(&button_return);
}

void screen_items_drawEntry(struct Button *b, struct Food *food, int color){
    glcd_putString(b->x+5, b->y+5, color, food->name);
    glcd_putChar(b->x+400, b->y+5, color, '$');
    glcd_putString(b->x+424, b->y+5, color, "xx.xx");
    glcd_putString(b->x+5, b->y+35, color, food->desc);
}

void screen_items_drawEntries(void){
    screen_items_drawEntry(&button_item1, &menu->foods[currentItemIndex], WHITE);
    screen_items_drawEntry(&button_item2, &menu->foods[currentItemIndex+1], WHITE);
    screen_items_drawEntry(&button_item3, &menu->foods[currentItemIndex+2], WHITE);

    screen_items_updateUpDown();
}

void screen_items_handleTouch(void){
    struct TouchData t;
    struct Button *b;

    t = glcd_getTouch();

    if ((t.pen&1) == 0){
	if (screen_isWithinBounds(&t, &button_item1)){
	    b = &button_item1;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_drawButton(b);
	    check_addItem(&menu->foods[currentItemIndex]);
	}
	else if (screen_isWithinBounds(&t, &button_item2)){
	    b = &button_item2;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_drawButton(b);
	    check_addItem(&menu->foods[currentItemIndex + 1]);
	}
	else if (screen_isWithinBounds(&t, &button_item3)){
	    b = &button_item3;
	    glcd_putBox(b->x, b->y, C005FAF, b->width, b->height);
	    screen_drawButton(b);
	    check_addItem(&menu->foods[currentItemIndex + 2]);
	}
	else if (screen_isWithinBounds(&t, &button_up) && upEnabled){
	    b = &button_up;
	    glcd_putBox(b->x, b->y, CAF5FFF, b->width, b->height);
	    screen_drawButton(b);

	    screen_items_clearEntries();
	    currentItemIndex += 3;
	    screen_items_drawEntries();
	}
	else if (screen_isWithinBounds(&t, &button_down) && downEnabled){
	    b = &button_down;
	    glcd_putBox(b->x, b->y, C87FF5F, b->width, b->height);
	    screen_drawButton(b);

	    if (currentItemIndex > 2){
		screen_items_clearEntries();
		currentItemIndex -=3;
		screen_items_drawEntries();
	    }
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
            screen_draw(CATEGORIES, -1);
	}
    }
}

void screen_items_updateUpDown(void){    
    if (menu->length > (currentItemIndex + 3) && upEnabled == false){
	// draw the up button
	screen_drawButton(&button_up);
	upEnabled = true;
    }
    else if (menu->length <= (currentItemIndex + 3) && upEnabled == true){
	// erase the up button
	glcd_putBox(button_up.x, button_up.y, BACKGROUND, button_up.width, button_up.height);
	upEnabled = false;
    }

    if (currentItemIndex > 0 && downEnabled == false){
	// draw the down button
	screen_drawButton(&button_down);
	downEnabled = true;
    }
    else if (currentItemIndex == 0 && downEnabled == true){
	// erase the down button
	glcd_putBox(button_down.x, button_down.y, BACKGROUND, button_down.width, button_down.height);
	downEnabled = false;
    }
}