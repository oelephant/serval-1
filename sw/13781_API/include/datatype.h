//////////////////////////////////////////////////////////////////////////////
///
///  \file DATATYPE.H
///
/// \par	 COMMON DATA TYPES
///
///	As much as possible, programs should use natural data types (eg. int) to
///	allow compiler optimizations to kick in.  When specific data widths are
///	required, then the following types should be used.  Note that on some
///	platforms, an int is ONLY 16-BITS, therefore, int must be used carefully,
///	and that all int expresions will not be promoted to longs.
///
//  (C)SEIKO EPSON CORPORATION 2006. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////////

//	Please do not declare HCL-specific datatypes here, put them in hcl.h!

#ifndef seDATATYPE_H_INCLUDED
#define seDATATYPE_H_INCLUDED

typedef unsigned char       UInt8;			///<  8-Bit Unsigned Integer 
typedef unsigned short      UInt16;			///< 16-Bit Unsigned Integer 
typedef unsigned long       UInt32;			///< 32-Bit Unsigned Integer 
typedef unsigned int		UInt;

typedef signed char         Int8;			///<  8-Bit Signed Integer 
typedef signed short        Int16;			///< 16-Bit Signed Integer 
typedef signed long         Int32;			///< 32-Bit Signed Integer 
typedef signed int			Int;

// If using C++, use bool (true/false).
typedef int					Boolean;		///< Boolean uses int as standard C doesn't have bool.		

typedef void*	            Handle;


#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL				0
#else
#define NULL				((void*)0)
#endif
#endif

#ifndef UINT32_MAX
#define UINT32_MAX          0xFFFFFFFF
#endif

#ifndef RGB32
typedef struct tagRGBQ
{	UInt8 rgbBlue;
	UInt8 rgbGreen;
	UInt8 rgbRed;
	UInt8 rgbReserved;
} RGBQ;

typedef union tagRGB32
{
	RGBQ rgbq;
	UInt32 rgb32;
} RGB32;
#endif

#endif // seDATATYPE_H_INCLUDED
