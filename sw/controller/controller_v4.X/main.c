#include "common.h"
#include "config.h"
#include <xc.h>
#include <libpic30.h>
#include <PPS.h>
#include "ethernet.h"
#include "wifi.h"
#include "lcd.h"
#include <string.h>

__eds__ char rx[RX_BUF_SIZE] __attribute__((eds));
int rxSize = 0;
__eds__ char tx[TX_BUF_SIZE] __attribute__((eds));
int txSize = 0;

void init()
{
    __fosc_init();
    __eth_ss_init();
    __wifi_ss_init();

    iPPSInput(IN_FN_PPS_SDI1, IN_PIN_PPS_RP4);         // MISO <- RP4
    iPPSOutput(OUT_PIN_PPS_RP23, OUT_FN_PPS_SDO1);      // MOSI -> RP23
    iPPSOutput(OUT_PIN_PPS_RP24, OUT_FN_PPS_SCK1OUT);   // SCLK -> RP24

    ethernetOpen();
    ethernetInit();
    lcdInit();
}

int main()
{
    __delay_ms(100);
    init();
    LATGbits.LATG7 = 1;
    TRISGbits.TRISG7 = 1;
    
    while(1) {
       ethernetClient();
    }
    
    return 1;
}
