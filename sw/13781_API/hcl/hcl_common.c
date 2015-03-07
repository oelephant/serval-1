//=============================================================================
//	HCL_COMMON.C
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//=============================================================================

#include "hcl.h"



//---------------------------------------------------------------------------
// PRIVATE HCL_xxx FUNCTION: seHclInitRegisters()
//---------------------------------------------------------------------------
void seHclInitRegisters( void )
{
	int 		i;
	UInt32		Index;

	for ( i = 0; segHclInfo.Regs[i].Index != REGFLAG_END_OF_TABLE; i++ )
	{
		Index = segHclInfo.Regs[i].Index;

		if ( Index < REGFLAG_BASE )
		{
			seWriteReg16( Index, segHclInfo.Regs[i].Value );
		}
		else if ( Index == REGFLAG_OFF_DELAY || Index == REGFLAG_ON_DELAY )
		{
			seOsDelayUS( segHclInfo.Regs[i].Value ); 	// us delay.
		}
		else if ( Index == REGFLAG_DELAY )
		{
			if ( !(seReadReg16(REG10_PLL_0) & 0x0002) ) // don't poll PLL Lock status bit if PLL Bypass is enabled
				while( !(seReadReg16(REG10_PLL_0) & 0x8000) )
					continue;
		}
	}
}
