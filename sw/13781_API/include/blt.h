////////////////////////////////////////////////////////////////////////////////
///
/// \file BLT.h
///
/// \par BLT OVERVIEW
/// The functions in the BLT module provide an interface to the S1D13781 2D
/// BitBLT engine.
///	\par
/// BitBLT is an abbreviation for BIT BLock Transfer and BitBLT's
///	are used to rapidly move data from one location to another.
///	During the process of transfering pixels a variety of logical
///	operations can be applied to the pixel.
///
/// \par BitBLT Operations
/// The S1D13781 2D engine provides three basic BitBLT operations.
/// - Move			\par
///					Move BitBLTs transfer data from one location in display memory
///					to a second location in display memory.
///					\par
///					Move BitBLTs can be performed in the positive or negative direction.
///					The software API automatically selects the proper direction depending
///					on the BitBLT parameters. Typically, negative Move BitBLTs are used
///					when there is overlap between source and destination data, to prevent
///					the source data from being overwritten before it is transferred.
/// - Solid Fill	\par
///					Solid Fill BitBLTs fill an area of display memory with a solid color.
///					\par
///					Solid Fills are useful for rapidly clearing the display to a fixed
///					value or for filling a rectangular area with a single color.
/// - Color Expand	\par
///					A special case of Move BitBLT, Color Expand is typically used to
///					increase the speed of text drawing operations.
///					\par
///					Color Expansion functions by examining each 1 bit of source data
///					and uses the Foreground and Background color to expand this bit
///					to the BitBLT color depth.
///					\par
/// For further information, refer to the 13781 Hardware Functional Specification
///
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved
//
////////////////////////////////////////////////////////////////////////////////

#ifndef seBLT_H_INCLUDED
#define seBLT_H_INCLUDED

#include "datatype.h"

#ifdef __cplusplus
	extern "C" {
#endif


//BitBLT Operations
#define seBLT_OPERATION_MOVE_POS		0x0000
#define seBLT_OPERATION_MOVE_NEG		0x0001
#define seBLT_OPERATION_SOLID_FILL		0x0002
#define seBLT_OPERATION_MOVE_COLOREXP	0x0004
#define seMAX_BLT_OPS					4

#define seBLT_MIN_X_POS				  (-1023)
#define seBLT_MAX_X_POS					1023
#define seBLT_MIN_Y_POS				  (-1023)
#define seBLT_MAX_Y_POS					1023

#define seBLT_MIN_OFFSET				   1
#define seBLT_MAX_OFFSET				1023

#define seBLT_MIN_WIDTH					   1
#define seBLT_MAX_WIDTH					1023

#define seBLT_MIN_HEIGHT				   1
#define seBLT_MAX_HEIGHT				1023

#define seBLT_SRC					0
#define seBLT_DST					1

#define seBLT_COMPLETE_TIMEOUT		1000

////////////////////////////////////////////////////////////////////////////////
/// \brief The seBltData structure is the control interface for  BitBLT 
///		   operations, and consists of the following
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	UInt32	Operation;			///< Operation: Write, Read, Move, SolidFill, PatternFill

	UInt32	ColorDepth;			///< BitBLT color depth in bits-per-pixel (8,16,24)
	UInt32	Width;				///< BitBLT Width in pixels.
	UInt32	Height;				///< BitBLT Height in lines.
	UInt32	MemoryOffset;		///< Stride, in bytes, of the BitBLT destination area.
	UInt32	MemoryBpp;			///< Memory color depth in bits-per-pixel (8,16,24). Could be different from BitBLT color depth.

	UInt32	SrcBaseAddress;		///< Offset into display memory to the start of source space
	int		SrcXposition;		///< Source X position relative to base address.
	int		SrcYposition;		///< Source Y position relative to base address.
	Boolean fSrcLinear;			///< TRUE:Linear, FALSE:rectangular.

	UInt32	DstBaseAddress;		///< Offset into display memory to the start of destination space
	int		DstXposition;		///< Destination X position relative to base address.
	int		DstYposition;		///< Destination Y position relative to base address.
	Boolean fDstLinear;			///< TRUE-Linear, FALSE-rectangular.

	UInt32	BackGrndColor;		///< Background color.
	UInt32	ForeGrndColor;		///< Foreground color.
} seBltData;



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Initializes the BitBLT API for use.
///
///	This function prepares the BitBLT API for use by taking any necessary steps 
///	to ensure the BitBLT engine is ready.
///
/// \param  None
///
/// \return Nothing
///
///	\see seBltApiShutdown
///
////////////////////////////////////////////////////////////////////////////////
void seBltApiInitialize( void );



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Terminates the 2D BitBLT API
///
///		This function is used to terminate the use of the BitBLT API.
///		This function should be called as part of the API shutdown sequence.
/// 
/// \param  None
///
/// \return Nothing
///
/// \see seBltApiInitialize
///
////////////////////////////////////////////////////////////////////////////////
void seBltApiShutdown( void );


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Primary BitBLT interface
///
///		This function is the primary entry point for any BitBLT operation.
///		Applications should call seBlBitBlt() to initiate a BitBLT.
///
///	\param pBltInfo
///			Pointer to a BltInfo structure containing structure elements
///			describing the desired BitBLT operation.
///
///	\return
///		- TRUE if the operation is successful.
///		- FALSE if there was a problem with the BitBLT operation.
///
///	\see seBltData
////////////////////////////////////////////////////////////////////////////////
Boolean seBltBitBlt( seBltData * pBltInfo );


#ifdef __cplusplus
	}
#endif

#endif // seBLT_H_INCLUDED