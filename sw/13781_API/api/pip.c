//===========================================================================
//	PIP.C
//
// (C)SEIKO EPSON CORPORATION 2005 - 2009. All rights reserved.
//===========================================================================

#include "seapi.h"
#include "registers.h"

#define MEM_SIZE 384*1024

//===========================================================================
// PIP
//===========================================================================

//---------------------------------------------------------------------------
// sePipSetDisplayMode()
//---------------------------------------------------------------------------
void sePipSetDisplayMode( int Mode )
{
	UInt16 CurrentMode = (UInt16)(seReadReg16( REG60_PIP_EN ) & 7);

	if ((CurrentMode == sePipFadeIn) || (CurrentMode == sePipFadeOut) || (CurrentMode == sePipContinous))
		sePipWaitForFade( 5000 );

	seModifyReg16( REG60_PIP_EN , 0x0007, (Mode & 0x0007) );
}



//---------------------------------------------------------------------------
// sePipGetDisplayMode()
//---------------------------------------------------------------------------
Boolean sePipGetDisplayMode( void )
{
	if ( seReadReg16( REG60_PIP_EN ) & 0x0007 )
		return TRUE;
	else
		return FALSE;
}


//---------------------------------------------------------------------------
// sePipSetRotation()
//---------------------------------------------------------------------------
void sePipSetRotation( UInt32 RotationDegrees )
{
	seASSERT( (RotationDegrees==0) || (RotationDegrees==90) || (RotationDegrees==180) || (RotationDegrees==270), ("sePipSetRotation(): RotationDegrees must be 0, 90, 180, or 270.") );

	seModifyReg16( REG50_PIP_SET, 0x0018, RotationDegrees/90 << 3 );
}


//---------------------------------------------------------------------------
// sePipGetRotation()
//---------------------------------------------------------------------------
UInt32 sePipGetRotation( void )
{
	return ( seReadReg16( REG50_PIP_SET ) >> 3 ) * 90;
}


//---------------------------------------------------------------------------
// sePipIsOrthogonal()
//---------------------------------------------------------------------------
Boolean sePipIsOrthogonal( void )
{
	if ( (seReadReg16( REG50_PIP_SET ) & 0x0018) == (seReadReg16( REG40_MAIN_SET ) & 0x0018))
		return TRUE;
	else
		return FALSE;
}



//---------------------------------------------------------------------------
// sePipSetColorDepth()
//---------------------------------------------------------------------------
void sePipSetColorDepth( seFormatData ColorDepth )
{
	seASSERT( ColorDepth==seFormat_RGB_888      || ColorDepth==seFormat_RGB_565 ||
			  ColorDepth==seFormat_RGB_332_MONO || ColorDepth==seFormat_RGB_888LUT ||
			  ColorDepth==seFormat_RGB_565LUT   || ColorDepth== seFormat_RGB_332LUT,
			("sePipSetColorDepth(): BitsPerPixel must be one of the values specified in seFormatData.") );

	seModifyReg16( REG50_PIP_SET, 0x0007, ColorDepth );
}


//---------------------------------------------------------------------------
// sePipGetColorDepth()
//---------------------------------------------------------------------------
seFormatData sePipGetColorDepth( void )
{
	return ( seReadReg16( REG50_PIP_SET ) & 0x0007 );
}


//---------------------------------------------------------------------------
// sePipGetBytesPerPixel()
//---------------------------------------------------------------------------
UInt32 sePipGetBytesPerPixel( void )
{
	UInt32 bytes;
	switch ( sePipGetColorDepth() )
	{
	  case seFormat_RGB_332_MONO:
	  case seFormat_RGB_332LUT:
		bytes = 1;
		break;
	  case seFormat_RGB_565:
	  case seFormat_RGB_565LUT:
		bytes = 2;
		break;
	  case seFormat_RGB_888:
	  case seFormat_RGB_888LUT:
		bytes = 3;
		break;
	  default:
		bytes = 0;
	}
	return bytes;
}


//---------------------------------------------------------------------------
// sePipSetStartAddress()
//---------------------------------------------------------------------------
void sePipSetStartAddress( UInt32 StartAddress )
{
	seASSERT( ((StartAddress & 0x0003) == 0), ("sePipSetStartAddress(): Must be 32bit aligned.") );
	seASSERT( StartAddress < MEM_SIZE, ("sePipSetStartAddress(): StartAddress is too big.") );

	seWriteReg16( REG52_PIP_SADDR_0, StartAddress );
	seWriteReg16( REG54_PIP_SADDR_1, StartAddress >> 16 );

}


