#include "common.h"
#include <xc.h>
#include <spi.h>
#include <libpic30.h>
#include "w5100.h"
#include "ethernet.h"
#include "lcd.h"
#include "serval.h"
#include "wifi.h"
#include "string.h"

#define W5100_S0_RX_BASE 0x6000
#define W5100_S0_RX_MASK 0x1FFF
#define W5100_S0_TX_BASE 0x4000
#define W5100_S0_TX_MASK 0x1FFF


void ethernetInit();
void ethernetOpen();
char ethernetRead(unsigned int addr);
void ethernetWrite(unsigned int addr, char data);
char ethernetClient();
void ethernetClientOpen();
void ethernetClientConnect();
char ethernetClientEstablished();
void ethernetClientReceive(unsigned int receiveSize);
void ethernetClientSend(unsigned int sendSize);

char msg[20];

char ethernetClient()
{
    unsigned char sr;
    char status = 0;
    
    // open socket for tcp
    // --------------------
    
    do {
        ethernetClientOpen();
        sr = ethernetRead(W5100_S0_SR);
    } while(sr != W5100_SOCK_INIT);

    // attempt connection
    // -------------------
    
    ethernetClientConnect();
    
    // check connection status
    // ------------------------
    
    do {
        sr = ethernetRead(W5100_S0_SR);
    } while (sr != W5100_SOCK_ESTABLISHED && sr != W5100_SOCK_CLOSED);

    // if connection is established let the following mehtod handle protocol
    if (sr == W5100_SOCK_ESTABLISHED)
        status = ethernetClientEstablished();

    // timeout has occured from established (>0) or connection request (0)
    return status;
}

char ethernetClientEstablished()
{
    unsigned int receiveSize = 0;
    char sr, ret, status = 0;
    char disconAfter = 10;          // TODO find a better way...

    lcdPrintChar('.');

    do {
        // check size of receive buffer
        // -----------------------------
        ret = ethernetRead(W5100_S0_RX_RSR0);
        receiveSize = ret << 8;
        ret = ethernetRead(W5100_S0_RX_RSR1);
        receiveSize |= ret;

        Nop();

        if (receiveSize != 0x0000) {
            ethernetClientReceive(receiveSize);
            ethernetFromServer();
        }

        // XBee stuff
        // ----------- TODO
        if (wifiRxAvailable()) {
            wifiOpen();
            wifiRead();
            wifiFromTable();
            Nop();
        }

        ethernetOpen();
        
        // determine if data is ready for sending
        // ---------------------------------------
        Nop();
        if (txSize != 0)
            ethernetClientSend(txSize);
__delay_ms(100);
        // check for last ack, disconnect, or timeout
        // -------------------------------------------

        sr = ethernetRead(W5100_S0_SR);

        if (sr == W5100_SOCK_CLOSE_WAIT)
            status = 1;

        // TODO find a better way...
        if(!disconAfter--)
            status = 2;
//        else if (disconAfter == 5) {
//            char msg[] = "Get /php/checks.php?test HTTP/1.1\nHost: 68.227.174.223\n\n";
//            int i = 0;
//            txSize = 0;
//            for (i = 0; i < strlen(msg); i++) {
//                tx[i] = msg[i];
//                txSize++;
//            }
//            Nop();
//        }

        if (sr == W5100_SOCK_CLOSED)
            status = 3;

        Nop();

    } while (status == 0);
    
    return status;
}

void ethernetClientSend(unsigned int sendSize)
{
    unsigned int txFreeSize;
    unsigned int txWr;
    unsigned int txOffset;
    unsigned int txStartAddr;
    unsigned char ret;
    int i;

    do {
        ret = ethernetRead(W5100_S0_TX_FSR0);
        txFreeSize = ret << 8;
        ret = ethernetRead(W5100_S0_TX_FSR1);
        txFreeSize |= ret;
    } while (txFreeSize < sendSize);

    ret = ethernetRead(W5100_S0_TX_WR0);
    txWr = ret << 8;
    ret = ethernetRead(W5100_S0_TX_WR1);
    txWr |= ret;

    txOffset = txWr & W5100_S0_TX_MASK;

    txStartAddr = W5100_S0_TX_BASE + txOffset;

    // over flow won't happen

    for (i = 0; i < sendSize; i++) {
        ethernetWrite(txStartAddr + i, tx[i]);
        tx[i] = 0;
    }
    txSize = 0;

    txWr += sendSize;

    ethernetWrite(W5100_S0_TX_WR0, txWr >> 8);
    ethernetWrite(W5100_S0_TX_WR1, txWr);

    ethernetWrite(W5100_S0_CR, W5100_CR_SEND);

    do {
        ret = ethernetRead(W5100_S0_CR);
    } while (ret != 0x00);
}

