//=============================================================================
//	HCL.C
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//
// This file selects one and ONLY one active HCL interface based
// on the selected interface in the settings.h file.  The selected
// HCL_xxx interface file is included into this file as a single
// compilation unit.
//
//=============================================================================

#include "hcl.h"



//===========================================================================
// INCLUDE ONLY ONE INTERFACE-SPECIFIC HCL CORE MODULE
//===========================================================================

//#ifdef seHOST_INTERFACE_PCI_INDIRECT8
//	#include "hcl_pci_indirect8.c"
//#endif

#ifdef seHOST_INTERFACE_USB
	#include "hcl_usb.c"
#endif