//---------------------------------------------------------------------------
// sePipGetStartAddress()
//---------------------------------------------------------------------------
UInt32 sePipGetStartAddress( void )
{
	return seReadReg16( REG52_PIP_SADDR_0 ) | (seReadReg16( REG54_PIP_SADDR_1) << 16);
}



//---------------------------------------------------------------------------
// sePipSetWidth()
//---------------------------------------------------------------------------
void sePipSetWidth( UInt32 Width )
{
//	seASSERT( Width < seMainGetWidth(), ("sePipSetWidth(): Width value too big"));

	seWriteReg16( REG56_PIP_WIDTH, Width );
}


//---------------------------------------------------------------------------
// sePipGetWidth()
//---------------------------------------------------------------------------
UInt32 sePipGetWidth( void )
{
	return seReadReg16( REG56_PIP_WIDTH );
}


//---------------------------------------------------------------------------
// sePipSetHeight()
//---------------------------------------------------------------------------
void sePipSetHeight( UInt32 Height )
{
//	seASSERT( Width < seMainGetHeight(), ("sePipSetHeight(): Height value too big"));

	seWriteReg16( REG58_PIP_HEIGHT, Height );
}



//---------------------------------------------------------------------------
// sePipGetHeight()
//---------------------------------------------------------------------------
UInt32 sePipGetHeight( void )
{
	return seReadReg16( REG58_PIP_HEIGHT );
}



//---------------------------------------------------------------------------
// sePipSetPosition()
//---------------------------------------------------------------------------
void sePipSetPosition( UInt32 x, UInt32 y )
{
	UInt32 xStart, yStart;
	UInt32 pipWidth, pipHeight;

	// BobE TODO: I hope I got all these cases in the correct order.

	pipWidth = seReadReg16( REG56_PIP_WIDTH );
	pipHeight = seReadReg16( REG58_PIP_HEIGHT );

	switch ( sePipGetRotation() )
	{
	  case 0:
		xStart = x;
		yStart = y;
		break;

	  case 90:
		xStart = x - pipHeight;
		yStart = y;
		break;
	  case 180:
		xStart = x + pipWidth;
		yStart = y + pipHeight;
		break;
	  case 270:
		xStart = x;
		yStart = y + pipHeight;
		break;
	  default:
		xStart = 0;
		yStart = 0;
	}

	seWriteReg16( REG5A_PIP_XSTART, xStart );
	seWriteReg16( REG5C_PIP_YSTART, yStart );
}


//---------------------------------------------------------------------------
// sePipGetPosition()
//---------------------------------------------------------------------------
void sePipGetPosition( UInt32 *px, UInt32 *py )
{
	UInt32 xStart, yStart;
	UInt32 pipWidth, pipHeight;
	UInt32 x, y;

	// BobE TODO: I hope I got all these cases in the correct order.

	xStart = seReadReg16( REG5A_PIP_XSTART );
	yStart = seReadReg16( REG5C_PIP_YSTART );
	pipWidth = seReadReg16( REG56_PIP_WIDTH );
	pipHeight = seReadReg16( REG58_PIP_HEIGHT );

	switch ( sePipGetRotation() )
	{
	  case 0:
		x = xStart;
		y = yStart;
		break;

	  case 90:
		x = xStart + pipHeight;
		y = yStart;
		break;
	  case 180:
		x = xStart - pipWidth;
		y = yStart - pipHeight;
		break;
	  case 270:
		x = xStart;
		y = yStart - pipHeight;
		break;
	  default:
		  x = 0;
		  y = 0;
	}

	if (px != NULL)
		*px = x;
	if (py != NULL)
		*py = y;
}


//---------------------------------------------------------------------------
//	sePipSetFadeRate()
//---------------------------------------------------------------------------
void sePipSetFadeRate( UInt32 FadeRate )
{
	seModifyReg16( REG60_PIP_EN, 0xFC00, (FadeRate & 0xFC) << 9);
}


//---------------------------------------------------------------------------
//	sePipGetFadeRate()
//---------------------------------------------------------------------------
UInt32 sePipGetFadeRate( void )
{
	return seReadReg16( REG60_PIP_EN ) >> 9;
}


//---------------------------------------------------------------------------
//	sePipWaitForFade()
//---------------------------------------------------------------------------
Boolean sePipWaitForFade( UInt32 MaxTime )
{
	Boolean fSuccess;

	fSuccess = sePollWhileHighReg16( REG60_PIP_EN, 0x0008, 0, MaxTime );

	return fSuccess;
}


//---------------------------------------------------------------------------
//	sePipSetAlphaBlendStep()
//---------------------------------------------------------------------------
void sePipSetAlphaBlendStep( UInt32 Step )
{
	seASSERT( (Step==1) || (Step==2) || (Step==4), ("sePipSetAlphaBlendStep(): value must be 1, 2, or 4.") );

	switch ( Step )
	{
		case 1:		Step = 0; break;
		case 2:		Step = 1; break;
		case 4:		Step = 2; break;
		default:	Step = 0; break;	// Do something for bad input
	}

	seModifyReg16( REG62_ALPHA, 0x0700, (Step & 7) << 8 );
}


