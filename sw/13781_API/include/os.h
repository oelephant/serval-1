/////////////////////////////////////////////////////////////////////////////
///
///	\file	OS.H
///
///	\par OS (Operating System) Support
///	This module abstracts operating system (OS) functions from the rest of 
///	the API. It is written for Microsoft Windows and all OS functions will 
///	need to rewritten if a different target OS is used. This file, along 
///	with pcibridge.h are specific to the environment.
///
//  (C)SEIKO EPSON CORPORATION 2007. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef seOS_H_INCLUDED
#define seOS_H_INCLUDED


#include "datatype.h"


// Allow both standard C and C++ linkages.
#ifdef __cplusplus
   extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	This enum is used for seOsSetThreadPriorityLevel().
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	sePriorityLow = 0,
	sePriorityNormal = 1,
	sePriorityHigh = 2
} sePriorityLevel;


////////////////////////////////////////////////////////////////////////////////
///
/// \brief	This structure is used for storing the date and time.
///
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	int sec;				///< seconds after the minute - [0,59]
	int min;				///< minutes after the hour - [0,59]
	int hour;				///< hours since midnight - [0,23]
	int day;				///< day of the month - [1,31]
	int mon;				///< months since January - [0,11]
	int year;				///< current year - [1970,2038]
} seDateTimeData;


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Perform accurate microsecond delays.
///
///	Generally, this function is used to perform short, sub-millisecond delays.
///	This function may or may not use a hardware timer, and therefore, this
///	function may be CPU intensive during the delay interval.
///
///	\param	Microseconds	Desired delay in microseconds.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seOsDelayUS( UInt32 Microseconds );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Perform accurate nanosecond delays.
///
///	Generally, this function is used to perform short, sub-microsecond delays.
///	This function may or may not use a hardware timer, and therefore, this
///	function may be CPU intensive during the delay interval.
///
///	\param	Nanoseconds	Desired delay in nanoseconds.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seOsDelayNS( UInt32 Nanoseconds );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Performs a less accurate millisecond delay.
///
///	Generally, this function is used to perform longer, multi-millisecond delays.
///	This function, because of the longer delay times, should use some form of
///	hardware timer or OS-provided sleep service so as to minimize CPU utilization.
///	No delay granularity is enforced on this function, and caller's should be
///	aware that delays of less than 10ms may either not delay at all, or may
///	cause a delay that is rounded up to the system's timer granularity.  It is
///	the system integrator's responsibility to code this function as they see fit.
///
///	\param	Milliseconds	Desired delay in milliseconds.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seOsSleepMS( UInt32 Milliseconds );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Returns the system time (tick), in milliseconds.
///
///	This function can be used to get a millisecond time reference from the OS.
///
///	\note that no time base reference is implied in this function. That is,
///	this function does not return a "wall clock" tick count, nor is there any
///	requirements for "number of millisecond ticks since midnight".
///
///	The system tick should be an incrementing value. To test for elapsed 
///	time, callers of this function should test the difference between values 
///	returned by this function. Elapsed time has expired when the difference 
///	is greater than or equal to the target elapsed time. This function should 
///	be implemented such that all 32-bits of the returned time are used, to 
///	prevent rollover time calculation errors. If a 32-bit time cannot be 
///	returned, this function should be re-coded to return a 16-bit value.
///
/// \param	None
///
///	\return	Current system's millisecond "tick" count.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seOsGetTimeMS( void );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief Retrieves the local date and time from the system.
///
/// The system time is returned in the seOsDateTime structure in the format 
///	of year, month, day, hour, minute, second.  The return value is returned 
///	in UTC format, and the seDataTimeData structure is in local time format.
///
/// \param  pDateTime	Pointer to the structure to store the date and time in.
///
/// \return	Number of seconds since Jan 1, 1970 00:00:00 UTC
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seOsGetDateTime( seDateTimeData *pDateTime );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	This function is used by the HCL to map the MGE hardware from a 
///			physical address to a virtual address, when required.
///
///	This function may or may not be required for any particular platform.
///
///	On many platforms, applications are not given direct access to hardware 
/// devices. Accessing the hardware required building and installing a device 
/// driver. This is not always convenient.
///
/// For rapid application prototyping and development, Epson has provided a
///	device driver that can map the physical address of a hardware device into 
///	an address is application address space.
///
///	This function along with the s1d13xxx driver perform this remapping.
///
///	If this function is called with PhysicalAddr set to ZERO (0) then the 
///	function makes the assumption that the MGE is connected to a PCI or USB
/// evaluation board. The function will load the appropriate driver, locate
///	the MGE hardware and map the physical address into application address
///	space.
///
///	If the function is called with PhysicalAddr set to any non-ZERO value
///	the assumption is made that the MGE is hardwired to a particular address.
///	In this situation, the routine simply maps the physical address to an
///	address in application space.
///
///	This routine and the s1d13xxx driver were written for a 32-bit Microsoft
///	Windows environment. For use on any other platform this routine will need 
/// to be recoded.
///
///	\param	PhysicalAddr	Physical address of the MGE hardware.
///							
///	\param	pVirtualAddr	Pointer to a 32-bit object to receive the translated address.
///							
///	\param	pBlockSize		Returned size of the virtual block of memory, in bytes.
///
///	\return					
///			- TRUE if mapping successful
///			- otherwise FALSE
///
//////////////////////////////////////////////////////////////////////////////
Boolean seOsMapPhysicalToVirtual( UInt32 PhysicalAddr, UInt32 *pVirtualAddr, UInt32 *pBlockSize );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief Provides a method of blocking access to the MGE in order for exclusive access.
///
///	This function is used by the HCL to prevent multiple threads of execution
///	from changing the state of the indirect interface. When MGE hardware access 
///	is being prevented, any other thread that uses the HCL to access the MGE 
///	will be blocked.
///
///	The HCL attempts to make protected access cycles as short as possible, 
///	however some HCL functions (such as burst operations) may block access 
///	to the MGE hardware for extended periods of time.
///
///	This function does not maintain a counter and therefore cannot be called 
///	multiple times to protect. It can only be called once to protect, and then 
///	once to release.
///
///	\param	fExclusive	TRUE to protect hardware, FALSE to unprotect.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seOsSetProcessExclusivity( Boolean fExclusive );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief Calls a user defined function at set periods
///
///	This function creates a periodic timer, and uses this period to call a 
///	user-supplied function every time the timer goes off.
///
///	If pfnTimerFunction is NULL, or if a timer cannot be created this function 
///	returns ZERO (0). A timer can be created that fires constantly by specifying 
///	zero for Milliseconds. This can be done when the callback function provides 
///	delays/sleeps itself.
///
///	\param	Milliseconds		Frequency of timer, in milliseconds.
///
///	\param	pfnTimerFunction	Function to call periodically (on the timer).
///
///	\param	pParam				Parameter data passed to the timer function callback.
///
///	\return						A handle to the timer (used to destroy timer).
///
///	\see seOsDestroyPeriodicTimer()
///
//////////////////////////////////////////////////////////////////////////////
Handle seOsCreatePeriodicTimer( UInt32 Milliseconds, void (*pfnTimerFunction)(void*), void *pParam );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Destroys the periodic timer created by seOsCreatePeriodicTimer().
///
///	This function kills a periodic timer created by the seOsCreatePeriodicTimer()
///	function. It is the caller's responsibility to free up any system resources 
///	used by the timer callback function.
///
///	\param	hPeriodicTimer	Handle to the periodic timer to be destroyed.
///
///	\return None
///
///	\see seOsCreatePeriodicTimer()
///
//////////////////////////////////////////////////////////////////////////////
void seOsDestroyPeriodicTimer( Handle hPeriodicTimer );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Creates an event handle, used by the interrupt handler.
///
///	This function creates an event handle, which is set to an non-signaled state.
///
///	\return	A handle to the created event.
///
///	\see seOsWaitForEvent()
///	\see seOsSignalEvent()
///	\see seOsClearEvent()
/// \see seOsDestroyEvent()
///
//////////////////////////////////////////////////////////////////////////////
Handle seOsCreateEvent( void );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Waits for an event handle to become 'signaled'.
///
///	This function waits for the specified event handle to be set to the 
///	signaled state. This function will wait no longer than timeoutMS for 
///	the event to be signal.
///
///	If the event is signaled, waiting for the event with this function will
///	automatically return the event handle to an non-signaled state.
///
///	\param	hEvent		An event handle, returned by seOsCreateEvent().
///						
///	\param	timeoutMS	Maximum time to wait for event to be signaled.
///						
///	\return				TRUE if event signaled, FALSE if timeout occurred.
///
///	\see seOsCreateEvent()
///	\see seOsSignalEvent()
///	\see seOsClearEvent()
/// \see seOsDestroyEvent()
///
//////////////////////////////////////////////////////////////////////////////
Boolean seOsWaitForEvent( Handle hEvent, UInt32 timeoutMS );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Signals an event handle.
///
///	This function is normally called by an interrupt routine (or another thread
///	different from the thread which would be waiting for the event) to signal
///	an event that a particular interrupt has fired. When this function is called,
///	any thread waiting for the event will become signaled, and that thread will
///	be unblocked from any event waiting.
///
///	\param	hEvent	An event handle, returned by seOsCreateEvent().
///
///	\return None
///
///	\see seOsCreateEvent()
///	\see seOsWaitForEvent()
///	\see seOsClearEvent()
/// \see seOsDestroyEvent()
///
//////////////////////////////////////////////////////////////////////////////
void seOsSignalEvent( Handle hEvent );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Clears an event handle.
///
///	This function is normally called by code that wishes to synchronize with an
///	interrupt event, by clearing any previous events before waiting for the next
///	event.  This is normally done in a situation where the code expects to wait
///	for the NEXT event, for example an LCD vertical sync, or camera frame.
///
///	\param	hEvent		An event handle, returned by seOsCreateEvent().
///
///	\return None
///
///	\see seOsCreateEvent()
///	\see seOsWaitForEvent()
///	\see seOsSignalEvent()
/// \see seOsDestroyEvent()
///
//////////////////////////////////////////////////////////////////////////////
void seOsClearEvent( Handle hEvent );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Destroys an event handle.
///
///	This function destroys the specified event handle, and should free any
///	system resources used to manage the event handle. This function should not
///	be called if any thread is currently executing a wait on the event handle.
///
///	\param	hEvent		An event handle, returned by seOsCreateEvent().
///
///	\return None
///
///	\see seOsCreateEvent()
///	\see seOsWaitForEvent()
///	\see seOsSignalEvent()
///	\see seOsClearEvent()
///
//////////////////////////////////////////////////////////////////////////////
void seOsDestroyEvent( Handle hEvent );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Creates a new thread of execution.
///
/// This function creates a new thread of execution and transfers control
///	to the ThreadFunction.
///
///	This function is provided so that some of the API test software can create
///	multiple threads to test and demonstrate MGE interrupt handling.
///
///	This is an optional function and is not used by any API functions.
///
///	\param	pThreadFunction		Pointer to the function that is the new thread to execute.
///
///	\param	pParam				Parameter data passed to the new thread function.
///
///	\return					
///			- 0 if thread creation failed or is not supported.
///			- If successful, the thread handle is returned.
///
///	\see seOsWaitForThreadExit()
///	\see seOsSetThreadPriorityLevel()
///	\see seOsSuspendThread(()
///	\see seOsResumeThread()
///
//////////////////////////////////////////////////////////////////////////////
Handle seOsBeginThread( void (*pThreadFunction)(void*), void* pParam );



