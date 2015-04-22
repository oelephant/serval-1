/* 
 * File:   check.h
 * Author: Tae
 *
 * Created on April 19, 2015, 7:39 PM
 */

#ifndef CHECK_H
#define	CHECK_H

#include "object.h"

#define MAX_CHECK_LENGTH 20

struct Check {   struct Food *foods[MAX_CHECK_LENGTH]; uint8_t qty[MAX_CHECK_LENGTH]; uint8_t length; float totalPrice;};

extern struct Check check;

void check_addItem(struct Food *food);
void check_reset(void);
void check_removeItem(struct Food *food);

#endif	/* CHECK_H */

