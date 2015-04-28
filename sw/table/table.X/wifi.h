/* 
 * File:   wifi.h
 * Author: Ellen Fluehr
 *
 * Created on April 10, 2015, 6:16 PM
 */

#ifndef WIFI_H
#define	WIFI_H

#define OP_SEND_ORDER 0xa0
#define OP_SEND_ORDER_ACK 0xa1
#define OP_PAGE_SERVER 0xa2
#define OP_PAGE_SERVER_ACK 0xa3
#define OP_REQ_CATS 0xa4
#define OP_REQ_CATS_SCK 0xa5
#define OP_REQ_ITEMS 0xa6
#define OP_REQ_ITEMS_ACK 0xa7
#define OP_PUSH_MENU 0xa8

#define TABLE_ID 1

char wifi_exchange(char value);
void wifi_pageServer(void);
int wifi_read();
int wifi_transmit(char *message, int messageLength);
void wifi_sendOrder(void);

#endif	/* WIFI_H */

