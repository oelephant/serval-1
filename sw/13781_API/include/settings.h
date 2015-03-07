//////////////////////////////////////////////////////////////////////////////
///
///	\file	SETTINGS.H
///
/// \par API Settings
/// This file determines which compile time options will be included in
/// the project build. Also included are definitions that may affect the
/// code compilation and which are not supported by other compilers.
///
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef seSETTINGS_H_INCLUDED
#define seSETTINGS_H_INCLUDED


// The following items are typically defined by the compiler.  Much of the
// API code makes use of these items. If the build environment does not 
// support these definitions, it may be necessary to enable them here.
//
// #define __cplusplus							// Defined to enable C++ linkage
// #define _DEBUG								// Defined to enable DEBUGGING code
// #define _WIN32								// Defined when compiled on a Windows platform



/// Select ONE and ONLY one of these definitions to compile the appropriate HCL
/// for the host interface type that is being used. The hcl.c file will include
/// the select interface-specific module, based on which definition is selected.
///
/// \verbatim
///
///     seHOST_INTERFACE_PCI_INDIRECT8			  - Host interface is parallel indirect.
///     seHOST_INTERFACE_USB					  - Host interface uses the USB adapter.
/// \endverbatim
#undef  seHOST_INTERFACE_PCI_INDIRECT8
#define seHOST_INTERFACE_USB

/// Defined if the platform supports threads. 
/// If this is not defined, the function seOsBeginThread() will always return FALSE.
#define seAPPLICATION_THREADS_SUPPORTED

#endif // seSETTINGS_H_INCLUDED

