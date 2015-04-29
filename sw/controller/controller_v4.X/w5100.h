#ifndef W5100_H
#define	W5100_H

#define W5100_MR 0x0000         // Mode (MR)

// Gateway Address
#define W5100_GAR0 0x0001       // (GAR0)
#define W5100_GAR1 0x0002       // (GAR1)
#define W5100_GAR2 0x0003       // (GAR2)
#define W5100_GAR3 0x0004       // (GAR3)

// Subnet Mask Address
#define W5100_SUBR0 0x0005      // (SUBR0)
#define W5100_SUBR1 0x0006      // (SUBR1)
#define W5100_SUBR2 0x0007      // (SUBR2)
#define W5100_SUBR3 0x0008      // (SUBR3)

// Source Hardware Address
#define W5100_SHAR0 0x0009      // (SHAR0)
#define W5100_SHAR1 0x000A      // (SHAR1)
#define W5100_SHAR2 0x000B      // (SHAR2)
#define W5100_SHAR3 0x000C      // (SHAR3)
#define W5100_SHAR4 0x000D      // (SHAR4)
#define W5100_SHAR5 0x000E      // (SHAR5)

// Source Hardware Address
#define W5100_SIPR0 0x000F      // (SIPR0)
#define W5100_SIPR1 0x0010      // (SIPR1)
#define W5100_SIPR2 0x0011      // (SIPR3)
#define W5100_SIPR3 0x0012      // (SIPR2)
#define W5100_IR 0x0015         // Interrupt (IR)
#define W5100_IMR 0x0016        // Interrupt Mask (IMR)

// Retry Time
#define W5100_RTR0 0x0017       // (RTR0)
#define W5100_RTR1 0x0018       // (RTR1)
#define W5100_RCR 0x0019        // Retry Count (RCR)
#define W5100_RMSR 0x001A       // RX Memory Size (RMSR)
#define W5100_TMSR 0x001B       // TX Memory Size (TMSR)

// Authentication Type in PPPoE
#define W5100_PATR0 0x001C      // (PATR0)
#define W5100_PATR1 0x001D      // (PATR1)
#define W5100_PTIMER 0x0028     // PPP LCP Request Timer (PTIMER)
#define W5100_PMAGIC 0x0029     // PPP LCP Magic number (PMAGIC)

// Unreachable IP Address
#define W5100_UIPR0 0x002A      // (UIPR0)
#define W5100_UIPR1 0x002B      // (UIPR1)
#define W5100_UIPR2 0x002C      // (UIPR2)
#define W5100_UIPR3 0x002D      // (UIPR3)

// Unreachable Port
#define W5100_UPORT0 0x002E     // (UPORT0)
#define W5100_UPORT1 0x002F     // (UPORT1)
#define W5100_S0_MR 0x0400      // Socket 0 Mode (S0_MR)
#define W5100_S0_CR 0x0401      // Socket 0 Command (S0_CR)
#define W5100_CR_OPEN 0x01
#define W5100_CR_LISTEN 0x02
#define W5100_CR_CONNECT 0x04
#define W5100_CR_DISCON 0x08
#define W5100_CR_CLOSE 0x10
#define W5100_CR_SEND 0x20
#define W5100_CR_SEND_MAC 0x21
#define W5100_CR_SEND_KEEP 0x22
#define W5100_CR_RECV 0x40
#define W5100_S0_IR 0x0402      // Socket 0 Interrupt (S0_IR)
#define W5100_S0_SR 0x0403      // Socket 0 Status (S0_SR)
#define W5100_SOCK_CLOSED 0x00
#define W5100_SOCK_INIT 0x13
#define W5100_SOCK_LISTEN 0x14
#define W5100_SOCK_ESTABLISHED 0x17
#define W5100_SOCK_CLOSE_WAIT 0x1C
#define W5100_SOCK_UDP 0x22
#define W5100_SOCK_IPRAW 0x32
#define W5100_SOCK_MACRAW 0x42
#define W5100_SOCK_PPPOE 0x5F
#define W5100_SOCK_SYNSENT 0x15
#define W5100_SOCK_SYNRECV 0x16
#define W5100_SOCK_FIN_WAIT 0x18
#define W5100_SOCK_CLOSING 0x1A
#define W5100_SOCK_TIME_WAIT 0x1B
#define W5100_SOCK_LAST_ACK 0x1D
#define W5100_SOCK_ARP1 0x11
#define W5100_SOCK_ARP2 0x21
#define W5100_SOCK_ARP3 0x31

// Socket 0 Source Port
#define W5100_S0_PORT0 0x0404   // (S0_PORT0)
#define W5100_S0_PORT1 0x0405   // (S0_PORT1)

// Socket 0 Destination Hardware Address
#define W5100_S0_DHAR0 0x0406   // (S0_DHAR0)
#define W5100_S0_DHAR1 0x0407   // (S0_DHAR1)
#define W5100_S0_DHAR2 0x0408   // (S0_DHAR2)
#define W5100_S0_DHAR3 0x0409   // (S0_DHAR3)
#define W5100_S0_DHAR4 0x040A   // (S0_DHAR4)
#define W5100_S0_DHAR5 0x040B   // (S0_DHAR5)

// Socket 0 Destination IP Address
#define W5100_S0_DIPR0 0x040C   // (S0_DIPR0)
#define W5100_S0_DIPR1 0x040D   // (S0_DIPR1)
#define W5100_S0_DIPR2 0x040E   // (S0_DIPR2)
#define W5100_S0_DIPR3 0x040F   // (S0_DIPR3)

// Socket 0 Destination Port
#define W5100_S0_DPORT0 0x0410  // (S0_DPORT0)
#define W5100_S0_DPORT1 0x0411  // (S0_DPORT1)

// Socket 0 Maximum Segment Size
#define W5100_S0_MSSR0 0x0412   // (S0_MSSR0)
#define W5100_S0_MSSR1 0x0413   // (S0_MSSR1)
#define W5100_S0_PROTO 0x0414   // Socket 0 Protocol in IP Raw mode (S0_PROTO)
#define W5100_S0_TOS 0x0415     // Socket 0 IP TOS (S0_TOS)
#define W5100_S0_TTL 0x0416     // Socket 0 IP TTL (S0_TTL)

// Socket 0 TX Free Size
#define W5100_S0_TX_FSR0 0x0420 // (S0_TX_FSR0)
#define W5100_S0_TX_FSR1 0x0421 // (S0_TX_FSR1)

// Socket 0 TX Read Pointer
#define W5100_S0_TX_RD0 0x0422  // (S0_TX_RD0)
#define W5100_S0_TX_RD1 0x0423  // (S0_TX_RD1)

// Socket 0 TX Write Pointer
#define W5100_S0_TX_WR0 0x0424  // (S0_TX_WR0)
#define W5100_S0_TX_WR1 0x0425  // (S0_TX_WR1)

// Socket 0 RX Received Size
#define W5100_S0_RX_RSR0 0x0426 // (S0_RX_RSR0)
#define W5100_S0_RX_RSR1 0x0427 // (S0_RX_RSR1)

// Socket 0 RX Read Pointer
#define W5100_S0_RX_RD0 0x0428  // (S0_RX_RD0)
#define W5100_S0_RX_RD1 0x0429  // (S0_RX_RD1)

#endif	/* W5100_H */

