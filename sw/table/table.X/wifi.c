#include "include.h"

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "uc_pins.h"
#include "spi_table.h"
#include "wifi.h"

uint8_t wifi_exchange(uint8_t value){
    uint8_t result;

    spi_open(WIFI);
    spi_ss_wifi = 0;	// enable
    result = spi_exchange(WIFI, value);
    spi_ss_wifi = 1;	// disable

    return result;
}

int wifi_read(){
    const uint8_t DUMMY = 0xff;
    uint8_t result[30];
    int i;

    for (i = 0; i < 30; i++){
	result[i] = wifi_exchange(DUMMY);
    }
    return 1;
}

int wifi_transmit(char *message){
    uint8_t i, checksum, value, messageLength;
    uint8_t address[4] = {0xc0, 0xa8, 0x01, 0x32};
    uint8_t destPort[2] = {0x26, 0x16};
    uint8_t sourcePort[2] = {0x00, 0x00};
    uint16_t packetLength;
    messageLength = strlen(message);
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

    // message
    for (i = 0; i < messageLength; i++){
        value = message[i];
        wifi_exchange(value);
        checksum += value;
    }

    // checksum
    wifi_exchange(0xff - checksum);

    return 1;
}