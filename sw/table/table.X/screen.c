#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_home.h"
#include "screen_order.h"

struct Button button_page = {380, 390, CFF8700, 250, 80, "PAGE SERVER", WHITE};
struct Button button_return = {20, 390, GRAY, 150, 80, "BACK", WHITE};
static int currentScreen = HOME;

void screen_change(int screen){
    currentScreen = screen;
    screen_drawBackground();
    screen_draw();
}

void screen_draw(void){
    if (currentScreen == HOME){
        screen_home_draw();
    }
    else if (currentScreen == ORDER){
        screen_order_draw();
    }
}

void screen_drawBackground(void){
    glcd_writeVram(0, WHITE, (uint32_t)GLCD_WIDTH*GLCD_HEIGHT);
}

void screen_drawButton(struct Button *b){
    glcd_putBox(b->x, b->y, b->color, b->width, b->height);
    glcd_putString(b->x+20, b->y+b->height/2, b->textColor, b->text);
}

void screen_handleTouch(void){
    if (currentScreen == HOME){
        screen_home_handleTouch();
    }
    else if (currentScreen == ORDER){
        screen_order_handleTouch();
    }
}

bool screen_isWithinBounds(struct TouchData *t, struct Button *b){
    if (t->x > b->x && t->x < b->x+b->width
	    && t->y > b->y && t->y < b->y+b->height){
	return true;
    }
    return false;
}
