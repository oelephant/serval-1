////////////////////////////////////////////////////////////////////////////////
///
/// \file CONSTANTS.H
///
/// \par Miscellaneous Constants
/// 
/// This module contains some miscellaneous hardware constants and
/// macro functions used by the API.
///
//  (C)SEIKO EPSON CORPORATION 2006. All rights reserved.
///	
////////////////////////////////////////////////////////////////////////////////

#ifndef seCONSTANTS_H_INCLUDED
#define seCONSTANTS_H_INCLUDED


/// Round up value to specified power-of-2 alignment
#define seROUNDUP(aln,val)			(((val)+(aln)-1) & ~((aln)-1))
/// Round down value to specified power-of-2 alignment
#define seROUNDDOWN(aln,val)		((val) & ~((aln)-1))


// Minimum dimension and resolution opposed by YCbCr subsampling
#define seMIN_HORIZ_444				1
#define seMIN_HORIZ_422				2
#define seMIN_HORIZ_420				2
#define seMIN_HORIZ_411				4
#define seMIN_VERTI_444				1
#define seMIN_VERTI_422				1
#define seMIN_VERTI_420				2
#define seMIN_VERTI_411				1
#define seRSL_HORIZ_444				1
#define seRSL_HORIZ_422				2
#define seRSL_HORIZ_420				2
#define seRSL_HORIZ_411				4
#define seRSL_VERTI_444				1
#define seRSL_VERTI_422				1
#define seRSL_VERTI_420				2
#define seRSL_VERTI_411				1

// Maximum, Minimum dimension and resolution to Resizer
#define seMAX_HORIZ_RESIZER			1280					///< Maximum Resizer Horizontal Dimension
#define seMAX_VERTI_RESIZER			1024					///< Maximum Resizer Vertical Dimension
#define seMIN_HORIZ_RESIZER			seMIN_HORIZ_444			///< Minimum Resizer Horizontal Dimension
#define seMIN_VERTI_RESIZER			seMIN_VERTI_444			///< Minimum Resizer Vertical Dimension
#define seRSL_HORIZ_RESIZER			seRSL_HORIZ_444			///< Minimum Resizer Horizontal Resolution
#define seRSL_VERTI_RESIZER			seRSL_VERTI_444			///< Minimum Resizer Vertical Resolution

// JPEG image max dimension
#define seMAX_HORIZ_JPEG			3840					///< Maximum JPEG Horizontal Dimension
#define seMAX_VERTI_JPEG			3840					///< Maximum JPEG Vertical Dimension

// Minimum dimension and resolution to JPEG Codec
#define seMIN_HORIZ_JPEG_444		8
#define seMIN_HORIZ_JPEG_422		16
#define seMIN_HORIZ_JPEG_420		16
#define seMIN_HORIZ_JPEG_411		32
#define seMIN_VERTI_JPEG_444		8
#define seMIN_VERTI_JPEG_422		8
#define seMIN_VERTI_JPEG_420		16
#define seMIN_VERTI_JPEG_411		8
#define seRSL_HORIZ_JPEG_444		1
#define seRSL_HORIZ_JPEG_422		2
#define seRSL_HORIZ_JPEG_420		2
#define seRSL_HORIZ_JPEG_411		4
#define seRSL_VERTI_JPEG_444		1
#define seRSL_VERTI_JPEG_422		1
#define seRSL_VERTI_JPEG_420		2
#define seRSL_VERTI_JPEG_411		1


// Maximum, Minimum dimension and resolution to Camera
#define seMAX_HORIZ_CAM			1280						///< Maximum Camera Horizontal Dimension
#define seMAX_VERTI_CAM			1024						///< Maximum Camera Vertical Dimension
#define seMIN_HORIZ_CAM			seMIN_HORIZ_422				///< Minimum Camera Horizontal Dimension
#define seMIN_VERTI_CAM			seMIN_VERTI_422				///< Minimum Camera Vertical Dimension
#define seRSL_HORIZ_CAM			seRSL_HORIZ_422				///< Minimum Camera Horizontal Resolution
#define seRSL_VERTI_CAM			seRSL_VERTI_422				///< Minimum Camera Vertical Resolution

// Maximum, Minimum dimension and resolution to RYV
#define seMAX_HORIZ_RYC			1024						///< Maximum RYV Horizontal Dimension
#define seMAX_VERTI_RYC			1024						///< Maximum RYV Vertical Dimension
#define seMIN_HORIZ_RYC			seMIN_HORIZ_422				///< Minimum RYV Horizontal Dimension
#define seMIN_VERTI_RYC			seMIN_VERTI_422				///< Minimum RYV Vertical Dimension
#define seRSL_HORIZ_RYC			seRSL_HORIZ_422				///< Minimum RYV Horizontal Resolution
#define seRSL_VERTI_RYC			seRSL_VERTI_422				///< Minimum RYV Vertical Resolution


#endif // seCONSTANTS_H_INCLUDED