//---------------------------------------------------------------------------
//	sePipGetAlphaBlendStep()
//---------------------------------------------------------------------------
UInt32 sePipGetAlphaBlendStep( void )
{
	return seReadReg16( REG62_ALPHA ) >> 8;
}


//---------------------------------------------------------------------------
//	sePipSetAlphaBlendRatio()
//---------------------------------------------------------------------------
void sePipSetAlphaBlendRatio( UInt32 Ratio )
{
	UInt32 abr;

	seASSERT( (Ratio <= 100), ("sePipSetAlphaBlendRation(): value must be 0% to 100%.") );

	// Calculate the nearest register value (abr) to the requested Alpha %
	// abr/64 = Ratio/100
	// abr = Ratio * 64 / 100
	abr = (Ratio * 64 / 100) - 1;
	
	seModifyReg16( REG62_ALPHA, 0x007F, (abr & 0x7F) );
}


//---------------------------------------------------------------------------
//	sePipGetAlphaBlendRatio()
//---------------------------------------------------------------------------
UInt32 sePipGetAlphaBlendRatio( void )
{
	return seReadReg16( REG62_ALPHA ) * 100 / 64; 
}


//---------------------------------------------------------------------------
//	sePipEnableTranparency()
//---------------------------------------------------------------------------
void sePipEnableTranparency( Boolean fEnable )
{
	if ( fEnable )
		seModifyReg16( REG64_TRANS, 0x0000, 0x0001 );
	else
		seModifyReg16( REG64_TRANS, 0x0000, 0x0000 );
}


//---------------------------------------------------------------------------
//	sePipGetTransparency()
//---------------------------------------------------------------------------
Boolean sePipGetTransparency( void )
{
	if ( seReadReg16( REG64_TRANS ) )
		return TRUE;
	else
		return FALSE;
}


//---------------------------------------------------------------------------
//	sePipSetTransColor()
//---------------------------------------------------------------------------
void sePipSetTransColor( RGB32 Clr )
{
	seWriteReg16( REG66_KEY_0, Clr.rgb32 );
	seWriteReg16( REG68_KEY_1, (Clr.rgb32)>>16 );
}


//---------------------------------------------------------------------------
//	sePipGetTransColor()
//---------------------------------------------------------------------------
void sePipGetTransColor( RGB32 * pClr )
{
	if ( pClr != NULL )
		(*pClr).rgb32 = seReadReg16( REG66_KEY_0 ) | (seReadReg16( REG68_KEY_1 )<<16);
}



//---------------------------------------------------------------------------
// sePipSetupWindow()
//---------------------------------------------------------------------------
void sePipSetupWindow( UInt32 x, UInt32 y, UInt32 Width, UInt32 Height )
{
	UInt32 xStart, yStart;

//	seASSERT( Width < seMainGetWidth(), ("sePipSetWidth(): Width value too big"));
	seWriteReg16( REG56_PIP_WIDTH, Width );

//	seASSERT( Width < seMainGetHeight(), ("sePipSetHeight(): Height value too big"));
	seWriteReg16( REG58_PIP_HEIGHT, Height );

	// BobE TODO: I hope I got all these cases in the correct order.
	switch ( sePipGetRotation() )
	{
	  case 0:
		xStart = x;
		yStart = y;
		break;

	  case 90:
		xStart = x - Height;
		yStart = y;
		break;
	  case 180:
		xStart = x + Width;
		yStart = y + Height;
		break;
	  case 270:
		xStart = x;
		yStart = y + Height;
		break;
	  default:
		xStart = 0;
		yStart = 0;
		break;

	}

	seWriteReg16( REG5A_PIP_XSTART, xStart );
	seWriteReg16( REG5C_PIP_YSTART, yStart );
}



//---------------------------------------------------------------------------
// sePipGetStride()
//---------------------------------------------------------------------------
UInt32 sePipGetStride( void )
{
	UInt32 bytes, width;

	switch (sePipGetColorDepth())
	{
	case seFormat_RGB_332_MONO:
	case seFormat_RGB_332LUT:
		bytes = 1;
		break;

	case seFormat_RGB_565:
	case seFormat_RGB_565LUT:
		bytes = 2;
		break;

	case seFormat_RGB_888:
	case seFormat_RGB_888LUT:
		bytes = 3;
		break;
	default:
		bytes = 0;
	}
	
	width = sePipGetWidth();

	return width * bytes;
}