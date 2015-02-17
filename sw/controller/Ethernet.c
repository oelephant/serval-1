#include <xc.h>
#include "io.h"
#include "utils.h"
#include "w5100.h"

#define _XTAL_FREQ 4000000
#define SS LATBbits.LATB3

unsigned char GAR[4] = {0,0,0,0};               // gateway address
unsigned char SUBR[4] = {255, 255, 255, 0};     // subnet address
unsigned char SHAR[6] = {0xEC, 0x55, 0xF9, 0xCE, 0x44, 0x78};     // hardware address
unsigned char SIPR[4] = {192, 168, 1, 2};       // source ip address

unsigned int S0_RX_BASE;                        // S0 receive memory base
unsigned int S0_RX_MASK;
unsigned int S0_TX_BASE;                        // S0 transmit memory base
unsigned int S0_TX_MASK;

unsigned char testChar;
unsigned char testArr4[4];
unsigned char testArr2[2];
unsigned int testInt;

void SPI_init()
{
    /* ============*
     * disabel SPI *
     * ============*/
    
    // MSSP2 Control Register 1 (SPI Mode)
    // [ WCOL | SSPOV | SSPEN | CKP | SSPM3 | SSPM2 | SSPM1 | SSPM0 ]
    SSP2CON1 &= 0xDF;

    /* ===============*
     * initalize pins *
     * ===============*/

    // these three lines caused me ~9hrs of headache...
    // if the ADC is left on SDI2 will ALWAYS be 0

    ADCON0bits.ADON = 0;            // disable A/D converters
    ANCON0 = 0xFF;                  // set pins to digital
    ANCON1 = 0x7F;                  // set pins to digial

    LATBbits.LATB4 = 0;             // SCK2 initialize as low
    LATBbits.LATB2 = 0;             // SD02 initialize as low
    SS = 1;                         // SS initialize as high

    TRISBbits.TRISB4 = 0;           // SCK2 as output
    TRISBbits.TRISB2 = 0;           // SDO2 as output
    TRISBbits.TRISB1 = 1;           // SDI2 as input
    TRISBbits.TRISB3 = 0;           // SS as output

    /* ================== *
     * remapping SPI pins *
     * ================== */

    GIE = 0;                        // turn off global interrupts
    EECON2 = 0x55;                  // write unlock sequence to EECON2
    EECON2 = 0xAA;
    IOLOCK = 0;                     // unlock IO configuration
    GIE = 0;                        // turn on global interrupts

    RPINR21 = 0x04;                 // SDI2 <- RP4 (B1)
    RPOR7 = 0x0A;                   // PR7 (B4) -> SCK2
    RPINR22 = 0x07;                 // SCK2IN <- RP7 (B4)
    RPOR5 = 0x09;                   // RP5 (RB2) -> SDO2

    //EECON2 = 0x55;                  // write unlock sequence to EECON2
    //EECON2 = 0xAA;
    //IOLOCK = 1;                     // lock IO configuration

    /* ========================== *
     * SPI configuation registers *
     * ========================== */

    // MSSP2 Status Register (SPI Mode)
    // [ SMP | CKE | D/A | P | S | R/W | UA | BF ]
    SSP2STAT |= 0x40;

    // MSSP2 Control Register 1 (SPI Mode)
    // [ WCOL | SSPOV | SSPEN | CKP | SSPM3 | SSPM2 | SSPM1 | SSPM0 ]
    SSP2CON1 |= 0x20;
}

unsigned char SPI_write(unsigned char data)
{
    unsigned char ret;

    ret = SSP2BUF;          // clear the serial buffer
    SSP2BUF = data;         // send char

    while(!SSP2IF);         // wait for end of tx/rx

    ret = SSP2BUF;          // read serial buffer
    SSP2IF = 0;             // clear endo of tx/rx interrupt

    return ret;
}

