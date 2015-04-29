#ifndef ETHERNET_H
#define	ETHERNET_H

/* macro for initializing ethernet spi slave select */
#define ETH_SS LATDbits.LATD5
#define __eth_ss_init() \
    TRISDbits.TRISD5 = 0; \
    ETH_SS = 1

void ethernetInit();
void ethernetOpen();
char ethernetClient();

#endif	/* ETHERNET_H */

