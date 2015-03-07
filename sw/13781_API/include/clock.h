//////////////////////////////////////////////////////////////////////////////
///
/// \file CLOCK.H
///
/// \par CLOCK OVERVIEW
/// The Clock API library functions provide the ability to for programs to find 
/// out information about the internal clock frequencies of the MGE.  All 
/// internal clocks are configured using the configuration program cfg.exe
///	/par
/// For further information on the MGE internal clocks, refer to the MGE Hardware 
/// Functional Specification
///
//	(C)SEIKO EPSON CORPORATION 2006. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef seCLOCK_H_INCLUDED
#define seCLOCK_H_INCLUDED


#include "datatype.h"		// Grab public definitions and data types.


#ifdef __cplusplus
   extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	This function is used to get the PLL Clock setting based on register
///			settings and the CLKI frequency set by the configuration program.
///
/// \param	None
///
/// \return	The PLL setting in Hz, or 0 if the PLL is disabled.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seClkGetPLL( void );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	This function is used to get the System Clock setting based on register
///			settings and the CLKI frequency set by the configuration program.
///
/// \param	None
///
/// \return	The System Clock setting in Hz
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seClkGetSysClk( void );

#ifdef __cplusplus
   }
#endif


#endif //seCLOCK_H_INCLUDED