//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Waits for a thread to finish.
///
///	This functions waits for a thread to finish executing. After this function 
///	returns successfully, the thread handle is no longer valid.
///
///	This function is provided so that some of the API test software can create
///	multiple threads to test and demonstrate MGE interrupt handling.
///
///	This is an optional function and is not used by any API functions.
///
///	\param	hThread			The thread handle from seOsBeginThread.
///
///	\param	timeoutMS		The time to wait, in milliseconds.
///
///	\return					
///			- TRUE if thread exited successfully.
///			- FALSE if thread exit timed out or threads not supported.
///
///	\see seOsBeginThread
///	\see seOsSetThreadPriorityLevel()
///	\see seOsSuspendThread(()
///	\see seOsResumeThread()
///
//////////////////////////////////////////////////////////////////////////////
Boolean seOsWaitForThreadExit( Handle hThread, UInt32 timeoutMS );



//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Sets a thread priority level.
///
/// Call this function to set the thread execution priority level. The priority 
///	level is finer grained control within the priority class.
///
///	This function is provided so that some of the API test software can create
///	multiple threads to test and demonstrate MGE interrupt handling.
///
///	This is an optional function and is not used by any API functions.
///
///	\param	hThread		The thread handle from seOsBeginThread.
///
///	\param	Level		The priority level to set the thread to.
///
///	\return					
///			- TRUE if priority changed
///			- FALSE if threads not supported or priority level not set 
///           successfully.
///
///	\see seOsBeginThread
///	\see seOsWaitForThreadExit()
///	\see seOsSuspendThread(()
///	\see seOsResumeThread()
///
//////////////////////////////////////////////////////////////////////////////
Boolean seOsSetThreadPriorityLevel( Handle hThread, sePriorityLevel Level );



