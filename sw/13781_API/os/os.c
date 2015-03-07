//===========================================================================
//	OS.C - OS for WIN32
//
//  (C)SEIKO EPSON CORPORATION 2002-2005. All rights reserved.
//	
//===========================================================================



#pragma warning(disable:4996)	// Disable "function strxxx() was declared deprecated" warning (can't use Microsoft's strxxx_s() secure replacements as they aren't portable).
#pragma warning(disable:4115)	// Disable "named type definition in parentheses" warning
#include <windows.h>			// HANDLE, QueryPerformanceFrequency/Counter(), Create/ReleaseMutex(), Create/SetEvent(), WaitForSingleObject(), CloseHandle()
#include <stdio.h>				// printf(), vprintf()
#include <stdlib.h>				// atexit(), exit()
#include <stdarg.h>				// va_start(), va_end()
#include <process.h>			// _beginthread
#include <conio.h>				// getch()
#include <time.h>				// time, localtime, struct tm
#include "settings.h"
#include "os.h"
#include "pcibridge.h"



typedef struct										// Data Structure for Periodic Timer Thread
{
	HANDLE	hTimerEvent;							// Event handle for this timer (used to kill timer thread)
	UInt32	Milliseconds;							// Frequency of timer to dispatch callback
	void	(*pfnTimerFunction)(void*);				// Pointer to timer dispatch function (runs in timer thread context)
	void	*pParam;								// Optional argument pointer passed from caller to dispatch function
} TimerData;



static HANDLE	ghHclMutex;							// Process synchronization mutex handle
static Boolean  gfMapped;							// True if virtual address has been mapped
static __int64	gSysClockBase;						// Value of CPU SysClock at calibration time
static __int64	gSysClockPS;						// Calculated number of picoseconds per SysClock tick



static Boolean	CalibrateSystemClock( void );
static void		PeriodicTimerThread( void* pParam );
static void 	ExitHandler( void );

#define RDTSC(var64)						\
	__asm									\
	{										\
		__asm rdtsc							\
		__asm mov dword ptr [var64+0],eax	\
		__asm mov dword ptr [var64+4],edx	\
	} ((void)0)



//----------------------------------------------------------------------------
// OS FUNCTION: seOsDelayUS()
//----------------------------------------------------------------------------
void seOsDelayUS( UInt32 Microseconds )
{
	__int64	SysClockCount, SysClockTarget;

	// If requested delay is zero, just leave quickly.
	if ( Microseconds == 0 )
		return;

	// Init the ps/Tick constant if this is the first time here.
	if ( !gSysClockPS )
		CalibrateSystemClock();

	// Get the starting initial counter value now.
	RDTSC( SysClockTarget );

	// Calculate the terminal count of ticks required for requested delay.
	SysClockTarget += (Microseconds*1000000i64)/gSysClockPS;

	// Loop until the counter exceeds the number of ticks in the requested delay.
	do
	{
		RDTSC( SysClockCount );
	} while ( SysClockCount < SysClockTarget );
}



//----------------------------------------------------------------------------
// OS FUNCTION: seOsDelayNS()
//----------------------------------------------------------------------------
void seOsDelayNS( UInt32 Nanoseconds )
{
	__int64	SysClockCount, SysClockTarget;

	// If requested delay is zero, just leave quickly.
	if ( Nanoseconds == 0 )
		return;

	// Init the ps/Tick constant if this is the first time here.
	if ( !gSysClockPS )
		CalibrateSystemClock();

	// Get the starting initial counter value now.
	RDTSC( SysClockTarget );

	// Calculate the terminal count of ticks required for requested delay.
	SysClockTarget += (Nanoseconds*1000i64)/gSysClockPS;

	// Loop until the counter exceeds the number of ticks in the requested delay.
	do
	{
		RDTSC( SysClockCount );
	} while ( SysClockCount < SysClockTarget );
}



//----------------------------------------------------------------------------
// OS FUNCTION: seOsSleepMS()
//----------------------------------------------------------------------------
void seOsSleepMS( UInt32 Milliseconds )
{
	// For Win32, the sleep granularity is 10ms, not 1ms.
	Sleep( Milliseconds );
}



