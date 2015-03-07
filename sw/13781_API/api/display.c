//===========================================================================
//	DISPLAY.C
//
//  (C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//===========================================================================


#include "hcl.h"
#include "os.h"
#include "display.h"
#include "clock.h"


//---------------------------------------------------------------------------
// seLcdSetRotation()
//---------------------------------------------------------------------------
void seLcdSetRotation( UInt32 RotationDegrees )
{
	seASSERT( (RotationDegrees==0) || (RotationDegrees==90) || (RotationDegrees==180) || (RotationDegrees==270), ("seLcdSetRotation(): RotationDegrees must be 0, 90, 180, or 270.") );

	seModifyReg16( REG40_MAIN_SET, 0x0018, RotationDegrees/90 << 3 );
}


//---------------------------------------------------------------------------
// seLcdGetRotation()
//---------------------------------------------------------------------------
UInt32 seLcdGetRotation( void )
{
	return ( seReadReg16( REG40_MAIN_SET ) >> 3 ) * 90;
}


//---------------------------------------------------------------------------
// seLcdSetColorDepth()
//---------------------------------------------------------------------------
void seLcdSetColorDepth( seFormatData ColorDepth )
{
	seASSERT( ColorDepth==seFormat_RGB_888      || ColorDepth==seFormat_RGB_565 ||
			  ColorDepth==seFormat_RGB_332_MONO || ColorDepth==seFormat_RGB_888LUT ||
			  ColorDepth==seFormat_RGB_565LUT   || ColorDepth== seFormat_RGB_332LUT,
			("seLcdSetColorDepth(): BitsPerPixel must be one of the values specified in seFormatData.") );

	seModifyReg16( REG40_MAIN_SET, 0x0007, ColorDepth );
}



//---------------------------------------------------------------------------
// seLcdGetColorDepth()
//---------------------------------------------------------------------------
seFormatData seLcdGetColorDepth( void )
{
	return ( seReadReg16( REG40_MAIN_SET ) & 0x0007 );
}