unsigned char SPI_regWrite(unsigned int reg, unsigned char data)
{
    unsigned char reg0,reg1;
    unsigned char ret;
    Utils_intToChars(reg, &reg0, &reg1);

    // Future Application,
    // during the write procedure, the data read back into the serial buffer
    // is 0x01, 0x02, 0x03 following the 0xF0 op code. Can by used to check
    // status

    SS = 0;                 // slave select

    ret = SPI_write(0xF0);      // register write op code
    ret = SPI_write(reg0);      // upper byte of address
    ret = SPI_write(reg1);      // lower byte of address
    ret = SPI_write(data);      // data to write

    SS = 1;                 // slave select

    return ret;                 // should always be 0x03
}

unsigned char SPI_regRead(unsigned int reg)
{
    unsigned char reg0,reg1;
    unsigned char ret;
    Utils_intToChars(reg, &reg0, &reg1);

    SS = 0;                 // slave select

    // Future Application,
    // during the write procedure, the data read back into the serial buffer
    // is 0x01, 0x02, <data> following the 0x0F op code. Can by used to check
    // status

    ret = SPI_write(0x0F);      // register write op code
    ret = SPI_write(reg0);      // upper byte of address
    ret = SPI_write(reg1);      // lower byte of address
    ret = SPI_write(0x01);      // dummy value to get results

    SS = 1;                 // slave select

    return ret;                 // register data
}

