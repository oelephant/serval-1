//=============================================================================
//	HCL_PROXY.C
//
//	(C)SEIKO EPSON CORPORATION 2009. All rights reserved.
//=============================================================================


#ifndef HCL_CALL_STRUCT_DEFINED
#define HCL_CALL_STRUCT_DEFINED
typedef struct
{
	Boolean	(*pfnAcquireController)( void );
	Boolean	(*pfnInitializeController)( UInt32 Flags );
	UInt32	(*pfnReadReg8)( UInt32 Index );
	UInt32	(*pfnReadReg16)( UInt32 Index );
	void	(*pfnWriteReg8)( UInt32 Index, UInt32 Value );
	void	(*pfnWriteReg16)( UInt32 Index, UInt32 Value );
	UInt32	(*pfnModifyReg8)( UInt32 Index, UInt32 ClearBits, UInt32 SetBits );
	UInt32	(*pfnModifyReg16)( UInt32 Index, UInt32 ClearBits, UInt32 SetBits );
	UInt32	(*pfnClearBitsReg8)( UInt32 Index, UInt32 ClearBits );
	UInt32	(*pfnClearBitsReg16)( UInt32 Index, UInt32 ClearBits );
	UInt32	(*pfnSetBitsReg8)( UInt32 Index, UInt32 SetBits );
	UInt32	(*pfnSetBitsReg16)( UInt32 Index, UInt32 SetBits );
	void	(*pfnBurstReadReg8)( UInt32 Index, UInt8* pData, UInt32 Count );
	void	(*pfnBurstWriteReg8)( UInt32 Index, const UInt8* pData, UInt32 Count );
	void	(*pfnBurstReadReg16)( UInt32 Index, UInt16* pData, UInt32 Count );
	void	(*pfnBurstWriteReg16)( UInt32 Index, const UInt16* pData, UInt32 Count );
	Boolean	(*pfnPollWhileHighReg8)( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );
	Boolean	(*pfnPollWhileHighReg16)( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );
	Boolean	(*pfnPollWhileLowReg8)( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );
	Boolean	(*pfnPollWhileLowReg16)( UInt32 Index, UInt32 Bits, UInt32 IntervalMS, UInt32 TimeoutMS );
	UInt8	(*pfnReadMemory8)( UInt32 Offset );
	UInt16	(*pfnReadMemory16)( UInt32 Offset );
	UInt32	(*pfnReadMemory32)( UInt32 Offset );
	void	(*pfnWriteMemory8)( UInt32 Offset, UInt32 Value );
	void	(*pfnWriteMemory16)( UInt32 Offset, UInt32 Value );
	void	(*pfnWriteMemory32)( UInt32 Offset, UInt32 Value );
	void	(*pfnFillMemory8)( UInt32 Offset, UInt32 Value, UInt32 Count );
	void	(*pfnFillMemory16)( UInt32 Offset, UInt32 Value, UInt32 Count );
	void	(*pfnFillMemory32)( UInt32 Offset, UInt32 Value, UInt32 Count );
	void	(*pfnBurstReadMemory8)( UInt32 Offset, UInt8* pData, UInt32 Count );
	void	(*pfnBurstReadMemory16)( UInt32 Offset, UInt16* pData, UInt32 Count );
	void	(*pfnBurstReadMemory32)( UInt32 Offset, UInt32* pData, UInt32 Count );
	void	(*pfnBurstWriteMemory8)( UInt32 Offset, const UInt8* pData, UInt32 Count );
	void	(*pfnBurstWriteMemory16)( UInt32 Offset, const UInt16* pData, UInt32 Count );
	void	(*pfnBurstWriteMemory32)( UInt32 Offset, const UInt32* pData, UInt32 Count );
	Boolean	(*pfnSetISRCallback)(void(*pISRCallback)(void));
	UInt32	(*pfnReadRaw)( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize );
	void	(*pfnWriteRaw)( UInt32 BaseIndex, UInt32 Offset, UInt32 ByteSize, UInt32 Value );
} HCL_CALL_STRUCT;
#endif