//---------------------------------------------------------------------------
// seLcdGetBytesPerPixel()
//---------------------------------------------------------------------------
UInt32 seLcdGetBytesPerPixel( void )
{
	UInt32 bytes;
	switch ( seLcdGetColorDepth() )
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
// seLcdSetStartAddress()
//---------------------------------------------------------------------------
void seLcdSetStartAddress( UInt32 StartAddress )
{
	seASSERT( ((StartAddress & 0x0003) == 0), ("seLcdSetStartAddress(): Must be 32bit aligned.") );
	seASSERT( StartAddress < seVMEM_SIZE, ("seLcdSetStartAddress(): StartAddress is too big.") );

	seWriteReg16( REG42_MAIN_SADDR_0, StartAddress );
	seWriteReg16( REG44_MAIN_SADDR_1, StartAddress >> 16 );
}



//---------------------------------------------------------------------------
// seLcdGetStartAddress()
//---------------------------------------------------------------------------
UInt32 seLcdGetStartAddress( void )
{
	return seReadReg16(REG42_MAIN_SADDR_0) | (seReadReg16(REG44_MAIN_SADDR_1)<<16);
}



//---------------------------------------------------------------------------
// seLcdSetWidth()
//---------------------------------------------------------------------------
void seLcdSetWidth( UInt32 Width )
{
	seWriteReg16( REG24_HDISP, Width/8 );
}



//---------------------------------------------------------------------------
// seLcdGetWidth() - .
//---------------------------------------------------------------------------
UInt32 seLcdGetWidth( void )
{
	return seReadReg16( REG46_MAIN_WIDTH );
}



//---------------------------------------------------------------------------
// seLcdSetHeight()
//---------------------------------------------------------------------------
void seLcdSetHeight( UInt32 Height )
{
	seWriteReg16( REG28_VDISP, Height );
}



//---------------------------------------------------------------------------
// seLcdGetHeight() - .
//---------------------------------------------------------------------------
UInt32 seLcdGetHeight( void )
{
	return seReadReg16( REG48_MAIN_HEIGHT );
}



//---------------------------------------------------------------------------
// seLcdGetStride()
//---------------------------------------------------------------------------
UInt32 seLcdGetStride( void )
{
	return seLcdGetWidth() * seLcdGetBytesPerPixel();
}


//===========================================================================
// LUT Functions
//===========================================================================


//---------------------------------------------------------------------------
// seLcdGetLutEntry() - .
//---------------------------------------------------------------------------
seRgbData seLcdGetLutEntry( UInt32 Index, seWindowData Window )
{
	seRgbData RgbData;
	UInt32 DataReg;

	seASSERT( Index<=255, ("seLcdGetLutEntry() Index must be between 0 and 255.") );

	DataReg = (Window==seWindow_Main?0x00060000:0x00060400) + (Index*4);

	RgbData.ColorRef = seReadMemory32( DataReg );

	return RgbData;
}



//---------------------------------------------------------------------------
// seLcdGetLut() - .
//---------------------------------------------------------------------------
void seLcdGetLut( UInt32 StartIndex, seRgbData *pRgbData, UInt32 Count, seWindowData Window )
{
	UInt32 DataReg;

	seASSERT( StartIndex<=255, ("seLcdGetLut() StartIndex must be between 0 and 255.") );
	seASSERT( StartIndex+Count<=256, ("seLcdGetLut() StartIndex+Count exceeds table size.") );

	DataReg = (Window==seWindow_Main?0x00060000:0x00060400) + (StartIndex*4);

	if ( pRgbData )
		seBurstReadMemory32( DataReg, &(pRgbData->ColorRef), Count );
}



//---------------------------------------------------------------------------
// seLcdSetLutEntry() - .
//---------------------------------------------------------------------------
void seLcdSetLutEntry( UInt32 Index, const seRgbData *pRgbData, seWindowData Window )
{
	UInt32 DataReg;

	seASSERT( Index<=255, ("seLcdSetLutEntry() Index must be between 0 and 255.") );

	DataReg = (Window==seWindow_Main?0x00060000:0x00060400) + (Index*4);

	if ( pRgbData )
		seWriteMemory32( DataReg, pRgbData->ColorRef );
}



//---------------------------------------------------------------------------
// seLcdSetLut() - .
//---------------------------------------------------------------------------
void seLcdSetLut( UInt32 StartIndex, const seRgbData *pRgbData, UInt32 Count, seWindowData Window )
{
	UInt32 DataReg;

	seASSERT( StartIndex<=255, ("seLcdSetLut() StartIndex must be between 0 and 255.") );
	seASSERT( StartIndex+Count<=256, ("seLcdSetLut() StartIndex+Count exceeds table size.") );

	DataReg = (Window==seWindow_Main?0x00060000:0x00060400) + (StartIndex*4);

	if ( pRgbData )
		seBurstWriteMemory32( DataReg, &(pRgbData->ColorRef), Count );
}



//---------------------------------------------------------------------------
// seLcdSetLutDefault() - .
// Notes:
//  1. When expanding 5-bit/6-bit data to 8 bits, use bit padding technique
//     which replicates the upper MSBits of data onto the empty bits.
//  2. Each LUT entry consists of 3 components: Rcomp, Gcomp, Bcomp
//---------------------------------------------------------------------------
void seLcdSetLutDefault( seWindowData Window )
{
	UInt32 i, lutsize, format, temp, r, g, b;
	seRgbData Array[256];

	format = seReadReg16(Window==seWindow_Main?REG40_MAIN_SET:REG50_PIP_SET) & 0x07;
	if ( format==1 || format==5 ) // 16bpp
	{
		lutsize = 64;
		for ( i=0; i<lutsize; i++ )
		{
			temp = (UInt8)(i << 3);
			Array[i].rgb.Red   = (UInt8)(temp | (temp>>5));
			temp = (UInt8)(i << 2);
			Array[i].rgb.Green = (UInt8)(temp | (temp>>6));
			temp = (UInt8)(i << 3);
			Array[i].rgb.Blue  = (UInt8)(temp | (temp>>5));
		}
	}
	else if ( format == 6 ) // 8bpp
	{
		lutsize = 256;
		if ( seReadReg16(REG20_PANEL_SET) & 0x08 ) // color panel
		{
			for ( i=0; i<lutsize; i++ )
			{
				temp = (UInt8)((i&0xE0) << 0);
				Array[i].rgb.Red   = (UInt8)(temp | (temp>>3) | (temp>>6));
				temp = (UInt8)((i&0x1C) << 3);
				Array[i].rgb.Green = (UInt8)(temp | (temp>>3) | (temp>>6));
				temp = (UInt8)((i&0x03) << 6);
				Array[i].rgb.Blue  = (UInt8)(temp | (temp>>2) | (temp>>4) | (temp>>6));
			}
		}
		else // monochrome panel
		{
			for ( i=0; i<lutsize; i++ )
			{
				temp = (UInt8)((i&0xE0) << 0);
				r = (UInt8)(temp | (temp>>3) | (temp>>6));
				temp = (UInt8)((i&0x1C) << 3);
				g = (UInt8)(temp | (temp>>3) | (temp>>6));
				temp = (UInt8)((i&0x03) << 6);
				b = (UInt8)(temp | (temp>>2) | (temp>>4) | (temp>>6));
				temp = (299*r + 587*g + 114*b)/1000; // get luminance
				Array[i].ColorRef = (UInt8)temp; // Bcomp=luminance, Rcomp=Gcomp=0
			}
		}
	}
	else if ( format == 2 ) // 8bpp mono
	{
		lutsize = 0; // do nothing since LUT is not used at this color format setting
	}
	else // 24bpp
	{
		lutsize = 256;
		for ( i=0; i<lutsize; i++ )
			Array[i].ColorRef = i | i<<8 | i<<16; // Rcomp=Gcomp=Bcomp
	}

	if ( format != 2 ) // program the LUT only if window format requires a LUT
		seLcdSetLut( 0, Array, lutsize, Window );
}
