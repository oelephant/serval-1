//----------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//----------------------------------------------------------------------------
#include "usb_transport.h"
#include "cscl.h"
#include "os.h"
#include "zzzCsclMutex.h"


/* Client Server Communication Layer - Prometheus Protocol */

/* Globals */
// Mge Descriptor keeps the host interface parameters, memory map information.
// The descriptor is declated as external in the cscl.h. The HCL files 
// can use the descriptor to access CSCL layer data.
seMgeDescriptor seMgeDesc = { 0, };

seCSCLJtagDescr seJtagDesc= { seCSCLPACKET_JTAG_MAX_DATA_SIZE, };


/* Private functions */
static seCSCL_PACKET_TYPE		csclParsePktHdr(UInt8 *pBuf);
static void						csclParseConfigRegsPkt(UInt8 * pBuf,seCsclConfigRegs *pRegInfo);
static Boolean					csclWaitForDone(Boolean fWait);
static void						seCsclInitGlobals( UInt32 Mge,UInt32 InterfaceFlags, seCsclConfigRegs *pConfigRegs );

/* Private data */
// The Prometheus Configuration Info is alocated as a private structure for all mges.
//  This structure is a shadow for the hw configuration registers.
static seCsclConfigRegs segCsclConfigRegsInfo[seMaxMgeNum] ={	0x00,0x00,0x00,0x00,0x00,0x00,
																0x00,0x00,0x00,0x00,0x00,0x00,
																0x00,0x00,0x00,0x00,0x00,0x00,
																0x00,0x00,0x00,0x00,0x00,0x00,};


/*-----------------------------------------------------------------------------
	seCsclInitGlobals( UInt32 InterfaceFlags )
------------------------------------------------------------------------------*/
static void seCsclInitGlobals( UInt32 Mge, UInt32 InterfaceFlags, seCsclConfigRegs *pConfigRegs )
{
	UNREFERENCED_PARAMETER(InterfaceFlags);
	UNREFERENCED_PARAMETER(pConfigRegs);
	seMgeDesc.MgeNum = Mge;
	seMgeDesc.InterfaceFlags =	InterfaceFlags;
	seMgeDesc.MgeMemAddr =		seMGE_MEM_ADDR_OFFSET;
	seMgeDesc.MgeRegAddr =		seMGE_REG_ADDR_OFFSET;
	seMgeDesc.RegsInfo =		&segCsclConfigRegsInfo[seMgeDesc.MgeNum];

	if ( pConfigRegs != NULL)
	{
		segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegA = pConfigRegs->ConfigRegA;
		segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegB = pConfigRegs->ConfigRegB;
		segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegC = pConfigRegs->ConfigRegC;
		segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegD = pConfigRegs->ConfigRegD;
		segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegE = pConfigRegs->ConfigRegE;
		segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegF = pConfigRegs->ConfigRegF;
	}
}


/*-----------------------------------------------------------------------------
	seCsclGetFpgaRev(void)
------------------------------------------------------------------------------*/
UInt32 seCsclGetHwFpgaRev(void )
{
	if ( seCsclRdConfigRegs() )
		return segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegF;
	else
		return (UInt32)UNKNOWN_FPGA_REVISION;
}
/*-----------------------------------------------------------------------------
	seCsclGetConfigReg(int RegNum ), RegNum is a number from 0 to 5
------------------------------------------------------------------------------*/
UInt32 seCsclGetConfigReg( int RegNum )
{
	if ( seCsclRdConfigRegs() )
	{
		UInt8 * ptr = &(segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegA);
		return ( ptr[RegNum]);
	}
	else
		return 0;
}

/*-----------------------------------------------------------------------------
    seCsclSetConfigReg (int RegNum, UInt8 value), RegNum is a number from 0 to 5
------------------------------------------------------------------------------*/
void seCsclSetConfigReg( int RegNum, UInt8 value )
{
	if ( seCsclRdConfigRegs() )
	{
		UInt8 * ptr = &(segCsclConfigRegsInfo[seMgeDesc.MgeNum].ConfigRegA);
		ptr[RegNum] = value;
		seCsclWrConfigRegs();
	}
}

/*-----------------------------------------------------------------------------
	seCsclSetup(UInt32 Mge, UInt32 InterfaceFlags, seCsclConfigRegs *pConfigRegs )
------------------------------------------------------------------------------*/
Boolean seCsclSetup(UInt32 Mge, UInt32 InterfaceFlags, seCsclConfigRegs *pConfigRegs )
{
	if ( !seUsbTransportSetup())
		return FALSE;

	seUsbTransportGetPipeInfo();

	seCsclInitGlobals(Mge, InterfaceFlags, pConfigRegs);

	seCsclWrConfigRegs();

	seCSCLPrintf( "CSCL HW revision %d\n", seCsclGetHwFpgaRev());

	return TRUE;
}

/*-----------------------------------------------------------------------------
	seCsclGetUsbTransportConfig() Gets USB Configuration descriptor and displays the retrieved 
	information
------------------------------------------------------------------------------*/
void seCsclGetUsbTransportConfig()
{
	seUsbTransportGetConfig();
}

/*-----------------------------------------------------------------------------
	seCsclDispMgeConfigDesc()
------------------------------------------------------------------------------*/
void seCsclDispMgeConfigDesc(void)
{
	seCSCLPrintf( "\nMge %d Interface Flags 0x%08x Mem Addr 0x%08x Reg Addr 0x%08x",
		seMgeDesc.MgeNum, seMgeDesc.InterfaceFlags, seMgeDesc.MgeMemAddr, seMgeDesc.MgeRegAddr );

	seCsclDispConfigRegs( &segCsclConfigRegsInfo[0] );
}

