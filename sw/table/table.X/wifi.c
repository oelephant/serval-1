#include <xc.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include <libpic30.h>   // for delays
#include "uc_pins.h"
#include "spi_table.h"
#include "wifi.h"

int wifi_command(){
    uint8_t result[30];
    int i;
    uint8_t sum = 0;
    uint8_t value;

    result[0] = wifi_exchange(0x7E);	// start delimiter

    result[1] = wifi_exchange(0x00);
    result[2] = wifi_exchange(0x04);	// number of bytes between this and checksum

    value = 0x08;
    result[3] = wifi_exchange(value);	// API frame identifier
    sum += value;

    value = 0x01;
    result[4] = wifi_exchange(value);	// frame ID
    sum += value;

    value = 'I';
    result[5] = wifi_exchange(value);
    sum += value;
    value = 'D';
    result[6] = wifi_exchange(value);	// command name
    sum += value;

    result[7] = wifi_exchange(0xFF - sum);	// checksum

    return 1;
}

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