//////////////////////////////////////////////////////////////////////////////
///
///	\file	HCL.H
///
/// \par HCL (Hardware Control Layer)
///	The HCL (Hardware Control Layer) abstracts low level hardware functions
///	such as reading and writing to registers and memory using functions such
///	as seWriteReg(), and seReadReg().  
///	\par
/// There are also common bit wise manipulation functions such as seClearBitsReg()
/// to make coding easier to read and faster to write.
///	\par
///	Burst read and write is also supported for both memory and registers 
///	These functions provide speed improvements when using indirect parallel 
///	or serial host interfaces as the address is only sent once at the 
/// beginning instead of each cycle.
///	\par
///	All API register and memory accesses should be abstracted through this 
///	interface. Different host interfaces can be chosen at compile time using 
///	the definitions in settings.h.
///
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef seHCL_H_INCLUDED
#define seHCL_H_INCLUDED


#include "settings.h"							// Grab compile-time API setting definitions.
#include "datatype.h"							// Grab public definitions and data types.
#include "registers.h"							// Grab chip-specific register information.
#include "constants.h"							// Grab chip-specific constant information.
#include "initdata.h"							// Grab API configuration structure definitions.
#include "os.h"									// Grab API OS services definitions.


#ifdef __cplusplus
   extern "C" {
#endif

#define seINIT_NORMAL			0					// Standard init for seInitializeController()


///////////////////////////////////////////////////////////////////////////
///
/// \brief	Establishes a connection to the MGE controller.
///
///	This function is used to find and establish a "connection" to the MGE 
///	controller. This function MUST be called before ANY other API function 
///	is used.
///
/// This function is the backbone of the entire MGE API. If this function 
///	fails, then the MGE hardware cannot be found and no other API functions 
///	can be used.
///
/// This function does not program the MGE hardware or do any MGE hardware
/// initialization. After this function returns, there is no change in the
/// current state of the MGE hardware itself.
///
/// Typically, this function is only called once to establish a link to the
/// MGE hardware, and if successful, the function seInitializeController()
/// is called next to do the actual MGE hardware initialization.
///
/// \param	None
///
/// \return				
///			- TRUE if successful
///			- FALSE if failure.
///
///////////////////////////////////////////////////////////////////////////
Boolean	seAcquireController( void );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Initializes the MGE controller to a preconfigured state.
///
///	This function is used to initialize the MGE controller to a preconfigured 
///	state. All MGE registers are initialized according to the settings that 
///	have been selected in the CONFIG program.
///
/// Generally, this function does not fail, but it will return FALSE if the
/// function seAcquireController() has not been previously called.
///
/// This function will initialize the MGE registers, in accordance with the 
///	list of registers as laid out in the CONFIG program.  Only those registers 
///	listed in the CONFIG program will be initialize, and only in the sequence 
///	listed in the CONFIG program.
///
/// After this function returns, the registers will be initialized, but the 
///	MGE hardware is not necessarily fully operational, as other API functions 
///	must be used to establish an LCD palette, to clear or display an image in 
///	video memory. Effectively, after this function returns, the MGE hardware 
/// and MGE API is "ready to use".
///
/// \param	None
///
///	\return				
///			- TRUE if successful
///			- FALSE if failure
///
//////////////////////////////////////////////////////////////////////////////
Boolean	seInitializeController( UInt32 Flags );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Performs an 8-bit MGE register read.
///
///	This function is used to do an 8-bit read the contents of a single MGE 
///	register.
///
/// This function is the lowest level method used to read any MGE register
/// values. The Index argument should only be one of the predefined register
/// names in the registers.h file, so as to prevent reading mis-aligned or
/// invalid (non-existent) registers.
///
/// Not all registers can be read at any given time. Some registers cannot be 
/// read if the MGE hardware is in power save mode. The application must
/// ensure that the MGE hardware is in the correct operating mode before 
///	reading registers (see specification for more details).
///
///	\param	Index		Index (offset) of the register to read.
///
///	\return	Returns the contents of the specified MGE register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seReadReg8( UInt32 Index );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Performs a 16-bit MGE register read.
///
//	This function is used to do 16-bit read of the contents of a single 
///	MGE register.
///
/// This function is the lowest level method used to read any MGE register
/// values.  The Index argument should only be one of the predefined register
/// names in the registers.h file, so as to prevent reading mis-aligned or
/// invalid (non-existent) registers.
///
/// Not all registers can be read at any given time. Some registers cannot be 
///	read if the MGE hardware is in power save mode. The application must 
///	ensure that the MGE hardware is in the correct operating mode before 
///	reading registers (see specification for more details).
///
///	\param	Index		Index (offset) of the register to read.
///
///	\return	Returns the contents of the specified MGE register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seReadReg16( UInt32 Index );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Performs an 8-bit write to a single MGE register.
///
///	This function writes 8-bits of data to a single MGE register.  
///
/// This function is the lowest level method used to write any MGE register
/// data.  The index argument should only be one of the predefined register
/// names in the registers.h file, so as to prevent writing to mis-aligned or
/// invalid (non-existent) registers.
///
/// Not all registers can be written at any time. Some registers cannot be 
///	written if the MGE hardware is in power save mode. The application must
/// ensure that the MGE hardware is in the correct operating mode before 
///	writing registers (see specification for more details).
///
/// \param	Index		Register index to write.
///
/// \param	Value		Data to write to the register.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seWriteReg8( UInt32 Index, UInt32 Value );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Performs a 16-bit write to an MGE register.
///
///	This function writes 16-bits of data to an MGE register.  
///
/// This function is the lowest level method used to write any MGE register
/// data.  The index argument should only be one of the predefined register
/// names in the registers.h file, so as to prevent writing to mis-aligned or
/// invalid (non-existent) registers.
///
/// Not all registers can be written at any time. Some registers cannot be 
///	written if the MGE hardware is in power save mode. The application must
/// ensure that the MGE hardware is in the correct operating mode before 
///	writing registers (see specification for more details).
///
/// \param	Index		Register index to write.
///
/// \param	Value		Data to write to the register.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seWriteReg16( UInt32 Index, UInt32 Value );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Modifies the contents of a single 8-bit MGE register. 
///
///	This function modifies the contents of a single 8-bit register by 
///	performing read-modify-write cycle to the specified register index. The 
///	register is first read, the value read is ANDed with the ones-compliment 
///	of ClearBits, then that value is ORed with SetBits, finally the resulting
///	value is written back to the register.
///
/// When using a direct interface, the register read-modify-write cycle is NOT
/// an atomic operation.  When using an indirect interface, the register
/// read-modify-write cycle is an atomic operation, but only to the extent that
/// the state of the indirect interface is protected by forcing an exclusive
/// operation that prevents/blocks other threads from preempting the cycle.
///
/// This function should not be used on any register in which any bit in the
/// register has a different effect for reading vs writing, such any status
/// register that requires a "1" to be written to clear the status state.
///
/// Normally, this function is used to set a new value for a bitfield in a
/// register that contains other bits other than just the bitfield.  If a
/// register contains only one single bitfield, and all other bits are marked
/// as n/a, then the basic seRead/WriteReg() functions can be used, as they
/// are more efficient.
///
/// \param	Index			Register index to modify.
///
///	\param	ClearBits		Bitmask of register bits to be cleared (set to '0').
///							Any bit positions set to '1' in this mask will be
///							cleared in the register value.
///
/// \param	SetBits			Bitmask of register bits to set ( to '1').
///
///	\return	The return value is the results that were written to the register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seModifyReg8( UInt32 Index, UInt32 ClearBits, UInt32 SetBits );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Modifies the contents of a 16-bit MGE register. 
///
///	This function modifies the contents of a 16-bit register by performing 
///	read-modify-write cycle to the specified register index. The register is 
///	first read, the value read is ANDed with the ones-compliment of ClearBits, 
///	then that value is ORed with SetBits, finally the resulting value is 
///	written back to the register.
///
/// When using a direct interface, the register read-modify-write cycle is NOT
/// an atomic operation.  When using an indirect interface, the register
/// read-modify-write cycle is an atomic operation, but only to the extent that
/// the state of the indirect interface is protected by forcing an exclusive
/// operation that prevents/blocks other threads from preempting the cycle.
///
/// This function should not be used on any register in which any bit in the
/// register has a different effect for reading vs writing, such any status
/// register that requires a "1" to be written to clear the status state.
///
/// Normally, this function is used to set a new value for a bitfield in a
/// register that contains other bits other than just the bitfield.  If a
/// register contains only one single bitfield, and all other bits are marked
/// as n/a, then the basic seRead/WriteReg() functions can be used, as they
/// are more efficient.
///
/// \param	Index			Register index to modify.
///
///	\param	ClearBits		Bitmask of register bits to be cleared (set to '0').
///							Any bit positions set to '1' in this mask will be
///							cleared in the register value.
///
/// \param	SetBits			Bitmask of register bits to set ( to '1').
///
///	\return	The return value is the results that were written to the register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seModifyReg16( UInt32 Index, UInt32 ClearBits, UInt32 SetBits );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief Sets (to '1') the bits of a single 8-bit MGE register.
///
///	This function sets (to '1') bits in a single MGE register by performing
///	a read-modify-write cycle at the specified register index. The register 
///	is first read, the read value is ORed with SetBits, and the resulting
///	value is written back to the register.
///
/// When using a direct interface, the register read-modify-write cycle is NOT
/// an atomic operation. When using an indirect interface, the register
/// read-modify-write cycle is an atomic operation, but only to the extent that
/// the state of the indirect interface is protected by forcing an exclusive
/// operation that prevents/blocks other threads from preempting the cycle.
///
/// Normally, this function is used to set a single bit in a register that
/// contains other unrelated bits. This function should not be used to set
/// an entire register to one, as seWriteReg() would be more efficient.
///
/// \param	Index		Register index to clear bits in.
///
///	\param	SetBits		Bitmask of bits to be set to '1' in the register.
///
/// \return	Returns the updated contents of the register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seSetBitsReg8( UInt32 Index, UInt32 SetBits );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief Sets (to '1') the bits of a single 16-bit MGE register.
///
///	This function sets (to '1') bits in a single MGE register by performing
///	a read-modify-write cycle at the specified register index. The register 
///	is first read, the read value is ORed with SetBits, and the resulting
///	value is written back to the register.
///
/// When using a direct interface, the register read-modify-write cycle is NOT
/// an atomic operation. When using an indirect interface, the register
/// read-modify-write cycle is an atomic operation, but only to the extent that
/// the state of the indirect interface is protected by forcing an exclusive
/// operation that prevents/blocks other threads from preempting the cycle.
///
/// Normally, this function is used to set a single bit in a register that
/// contains other unrelated bits. This function should not be used to set
/// an entire register to one, as seWriteReg() would be more efficient.
///
/// \param	Index		Register index to clear bits in.
///
///	\param	SetBits		Bitmask of bits to be set to '1' in the register.
///
/// \return	Returns the updated contents of the register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seSetBitsReg16( UInt32 Index, UInt32 SetBits );


///////////////////////////////////////////////////////////////////////////
///
/// \brief	Clears the bits of a single 8-bit MGE register.
///
///	This function clears (to '0') bits in a single MGE register by performing
///	a read-modify-write cycle at the specified register index. The register 
///	is first read, the read value is ANDed with the ones complement of ClearBits,
///	and the resulting value is written back to the register.
///
/// When using a direct interface, the register read-modify-write cycle is NOT
/// an atomic operation. When using an indirect interface, the register
/// read-modify-write cycle is an atomic operation, but only to the extent that
/// the state of the indirect interface is protected by forcing an exclusive
/// operation that prevents/blocks other threads from preempting the cycle.
///
/// Normally, this function is used to clear a single bit in a register that
/// contains other unrelated bits. This function should not be used to clear
/// an entire register to zero, as seWriteReg() would be more efficient.
///
/// \param	Index			Register index to clear bits in.
///
/// \param	ClearBits		Bitmask of the bits to clear (to '0').
///							Any bit positions set to '1' in this mask 
///							will be cleared in the register.
///
/// \return	Returns the updated contents of the register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seClearBitsReg8( UInt32 Index, UInt32 ClearBits );


///////////////////////////////////////////////////////////////////////////
///
/// \brief	Clears the bits of a single 16-bit MGE register.
///
///	This function clears (to '0') bits in a single MGE register by performing
///	a read-modify-write cycle at the specified register index. The register 
///	is first read, the read value is ANDed with the ones complement of ClearBits,
///	and the resulting value is written back to the register.
///
/// When using a direct interface, the register read-modify-write cycle is NOT
/// an atomic operation. When using an indirect interface, the register
/// read-modify-write cycle is an atomic operation, but only to the extent that
/// the state of the indirect interface is protected by forcing an exclusive
/// operation that prevents/blocks other threads from preempting the cycle.
///
/// Normally, this function is used to clear a single bit in a register that
/// contains other unrelated bits. This function should not be used to clear
/// an entire register to zero, as seWriteReg() would be more efficient.
///
/// \param	Index			Register index to clear bits in.
///
/// \param	ClearBits		Bitmask of the bits to clear (to '0').
///							Any bit positions set to '1' in this mask 
///							will be cleared in the register.
///
/// \return	Returns the updated contents of the register.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seClearBitsReg16( UInt32 Index, UInt32 ClearBits );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Polls a register until the specified bits go to zero or
///			the operation times out.
///
///	This function polls an MGE register, waiting for the specified status 
///	bits to go to ZERO (0). Both the maximum time and a polling interval time 
///	can be specified.
///
/// The function constantly reads the specified register at an interval set by 
///	IntervalMS. If IntervalMS is zero, then this function polls the register 
/// as fast as possible with no intermediate delays.
///
///	The return value will return TRUE only if all the specified bits in the 
///	register are ZERO. If the bits do not go to zero in the time frame 
///	specified by TimeoutMS, then this function will return FALSE indicating 
///	a timeout condition. If the value of TimeoutMS is zero, this function 
///	will test the register only once.
///
/// If a status bit has a brief state change lifetime (like VSYNC), it is best
/// to use an IntervalMS of ZERO, otherwise the low state may be missed.  Using
/// a non-ZERO value for IntervalMS should done when the polled status bit is
/// "sticky" and the event itself is of a longer duration (50ms, 100ms, etc).
/// Since the status bit is not being polling during the interval delay, overall,
/// the average polling latency after a status bit changes will be IntervalMS/2.
/// If the caller needs control immediately after a status bit state change, then
/// IntervalMS should be ZERO or very small (at the expense of CPU and power).
///
///	\param	Index			Register index to poll.
///
///	\param	Bits			Register bits to poll.
///
///	\param	IntervalMS		Interval delay (in ms) between each polling event.
///
///	\param	TimeoutMS		Maximum time to wait (in ms) before giving up polling.
///
///	\return			
///			- TRUE if all monitored bits in register are cleared to zero.
///			- FALSE if the function times out waiting for all bits to go clear.
///
//////////////////////////////////////////////////////////////////////////////
Boolean sePollWhileHighReg8( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Polls a register until the specified bits go to zero or
///			the operation times out.
///
///	This function polls an MGE register, waiting for the specified status 
///	bits to go to ZERO (0). Both the maximum time and a polling interval time 
///	can be specified.
///
/// The function constantly reads the specified register at an interval set by 
///	IntervalMS. If IntervalMS is zero, then this function polls the register 
/// as fast as possible with no intermediate delays.
///
///	The return value will return TRUE only if all the specified bits in the 
///	register are ZERO. If the bits do not go to zero in the time frame 
///	specified by TimeoutMS, then this function will return FALSE indicating 
///	a timeout condition. If the value of TimeoutMS is zero, this function 
///	will test the register only once.
///
/// If a status bit has a brief state change lifetime (like VSYNC), it is best
/// to use an IntervalMS of ZERO, otherwise the low state may be missed.  Using
/// a non-ZERO value for IntervalMS should done when the polled status bit is
/// "sticky" and the event itself is of a longer duration (50ms, 100ms, etc).
/// Since the status bit is not being polling during the interval delay, overall,
/// the average polling latency after a status bit changes will be IntervalMS/2.
/// If the caller needs control immediately after a status bit state change, then
/// IntervalMS should be ZERO or very small (at the expense of CPU and power).
///
///	\param	Index			Register index to poll.
///
///	\param	Bits			Register bits to poll.
///
///	\param	IntervalMS		Interval delay (in ms) between each polling event.
///
///	\param	TimeoutMS		Maximum time to wait (in ms) before giving up polling.
///
///	\return			
///			- TRUE if all monitored bits in register are cleared to zero.
///			- FALSE if the function times out waiting for all bits to go clear.
///
//////////////////////////////////////////////////////////////////////////////
Boolean sePollWhileHighReg16( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Polls a register until the specified bits are set (go to '1') or
///			the operation times out.
///
///	This function polls an MGE register, waiting for the specified status 
///	bits to go to ONE (1). Both the maximum time and a polling interval time 
///	can be specified.
///
/// The function constantly reads the specified register at an interval set by 
///	IntervalMS. If IntervalMS is zero, then this function polls the register 
/// as fast as possible with no intermediate delays.
///
///	The return value will return TRUE only if all the specified bits in the 
///	register are ONE. If the bits are not set in the time frame specified by 
///	TimeoutMS, then this function will return FALSE indicating a timeout 
///	condition. If the value of TimeoutMS is zero, this function  will test 
///	the register only once.
///
/// If a status bit has a brief state change lifetime (like VSYNC), it is best
/// to use an IntervalMS of zero, otherwise the low state may be missed. Using
/// a non-zero value for IntervalMS should done when the polled status bit is
/// "sticky" and the event itself is of a longer duration (50ms, 100ms, etc).
/// Since the status bit is not being polling during the interval delay, overall,
/// the average polling latency after a status bit changes will be IntervalMS/2.
/// If the caller needs control immediately after a status bit state change, then
/// IntervalMS should be ZERO or very small (at the expense of CPU and power).
///
///	\param	Index			Register index to poll.
///
///	\param	Bits			Register bits to poll.
///
///	\param	IntervalMS		Interval delay (in ms) between each polling event.
///
///	\param	TimeoutMS		Maximum time to wait (in ms) before giving up polling.
///
///	\return			
///			- TRUE if all monitored bits in register are set to one.
///			- FALSE if the function times out waiting for all bits to set.
///
//////////////////////////////////////////////////////////////////////////////
Boolean sePollWhileLowReg8( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Polls a register until the specified bits are set (go to '1') or
///			the operation times out.
///
///	This function polls an MGE register, waiting for the specified status 
///	bits to go to ONE (1). Both the maximum time and a polling interval time 
///	can be specified.
///
/// The function constantly reads the specified register at an interval set by 
///	IntervalMS. If IntervalMS is zero, then this function polls the register 
/// as fast as possible with no intermediate delays.
///
///	The return value will return TRUE only if all the specified bits in the 
///	register are ONE. If the bits are not set in the time frame specified by 
///	TimeoutMS, then this function will return FALSE indicating a timeout 
///	condition. If the value of TimeoutMS is zero, this function  will test 
///	the register only once.
///
/// If a status bit has a brief state change lifetime (like VSYNC), it is best
/// to use an IntervalMS of zero, otherwise the low state may be missed. Using
/// a non-zero value for IntervalMS should done when the polled status bit is
/// "sticky" and the event itself is of a longer duration (50ms, 100ms, etc).
/// Since the status bit is not being polling during the interval delay, overall,
/// the average polling latency after a status bit changes will be IntervalMS/2.
/// If the caller needs control immediately after a status bit state change, then
/// IntervalMS should be ZERO or very small (at the expense of CPU and power).
///
///	\param	Index			Register index to poll.
///
///	\param	Bits			Register bits to poll.
///
///	\param	IntervalMS		Interval delay (in ms) between each polling event.
///
///	\param	TimeoutMS		Maximum time to wait (in ms) before giving up polling.
///
///	\return			
///			- TRUE if all monitored bits in register are set to one.
///			- FALSE if the function times out waiting for all bits to set.
///
//////////////////////////////////////////////////////////////////////////////
Boolean sePollWhileLowReg16( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	This function reads a single 8-bit value from the MGE's video memory.
///
/// This function is the lowest level method used to read MGE video memory.
/// The Offset argument is a relative offset from the video memory base address,
/// as setup in the CONFIG program, and must be within the valid memory space.
/// All 8-bit video memory accesses have no alignment restrictions.
///
/// \param	Offset	Offset into the MGE's video memory.
///
///	\return	The 8-bit data stored in video memory at Offset.
///
//////////////////////////////////////////////////////////////////////////////
UInt8 seReadMemory8( UInt32 Offset );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	This function reads a single 16-bit value from the MGE's video memory.
///
/// This function is the lowest level method used to read MGE video memory.
/// The Offset argument is a relative offset from the video memory base address,
/// as setup in the CONFIG program, and must be within the valid memory space.
/// Offset must be evenly divisible by 2 to ensure proper memory alignment.
///
///	\param	Offset	Offset into the MGE's video memory.
///
///	\return	The 16-bit data stored in video memory at Offset.
///
//////////////////////////////////////////////////////////////////////////////
UInt16 seReadMemory16( UInt32 Offset );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	This function reads a single 32-bit value from the MGE's video memory.
///
/// This function is the lowest level method used to read MGE video memory.
/// The Offset argument is a relative offset from the video memory base address,
/// as setup in the CONFIG program, and must be within the valid memory space.
/// Offset must be evenly divisible by 4 to ensure proper memory alignment.
///
///	\param	Offset	Offset into the MGE's video memory.
///
///	\return	The 32-bit data stored in video memory at Offset.
///
//////////////////////////////////////////////////////////////////////////////
UInt32 seReadMemory32( UInt32 Offset );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	This function writes a single 8-bit value to the MGE's video memory.
///
///	This function is the lowest level method used to write MGE video memory.
///	Only the lower 8-bits of Value are written, the upper 24-bits are ignored.
///
///	\param	Offset		Offset into the MGE's video memory.
///
///	\param	Value		The 8-bit data value to write in video memory at Offset.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seWriteMemory8( UInt32 Offset, UInt32 Value );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	This function writes a single 16-bit value to the MGE's video memory.
///
/// This function is the lowest level method used to write MGE video memory.
///
///	\param	Offset		Offset into the MGE's video memory.
///
///	\param	Value		The 16-bit data value to write in video memory at Offset.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seWriteMemory16( UInt32 Offset, UInt32 Value );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	This function writes a single 32-bit value to the MGE's video memory.
///
///	This function is the lowest level method used to write MGE video memory.
///
///	\param	Offset		Offset into the MGE's video memory.
///
///	\param	Value		The 32-bit data value to write in video memory at Offset.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seWriteMemory32( UInt32 Offset, UInt32 Value );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Fills a contiguous block of memory with one repeating 8-bit value.
///
///	This function writes a repeating 8-bit value to a contiguous block of MGE
///	video memory. This can be used to "flood fill" video memory with a value.
///
///	\param	Offset		Offset into the MGE's video memory.
///
///	\param	Value		The 8-bit data value to fill in video memory.
///						Only the lower 8-bits of Value are written, the upper 
///						24-bits are ignored.
///
///	\param	Count		Number of times to write Value, starting at Offset.
///						If Count is Zero (0), then no operation is performed.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seFillMemory8( UInt32 Offset, UInt32 Value, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Fills a contiguous block of memory with one repeating 16-bit value.
///
///	This function writes a repeating 16-bit value to a contiguous block of MGE
///	video memory. This can be used to "flood fill" video memory with a value.
///
///
///	\param	Offset		Offset, in bytes, into the MGE's video memory.
///
///	\param	Value		The 16-bit data value to fill in video memory.
///
///	\param	Count		Number of times to write Value, starting at Offset.
///						If Count is Zero (0), then no operation is performed.  
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seFillMemory16( UInt32 Offset, UInt32 Value, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///
/// \brief	Fills a contiguous block of memory with one repeating 32-bit value.
///
///	This function writes a repeating 32-bit value to a contiguous block of MGE
///	video memory. This can be used to "flood fill" video memory with a value.
///
///	\param	Offset			Offset, in bytes, into the MGE's video memory.
///
///	\param	Value			The 32-bit data value to fill in video memory.
///
///	\param	Count			Number of times to write "Value".
///							If Count is Zero (0), then no operation is performed.  
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seFillMemory32( UInt32 Offset, UInt32 Value, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Optimized 8-bit read operation on a block of MGE display memory.
///
///	This function is used to optimize 8-bit MGE large-block video 
///	memory reads. This routine is intended primarily for Indirect 
///	Interface access modes.
///
/// To prevent video memory overflow, the sum of Offset+Count must not exceed 
/// the end of the memory space. The Offset argument is a relative offset 
/// from the video memory base address. The buffer pointed to by pData must 
/// be sufficiently large to hold all the data that this function will read 
/// from video memory.
///
///	\param	Offset			Offset in video memory to start burst reading.
///
///	\param	pData			Pointer to a caller-supplied buffer to hold the 8-bit read data.
///							If pData is NULL, then no operation is performed.
///
///	\param	Count			Number of times to perform an 8-bit video memory read operation.
///							If Count is ZERO (0) then no operation is performed.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seBurstReadMemory8( UInt32 Offset, UInt8* pData, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Optimized 16-bit read operation on a block of MGE display memory.
///
///	This function is used to optimize 16-bit MGE large-block video 
///	memory reads. This routine is intended primarily for Indirect 
///	Interface access modes.
///
/// To prevent video memory overflow, the sum of Offset+Count must not exceed 
/// the end of the memory space. The Offset argument is a relative offset 
/// from the video memory base address. The buffer pointed to by pData must 
/// be sufficiently large to hold all the data that this function will read 
/// from video memory.
///
///	\param	Offset			Offset in video memory to start burst reading.
///
///	\param	pData			Pointer to a caller-supplied buffer to hold the 16-bit read data.
///							If pData is NULL, then no operation is performed.
///
///	\param	Count			Number of times to perform an 8-bit video memory read operation.
///							If Count is ZERO (0) then no operation is performed.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seBurstReadMemory16( UInt32 Offset, UInt16* pData, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///
///	\brief	Optimized 32-bit read operation on a block of MGE display memory.
///
///	This function is used to optimize 32-bit MGE large-block video 
///	memory reads. This routine is intended primarily for Indirect 
///	Interface access modes.
///
/// To prevent video memory overflow, the sum of Offset+Count must not exceed 
/// the end of the memory space. The Offset argument is a relative offset 
/// from the video memory base address. The buffer pointed to by pData must 
/// be sufficiently large to hold all the data that this function will read 
/// from video memory.
///
///	\param	Offset			Offset in video memory to start burst reading.
///
///	\param	pData			Pointer to a caller-supplied buffer to hold the 32-bit read data.
///							If pData is NULL, then no operation is performed.
///
///	\param	Count			Number of times to perform an 8-bit video memory read operation.
///							If Count is ZERO (0) then no operation is performed.
///
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seBurstReadMemory32( UInt32 Offset, UInt32* pData, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///	
///	\brief	Optimized 8-bit memory block write operation.
///
///	This function is used to optimize 8-bit MGE video memory writes. This
///	routine is primarily intended for use with in Indirect Interface access modes.
///	
///	\param	Offset			Offset into the MGE's video memory.
///
///	\param	pData			Pointer to a caller-supplied buffer that contains the
///							8-bit data to write to video memory.
///							If pData is NULL, then no operation is performed.
///	
///	\param	Count			Number of times to perform an 8-bit video memory write operation.
///							If Count is ZERO (0) then no operation is performed.
///	
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seBurstWriteMemory8( UInt32 Offset, const UInt8* pData, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///	
///	\brief	Optimized 16-bit memory block write operation.
///
///	This function is used to optimize 16-bit MGE video memory writes. This
///	routine is primarily intended for use with in Indirect Interface access modes.
///	
///	\param	Offset			Offset into the MGE's video memory.
///
///	\param	pData			Pointer to a caller-supplied buffer that contains the
///							16-bit data to write to video memory.
///							If pData is NULL, then no operation is performed.
///	
///	\param	Count			Number of times to perform an 8-bit video memory write operation.
///							If Count is ZERO (0) then no operation is performed.
///	
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seBurstWriteMemory16( UInt32 Offset, const UInt16* pData, UInt32 Count );


//////////////////////////////////////////////////////////////////////////////
///	
///	\brief	Optimized 32-bit memory block write operation.
///
///	This function is used to optimize 32-bit MGE video memory writes. This
///	routine is primarily intended for use with in Indirect Interface access modes.
///	
///	\param	Offset			Offset into the MGE's video memory.
///
///	\param	pData			Pointer to a caller-supplied buffer that contains the
///							32-bit data to write to video memory.
///							If pData is NULL, then no operation is performed.
///	
///	\param	Count			Number of times to perform an 8-bit video memory write operation.
///							If Count is ZERO (0) then no operation is performed.
///	
///	\return Nothing
///
//////////////////////////////////////////////////////////////////////////////
void seBurstWriteMemory32( UInt32 Offset, const UInt32* pData, UInt32 Count );



//////////////////////////////////////////////////////////////////////////
// Deprecated funtions - to be removed.
//////////////////////////////////////////////////////////////////////////
UInt32 seReadRaw( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize );
void  seWriteRaw( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize, UInt32 Value );

UInt32 seReadReg32( UInt32 Index );
void seWriteReg32( UInt32 Index, UInt32 Value );

void seBurstReadReg8( UInt32 Index, UInt8* pData, UInt32 Count );
void seBurstReadReg16( UInt32 Index, UInt16* pData, UInt32 Count );
void seBurstReadReg32( UInt32 Index, UInt32* pData, UInt32 Count );

void seBurstWriteReg8( UInt32 Index, const UInt8* pData, UInt32 Count );
void seBurstWriteReg16( UInt32 Index, const UInt16* pData, UInt32 Count );
void seBurstWriteReg32( UInt32 Index, const UInt32* pData, UInt32 Count );


#ifdef __cplusplus
   }
#endif


#endif // seHCL_H_INCLUDED