/*-----------------------------------------------------------------------------
	seCsclUpdateConfigRegs()
------------------------------------------------------------------------------*/
void seCsclUpdateConfigRegs(void)
{
	seMgeDesc.RegsInfo = &segCsclConfigRegsInfo[seMgeDesc.MgeNum];
}
/*-----------------------------------------------------------------------------
	seCsclInterruptsSetup(void)
------------------------------------------------------------------------------*/
Boolean seCsclInterruptsSetup(void)
{
	return seUsbTransportInterruptsSetup();
}
/*-----------------------------------------------------------------------------
	seCsclInterruptsReset(void)
------------------------------------------------------------------------------*/
Boolean seCsclInterruptsReset(void)
{
	// Reset valid only after the Cscl has been initialized
	return ( seUsbTransportInterruptsReset());
}
/*-----------------------------------------------------------------------------
	seCsclSetup(void)
------------------------------------------------------------------------------*/
Boolean seCsclReset( void )
{
	// Reset valid only after the Cscl has been initialized 
	if ( seUsbTransportReset())
	{
		return seCsclSetup( seMgeDesc.MgeNum, seMgeDesc.InterfaceFlags, NULL );	
	}
	else
		return FALSE;
}

/*-----------------------------------------------------------------------------
	seCsclWrConfigRegs() 
------------------------------------------------------------------------------*/
void seCsclWrConfigRegs(void)
{
	int i = seMaxMgeNum;
	seCsclConfigRegs *pRegInfo = &segCsclConfigRegsInfo[0];
	seCsclWrConfigRegsPkt *pPkt = (seCsclWrConfigRegsPkt*)malloc (sizeof(seCsclWrConfigRegsPkt)); // TBD check the null
	UInt8 * pBuf = NULL;

	seCsclDispConfigRegs(&segCsclConfigRegsInfo[0]);

	memset( (PVOID) pPkt, 0, sizeof(seCsclWrConfigRegsPkt));
	pPkt->hdr.byte0 =  seCSCL_WRITE_CONFIG_REGS;
	pPkt->hdr.byte1 = 0;

	pBuf = (UInt8 *)pPkt + 2;

	do {
		*pBuf++ = pRegInfo->ConfigRegA; 
		*pBuf++ = pRegInfo->ConfigRegB;
		*pBuf++ = pRegInfo->ConfigRegC;
		*pBuf++ = pRegInfo->ConfigRegD;
		*pBuf++ = pRegInfo->ConfigRegE;
		*pBuf++ = pRegInfo->ConfigRegF;
		pRegInfo++;
	} while ( --i > 0);

	seUsbTransportPktOut(seCSCL_WRITE_CONFIG_REGS, (PVOID) pPkt, sizeof(seCsclWrConfigRegsPkt), FALSE);

	free( (PVOID)pPkt );

	csclWaitForDone(TRUE);
}

/*-----------------------------------------------------------------------------
	seCsclRdConfigRegs()
------------------------------------------------------------------------------*/
Boolean seCsclRdConfigRegs(void)
{
	UInt8 *pBuf = NULL;
	seCSCL_PACKET_TYPE pktType = seCSCL_WRONG;
	UInt32 TimeoutMS = 5; // asume 5 frames for the Full speed devices ( 1.0 USB spec)
	UInt32 Timer;

	seUsbTransportPktOut(seCSCL_READ_CONFIG_REGS, NULL, 2, TRUE);

	pBuf = (UInt8 *)malloc(sizeof(seCsclConfigRegsDataPkt));
	if (!pBuf)
	{
		seASSERT( FALSE, ("Unable to allocate transfer buffer"));
		return FALSE;
	}

	Timer = seOsGetTimeMS();
	do {
		pktType = seCSCL_WRONG;
		memset( (PVOID)pBuf, 0, sizeof(seCsclConfigRegsDataPkt));
		if ( seUsbTransportPktIn( pBuf, sizeof(seCsclConfigRegsDataPkt), FALSE) == 0)
			break;

		if ( (pktType = csclParsePktHdr( pBuf )) == seCSCL_CONFIG_REGS_DATA)
		{
			csclParseConfigRegsPkt( pBuf , &segCsclConfigRegsInfo[0]);
			break;
		}
		else
			seASSERT( FALSE, ("seCsclRdConfigRegs:Bogus packet received 0x%x, expected 0x%x", pktType,seCSCL_CONFIG_REGS_DATA));
	} while ( (seOsGetTimeMS() - Timer) < TimeoutMS );

	free((PVOID)pBuf);
	return TRUE;
}

/*-----------------------------------------------------------------------------
	seCsclDispConfigRegs()
------------------------------------------------------------------------------*/
void seCsclDispConfigRegs(seCsclConfigRegs *pRegInfo)
{
	int i = seMaxMgeNum;

	do {
		seCSCLPrintf( "\nCSCL:MGE %d Regs: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", (seMaxMgeNum -i),
					pRegInfo->ConfigRegA, pRegInfo->ConfigRegB, pRegInfo->ConfigRegC,
					pRegInfo->ConfigRegD, pRegInfo->ConfigRegE, pRegInfo->ConfigRegF );
		pRegInfo++;
	} while ( --i > 0);

	seCSCLPrintf( "\n");
}

