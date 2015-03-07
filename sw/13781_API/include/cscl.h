//----------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//----------------------------------------------------------------------------
#ifndef seCSCL_H_INCLUDED
#define seCSCL_H_INCLUDED
#define __RPCASYNC_H__
#include <stdio.h>
#include "datatype.h"

/* Prometheus Protocol defines */
#ifdef _DEBUG
    #ifndef _WINDOWS
    int seCSCLPrintf(const char *f, ...);
    #define seCSCLPrintf printf
    #else
    int seCSCLLog(const char *f, ...);
    #define seCSCLPrintf seCSCLLog
    #endif
#else
#define seCSCLPrintf
#endif

#define seMaxMgeNum 4			/* Number of supported MGEs */
#define UNKNOWN_FPGA_REVISION	(~0)
/* Bit assignment for the Read MGE/ Write MGE packets */
#define seCSCL_MGE_MEMORY		1
#define seCSCL_MGE_REGISTERS	0
#define seCSCL_MGE_NORMAL		2
#define seCSCL_MGE_BYPASS		0
#define seCSCL_MGE_ADDR_INC		4
#define seCSCL_MGE_ADDR_SAME	0
#define seCSCL_MGE_DIRECT		8
#define seCSCL_MGE_INDIRECT		0
/* End of Bit assignment for the Read MGE/ Write MGE packets */

