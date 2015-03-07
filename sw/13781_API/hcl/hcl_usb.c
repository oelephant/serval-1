//=============================================================================
//	HCL_USB.C - One file to handle DIRECT,INDIRECT,SPI,8/16-BIT INTERFACES
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//=============================================================================

#include "stdlib.h"
#include "malloc.h"

#include "hcl.h"
#include "hcl_common.h"
#include "cscl.h"
#include "..\usbbridge\usbbridge.h"



//===========================================================================
// PRIVATE GLOBAL VARIABLES
//===========================================================================

static UInt32 gBaseAddress;				// HCL acquired base address (virtual)


//===========================================================================
// PUBLIC GLOBAL VARIABLES
//===========================================================================

seCsclConfigRegs segCsclConfigRegsInfo;


//===========================================================================
// PRIVATE (STATIC) FUNCTION PROTOTYPES
//===========================================================================

// MapToRegSpace() - Add 0x60800 to the index in order to access register space.
#if 1 // enable hack for now; ideally the chip should automatically map addresses 00h-FFh to reg space.
#define MapToRegSpace(Index) {if (Index<0xFC) Index+=0x60800; }
#else
#define MapToRegSpace(Index) ;
#endif
//static void MapToRegSpace( UInt32* Index );



//---------------------------------------------------------------------------
// PUBLIC FUNCTION: seAcquireController()
//---------------------------------------------------------------------------
Boolean seAcquireController( void )
{
	// Use the configured base address to retrieve the virtual address, if any.  On
	// non-Windows platforms, the base address will be copied to the virtual address.

	// Remove the check for re-entrancy since similar check was done in TestAPI seAcquireController
	//if ( gBaseAddress == 0 ) 	// Only do this once, so we are re-entrant.
	{
		segCsclConfigRegsInfo.ConfigRegA = segHclInfo.USBConfig[0];
		segCsclConfigRegsInfo.ConfigRegB = segHclInfo.USBConfig[1];
		segCsclConfigRegsInfo.ConfigRegC = segHclInfo.USBConfig[2];
		segCsclConfigRegsInfo.ConfigRegD = segHclInfo.USBConfig[3];
		segCsclConfigRegsInfo.ConfigRegE = segHclInfo.USBConfig[4];
		segCsclConfigRegsInfo.ConfigRegF = segHclInfo.USBConfig[5];

		seOsSetProcessExclusivity( TRUE );

		if (!seCsclSetup(0, seCSCL_MGE_REGISTERS | seCSCL_MGE_ADDR_INC, &segCsclConfigRegsInfo))							// Client Server Communication Layer
		{
			gBaseAddress = 0;
			seOsSetProcessExclusivity( FALSE );
			return FALSE;
		}

		seOsSetProcessExclusivity( FALSE );
		gBaseAddress = 0xFF;						// Global address does not have meaning other then a "re-entrancy" flag
	}

	return TRUE;
}



//---------------------------------------------------------------------------
// PUBLIC FUNCTION: seInitializeController()
//---------------------------------------------------------------------------
Boolean seInitializeController( UInt32 Flags )
{
	Flags;

	// Fail if seAcquireController() was not called, or if it failed.

	if ( gBaseAddress == 0 )
	{
		seASSERT( FALSE, ("seInitializeController() called but controller not acquired.") );
		return FALSE;
	}

	// Initialize the registers.

	seHclInitRegisters();

	// Program the parallel LCD panels.

	//if ( segHclInfo.LCD1Regs[0] != seHCL_LCD_ENDOFTABLE || segHclInfo.LCD2Regs[0] != seHCL_LCD_ENDOFTABLE )
	//	ProgramPanelInterfaceCommands();

	return TRUE;
}




//===========================================================================
// COMMON FUNCTIONS
//===========================================================================



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seReadReg8()
// COMMON FUNCTION: seReadReg16()
// COMMON FUNCTION: seReadReg32()
//-----------------------------------------------------------------------------
UInt32 seReadReg8( UInt32 Index )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

UInt32 seReadReg16( UInt32 Index )
{
	UInt32 Value;

	seASSERT( (Index&1)==0, ("seReadReg16() Index must be even.") );
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seWriteReg8()
// COMMON FUNCTION: seWriteReg16()
// COMMON FUNCTION: seWriteReg32()
//-----------------------------------------------------------------------------
void seWriteReg8( UInt32 Index, UInt32 Value )
{
	MapToRegSpace( Index );
	seOsSetProcessExclusivity( TRUE );

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seWriteReg8() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );
}

