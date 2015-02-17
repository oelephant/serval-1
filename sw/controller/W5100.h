/* 
 * File:   W5100.h
 * Author: Coleby
 *
 * Created on December 14, 2014, 3:17 PM
 */

#ifndef W5100_H
#define	W5100_H

/* Common Registers (pg 19) */
#define MR 0x0000
#define GAR0 0x0001 // Gateway Address
#define GAR1 0x0002
#define GAR2 0x0003
#define GAR3 0x0004
#define SUBR0 0x0005 // Subnet Mask Address
#define SUBR1 0x0006
#define SUBR2 0x0007
#define SUBR3 0x0008
#define SHAR0 0x0009 // Source Hardware Address
#define SHAR1 0x000A
#define SHAR2 0x000B
#define SHAR3 0x000C
#define SHAR4 0x000D
#define SHAR5 0x000E
#define SIPR0 0x000F // Source IP Address
#define SIPR1 0x0010
#define SIPR2 0x0011
#define SIPR3 0x0012
#define IR 0x0015 // Interrupt
#define IMR 0x0016 // Interrupt Mask
#define RTR0 0x0017 // Retry Time
#define RTR1 0x0018
#define RCR 0x0019 // Retry Count
#define RMSR 0x001A // RX Memory Size
#define TMSR 0x001B // TX Memory Size
#define PATR0 0x001C // Authentication Type in PPPoE
#define PATR1 0x001D
#define PTIMER 0x0028 // PPP LCP Request Timer
#define PMAGIC 0x0029 // PPP LCP Magic Number
#define UIPR0 0x002A // Unreachable IP Address
#define UIPR1 0x002B
#define UIPR2 0x002C
#define UIPR3 0x002D
#define UPORT0 0x002E // Unreachable Port
#define UPORT1 0x002F

