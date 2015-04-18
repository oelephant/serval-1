/* 
 * File:   screen_home.h
 * Author: Tae
 *
 * Created on April 17, 2015, 2:18 PM
 */

#ifndef SCREEN_HOME_H
#define	SCREEN_HOME_H

#include "object.h"

void screen_home_draw(void);
void screen_home_handleTouch(void);
struct Button * screen_home_identifyTouch(struct TouchData *t);

#endif	/* SCREEN_HOME_H */

