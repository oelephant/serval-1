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

typedef enum {  HOME,
                ORDER} Screens;

extern struct Button button_page;
extern struct Button button_return;

void screen_change(int screen);
void screen_draw(void);
void screen_drawBackground(void);
void screen_drawButton(struct Button *b);
void screen_handleTouch(void);
bool screen_isWithinBounds(struct TouchData *t, struct Button *b);

#endif	/* SCREEN_H */

