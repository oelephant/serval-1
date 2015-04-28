/*  screen.c
    created by Ellen Fluehr
 */

#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_categories.h"
#include "screen_check.h"
#include "screen_home.h"
#include "screen_items.h"
#include "screen_numberpad.h"
#include "wifi.h"

struct Button button_up = {550, 10, BTN_UP_BG, 80, 170, "(U)", WHITE};
struct Button button_down = {550, 190, BTN_DOWN_BG, 80, 170, "(D)", WHITE};
struct Button button_page = {180, 390, BTN_PAGE_BG, 220, 80, "PAGE SERVER", WHITE};
struct Button button_return = {10, 390, BTN_RETURN_BG, 160, 80, "BACK", WHITE};
struct Button button_viewCheck = {410, 390, BTN_VIEWCHECK_BG, 220, 80, "REVIEW CHECK", WHITE};
static int currentScreen = HOME;

/* parameters
 *  screen: which screen to draw
 *  parameter: value to pass to the screen
 * return
 *  none
 * purpose
 *  places screen-specific objects
 */
void screen_draw(int screen, int parameter){
    if (currentScreen == HOME){
        screen_home_clear();
    }
    else if (currentScreen == CATEGORIES){
	screen_categories_clear();
    }
    else if (currentScreen == ITEMS){
	screen_items_clear();
    }
    else if (currentScreen == CHECK){
	screen_check_clear();
    }
    else if (currentScreen == NUMBERPAD){
	screen_numberpad_clear();
    }

    if (screen == HOME){
        screen_home_draw();
    }
    else if (screen == CATEGORIES){
        screen_categories_draw();
    }
    else if (screen == ITEMS){
        screen_items_draw(parameter);
    }
    else if (screen == CHECK){
        screen_check_draw();
    }
    else if (screen == NUMBERPAD){
        screen_numberpad_draw();
    }

    currentScreen = screen;
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  fills the entire screen with the background color
 */
void screen_drawBackground(void){
    glcd_writeVram(0, BACKGROUND, (long)GLCD_WIDTH*GLCD_HEIGHT);
    glcd_putBox(20, 375, CAFFFFF, GLCD_WIDTH-40, 2);
    glcd_putBox(20, 377, CAFAFFF, GLCD_WIDTH-40, 2);
}

/* parameters
 *  b: button to draw
 * return
 *  none
 * purpose
 *  draws a button on the screen using the passed struct's data
 */
void screen_drawButton(struct Button *b){
    int horizontalOffset = 20;	// TODO: centering algorithm
    glcd_putBox(b->x, b->y, b->color, b->width, b->height);
    glcd_putString(b->x+horizontalOffset, b->y+b->height/2, b->textColor, b->text);
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  responds to touch signals on screen-specific objects
 */
void screen_handleTouch(void){
    struct TouchData t;
    struct Button *b;
    t = glcd_getTouch();

    if ((t.pen&1) != 0){
	return;
    }

    if (screen_isWithinBounds(&t, &button_page)){
	b = &button_page;
	glcd_putBox(b->x, b->y, BTN_PAGE_BG2, b->width, b->height);
	screen_drawButton(b);
	wifi_pageServer();
	return;
    }

    if (currentScreen == HOME){
        screen_home_handleTouch(t);
    }
    else if (currentScreen == CATEGORIES){
        screen_categories_handleTouch(t);
    }
    else if (currentScreen == ITEMS){
        screen_items_handleTouch(t);
    }
    else if (currentScreen == CHECK){
        screen_check_handleTouch(t);
    }
    else if (currentScreen == NUMBERPAD){
        screen_numberpad_handleTouch(t);
    }
}

/* parameters
 *  t: touch data
 *  b: button to check
 * return
 *  whether the touch is within the passed button
 * purpose
 *  determines if the touch data is within the bounds of the passed button
 */
bool screen_isWithinBounds(struct TouchData *t, struct Button *b){
    if (t->x > b->x && t->x < b->x+b->width
	    && t->y > b->y && t->y < b->y+b->height){
	return true;
    }
    return false;
}
