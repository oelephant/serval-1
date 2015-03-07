////////////////////////////////////////////////////////////////////////////////
///
/// \file display.h
///
/// \par Display Overview
/// The Display API provides support for drawing images to display memory.
/// This API is divided into two sections: Main Layer Functions, and
/// LUT Functions.
///
/// \par Main Layer Functions
/// - seLcdSetRotation()
/// - seLcdGetRotation()
/// - seLcdSetColorDepth()
/// - seLcdGetColorDepth()
/// - seLcdGetBytesPerPixel()
/// - seLcdSetStartAddress()
/// - seLcdGetStartAddress()
/// - seLcdSetWidth()
/// - seLcdGetWidth()
/// - seLcdSetHeight()
/// - seLcdGetHeight()
/// - seLcdGetStride()
///
///	\par
/// The 13781 supports two types of layers: Main and PIP. The Main layer 
///	represents the background image, which fills the entire display. 
///	\par
/// The Main layer functions define the size, stride, position and 
///	orientation of the Main Layer.
///	\par
///	The PIP layer is an overlay window displayed on top of the Main layer.
/// A separate API file handles the PIP layer features.
///
/// \par LUT Functions
/// - seLcdGetLutEntry()
/// - seLcdGetLut()
/// - seLcdSetLut()
/// - seLcdSetLutDefault()
/// - seLcdSetLutEntry()
///	\par
/// The LUT (Look-Up Table) Functions define the two available LUTs
/// (Main and PIP).
///
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef seDISPLAY_H_INCLUDED
#define seDISPLAY_H_INCLUDED


#include "datatype.h"

////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Input images formats
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	seFormat_RGB_888		= 0,		///< RGB 8:8:8
	seFormat_RGB_565		= 1,		///< RGB 5:6:5
	seFormat_RGB_332_MONO	= 2,		///< RGB 3:3:2 Monochrome
	seFormat_Reserved		= 3,
	seFormat_RGB_888LUT		= 4,
	seFormat_RGB_565LUT		= 5,
	seFormat_RGB_332LUT		= 6,		///< RGB 3:3:2 Color
	seFormat_Invalid		= 7			///< Invalid Format
} seFormatData;


typedef union
{
	struct
	{
		UInt8	Blue;					// Blue primary
		UInt8	Green;					// Green primary
		UInt8	Red;					// Red primary
		UInt8	unused;					// not used
	} rgb;
	UInt32		ColorRef;				// RGB 32-bit composite value
} seRgbData;


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Tearing Effect functions
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	seTvOddVeritcalNonDisplayPeriod,	///< Tearing Effect output pin is high during an odd VNDP
	seTvEvenVeritcalNonDisplayPeriod,	///< Tearing Effect output pin is high during an even VNDP
	seTvMemoryFetchInactive,			///< Tearing Effect output pin is high when the output scalar 
										///< is not fetching/reading image data from memory and goes 
										///< low when the output scalar is fetching image data from memory.
	seTvVndpLineCount,					///< Tearing Effect output pin is high when the line count is reached
	seTvHndpOrVndp,						///< Tearing Effect output pin is high during an HNDP or a VNDP
	seTvVerticalNonDisplayPeriod,		///< Tearing Effect output pin is high during a VNDP
} seTearingEffectData;


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Input YUV formats
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	seDataType_YUV,				///< YUV, 0<=Y<=255, -128<=U<=127, -128<=V<=127
	seDataType_YCbCr,			///< YCbCr, 16<=Y<=235, -113<=U<=112, -113<=V<=112
	seDataType_YUV_Offset,		///< YUV Offset, 0<=Y<=255, 0<=U<=255, 0<=V<=255
	seDataType_YCbCr_Offset,	///< YCbCr Offset, 16<=Y<=235, 16<=U<=240, 16<=V<=240
	seDataType_Invalid			///< Invalid data type
} seYrcData;


typedef enum
{
	seWindow_Main,
	seWindow_PIP,
	seWindow_Invalid
} seWindowData;


#define seVMEM_SIZE		0x60000	///< Total video memory size


#ifdef __cplusplus
   extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Sets the Main rotation.