// Pure C pre-processor "FM".
#define HCL_PROXYtp(aaa,bbb)	aaa##bbb
#define HCL_PROXY(aaa,bbb)		HCL_PROXYtp(aaa,bbb)

#define seAcquireController		HCL_PROXY(seAcquireController,HCL_PROXY_STUB)
#define seInitializeController	HCL_PROXY(seInitializeController,HCL_PROXY_STUB)
#define seReadReg8				HCL_PROXY(seReadReg8,HCL_PROXY_STUB)
#define seReadReg16				HCL_PROXY(seReadReg16,HCL_PROXY_STUB)
#define seWriteReg8				HCL_PROXY(seWriteReg8,HCL_PROXY_STUB)
#define seWriteReg16			HCL_PROXY(seWriteReg16,HCL_PROXY_STUB)
#define seModifyReg8			HCL_PROXY(seModifyReg8,HCL_PROXY_STUB)
#define seModifyReg16			HCL_PROXY(seModifyReg16,HCL_PROXY_STUB)
#define seClearBitsReg8			HCL_PROXY(seClearBitsReg8,HCL_PROXY_STUB)
#define seClearBitsReg16		HCL_PROXY(seClearBitsReg16,HCL_PROXY_STUB)
#define seSetBitsReg8			HCL_PROXY(seSetBitsReg8,HCL_PROXY_STUB)
#define seSetBitsReg16			HCL_PROXY(seSetBitsReg16,HCL_PROXY_STUB)
#define seBurstReadReg8			HCL_PROXY(seBurstReadReg8,HCL_PROXY_STUB)
#define seBurstWriteReg8		HCL_PROXY(seBurstWriteReg8,HCL_PROXY_STUB)
#define seBurstReadReg16		HCL_PROXY(seBurstReadReg16,HCL_PROXY_STUB)
#define seBurstWriteReg16		HCL_PROXY(seBurstWriteReg16,HCL_PROXY_STUB)
#define sePollWhileHighReg8		HCL_PROXY(sePollWhileHighReg8,HCL_PROXY_STUB)
#define sePollWhileHighReg16	HCL_PROXY(sePollWhileHighReg16,HCL_PROXY_STUB)
#define sePollWhileLowReg8		HCL_PROXY(sePollWhileLowReg8,HCL_PROXY_STUB)
#define sePollWhileLowReg16		HCL_PROXY(sePollWhileLowReg16,HCL_PROXY_STUB)
#define seReadMemory8			HCL_PROXY(seReadMemory8,HCL_PROXY_STUB)
#define seReadMemory16			HCL_PROXY(seReadMemory16,HCL_PROXY_STUB)
#define seReadMemory32			HCL_PROXY(seReadMemory32,HCL_PROXY_STUB)
#define seWriteMemory8			HCL_PROXY(seWriteMemory8,HCL_PROXY_STUB)
#define seWriteMemory16			HCL_PROXY(seWriteMemory16,HCL_PROXY_STUB)
#define seWriteMemory32			HCL_PROXY(seWriteMemory32,HCL_PROXY_STUB)
#define seFillMemory8			HCL_PROXY(seFillMemory8,HCL_PROXY_STUB)
#define seFillMemory16			HCL_PROXY(seFillMemory16,HCL_PROXY_STUB)
#define seFillMemory32			HCL_PROXY(seFillMemory32,HCL_PROXY_STUB)
#define seBurstReadMemory8		HCL_PROXY(seBurstReadMemory8,HCL_PROXY_STUB)
#define seBurstReadMemory16		HCL_PROXY(seBurstReadMemory16,HCL_PROXY_STUB)
#define seBurstReadMemory32		HCL_PROXY(seBurstReadMemory32,HCL_PROXY_STUB)
#define seBurstWriteMemory8		HCL_PROXY(seBurstWriteMemory8,HCL_PROXY_STUB)
#define seBurstWriteMemory16	HCL_PROXY(seBurstWriteMemory16,HCL_PROXY_STUB)
#define seBurstWriteMemory32	HCL_PROXY(seBurstWriteMemory32,HCL_PROXY_STUB)
#define SetMemoryReadAddress	HCL_PROXY(SetMemoryReadAddress,HCL_PROXY_STUB)
#define gpIndirectIndex			HCL_PROXY(gpIndirectIndex,HCL_PROXY_STUB)
#define gpIndirectData			HCL_PROXY(gpIndirectData,HCL_PROXY_STUB)
#define gBaseAddress			HCL_PROXY(gBaseAddress,HCL_PROXY_STUB)
#define seSetISRCallback		HCL_PROXY(seSetISRCallback,HCL_PROXY_STUB)
#define seReadRaw				HCL_PROXY(seReadRaw,HCL_PROXY_STUB)
#define seWriteRaw				HCL_PROXY(seWriteRaw,HCL_PROXY_STUB)

