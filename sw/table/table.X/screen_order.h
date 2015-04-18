/* 
 * File:   screen_order.h
 * Author: Tae
 *
 * Created on April 17, 2015, 2:34 PM
 */

#ifndef SCREEN_ORDER_H
#define	SCREEN_ORDER_H

#include "object.h"

void screen_order_draw(void);
void screen_order_handleTouch(void);
struct Button * screen_order_identifyTouch(struct TouchData *t);

#endif	/* SCREEN_ORDER_H */

