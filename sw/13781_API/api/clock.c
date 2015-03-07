//===========================================================================
//	CLOCK.C
//
//  (C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//===========================================================================


#include "hcl.h"
#include "clock.h"
#include "os.h"

//---------------------------------------------------------------------------
// seClkGetPLL() - Get the PLL Clock setting, returns a 0 if PLL disabled
// or returns the PLL setting in Hz
//---------------------------------------------------------------------------
UInt32 seClkGetPLL( void )
{
	UInt32 pllOutput;
	UInt32 pllMDiv;
	UInt32 pllLClk;

	pllMDiv = (seReadReg16(REG12_PLL_1)&0x03FF) + 1;
	pllLClk = seReadReg16(REG14_PLL_2) & 0x03FF;

	pllOutput = (pllLClk + 1) * (segHclInfo.ClkI / pllMDiv);

	return pllOutput;
}

//---------------------------------------------------------------------------
// seClkGetSysClk() - returns the system clock setting in Hz
//---------------------------------------------------------------------------
UInt32 seClkGetSysClk( void )
{
    return seClkGetPLL();
}