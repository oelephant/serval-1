/* 
 * File:   menu.h
 * Author: Ellen Fluehr
 *
 * Created on April 18, 2015, 8:47 PM
 */

#ifndef MENU_H
#define	MENU_H

#include "object.h"

void menu_addItem(int id, char* name, char* desc, int category, float price);
struct Menu * menu_getRoot(int category);
void menu_init(void);

#endif	/* MENU_H */

