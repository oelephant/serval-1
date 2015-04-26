/* 
 * File:   screen_items.h
 * Author: Tae
 *
 * Created on April 18, 2015, 6:43 PM
 */

#ifndef SCREEN_ITEMS_H
#define	SCREEN_ITEMS_H

#include "object.h"

void screen_items_clear(void);
void screen_items_clearEntries(void);
void screen_items_draw(int category);
void screen_items_drawEntry(struct Button *b, int index, int color);
void screen_items_drawEntries(void);
void screen_items_handleTouch(struct TouchData t);
void screen_items_updateUpDown(void);

#endif	/* SCREEN_ITEMS_H */

