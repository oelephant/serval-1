/* 
 * File:   screen_check.h
 * Author: Tae
 *
 * Created on April 19, 2015, 7:57 PM
 */

#ifndef SCREEN_CHECK_H
#define	SCREEN_CHECK_H

#include "object.h"

void asdf(void);
void screen_check_clear(void);
void screen_check_clearEntries(void);
void screen_check_draw(void);
void screen_check_drawEntry(struct Button *b, int index, int color);
void screen_check_drawEntries(void);
void screen_check_drawTotalPrice(int color);
void screen_check_handleTouch(struct TouchData t);
void screen_check_preparePacket(void);
void screen_check_updateUpDown(void);

#endif	/* SCREEN_CHECK_H */