//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Suspends execution of a thread.
///
///	This function suspends the operation of a thread. Every call to 
///	seOsSuspendThread increments the suspend counter. Every call to 
///	seOsResumeThread reduces the counter. As long as the suspend counter
/// is not ZERO (0) thread operation is suspended.
///
///	This function is provided so that some of the API test software can create
///	multiple threads to test and demonstrate MGE interrupt handling.
///
///	This is an optional function and is not used by any API functions.
///
///	\param	hThread		The thread handle from seOsBeginThread.
///
///	\return	
///         - -1 if failed.
///			- Otherwise the previous suspend count.
///			
///	\see seOsBeginThread
///	\see seOsWaitForThreadExit()
///	\see seOsSetThreadPriorityLevel()
///	\see seOsResumeThread()
///
//////////////////////////////////////////////////////////////////////////////
int seOsSuspendThread( Handle hThread );



//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Resumes execution of a thread.
///
///	Call to resume thread operation.
///
///	This function suspends the operation of a thread. Every call to 
///	seOsSuspendThread increments the suspend counter. Every call to 
///	seOsResumeThread reduces the counter. Thread operation is resumed 
///	when its suspend counter reaches 0.
///
///	This function is provided so that some of the API test software can create
///	multiple threads to test and demonstrate MGE interrupt handling.
///
///	This is an optional function and is not used by any API functions.
///
///	\param	hThread			The thread handle from seOsBeginThread.
///
///	\return	
///         - -1 if failed.
///			- Otherwise the previous suspend count. If the return value is 0,
///           the thread was not previously suspended. If it is 1, the thread
///           was suspended, but is now resumed. Greater than 1 means that
///           the thread is still suspended.
///
///	\see seOsBeginThread
///	\see seOsWaitForThreadExit()
///	\see seOsSetThreadPriorityLevel()
///	\see seOsSuspendThread(()
///
//////////////////////////////////////////////////////////////////////////////
int seOsResumeThread( Handle hThread );