///
///	Call this function to set the rotation for the Main layer.
///
/// \param	RotationDegrees	The Main layer counter-clockwise rotation in degrees.
///			RotationDegrees must be one of 0, 90, 180, or 270.
///
/// \return	Nothing
///
///	\see
///		seLcdGetRotation()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetRotation( UInt32 RotationDegrees );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the current Main layer rotation.
///
/// This function returns the current Main layer rotation angle in degrees.
///
/// \param	None
///
/// \return	Main rotation in degrees counter-clockwise. The return value
///			will be one of  0, 90, 180, or 270.
///
///	\see
///		seLcdSetRotation()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 seLcdGetRotation( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the Main layer color depth.
///
///	This routine sets the color depth for the Main layer.
///
/// \param	BitsPerPixel	The desired color depth.
///							seFormat_RGB_888
///							seFormat_RGB_565
///							seFormat_RGB_332_MONO
///							seFormat_RGB_888LUT
///							seFormat_RGB_565LUT
///							seFormat_RGB_332LUT
///
/// \return	Nothing
///
///	\see
///		seLcdGetColorDepth()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetColorDepth( seFormatData Data );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the Main layer color depth.
///
///	This function returns the Main layer color depth.
///
/// \param	None
///
/// \return
///		The color depth as one of
///							seFormat_RGB_888
///							seFormat_RGB_565
///							seFormat_RGB_332_MONO
///							seFormat_RGB_888LUT
///							seFormat_RGB_565LUT
///							seFormat_RGB_332LUT
///
/// \see
///		seLcdSetColorDepth()
///
////////////////////////////////////////////////////////////////////////////////
seFormatData seLcdGetColorDepth( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the Main layer color depth in bytes per pixel.
///
///	This function returns the Main layer color depth in bytes per pixel.
///
/// \param	None
///
/// \return
///		The Main layer color depth in bytes per pixel.
///
/// \see
///		seLcdGetColorDepth()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 seLcdGetBytesPerPixel( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Set the Main layer Start Address.
///
///	This functions sets the Start Address for the Main layer.
///
///	The Start Address is the offset into display memory to the start of
/// the Main layer data area.
///
/// \param	StartAddress	Offset, in bytes, into display memory to the 
///							beginning of Main layer.
///
/// \return	Nothing
///
///	\see
///		seLcdGetStartAddress()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetStartAddress( UInt32 StartAddress );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the Main layer Start Address.
///
/// This function returns the Main layer Start Address. Start Address refers to
/// the offset in display memory to the beginning of Main layer memory.
///
/// \param	None
///
/// \return	The Main layer Start Address.
///
///	\see
///		seLcdSetStartAddress()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 seLcdGetStartAddress( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the width, in pixels, for the Main layer.
///
///	Call this function to set the width of the Main layer, in pixels.
///
/// \param	Width	New Main layer width, in pixels.
///
/// \return	Nothing
///
///	\see
///		seLcdGetWidth()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetWidth( UInt32 Width );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Returns the width of the Main layer in pixels.
///
///	This function returns the width of the Main layer, in pixels.
///
/// \param	None
///
/// \return	The width of the Main layer in pixels.
///
///	\see
///		seLcdSetWidth()	\n
///		seLcdGetHeight()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 seLcdGetWidth( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the height, in pixels, for the Main layer.
///
///	Call this function to set the height of the Main layer, in pixels.
///
/// \param	Height	New Main layer height, in pixels.
///
/// \return	Nothing
///
///	\see
///		seLcdGetHeight()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetHeight( UInt32 Height );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Returns the height of the Main layer in pixels.
///
///	This function returns the height of the Main layer, in pixels.
///
/// \param	None
///
/// \return	The height of the Main layer in pixels.
///
///	\see
///		seLcdSetHeight() \n
///		seLcdGetWidth()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 seLcdGetHeight( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Returns the Main layer stride.
///
///	This function returns the Main layer stride, in bytes.
///
///	Stride is the number of bytes that must be added to the address of a
///	pixel in display memory to obtain the address of the pixel directly
///	below it. This value is also occasionally referred to as the line
///	address offset.
///
/// \param	None
///
/// \return	The Main layer stride in bytes.
///
///	\see
///		seLcdSetStride()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 seLcdGetStride( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Read one RGB entry of LUT data.
///
/// This function reads one entry of LUT data from the selected LUT.
///
/// \param Index				The LUT index to read the LUT data.
///
/// \param Window				Selects which LUT to read: \n
/// \a seWindow_Main			Access the Main Window LUT. \n
/// \a seWindow_PIP				Access the PIP Window LUT. \n
///
/// \return
/// The RGB data read from the LUT.
///
/// \see
/// seLcdSetLut(), seLcdSetLutEntry()
///
////////////////////////////////////////////////////////////////////////////////
seRgbData seLcdGetLutEntry( UInt32 Index, seWindowData Window );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Read LUT contents.
///
/// This function reads several entries of LUT data from the selected LUT.
///
/// \param StartIndex			The starting LUT index to begin reading LUT data.
/// \param pRgbData				The buffer to store LUT contents.
/// \param Count				The number of LUT entries to read.
///
/// \param Window				Selects which LUT to set: \n
/// \a seWindow_Main			Access the Main Window LUT. \n
/// \a seWindow_PIP				Access the PIP Window LUT. \n
///
/// \return
/// Nothing.
///
/// \see
/// seLcdSetLut(), seLcdGetLutEntry()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdGetLut( UInt32 StartIndex, seRgbData *pRgbData, UInt32 Count, seWindowData Window );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Write LUT data to LUT.
///
/// This function writes a LUT.
///
/// \param StartIndex			The starting LUT index to begin writing LUT data.
/// \param pRgbData				The data to write to the LUT.
/// \param Count				The number of LUT entries in pRgbData.
///
/// \param Window				Selects which LUT to set: \n
/// \a seWindow_Main			Access the Main Window LUT. \n
/// \a seWindow_PIP				Access the PIP Window LUT. \n
///
/// \return
/// Nothing.
///
/// \see
/// seLcdSetLutEntry(), seLcdGetLutEntry()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetLut( UInt32 StartIndex, const seRgbData *pRgbData, UInt32 Count, seWindowData Window );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Writes default data to LUT.
///
/// This function writes a default configuration to the LUT.
///
/// \param Window				Selects which LUT to set: \n
/// \a seWindow_Main			Access the Main Window LUT. \n
/// \a seWindow_PIP				Access the PIP Window LUT. \n
///
/// \return
/// Nothing.
///
/// \see
/// seLcdSetLut()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetLutDefault( seWindowData Window );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Write one RGB entry of LUT data to LUT.
///
/// This function writes one entry of LUT data to the selected LUT.
///
/// \param Index				The LUT index to write the LUT data.
/// \param pRgbData				The data to write to the LUT.
///
/// \param Window				Selects which LUT to set: \n
/// \a seWindow_Main			Access the Main Window LUT. \n
/// \a seWindow_PIP				Access the PIP Window LUT. \n
///
/// \return
/// Nothing.
///
/// \see
/// seLcdSetLut(), seLcdGetLutEntry()
///
////////////////////////////////////////////////////////////////////////////////
void seLcdSetLutEntry( UInt32 Index, const seRgbData *pRgbData, seWindowData Window );


#ifdef __cplusplus
   }
#endif



#endif //seDISPLAY_H_INCLUDED
