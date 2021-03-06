/*  screen_check.c
    created by Ellen Fluehr
 */

// TODO: remove blank items

#include "include.h"

#include "check.h"
#include "glcd.h"
#include "lut_colors.h"
#include "object.h"
#include "screen.h"
#include "screen_check.h"
#include "stdio.h"
#include "user.h"
#include "wifi.h"

char packet[0xFF];
static unsigned int currentItemIndex;
static bool upEnabled;
static bool downEnabled;
extern struct Check check;

struct Button button_itemA = {10, 10, BTN_GRAD1_BG, 530, 80, "", WHITE};
struct Button button_itemB = {10, 100, BTN_GRAD2_BG, 530, 80, "", WHITE};
struct Button button_itemC = {10, 190, BTN_GRAD3_BG, 530, 80, "", WHITE};
struct Button button_itemD = {10, 280, BTN_GRAD4_BG, 530, 80, "", WHITE};
struct Button button_submit = {410, 390, BTN_SUBMIT_BG, 220, 80, "SUBMIT", WHITE};
extern struct Button button_up;
extern struct Button button_down;
extern struct Button button_return;

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  replaces screen-specific objects with background color
 */
void screen_check_clear(void){
    struct Button *b;
    b = &button_itemA;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_itemB;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_itemC;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_itemD;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_up;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_down;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_submit;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
    b = &button_return;
    glcd_putBox(b->x, b->y, BACKGROUND, b->width, b->height);
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  writes over text on item entries with button background color
 */
void screen_check_clearEntries(void){
    screen_check_drawEntry(&button_itemA, currentItemIndex, button_itemA.color);
    screen_check_drawEntry(&button_itemB, currentItemIndex+1, button_itemB.color);
    screen_check_drawEntry(&button_itemC, currentItemIndex+2, button_itemC.color);
    screen_check_drawEntry(&button_itemD, currentItemIndex+3, button_itemD.color);
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  places screen-specific objects
 */
void screen_check_draw(void){
    currentItemIndex = 0;
    upEnabled = false;
    downEnabled = false;
    screen_drawButton(&button_itemA);
    screen_drawButton(&button_itemB);
    screen_drawButton(&button_itemC);
    screen_drawButton(&button_itemD);
    screen_check_drawEntries();
    screen_drawButton(&button_submit);
    screen_check_drawTotalPrice(WHITE);
    screen_drawButton(&button_return);
}

/* parameters
 *  b: the button to draw on
 *  index: the index of the current item in the menu
 *  color: the text color to use
 * return
 *  none
 * purpose
 *  draws the text for the selected check entry
 */
void screen_check_drawEntry(struct Button *b, int index, int color){
    char buffer[6];
    if (index < check.length){
	glcd_putString(b->x+5, b->y+5, color, check.foods[index]->name);
	sprintf(buffer, "%d", check.qty[index]);
	//intToStr(check.qty[index], buffer, 0);
	glcd_putString(b->x+370, b->y+5, color, buffer);
	glcd_putChar(b->x+420, b->y+5, color, '$');
	sprintf(buffer, "%.2f", check.foods[index]->price);
	//ftoa(check.foods[index]->price, buffer, 2);
	glcd_putString(b->x+444, b->y+5, color, buffer);
    }
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  draws the text for check entries
 */
void screen_check_drawEntries(void){
    screen_check_drawEntry(&button_itemA, currentItemIndex, WHITE);
    screen_check_drawEntry(&button_itemB, currentItemIndex+1, WHITE);
    screen_check_drawEntry(&button_itemC, currentItemIndex+2, WHITE);
    screen_check_drawEntry(&button_itemD, currentItemIndex+3, WHITE);

    screen_check_updateUpDown();
}

/* parameters
 *  color: the text color to use
 * return
 *  none
 * purpose
 *  draws the text for the running total price
 */
void screen_check_drawTotalPrice(int color){
    char price[9];
    price[0] = '$';
    price[1] = ' ';
    struct Button *b = &button_submit;
    sprintf(price, "%.2f", check.totalPrice);
    //ftoa(check.totalPrice, price+2, 2);
    glcd_putString(b->x+110, b->y+b->height/2, color, price);
}

/* parameters
 *  t: last touch data
 * return
 *  none
 * purpose
 *  responds to touch signals on screen-specific objects
 */
void screen_check_handleTouch(struct TouchData t){
    struct Button *b;
    unsigned int index;

    if (screen_isWithinBounds(&t, &button_itemA)){
	b = &button_itemA;
	index = currentItemIndex;
	screen_check_drawTotalPrice(button_submit.color);
	if (check.qty[index] == 1){
	    screen_check_clearEntries();
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntries();
	}
	else{
	    screen_check_drawEntry(b, index, b->color);
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntry(b, index, WHITE);
	}
	screen_check_drawTotalPrice(WHITE);
	screen_check_updateUpDown();
    }
    else if (screen_isWithinBounds(&t, &button_itemB)){
	b = &button_itemB;
	index = currentItemIndex+1;
	screen_check_drawTotalPrice(button_submit.color);
	if (check.qty[index] == 1){
	    screen_check_clearEntries();
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntries();
	}
	else{
	    screen_check_drawEntry(b, index, b->color);
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntry(b, index, WHITE);
	}
	screen_check_drawTotalPrice(WHITE);
	screen_check_updateUpDown();
    }
    else if (screen_isWithinBounds(&t, &button_itemC)){
	b = &button_itemC;
	index = currentItemIndex+2;
	screen_check_drawTotalPrice(button_submit.color);
	if (check.qty[index] == 1){
	    screen_check_clearEntries();
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntries();
	}
	else{
	    screen_check_drawEntry(b, index, b->color);
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntry(b, index, WHITE);
	}
	screen_check_drawTotalPrice(WHITE);
	screen_check_updateUpDown();
    }
    else if (screen_isWithinBounds(&t, &button_itemD)){
	b = &button_itemD;
	index = currentItemIndex+3;
	screen_check_drawTotalPrice(button_submit.color);
	if (check.qty[index] == 1){
	    screen_check_clearEntries();
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntries();
	}
	else{
	    screen_check_drawEntry(b, index, b->color);
	    check_removeItem(check.foods[index]);
	    screen_check_drawEntry(b, index, WHITE);
	}
	screen_check_drawTotalPrice(WHITE);
	screen_check_updateUpDown();
    }
    else if (screen_isWithinBounds(&t, &button_up) && upEnabled){
	b = &button_up;
	glcd_putBox(b->x, b->y, BTN_UP_BG2, b->width, b->height);
	screen_drawButton(b);

	screen_check_clearEntries();
	currentItemIndex += 4;
	screen_check_drawEntries();
    }
    else if (screen_isWithinBounds(&t, &button_down) && downEnabled){
	b = &button_down;
	glcd_putBox(b->x, b->y, BTN_DOWN_BG2, b->width, b->height);
	screen_drawButton(b);

	if (currentItemIndex > 2){
	    screen_check_clearEntries();
	    currentItemIndex -=4;
	    screen_check_drawEntries();
	}
    }
    else if (screen_isWithinBounds(&t, &button_submit)){
	b = &button_submit;
	glcd_putBox(b->x, b->y, BTN_SUBMIT_BG2, b->width, b->height);
	screen_drawButton(b);
	screen_check_drawTotalPrice(WHITE);
	wifi_sendOrder();
	screen_check_clear();
	glcd_putString(200, 240, MAGENTA, "Order submitted!");
	check_reset();
	//screen_check_preparePacket();
	//wifi_sendOrder();
	__delay_ms(2000);
	screen_draw(HOME, 0);
    }
    else if (screen_isWithinBounds(&t, &button_return)){
	b = &button_return;
	glcd_putBox(b->x, b->y, BTN_RETURN_BG2, b->width, b->height);
	screen_draw(CATEGORIES, -1);
    }
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  assembles the packet with check contents to be sent through wifi
 */
void screen_check_preparePacket(void){
    unsigned int i;

    for (i = 0; i < check.length; i++) {
	packet[2*i] = check.foods[i]->id;
	packet[2*i+1] = check.qty[i];
    }

    packet[2*i] = '\0';
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  draws or erases the up/down scroll buttons depending on whether there are more items to view
 */
void screen_check_updateUpDown(void){
    if (check.length > (currentItemIndex + 4) && upEnabled == false){
	// draw the up button
	screen_drawButton(&button_up);
	upEnabled = true;
    }
    else if (check.length <= (currentItemIndex + 4) && upEnabled == true){
	// erase the up button
	glcd_putBox(button_up.x, button_up.y, BACKGROUND, button_up.width, button_up.height);
	upEnabled = false;
    }

    if (currentItemIndex > 0 && downEnabled == false){
	// draw the down button
	screen_drawButton(&button_down);
	downEnabled = true;
    }
    else if (currentItemIndex == 0 && downEnabled == true){
	// erase the down button
	glcd_putBox(button_down.x, button_down.y, BACKGROUND, button_down.width, button_down.height);
	downEnabled = false;
    }
}