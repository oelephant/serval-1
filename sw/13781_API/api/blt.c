//===========================================================================
//	BLT.C
//
//	(C) SEIKO EPSON CORPORATION 2009. All rights reserved
//===========================================================================

#include "hcl.h"
#include "display.h"
#include "os.h"
#include "settings.h"
#include "blt.h"

#include <stdio.h>

#define LOBYTE(w)		((UInt8)(w))
#define HIBYTE(w)		((UInt8)(((UInt16)(w) >> 8) & 0xFF))
#define LOWORD(l)		((UInt16)(UInt32)(l))
#define HIWORD(l)		((UInt16)((((UInt32)(l)) >> 16) & 0xFFFF))


//===========================================================================
// Private Function Prototypes
//===========================================================================
Boolean	seBltRun( seBltData * pBltInfo );
Boolean	IsOverlap( seBltData * pBltInfo );
void	CalcBltOffsets( UInt32 SrcDst, seBltData * pBltInfo, UInt32 * pFirst, UInt32 * pLast, UInt32 * pStride );
static	Boolean	WaitForComplete( void );



//===========================================================================
// Start of CODE
//===========================================================================

//---------------------------------------------------------------------------
// seBltApiInitialize()
//---------------------------------------------------------------------------
void seBltApiInitialize( void )
{
	// Reset the BitBLT engine.
	seWriteReg16( REG80_BLT_CTRL_0, 0x0080 );
}



//---------------------------------------------------------------------------
// seBltApiShutdown() - .
//---------------------------------------------------------------------------
void seBltApiShutdown( void )
{
	;
}