void Ethernet_clientMode()
{
    unsigned char DIPR[4] = {192, 168, 1, 100};     // destination ip address
    unsigned char DPORT[2] = {0x23, 0x8C};          // destination port (9100)
    
    unsigned int RX_RSR;                            // RX received size
    unsigned char RX_RSR0;
    unsigned char RX_RSR1;
    
    unsigned int TX_FSR;                            // TX free size
    unsigned char TX_FSR0;
    unsigned char TX_FSR1;
    
    unsigned int TX_WR;                            // TX write pointer
    unsigned char TX_WR0;
    unsigned char TX_WR1;
    
    // memory variables
    unsigned int get_offset, get_start_address;
    unsigned int send_size;

    /* ===================== *
     * Socket Initizaliation *
     * ===================== */

    SPI_regWrite(S0_MR, 0x01);
    SPI_regWrite(S0_PORT0, 0x13);
    SPI_regWrite(S0_PORT1, 0x88);
    SPI_regWrite(S0_CR, OPEN);
    __delay_ms(1);

    while(SPI_regRead(S0_SR) != SOCK_INIT);
    NOP();
    
    /* ========= *
     *  Connect  *
     * ========= */
    
    // set destination ip address
    // [ Sn_DIPR0 | Sn_DIPR1 | Sn_DIPR2 | Sn_DIPR3 ]
    SPI_regWrite(S0_DIPR0, DIPR[0]);
    SPI_regWrite(S0_DIPR1, DIPR[1]);
    SPI_regWrite(S0_DIPR2, DIPR[2]);
    SPI_regWrite(S0_DIPR3, DIPR[3]);

    // set destination port address
    // [ Sn_DPORT0 | Sn_DPORT1 ]
    SPI_regWrite(S0_DPORT0, DPORT[0]);
    SPI_regWrite(S0_DPORT1, DPORT[1]);
    
    // set connect command
    SPI_regWrite(S0_CR, CONNECT);
    __delay_ms(1);


    /* =============================== *
     * Established? check for timeout? *
     * =============================== */
    
//    // using interrupts
//    if (S0_IR & 0x01 > 1) {
//        // connection established
//    } else if (S0_IR & 0x04 > 1) {
//        // timeout has occured
//    }

    // check status register
    while (SPI_regRead(S0_SR) != SOCK_ESTABLISHED) {

        // socket will close upon timeout
        if (SPI_regRead(S0_SR) == SOCK_CLOSED) {
            // TODO process timeout
            return;
        }
    }

    /* =========================== *
     * Established, received data? *
     * =========================== */
    
//    // using interrups
//    if (S0_IR & 0x04 > 0) {
//        // recieve data occured, refer to IR, IMR and socket
//    }
    int i = 0;
    for (i = 0; i < 10; i++) {
    // check status register
    RX_RSR0 = SPI_regRead(S0_RX_RSR0);
    RX_RSR1 = SPI_regRead(S0_RX_RSR1);
    RX_RSR = Utils_charsToInt(RX_RSR0, RX_RSR1);
    
    if (RX_RSR != 0x0000) {    
        // TODO go receive
        NOP();
    }

    /* ======================= *
     * Established, send data? *
     * ======================= */
    
    TX_FSR0 = SPI_regRead(S0_TX_FSR0);
    TX_FSR1 = SPI_regRead(S0_TX_FSR1);
    TX_FSR = Utils_charsToInt(TX_FSR0, TX_FSR1);
    
    // sending 4 bytes eventually,
    // TODO, find a more elegant way to specify this
    send_size = 36;
    
    // wait until enough room is available, be sure to set this
    // at some point (send_size)
    while (TX_FSR < send_size) {
        TX_FSR0 = SPI_regRead(S0_TX_FSR0);
        TX_FSR1 = SPI_regRead(S0_TX_FSR1);
        TX_FSR = Utils_charsToInt(TX_FSR0, TX_FSR1);    
    }
    
    // calculation offset address
    TX_WR0 = SPI_regRead(S0_TX_WR0);
    TX_WR1 = SPI_regRead(S0_TX_WR1);
    TX_WR = Utils_charsToInt(TX_WR0, TX_WR1);
    
    get_offset = TX_WR & S0_TX_MASK;
    get_start_address = S0_TX_BASE + get_offset;
    
    // TODO, check for over flow

    // write data to start address
    SPI_regWrite(get_start_address, 0x1B);
    SPI_regWrite(get_start_address, 0x40);
    SPI_regWrite(get_start_address, 0x1B);
    SPI_regWrite(get_start_address, 0x40);
    SPI_regWrite(get_start_address, 0x1B);
    SPI_regWrite(get_start_address, 0x40);
    SPI_regWrite(get_start_address, 0x0A);
    SPI_regWrite(get_start_address, 0x0A);
    SPI_regWrite(get_start_address, 0x0A);
    SPI_regWrite(get_start_address, 'C');
    SPI_regWrite(get_start_address, 'E');
    SPI_regWrite(get_start_address, 'E');
    SPI_regWrite(get_start_address, 'N');
    SPI_regWrite(get_start_address, ' ');
    SPI_regWrite(get_start_address, '4');
    SPI_regWrite(get_start_address, '3');
    SPI_regWrite(get_start_address, '6');
    SPI_regWrite(get_start_address, '0');
    SPI_regWrite(get_start_address, ' ');
    SPI_regWrite(get_start_address, 'T');
    SPI_regWrite(get_start_address, 'E');
    SPI_regWrite(get_start_address, 'S');
    SPI_regWrite(get_start_address, 'T');
    SPI_regWrite(get_start_address, ' ');
    SPI_regWrite(get_start_address, 'T');
    SPI_regWrite(get_start_address, 'E');
    SPI_regWrite(get_start_address, 'S');
    SPI_regWrite(get_start_address, 'T');
    SPI_regWrite(get_start_address, ' ');
    SPI_regWrite(get_start_address, 'T');
    SPI_regWrite(get_start_address, 'E');
    SPI_regWrite(get_start_address, 'S');
    SPI_regWrite(get_start_address, 'T');
    SPI_regWrite(get_start_address, 0x0A);
    SPI_regWrite(get_start_address, 0x0A);
    SPI_regWrite(get_start_address, 0x0A);
    __delay_ms(1);

    TX_WR += send_size;
    Utils_intToChars(TX_WR, &TX_WR0, &TX_WR1);
    SPI_regWrite(S0_TX_WR0, TX_WR0);
    SPI_regWrite(S0_TX_WR1, TX_WR1);
    __delay_ms(100);
    // send commmand
    SPI_regWrite(S0_CR, SEND);
    
    /* ========================== *
     * Established, received FIN? *
     * ========================== */

//    // using interrupts
//    if (S0_IR & 0x02 > 0) {
//        // go to closed stage
//    }
    }
    // wait for something to happen
    while(1) {

        // check status register

        if (SPI_regRead(S0_SR) == SOCK_CLOSE_WAIT) {
            break;
        }

        if (SPI_regRead(S0_SR) == SOCK_CLOSED) {
            break;
        }

        if (SPI_regRead(S0_SR) == SOCK_CLOSED) {
            break;
        }

        // to request termination
        SPI_regWrite(S0_SR, DISCON);

        // force close
        SPI_regWrite(S0_CR, CLOSE);
    }
}

