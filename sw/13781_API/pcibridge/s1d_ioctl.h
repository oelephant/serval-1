///////////////////////////////////////////////////////////////////////////////
//
// S1D_IOCTL.H
//
// (C)SEIKO EPSON CORPORATION 2005. All rights reserved.
//
// Include file for S1D13xxx PCI Board Driver.
// Define the IOCTL codes we will use.  The IOCTL code contains a command
// identifier, plus other information about the device, the type of access
// with which the file must have been opened, and the type of buffering.
//
// This header file is used by both the HAL and the S1D13xxx drivers.  It
// must be kept in sync to prevent version skews.
//
///////////////////////////////////////////////////////////////////////////////

#define PCI_BLOCK_SIZE	0x00400000UL		// Default PCI virtual memory block size
#define ISA_BLOCK_SIZE	0x00200000UL		// Default ISA block allocation size

#define	MAX_INT_REGS	8					// Maximum number of interrupt registers
#define DWORD   ULONG

#pragma pack(push)
#pragma pack(1)

typedef struct tagIntStruct
{
	DWORD	EnableReg;						// Interrupt Enable register offset
	DWORD	StatusReg;						// Interrupt Status register offset
	DWORD	BitMask;						// Which bits to look for
	DWORD	AccessSize;						// Size of register accesses in bytes (1,2,4)
	DWORD	Flags;							// See INTFLAG_xxx definitions
} INT_STRUCT, *PINT_STRUCT;

#define INTFLAG_NONE		0x00000000		// No special flags specified
#define INTFLAG_RAWSTATUS	0x00000001		// True if status register uses raw (sticky) status bits

// This is the structure passed to the S1D13xxx PCI driver.
// The S1D13xxx PCI driver is capable of monitoring more than one interrupt
// Status/Enable register combination.  We reserve all 8 slots, but only pass
// the number of registers that need to be monitored for THIS chip.

typedef struct tagISRStruct
{
	DWORD			dwProcessID;			// Current process ID for device driver to identify us
	DWORD			nIntInfoItems;			// Number of interrupt register pairs to maintain
	INT_STRUCT		aIntInfo[MAX_INT_REGS];	// A description of the interrupt register pairs
} ISR_STRUCT, *PISR_STRUCT;

#define SED_TYPE FILE_DEVICE_CONTROLLER 

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.

#define IOCTL_SED_QUERY_NUMBER_OF_PCI_BOARDS    CTL_CODE( SED_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL_SED_MAP_PCI_BOARD return value:
// For revision < 3:
// aRetPCI[0](linear address), aRetPCI[1](physical address)
// For revision >= 3:
// aRetPCI[0](linear address), aRetPCI[1](physical address), aRetPCI[2](size of memory mapped in bytes)

#define IOCTL_SED_MAP_PCI_BOARD                 CTL_CODE( SED_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_MAP_PHYSICAL_MEMORY           CTL_CODE( SED_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_UNMAP_LINEAR_MEMORY           CTL_CODE( SED_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SED_HOOK_ISR_CALLBACK             CTL_CODE( SED_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_UNHOOK_ISR_CALLBACK           CTL_CODE( SED_TYPE, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SED_WAIT_FOR_IRQ                  CTL_CODE( SED_TYPE, 0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_GET_REVISION                  CTL_CODE( SED_TYPE, 0x907, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Helper IOCTLs, used to read/write the attribute memory on PCCARD.
// Used to reprogram CIS info.

typedef struct
    {
    DWORD offset;
    DWORD length;
    UCHAR data[1];
    } CISBUFFERST;

#define IOCTL_SED_READ_PCCARD_CIS      CTL_CODE(SED_TYPE, 0x908, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_WRITE_PCCARD_CIS     CTL_CODE(SED_TYPE, 0x909, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
    {
    DWORD offset;
    DWORD byte;
    } DATAOFFST;

typedef struct
    {
    DWORD length;
    DATAOFFST data[1];
    } CISBUFFEREXST;

#define IOCTL_SED_WRITE_PCCARD_CISEX   CTL_CODE(SED_TYPE, 0x90A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_GET_ID		       CTL_CODE(SED_TYPE, 0x90B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_ENABLE_INTERRUPT	   CTL_CODE(SED_TYPE, 0x90C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SED_MAP_PCI_BOARD_EXTENDED		CTL_CODE(SED_TYPE, 0x90D, METHOD_BUFFERED, FILE_ANY_ACCESS)

#pragma pack(pop)