//----------------------------------------------------------------------------
// OS FUNCTION: seOsGetTimeMS()
//----------------------------------------------------------------------------
UInt32 seOsGetTimeMS( void )
{
	__int64 SysClockCount;

	// Init the ps/Tick constant if this is the first time here.
	if ( !gSysClockPS )
		CalibrateSystemClock();

	RDTSC( SysClockCount );

	return (UInt32)(((SysClockCount - gSysClockBase)*gSysClockPS)/1000000000i64);
}



//----------------------------------------------------------------------------
// OS FUNCTION: seOsGetDateTime()
//----------------------------------------------------------------------------
UInt32 seOsGetDateTime( seDateTimeData *pDateTime )
{
	time_t	TimeUTC = time( NULL );

	if ( pDateTime )
	{
		struct tm *pTM = localtime( &TimeUTC );

		pDateTime->day = pTM->tm_mday;
		pDateTime->mon = pTM->tm_mon;
		pDateTime->year = pTM->tm_year + 1900;
		pDateTime->sec = pTM->tm_sec;
		pDateTime->min = pTM->tm_min;
		pDateTime->hour = pTM->tm_hour;
	}

	return (UInt32)TimeUTC;
}



//---------------------------------------------------------------------------
// OS FUNCTION: seOsMapPhysicalToVirtual()
//---------------------------------------------------------------------------
Boolean seOsMapPhysicalToVirtual( UInt32 PhysicalAddr, UInt32 *pVirtualAddr, UInt32 *pBlockSize )
{
	static Boolean fExitHandler = FALSE;

	// Always register the atexit routine at least once, so any allocated
	// resources are guaranteed to be released and freed.

	if ( !fExitHandler )
	{
		fExitHandler = TRUE;
		atexit( ExitHandler );
	}

    gfMapped = pcibMapAddress( PhysicalAddr, pVirtualAddr, pBlockSize ); 

    return gfMapped;
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsSetProcessExclusivity()
//---------------------------------------------------------------------------------------
void seOsSetProcessExclusivity( Boolean fExclusive )
{
	if ( fExclusive )
	{
		if ( !ghHclMutex )
		{
			char mutex_string[MAX_PATH];

			sprintf( mutex_string, "13513MGEMutex%s", getenv( "S1DBOARD" ) );

			ghHclMutex = CreateMutex( NULL, FALSE, mutex_string );
		}
		
		if ( ghHclMutex )
			WaitForSingleObject( ghHclMutex, INFINITE );
	}
	else
	{
		if ( ghHclMutex )
			ReleaseMutex( ghHclMutex );
	}
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsCreatePeriodicTimer()
//---------------------------------------------------------------------------------------
Handle seOsCreatePeriodicTimer( UInt32 Milliseconds, void (*pfnTimerFunction)(void*), void *pParam )
{
	HANDLE	hPeriodicTimer = NULL;

	if ( pfnTimerFunction )
	{
		TimerData *pTD = malloc( sizeof(TimerData) );
		if ( pTD )
		{
			hPeriodicTimer = CreateEvent( NULL, TRUE, FALSE, NULL );
			if ( hPeriodicTimer )
			{
				pTD->hTimerEvent = hPeriodicTimer;
				pTD->Milliseconds = Milliseconds;
				pTD->pfnTimerFunction = pfnTimerFunction;
				pTD->pParam = pParam;
				_beginthread( PeriodicTimerThread, 0, pTD );
			}
			else
				free( pTD );
		}
	}

	return (Handle)hPeriodicTimer;
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsDestroyPeriodicTimer()
//---------------------------------------------------------------------------------------
void seOsDestroyPeriodicTimer( Handle hPeriodicTimer )
{
	SetEvent( (HANDLE)hPeriodicTimer );
}



#ifdef _DEBUG
//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsAssert()
//---------------------------------------------------------------------------------------
#ifndef _WINDOWS
int seOsAssert( const char * pszFormat, ... )
{
	va_list pArgs;
	int Len;
	int Chr;

	Len = printf ( "ASSERT - " );

	va_start( pArgs, pszFormat );
	Len += vprintf( pszFormat, pArgs );
	va_end( pArgs );

	printf ("\nAbort [Y]: " );
	do
	{
		printf ( "\a" );
		Chr = tolower( getch() );
	} while ( Chr != 'y' && Chr != 'n' && Chr != '\r' );

	printf( "\n" );

	if ( Chr != 'n' )
		exit( 99 );

	return Len;
}

#else

int seOsAssert( const char * pszFormat, ... )
{
	int i;
	char szOutput[1024+1];    //max string size
	va_list ArgList;

	va_start(ArgList, pszFormat);
	wvsprintf(szOutput, pszFormat, ArgList); //wvsprintf will not exceed 1024 bytes
	va_end(ArgList);

	i = MessageBox(NULL, szOutput, "ASSERTION FAILURE: Click CANCEL to abort", MB_OKCANCEL);

	if (i == IDCANCEL)
		exit(99);

	return i;
}
#endif //_WINDOWS
#endif //_DEBUG



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsCreateEvent()
//---------------------------------------------------------------------------------------
Handle seOsCreateEvent( void )
{
	return (Handle)CreateEvent( NULL,  // No security descriptor
								FALSE, // Auto reset
								FALSE, // Initially not signaled
								NULL ); // No name
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsWaitForEvent()
//---------------------------------------------------------------------------------------
Boolean seOsWaitForEvent( Handle hEvent, UInt32 timeoutMS )
{
	if ( WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)hEvent,timeoutMS) )
		return TRUE;
	else
		return FALSE; // Probably a timeout, could also have been abandoned. Treat the same.
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsSignalEvent()
//---------------------------------------------------------------------------------------
void seOsSignalEvent( Handle hEvent )
{
	SetEvent( (HANDLE)hEvent );
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsClearEvent()
//---------------------------------------------------------------------------------------
void seOsClearEvent( Handle hEvent )
{
	ResetEvent( (HANDLE)hEvent );
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsDeleteEvent()
//---------------------------------------------------------------------------------------
void seOsDestroyEvent( Handle hEvent )
{
	CloseHandle( (HANDLE)hEvent );
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOSBeginThread()
//
// DESCRIPTION:
//     Create a thread.
// INPUT        pThreadFunction : Function which new thread will start to execute
//              pParam          : Argument passed to new thread
// OUTPUT       RetValue        : Handle to the newly created thread.  -1 means the
// 								  thread creation failed.
//---------------------------------------------------------------------------------------
Handle seOsBeginThread(void (*pThreadFunction) (void*), void* pParam)
{
#ifdef seAPPLICATION_THREADS_SUPPORTED
    return (Handle)CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)(pThreadFunction), pParam, 0, NULL );
#else
	return 0;
#endif
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsWaitForThreadExit()
//
// DESCRIPTION:
//     Wait for a thread to exit.
// INPUT        hThread    : Function which new thread will start to execute
//              timeoutMS       : Time in ms to wait for thread.
// OUTPUT       RetValue        : TRUE on success, FALSE on failure.
//---------------------------------------------------------------------------------------
Boolean seOsWaitForThreadExit( Handle hThread, UInt32 timeoutMS )
{
#ifdef seAPPLICATION_THREADS_SUPPORTED
    if ( WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)hThread,timeoutMS) )
	{
		CloseHandle( hThread );
		return TRUE;
	}
	else
		return FALSE; // Probably a timeout, could also be invalid handle
#else
	return FALSE;
#endif
}


//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsSetThreadPriorityLevel()
//
// DESCRIPTION:
//     Set priority level of a thread
// INPUT        hThread    : Function which new thread will start to execute
//              Level   : New thread priority
// OUTPUT       RetValue        : TRUE on success, FALSE on failure.
//---------------------------------------------------------------------------------------
Boolean seOsSetThreadPriorityLevel( Handle hThread, sePriorityLevel Level )
{
#ifdef seAPPLICATION_THREADS_SUPPORTED
	// Map API priority levels to Win32 levels
	switch( Level )
	{
		default:               return FALSE;

		case sePriorityNormal: return SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );

		case sePriorityLow:    return SetThreadPriority( hThread, THREAD_PRIORITY_LOWEST );

		case sePriorityHigh:   return SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );
	}

#else
	return FALSE;
#endif
}




//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsSuspendThread()
//
// DESCRIPTION:
//     Increment the suspend counter of a thread.
// INPUT        hThread    : Function which new thread will start to execute
//
// OUTPUT       RetValue        : -1 if failed, previous suspend count if successful.
//---------------------------------------------------------------------------------------
int seOsSuspendThread( Handle hThread )
{
#ifdef seAPPLICATION_THREADS_SUPPORTED
    return SuspendThread( hThread );
#else
	return -1;
#endif
}




//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsResumeThread()
//
// DESCRIPTION:
//     Decrement the suspend counter of a thread.
// INPUT        hThread    : Function which new thread will start to execute
//
// OUTPUT       RetValue        : -1 if failed, previous suspend count if successful.
//---------------------------------------------------------------------------------------
int seOsResumeThread( Handle hThread )
{
#ifdef seAPPLICATION_THREADS_SUPPORTED
    return ResumeThread( hThread );
#else
	return -1;
#endif
}


#include "hcl.h"

#define SCANNER_TIME_TO_DIE_MS	1000
#define SCANNER_RESOLUTION_MS	1

#define MAX_INT_REGS			8

static UInt32	gIntStatusReg[MAX_INT_REGS];
static UInt32	gIntBitMask[MAX_INT_REGS];
static UInt32	gIntRegCount = 0;
static void		(*gDispatchFunction)(void) = NULL;
static HANDLE	ghScannerThread = NULL;
static Boolean	gTerminateScanner = FALSE;

static DWORD WINAPI ScannerProc( void* param );



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsAddInterruptRegister()
//
// DESCRIPTION:
//     This call identifies to the OS the MGE's interrupt registers. The OS may need this
//     information (1) to disable the hardware MGE hardware interrupt in resonse to that 
//     interrupt firing (some platforms can do this at Level 0 i.e. without having to 
//     access any MGE registers, in which case, the function may simply return 
//     TRUE) and (2) to simulate hardware interrupts perhaps by continuously polling the 
//     the MGE raw interrupt status register..
//---------------------------------------------------------------------------------------
Boolean seOsAddInterruptRegister( UInt32 RegisterOffset, UInt32 EnableReg, UInt32 StatusReg, UInt32 RawStatusReg, UInt32 BitMask )
{
	UNREFERENCED_PARAMETER(RegisterOffset);
	UNREFERENCED_PARAMETER(EnableReg);
	UNREFERENCED_PARAMETER(RawStatusReg);

	if (gIntRegCount<MAX_INT_REGS)
	{
		gIntStatusReg[gIntRegCount] = StatusReg;
		gIntBitMask[gIntRegCount] = BitMask;
		gIntRegCount++;
		return TRUE;
	}
	else
		return FALSE;
}



//---------------------------------------------------------------------------------------
// OS FUNCTION: seOsSetInterruptDispatchFunction()
//---------------------------------------------------------------------------------------
Boolean seOsSetInterruptDispatchFunction( void(*pDispatchFunction)(void) )
{
	gDispatchFunction = pDispatchFunction;

	if ( pDispatchFunction )
	{
		if ( ghScannerThread == NULL )
		{
			DWORD threadID;

			gTerminateScanner = FALSE;

			// Create a thread which scans interrupt raw status for changes.
			ghScannerThread = CreateThread( NULL, 0, ScannerProc, NULL, 0, &threadID );

			if ( ghScannerThread == NULL )
			{
				gDispatchFunction = NULL;
				return FALSE;
			}
		}
	}
	else
	{
		// Kill scanner thread if it exists.
		if ( ghScannerThread )
		{
			gTerminateScanner = TRUE;

			WaitForSingleObject( ghScannerThread, SCANNER_TIME_TO_DIE_MS );
			// Ignore return value. Not much we can do about it if it times out.

			CloseHandle( ghScannerThread );
			ghScannerThread = NULL;
		}
	}

	return TRUE;
}



//---------------------------------------------------------------------------------------
// PRIVATE FUNCTION: ScannerProc runs in it's own thread and polls the raw IRQ status for 
//                   pending interrupts. It simulates a hardware interrupt generator.
//---------------------------------------------------------------------------------------
static DWORD WINAPI ScannerProc( void* param )
{
	UInt32 i;

	UNREFERENCED_PARAMETER(param);

	while ( gTerminateScanner == FALSE )
	{
		for(i=0;i<gIntRegCount;i++)
		{
			if ( (seReadReg16(gIntStatusReg[i]) & gIntBitMask[i]) > 0 )
			{
				if ( gDispatchFunction )
					gDispatchFunction();
			}
		}

		Sleep( SCANNER_RESOLUTION_MS );		// Wait for a millisecond and continue.
	}

	return 0;
}




//---------------------------------------------------------------------------
// PRIVATE FUNCTION: CalibrateSystemClock()
//---------------------------------------------------------------------------
static Boolean CalibrateSystemClock( void )
{
	#define SYSCLOCK_CALIBRATE_US	500			// Time (in us) to use to calibrate system clock.
	__int64	PerfCountPS;
	__int64	PerfCount, PerfTarget;
	__int64	SysClockCount;
	HANDLE	hProcess = GetCurrentProcess();
	HANDLE	hThread = GetCurrentThread();
	DWORD	ClassPriority = GetPriorityClass( hProcess );
	int		ThreadPriority = GetThreadPriority( hThread );

	if ( QueryPerformanceFrequency((PLARGE_INTEGER)&PerfCount) )
		PerfCountPS = 1000000000000i64/PerfCount;
	else
	{
		seASSERT( FALSE, ("CalibrateSystemClock() QueryPerformanceFrequency() failed.") );
		return FALSE;
	}

	SetPriorityClass( hProcess, REALTIME_PRIORITY_CLASS );
	SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );

	QueryPerformanceCounter( (PLARGE_INTEGER)&PerfTarget );
	PerfTarget += (SYSCLOCK_CALIBRATE_US*1000000i64)/PerfCountPS;

	RDTSC( gSysClockBase );

	do
	{
		QueryPerformanceCounter( (PLARGE_INTEGER)&PerfCount );
	} while ( PerfCount < PerfTarget );

	RDTSC( SysClockCount );

	SetThreadPriority( hThread, ThreadPriority );
	SetPriorityClass( hProcess, ClassPriority );

	gSysClockPS = (SYSCLOCK_CALIBRATE_US*1000000i64)/(SysClockCount-gSysClockBase);

	seASSERT( gSysClockPS > 1000000/4500 && gSysClockPS < 1000000/90, ("CalibrateSystemClock() SysClock not between 90MHz and 4.5GHz") );

	return TRUE;
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: PeriodicTimerThread()
//---------------------------------------------------------------------------
static void PeriodicTimerThread( void* pParam )
{
	TimerData *pTD = (TimerData*)pParam;
	UInt32 CallbackMS = 0;

	while ( WaitForSingleObject(pTD->hTimerEvent,pTD->Milliseconds-CallbackMS) == WAIT_TIMEOUT )
	{
    	UInt32 StartMS = seOsGetTimeMS();

	    pTD->pfnTimerFunction( pTD->pParam );

		if ( (CallbackMS = seOsGetTimeMS()-StartMS) > pTD->Milliseconds )
			CallbackMS = pTD->Milliseconds;
	}

	CloseHandle( pTD->hTimerEvent );
	free( pTD );
}



//---------------------------------------------------------------------------
// PRIVATE FUNCTION: ExitHandler() - Free up resources at exit time.
//---------------------------------------------------------------------------
static void ExitHandler( void )
{
    gfMapped = FALSE;

    pcibUnmapAddress();

	if ( ghHclMutex )
		CloseHandle( ghHclMutex ), ghHclMutex = NULL;
}




