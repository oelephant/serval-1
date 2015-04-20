#include "include.h"

#include "menu.h"
#include "object.h"

static struct Menu menu1, menu2, menu3, menu4;

void menu_addItem(int id, char* name, char* desc, int category, float price){
    struct Menu *m;
    if (category == ENTREE){
	m = &menu1;
    }
    else if (category == APPETIZER){
	m = &menu2;
    }
    else if (category == DRINK){
	m = &menu3;
    }
    else{
	m = &menu4;
    }

    if (m->length >= 20){
	return;
    }

    m->foods[m->length].id = id;
    m->foods[m->length].name = name;
    m->foods[m->length].desc = desc;
    m->foods[m->length].category = category;
    m->foods[m->length].price = price;
    m->length++;
}

struct Menu * menu_getRoot(int category){
    if (category == ENTREE){
	return &menu1;
    }
    else if (category == APPETIZER){
	return &menu2;
    }
    else if (category == DRINK){
	return &menu3;
    }
    else{
	return &menu4;
    }
}

void menu_init(void){
    menu_addItem(1, "Toast", "is toasty", ENTREE, 11.12);
    menu_addItem(2, "Eggs", "are eggy", ENTREE, 23.11);
    menu_addItem(3, "Jam", "it's very jammy", ENTREE, 2.11);
    menu_addItem(5, "Spider Goulash", "ndesc", ENTREE, 99.99);
    menu_addItem(6, "Ramen", "ndesc", ENTREE, 99.99);
    menu_addItem(7, "Shrimp Scampi", "ndesc", ENTREE, 99.99);
    menu_addItem(13, "Oni Giri", "ndesc", ENTREE, 99.99);
    menu_addItem(4, "Doughnuts", "are frosted", DESSERT, 99.99);
    menu_addItem(9, "Napoleonas Torte", "ndesc", DESSERT, 99.99);
    menu_addItem(12, "Pudding", "ndesc", DESSERT, 99.99);
    menu_addItem(10, "Shasta", "ndesc", DRINK, 99.99);
    menu_addItem(11, "Cactus juice", "It's the quenchiest", DRINK, 99.99);
    menu_addItem(8, "Spring Roll", "ndesc", APPETIZER, 99.99);
}