/* 
 * File:   screen.h
 * Author: Tae
 *
 * Created on April 13, 2015, 9:35 PM
 */

#ifndef SCREEN_H
#define	SCREEN_H

#include "include.h"

#include "object.h"

// bg's
#define BACKGROUND WHITE
#define BTN_DOWN_BG C5FFF00
#define BTN_GRAD1_BG C00FFAF
#define BTN_GRAD2_BG C00D7AF
#define BTN_GRAD3_BG C00AFAF
#define BTN_GRAD4_BG C00FFAF
#define BTN_PAGE_BG CFF8700
#define BTN_RETURN_BG CAF5FAF
#define BTN_SUBMIT_BG C0087FF
#define BTN_UP_BG CFF5F5F
#define BTN_VIEWCHECK_BG C0087FF

// shader bg's
#define BTN_DOWN_BG2 C87FF5F
#define BTN_GRAD_BG2 C005FAF
#define BTN_PAGE_BG2 CD75F00
#define BTN_RETURN_BG2 CAF5FFF
#define BTN_SUBMIT_BG2 C005FFF
#define BTN_UP_BG2 CAF5FFF
#define BTN_VIEWCHECK_BG2 C005FFF

typedef enum {  HOME,
                CATEGORIES,
                ITEMS,
                CHECK,
                NUMBERPAD} Screens;

extern struct Button button_up;
extern struct Button button_down;
extern struct Button button_page;
extern struct Button button_return;
extern struct Button button_viewCheck;

void screen_draw(int screen, int parameter);
void screen_drawBackground(void);
void screen_drawButton(struct Button *b);
void screen_handleTouch(void);
bool screen_isWithinBounds(struct TouchData *t, struct Button *b);

#endif	/* SCREEN_H */

