/* 
 * File:   screen_numberpad.h
 * Author: Ellen Fluehr
 *
 * Created on April 23, 2015, 9:20 PM
 */

#ifndef SCREEN_NUMBERPAD_H
#define	SCREEN_NUMBERPAD_H

#include "object.h"

void screen_numberpad_clear(void);
void screen_numberpad_draw(void);
void screen_numberpad_handleTouch(struct TouchData t);

#endif	/* SCREEN_NUMBERPAD_H */

