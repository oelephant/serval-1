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
#include "user.h"
#include "wifi.h"

static unsigned int currentItemIndex;
static bool upEnabled;
static bool downEnabled;
static struct Menu *menu;
extern struct Check check;

struct Button button_item1 = {10, 10, BTN_GRAD1_BG, 530, 110, "", WHITE};
struct Button button_item2 = {10, 130, BTN_GRAD2_BG, 530, 110, "", WHITE};
struct Button button_item3 = {10, 250, BTN_GRAD3_BG, 530, 110, "", WHITE};
extern struct Button button_viewCheck;
extern struct Button button_up;
extern struct Button button_down;
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
    b = &button_viewCheck;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_return;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
}

void screen_items_clearEntries(void){
    screen_items_drawEntry(&button_item1, currentItemIndex, button_item1.color);
    screen_items_drawEntry(&button_item2, currentItemIndex+1, button_item2.color);
    screen_items_drawEntry(&button_item3, currentItemIndex+2, button_item3.color);
}

void screen_items_draw(int category){
    if (category != -1){
	menu = menu_getRoot(category);
    }
    currentItemIndex = 0;
    upEnabled = false;
    downEnabled = false;
    screen_drawButton(&button_item1);
    screen_drawButton(&button_item2);
    screen_drawButton(&button_item3);
    screen_items_drawEntries();
    screen_drawButton(&button_viewCheck);
    screen_drawButton(&button_return);
}

void screen_items_drawEntry(struct Button *b, int index, int color){
    char price[6];
    if (index < menu->length){
	ftoa(menu->foods[index].price, price, 2);
	glcd_putString(b->x+5, b->y+5, color, menu->foods[index].name);
	glcd_putChar(b->x+420, b->y+5, color, '$');
	glcd_putString(b->x+444, b->y+5, color, price);
	glcd_putString(b->x+5, b->y+35, color, menu->foods[index].desc);
    }
}

void screen_items_drawEntries(void){
    screen_items_drawEntry(&button_item1, currentItemIndex, WHITE);
    screen_items_drawEntry(&button_item2, currentItemIndex+1, WHITE);
    screen_items_drawEntry(&button_item3, currentItemIndex+2, WHITE);

    screen_items_updateUpDown();
}

void screen_items_handleTouch(struct TouchData t){
    struct Button *b;

    if (screen_isWithinBounds(&t, &button_item1)){
	b = &button_item1;
	screen_items_drawEntry(b, currentItemIndex, b->color);
	screen_items_drawEntry(b, currentItemIndex, WHITE);
	check_addItem(&menu->foods[currentItemIndex]);
	//glcd_putString(b->x+450, b->y+80, WHITE, "Added");
    }
    else if (screen_isWithinBounds(&t, &button_item2)){
	b = &button_item2;
	screen_items_drawEntry(b, currentItemIndex+1, b->color);
	screen_items_drawEntry(b, currentItemIndex+1, WHITE);
	check_addItem(&menu->foods[currentItemIndex + 1]);
	//glcd_putString(b->x+450, b->y+80, WHITE, "Added");
    }
    else if (screen_isWithinBounds(&t, &button_item3)){
	b = &button_item3;
	screen_items_drawEntry(b, currentItemIndex+2, b->color);
	screen_items_drawEntry(b, currentItemIndex+2, WHITE);
	check_addItem(&menu->foods[currentItemIndex + 2]);
	//glcd_putString(b->x+450, b->y+80, WHITE, "Added");
    }
    else if (screen_isWithinBounds(&t, &button_up) && upEnabled){
	b = &button_up;
	glcd_putBox(b->x, b->y, BTN_UP_BG2, b->width, b->height);
	screen_drawButton(b);

	screen_items_clearEntries();
	currentItemIndex += 3;
	screen_items_drawEntries();
    }
    else if (screen_isWithinBounds(&t, &button_down) && downEnabled){
	b = &button_down;
	glcd_putBox(b->x, b->y, BTN_DOWN_BG2, b->width, b->height);
	screen_drawButton(b);

	if (currentItemIndex > 2){
	    screen_items_clearEntries();
	    currentItemIndex -=3;
	    screen_items_drawEntries();
	}
    }
    else if (screen_isWithinBounds(&t, &button_viewCheck)){
	b = &button_viewCheck;
	glcd_putBox(b->x, b->y, BTN_VIEWCHECK_BG2, b->width, b->height);
	screen_drawButton(b);
	screen_draw(CHECK, -1);
    }
    else if (screen_isWithinBounds(&t, &button_return)){
	b = &button_return;
	glcd_putBox(b->x, b->y, BTN_RETURN_BG2, b->width, b->height);
	screen_draw(CATEGORIES, -1);
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