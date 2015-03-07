//===========================================================================
//	PCIBRIDGE.C
//---------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2005. All rights reserved.
//===========================================================================

#include "pcibridge.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning( disable : 4201 )	// Disable "nameless struct/union" warning in winioctl.h.
#include <winioctl.h>
#include "s1d_ioctl.h"



#define MAX_MEMORYMAPS		6
static HANDLE		ghPCIDriver;			// Handle to PCI driver
static UInt32		gPCIAddress[MAX_MEMORYMAPS]	// Pointer to PCI memory
= { 0,0,0,0,0,0 };

static ISR_STRUCT	gISRInfo;				// Interrupt Service Routine info passed to PCI driver
static DWORD		ghISRId;				// Handle to driver-returned ISR ID (0-31)
static DWORD		gVendorId;				// PCI vendor ID
static DWORD		gProductId;				// PCI device ID

static PISR_CALLBACK gpISRCallback;			// Application supplied callback function
static Boolean		gfISRHooked;			// True if device driver has hooked ISR


static void		UnhookInterrupt( void );
static Boolean	DispatchPCIInterrupt( LPVOID hISREvent );
static void		GetPciID( void );
static void		EnablePciInterrupt( void );




//---------------------------------------------------------------------------
// PUBLIC FUNCTION: pcibMapAddress()
//---------------------------------------------------------------------------
Boolean pcibMapAddress( UInt32 PhysicalAddr, UInt32 *pVirtualAddr, UInt32 *pBlockSize )
{
	return pcibMapAddressExtended( PhysicalAddr, pVirtualAddr, pBlockSize, 0 );
}

Boolean pcibMapAddressExtended( UInt32 PhysicalAddr, UInt32 *pVirtualAddr, UInt32 *pBlockSize, UInt32 mappednum)
{
	BOOL	fRet = FALSE;
	DWORD	nReturned;
	UInt32	aRetPCI[3];

    // If computer has multiple processors or is hyperthreaded we must ensure that all
    // of this process's threads run on the same CPU. Otherwise a race condition related to 
    // the S1D13xxx driver will cause hardware interrupts to be missed occasionally. This is
    // particularly noticeable for BLT interrupts which can occur in rapid succession. 
    SetProcessAffinityMask(GetCurrentProcess(), 1);

	// Determine the operating system we are running under and act accordingly.

	if ( GetVersion() < 0x80000000 )	// Open the device using NT semantics.
		ghPCIDriver = CreateFile( "\\\\.\\S1D13xxx", GENERIC_READ | GENERIC_WRITE,	0, NULL, OPEN_EXISTING,	FILE_FLAG_OVERLAPPED,		NULL );
	else								// Open the device using Win9x semantics.
		ghPCIDriver = CreateFile( "\\\\.\\S1D13xxx.VXD", 0,							0, NULL, CREATE_NEW,	FILE_FLAG_DELETE_ON_CLOSE,	NULL );

	GetPciID();

	// If we have an invalid handle then we couldn't open the driver.

	if ( ghPCIDriver != INVALID_HANDLE_VALUE )
	{
		OVERLAPPED OverLapped;
		ZeroMemory( &OverLapped, sizeof(OverLapped) );

		if ( PhysicalAddr == 0 )		// Map in a PCI Board
		{
			// Get virtual address into our PCI array ([0]=virtual, [1]=physical, [2]=blocksize (future))

			fRet = DeviceIoControl( ghPCIDriver, IOCTL_SED_MAP_PCI_BOARD_EXTENDED, &mappednum, sizeof(mappednum), aRetPCI, sizeof(aRetPCI), &nReturned, &OverLapped );
			gPCIAddress[mappednum] = aRetPCI[0];
		}
		else							// The user insists on a particular physical address
		{
			aRetPCI[0] = PhysicalAddr;
			aRetPCI[1] = ISA_BLOCK_SIZE;
			fRet = DeviceIoControl( ghPCIDriver, IOCTL_SED_MAP_PHYSICAL_MEMORY, aRetPCI, sizeof(aRetPCI), &gPCIAddress[mappednum], sizeof(gPCIAddress[mappednum]), &nReturned, &OverLapped );
		}

		if ( fRet )
		{
			if ( pVirtualAddr )
				*pVirtualAddr = gPCIAddress[mappednum];

			if ( pBlockSize )
            {
				if ( nReturned > 2*sizeof(aRetPCI[0]) )
                    *pBlockSize = aRetPCI[2];
                else
				    *pBlockSize = PCI_BLOCK_SIZE;
            }
		}
		else
		{
			gPCIAddress[mappednum] = 0;
		}
	}
	else
	{
		ghPCIDriver = NULL;
	}

	return fRet;
}



