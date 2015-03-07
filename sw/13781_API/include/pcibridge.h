//////////////////////////////////////////////////////////////////////////////
///
///	\file	PCIBRIDGE.H
///
/// \par	PCI Interface
/// This module provide an interface to the B00B (PCI) and B00C (PC Card) evaluation
/// boards. These functions will need to be rewritten or removed when porting
/// the API to new hardware.
///
//  (C)SEIKO EPSON CORPORATION 2006. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef sePCIBRIDGE_H_INCLUDED
#define sePCIBRIDGE_H_INCLUDED


#include "datatype.h"  					// Grab public definitions and data types.



typedef void (*PISR_CALLBACK)(void);	// Define type for pointer to ISR callback function



// Allow both standard C and C++ linkages.
#ifdef __cplusplus
   extern "C" {
#endif

//-----------------------------------------------------------------------------
//	Public Functions
//-----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Used by the HCL to map MGE physical address to application address.
///
///	This function is used by the HCL to map the MGE hardware from a physical
///	address to a virtual address.
///
///	This function may not be require on all platforms.  When the MGE hardware 
///	is on a PCI bus, or PC-CARD bus, then this function is normally called with 
///	PhysicalAddr set to ZERO (0), and this function is responsible for locating 
///	and virtualizing the MGE hardware.  When the MGE hardware is located at a 
///	fixed location in the system's memory map, then PhysicalAddr is set to the 
///	physical offset of the MGE hardware, and under normal circumstances, this 
///	function will return the value of PhysicalAddr in pVirtualAddr, effectively
///	just echoing the physical address as a virtual address is not required. Once
///	this function returns, the HCL uses the returned pVirtualAddr for all subsequent
///	access to the MGE hardware.
///
///	NOTE
///	\n	It is recommended to call seOsMapPhysicalToVirtual() instead of
///		pcibMapAddress() in order to guarantee that system resources are properly
///		released to the Operating System when the application exits.
///
///	\param	PhysicalAddr		Physical address offset base of the MGE hardware.
///
///	\param	pVirtualAddr		Returned virtual address of the base of the MGE hardware.
///
///	\param	pBlockSize			Returned size of the virtual block of memory, in bytes.
///
///	\return				
///			- TRUE if mapping successful
///			- FALSE if there was a problem
///
/// \see	pcibMapAddressExtended()
///
//////////////////////////////////////////////////////////////////////////////
Boolean pcibMapAddress( UInt32 PhysicalAddr, UInt32 *pVirtualAddr, UInt32 *pBlockSize );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Used by the HCL to map MGE physical address to application address.
///
///	This function is used by the HCL to map the MGE hardware from a physical
///	address to a virtual address.
///
///	This function may not be require on all platforms.  When the MGE hardware 
///	is on a PCI bus, or PC-CARD bus, then this function is normally called with 
///	PhysicalAddr set to ZERO (0), and this function is responsible for locating 
///	and virtualizing the MGE hardware.  When the MGE hardware is located at a 
///	fixed location in the system's memory map, then PhysicalAddr is set to the 
///	physical offset of the MGE hardware, and under normal circumstances, this 
///	function will return the value of PhysicalAddr in pVirtualAddr, effectively
///	just echoing the physical address as a virtual address is not required. Once
///	this function returns, the HCL uses the returned pVirtualAddr for all subsequent
///	access to the MGE hardware.
///
/// This function, unlike pcibMapAddress(), also supports mapping devices with multiple
/// BARs (Base Address Register).
///
///	NOTE
///	\n	It is recommended to call seOsMapPhysicalToVirtual() instead of
///		pcibMapAddress() in order to guarantee that system resources are properly
///		released to the Operating System when the application exits.
///
///	\param	PhysicalAddr		Physical address offset base of the MGE hardware.
///
///	\param	pVirtualAddr		Returned virtual address of the base of the MGE hardware.
///
///	\param	pBlockSize			Returned size of the virtual block of memory, in bytes.
///
/// \param	mappednum			Selects which Base Address Register to map.
///
///	\return				
///			- TRUE if mapping successful
///			- FALSE if there is a problem encountered
///
/// \see pcibMapAddress()
///
//////////////////////////////////////////////////////////////////////////////
Boolean pcibMapAddressExtended( UInt32 PhysicalAddr, UInt32 *pVirtualAddr, UInt32 *pBlockSize, UInt32 mappednum);


//////////////////////////////////////////////////////////////////////////////
///
///	\brief Releases system resources allocated by pcibMapAddress().
///
///	This function releases any system resources allocated by pcibMapAddress().
///
///	\note It is recommended that applications call seOsMapPhysicalToVirtual()
/// so that this functions is called automatically when the application exits.
///
/// \param	None
///
///	\return	Nothing		
///
//////////////////////////////////////////////////////////////////////////////
void pcibUnmapAddress( void );


#ifdef __cplusplus
   }
#endif


#endif	// sePCIBRIDGE_H_INCLUDED