//---------------------------------------------------------------------------
// seBltBitBlt()
//---------------------------------------------------------------------------
Boolean seBltBitBlt( seBltData * pBltInfo )
{
	Boolean tmp;

	// The following limitations apply to all BitBLTs
	seASSERT( (pBltInfo->Width  >= 1) && (pBltInfo->Width  <= 1023), ("BitBLT: 1 <= Width <= 1023") );
	seASSERT( (pBltInfo->Height >= 1) && (pBltInfo->Height <= 1023), ("BitBLT: 1 <= Height <= 1023") );

	switch ( pBltInfo->Operation )
	{
	  case seBLT_OPERATION_MOVE_POS:
	  case seBLT_OPERATION_MOVE_COLOREXP:
	  case seBLT_OPERATION_SOLID_FILL:
		tmp =  seBltRun( pBltInfo );
		return tmp;
		break;
	  case seBLT_OPERATION_MOVE_NEG: // program shouldn't go here since the calling routine should not have differentiated between + and - Move BLTs yet
	  default:
		return FALSE;
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
/// seBltRun( ) - .
////////////////////////////////////////////////////////////////////////////////
Boolean seBltRun( seBltData * pBltInfo )
{
	UInt16	Reg82, Reg86;
	UInt32	BytesPP, SrcTop, SrcEnd, DstTop, DstEnd, SrcAddr, DstAddr;
	Boolean	fReverse = FALSE;

	switch ( pBltInfo->ColorDepth )
	{
		case 24:	Reg82 = 0x08;	break;
		case 16:	Reg82 = 0x04;	break;
		default:
		case 8:		Reg82 = 0x00;	break;
	}
	if ( pBltInfo->fDstLinear )
		Reg82 |= 0x02;
	if ( pBltInfo->fSrcLinear )
		Reg82 |= 0x01;
	seWriteReg16( REG82_BLT_CTRL_1, Reg82 );

	Reg86 = (UInt16)(pBltInfo->Operation);
	if ( (pBltInfo->Operation==seBLT_OPERATION_MOVE_POS) && IsOverlap(pBltInfo) ) // if Move BLT and Src overlap Dst then do a Move Negative BLT
	{
		Reg86 = seBLT_OPERATION_MOVE_NEG;
		fReverse = TRUE;
	}
	seWriteReg16( REG86_BLT_CMD, Reg86 );

	BytesPP = pBltInfo->ColorDepth / 8;
	CalcBltOffsets( seBLT_SRC, pBltInfo, &SrcTop, &SrcEnd, NULL );
	CalcBltOffsets( seBLT_DST, pBltInfo, &DstTop, &DstEnd, NULL );
	if ( fReverse ) // 781 internal addressing is byte-based
	{
		SrcEnd += (BytesPP-1);
		DstEnd += (BytesPP-1);
	}
	SrcAddr = !fReverse ? SrcTop : SrcEnd;
	DstAddr = !fReverse ? DstTop : DstEnd;
	seWriteReg16( REG88_BLT_SSADDR_0, LOWORD(SrcAddr) );
	seWriteReg16( REG8A_BLT_SSADDR_1, HIWORD(SrcAddr) );
	seWriteReg16( REG8C_BLT_DSADDR_0, LOWORD(DstAddr) );
	seWriteReg16( REG8E_BLT_DSADDR_1, HIWORD(DstAddr) );

	seWriteReg16( REG90_BLT_RECTOFFSET, pBltInfo->MemoryOffset/(pBltInfo->MemoryBpp/8) ); // 781 requires stride in pixels (not bytes)
	seWriteReg16( REG92_BLT_WIDTH, pBltInfo->Width );
	seWriteReg16( REG94_BLT_HEIGHT, pBltInfo->Height );

	if ( pBltInfo->Operation==seBLT_OPERATION_MOVE_COLOREXP )
	{
		seWriteReg16( REG96_BLT_BGCOLOR_0, LOWORD(pBltInfo->BackGrndColor) );
		seWriteReg16( REG98_BLT_BGCOLOR_1, HIWORD(pBltInfo->BackGrndColor) );
	}
	if ( pBltInfo->Operation==seBLT_OPERATION_SOLID_FILL || pBltInfo->Operation==seBLT_OPERATION_MOVE_COLOREXP )
	{
		seWriteReg16( REG9A_BLT_FGCOLOR_0, LOWORD(pBltInfo->ForeGrndColor) );
		seWriteReg16( REG9C_BLT_FGCOLOR_1, HIWORD(pBltInfo->ForeGrndColor) );
	}

	// Start the BitBLT
	seWriteReg16( REG80_BLT_CTRL_0, 1 );
	
	// Wait for completion
	return WaitForComplete();
}


//==============================================================================
// Private BitBLT Functions
//==============================================================================

//--------------------------------------------------------------------------
//	IsOverlap
//
//	This routine determines if there is a destructive overlap of the source
//	and destination BitBLT rectangles. A destructive overlap is one where
//	the destination data in a MoveBLT will overwrite source data before
//	the source data is used. In these cases the BitBLT engine must be
//	run with negative stepping.
//
//	Returns:
//		TRUE  - if there is a destructive overlap.
//		FALSE - for all other cases
//--------------------------------------------------------------------------
Boolean IsOverlap( seBltData * pBltInfo )
{
	UInt32 SrcAddrMin, SrcAddrMax;
	UInt32 DstAddrMin, DstAddrMax;

	// Special case Dst:Rect & Src:Rect
	if ( !pBltInfo->fSrcLinear && !pBltInfo->fDstLinear )
	{
		int dsttop    = pBltInfo->DstYposition;
		int dstbottom = dsttop + pBltInfo->Height;
		int dstleft   = pBltInfo->DstXposition;
		int dstright  = dstleft + pBltInfo->Width;

		int srctop    = pBltInfo->SrcYposition;
		int srcbottom = srctop + pBltInfo->Height;
		int srcleft   = pBltInfo->SrcXposition;
		int srcright  = srcleft + pBltInfo->Width;

		if ( (srcbottom >= dsttop) && (srcbottom < dstbottom) )
			if ( ((srcleft >= dstleft) && (srcleft < dstright)) || ((srcright >= dstleft) && (srcright <= dstright)) )
				return TRUE;

		if ( srcbottom == dstbottom )
			if ( (srcright >= dstleft) && (srcright < dstright) )
				return TRUE;

		return FALSE;
	}

	// All other cases
	CalcBltOffsets( seBLT_SRC, pBltInfo, &SrcAddrMin, &SrcAddrMax, NULL );
	CalcBltOffsets( seBLT_DST, pBltInfo, &DstAddrMin, &DstAddrMax, NULL );

	if ((SrcAddrMin > DstAddrMax) || (SrcAddrMax < DstAddrMin))
		return FALSE;

	if ( pBltInfo->fSrcLinear && pBltInfo->fDstLinear ) // for Linear Src & Dst, overlap is not destructive if SrcAddr>DstAddr as long as BLT traverses forward direction
		if ( (SrcAddrMin>DstAddrMin) && (SrcAddrMax>DstAddrMax) )
			return FALSE;

	return TRUE;
}



//-------------------------------------------------------------------------------
//	CalcBltOffsets()
//
//	Determines the first and last address of the BitBLT rectangle as well as
//	the stride that was used to calculate the values.
//	The stride is necessary because it is different for linear<->rectangular
//	calculations.
//-------------------------------------------------------------------------------
void CalcBltOffsets( UInt32 SrcDst, seBltData *pBltInfo, UInt32 *pFirst, UInt32 *pLast, UInt32 *pStride )
{
	UInt32 X, Y, Start, End;
	UInt32 Stride, Bits;
	Boolean Linear;
	UInt32 BytesPP = pBltInfo->ColorDepth / 8;

	if ( SrcDst == seBLT_SRC )
	{
		Linear = pBltInfo->fSrcLinear;
		Start  = pBltInfo->SrcBaseAddress;
		X      = pBltInfo->SrcXposition;
		Y      = pBltInfo->SrcYposition;
	}
	else
	{
		Linear = pBltInfo->fDstLinear;
		Start  = pBltInfo->DstBaseAddress;
		X      = pBltInfo->DstXposition;
		Y      = pBltInfo->DstYposition;
	}

	if ( Linear )
	{
		// No need to compute Start, it is simply the Base Address
		if ( SrcDst==seBLT_SRC && pBltInfo->Operation==seBLT_OPERATION_MOVE_COLOREXP ) // special case 1 bit per pixel
		{
			// note: if Width not a multiple of 8 then add 1 to Stride and End
			Stride = (pBltInfo->Width/8) + ((pBltInfo->Width%8)?1:0);
			Bits = pBltInfo->Height * pBltInfo->Width;
			End = Start + (Bits/8) + ((Bits%8)?1:0);
		}
		else
		{
			Stride = pBltInfo->Width * BytesPP;
			End = Start + pBltInfo->Height * (pBltInfo->Width*BytesPP);
		}
	}
	else
	{
		Stride = pBltInfo->MemoryOffset;
		Start += (Y*Stride) + (X*BytesPP);
		End = Start + ((pBltInfo->Height - 1)*Stride) + ((pBltInfo->Width - 1)*BytesPP);
	}


	if ( pFirst != NULL )
		*pFirst  = Start;
	if ( pLast != NULL )
		*pLast   = End;
	if ( pStride != NULL )
		*pStride = Stride;
}


//------------------------------------------------------------------------------
//	WaitForComplete()
//
//	This routine polls BLT Status to wait for BitBLT operations to complete.
//------------------------------------------------------------------------------
static Boolean WaitForComplete( void )
{
	return sePollWhileHighReg16( REG84_BLT_STATUS, 0x0001, 0, seBLT_COMPLETE_TIMEOUT );
}