/* Socket Registers (pg 25) */
#define S0_MR 0x0400 // Socket 0 Mode
#define S0_CR 0x0401 // Socket 0 Command
#define S0_IR 0x0402 // Socket 0 Interrupt
#define S0_SR 0x0403 // Socket 0 Status
#define S0_PORT0 0x0404 // Socket 0 Source Port
#define S0_PORT1 0x0405
#define S0_DHAR0 0x0406 // Socket 0 Destination Hardware Address
#define S0_DHAR1 0x0407
#define S0_DHAR2 0x0408
#define S0_DHAR3 0x0409
#define S0_DHAR4 0x040A
#define S0_DHAR5 0x040B
#define S0_DIPR0 0x040C // Socket 0 Destination IP Address
#define S0_DIPR1 0x040D
#define S0_DIPR2 0x040E
#define S0_DIPR3 0x040F
#define S0_DPORT0 0x0410 // Socket 0 Destination Port
#define S0_DPORT1 0x0411
#define S0_MSSR0 0x0412 // Socket 0 Maximum Segment Size
#define S0_MSSR1 0x0413
#define S0_PROTO 0x0414 // Socket 0 Protocol in IP Raw Mode
#define S0_TOS 0x0415 // Socket 0 IP TOS
#define S0_TTL 0x0416 // Socket 0 IP TTL
#define S0_TX_FSR0 0x0420 // Socket 0 TX Free Size
#define S0_TX_FSR1 0x0421
#define S0_TX_RD0 0x0422 // Socket 0 TX Read Pointer
#define S0_TX_RD1 0x0423
#define S0_TX_WR0 0x0424 // Socket 0 TX Write Pointer
#define S0_TX_WR1 0x0425
#define S0_RX_RSR0 0x0426 // Socket 0 RX Recieved Size
#define S0_RX_RSR1 0x0427
#define S0_RX_RD0 0x0428 // Socket 0 RX Read Pointer
#define S0_RX_RD1 0x0429
#define S1_MR 0x0500 // Socket 1 Mode
#define S1_CR 0x0501 // Socket 1 Command
#define S1_IR 0x0502 // Socket 1 Interrupt
#define S1_SR 0x0503 // Socket 1 Status
#define S1_PORT0 0x0504 // Socket 1 Source Port
#define S1_PORT1 0x0505
#define S1_DHAR0 0x0506 // Socket 1 Destination Hardware Address
#define S1_DHAR1 0x0507
#define S1_DHAR2 0x0508
#define S1_DHAR3 0x0509
#define S1_DHAR4 0x050A
#define S1_DHAR5 0x050B
#define S1_DIPR0 0x050C // Socket 1 Destination IP Address
#define S1_DIPR1 0x050D
#define S1_DIPR2 0x050E
#define S1_DIPR3 0x050F
#define S1_DPORT0 0x0510 // Socket 1 Destination Port
#define S1_DPORT1 0x0511
#define S1_MSSR0 0x0512 // Socket 1 Maximum Segment Size
#define S1_MSSR1 0x0513
#define S1_PROTO 0x0514 // Socket 1 Protocol in IP Raw Mode
#define S1_TOS 0x0515 // Socket 1 IP TOS
#define S1_TTL 0x0516 // Socket 1 IP TTL
#define S1_TX_FSR0 0x0520 // Socket 1 TX Free Size
#define S1_TX_FSR1 0x0521
#define S1_TX_RD0 0x0522 // Socket 1 TX Read Pointer
#define S1_TX_RD1 0x0523
#define S1_TX_WR0 0x0524 // Socket 1 TX Write Pointer
#define S1_TX_WR1 0x0525
#define S1_RX_RSR0 0x0526 // Socket 1 RX Recieved Size
#define S1_RX_RSR1 0x0527
#define S1_RX_RD0 0x0528 // Socket 1 RX Read Pointer
#define S1_RX_RD1 0x0529
#define S2_MR 0x0600 // Socket 2 Mode
#define S2_CR 0x0601 // Socket 2 Command
#define S2_IR 0x0602 // Socket 2 Interrupt
#define S2_SR 0x0603 // Socket 2 Status
#define S2_PORT0 0x0604 // Socket 2 Source Port
#define S2_PORT1 0x0605
#define S2_DHAR0 0x0606 // Socket 2 Destination Hardware Address
#define S2_DHAR1 0x0607
#define S2_DHAR2 0x0608
#define S2_DHAR3 0x0609
#define S2_DHAR4 0x060A
#define S2_DHAR5 0x060B
#define S2_DIPR0 0x060C // Socket 2 Destination IP Address
#define S2_DIPR1 0x060D
#define S2_DIPR2 0x060E
#define S2_DIPR3 0x060F
#define S2_DPORT0 0x0610 // Socket 2 Destination Port
#define S2_DPORT1 0x0611
#define S2_MSSR0 0x0612 // Socket 2 Maximum Segment Size
#define S2_MSSR1 0x0613
#define S2_PROTO 0x0614 // Socket 2 Protocol in IP Raw Mode
#define S2_TOS 0x0615 // Socket 2 IP TOS
#define S2_TTL 0x0616 // Socket 2 IP TTL
#define S2_TX_FSR0 0x0620 // Socket 2 TX Free Size
#define S2_TX_FSR1 0x0621
#define S2_TX_RD0 0x0622 // Socket 2 TX Read Pointer
#define S2_TX_RD1 0x0623
#define S2_TX_WR0 0x0624 // Socket 2 TX Write Pointer
#define S2_TX_WR1 0x0625
#define S2_RX_RSR0 0x0626 // Socket 2 RX Received Size
#define S2_RX_RSR1 0x0627
#define S2_RX_RD0 0x0628 // Socket 2 RX Read Pointer
#define S2_RX_RD1 0x0629
#define S3_MR 0x0700 // Socket 3 Mode
#define S3_CR 0x0701 // Socket 3 Command
#define S3_IR 0x0702 // Socket 3 Interrupt
#define S3_SR 0x0703 // Socket 3 Status
#define S3_PORT0 0x0704 // Socket 3 Source Port
#define S3_PORT1 0x0705
#define S3_DHAR0 0x0706 // Socket 3 Destination Hardware Address
#define S3_DHAR1 0x0707
#define S3_DHAR2 0x0708
#define S3_DHAR3 0x0709
#define S3_DHAR4 0x070A
#define S3_DHAR5 0x070B
#define S3_DIPR0 0x070C // Socket 3 Destination IP Address
#define S3_DIPR1 0x070D
#define S3_DIPR2 0x070E
#define S3_DIPR3 0x070F
#define S3_DPORT0 0x0710 // Socket 3 Destination Port
#define S3_DPORT1 0x0711
#define S3_MSSR0 0x0712 // Socket 3 Maximum Segment Size
#define S3_MSSR1 0x0713
#define S3_PROTO 0x0714 // Socket 3 Protocol in IP Raw Mode
#define S3_TOS 0x0715 // Socket 3 IP TOS
#define S3_TTL 0x0716 // Socket 3 IP TTL
#define S3_TX_FSR0 0x0720 // Socket 3 TX Free Size
#define S3_TX_FSR1 0x0721
#define S3_TX_RD0 0x0722 // Socket 3 TX Read Pointer
#define S3_TX_RD1 0x0723
#define S3_TX_WR0 0x0724 // Socket 3 TX Write Pointer
#define S3_TX_WR1 0x0725
#define S3_RX_RSR0 0x0726 // Socket 3 RX Received Size
#define S3_RX_RSR1 0x0727
#define S3_RX_RD0 0x0728 // Socket 3 RX Read Pointer
#define S3_RX_RD1 0x0729

