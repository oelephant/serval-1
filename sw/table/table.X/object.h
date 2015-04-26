/* 
 * File:   object.h
 * Author: Tae
 *
 * Created on April 13, 2015, 10:13 PM
 */

#ifndef OBJECT_H
#define	OBJECT_H

typedef enum {  ENTREE,
                APPETIZER,
                DRINK,
                DESSERT} Categories;

struct Button { int x; int y;
                char color;
                int width; int height;
                char *text; char textColor;};

struct Food {   int id; char* name; char* desc; int category; float price;};

struct Menu {   struct Food foods[20]; int length;};

#endif	/* OBJECT_H */