void seWriteReg16( UInt32 Index, UInt32 Value )
{
	seASSERT( (Index&1)==0, ("seWriteReg16() Index must be even.") );
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seWriteReg16() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seModifyReg8()
// COMMON FUNCTION: seModifyReg16()
//-----------------------------------------------------------------------------
UInt32 seModifyReg8( UInt32 Index, UInt32 ClearBits, UInt32 SetBits )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags );

	Value = (Value & ~ClearBits) | SetBits;

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seModifyReg8() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

UInt32 seModifyReg16( UInt32 Index, UInt32 ClearBits, UInt32 SetBits )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags );

	Value = (Value & ~ClearBits) | SetBits;

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seModifyReg16() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: seSetBitsReg8()
// COMMON FUNCTION: seSetBitsReg16()
//-----------------------------------------------------------------------------
UInt32 seSetBitsReg8( UInt32 Index, UInt32 SetBits )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags );

	Value |= SetBits;

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seSetBitsReg8() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

UInt32 seSetBitsReg16( UInt32 Index, UInt32 SetBits )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags );

	Value |= SetBits;

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seSetBitsReg16() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: seClearBitsReg8()
// COMMON FUNCTION: seClearBitsReg16()
//-----------------------------------------------------------------------------
UInt32 seClearBitsReg8( UInt32 Index, UInt32 ClearBits )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags );

	Value &= ~ClearBits;

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seClearBitsReg8() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