void Ethernet_init()
{
    /* ============== *
     * initialize SPI *
     * ============== */

    SPI_init();

    /* ============== *
     * Basic Settings *
     * ============== */

    // set mode register
    // [ RST | _ | _ | PB | PPPoE | _ | AI | IND ]
    SPI_regWrite(MR, 0x80);


    // set interrupt mask register
    // [ IM_IR7 | IM_IR6 | IM_IR5 | _ | IM_IR3 | IM_IR2 | IM_IR1 | IM_IR0 ]
    SPI_regWrite(IMR, 0x00);

    // set retry time value register
    // [ RTR0 | RTR1 ]
    //SPI_regWrite(RTR0, 0xFF);
    //SPI_regWrite(RTR1, 0xFF);

    // set retry count register
    // RCR, number of re-transmissions before TIMEOUT Interrupt.
    //  TIMOUT bit in Sn_IR is set to '1'
    //SPI_regWrite(RCR, 0xFF);

    /* ================ *
     * Network Settings *
     * ================ */

    // set gateway address register
    // [ GAR0 | GAR1 | GAR2 | GAR3 ]
    SPI_regWrite(GAR0, GAR[0]);
    SPI_regWrite(GAR1, GAR[1]);
    SPI_regWrite(GAR2, GAR[2]);
    SPI_regWrite(GAR3, GAR[3]);

    // set source hardware address
    // [ SHAR0 | SHAR1 | SHAR2 | SHAR3 | SHAR4 | SHAR5 ]
    SPI_regWrite(SHAR0, SHAR[0]);
    SPI_regWrite(SHAR1, SHAR[1]);
    SPI_regWrite(SHAR2, SHAR[2]);
    SPI_regWrite(SHAR3, SHAR[3]);
    SPI_regWrite(SHAR4, SHAR[4]);
    SPI_regWrite(SHAR5, SHAR[5]);

    // set subnet mask register
    // [ SUBR0 | SUBR1 | SUBR2 | SUBR3 ]
    SPI_regWrite(SUBR0, SUBR[0]);
    SPI_regWrite(SUBR1, SUBR[1]);
    SPI_regWrite(SUBR2, SUBR[2]);
    SPI_regWrite(SUBR3, SUBR[3]);

    // set source ip address register
    // [ SIPR0 | SIPR1 | SIPR2 | SIPR3 ]
    SPI_regWrite(SIPR0, SIPR[0]);
    SPI_regWrite(SIPR1, SIPR[1]);
    SPI_regWrite(SIPR2, SIPR[2]);
    SPI_regWrite(SIPR3, SIPR[3]);

    // assign memory allocations (2K per socket)
    SPI_regWrite(RMSR, 0x55);
    SPI_regWrite(TMSR, 0x55);

    // store base address and masks
    S0_RX_BASE = 0x6000;
    S0_RX_MASK = 0x7FFF;
    S0_TX_BASE = 0x4000;
    S0_TX_MASK = 0x7FFF;
}

void Ethernet()
{
    Ethernet_init();
    Ethernet_clientMode();
}