void ethernetClientReceive(unsigned int receiveSize)
{
    unsigned int rxRd;
    unsigned int rxOffset;
    unsigned int rxStartAddr;
    unsigned char ret;
    unsigned int i;

    ret = ethernetRead(W5100_S0_RX_RSR0);
    receiveSize = ret << 8;
    ret = ethernetRead(W5100_S0_RX_RSR1);
    receiveSize |= ret;

    ret = ethernetRead(W5100_S0_RX_RD0);
    rxRd = ret << 8;
    ret = ethernetRead(W5100_S0_RX_RD1);
    rxRd |= ret;

    Nop();

    rxOffset = rxRd & W5100_S0_RX_MASK;

    rxStartAddr = W5100_S0_RX_BASE + rxOffset;

    // memory allocated is 8k, don't need to check for overflow

    rxSize = receiveSize;
    for (i = 0; i < rxSize; i++) {
        rx[i] = ethernetRead(rxStartAddr + i);
    }

    Nop();
    rxRd += receiveSize;

    ethernetWrite(W5100_S0_RX_RD0, rxRd >> 8);
    ethernetWrite(W5100_S0_RX_RD1, rxRd);

    ethernetWrite(W5100_S0_CR, W5100_CR_RECV);
    do {
        ret = ethernetRead(W5100_S0_CR);
    } while (ret != 0x00);
}

void ethernetClientConnect()
{
    unsigned char ip[] = {68,227,174,223};
    unsigned char port[] = {0x1F, 0x90};

    ethernetWrite(W5100_S0_DIPR0, ip[0]);       // destination ip
    ethernetWrite(W5100_S0_DIPR1, ip[1]);
    ethernetWrite(W5100_S0_DIPR2, ip[2]);
    ethernetWrite(W5100_S0_DIPR3, ip[3]);
    ethernetWrite(W5100_S0_DPORT0, port[0]);    // destination port
    ethernetWrite(W5100_S0_DPORT1, port[1]);

    // connect
    ethernetWrite(W5100_S0_CR, W5100_CR_CONNECT);
    while(ethernetRead(W5100_S0_CR) != 0x00);
}

void ethernetClientOpen()
{
    ethernetWrite(W5100_S0_MR, 0x01);       // TCP mode
    ethernetWrite(W5100_S0_PORT0, 0);       // port: 80
    ethernetWrite(W5100_S0_PORT1, 80);

    // open the socket
    ethernetWrite(W5100_S0_CR, W5100_CR_OPEN);
    while(ethernetRead(W5100_S0_CR) != 0x00);
}

void ethernetInit()
{
    // basic settings
    ethernetWrite(W5100_MR, 0x00);
    ethernetWrite(W5100_IMR, 0x00);     // interrupts off
    ethernetWrite(W5100_RTR0, 0x07);    // 200ms retry timer
    ethernetWrite(W5100_RTR1, 0xD0);
    ethernetWrite(W5100_RCR, 0x03);     // 3 retry attemps

    // network settings
    ethernetWrite(W5100_GAR0, 192);    // gateway, 192.168.1.1
    ethernetWrite(W5100_GAR1, 168);
    ethernetWrite(W5100_GAR2, 1);
    ethernetWrite(W5100_GAR3, 1);
    ethernetWrite(W5100_SHAR0, 0x00);   // mac: 00:08:DC:01:02:03
    ethernetWrite(W5100_SHAR1, 0x08);
    ethernetWrite(W5100_SHAR2, 0xDC);
    ethernetWrite(W5100_SHAR3, 0x01);
    ethernetWrite(W5100_SHAR4, 0x02);
    ethernetWrite(W5100_SHAR5, 0x03);
    ethernetWrite(W5100_SIPR0, 192);   // ip: 192.168.1.8
    ethernetWrite(W5100_SIPR1, 168);
    ethernetWrite(W5100_SIPR2, 1);
    ethernetWrite(W5100_SIPR3, 105);
    ethernetWrite(W5100_SUBR0, 0xFF);   // subnet: 255.255.255.0
    ethernetWrite(W5100_SUBR1, 0xFF);
    ethernetWrite(W5100_SUBR3, 0xFF);
    ethernetWrite(W5100_SUBR3, 0x00);

    // socket memory
    ethernetWrite(W5100_RMSR, 0x03);   // 8K rx memory assigned to socket 0
    ethernetWrite(W5100_TMSR, 0x03);   // 8K tx memory assigned to socket 0

}

void ethernetOpen()
{
    // ensure spi channel is closed before opening
    CloseSPI1();

    __delay_ms(100);

    // open spi channel for wifi
    OpenSPI1((
                ENABLE_SCK_PIN &
                ENABLE_SDO_PIN &
                SPI_MODE8_ON &
                SPI_SMP_OFF &
                SPI_CKE_ON &
                SLAVE_ENABLE_OFF &
                CLK_POL_ACTIVE_HIGH &
                MASTER_ENABLE_ON &
                SEC_PRESCAL_2_1 &
                PRI_PRESCAL_1_1
            ),(
                0x00
            ),(
                SPI_ENABLE &
                SPI_IDLE_CON
            ));
    __delay_ms(100);
}

void ethernetWrite(unsigned int addr, char data)
{
    char ret;

    ETH_SS = 0;

    WriteSPI1(0xF0);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();
    
    Nop();

    WriteSPI1(addr >> 8);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();

    Nop();

    WriteSPI1(addr);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();
    
    Nop();

    WriteSPI1(data);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();

    Nop();

    ETH_SS = 1;
}

char ethernetRead(unsigned int addr)
{
    char ret;

    ETH_SS = 0;

    WriteSPI1(0x0F);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();

    WriteSPI1(addr >> 8);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();

    WriteSPI1(addr);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();

    WriteSPI1(0xFF);
    while(!SPI1_Rx_Buf_Full);
    ret = ReadSPI1();

    ETH_SS = 1;

    return ret;
}