/*-----------------------------------------------------------------------------
	seCsclWrMge()
------------------------------------------------------------------------------*/
Boolean seCsclWrMge(/* UInt32 mge,*/ UInt32 address, UInt32 bytes, UInt32 param, UInt32 value)
{
	seCsclWrMgeDataPkt * pPkt;
	int bufferSize = 0;
	UInt8 * pData = NULL;
	bufferSize = sizeof(seCsclWrMgeDataPkt) + bytes; 

	//seASSERT(bytes <=4), ("seCsclRdMge() too many bytes for a single write") );
	//if (bytes > 4 )
	//	return 0;

	pPkt = (seCsclWrMgeDataPkt *)malloc(bufferSize);

	if (!pPkt)
	{
		seASSERT( FALSE, ("Unable to allocate transfer buffer"));
		return FALSE;
	}

	memset( (PVOID) pPkt, 0, bufferSize);

	pPkt->hdr.byte0 = seCSCL_WRITE_MGE; // MGE TBD
	pPkt->Addr0		= (UInt8) (address & 0xFF);
	pPkt->Addr1		= (UInt8) ((address >> 8 ) & 0xFF);
	pPkt->Addr2		= (UInt8) ((address >> 16 ) & 0xFF);
	pPkt->Param		= (UInt8) (param & 0xFF);
	pPkt->N0		= (UInt8) (bytes & 0xFF);
	pPkt->N1		= (UInt8) ((bytes >> 8) & 0xFF);
	pData = (UInt8 *) (&(pPkt->N1));

	pData ++;

	if (bytes == 1)
	{
		* pData++ = (UInt8) (value & 0xFF);
	}
	else if (bytes == 2)
	{
		* pData++ = (UInt8) (value & 0xFF);
		* pData++ = (UInt8) ((value >>  8) & 0xFF);
	}
	else if (bytes == 3)
	{
		* pData++ = (UInt8) (value & 0xFF);
		* pData++ = (UInt8) ((value >>  8) & 0xFF);
		* pData++ = (UInt8) ((value >> 16) & 0xFF);
	}
	else if (bytes == 4)
	{
		* pData++ = (UInt8) (value & 0xFF);
		* pData++ = (UInt8) ((value >>  8) & 0xFF);
		* pData++ = (UInt8) ((value >> 16) & 0xFF);
		* pData++ = (UInt8) ((value >> 24) & 0xFF);
	}
	else
	{

	}

	seUsbTransportPktOut(seCSCL_WRITE_MGE, (UInt8 *) pPkt, bufferSize, FALSE);

	free((PVOID) pPkt);

	return (csclWaitForDone(FALSE));
}

/*-----------------------------------------------------------------------------
	seCsclBurstWrMge()
------------------------------------------------------------------------------*/
Boolean seCsclBurstWrMge( /* UInt32 mge,*/ UInt32 address, UInt32 bytes, UInt32 param, UInt8 * pBuffer)
{
	seCsclWrMgeDataPkt * pPkt;
	int bufferSize = 0;
	UInt8 * pData = NULL;
	UInt32 bytesLeft = bytes;
	UInt32 bytesSent = 0;
	Boolean lock = FALSE;

	if (bytes == 0 || pBuffer == NULL)
		return FALSE;

/*-----------------------------------------------------------------------------------------
	Do Prometheus CSCL packet fragmentation if nessesary
-------------------------------------------------------------------------------------------*/
	do
	{
		bytes = ( bytesLeft > /*seCSCLPACKET_MAX_OUT_DATA_SIZE*/seCSCLPACKET_MAX_DATA_SIZE ) ? (/*seCSCLPACKET_MAX_OUT_DATA_SIZE*/seCSCLPACKET_MAX_DATA_SIZE) : (bytesLeft);
		bytesLeft -= bytes;
		lock = ( bytesLeft > 0 ) ? TRUE : FALSE;
/*-----------------------------------------------------------------------------------------
		Send a request packet. Requesting Prometheus Server to write the <bytes> of data.
-------------------------------------------------------------------------------------------*/
		
		bufferSize = sizeof(seCsclWrMgeDataPkt) + bytes;
		pPkt = (seCsclWrMgeDataPkt *)malloc(bufferSize);

		if (!pPkt)
		{
			seASSERT( FALSE, ("Unable to allocate transfer buffer"));
			return FALSE;
		}

		memset( (PVOID) pPkt, 0, bufferSize);

		pPkt->hdr.byte0 = seCSCL_WRITE_MGE; // MGE TBD
		pPkt->Addr0		= (UInt8) (address & 0xFF);
		pPkt->Addr1		= (UInt8) ((address >> 8 ) & 0xFF);
		pPkt->Addr2		= (UInt8) ((address >> 16 ) & 0xFF);
		pPkt->Param		= (UInt8) (param & 0xFF);
		pPkt->N0		= (UInt8) (bytes & 0xFF);
		pPkt->N1		= (UInt8) ((bytes >> 8) & 0xFF);
		pData = (UInt8 *) &(pPkt->N1);

		pData ++;
		memcpy( pData, pBuffer, bytes);

		bytesSent = seUsbTransportPktOut(seCSCL_WRITE_MGE, (UInt8 *) pPkt, bufferSize, lock);
		pBuffer += bytes;
		address += bytes;
//		address = ((seMgeDesc.InterfaceFlags & seCSCL_MGE_DIRECT)==seCSCL_MGE_DIRECT) ? (address + bytes) : address;

		free((PVOID) pPkt);
	} while ( bytesLeft > 0 );

	return (csclWaitForDone(FALSE));
}