//---------------------------------------------------------------------------
// PUBLIC FUNCTION: pcibUnmapAddress()
//---------------------------------------------------------------------------
void pcibUnmapAddress( void )
{
	DWORD nReturned;
	DWORD i;

	// Free up any active interrupt objects to ensure handles are sync'd with resources.

	UnhookInterrupt();

	// Only free up these resources, if they are currently allocated and in use.

	if ( ghPCIDriver )
	{
		// We already have a handle to the driver and the memory we wish to free.
		// Simply call the driver with the appropriate values to free the our memory.

		for(i=0;i<MAX_MEMORYMAPS;i++)
		{
			if ( gPCIAddress )
			{
				OVERLAPPED OverLapped;
				ZeroMemory( &OverLapped, sizeof(OverLapped) );

				DeviceIoControl( ghPCIDriver, IOCTL_SED_UNMAP_LINEAR_MEMORY, &gPCIAddress[i], sizeof(gPCIAddress[i]), NULL, 0, &nReturned, &OverLapped );
				gPCIAddress[i] = 0;
			}
		}

		// Close the handle. This also dynamically UNLOADs the driver under Win9x.

		CloseHandle( ghPCIDriver );
		ghPCIDriver = NULL;
	}
}



//---------------------------------------------------------------------------------------
// PUBLIC FUNCTION: pcibAddInterruptRegister()
//---------------------------------------------------------------------------------------
Boolean pcibAddInterruptRegister( UInt32 EnableRegOffset, UInt32 StatusRegOffset, UInt32 BitMask, UInt32 AccessSize, Boolean fRawStatus )
{
	Boolean fRet = TRUE;

	if ( gISRInfo.nIntInfoItems < MAX_INT_REGS )
	{
		INT_STRUCT *pIntInfo = &gISRInfo.aIntInfo[gISRInfo.nIntInfoItems];

		// Set our process ID if this is the first time here.

		if ( gISRInfo.nIntInfoItems == 0 )
			gISRInfo.dwProcessID = GetCurrentProcessId();

		// Add this interrupt register information to interrupt register array.

		pIntInfo->EnableReg	= EnableRegOffset;
		pIntInfo->StatusReg	= StatusRegOffset;
		pIntInfo->BitMask	= BitMask;
		pIntInfo->AccessSize= AccessSize;
		pIntInfo->Flags		= (fRawStatus) ? INTFLAG_RAWSTATUS : INTFLAG_NONE;

		gISRInfo.nIntInfoItems++;
	}
	else
		fRet = FALSE;

	return fRet;
}



//---------------------------------------------------------------------------------------
// PUBLIC FUNCTION: pcibHookInterrupt()
//---------------------------------------------------------------------------------------
Boolean pcibHookInterrupt( PISR_CALLBACK pISRCallback )
{
	DWORD	nReturned;
	Boolean	fRet;

	// If the PCI driver has not been acquired, exit with an error.

	if ( !ghPCIDriver )
		return FALSE;

	// If the caller passes a NULL callback, kill the ISR hook and return successful.

	if ( pISRCallback == NULL )
	{
		UnhookInterrupt();
		return TRUE;
	}

	// If an ISR is already hooked and active, allow the caller to change the callback ISR.

	if ( gfISRHooked )
	{
		gpISRCallback = pISRCallback;
		return TRUE;
	}

	fRet = FALSE;													// Assume code below fails, set to no error is everything is OK.

	// Now, ask the driver to hook in the new ISR callback.

	gfISRHooked = DeviceIoControl( ghPCIDriver, IOCTL_SED_HOOK_ISR_CALLBACK, (LPVOID)&gISRInfo, sizeof(gISRInfo), &ghISRId, sizeof(ghISRId), &nReturned, NULL );

	if ( gfISRHooked )
	{
		HANDLE	 hISREvent;				// ISR thread completion/unblock event

		// Create the ISR complete/unblock event handle.

		hISREvent = CreateEvent( NULL, TRUE, FALSE, NULL );		// Manual reset, Initial State = non-signaled, Unnamed object

		if ( hISREvent )
		{
			HANDLE	hISRThread;				// Thread handle to ISR dispatcher code
			DWORD	dwISRThreadId;			// Thread ID of ISR dispatcher code

			gpISRCallback = pISRCallback;

			// Create the ISR dispatcher thread itself.
			// (handle cannot be inherited, use default stack size, pass event handle to thread, thread to run immediately).

			hISRThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)DispatchPCIInterrupt, hISREvent, 0, &dwISRThreadId );

			if ( hISRThread )
			{
				// Testing with the API code indicates that this thread MUST be high priority,
				// otherwise some interrupts are lost.  If this thread signals an event and
				// if this thread is not high priority, then the signalled event's thread will
				// start immediately, causing problems with the device driver's int handling.
				// If this thread is high priority, it will ensure that it gets back to the
				// overlapped I/O pending state before other threads get invoked.
				SetThreadPriority( hISRThread, THREAD_PRIORITY_TIME_CRITICAL );	// DSM TEST!!!

				CloseHandle( hISRThread );					// Release the thread handle.
				fRet = TRUE;								// Flag that everything is OK.
			}
			else
				CloseHandle( hISREvent );
		}
	}

	if ( !fRet )
		UnhookInterrupt();

	return fRet;
}




