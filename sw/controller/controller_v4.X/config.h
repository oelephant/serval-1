/* 
 * File:   config.h
 * Author: Coleby.Conner
 *
 * Created on April 23, 2015, 11:22 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

// CONFIG3
#pragma config WPFP = WPFP0             // Write Protection Flash Page Segment Boundary (Page 0 (0x00))
#pragma config SOSCSEL = EC             // Secondary Oscillator Power Mode Select (External clock (SCLKI) or Digital I/O mode()
#pragma config WUTSEL = LEG             // Voltage Regulator Wake-up Time Select (Default regulator start-up time is used)
#pragma config ALTPMP = ALPMPDIS        // Alternate PMP Pin Mapping (EPMP pins are in default location mode)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Segmented code protection is disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

// CONFIG2
#pragma config POSCMOD = EC             // Primary Oscillator Select (EC Oscillator mode is selected)
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable (The IOLOCK bit can be set and cleared as needed, provided the unlock sequence has been completed)
#pragma config OSCIOFNC = OFF           // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL              // Initial Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config PLL96MHZ = ON            // 96MHz PLL Startup Select (96 MHz PLL is enabled automatically on start-up)
#pragma config PLLDIV = DIV2            // 96 MHz PLL Prescaler Select (Oscillator input is divided by 2 (8 MHz input))
#pragma config IESO = OFF               // Internal External Switchover (IESO mode (Two-Speed Start-up) is disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#endif	/* CONFIG_H */