#undef seHCL_H_INCLUDED
#include HCL_PROXY_FILE

static const HCL_CALL_STRUCT HCL_PROXY(gHclCall,HCL_PROXY_STUB) =
{
	seAcquireController,
	seInitializeController,
	seReadReg8,
	seReadReg16,
	seWriteReg8,
	seWriteReg16,
	seModifyReg8,
	seModifyReg16,
	seClearBitsReg8,
	seClearBitsReg16,
	seSetBitsReg8,
	seSetBitsReg16,
	seBurstReadReg8,
	seBurstWriteReg8,
	seBurstReadReg16,
	seBurstWriteReg16,
	sePollWhileHighReg8,
	sePollWhileHighReg16,
	sePollWhileLowReg8,
	sePollWhileLowReg16,
	seReadMemory8,
	seReadMemory16,
	seReadMemory32,
	seWriteMemory8,
	seWriteMemory16,
	seWriteMemory32,
	seFillMemory8,
	seFillMemory16,
	seFillMemory32,
	seBurstReadMemory8,
	seBurstReadMemory16,
	seBurstReadMemory32,
	seBurstWriteMemory8,
	seBurstWriteMemory16,
	seBurstWriteMemory32,
	seSetISRCallback,
	seReadRaw,
	seWriteRaw,
};

#undef seAcquireController
#undef seInitializeController
#undef seReadReg8
#undef seReadReg16
#undef seWriteReg8
#undef seWriteReg16
#undef seModifyReg8
#undef seModifyReg16
#undef seClearBitsReg8
#undef seClearBitsReg16
#undef seSetBitsReg8
#undef seSetBitsReg16
#undef seBurstReadReg8
#undef seBurstWriteReg8
#undef seBurstReadReg16
#undef seBurstWriteReg16
#undef sePollWhileHighReg8
#undef sePollWhileHighReg16
#undef sePollWhileLowReg8
#undef sePollWhileLowReg16
#undef seReadMemory8
#undef seReadMemory16
#undef seReadMemory32
#undef seWriteMemory8
#undef seWriteMemory16
#undef seWriteMemory32
#undef seFillMemory8
#undef seFillMemory16
#undef seFillMemory32
#undef seBurstReadMemory8
#undef seBurstReadMemory16
#undef seBurstReadMemory32
#undef seBurstWriteMemory8
#undef seBurstWriteMemory16
#undef seBurstWriteMemory32
#undef SetMemoryReadAddress
#undef gpIndirectIndex
#undef gpIndirectData
#undef gBaseAddress
#undef seSetISRCallback
#undef seReadRaw
#undef seWriteRaw

#undef HCL_PROXYtp
#undef HCL_PROXY
#undef HCL_PROXY_STUB
#undef HCL_PROXY_FILE