#define TX_MEM 0x4000 // ... 0x5FFF
#define RX_MEM 0x6000 // ... 0x7FFF

/* Socket Commands */
#define OPEN 0x01 // used to initialize the socket
#define LISTEN 0x02 // only used in TCP, waits for incoming connections
#define CONNECT 0x04 // only used in TCP, sends connection request
#define DISCON 0x08 // only used in TCP, sends connection termination request
#define CLOSE 0x10 // closes socket
#define SEND 0x20 // transmits data as much as the increased size of Sn_TX_WR
#define SEND_MAC 0x21 // used in UDP, sends MAC without ARP process
#define SEND_KEEP 0x22 // only used in TCP, keeps checking connection status, timeout interrupt occurs if no repsponse
#define RECV 0x40 // recieving is processed for socket

/* Socket Status */
#define SOCK_CLOSED 0x00 // socket is closed
#define SOCK_INIT 0x13 // set as TCP and open commands given to Sn_CR
#define SOCK_LISTEN 0x14 // when LISTEN command given to Sn_CR
#define SOCK_ESTABLISHED 0x17 // when TCP data can be tx and rx
#define SOCK_CLOSE_WAIT 0x1C // termination ack'd from peer host (not closed)
#define SOCK_UDP 0x22 // open command given to Sn_CR when Sn_MR set for UDP
#define SOCK_IPRAW 0x32 // open command given to Sn_CR when Sn_MR set for IPRAW
#define SOCK_MACRAW 0x42 // open command given to S0_CR when S0_MR set for MACRAW
#define SOCK_PPPOE 0x5F // open command given to S0_CR when S0_MR set for PPPoE

/* shown during change of socket status */
#define SOCK_SYNSENT 0x15 // CONNECT given to Sn_CR at SOCK_INIT status
#define SOCK_SYNRECV 0x16 // connection request received from remote client
#define SOCK_FIN_WAIT 0x18 // during connection termination
#define SOCK_CLOSING 0x1A
#define SOCK_TIME_WAIT 0x1B
#define SOCK_LAST_ACK 0x1D
#define SOCK_ARP0 0x11 // when ARP request is sent to acquire MAC
#define SOCK_ARP1 0x21
#define SOCK_ARP2 0x31

#endif	/* W5100_H */