UInt32 seClearBitsReg16( UInt32 Index, UInt32 ClearBits )
{
	UInt32 Value;
	MapToRegSpace( Index );

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags );

	Value &= ~ClearBits;

	if ( !seCsclWrMge(seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags, Value) )
		seASSERT( FALSE, ("seClearBitsReg16() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}


//-----------------------------------------------------------------------------
void seBurstReadReg8( UInt32 Index, UInt8* pData, UInt32 Count )
{
//	UInt32 InterfaceFlags = seCSCL_MGE_REGISTERS;
	seASSERT( (Index&1)==0, ("seBurstReadReg8() Index must be even.") );
	MapToRegSpace( Index );

	if ( Count && pData )
	{
		UInt32 len;
		seOsSetProcessExclusivity( TRUE );

//		len = seCsclBurstRdMge( seMgeDesc.MgeRegAddr+Index, Count, InterfaceFlags, pData );
		len = seCsclBurstRdMge( seMgeDesc.MgeRegAddr+Index, Count, seMgeDesc.InterfaceFlags, pData );
		seASSERT( len==Count, ("seBurstReadReg8() CSCL Error: Bytes number != Count.") );

		seOsSetProcessExclusivity( FALSE );
	}
}

void seBurstWriteReg8( UInt32 Index, const UInt8* pData, UInt32 Count )
{
//	UInt32 InterfaceFlags = seCSCL_MGE_REGISTERS;
	seASSERT( (Index&1)==0, ("seBurstWriteReg8() Index must be even.") );
	MapToRegSpace( Index );

	if ( Count && pData )
	{
		seOsSetProcessExclusivity( TRUE );

//		if ( !seCsclBurstWrMge(seMgeDesc.MgeRegAddr+Index, Count, InterfaceFlags, (UInt8*)pData) )
		if ( !seCsclBurstWrMge(seMgeDesc.MgeRegAddr+Index, Count, seMgeDesc.InterfaceFlags, (UInt8*)pData) )
			seASSERT( FALSE, ("seBurstWriteReg8() CSCL Error") );

		seOsSetProcessExclusivity( FALSE );
	}
}

void seBurstReadReg16( UInt32 Index, UInt16* pData, UInt32 Count )
{
	seASSERT( (Index&1)==0, ("seBurstReadReg16() Index must be even.") );
	MapToRegSpace( Index );

	seBurstReadReg8( Index, (UInt8*)pData, Count*2 );
}

void seBurstWriteReg16( UInt32 Index, const UInt16* pData, UInt32 Count )
{
	seASSERT( (Index&1)==0, ("seBurstWriteReg16() Index must be even.") );
	MapToRegSpace( Index );

	seBurstWriteReg8( Index, (UInt8*)pData, Count*2 );
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: sePollWhileHighReg8()
// COMMON FUNCTION: sePollWhileHighReg16()
//
// If IntervalMS is non-zero, then this is a long event poll, and this code
// will only go process-exclusive during the register read.
// If IntervalMS is zero, then this is a short event poll, and this code will
// go process-exclusive for the entire duration of the poll (TimeoutMS).
// In other words, if IntervalMS is zero, we optimize this for performance.
//-----------------------------------------------------------------------------
Boolean sePollWhileHighReg8( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS )
{
	Boolean fResult = FALSE;
	UInt32 Timer;
	MapToRegSpace( Index );

	if ( IntervalMS == 0 )
	{
		seOsSetProcessExclusivity( TRUE );

		Timer = seOsGetTimeMS();
		do
		{
			if ( !(seCsclRdMge(seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags) & Bits) )
			{
				fResult = TRUE;
				break;
			}
		} while ( (seOsGetTimeMS()-Timer) < TimeoutMS );

		seOsSetProcessExclusivity( FALSE );
	}
	else
	{
		Timer = 0;
		do
		{
			if ( !(seReadReg8(Index) & Bits) )
			{
				fResult = TRUE;
				break;
			}
			seOsSleepMS( IntervalMS );
			Timer += IntervalMS;
		} while ( Timer < TimeoutMS );
	}

	return fResult;
}

Boolean sePollWhileHighReg16( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS )
{
	Boolean fResult = FALSE;
	UInt32 Timer;
	MapToRegSpace( Index );

	if ( IntervalMS == 0 )
	{
		seOsSetProcessExclusivity( TRUE );

		Timer = seOsGetTimeMS();
		do
		{
			if ( !(seCsclRdMge(seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags) & Bits) )
			{
				fResult = TRUE;
				break;
			}
		} while ( (seOsGetTimeMS()-Timer) < TimeoutMS );

		seOsSetProcessExclusivity( FALSE );
	}
	else
	{
		Timer = 0;
		do
		{
			if ( !(seReadReg16(Index) & Bits) )
			{
				fResult = TRUE;
				break;
			}
			seOsSleepMS( IntervalMS );
			Timer += IntervalMS;
		} while ( Timer < TimeoutMS );
	}

	return fResult;
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: sePollWhileLowReg8()
// COMMON FUNCTION: sePollWhileLowReg16()
//
// If IntervalMS is non-zero, then this is a long event poll, and this code
// will only go process-exclusive during the register read.
// If IntervalMS is zero, then this is a short event poll, and this code will
// go process-exclusive for the entire duration of the poll (TimeoutMS).
// In other words, if IntervalMS is zero, we optimize this for performance.
//-----------------------------------------------------------------------------
Boolean sePollWhileLowReg8( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS )
{
	Boolean fResult = FALSE;
	UInt32 Timer;
	MapToRegSpace( Index );

	if ( IntervalMS == 0 )
	{
		seOsSetProcessExclusivity( TRUE );

		Timer = seOsGetTimeMS();
		do
		{
			if ( seCsclRdMge(seMgeDesc.MgeRegAddr+Index, 1, seMgeDesc.InterfaceFlags) & Bits )
			{
				fResult = TRUE;
				break;
			}
		} while ( (seOsGetTimeMS()-Timer) < TimeoutMS );

		seOsSetProcessExclusivity( FALSE );
	}
	else
	{
		Timer = 0;
		do
		{
			if ( seReadReg8(Index) & Bits )
			{
				fResult = TRUE;
				break;
			}
			seOsSleepMS( IntervalMS );
			Timer += IntervalMS;
		} while ( Timer < TimeoutMS );
	}

	return fResult;
}

Boolean sePollWhileLowReg16( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS )
{
	Boolean fResult = FALSE;
	UInt32 Timer;
	MapToRegSpace( Index );

	if ( IntervalMS == 0 )
	{
		seOsSetProcessExclusivity( TRUE );

		Timer = seOsGetTimeMS();
		do
		{
			if ( seCsclRdMge(seMgeDesc.MgeRegAddr+Index, 2, seMgeDesc.InterfaceFlags) & Bits )
			{
				fResult = TRUE;
				break;
			}
		} while ( (seOsGetTimeMS()-Timer) < TimeoutMS );

		seOsSetProcessExclusivity( FALSE );
	}
	else
	{
		Timer = 0;
		do
		{
			if ( seReadReg16(Index) & Bits )
			{
				fResult = TRUE;
				break;
			}
			seOsSleepMS( IntervalMS );
			Timer += IntervalMS;
		} while ( Timer < TimeoutMS );
	}

	return fResult;
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: seReadMemory8()
// COMMON FUNCTION: seReadMemory16()
// COMMON FUNCTION: seReadMemory32()
//-----------------------------------------------------------------------------
UInt8 seReadMemory8( UInt32 Offset )
{
	UInt32 Value;

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeMemAddr+Offset, 1, seCSCL_MGE_MEMORY );

	seOsSetProcessExclusivity( FALSE );

	return (UInt8)Value;
}

UInt16 seReadMemory16( UInt32 Offset )
{
	UInt32 Value;

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeMemAddr+Offset, 2, seCSCL_MGE_MEMORY );

	seOsSetProcessExclusivity( FALSE );

	return (UInt16)Value;
}

UInt32 seReadMemory32( UInt32 Offset )
{
	UInt32 Value;

	seBurstReadMemory32( Offset, &Value, 1);

	return Value;
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seWriteMemory8()
// COMMON FUNCTION: seWriteMemory16()
// COMMON FUNCTION: seWriteMemory32()
//-----------------------------------------------------------------------------
void seWriteMemory8( UInt32 Offset, UInt32 Value )
{
	seOsSetProcessExclusivity( TRUE );

	if ( !seCsclWrMge(seMgeDesc.MgeMemAddr+Offset, 1, seCSCL_MGE_MEMORY, Value) )
		seASSERT( FALSE, ("seWriteMemory8() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );
}

void seWriteMemory16( UInt32 Offset, UInt32 Value )
{
	seOsSetProcessExclusivity( TRUE );

	if ( !seCsclWrMge(seMgeDesc.MgeMemAddr+Offset, 2, seCSCL_MGE_MEMORY, Value) )
		seASSERT( FALSE, ("seWriteMemory16() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );
}

void seWriteMemory32( UInt32 Offset, UInt32 Value )
{
	seBurstWriteMemory32( Offset, &Value, 1 );
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seFillMemory8()
// COMMON FUNCTION: seFillMemory16()
// COMMON FUNCTION: seFillMemory32()
//-----------------------------------------------------------------------------
void seFillMemory8( UInt32 Offset, UInt32 Value, UInt32 Count )
{
	UInt8 *pData;
	UInt32 svCount = Count;
	int i = 0;

	if (Count)
	{
		if ((pData=(UInt8*)malloc(Count)) == NULL) return;
		do{
			pData[i] = (UInt8)Value;
			i++;
		} while ( --Count ); // Number of 8 bit words

		seBurstWriteMemory8( Offset, pData, svCount );
		free( pData );
	}
	return;
}

void seFillMemory16( UInt32 Offset, UInt32 Value, UInt32 Count )
{
	UInt16 *pData;
	UInt32 svCount = Count;
	int i = 0;

	if (Count)
	{
		if ((pData=(UInt16*)malloc(Count*2)) == NULL) return;
		do{
			pData[i] = (UInt16)Value;
			i++;
		} while ( --Count ); // Number of 16 bit words

		seBurstWriteMemory16( Offset, pData, svCount );
		free( pData );
	}
	return;
}

void seFillMemory32( UInt32 Offset, UInt32 Value, UInt32 Count )
{
	UInt32 *pData;
	UInt32 svCount = Count;
	int i = 0;

	if (Count)
	{
		if ((pData=(UInt32*)malloc(Count*4)) == NULL) return;
		do{
			pData[i] = Value;
			i++;
		} while ( --Count );// Number of 32 bit words
	
		seBurstWriteMemory32( Offset, pData, svCount );
		free( pData );
	}
	return;
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: seBurstReadMemory8()
// COMMON FUNCTION: seBurstReadMemory16()
// COMMON FUNCTION: seBurstReadMemory32()
//-----------------------------------------------------------------------------
void seBurstReadMemory8( UInt32 Offset, UInt8* pData, UInt32 Count )
{
	UInt32 InterfaceFlags = seCSCL_MGE_MEMORY | seCSCL_MGE_ADDR_INC;
	if ( Count && pData )
	{
		UInt32 len;
		seOsSetProcessExclusivity( TRUE );
		len = seCsclBurstRdMge( seMgeDesc.MgeMemAddr+Offset, Count, InterfaceFlags, pData );
		//seASSERT( len==Count, ("seBurstReadMemory8() CSCL Error: Bytes number != Count.") );
		seOsSetProcessExclusivity( FALSE );
	}
}

void seBurstReadMemory16( UInt32 Offset, UInt16* pData, UInt32 Count )
{
	UInt32 InterfaceFlags = seCSCL_MGE_MEMORY | seCSCL_MGE_ADDR_INC;
	if ( Count && pData )
	{
		UInt32 len;
		seOsSetProcessExclusivity( TRUE );
		len = seCsclBurstRdMge( seMgeDesc.MgeMemAddr+Offset, Count*2, InterfaceFlags, (UInt8*)pData );
		//seASSERT( len==Count*2, ("seBurstReadMemory16() CSCL Error: Bytes number != Count.") );
		seOsSetProcessExclusivity( FALSE );
	}
}

void seBurstReadMemory32( UInt32 Offset, UInt32* pData, UInt32 Count )
{
	UInt32 InterfaceFlags = seCSCL_MGE_MEMORY | seCSCL_MGE_ADDR_INC;
	if ( Count && pData )
	{
		UInt32 len;
		seOsSetProcessExclusivity( TRUE );
		len = seCsclBurstRdMge( seMgeDesc.MgeMemAddr+Offset, Count*4, InterfaceFlags, (UInt8*)pData );
		//seASSERT( len==Count*4, ("seBurstReadMemory32() CSCL Error: Bytes number != Count.") );
		seOsSetProcessExclusivity( FALSE );
	}
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: seBurstWriteMemory8()
// COMMON FUNCTION: seBurstWriteMemory16()
// COMMON FUNCTION: seBurstWriteMemory32()
//-----------------------------------------------------------------------------
void seBurstWriteMemory8( UInt32 Offset, const UInt8* pData, UInt32 Count )
{
	UInt32 InterfaceFlags = seCSCL_MGE_MEMORY | seCSCL_MGE_ADDR_INC;
	if ( Count && pData )
	{
		seOsSetProcessExclusivity( TRUE );
		seCsclBurstWrMge( seMgeDesc.MgeMemAddr+Offset, Count, InterfaceFlags, (UInt8*)pData );
		seOsSetProcessExclusivity( FALSE );
	}
}

void seBurstWriteMemory16( UInt32 Offset, const UInt16* pData, UInt32 Count )
{
	UInt32 InterfaceFlags = seCSCL_MGE_MEMORY | seCSCL_MGE_ADDR_INC;
	if ( Count && pData )
	{
		seOsSetProcessExclusivity( TRUE );
		seCsclBurstWrMge( seMgeDesc.MgeMemAddr+Offset, Count*2, InterfaceFlags, (UInt8*)pData );
		seOsSetProcessExclusivity( FALSE );
	}
}

void seBurstWriteMemory32( UInt32 Offset, const UInt32* pData, UInt32 Count )
{
	UInt32 InterfaceFlags = seCSCL_MGE_MEMORY | seCSCL_MGE_ADDR_INC;
	if ( Count && pData )
	{
		seOsSetProcessExclusivity( TRUE );
		seCsclBurstWrMge( seMgeDesc.MgeMemAddr+Offset, Count*4, InterfaceFlags, (UInt8*)pData );
		seOsSetProcessExclusivity( FALSE );
	}
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seSetISRCallback()
//-----------------------------------------------------------------------------
int seSetISRCallback( void (*pISRCallback)(void) )
{
	// If the USB driver has not been acquired, exit with an error.
	if ( gBaseAddress == 0 )
		return FALSE;

	return usbHookInterrupt( pISRCallback );
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seReadRaw()
//-----------------------------------------------------------------------------
UInt32 seReadRaw( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize )
{
	UInt32 Value;

	seASSERT( BaseIndex==0, ("seReadRaw() BaseIndex must be zero for USB interface.") );
	if ( BaseIndex != 0 )
		return 0;

	seOsSetProcessExclusivity( TRUE );

	Value = seCsclRdMge( seMgeDesc.MgeMemAddr+Offset, ByteSize, seCSCL_MGE_MEMORY );

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seWriteRaw()
//-----------------------------------------------------------------------------
void seWriteRaw( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize, UInt32 Value )
{
	seASSERT( BaseIndex==0, ("seWriteRaw() BaseIndex must be zero for USB interface.") );
	if ( BaseIndex != 0 )
		return;

	seOsSetProcessExclusivity( TRUE );

	if ( !seCsclWrMge(seMgeDesc.MgeMemAddr+Offset, ByteSize, seCSCL_MGE_MEMORY, Value) )
		seASSERT(FALSE, ("seWriteRaw() CSCL Error") );

	seOsSetProcessExclusivity( FALSE );
}
