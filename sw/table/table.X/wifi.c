/*  wifi.c
    created by Ellen Fluehr
 */

#include "include.h"

#include "check.h"
#include "menu.h"
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "uc_pins.h"
#include "spi_table.h"
#include "wifi.h"

extern struct Check check;
char wifi_resultPacket[0xfff];
int wifi_resultLength;
static int check_id = 1;

/* parameters
 *  value: data to send to the wifi
 * return
 *  data received from the wifi
 * purpose
 *  performs 8-bit SPI exchange with the wifi
 */
char wifi_exchange(char value){
    char result;

    spi_open(WIFI);
    spi_ss_wifi = 0;	// enable
    result = spi_exchange(WIFI, value);
    spi_ss_wifi = 1;	// disable

    return result;
}

void wifi_manageReceivedPacket(void){
    unsigned int i;
    unsigned char opcode = wifi_resultPacket[0];
    char *newPointer;
    char id[10];
    char category[10];
    char name[35];
    char desc[120];
    char price[10];
    switch (opcode){
	case (OP_REQ_ID_ACK):
	    check.id = wifi_resultPacket[1];
	    break;
	case (OP_REQ_ITEMS_ACK):
	    newPointer = wifi_resultPacket;
	    for (i = 0; i < wifi_resultLength && i < 0xfff; i++){
		newPointer = wifi_parse(id, newPointer, ';');
		newPointer = wifi_parse(category, newPointer, ';');
		newPointer = wifi_parse(name, newPointer, ';');
		newPointer = wifi_parse(desc, newPointer, ';');
		newPointer = wifi_parse(price, newPointer, ';');
		menu_addItem(1, name, desc, 0, 1.00);
	    }
	    break;
    }
    
//    if (wifi_resultPacket[0] == OP_REQ_ID_ACK){
//	check.id = wifi_resultPacket[1];
//    }
//    else if (wifi_resultPacket[0] == OP_REQ_ITEMS_ACK){
//	wifi_parse(dummy, wifi_resultPacket, ';');
//	wifi_parse(dummy, wifi_resultPacket, ';');
//	wifi_parse(name, wifi_resultPacket, ';');
//	wifi_parse(desc, wifi_resultPacket, ';');
//	wifi_parse(dummy, wifi_resultPacket, ';');
//	Nop();
//    }
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  submits a wifi request to page the server
 */
//char msg[280];
void wifi_pageServer(void){
    //int i;
    char msg[2];
    msg[0] = OP_PAGE_SERVER;
    msg[1] = TABLE_ID;
    wifi_transmit(msg, 2);
    //for (i = 0; i < 280; i++){
//	msg[i] = i%127 + 1;
    //}
    //wifi_transmit(msg, 280);
    //wifi_transmit("paging", strlen("paging"));
}

char * wifi_parse(char *buffer, char *string, char delimiter){
    unsigned int i;
    for (i = 0; i < 120 && string[i] != delimiter; i++){
	buffer[i] = string[i];
    }
    buffer[i] = '\0';
    return &string[i+1];
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  TODO: comment
 */
int wifi_read(){
    const char DUMMY = 0xff;
    int msgLength;
    char result;
    int i;

    for (i = 0; i < 0xff; i++){
	result = wifi_exchange(DUMMY);	// should be packet header, or keep checking until it is
	if (result == PACKET_HEADER){
	    break;
	}
    }
    if (result == PACKET_HEADER){
	msgLength = wifi_exchange(DUMMY);   // upper byte of message length
	msgLength = msgLength << 8;
	msgLength |= (0x0011 & wifi_exchange(DUMMY));  // lower byte of message length
    }
    else{
	return result;
    }
    wifi_resultLength = msgLength - 11;

    for (i = 0; i < 11; i++){
	wifi_exchange(DUMMY);	// 11 bytes of protocol;
    }
    for (i = 0; i < wifi_resultLength; i++){
	wifi_resultPacket[i] = wifi_exchange(DUMMY);	// actual message
    }
    wifi_exchange(DUMMY);   // checksum
    wifi_manageReceivedPacket();

    return 1;
}

void wifi_reqID(void){
    char msg[3];
    msg[0] = OP_REQ_ID;
    msg[1] = check_id;
    msg[2] = TABLE_ID;
    wifi_transmit(msg, 3);
    check_id++;
}

void wifi_reqItems(void){
    char msg[2];
    msg[0] = OP_REQ_ITEMS;
    msg[1] = TABLE_ID;
    wifi_transmit(msg, 2);
}

/* parameters
 *  message: the message to transmit
 *  messageLength: the size of the message to transmit
 * return
 *  none
 * purpose
 *  submits a wifi message to the controller
 */
int wifi_transmit(char *message, int messageLength){
    unsigned int i;
    char checksum, value;
    char address[4] = {0xc0, 0xa8, 0x01, 0x32};
    char destPort[2] = {0x26, 0x16};
    char sourcePort[2] = {0x00, 0x00};
    int packetLength;
    checksum = 0;
    packetLength = messageLength + 12;  // message bytes + 1x frame type, 1x frame id, 4x dest address, 2x dest port, 2x src port, 1x protocol, 1x transmit options

    // start delimiter
    wifi_exchange(0x7e);

    // number of bytes between this and checksum
    wifi_exchange(packetLength>>8);
    wifi_exchange(packetLength);

    // frame type
    value = 0x20;   // Wi-Fi Transmit Request IPv4
    wifi_exchange(value);
    checksum += value;

    // frame ID
    value = 0x00;
    wifi_exchange(value);
    checksum += value;

    // destination address
    for (i = 0; i < 4; i++){
        value = address[i];
        wifi_exchange(value);
        checksum += value;
    }

    // destination port
    for (i = 0; i < 2; i++){
        value = destPort[i];
        wifi_exchange(value);
        checksum += value;
    }

    // source port
    for (i = 0; i < 2; i++){
        value = sourcePort[i];
        wifi_exchange(value);
        checksum += value;
    }

    // protocol
    value = 0x01;   // TCP
    wifi_exchange(value);
    checksum += value;

    // transmit options
    value = 0x00;
    wifi_exchange(value);
    checksum += value;

    // message, max 1392 bytes
    for (i = 0; i < messageLength; i++){
        value = message[i];
        wifi_exchange(value);
        checksum += value;
    }

    // checksum
    wifi_exchange(0xff - checksum);

    return 1;
}

/* parameters
 *  none
 * return
 *  none
 * purpose
 *  submits a wifi check
 */
void wifi_sendOrder(void){
    unsigned int i,j,k;
    char msg[2+check.totalLength];
    msg[0] = OP_SEND_ORDER;
    msg[1] = TABLE_ID;
    k=0;
    for (i = 0; i < check.length; i++){
	for (j = 0; j < check.qty[i]; j++){
	    msg[k+2] = check.foods[i]->id;
	    k++;
	}
    }
    wifi_transmit(msg, 2+check.totalLength);
}