/*-----------------------------------------------------------------------------
	seCsclBurstRdMge() -- There is no limitation (UInt32) on the number of bytes requested 
	by application.
	Packet fragmentation shall be done since Prometheus CSCL protocol has 16 bit limitation 
	for the length of the data.
------------------------------------------------------------------------------*/
UInt32 seCsclBurstRdMge( /*UInt32 mge,*/ UInt32 address, UInt32 bytes, UInt32 param, UInt8 * pData)
{
	seCsclRdMgePkt * pPkt;
	seCsclMgeRdDataPkt * pDataPkt;
	UInt32 packetLength = 0, totalLength = 0;
	int bufferSize = 0;
	seCSCL_PACKET_TYPE pktType = seCSCL_WRONG;
	UInt32 bytesLeft = bytes;
	UInt32 lengthReceived = 0;
	UInt32 length2Expect = 0;
	Boolean lock = FALSE;

	if (!pData) 
		return 0;

/*-----------------------------------------------------------------------------------------
	Do Prometheus CSCL packet fragmentation if nessesary
-------------------------------------------------------------------------------------------*/
	do
	{
		bytes = ( bytesLeft > seCSCLPACKET_MAX_DATA_SIZE ) ? (seCSCLPACKET_MAX_DATA_SIZE) : (bytesLeft);
		bytesLeft -= bytes;
		lock = ( bytesLeft != 0 ) ? TRUE : FALSE;

/*-----------------------------------------------------------------------------------------
		Send a request packet. Requesting <bytes> of data from the Prometheus Server
-------------------------------------------------------------------------------------------*/
		bufferSize = sizeof(seCsclRdMgePkt);
		pPkt = (seCsclRdMgePkt *)malloc(bufferSize);
		if (!pPkt)
		{
			seASSERT( FALSE, ("Unable to allocate transfer buffer"));
			return 0;
		}
		memset( (PVOID)pPkt, 0, bufferSize);

		pPkt->hdr.byte0 = seCSCL_READ_MGE; // MGE TBD
		pPkt->Addr0		= (UInt8) (address & 0xFF);
		pPkt->Addr1		= (UInt8) ((address >> 8 ) & 0xFF);
		pPkt->Addr2		= (UInt8) ((address >> 16 ) & 0xFF);
		pPkt->Param		= (UInt8) (param & 0xFF);
		pPkt->N0		= (UInt8) (bytes & 0xFF);
		pPkt->N1		= (UInt8) ((bytes >> 8) & 0xFF);

		seUsbTransportPktOut(seCSCL_READ_MGE, (UInt8 *) pPkt, bufferSize, TRUE);
		free( pPkt);
/*-----------------------------------------------------------------------------------------
		Prepare to receive requested data
-------------------------------------------------------------------------------------------*/
		lengthReceived = 0;

		// tpk Commented out just in case if Prometheus state machine works
		// with exact number of odd bytes. Then waiting for <lengthReceived < bytes> could fail.
		//bytes = (bytes & 1) ? (bytes+1) : bytes; // round up 
		bufferSize = sizeof( seCsclMgeRdDataPkt ) + ((bytes+1) & ~1);

		pDataPkt = (seCsclMgeRdDataPkt *)malloc( bufferSize);

		if (!pDataPkt) 
		{
			seASSERT( FALSE, ("Unable to allocate a transfer buffer"));
			return 0;
		}

		do {
			UInt8 *pBuf = NULL;
			pktType = seCSCL_WRONG;
			memset( (PVOID)pDataPkt, 0, bufferSize);

			// Keep reading untill a desired number (Prometheus protocol) of data received.
			packetLength = seUsbTransportPktIn( (UInt8 *)pDataPkt, bufferSize, lock);

			if (packetLength == 0)
				continue; // in case of the glitch give it another try
				//break;

			pBuf = (UInt8 *)pDataPkt;

			if ( ((pktType = csclParsePktHdr( (UInt8 *) pDataPkt)) == seCSCL_MGE_READ_DATA) && lengthReceived ==0)
			{
				// First Packet
				pBuf += 2;
				length2Expect = pBuf[0] + (pBuf[1]<<8);
#if 1
				if ((length2Expect&(~1)) != (bytes&(~1)))
				{
					//seASSERT( FALSE, ("seCsclBurstRdMge: Wrong data length. Pkt type 0x%x waiting for %d bytes, coming %d bytes)", pktType, bytes, length2Expect));
					continue;
				}
#endif
				pBuf += 2;
				packetLength -= 4; // adjust for the CSCL header
				memcpy( pData, pBuf, packetLength);
				pData +=packetLength;
			}
			else if ( lengthReceived !=0 )
			{
				// Following packets
				memcpy( pData, pBuf, packetLength);
				pData +=packetLength;
			}
			else
			{
				seASSERT( FALSE, ("seCsclBurstRdMge:Bogus packet received 0x%x, expected 0x%x (len rcvd %d)", pktType, seCSCL_MGE_READ_DATA, lengthReceived));
			}
			lengthReceived += packetLength;

		} while (lengthReceived < bytes);
		// round down for the odd /even number of bytes.
		lengthReceived = ( length2Expect < lengthReceived ) ? length2Expect : lengthReceived ;
		free((PVOID)pDataPkt);
		totalLength +=lengthReceived;
		address += bytes;
//		address = ((seMgeDesc.InterfaceFlags & seCSCL_MGE_DIRECT)==seCSCL_MGE_DIRECT) ? (address + bytes) : address;
	} while ( bytesLeft > 0 );

	//return ( ( length2Expect < totalLength ) ? length2Expect : totalLength );
	return ( totalLength );
}