//////////////////////////////////////////////////////////////////////////////
//
//	int seASSERT( exp, args_in_parens );		*** THIS IS A MACRO ***
//
//	\brief	This function is provided to aid in API code debugging.  The API code makes
//			use of this assert function to help diagnose common MGE coding errors.
//
//	See examples in the API code on how seASSERT() is used to catch many different
//	possible API errors, usually a result of bad arguments.  Internally to the OS
//	layer, this MACRO makes a call to the function, seOsAssert().  This function
//	is provided in the OS layer so that system integrators can decide how they
//	want asserts to be handled, which could be anything from do nothing, to
//	using a serial port, or USB port, or a custom debugging system.
//
//	\param	exp		Expression that must resolve TRUE if not an error.
//
//	\param	args_in_parens	A printf-style argument list, that will be used if exp
//							resolved to FALSE.
//
//	\return	Like printf, returns the number of characters displayed
//			if this function is allowed to return to the caller.
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
  #define	seASSERT( exp, assert_args_in_parens )			(int)( !(exp) ? seOsAssert assert_args_in_parens : 0 )
  int		seOsAssert( const char* pszFormat, ... );
#else
  #define	seASSERT( e, a )								((int)0)
#endif


#ifdef __cplusplus
   }
#endif


#endif	// seOS_H_INCLUDED