//---------------------------------------------------------------------------
// PRIVATE FUNCTION: UnhookInterrupt()
//---------------------------------------------------------------------------
static void UnhookInterrupt( void )
{
	DWORD		nReturned;

	// Remove the active application ISR callback, if any.

	gpISRCallback = NULL;

	// If the ISR is still hooked, ask the driver to take it down.

	if ( gfISRHooked )
	{
		gfISRHooked = FALSE;

		// There is previously hooked ISR, ask the driver to unhook it if the driver is still alive.
		if ( ghPCIDriver )
			DeviceIoControl( ghPCIDriver, IOCTL_SED_UNHOOK_ISR_CALLBACK, (LPVOID)&gISRInfo, sizeof(gISRInfo), &ghISRId, sizeof(ghISRId), &nReturned, NULL );
	}
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: DispatchPCIInterrupt()
//---------------------------------------------------------------------------
static Boolean DispatchPCIInterrupt( LPVOID hISREvent )
{
	DWORD		nReturned;
	OVERLAPPED	OverLapped;

	OverLapped.hEvent		= (HANDLE)hISREvent;
	OverLapped.Offset		= 0;
	OverLapped.OffsetHigh	= 0;

	while ( gfISRHooked )
	{
		if ( !WriteFile(ghPCIDriver, &ghISRId, sizeof(ghISRId), &nReturned, &OverLapped) )
		{
			if ( GetLastError() != ERROR_IO_PENDING )
				continue;

			// Enable the PCI Interrupt if the product supports disabling interrupts at the BEM.
			if (gVendorId==0x14EB && gProductId==0x0030)
			{
				EnablePciInterrupt();
			}

			WaitForSingleObject( OverLapped.hEvent, INFINITE );
			ResetEvent( OverLapped.hEvent );
		}

		if ( !gfISRHooked )
			break;

		if ( gpISRCallback )
			gpISRCallback();
	}

	CloseHandle( OverLapped.hEvent );

	return TRUE;
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: GetPciID()
//---------------------------------------------------------------------------
static void GetPciID( void )
{
	DWORD		nReturned;
	DWORD		inBuffer;
	DWORD		id=0;

	if ( ghPCIDriver )
		DeviceIoControl( ghPCIDriver, IOCTL_SED_GET_ID, (LPVOID)&inBuffer, sizeof(inBuffer), &id, sizeof(id), &nReturned, NULL );

	gVendorId = id>>16;
	gProductId = id&0xFFFF;
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: EnablePciInterrupt()
//---------------------------------------------------------------------------
static void EnablePciInterrupt( void )
{
	DWORD		nReturned;
	DWORD		inBuffer;
	DWORD		outBuffer;

	if ( ghPCIDriver )
		DeviceIoControl( ghPCIDriver, IOCTL_SED_ENABLE_INTERRUPT, (LPVOID)&inBuffer, sizeof(inBuffer), &outBuffer, sizeof(outBuffer), &nReturned, NULL );
}

