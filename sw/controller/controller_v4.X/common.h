#ifndef COMMON_H
#define	COMMON_H

/* macro for initializing clock division */
#define FOSCPLL_32 0
#define FOSCPLL_16 1
#define FOSCPLL_8 2
#define FOSCPLL_4 3
#define __fosc_init() CLKDIVbits.CPDIV = FOSCPLL_32

/* FCY = (FOSC / 2) */
#define FCY 16000000

#define RX_BUF_SIZE 8192
#define TX_BUF_SIZE 8192
__eds__ extern char rx[RX_BUF_SIZE] __attribute__((space(eds)));
__eds__ extern char tx[TX_BUF_SIZE] __attribute__((space(eds)));
extern int rxSize;
extern int txSize;

#include <stdio.h>

#endif	/* COMMON_H */

