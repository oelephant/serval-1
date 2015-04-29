#include "common.h"
#include <xc.h>
#include "string.h"
#include "stdlib.h"
#include "lcd.h"
#include "wifi.h"

#define PAGE_SERVER 0xA2
#define CREATE_CHECK 0xA9

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
        case PAGE_SERVER:
            txSize = 0;
//            for (i = 0; i < strlen(cmdTest); i++) {
//                tx[i] = cmdTest[i];
//                txSize++;
//            }
            break;
        case CREATE_CHECK:
            txSize = 0;
            strcpy(header,"Get /php/checks.php?create_check=");
            sprintf(temp, "%d", rx[1]);
            lcdPrintStr(temp);
            strcat(header, temp);
            strcat(header, " HTTP/1.1\nHost: 68.227.174.223\n\n");
            
            txSize = strlen(header);
            for (i = 0; i < txSize; i++) {
                tx[i] = header[i];
            }
            rx[0] = 0;
            rxSize = 0;

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
    for (;i < rxSize; i++) {
        tx[txSize++] = rx[i];
    }

    rxSize = 0;
    rx[0] = 0;
    Nop();

    wifiTransmit();

}