/*-----------------------------------------------------------------------------
	seCsclRdMge()
------------------------------------------------------------------------------*/
UInt32 seCsclRdMge( UInt32 address, UInt32 bytes, UInt32 param)
{
	seCsclRdMgePkt * pPkt;
	seCsclMgeRdDataPkt * pData;
	int bufferSize = 0;
	UInt32 regValue = 0;
	seCSCL_PACKET_TYPE pktType = seCSCL_WRONG;
	UInt32 TimeoutMS = 5; // assume 5 frames for the Full speed devices (1.0 USB spec)
	UInt32 Timer;

	//seASSERT(bytes <=4), ("seCsclRdMge() too many bytes for a single read") );

	bufferSize = sizeof(seCsclRdMgePkt);

	pPkt = (seCsclRdMgePkt *)malloc(bufferSize);

	if (!pPkt)
	{
		seASSERT( FALSE, ("Unable to allocate transfer buffer"));
		return regValue;
	}

	memset( (PVOID)pPkt, 0, bufferSize);

	pPkt->hdr.byte0 = seCSCL_READ_MGE; // MGE TBD
	pPkt->Addr0		= (UInt8) (address & 0xFF);
	pPkt->Addr1		= (UInt8) ((address >> 8 ) & 0xFF);
	pPkt->Addr2		= (UInt8) ((address >> 16 ) & 0xFF);
	pPkt->Param		= (UInt8) (param & 0xFF);
	pPkt->N0		= (UInt8) (bytes & 0xFF);
	pPkt->N1		= (UInt8) ((bytes >> 8) & 0xFF);

	seUsbTransportPktOut(seCSCL_READ_MGE, (UInt8 *) pPkt, bufferSize, TRUE);
	
	bufferSize = sizeof( seCsclWrMgeDataPkt ) + bytes;

	pData = (seCsclMgeRdDataPkt *)realloc((UInt8 *)pPkt, bufferSize);

	if (!pData)
	{
		seASSERT( FALSE, ("Unable to re allocate transfer buffer"));
		return 0;
	}
	Timer = seOsGetTimeMS();
	do {
		UInt8 *pBuf = 0;
		int packetLength = 0;
		pktType = seCSCL_WRONG;
		memset( (PVOID)pData, 0, bufferSize);
		if ( seUsbTransportPktIn( (UInt8 *) pData, bufferSize, FALSE) == 0)
			continue;  // in case of a glitch give it another try
			//break;
		if ( (pktType = csclParsePktHdr( (UInt8 *) pData)) == seCSCL_MGE_READ_DATA)
		{
			pBuf = (UInt8 *)pData;
			pBuf += 2;
			packetLength = pBuf[0] + (pBuf[1] << 8);
			pBuf += 2;
			switch (bytes)
			{
				case 4: regValue = pBuf[0] + (pBuf[1] << 8) + (pBuf[2] << 16) + (pBuf[3] << 24); break;
				case 2: regValue = pBuf[0] + (pBuf[1] << 8); break;
				case 1: regValue = pBuf[0]; break;
				default: break;
			}
			break;
		}
		else
		{
			seASSERT( FALSE, ("seCsclRdMge:Bogus packet received 0x%x, expected 0x%x ", pktType, seCSCL_MGE_READ_DATA ));
		}
	} while ( (seOsGetTimeMS() - Timer) < TimeoutMS );

	free((PVOID)pData);

	return regValue;
}


/*-----------------------------------------------------------------------------
	seCsclRdMgeStat()
------------------------------------------------------------------------------*/
UInt32 seCsclRdMgeStat(void)
{
	seCsclMgeStatPkt * pPkt;
	int bufferSize = 0;
	UInt32 regValue = 0;
	seCSCL_PACKET_TYPE pktType = seCSCL_WRONG;
	UInt32 TimeoutMS = 5; // asume 5 frames for the Full speed devices ( 1.0 USB spec)
	UInt32	Timer;

	seUsbTransportPktOut(seCSCL_READ_MGE_STATUS, NULL, 2, TRUE);

	bufferSize = sizeof( seCsclMgeStatPkt ) + 20;

	// Now get the entire descriptor
	pPkt = (seCsclMgeStatPkt * ) malloc( bufferSize);

	if (!pPkt)
	{
		seASSERT( FALSE, ("Unable to allocate transfer buffer"));
		return 0;
	}

	Timer = seOsGetTimeMS();
	do {
		pktType = seCSCL_WRONG;
		memset((PVOID )pPkt, 0, bufferSize);
		if ( seUsbTransportPktIn( (UInt8 *) pPkt, bufferSize, FALSE) == 0)
			break;

		if ( (pktType = csclParsePktHdr( (UInt8 *) pPkt)) == seCSCL_MGE_STATUS)
		{
			regValue = pPkt->Mge0_Status + ((pPkt->Mge1_Status) << 8) + ((pPkt->Mge2_Status) << 16) + ((pPkt->Mge3_Status) << 24);
			break;
		}
		else
			seASSERT( FALSE, ("seCsclRdMgeStat:Bogus packet received 0x%x, expected 0x%x", pktType, seCSCL_MGE_STATUS));
	} while ( (seOsGetTimeMS() - Timer) < TimeoutMS );

	free((PVOID)pPkt);

	return regValue;
}


/*-----------------------------------------------------------------------------
	seCsclGetMgeInterrupt()
------------------------------------------------------------------------------*/
Boolean seCsclGetMgeInterrupt( UInt32 * pInt)
{
	seCsclInterrupt IntPkt;
	Boolean fResult;
	
	fResult = seUsbTransportIntPktIn( &IntPkt, 2);
	
	// Comment out next line since the pipe failure shall be allowed as 
	// a condition. In this case the resulting numebr of bytes is equal to 0
	// seASSERT( fResult, ("seCsclGetMgeInterrupt() Interrupt pipe failure") );

	if ( fResult )
		*pInt = IntPkt.byte0;
	else
		*pInt = 0;

	return fResult;
}


