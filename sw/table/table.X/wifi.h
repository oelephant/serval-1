/* 
 * File:   wifi.h
 * Author: Tae
 *
 * Created on April 10, 2015, 6:16 PM
 */

#ifndef WIFI_H
#define	WIFI_H

uint8_t wifi_exchange(uint8_t value);
int wifi_read();
int wifi_transmit(char *message, uint8_t messageLength);

#endif	/* WIFI_H */

