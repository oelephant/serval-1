//===========================================================================
//	DRAW.C - Auxilliary Helper Library - Draw Functions
//
//	(C)SEIKO EPSON CORPORATION 2006. All rights reserved.
//===========================================================================


#include "seaux.h"



void seAuxDrawHLine( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Width, const void* pColor )
{
	UInt8*	pTemp;
	int		i;

	if ( pDrawInfo )
	{
		
		pTemp = y*pDrawInfo->Stride + x*pDrawInfo->ByteSize + pDrawInfo->Address;
		
		while ( Width-- )
		{
			UInt8* pColorArray = (UInt8*)pColor;
			for ( i = pDrawInfo->ByteSize; i != 0; i-- )
			{
				*pTemp++ = *pColorArray++;
			}
		}
	}
}



void seAuxDrawVLine( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Height, const void* pColor )
{
	UInt8*	pTemp;
	int		i;

	if ( pDrawInfo )
	{
		
		pTemp = y*pDrawInfo->Stride + x*pDrawInfo->ByteSize + pDrawInfo->Address;
		
		while ( Height-- )
		{
			UInt8* pColorArray = (UInt8*)pColor;
			for ( i = pDrawInfo->ByteSize; i != 0; i-- )
			{
				*pTemp++ = *pColorArray++;
			}
			pTemp += pDrawInfo->Stride - pDrawInfo->ByteSize;
		}
	}
}



void seAuxDrawRectangle( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Width, UInt32 Height, const void* pColor )
{
	UInt8*	pTemp;
	int		i, j;

	if ( pDrawInfo && Width && Height )
	{
		pTemp = y*pDrawInfo->Stride + x*pDrawInfo->ByteSize + pDrawInfo->Address;
		
		while ( Height-- )
		{
			for ( i = Width; i != 0; i-- )
			{
				UInt8* pColorArray = (UInt8*)pColor;
				for ( j = pDrawInfo->ByteSize; j != 0; j-- )
				{
					*pTemp++ = *pColorArray++;
				}
			}
			pTemp += pDrawInfo->Stride - Width*pDrawInfo->ByteSize;
		}
	}
}



void seAuxDrawBorder( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Width, UInt32 Height, const void* pColor )
{
	if ( pDrawInfo && Width && Height )
	{

		seAuxDrawHLine( pDrawInfo, x, y, Width, pColor );
		seAuxDrawHLine( pDrawInfo, x, y+Height-1, Width, pColor );

		if ( Height > 2 )
		{
			seAuxDrawVLine( pDrawInfo, x, y+1, Height-2, pColor );
			seAuxDrawVLine( pDrawInfo, x+Width-1, y+1, Height-2, pColor );
		}
	}
}
