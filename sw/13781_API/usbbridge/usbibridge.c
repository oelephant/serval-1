//===========================================================================
//	USBRIDGE.C
//---------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2007. All rights reserved.
//===========================================================================
#pragma warning(disable:4115)   // Disable nonstandard extension 
#pragma warning(disable:4201)   // nonstandard extension used


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning( disable : 4201 )	// Disable "nameless struct/union" warning in winioctl.h.
#include <winioctl.h>
#include "usbbridge.h"
#include "datatype.h"
#include "cscl.h"
#include <assert.h>


static int			gLastError;				// USBridge last error code
static Boolean		gfISRHooked;			// True if device driver has hooked ISR

static USB_CALLBACK gpISRCallback;			// Application supplied callback function

static void		UnhookInterrupt( void );
static Boolean	DispatchUSBInterrupt( LPVOID hISREvent );


//---------------------------------------------------------------------------------------
// PUBLIC FUNCTION: usbHookInterrupt()
//---------------------------------------------------------------------------------------
Boolean usbHookInterrupt( USB_CALLBACK pISRCallback )
{
	//DWORD		nReturned;

	gLastError = USB_ERR_NONE;

	//// If the USC driver has not been acquired, exit with an error.

	//if ( !ghPCIDriver )
	//{
	//	gLastError = PCIB_ERR_ADAPTER_NOT_MAPPED;
	//	return FALSE;
	//}

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

	gLastError = USB_ERR_IRQ_FAILURE;								// Assume code below fails, set to no error is everything is OK.

	//// Now, ask the driver to hook in the new ISR callback.
	// tpk TBD : The driver must be modified for for asynchronous support
	//gfISRHooked = DeviceIoControl( ghPCIDriver, IOCTL_SED_HOOK_ISR_CALLBACK, (LPVOID)&gISRInfo, sizeof(gISRInfo), &ghISRId, sizeof(ghISRId), &nReturned, NULL );

    if ( !seCsclInterruptsSetup())
        return FALSE;

	gfISRHooked = TRUE;
 	

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

			hISRThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)DispatchUSBInterrupt, hISREvent, 0, &dwISRThreadId );

			if ( hISRThread )
			{
				// Raise the thread priority up to "very high".
				// SetThreadPriority( hISRThread, THREAD_PRIORITY_TIME_CRITICAL );	// DSM FIXIT: Juro says not needed???
				CloseHandle( hISRThread );					// Release the thread handle.
				gLastError = USB_ERR_NONE;							// Flag that everything is OK.
			}
			else
				CloseHandle( hISREvent );
		}
	}

	if ( gLastError != USB_ERR_NONE )
		UnhookInterrupt();

	return (gLastError == USB_ERR_NONE);
}

//---------------------------------------------------------------------------
// PUBLIC FUNCTION: usbbGetLastError() - Get last error code.
//---------------------------------------------------------------------------
int usbbGetLastError( void )
{
	return gLastError;
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: UnhookInterrupt()
//---------------------------------------------------------------------------
static void UnhookInterrupt( void )
{
	// Remove the active application ISR callback, if any.
	gpISRCallback = NULL;

	// If the ISR is still hooked, ask the driver to take it down.
	if ( gfISRHooked )
	{
		gfISRHooked = FALSE;
	}

    // (tpk) TO BE DONE : kill the thread to make a cleaner exit. Otherwise
    // it may not exit while waiting for the seCsclGetMgeInterrupt event.
    seCsclInterruptsReset();
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: DispatchUSBInterrupt()
//---------------------------------------------------------------------------
static Boolean DispatchUSBInterrupt( LPVOID hISREvent )
{
	//OVERLAPPED		OverLapped;
	//OverLapped.hEvent		= (HANDLE)hISREvent;
	//OverLapped.Offset		= 0;
	//OffsetHigh	= 0;
	UInt32 result = 0;

 
    while ( gfISRHooked )
	{
		if (! seCsclGetMgeInterrupt( &result)) 
			break;

		assert( result & 1);
	
		if ( !gfISRHooked )
			break;

		if ( gpISRCallback )
			gpISRCallback();
	}

	CloseHandle( hISREvent );

	return TRUE;

}
