#include "common.h"
#include <xc.h>
#include <spi.h>
#include "wifi.h"
#include "libpic30.h"
#include <string.h>

void wifiOpen()
{
     // ensure spi channel is closed before opening
    CloseSPI1();

    __delay_ms(100);

    // open spi channel for wifi
    OpenSPI1((
                ENABLE_SCK_PIN &            // enable spi clock
                ENABLE_SDO_PIN &            // enable spi mosi
                SPI_MODE8_ON &              // 8-bit transmission
                SPI_SMP_OFF &               // do not sample (manual only)
                SPI_CKE_OFF &               // high->low clock transition
                SLAVE_ENABLE_OFF &          // do not use slave select
                CLK_POL_ACTIVE_LOW &        // active low clock
                MASTER_ENABLE_ON &          // enable spi master mode
                SEC_PRESCAL_1_1 &           // secondary prescaler 2 to 1
                PRI_PRESCAL_1_1             // primary prescaler 1 to 1
            ),(
                0x00                        // no frames or enhanced buffer
            ),(
                SPI_ENABLE &                // enable spi
                SPI_IDLE_CON                // continue spi in idle mode
            ));
    __delay_ms(100);
}

unsigned char wifiExchange(unsigned char data)
{
    unsigned char ret;

    // enable wifi module
    WIFI_SS = 0;

    // write data into rx buffer
    WriteSPI1(data);

    // wait for transmit to complete
    while(!SPI1_Rx_Buf_Full);

    // disable wifi module
    WIFI_SS = 1;

    // read recieve buffer
    ret = ReadSPI1();
    Nop();
    return ret;
}

void wifiPurgeRxBuffer(int depth)
{
    char i = 0;
    char data[2];

    // loop until sequence of received characters is equal up to
    // purge resolution.
    do {

        // read two bytes back from spi
        data[0] = wifiExchange(0xFF);
        data[1] = wifiExchange(0xFF);

        // reset counter if bytes do not equal, else increment counter
        if (i > 0 && data[0] != data[1]) {
            i = 0;
        } else if (data[0] == data[1]) {
            i++;
        }
        Nop();
    } while (i < depth);
}

char wifiTransmit()
{
    unsigned char dip[] = {192, 168, 1, 40};   // 192.168.1.40
    unsigned char dport[] = {0x26, 0x16};      // 9750
    unsigned char sport[] = {0x00, 0x00};      // 0

    unsigned char checksum = 0xFF;
    unsigned int len = 0;
    unsigned int i = 0;

    // get length of data and add 12 bytes of overhead
    if (txSize > 0) {
        len = txSize + 12;
    } else {
        return -1;
    }

    // send start delimiter
    wifiExchange(0x7E);

    // send length frame data
    wifiExchange(len >> 8);
    wifiExchange(len);

    // send frame identifier
    wifiExchange(0x20);
    checksum -= 0x20;

    // send frame id
    wifiExchange(0x0);
    checksum -= 0x00;

    // send destination ip
    for (i = 0; i < 4; i++) {
        wifiExchange(dip[i]);
        checksum-= dip[i];
    }

    // send destination port
    for (i = 0; i < 2; i++) {
        wifiExchange(dport[i]);
        checksum -= dport[i];
    }

    // send source port
    for (i = 0; i < 2; i++) {
        wifiExchange(sport[i]);
        checksum -= sport[i];
    }

    // send protocol
    wifiExchange(0x01);
    checksum -= 0x01;

    // send transmit options
    wifiExchange(0x00);
    checksum -= 0x00;

    // send data
    for (i = 0; i < txSize; i++) {
        wifiExchange(tx[i]);
        checksum -= tx[i];
        tx[i] = 0;
    }
    txSize = 0;

    // send checksum
    wifiExchange(checksum);

    // return checksum
    return checksum;
}

char wifiRxAvailable()
{
    return !(WIFI_ATTN);
}

char wifiRead()
{
    unsigned int len;
    unsigned char fid = 0xFF;
    unsigned char checksum = 0xFF;
    unsigned int i = 0;

    Nop();

    // read receive buffer until new packet is found
    while(wifiExchange(0xFF) != 0x7E); // 0x7E
    Nop();
    // the next two bytes give us the length of the packet
    len = wifiExchange(0xFF);
    len <<= 8;
    len |= wifiExchange(0xFF);

    // the next byte indicates the type of frame being received
    fid = wifiExchange(0xFF);
    checksum -= fid;

    if (fid == 0xB0) {
        for (i = 0; i < 10; i++)
            wifiExchange(0xFF);

        Nop();

        rxSize = len - 11;
        for (i = 0; i < rxSize; i++) {
            rx[i] = wifiExchange(0xFF);
            checksum -= rx[i];
        }

        Nop();

        // read checksum
        if (wifiExchange(0xFF) != checksum) {
            fid = -1;
        }
    }

    // purge rx buffer of any  remaining data
    wifiPurgeRxBuffer(3);
    Nop();
    // return frame identifier for referance or -1 for error
    return fid;
}