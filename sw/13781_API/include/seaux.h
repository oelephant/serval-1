//////////////////////////////////////////////////////////////////////////////
///
///	\file	SEAUX.H
///
/// \par	Auxiliary API Functions
///	This modules contains a selection of optional functions which are not part of 
/// the core API but may still be helpful to developers. Example usage of these
/// functions can be found in the API sample programs.
///
//	(C)SEIKO EPSON CORPORATION 2006. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef seSEAUX_H_INCLUDED
#define seSEAUX_H_INCLUDED


#include "datatype.h"


typedef struct
{
	UInt8*	Address;			// Image data
	UInt32	Stride;				// Image stride
	UInt32	ByteSize;			// Number of bytes per pixel
} seDrawData;


// Allow both standard C and C++ linkages.
#ifdef __cplusplus
   extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Draws a horizontal line on the host memory image.
/// 
/// This function is used by the seAPI sample code to draw horizontal lines on 
/// the LCD display. The function performs no error checking or line clipping. 
/// The line is drawn by copying the color value pointed by pColor to every 
/// pixel of the line.
///
/// \param	pDrawInfo	Pointer to the structure describing the drawing surface.
///
///	\param	x			X coordinate of where to start drawing line.
///
///	\param	y			Y coordinate of where to start drawing line.
///
///	\param	Width		The width, in pixels, of the line to draw.
///
/// \param	pColor		Color of the horizontal line to be drawn.
///
//////////////////////////////////////////////////////////////////////////////
void seAuxDrawHLine( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Width, const void* pColor );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Draws a vertical line on the host memory image.
///
/// This function is used by the seAPI sample code to draw vertical lines on the 
/// LCD display. The function performs no error checking or line clipping. The 
/// line is drawn by copying the color value pointed by pColor to every pixel of 
/// the line.
///
/// \param	pDrawInfo	Pointer to the structure describing the drawing surface.
///
///	\param	x			X coordinate of where to start drawing line.
///
///	\param	y			Y coordinate of where to start drawing line.
///
///	\param	Height		The height, in scans, of the line to draw.
///
/// \param	pColor		Color of the vertical line to be drawn.
///
//////////////////////////////////////////////////////////////////////////////
void seAuxDrawVLine( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Height, const void* pColor );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Draws a solid rectangle on the host memory image.
///
/// This function is used by the seAPI sample code to draw a solid rectangle on 
/// the LCD display. The function performs no error checking or line clipping. 
/// The rectangle is drawn by copying the color value pointed by pColor to every 
/// pixel of the rectangular area.
///
/// \param	pDrawInfo	Pointer to the structure describing the drawing surface.
///
///	\param	x			X coordinate of the top-left corner of the rectangle.
///
///	\param	y			Y coordinate of the top-left corner of the rectangle.
///
///	\param	Width		The width, in pixels, of the rectangle to draw.
///
///	\param	Height		The height, in scans, of the rectangle to draw.
///
///	\param	pColor		The color of the solid rectangle to draw.
///
//////////////////////////////////////////////////////////////////////////////
void seAuxDrawRectangle( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Width, UInt32 Height, const void* pColor );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Draws a border only rectangle on the host memory image.
///
///
/// This function is used by the seAPI sample code to draw a border rectangle on 
/// the LCD display. The function performs no error checking or line clipping. 
/// The border rectangle is drawn by copying the color value pointed by pColor 
/// to every pixel of the border.
///
/// \param	pDrawInfo	Pointer to the structure describing the drawing surface.
///
///	\param	x			X coordinate of the top-left corner of the border.
///
///	\param	y			Y coordinate of the top-left corner of the border.
///
///	\param	Width		The width, in pixels, of the border to draw.
///
///	\param	Height		The height, in scans, of the border to draw.
///
///	\param	pColor		The color of the border rectangle to draw.
///
//////////////////////////////////////////////////////////////////////////////
void seAuxDrawBorder( const seDrawData* pDrawInfo, Int32 x, Int32 y, UInt32 Width, UInt32 Height, const void* pColor );



#ifdef __cplusplus
   }
#endif


#endif	// seSEAUX_H_INCLUDED