// Allow both standard C and C++ linkages.
#ifdef __cplusplus
   extern "C" {
#endif

/* Global MGE Configuration */
typedef struct seCsclConfigRegs {
	UInt8 ConfigRegA;
	UInt8 ConfigRegB;
	UInt8 ConfigRegC;
	UInt8 ConfigRegD;
	UInt8 ConfigRegE;
	UInt8 ConfigRegF;
}seCsclConfigRegs;

#define seCsclHostParDir			0
#define seCsclHostParInDir			1
#define seCsclHostSerial			2

#define seCsclSigIntel80Type1		(0<<2)
#define seCsclSigREdgeTrig			(0<<2)
#define seCsclSigIntel80Type2		(1<<2)
#define seCsclSigFEdgeTrig			(1<<2)
#define seCsclSigIntel80Type3		(2<<2)
#define seCsclSigMotorola68			(3<<2)

#define seCsclDWidth32				(0<<5)
#define seCsclDWidth8				(1<<5)
#define seCsclDWidth16				(2<<5)

#define seCsclActiveHigh			(0<<7)
#define seCsclActiveLow				(1<<7)


#define seCs2RD(a)	(a)
#define seCs2WR(a)	((a)<<2)
#define seRD2CS(a)	((a)<<4)
#define seWR2CS(a)	((a)<<6)

#define RDWAIT(a)	(a)
#define WRWAIT(a)	((a)<<4)

#define B2BWAIT(a)		(a)

#define B2BCSHIGH		(1<<4)
#define B2BCSLOW		(0)

#define seAddrBusWidth8		(1<<5)
#define seAddrBusWidth16	(2<<5)
#define seAddrBusWidth32	(0<<5)

#define seEnableExternalWAIT (1<<7)

/* End of Global MGE Configuration */
typedef struct seMgeDescriptor {
	UInt32  MgeNum;
	UInt32  InterfaceFlags;
	UInt32  MgeMemAddr;
	UInt32  MgeRegAddr;
	seCsclConfigRegs *RegsInfo;
}seMgeDescriptor;

typedef  struct seCSCLJtagDescr {
	UInt32 MaxPacketDataSize;
} seCSCLJtagDescr;
#define seMGE_REG_ADDR_OFFSET			0//(0x00060000)
#define seMGE_MEM_ADDR_OFFSET			0
#define seCSCLPACKET_MAX_DATA_SIZE		(0xfff0)
#define seCSCLPACKET_MAX_OUT_DATA_SIZE	(500)
#define seCSCL_MULTIPLE_READ_MIN_SIZE	(5)
#define seCSCL_MULTIPLE_READ_WRITE		(0xFF)
#define seCSCLPACKET_JTAG_FULL_SPEED_MAX_DATA_SIZE	(56)
#define seCSCLPACKET_JTAG_MAX_DATA_SIZE	(500)


#define seCSCL_TEST_LOGIG_RESET 0
#define seCSCL_RUN_TEST_IDLE	1
#define seCSCL_JTAG_HDR			0
#define seCSCL_JTAG_TRLR		1
#define seCSCL_JTAG_SEQUENCE_0	0
#define seCSCL_JTAG_SEQUENCE_1	1

/* Data Formats */
typedef enum{
	seCSCL_READ_MGE = 0,				//0x000
	seCSCL_MGE_READ_DATA = 1,			//0x001
	seCSCL_WRITE_MGE = 2,				//0x002
	seCSCL_CMD_DONE = 3,				//0x003
	seCSCL_WRITE_CONFIG_REGS = 4,		//0x004
	seCSCL_READ_CONFIG_REGS = 5,		//0x005
	seCSCL_CONFIG_REGS_DATA = 6,		//0x006
	seCSCL_READ_MGE_STATUS = 7,			//0x007
	seCSCL_MGE_STATUS = 8,				//0x008
	seCSCL_RELOAD_FPGA = 9,				//0x009
	seCSCL_PROGRAM_FPGA = 0x00a,		//0x00a
	seCSCL_CMD_NAK = 0x00b,				//0x00b
	seCSCL_FPGA_DATA = 0x00c,			//0x00c
	seCSCL_JTAG_ENABLE = 0x00d,			//0x00d
	seCSCL_JTAG_DISABLE = 0x00e,		//0x00e
	seCSCL_JTAG_SYNC = 0x00f,			//0x00f
	seCSCL_JTAG_PREAMBLE = 0x010,		//0x010
	seCSCL_JTAG_POSTAMBLE = 0x011,		//0x011
	seCSCL_JTAG_HDR_TRLR = 0x012,		//0x012
	seCSCL_JTAG_DATA_SHIFT = 0x013,		//0x013
	seCSCL_JTAG_TDO_DATA = 0x014,		//0x014
	seCSCL_WRONG = 0xFF
} seCSCL_PACKET_TYPE;

//#define seCSCL_READ_MGE			0
//#define seCSCL_MGE_READ_DATA		1
//#define seCSCL_WRITE_MGE			2
//#define seCSCL_CMD_DONE			3
//#define seCSCL_WRITE_CONFIG_REGS	4
//#define seCSCL_READ_CONFIG_REGS	5
//#define seCSCL_CONFIG_REGS_DATA	6
//#define seCSCL_READ_MGE_STATUS	7
//#define seCSCL_MGE_STATUS			8
//#define seCSCL_WRONG				0xFF


// Prometheus Protocol Packets

typedef struct seCsclInterrupt {
	UInt8 byte0;
	UInt8 byte1;
} seCsclInterrupt;

typedef struct seCsclHeader {
	UInt8 byte0;
	UInt8 byte1;
} seCsclHeader;

typedef struct seCsclRdMgePkt {
	seCsclHeader hdr;
	UInt8 Addr0;
	UInt8 Addr1;
	UInt8 Addr2;
	UInt8 Param;
	UInt8 N0;
	UInt8 N1;
} seCsclRdMgePkt;

typedef struct seCsclMgeRdDataPkt {
	seCsclHeader hdr;
	UInt8 N0;
	UInt8 N1;
} seCsclMgeRdDataPkt; // Variable lenght packet

typedef struct seCsclWrMgeDataPkt {
	seCsclHeader hdr;
	UInt8 Addr0;
	UInt8 Addr1;
	UInt8 Addr2;
	UInt8 Param;
	UInt8 N0;
	UInt8 N1;
} seCsclWrMgeDataPkt; // Variable lenght packet

typedef struct seCsclCmdDonePkt {
	seCsclHeader hdr;
} seCsclCmdDonePkt; // Header Only packet

typedef struct seCsclWrConfigRegsPkt {
/*  MGE 0 */
	seCsclHeader hdr;
	UInt8 Mge0_ConfigA;
	UInt8 Mge0_ConfigB;
	UInt8 Mge0_ConfigC;
	UInt8 Mge0_ConfigD;
	UInt8 Mge0_ConfigE;
	UInt8 Mge0_ConfigF;
/*  MGE 1 */
	UInt8 Mge1_ConfigA;
	UInt8 Mge1_ConfigB;
	UInt8 Mge1_ConfigC;
	UInt8 Mge1_ConfigD;
	UInt8 Mge1_ConfigE;
	UInt8 Mge1_ConfigF;
/*  MGE 2 */
	UInt8 Mge2_ConfigA;
	UInt8 Mge2_ConfigB;
	UInt8 Mge2_ConfigC;
	UInt8 Mge2_ConfigD;
	UInt8 Mge2_ConfigE;
	UInt8 Mge2_ConfigF;
/*  MGE 3 */
	UInt8 Mge3_ConfigA;
	UInt8 Mge3_ConfigB;
	UInt8 Mge3_ConfigC;
	UInt8 Mge3_ConfigD;
	UInt8 Mge3_ConfigE;
	UInt8 Mge3_ConfigF;
} seCsclWrConfigRegsPkt; // Fixedlength packet

typedef struct seCsclRdConfigRegsPkt {
	seCsclHeader hdr;
} seCsclRdConfigRegsPkt; // Header Only packet

typedef struct seCsclConfigRegsDataPkt {
/*  MGE 0 */
	seCsclHeader hdr;
	UInt8 Mge0_ConfigA;
	UInt8 Mge0_ConfigB;
	UInt8 Mge0_ConfigC;
	UInt8 Mge0_ConfigD;
	UInt8 Mge0_ConfigE;
	UInt8 Mge0_ConfigF;
/*  MGE 1 */
	UInt8 Mge1_ConfigA;
	UInt8 Mge1_ConfigB;
	UInt8 Mge1_ConfigC;
	UInt8 Mge1_ConfigD;
	UInt8 Mge1_ConfigE;
	UInt8 Mge1_ConfigF;
/*  MGE 2 */
	UInt8 Mge2_ConfigA;
	UInt8 Mge2_ConfigB;
	UInt8 Mge2_ConfigC;
	UInt8 Mge2_ConfigD;
	UInt8 Mge2_ConfigE;
	UInt8 Mge2_ConfigF;
/*  MGE 3 */
	UInt8 Mge3_ConfigA;
	UInt8 Mge3_ConfigB;
	UInt8 Mge3_ConfigC;
	UInt8 Mge3_ConfigD;
	UInt8 Mge3_ConfigE;
	UInt8 Mge3_ConfigF;
} seCsclConfigRegsDataPkt; // Fixedlength packet

typedef struct seCsclRdMgeStatPkt {
	seCsclHeader hdr;
} seCsclRdMgeStatPkt; // Header Only packet

typedef struct seCsclMgeStatPkt {
	seCsclHeader hdr;
	UInt8 Mge0_Status;
	UInt8 Mge1_Status;
	UInt8 Mge2_Status;
	UInt8 Mge3_Status;
} seCsclMgeStatPkt;

/* Packets for FPGA Download */

typedef struct seCsclReloadFpgaPkt {
	seCsclHeader hdr;
} seCsclReloadFpgaPkt;

typedef struct seCsclProgramFpgaPkt {
	seCsclHeader hdr;
} seCsclProgramFpgaPkt;

typedef struct seCsclCmdNakPkt {
	seCsclHeader hdr;
} seCsclCmdNakPkt;

typedef struct seCsclFpgaDataPkt {
	seCsclHeader hdr;
	UInt8 N0;
	UInt8 N1;
	UInt8 N2;
	UInt8 N3;
} seCsclFpgaDataPkt;

typedef struct seCsclJtagEnablePkt {
	seCsclHeader hdr;
} seCsclJtagEnablePkt;

typedef struct seCsclJtagDisablePkt {
	seCsclHeader hdr;
} seCsclJtagDisablePkt;

typedef struct seCsclJtagSyncPkt {
	seCsclHeader hdr;
} seCsclJtagSyncPkt;

typedef struct seCsclJtagPreamblePkt {
	seCsclHeader hdr;
} seCsclJtagPreamblePkt;

typedef struct seCsclJtagPostamblePkt {
	seCsclHeader hdr;
} seCsclJtagPostamblePkt;

typedef struct seCsclJtagHdrTrlrPkt {
	seCsclHeader hdr;
	UInt8 N0;
	UInt8 N1;
} seCsclJtagHdrTrlrPkt;


typedef struct seCsclJtagDataShiftPkt {
	seCsclHeader hdr;
	UInt8 N0;
	UInt8 N1;
} seCsclJtagDataShiftPkt;


typedef struct seCsclJtagTdoDataPkt {
	seCsclHeader hdr;
	UInt8 N0;
	UInt8 N1;
} seCsclJtagTdoDataPkt;


/* End of packets for FPGA Download */

extern seMgeDescriptor seMgeDesc;

/* Function Prototypes */
Boolean		seCsclSetup( UInt32 Mge, UInt32 InterfaceFlags, seCsclConfigRegs *pConfigRegs );
Boolean		seCsclReset( void );
UInt32		seCsclGetHwFpgaRev(void );
UInt32		seCsclGetConfigReg( int RegNum );
void		seCsclSetConfigReg( int RegNum, UInt8 value );
void		seCsclGetUsbTransportConfig( void );
Boolean		seCsclRdConfigRegs(void);
void		seCsclWrConfigRegs( void );
void		seCsclDispConfigRegs( seCsclConfigRegs *pRegInfo );
UInt32		seCsclRdMge( UInt32 address, UInt32 length, UInt32 params);
Boolean		seCsclWrMge( UInt32 address, UInt32 bytes, UInt32 params, UInt32 Value);
UInt32		seCsclRdMgeStat(void);
void		seCsclDispMgeConfigDesc( void );
void		seCsclUpdateConfigRegs( void );
UInt32		seCsclBurstRdMge( UInt32 address, UInt32 bytes, UInt32 param, UInt8 * pData);
Boolean		seCsclBurstWrMge( UInt32 address, UInt32 bytes, UInt32 param, UInt8 * pBuffer);
Boolean		seCsclGetMgeInterrupt( UInt32 * );
Boolean		seCsclInterruptsSetup( void );
Boolean		seCsclInterruptsReset( void );


Boolean		seCsclReloadFpga(void);
Boolean		seCsclProgramFpga(void);
Boolean		seCsclJtagEnable(void);
Boolean		seCsclJtagDisable(void);

#if defined(__cplusplus)
extern "C"	{
Boolean		seCsclJtagDataShift( UInt32 rlength, UInt32 flag, UInt8 * pOutBuffer, UInt8 * pInBuffer);
Boolean		seCsclFpgaData( UInt32 bytes, UInt8 * pBuffer);
UInt32		seCsclJtagTdoData( UInt32 rlength, UInt8 * pData);
void		seCsclJtagSync( UInt32 Flag );
void		seCsclJtagPreamble( UInt32 Flag );
void		seCsclJtagPostamble( UInt32 Flag );
void		seCsclJtagHdrTrlr( UInt32 Flag, UInt32 RLength );
}
#else
Boolean		seCsclJtagDataShift( UInt32 slength, UInt32 flag, UInt8 * pOutBuffer, UInt8 * pInBuffer);
Boolean		seCsclFpgaData( UInt32 bytes, UInt8 * pBuffer);
UInt32		seCsclJtagTdoData( UInt32 rlength, UInt8 * pData);
void		seCsclJtagSync( UInt32 Flag );
void		seCsclJtagPreamble( UInt32 Flag );
void		seCsclJtagPostamble( UInt32 Flag );
void		seCsclJtagHdrTrlr( UInt32 Flag, UInt32 RLength );

#endif
/*-----------------------------------------------------------------------------
	Boolean		seCsclSetup(UInt32 InterfaceFlags);
	Sets up the Prometheus Client Server Communication Layer.

	Argumets:	Chip Specific Flags are passed to this layer from application.
	Returns: TRUE if CSCL is not a "NULL Layer".
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    UInt32      seCsclGetConfigReg( int RegNum );
    Argumets:
    RegA --- RegNum = 0
    RegB --- RegNum = 1
    RegC --- RegNum = 2
    RegD --- RegNum = 3
    RegE --- RegNum = 4
    RegF --- RegNum = 5
    Returns the value (a byte) of the FPGA configuration register
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    void        seCsclSetConfigReg( int RegNum, UInt8 Value );
    Sets the FPGA configuration register to the 'Value' (byte)
    Argumets:
    RegA --- RegNum = 0
    RegB --- RegNum = 1
    RegC --- RegNum = 2
    RegD --- RegNum = 3
    RegE --- RegNum = 4
    RegF --- RegNum = 5
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	void		seCsclGetUsbTransportConfig(void);
	Gets USB Configuration Descriptor and displays the retrieved information.
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	void		seCsclRdConfigRegs(void);
	Gets CSCL Configuration Registers from the board.
	The function assigns the current configuration values to the global
	segCsclConfigRegsInfo.
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	void		seCsclWrConfigRegs(void);
	Sets CSCL Configuration Registers by sending the CSCL "write config" packet
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	void		seCsclDispConfigRegs(seCsclConfigRegs *pRegInfo);
	Displays the CSCL Configuration Registers.
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	UInt32		seCsclRdMge( UInt32 address, UInt32 length, UInt32 params);
	Sets the packet to Server in attempt to read Mge value.
	Reads the responce from Server.

	Arguments:
	address		--- Data Address
	length		--- Data width in bytes
	param		--- Data Read packet flags
	Returns		--- UInt32 Data value read from Server
------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Boolean		seCsclWrMge( UInt32 address, UInt32 bytes, UInt32 params, UInt32 Value);
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
UInt32		seCsclRdMgeStat(void);
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
void		seCsclDispMgeConfigDesc(void);
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
void		seCsclUpdateConfigRegs(void);
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
UInt32		seCsclBurstRdMge(  UInt32 address, UInt32 bytes, UInt32 param, UInt8 * pData);
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Boolean		seCsclBurstWrMge(  UInt32 address, UInt32 bytes, UInt32 param, UInt8 * pBuffer);
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Boolean		seCsclGetMgeInterrupt(UInt32 *);
------------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif


#endif //seCSCL_H_INCLUDED
