// APIMUTEX.H - S1D13781 API Project Revision History File
// (C)SEIKO EPSON CORPORATION 2009. All rights reserved.

#define	API_MUTEX_VERSION	"1.00"
#define	API_MUTEX_REVISION	"$Revision: 58 $"

// $Header: /13781/API/zzzMutex/apiMutex.h 58    14-03-03 2:49p Rocca $
// 
// $Log: /13781/API/zzzMutex/apiMutex.h $
// 
// 58    14-03-03 2:49p Rocca
// Removed camera code, which isn't supported by 781.
// 
// 57    5/03/13 11:56a Bobe
// - There was a problem reported with the LUT command for individual
// writes. The LUT command called seWritememory32() for individual writes
// and seBurstWriteMemory32() for writing the entire LUT.
// seWriteMemory32() did a full swap of the data bytes while
// seBurstWriteMemory32() copied data byte by byte. The technique used by
// the bust write method is the correct way to handle endian issues over a
// 16-bit bus. I made seWriteMemory32() simply call through to
// seBurstWriteMemory32().
// 
// 56    8/06/10 10:23a Bobe
// - Updated the Prometheus startup code to match the actual configuration
// of the customer evalution boards.
// 
// 55    9/25/09 5:53p Paul
// hcl_common.c
// - During initialization, don't poll PLL Lock status bit if PLL Bypass
// is enabled
// 
// 54    9/22/09 1:35p Paul
// initdata.c
// - Add Reg[D4h] in config initialization data. Requested by Dax.
// 
// 53    25/06/09 2:08p Bobe
// - Update .H file documentation in preparation for Beta API release
// - Removed routines from the API that reference functionality the 13781
// does not support.
// 
// 52    6/23/09 4:56p Paul
// api.sln
// - Restore testinit/testhcl/testdisplay projects in API solution build.
// testdisplay.c, testhcl.c
// - Update the sample program to work with 781 API.
// pip.c
// - Use 16-bit register access routines for sePipSetTransColor,
// sePipGetTransColor.
// 
// 51    4/23/09 11:05a Paul
// blt.c
// - Fix bug in IsOverlap(). If Src=Dst=Linear and SrcAddr>DstAddr, then
// it is not a destructive overlap as long as MoveBLT traverses in
// positive direction (which is the default direction).
// 
// 50    4/21/09 9:48a Paul
// api.vcproj, blt.c, blt.h
// 
// - Added BLT support to the API.
// display.c, display.h
// - Added Main Layer functions.
// - Code cleanup.
// pip.c, pip.h
// - Added seGetBytesPerPixel.
// - Some bugfixes.
// 
// 49    4/17/09 3:22p Paul
// registers.h
// - Updated to v0.41 of spec
// - Added BitBLT registers
// initdata.c
// - Modify host bus cycle times to fix problem in Direct 8-bit interface.
// Bug detected by JT, solution by Ricardo.
// 
// 48    4/16/09 10:53a Paul
// initdata.c
// Use shorter host bus cycle times for access to memory/LUT. Values taken
// from Ricardo's email.
// 
// 47    8/04/09 11:40a Bobe
// - Made changes to bring PIP alpha fades more inline with what the spec
// will become
// - Corrected a couple of bit handling bugs with alpha fades and step
// rates.
// 
// 46    4/06/09 11:19a Paul
// initdata.c
// Edit default PCLK Polarity (Reg[20]) value to match Sharp QVGA color
// LCD panel. This is required after CFG was modified to program PCLK
// Polarity based on the selected LCD panel. Requested by Dax.
// 
// 45    1/04/09 1:17p Bobe
// Slight tweaks to the PIP API
// 
// 44    27/03/09 1:57p Bobe
// - Added initial PIP support to the API. There are portions of the spec
// still marked TBD so I expect this code to undergo several revisions.
// 
// 43    3/25/09 4:09p Paul
// initdata.c
// - Fix clock M-Divider & L-Counter settings so that the displayed SYSCLK
// and CLKI values in CFG GUI will match actual FPGA clocks. Note: this
// does not have any effect on hardware since the PLL is not yet
// implemented.
// 
// 42    3/24/09 4:04p Paul
// registers.h
// - Updated to v0.35 of spec.
// 
// 41    3/13/09 12:24p Paul
// initdata.c
// Spec change: Except for clock registers which need to be in PSM0 in
// order to be written, all other registers can now be written in any
// PowerSaveMode. Therefore remove extra writes to PowerSaveMode Reg[04].
// 
// 40    3/12/09 6:21p Paul
// initdata.c
// - Edit default VPS (Reg[32]) value to match Sharp QVGA color LCD panel.
// New value is 17 lines (previously 16). Local panels.xml also modified.
// 
// 39    3/12/09 3:30p Paul
// - Initial 781 checkin. Ported from 13772 codebase. Work in progress.
// - Updated copyright year.
// - Updated to v0.34 of spec.
// 
