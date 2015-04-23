#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_categories.h"
#include "screen_check.h"
#include "screen_home.h"
#include "screen_items.h"

struct Button button_up = {550, 10, CFF5F5F, 80, 170, "(U)", WHITE};
struct Button button_down = {550, 190, C5FFF00, 80, 170, "(D)", WHITE};
struct Button button_page = {180, 390, CFF8700, 220, 80, "PAGE SERVER", WHITE};
struct Button button_return = {10, 390, CAF5FAF, 160, 80, "BACK", WHITE};
struct Button button_viewCheck = {410, 390, C0087FF, 220, 80, "REVIEW CHECK", WHITE};
static int currentScreen = HOME;

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

    currentScreen = screen;
}

void screen_drawBackground(void){
    glcd_writeVram(0, BACKGROUND, (long)GLCD_WIDTH*GLCD_HEIGHT);
    glcd_putBox(20, 375, CAFFFFF, GLCD_WIDTH-40, 2);
    glcd_putBox(20, 377, CAFAFFF, GLCD_WIDTH-40, 2);
}

void screen_drawButton(struct Button *b){
    int horizontalOffset = 20;	// TODO: centering algorithm
    glcd_putBox(b->x, b->y, b->color, b->width, b->height);
    glcd_putString(b->x+horizontalOffset, b->y+b->height/2, b->textColor, b->text);
}

void screen_handleTouch(void){
    if (currentScreen == HOME){
        screen_home_handleTouch();
    }
    else if (currentScreen == CATEGORIES){
        screen_categories_handleTouch();
    }
    else if (currentScreen == ITEMS){
        screen_items_handleTouch();
    }
    else if (currentScreen == CHECK){
        screen_check_handleTouch();
    }
}

bool screen_isWithinBounds(struct TouchData *t, struct Button *b){
    if (t->x > b->x && t->x < b->x+b->width
	    && t->y > b->y && t->y < b->y+b->height){
	return true;
    }
    return false;
}
