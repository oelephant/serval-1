#include "common.h"
#include <xc.h>
#include "string.h"
#include "stdlib.h"
#include "lcd.h"
#include "wifi.h"

#define SUBMIT_ORDER 0xA0
#define PAGE_SERVER 0xA2
#define REQUEST_ITEMS 0xA6
#define REQUEST_ITEMS_ACK 0xa7
#define CREATE_CHECK 0xA9

static char transmitOpcode;

void getPrintTasks(void){
    char header[512];
    char temp[100];
    int i = 0;

    txSize = 0;
    strcpy(header,"Get /php/checks.php?checks_to_print");
    strcat(header, " HTTP/1.1\nHost: 192.168.1.123\n\n");

    txSize = strlen(header);
    for (i = 0; i < txSize; i++) {
	tx[i] = header[i];
    }
    rx[0] = 0;
    rxSize = 0;

    transmitOpcode = 0;
    Nop();
}

void wifiFromTable()
{
    unsigned char opcode;
    char header[512];
    char temp[100];
    int i = 0;

    // TODO check buffers
    Nop();
    if (rxSize == 0)
        return;

    opcode = rx[0];

    switch(opcode) {
        case SUBMIT_ORDER:
            txSize = 0;
            strcpy(header,"Get /php/checks.php?insert_order={\"table_id\":");
            sprintf(temp, "%d", rx[1]);
            strcat(header, temp);
            strcat(header, ",\"items\":[");
	    for (i = 2; i < rxSize; i++){
		sprintf(temp, "%d", rx[i]);
		strcat(header, temp);
		if (i != rxSize - 1){
		    strcat(header, ",");
		}
	    }
	    strcat(header, "]}");
            strcat(header, " HTTP/1.1\nHost: 192.168.1.123\n\n");

            txSize = strlen(header);
            for (i = 0; i < txSize; i++) {
                tx[i] = header[i];
            }
            rx[0] = 0;
            rxSize = 0;

	    transmitOpcode = 0;
            Nop();

            break;
        case PAGE_SERVER:
            txSize = 0;
            strcpy(header,"Get /php/staff.php?page_server=");
            sprintf(temp, "%d", rx[1]);
            strcat(header, temp);
            strcat(header, " HTTP/1.1\nHost: 192.168.1.123\n\n");

            txSize = strlen(header);
            for (i = 0; i < txSize; i++) {
                tx[i] = header[i];
            }
            rx[0] = 0;
            rxSize = 0;

	    transmitOpcode = 0;
            Nop();

            break;
	case REQUEST_ITEMS:
            txSize = 0;
            strcpy(header,"Get /php/menus.php?get_items");
            strcat(header, " HTTP/1.1\nHost: 192.168.1.123\n\n");

            txSize = strlen(header);
            for (i = 0; i < txSize; i++) {
                tx[i] = header[i];
            }
            rx[0] = 0;
            rxSize = 0;

	    transmitOpcode = REQUEST_ITEMS_ACK;
            Nop();
	    break;
        case CREATE_CHECK:
            txSize = 0;
            strcpy(header,"Get /php/checks.php?create_check=");
            sprintf(temp, "%d", rx[1]);
            strcat(header, temp);
            strcat(header, "&table_id=");
            sprintf(temp, "%d", rx[2]);
            strcat(header, temp);
            strcat(header, " HTTP/1.1\nHost: 192.168.1.123\n\n");
            
            txSize = strlen(header);
            for (i = 0; i < txSize; i++) {
                tx[i] = header[i];
            }
            rx[0] = 0;
            rxSize = 0;

	    transmitOpcode = 0;
            Nop();

            break;
    }
}

void ethernetFromServer()
{
    unsigned char temp[4];
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < rxSize; i++) {
        if (rx[i] == 0x0D && rx[i+1] == 0x0A && rx[i+2] == 0x0D && rx[i+3] == 0x0A) {
            i += 4;
            break;
        }
    }
    txSize = 0;
    tx[txSize++] = transmitOpcode;
    for (;i < rxSize; i++) {
        tx[txSize++] = rx[i];
    }

    rxSize = 0;
    rx[0] = 0;
    Nop();
    return;
    wifiTransmit();

}