//=============================================================================
//	HCL_PCI_INDIRECT8.C - PARALLEL INDIRECT 8-BIT INTERFACE
//	(C)SEIKO EPSON CORPORATION 2008. All rights reserved.
//=============================================================================

#include "hcl.h"
#include "hcl_common.h"



//===========================================================================
// PRIVATE GLOBAL VARIABLES
//===========================================================================

static UInt32	gBaseAddress;				// HCL acquired base address (virtual)
static UInt32	gBlockSize;					// HCL acquired memory block size (bytes)

static volatile UInt8	*gpIndirectIndex;	// HCL indirect index register
static volatile UInt8	*gpIndirectData;	// HCL indirect data register


//===========================================================================
// PRIVATE (STATIC) FUNCTION PROTOTYPES
//===========================================================================

static void		SetMemoryReadAddress( UInt32 Offset );



//---------------------------------------------------------------------------
// PUBLIC FUNCTION: seAcquireController()
//---------------------------------------------------------------------------
Boolean seAcquireController( void )
{
	// Use the configured base address to retrieve the virtual address, if any.  On
	// non-Windows platforms, the base address will be copied to the virtual address.

	if ( gBaseAddress == 0 ) 	// Only do this once, so we are re-entrant.
	{
		if ( !seOsMapPhysicalToVirtual( segHclInfo.BaseAddress, &gBaseAddress, &gBlockSize ) )
		{
			gBaseAddress = 0;
			return FALSE;
		}

		gpIndirectIndex		= (UInt8*)(gBaseAddress + 0);		// Parallel indirect 8 index OFFSET 0
		gpIndirectData		= (UInt8*)(gBaseAddress + 4);		// Parallel indirect 8 data OFFSET 4
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

	return TRUE;
}


//-----------------------------------------------------------------------------
// COMMON FUNCTION: seReadReg8()
// COMMON FUNCTION: seReadReg16()
//-----------------------------------------------------------------------------
UInt32 seReadReg8( UInt32 Index )
{
	UInt32	Value;

	seASSERT( (Index&1)==0, ("seReadReg8() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;
	Value = *gpIndirectData;

	seOsSetProcessExclusivity( FALSE );

	return Value;
}

UInt32 seReadReg16( UInt32 Index )
{
	UInt32	Value;

	seASSERT( (Index&1)==0, ("seReadReg16() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;
	// Make use of register index auto-increment feature
	Value = *gpIndirectData;
	Value += (*gpIndirectData << 8);

	seOsSetProcessExclusivity( FALSE );

	return Value;
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seWriteReg8()
// COMMON FUNCTION: seWriteReg16()
//-----------------------------------------------------------------------------
void seWriteReg8( UInt32 Index, UInt32 Value )
{
	seASSERT( (Index&1)==0, ("seWriteReg8() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;
	*gpIndirectData = (UInt8)Value;

	seOsSetProcessExclusivity( FALSE );
}

void seWriteReg16( UInt32 Index, UInt32 Value )
{
	seASSERT( (Index&1)==0, ("seWriteReg16() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;
	// Make use of register index auto-increment feature
	*gpIndirectData = (UInt8)Value;
	*gpIndirectData = (UInt8)(Value >> 8);

	seOsSetProcessExclusivity( FALSE );
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seModifyReg8()
//-----------------------------------------------------------------------------
UInt32 seModifyReg8( UInt32 Index, UInt32 ClearBits, UInt32 SetBits )
{
	UInt32	Value;

	seASSERT( (Index&1)==0, ("seModifyReg8() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;

	Value = ((*gpIndirectData & ~ClearBits) | SetBits);

	*gpIndirectIndex = (UInt8)Index;
	*gpIndirectData = (UInt8)Value;

	seOsSetProcessExclusivity( FALSE );

	return Value;
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seSetBitsReg8()
//-----------------------------------------------------------------------------
UInt32 seSetBitsReg8( UInt32 Index, UInt32 SetBits )
{
	UInt32	Value;

	seASSERT( (Index&1)==0, ("seSetBitsReg8() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;

	Value = (*gpIndirectData | SetBits);

	*gpIndirectIndex = (UInt8)Index;
	*gpIndirectData = (UInt8)Value;

	seOsSetProcessExclusivity( FALSE );

	return Value;
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seClearBitsReg8()
//-----------------------------------------------------------------------------
UInt32 seClearBitsReg8( UInt32 Index, UInt32 ClearBits )
{
	UInt32	Value;

	seASSERT( (Index&1)==0, ("seClearBitsReg8() Index must be even.") );

	seOsSetProcessExclusivity( TRUE );

	*gpIndirectIndex = (UInt8)Index;

	Value = (*gpIndirectData & ~ClearBits);

	*gpIndirectIndex = (UInt8)Index;
	*gpIndirectData = (UInt8)Value;

	seOsSetProcessExclusivity( FALSE );

	return Value;
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: sePollWhileHighReg8()
//
// If IntervalMS is non-zero, then this is a long event poll, and this code
// will only go process exclusive during the register read.
// If IntervalMS is zero, then this is a short event poll, and this code will
// go process exclusive for the entire duration of the poll (TimeoutMS).
// In other words, if IntervalMS is zero, we optimize this for performance.
//-----------------------------------------------------------------------------
Boolean sePollWhileHighReg8( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS )
{
	Boolean	fResult = FALSE;
	UInt32	Timer;

	if ( IntervalMS == 0 )
	{
		seOsSetProcessExclusivity( TRUE );

		Timer = seOsGetTimeMS();
		do
		{
			*gpIndirectIndex = (UInt8)Index;

			if ( !(*gpIndirectData & Bits) )
			{
				fResult = TRUE;
				break;
			}
		} while ( (seOsGetTimeMS() - Timer) < TimeoutMS );

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



//-----------------------------------------------------------------------------
// COMMON FUNCTION: sePollWhileLowReg8()
//
// If IntervalMS is non-zero, then this is a long event poll, and this code
// will only go process exclusive during the register read.
// If IntervalMS is zero, then this is a short event poll, and this code will
// go process exclusive for the entire duration of the poll (TimeoutMS).
// In other words, if IntervalMS is zero, we optimize this for performance.
//-----------------------------------------------------------------------------
Boolean sePollWhileLowReg8( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS )
{
	Boolean	fResult = FALSE;
	UInt32	Timer;

	if ( IntervalMS == 0 )
	{
		seOsSetProcessExclusivity( TRUE );

		Timer = seOsGetTimeMS();
		do
		{
			*gpIndirectIndex = (UInt8)Index;
			
			if ( (*gpIndirectData & Bits) )
			{
				fResult = TRUE;
				break;
			}
		} while ( (seOsGetTimeMS() - Timer) < TimeoutMS );

		seOsSetProcessExclusivity( FALSE );
	}
	else
	{
		Timer = 0;
		do
		{
			if ( (seReadReg8(Index) & Bits) )
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
	UInt32	Value;

	seOsSetProcessExclusivity( TRUE );

	SetMemoryReadAddress( Offset );
	Value = *gpIndirectData;

	seOsSetProcessExclusivity( FALSE );

	return (UInt8)Value;
}

UInt16 seReadMemory16( UInt32 Offset )
{
	UInt32	Value;

	seOsSetProcessExclusivity( TRUE );

	SetMemoryReadAddress( Offset );
	Value  = *gpIndirectData;
	Value += (*gpIndirectData << 8);

	seOsSetProcessExclusivity( FALSE );

	return (UInt16)Value;
}

UInt32 seReadMemory32( UInt32 Offset )
{
	UInt32	Value;

	seOsSetProcessExclusivity( TRUE );

	SetMemoryReadAddress( Offset );
	Value  = *gpIndirectData;
	Value += (*gpIndirectData << 8);
	Value += (*gpIndirectData << 16);
	Value += (*gpIndirectData << 24);

	seOsSetProcessExclusivity( FALSE );

	return Value;
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seWriteMemory8()
// COMMON FUNCTION: seWriteMemory16()
// COMMON FUNCTION: seWriteMemory32()
//-----------------------------------------------------------------------------
void seWriteMemory8( UInt32 Value )
{
	//seOsSetProcessExclusivity( TRUE );

	//*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//*gpIndirectData = (UInt8)Value;

	//seOsSetProcessExclusivity( FALSE );
}

void seWriteMemory16( UInt32 Value )
{
	//seOsSetProcessExclusivity( TRUE );

	//*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//*gpIndirectData = (UInt8)Value;
	//*gpIndirectData = (UInt8)(Value >> 8);

	//seOsSetProcessExclusivity( FALSE );
}

void seWriteMemory32( UInt32 Value )
{
	//seOsSetProcessExclusivity( TRUE );

	//*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//*gpIndirectData = (UInt8)Value;
	//*gpIndirectData = (UInt8)(Value >> 8);
	//*gpIndirectData = (UInt8)(Value >> 16);
	//*gpIndirectData = (UInt8)(Value >> 24);

	//seOsSetProcessExclusivity( FALSE );
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seFillMemory8()
// COMMON FUNCTION: seFillMemory16()
// COMMON FUNCTION: seFillMemory32()
//-----------------------------------------------------------------------------
void seFillMemory8( UInt32 Value, UInt32 Count )
{
	//if ( Count )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//	do
	//		*gpIndirectData = (UInt8)Value;
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

void seFillMemory16( UInt32 Value, UInt32 Count )
{
	//if ( Count )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//	do
	//	{
	//		*gpIndirectData = (UInt8)(Value >> 8);
	//		*gpIndirectData = (UInt8)Value;
	//	}
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

void seFillMemory32( UInt32 Value, UInt32 Count )
{
	//if ( Count )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//	do
	//	{
	//		*gpIndirectData = (UInt8)(Value >> 8);
	//		*gpIndirectData = (UInt8)Value;
	//		*gpIndirectData = (UInt8)(Value >> 24);
	//		*gpIndirectData = (UInt8)(Value >> 16);
	//	} while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seBurstReadMemory8()
// COMMON FUNCTION: seBurstReadMemory16()
// COMMON FUNCTION: seBurstReadMemory32()
//-----------------------------------------------------------------------------
void seBurstReadMemory8( UInt32 Offset, UInt8* pData, UInt32 Count )
{
	//if ( Count && pData )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	SetMemoryReadAddress( Offset );
	//	do
	//		*pData++ = *gpIndirectData;
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

void seBurstReadMemory16( UInt32 Offset, UInt16* pData, UInt32 Count )
{
	//UInt32 Value;

	//if ( Count && pData )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	SetMemoryReadAddress( Offset );
	//	do
	//	{
	//		Value = *gpIndirectData;
	//		*pData++ = (UInt16)(Value + (*gpIndirectData << 8));
	//	}
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

void seBurstReadMemory32( UInt32 Offset, UInt32* pData, UInt32 Count )
{
	//UInt32	Value;

	//if ( Count && pData )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	SetMemoryReadAddress( Offset );
	//	do
	//	{
	//		Value  = *gpIndirectData;
	//		Value += (*gpIndirectData << 8);
	//		Value += (*gpIndirectData << 16);
	//		Value += (*gpIndirectData << 24);
	//		*pData++ = Value;
	//	}
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}



//-----------------------------------------------------------------------------
// COMMON FUNCTION: seBurstWriteMemory8()
// COMMON FUNCTION: seBurstWriteMemory16()
// COMMON FUNCTION: seBurstWriteMemory32()
//-----------------------------------------------------------------------------
void seBurstWriteMemory8( const UInt8* pData, UInt32 Count )
{
	//if ( Count && pData )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//	do
	//		*gpIndirectData = *pData++;
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

void seBurstWriteMemory16( const UInt16* pData, UInt32 Count )
{
	//if ( Count && pData )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//	do
	//	{
	//		*gpIndirectData = (UInt8)(*pData >> 8);
	//		*gpIndirectData = (UInt8)*pData++;
	//	}
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

void seBurstWriteMemory32( const UInt32* pData, UInt32 Count )
{
	//if ( Count && pData )
	//{
	//	seOsSetProcessExclusivity( TRUE );

	//	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
	//	do
	//	{
	//		*gpIndirectData = (UInt8)(*pData >> 8);
	//		*gpIndirectData = (UInt8)*pData;
	//		*gpIndirectData = (UInt8)(*pData >> 24);
	//		*gpIndirectData = (UInt8)(*pData++ >> 16);
	//	}
	//	while ( --Count );

	//	seOsSetProcessExclusivity( FALSE );
	//}
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seSetISRCallback()
//-----------------------------------------------------------------------------
int seSetISRCallback( void (*pISRCallback)(void) )
{
	// If the driver has not been acquired, exit with an error.
	if ( gBaseAddress == 0 )
	{
		return FALSE;
	}

	pISRCallback;
	return TRUE;
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seReadRaw()
//-----------------------------------------------------------------------------
UInt32 seReadRaw( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize )
{
	UInt32 Value = 0;
	BaseIndex;

	if ( ByteSize == 1 )
		Value = *( UInt8*)(gBaseAddress + Offset);
	else if ( ByteSize == 2 )
		Value = *(UInt16*)(gBaseAddress + Offset);
	else if ( ByteSize == 4 )
		Value = *(UInt32*)(gBaseAddress + Offset);

	return Value;
}

//-----------------------------------------------------------------------------
// COMMON FUNCTION: seWriteRaw()
//-----------------------------------------------------------------------------
void seWriteRaw( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize, UInt32 Value )
{
	BaseIndex;

	if ( ByteSize == 1 )
		*( UInt8*)(gBaseAddress + Offset) = (UInt8)Value;
	else if ( ByteSize == 2 )
		*(UInt16*)(gBaseAddress + Offset) = (UInt16)Value;
	else if ( ByteSize == 4 )
		*(UInt32*)(gBaseAddress + Offset) = (UInt32)Value;
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: SetMemoryReadAddress()
//---------------------------------------------------------------------------
static void SetMemoryReadAddress( UInt32 Offset )
{
//	*gpIndirectIndex = (UInt8)REGA2_MEM_RD_ADDR_0;

	*gpIndirectData  = (UInt8)Offset;
	*gpIndirectData  = (UInt8)(Offset >> 8);
	*gpIndirectData  = (UInt8)(Offset >> 16);

	*gpIndirectIndex = (UInt8)REGA0_MEM_PORT;
}