/*-----------------------------------------------------------------------------
	Private csclParsePktHdr() returns a CSCL packet type 
------------------------------------------------------------------------------*/
static seCSCL_PACKET_TYPE csclParsePktHdr(UInt8 *pBuf)
{
	return (pBuf[0] & 0x3F);
}

/*-----------------------------------------------------------------------------
	csclParseConfigRegsPkt()
------------------------------------------------------------------------------*/
static void csclParseConfigRegsPkt(UInt8 * pBuf,seCsclConfigRegs *pRegInfo)
{
	int i = seMaxMgeNum;
	seCSCL_PACKET_TYPE pktType = csclParsePktHdr( pBuf);

	if ( pktType != seCSCL_CONFIG_REGS_DATA)
		return;

	pBuf += 2;

	do {
		pRegInfo->ConfigRegA=*pBuf++;
		pRegInfo->ConfigRegB=*pBuf++;
		pRegInfo->ConfigRegC=*pBuf++;
		pRegInfo->ConfigRegD=*pBuf++;
		pRegInfo->ConfigRegE=*pBuf++;
		pRegInfo->ConfigRegF=*pBuf++;
		pRegInfo++;
	} while ( --i > 0);
}

/*-----------------------------------------------------------------------------

------------------------------------------------------------------------------*/
static Boolean csclWaitForDone(Boolean fWait)
{
	UInt8 * pBuf = NULL;
	int packetLength = 0;
	Boolean fResult = FALSE;
	seCSCL_PACKET_TYPE pktType = seCSCL_WRONG;
	UInt32 Timer;
	UInt32 TimeoutMS = 5; // asume 5 frames for the Full speed devices ( 1.0 USB spec)

	if (!fWait)
		return TRUE;

	pBuf = malloc (26 /*sizeof(seCsclCmdDonePkt)*/);

	Timer = seOsGetTimeMS();
	do {
		pktType = seCSCL_WRONG;
		memset ((PVOID)pBuf, 0, 26);

		if ( ( packetLength = seUsbTransportPktIn( pBuf, 26/* sizeof(seCsclCmdDonePkt)*/, FALSE)) == 0)
		{
			fResult = FALSE;
			break;
		}
		else if ( (pktType = csclParsePktHdr( pBuf )) == seCSCL_CMD_NAK)
		{
			fResult = FALSE;
			break;
		}
		if ( (pktType = csclParsePktHdr( pBuf )) == seCSCL_CMD_DONE)
		{
			fResult = TRUE;
			break;
		}
		else
		{
			seASSERT( FALSE, ("csclWaitForDone:Bogus packet received 0x%x, expected 0x%x", pktType, seCSCL_CMD_DONE));
			fResult = FALSE;
		}

	} while ( (seOsGetTimeMS() - Timer) < TimeoutMS );

	free( (PVOID)pBuf );

	return fResult;
}
/*-----------------------------------------------------------------------------
	FPGA DOWNLOAD COMMANDS AND RESPONSES
------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
	seCsclReloadFpga()
------------------------------------------------------------------------------*/
Boolean seCsclReloadFpga(void)
{
	seUsbTransportPktOut(seCSCL_RELOAD_FPGA, NULL, 0, FALSE);

	return csclWaitForDone(TRUE);
}

/*-----------------------------------------------------------------------------
	seCsclProgramFpga()
------------------------------------------------------------------------------*/
Boolean seCsclProgramFpga(void)
{
	seUsbTransportPktOut(seCSCL_PROGRAM_FPGA, NULL, 0, FALSE);

	return csclWaitForDone(TRUE);
}


/*-----------------------------------------------------------------------------
	seCsclFpgaData()
------------------------------------------------------------------------------*/
Boolean seCsclFpgaData( UInt32 bytes, UInt8 * pBuffer)
{
	seCsclFpgaDataPkt * pPkt;
	int bufferSize = 0;
	UInt8 * pData = NULL;
	UInt32 bytesLeft = bytes;
	UInt32 bytesSent = 0;

	if (bytes == 0 || pBuffer == NULL)
		return FALSE;

/*-----------------------------------------------------------------------------------------
	Do Prometheus CSCL packet fragmentation if nessesary
-------------------------------------------------------------------------------------------*/
	do
	{
		bytes = ( bytesLeft > seCSCLPACKET_MAX_DATA_SIZE ) ? (seCSCLPACKET_MAX_DATA_SIZE) : (bytesLeft);
		bytesLeft -= bytes;
/*-----------------------------------------------------------------------------------------
		Send a request packet. Requesting Prometheus Sserver to write the <bytes> of data. 
-------------------------------------------------------------------------------------------*/
		
		bufferSize = sizeof(seCsclFpgaDataPkt) + bytes; 
		pPkt = (seCsclFpgaDataPkt *)malloc(bufferSize);

		if (!pPkt)
		{
			seASSERT( FALSE, ("CSCL: Unable to allocate transfer buffer"));
			return FALSE;
		}

		memset( (PVOID) pPkt, 0, bufferSize);

		pPkt->hdr.byte0 = seCSCL_JTAG_DATA_SHIFT; 
		pPkt->N0		= (UInt8) (bytes & 0xFF);
		pPkt->N1		= (UInt8) ((bytes >> 8) & 0xFF);
		pPkt->N2		= (UInt8) ((bytes >> 16) & 0xFF);
		pPkt->N3		= (UInt8) ((bytes >> 24) & 0xFF);

		pData = (UInt8 *) &(pPkt->N1);

		pData ++;
		memcpy( pData, pBuffer, bytes);

		bytesSent = seUsbTransportPktOut(seCSCL_WRITE_MGE, (UInt8 *) pPkt, bufferSize, FALSE);
		pBuffer += bytes;

		free((PVOID) pPkt); 
	} while ( bytesLeft > 0 );

	return (csclWaitForDone(FALSE));
}

