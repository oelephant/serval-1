#ifndef WIFI_H
#define	WIFI_H

/* macro for initializing wifi spi slave select */
#define WIFI_SS LATGbits.LATG8
#define WIFI_ATTN PORTGbits.RG7
#define __wifi_ss_init() \
    TRISGbits.TRISG8 = 0; \
    TRISGbits.TRISG7 = 1; \
    ANSGbits.ANSG8 = 0; \
    ANSGbits.ANSG7 = 0; \
    WIFI_SS = 1

void wifiOpen();
unsigned char wifiExchange(unsigned char data);
void wifiPurgeRxBuffer(int depth);
char wifiTransmit();
unsigned char wifiExchange(unsigned char data);
char wifiRxAvailable();
char wifiRead();
unsigned char wifi_transmit(char *message);
#endif	/* WIFI_H */

