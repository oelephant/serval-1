/* 
 * File:   wifi.h
 * Author: Ellen Fluehr
 *
 * Created on April 10, 2015, 6:16 PM
 */

#ifndef WIFI_H
#define	WIFI_H

// Xbee protocol
#define PACKET_HEADER 0x7e

// application opcodes
#define OP_SEND_ORDER 0xa0
#define OP_SEND_ORDER_ACK 0xa1
#define OP_PAGE_SERVER 0xa2
#define OP_PAGE_SERVER_ACK 0xa3
#define OP_REQ_CATS 0xa4
#define OP_REQ_CATS_ACK 0xa5
#define OP_REQ_ITEMS 0xa6
#define OP_REQ_ITEMS_ACK 0xa7
#define OP_PUSH_MENU 0xa8

#define OP_REQ_ID 0xa9
#define OP_REQ_ID_ACK 0xaa

#define TABLE_ID 1

extern char wifi_resultPacket[0xfff];
extern int wifi_resultLength;

char wifi_exchange(char value);
void wifi_manageReceivedPacket(void);
void wifi_pageServer(void);
char * wifi_parse(char *buffer, char *string, char delimiter);
int wifi_read();
void wifi_reqID(void);
void wifi_reqItems(void);
int wifi_transmit(char *message, int messageLength);
void wifi_sendOrder(void);

#endif	/* WIFI_H */