/*-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
	seCsclJtagEnable()
------------------------------------------------------------------------------*/
Boolean seCsclJtagEnable(void)
{
	seUsbTransportPktOut(seCSCL_JTAG_ENABLE, NULL, 0, FALSE);

	return csclWaitForDone(TRUE);
}

/*-----------------------------------------------------------------------------
	seCsclJtagDisable()
------------------------------------------------------------------------------*/
Boolean seCsclJtagDisable(void)
{
	seUsbTransportPktOut(seCSCL_JTAG_DISABLE, NULL, 0, FALSE);

	return csclWaitForDone(TRUE);
}


/*-----------------------------------------------------------------------------
	seCsclJtagSync()
------------------------------------------------------------------------------*/
void seCsclJtagSync( UInt32 Flag )
{
	seCsclJtagSyncPkt Pkt= {0,};

	Pkt.hdr.byte0 = seCSCL_JTAG_SYNC;
	Pkt.hdr.byte1 = (Flag == seCSCL_TEST_LOGIG_RESET) ? 0 : 0x80; // seCSCL_TEST_LOGIG_RESET == 0

	seUsbTransportPktOut(seCSCL_JTAG_SYNC, (PVOID) &Pkt, sizeof(seCsclJtagSyncPkt), FALSE);
}

/*-----------------------------------------------------------------------------
	seCsclJtagPreamble()
------------------------------------------------------------------------------*/
void seCsclJtagPreamble( UInt32 Flag )
{
	seCsclJtagPreamblePkt Pkt= {0,};

	Pkt.hdr.byte0 = seCSCL_JTAG_PREAMBLE;
	Pkt.hdr.byte1 = (Flag == seCSCL_JTAG_SEQUENCE_0) ? 0 : 0x80;	
	seUsbTransportPktOut(seCSCL_JTAG_PREAMBLE, (PVOID) &Pkt, sizeof(seCsclJtagPreamblePkt), FALSE);
}

/*-----------------------------------------------------------------------------
	seCsclJtagPostamble()
------------------------------------------------------------------------------*/
void seCsclJtagPostamble( UInt32 Flag )
{
	seCsclJtagPostamblePkt Pkt= {0,};

	Pkt.hdr.byte0 = seCSCL_JTAG_POSTAMBLE;
	Pkt.hdr.byte1 = (Flag == seCSCL_JTAG_SEQUENCE_0) ? 0 : 0x80;

	seUsbTransportPktOut(seCSCL_JTAG_POSTAMBLE, (PVOID) &Pkt, sizeof(seCsclJtagPostamblePkt), FALSE);
}

/*-----------------------------------------------------------------------------
	seCsclHdrTrlr()
------------------------------------------------------------------------------*/
void seCsclJtagHdrTrlr( UInt32 Flag, UInt32 RLength )
{
	seCsclJtagHdrTrlrPkt Pkt= {0,};

	Pkt.hdr.byte0 = seCSCL_JTAG_HDR_TRLR;
	Pkt.hdr.byte1 = (Flag == seCSCL_JTAG_HDR) ? 0 : 0x80; // seCSCL_JTAG_HDR == 0

	Pkt.N0 = (UInt8)(RLength & 0xff);
	Pkt.N1 = (UInt8)((RLength >> 8) & 0xff);

	seUsbTransportPktOut(seCSCL_JTAG_HDR_TRLR, (PVOID) &Pkt, sizeof(seCsclJtagHdrTrlrPkt), FALSE);
}
/*-----------------------------------------------------------------------------
	seCsclJtagDataShift()
------------------------------------------------------------------------------*/
Boolean seCsclJtagDataShift( UInt32 slength, UInt32 flag, UInt8 * pOutBuffer, UInt8 * pInBuffer)
{
	seCsclJtagDataShiftPkt * pPkt; 
	int bufferSize = 0;
	UInt8 * pData = NULL;
	UInt32 slengthLeft = slength;
	UInt32 slengthSent = 0;
	int lTemp;
	UInt8 * pTempOutBuffer = NULL;
	UInt8 * pTempOutSave = NULL;
	UInt8 * pTempInBuffer = NULL;
	UInt8 * pTempInSave = NULL;
	UInt32 slengthSave = slength;

	if (slength == 0 || pInBuffer == 0)
		return FALSE;

	if ( pOutBuffer != NULL)
	{
		pTempOutBuffer = malloc( slength);

		if ( !pTempOutBuffer )
			return FALSE;

		pTempOutSave = pTempOutBuffer;

		for (lTemp = slength-1; lTemp >= 0; --lTemp)
		{
			pTempOutBuffer[slength-1 -lTemp] = pOutBuffer[lTemp];
		}
	}

	pTempInBuffer = malloc( slength +1);

	if ( !pTempInBuffer )
		return FALSE;

	pTempInSave = pTempInBuffer;

/*-----------------------------------------------------------------------------------------
	Do Prometheus CSCL packet fragmentation if nessesary
-------------------------------------------------------------------------------------------*/
	do
	{
		slength = ( slengthLeft > seJtagDesc.MaxPacketDataSize ) ? (seJtagDesc.MaxPacketDataSize) : (slengthLeft);
		slengthLeft -= slength;
/*-----------------------------------------------------------------------------------------
		Send a request packet. Requesting Prometheus Sserver to write the <bytes> of data.
-------------------------------------------------------------------------------------------*/

		bufferSize = sizeof(seCsclJtagDataShiftPkt) + slength; 
		pPkt = (seCsclJtagDataShiftPkt *)malloc(bufferSize);

		if (!pPkt)
		{
			seASSERT( FALSE, ("CSCL: Unable to allocate transfer buffer"));
			if ( pTempOutSave )
				free( pTempOutSave );
			free( pTempInSave);
			return FALSE;
		}

		memset( (PVOID) pPkt, 0, bufferSize);

		pPkt->hdr.byte0 = seCSCL_JTAG_DATA_SHIFT; 
		pPkt->hdr.byte1 = (flag && slengthLeft == 0) ?  0x80 : 0;
		pPkt->N0		= (UInt8) (slength & 0xFF);
		pPkt->N1		= (UInt8) ((slength >> 8) & 0xFF);
		pData = (UInt8 *) &(pPkt->N1);

		pData ++;

		if ( pOutBuffer != NULL)
			memcpy( pData, pTempOutBuffer, slength);
		else
			memset( pData, ('0' + 1 ),slength);

		slengthSent = seUsbTransportPktOut(seCSCL_WRITE_MGE, (UInt8 *) pPkt, bufferSize, FALSE);

		free((PVOID) pPkt); 

		pTempInBuffer += seCsclJtagTdoData( slength, pTempInBuffer);
		//pTempInBuffer[0] = 0;

		if ( pTempOutBuffer != NULL)
			pTempOutBuffer +=slength;

	} while ( slengthLeft > 0 );

	pTempInBuffer[0] = 0;

	if ( slengthSave >= 0x1000)
		pTempInBuffer[0] = 0;

	for (lTemp = slengthSave-1; lTemp >= 0; --lTemp)
	{
		pInBuffer[slengthSave-1 -lTemp] = pTempInSave[lTemp];
	}


	if ( pTempOutSave )
		free((PVOID) pTempOutSave);

	free((PVOID) pTempInSave);

	return (csclWaitForDone(FALSE));
}

