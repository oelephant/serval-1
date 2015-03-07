////////////////////////////////////////////////////////////////////////////////
///
/// \file PIP.H
///
/// \par PIP Overview
///	The 13781 supports two types of display layers; Main and PIP.
///	\par
///	The Main layer represents the background image, which fills the entire 
///	display. The PIP layer is closely linked to the Main layer, and shares 
///	the same bits-per-pixel, mirror, and rotation features.
///	\par
///	The functions in this section deal with controlling the PIP features.
///
/// \par PIP Layer Functions
///		The PIP API is designed to work with the DISPLAY functions. In fact
///		several PIP functions call main windows display functions to
///		update the ViewPort.
///	\par
///		The PIP Window Display Functions define the size, stride, position
///		and orientation of the PIP Window.
///
//	(c) SEIKO EPSON CORPORATION 2009. All Rights Reserved. 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef sePIP_H_INCLUDED
#define sePIP_H_INCLUDED

#include "datatype.h"

#ifdef __cplusplus
   extern "C" {
#endif

typedef enum
{
	sePipDisabled,
	sePipNormal,
	sePipBlink1,
	sePipBlink2,
	sePipFadeOut,
	sePipFadeIn,
	sePipContinous
} sePipEnableMode;


typedef enum
{
	sePip2AlphaDisabled,
	sePip2ConstantAlphaBlending,
	sePip2BlendingWithAlphaMap,
	sePip2BlendingWithConstantAndAlphaMap
} sePip2AlphaModeData;



//=============================================================================
// PIP
//=============================================================================

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Controls the visibility state of the PIP
///
/// This function controls the visibility and display mode of the PIP when
///	it is enabled.
///
/// \param Mode	Set to one of the following:
///		- sePipBlank	Stops the PIP from being displayed immediately
///		- sePipNormal	Causes the PIP to be displayed immediately. The PIP will be 
///						displayed with the currently set alpha blending mode. If the 
///						alpha blending ratio is changed while the PIP is displayed the 
///						effect will take place on the next frame.
///		- sePipBlink1	The PIP layer toggles between the set alpha blend mode and no PIP
///		- sePipBlink2	The PIP layer toggles between normal and invert. Alpha blend ration remains constant.
///		- sePipFadeOut	Causes the PIP to fade from the current alpha blend value to 0x0000 (blank)
///		- sePipFadeIn	Causes the PIP layer to fade from 0x0000 to the set alpha blend ratio
///		-sePipInOut		Cycles between alpha blend 0x000 and the current set alpha blend value.
///
/// \return	Nothing
///
/// \see sePipGetDisplayMode()
///	\see sePipSetAlphaBlend()
///	\see sePipSetFlashRate()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetDisplayMode( int Mode );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the current PIP display mode
///
///	This routine returns the current PIP display mode. The return value will
///	be one of:
///		- sePipBlank	The PIP is blanked and only the main layer is visible
///		- sePipNormal	The PIP is being displayed using the current PIP alpha blend setting
///		- sePipBlink1	The PIP is toggling between the set alpha blend mode and no PIP
///		- sePipBlink2	The PIP is toggling between normal and invert.
///		- sePipFadeOut	
///		- sePipFadeIn	
///		- sePipInOut	The PIP is cycling between alpha blend 0x000 and the current PIP alpha blend value.
///
/// \param	None
///
/// \return
///
/// \see sePipSetDisplayMode()
///
////////////////////////////////////////////////////////////////////////////////
int sePipGetDisplayMode( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Sets the PIP rotation.
///
///	Call this function to set the rotation for the PIP.
///
/// \param	RotationDegrees	The PIP counter-clockwise rotation in degrees.
///			RotationDegrees must be one of 0, 90, 180, or 270.
///
/// \return	Nothing
///
///	\see sePipGetRotation()
///	\see sePipIsOrthagonal()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetRotation( UInt32 RotationDegrees );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the current PIP layer rotation.
///
/// This function returns the current PIP rotation angle in degrees.
///
/// \param	None
///
/// \return	PIP rotation in degrees counter-clockwise. The return value
///			will be one of  0, 90, 180, or 270.
///
///	\see sePipSetRotation()
///	\see sePipIsOrthagonal()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetRotation( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Determines if the PIP window has the same rotation as the main window.
///
///	This function determines if the PIP window is orthogonal (has the same
///	rotation) to the main window.
///
///	\param	None
///
///	\return	TRUE if both the PIP and the Main layers have the same angular rotation
///			FALSE if they layers use different rotation
///
/// \see
///	\see sePipSetRotation()
///	\see seMainSetRotation()
///
////////////////////////////////////////////////////////////////////////////////
Boolean sePipIsOrthogonal( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the PIP color depth.
///
///	This routine sets the color depth for the PIP. Refer to BitsPerPixel for
///	valid arguments.
///
/// \param	BitsPerPixel	The desired color depth in bits-per-pixel, which 
///							should be one of:
///							- seFormat_RGB_888
///							- seFormat_RGB_565
///							- seFormat_RGB_332_MONO
///							- seFormat_RGB_888LUT
///							- seFormat_RGB_565LUT
///							- seFormat_RGB_332LUT
///
/// \return	Nothing
///
///	\see sePipGetColorDepth()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetColorDepth( UInt32 BitsPerPixel );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the PIP color depth in bits per pixel.
///
///	This function returns the PIP color depth. 
///
/// \param	None
///
/// \return	The color depth as one of:
///			- seFormat_RGB_888
///			- seFormat_RGB_565
///			- seFormat_RGB_332_MONO
///			- seFormat_RGB_888LUT
///			- seFormat_RGB_565LUT
///			- seFormat_RGB_332LUT
///
/// \see sePipSetColorDepth()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetColorDepth( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the PIP layer color depth in bytes per pixel.
///
///	This function returns the PIP layer color depth in bytes per pixel.
///
/// \param	None
///
/// \return	The PIP layer color depth in bytes per pixel.
///
/// \see sePipGetColorDepth()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetBytesPerPixel( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Set the PIP Start Address.
///
///	This functions sets the Start Address for the PIP window.
///
///	The Start Address is the offset into display memory to the start of
/// the PIP data area.
///
/// \param	StartAddress	Offset, in bytes, into display memory to the 
///							beginning of PIP layer.
///
/// \return	Nothing
///
///	\see sePipGetStartAddress()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetStartAddress( UInt32 StartAddress );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the PIP Start Address.
///
/// This function returns the PIP Start Address. Start Address refers to
/// the offset in display memory to the beginning of PIP layer memory.
///
/// \param	None
///
/// \return	The PIP Start Address.
///
///	\see sePipSetStartAddress()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetStartAddress( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the width, in pixels, for the PIP layer.
///
///	Call this function to set the width of the PIP layer, in pixels.
///
/// \param	Width	New PIP width, in pixels.
///
/// \return	Nothing
///
///	\see sePipGetWidth()
///	\see sePipSetupWindow()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetWidth( UInt32 Width );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Returns the width of the PIP layer in pixels.
///
///	This function returns the width of the PIP layer, in pixels.
///
/// \param	None
///
/// \return	The width of the PIP in pixels.
///
///	\see sePipSetWidth()
///	\see sePipGetHeight()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetWidth( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the height, in pixels, for the PIP layer.
///
///	Call this function to set the height of the PIP layer, in pixels.
///
/// \param	Height	New PIP height, in pixels.
///
/// \return	Nothing
///
///	\see sePipGetHeight()
///	\see sePipSetupWindow()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetHeight( UInt32 Height );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Returns the height of the PIP layer in pixels.
///
///	This function returns the height of the PIP layer, in pixels.
///
/// \param	None
///
/// \return	The height of the PIP in pixels.
///
///	\see sePipSetHieght()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetHeight( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Sets the PIP X,Y coordinates.
///
/// This functions sets the position of the upper left corner of the PIP
/// layer relative to the upper left corner of the pane origin.
///
/// The API code is written to be as lean and flexible as possible.
/// To permit the greatest flexibility there are few checks placed
/// in the code. Consequently, it is the responsibility of the
/// application to ensure that PIP coordinates are set within the 
/// panel display area.
///
/// \note	Currently the main layer rotation is not taken into account.
///			The PIP positioning is always relative to the top left corner
///			of the panel.
///
/// \param	x		New X coordinate for the PIP window.
///	\param	y		New Y coordinate for the PIP window.
///
/// \return	Nothing
///
///	\see sePipGetPosition()
///	\see sePipSetupWindow()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetPosition( UInt32 x, UInt32 y );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Retrieves the X,Y coordinates of the PIP window.
///
///	This function retrieves the X,Y coordinates of the PIP window with
///	respect to the panel origin
///
///	Rotation is taken into account when the coordinates are calculated. 
/// The returned coordinates will always be relative to the "upper left" 
/// corner of the panel origin.
///
/// \param	px	Pointer to a 32-bit integer to receive the current X coordinate.
///	\param	py	Pointer to a 32-bit integer to receive the current Y coordinate.
///
/// \return	Nothing
///
/// \see sePipSetPosition()
///
////////////////////////////////////////////////////////////////////////////////
void sePipGetPosition( UInt32 *px, UInt32 *py );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Set the PIP layer fade rate.
///
/// When the PIP layer is set to use a Fade In, Fade Out, or continuous Fade 
/// In/Out, the FadeRate value is used to determine the number of frames to
/// pause before setting the Alpha blend ratio to the next step.
///
/// \param	FadeRate	The number of frames to wait between automatic Alpha
///						blend ratio steps. Permissable values range from 1 to 64.
///
/// \return	Nothing
///
/// \see sePipgetFadeRate()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetFadeRate( UInt32 FadeRate );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Retrieves the PIP layer fade rate.
///
/// When the PIP layer is set to use a Fade In, Fade Out, or continuous Fade 
/// In/Out, the FadeRate value determines the number of frames to pause before 
/// each change to the Alpha blend ratio.
///
/// Call this function to retrieve the current fade rate in frames.
///
/// \return	Fade rate in frames
///
/// \see sePipSetFadeRate()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetFadeRate( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Waits for the current PIP layer fade action to complete
///
/// When the PIP layer is set to use a Fade In, Fade Out, or continuous Fade 
/// In/Out, the FadeRate value determines the number of frames to pause before 
/// each change to the Alpha blend ratio.
///
/// If continual fade In/Out is selected the fade will never complete and this
/// routine will time out at the specified maximum time.
///
/// The maximum time that can pass is 0x7F * 0x7F = 0x3F01 frames.
/// Assuming about 60 fps this equates to 0xA80 seconds (2688 seconds = 44.8 minutes)
///
/// \param	MaxTime	The maximum number of milliseconds to wait before returning
///
/// \return
///			- TRUE	when the Alpha fade has completed.
///			- FALSE	if the routine timed out waiting for the fade to complete.
///
/// \see sePipSetFadeRate()
///
////////////////////////////////////////////////////////////////////////////////
Boolean sePipWaitForFade( UInt32 MaxTime );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief 
///
/// When the PIP layer is set to use a Fade In, Fade Out, or continuous Fade 
/// In/Out, the FadeRate value determines the number of frames to pause before 
/// each change to the Alpha blend ratio.
///
/// Call this function to set the fade rate
///
/// \param Step	The number of steps to change the Alpha blend ratio.
///			Acceptable values are 1, 2, and 4.
///
/// \return	Nothing
///
/// \see sePipSetFadeRate()
///	\see sePipGetAlphaBlendStep()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetAlphaBlendStep( UInt32 Step );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Retrieves the PIP layer fade rate.
///
/// When the PIP layer is set to use a Fade In, Fade Out, or continuous Fade 
/// In/Out, the FadeRate value determines the number of frames to pause before 
/// each change to the Alpha blend ratio.
///
/// Call this function to retrieve the current fade rate in frames.
///
/// \return	PIP layer Alpha fade rate. The value will be one of 1, 2, or 4.
///
/// \see sePipSetAlphaBlendStep()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetAlphaBlendStep( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Set the Alpha blend (transparency) of the PIP layer.
///
/// The PIP layer is capable of alpha blending itself with the main layer
/// image. This function sets the Alpha blend ratio for the PIP layer.
///
///	The 13781 hardware supports 64 levels of blending however humans are
/// accustomed to thinking in %. Consequently, this routine accepts a %
/// value and converts to the nears 1/64th. 
///
/// \param Ratio	% of the PIP layer Alpha, ranging from 0% (no PIP visible)
///					to 100% (full PIP visibility)
///
/// \return	Nothing
///
/// \see sePipGetAlphaBlendRatio()
///	\see sePipSetFadeRate()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetAlphaBlendRatio( UInt32 Ratio );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Retrieves the current PIP layer alpha blend ratio.
///
/// The PIP layer is capable of alpha blending itself with the main layer
/// image. This function retrieves the Alpha blend ratio for the PIP layer.
///
///	The 13781 hardware supports 64 levels of blending however humans are
/// accustomed to thinking in %. Consequently, this routine reads the PIP
///	Alpha blend value and converts to the nears %.
///
/// \param	None
///
/// \return	The PIP layer alpha blend ratio to the nearest %.
///
/// \see sePipSetAlphaBlendRatio()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetAlphaBlendRatio( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Enable/disable PIP layer transparency.
///
///	Call this routine to enable or disable PIP window transparency.
///
/// \param	fEnable	Set to TRUE to enable transparency or set to FALSE to disable
///			transparency.
///
/// \return	Nothing
///
/// \see sePipGetTransparency()
///	\see sePipSetTransColor()
///
////////////////////////////////////////////////////////////////////////////////
void sePipEnableTranparency( Boolean fEnable );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Retrieves the current state of Pip layer transparency.
///
/// \param	None
///
/// \return	- TRUE if PIP layer transparency is enabled
///			- FALSE if PIP layer transparency is disabled.
///
/// \see sePipEnableTranparency()
///
////////////////////////////////////////////////////////////////////////////////
Boolean sePipGetTransparency( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Sets the PIP layer transparent color.
///
/// Call this function to set the PIP layer transparent color. When PIP layer
/// transparency is enabled, any pixels in the PIP layer that match the transparent
/// color cause the the Main layer pixels to be displayed.
///
/// \param Color	An RGB32 value of the color to be used as the transparent key color.
///	\param
///
/// \return	Nothing
///
/// \see sePipGetTransColor
///	\see sePipEnableTranparency
////////////////////////////////////////////////////////////////////////////////
void sePipSetTransColor( RGB32 Color );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Retrieves the current PIP transparency key color.
///
///	Call this function to retrieve the current PIP layer transparent key color.
///
///	\param	pColor	Pointer to a 32-bit value to receive the current key color.
///
/// \return	Nothing
///
/// \see sePipSetTransColor()
///
////////////////////////////////////////////////////////////////////////////////
void sePipGetTransColor( RGB32 * pClr );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Set most PIP parameters with one function call.
///
///	This function permits setting PIP X/Y coordinates, width, height and
///	stride with one function call. Use this function to initially setup
///	PIP or any time several parameters need to be simultaneously updated.
///
/// \param	x		New X position, in pixels, for PIP relative to the upper
///					left corner of the main window.
///
/// \param	y		New Y position, in pixels, for PIP relative to the upper
///					left corner of the main window.
///
/// \param	Width	New width, in pixels, for the PIP window.
///
/// \param	Height	New height, in pixels, for the PIP window.
///
/// \return	Nothing
///
///	\see sePipSetPosition()
///	\see sePipSetWidth()
/// \see sePipSetHeight()
///
////////////////////////////////////////////////////////////////////////////////
void sePipSetupWindow( UInt32 x, UInt32 y, UInt32 Width, UInt32 Height );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	Returns the PIP stride.
///
///	This function returns the PIP window stride, in bytes.
///
///	Stride is the number of bytes that must be added to the address of a
///	pixel in display memory to obtain the address of the pixel directly
///	below it. This value is also occasionally referred to as the line
///	address offset.
///
/// \param	None
///
/// \return	The PIP window stride in bytes.
///
///	\see	sePipSetStride()
///
////////////////////////////////////////////////////////////////////////////////
UInt32 sePipGetStride( void );


#ifdef __cplusplus
   }
#endif



#endif // sePIP_H_INCLUDED

