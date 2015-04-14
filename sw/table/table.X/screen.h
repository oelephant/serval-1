/* 
 * File:   screen.h
 * Author: Tae
 *
 * Created on April 13, 2015, 9:35 PM
 */

#ifndef SCREEN_H
#define	SCREEN_H

#include "object.h"

typedef enum {  HOME,
                SCR2} Screens;


void screen_drawButton(struct Button *b);
struct Button * screen_identifyTouch(struct TouchData *t);
void screen_drawBackground(void);
void screen_drawHome(void);
void screen_handleTouch(void);

#endif	/* SCREEN_H */