/*-----------------------------------------------------------------------------
	seCsclJtagTdoData()
------------------------------------------------------------------------------*/
UInt32 seCsclJtagTdoData( UInt32 rlength, UInt8 * pData)
{
	//seCsclJtagTdoPkt * pPkt;
	seCsclJtagTdoDataPkt * pDataPkt;
	UInt32 packetLength = 0, totalLength = 0;
	int bufferSize = 0;
	seCSCL_PACKET_TYPE pktType = seCSCL_WRONG;
	UInt32 rlengthLeft = rlength;
	UInt32 rlengthReceived = 0;
	UInt32 rlength2Expect = 0;

	if (!pData) 
		return 0;

/*-----------------------------------------------------------------------------------------
	Do Prometheus CSCL packet fragmentation if nessesary
-------------------------------------------------------------------------------------------*/
	do
	{
		rlength = ( rlengthLeft > seCSCLPACKET_MAX_DATA_SIZE ) ? (seCSCLPACKET_MAX_DATA_SIZE) : (rlengthLeft);
		rlengthLeft -= rlength;
/*-----------------------------------------------------------------------------------------
		Prepare to receive requested data
-------------------------------------------------------------------------------------------*/
		rlengthReceived = 0;

		rlength = (rlength & 1) ? (rlength + 1) : rlength; // round up

		bufferSize = sizeof( seCsclJtagTdoDataPkt ) + rlength;

		pDataPkt = (seCsclJtagTdoDataPkt *)malloc( bufferSize);

		if (!pDataPkt)
		{
			seASSERT( FALSE, ("Unable to allocate a transfer buffer"));
			return 0;
		}

		do {
			UInt8 *pBuf = NULL;
			pktType = seCSCL_WRONG;
			memset( (PVOID)pDataPkt, 0, bufferSize);

			// Keep reading untill a desired number (Prometheus protocol) of data received.
			packetLength = seUsbTransportPktIn( (UInt8 *)pDataPkt, bufferSize, FALSE);

			if (packetLength == 0)
				break;

			pBuf = (UInt8 *)pDataPkt;

			if ( ((pktType = csclParsePktHdr( (UInt8 *) pDataPkt)) == seCSCL_JTAG_TDO_DATA) && rlengthReceived ==0)
			{
				// First Packet
				pBuf += 2;
				rlength2Expect = pBuf[0] + (pBuf[1]<<8);
				pBuf += 2;
				packetLength -= 4; // adjust for the CSCL header
				memcpy( pData, pBuf, packetLength);
				pData +=packetLength;
			}
			else if ( rlengthReceived !=0 )
			{
				// Following packets
				memcpy( pData, pBuf, packetLength);
				pData +=packetLength;
			}
			else
			{
				seASSERT( FALSE, ("Bogus packet received 0x%2x", pktType));
			}
			rlengthReceived += packetLength;

		} while (rlengthReceived < rlength);

		free((PVOID)pDataPkt);
		totalLength +=rlengthReceived;
	} while ( rlengthLeft > 0 );

	return ( ( rlength2Expect < totalLength ) ? rlength2Expect : totalLength );
}


#ifdef _DEBUG
int seCSCLLog(const char *f, ...)		// variable arguments
{
	FILE * pF = fopen( "cscl.log", "a" );
	if (pF)
	{
		fprintf( pF, f );
		fclose( pF );
		return 1;
	}
	else
		return 0;
}
#endif
