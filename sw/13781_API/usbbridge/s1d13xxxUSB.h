//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997 Anchor Chips, Inc.  May not be reproduced without
// permission.  See the license agreement for more details.
//
// (C)SEIKO EPSON CORPORATION 2005-2007. All rights reserved.
// 
// File:      13xxxUSB.h
//
// Purpose:
//    Header file for the Ezusb USB Device Driver
//
// Environment:
//    kernel mode
//
//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Ezusb Fx2 specific section. Defines related to FW download and chip control.
//
// Vendor specific request code for Anchor Upload/Download
//
// This one is implemented in the core
//
#define ANCHOR_LOAD_INTERNAL  0xA0
//
// These commands are not implemented in the core.  Requires firmware
//
#define ANCHOR_LOAD_EXTERNAL  0xA3
#define ANCHOR_ISFX2          0xAC
//
// This is the highest internal RAM address for the AN2131Q
//
#define MAX_INTERNAL_ADDRESS  0x1B3F
#define INTERNAL_RAM(address) ((address <= MAX_INTERNAL_ADDRESS) ? 1 : 0)
//
// EZ-USB Control and Status Register.  Bit 0 controls 8051 reset
//
#define CPUCS_REG_EZUSB    0x7F92
#define CPUCS_REG_FX2      0xE600
//
#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char BYTE;
#endif // !_BYTE_DEFINED
//
#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short WORD;
#endif // !_WORD_DEFINED
//
typedef struct _VENDOR_REQUEST_IN
{
    BYTE    bRequest;
    WORD    wValue;
    WORD    wIndex;
    WORD    wLength;
    BYTE    direction;
    BYTE    bData;
} VENDOR_REQUEST_IN, *PVENDOR_REQUEST_IN;
///////////////////////////////////////////////////////////
//
// control structure for bulk and interrupt data transfers
//
///////////////////////////////////////////////////////////
typedef struct _BULK_TRANSFER_CONTROL
{
   ULONG pipeNum;
} BULK_TRANSFER_CONTROL, *PBULK_TRANSFER_CONTROL;
//
typedef struct _BULK_LATENCY_CONTROL
{
   ULONG bulkPipeNum;
   ULONG intPipeNum;
   ULONG loops;
} BULK_LATENCY_CONTROL, *PBULK_LATENCY_CONTROL;
///////////////////////////////////////////////////////////
//
// control structure for sending vendor or class specific requests
// to the control endpoint.
//
///////////////////////////////////////////////////////////
typedef struct _VENDOR_OR_CLASS_REQUEST_CONTROL
{
   // transfer direction (0=host to device, 1=device to host)
   UCHAR direction;
   // request type (1=class, 2=vendor)
   UCHAR requestType;
   // recipient (0=device,1=interface,2=endpoint,3=other)
   UCHAR recepient;
   // see the USB Specification for an explanation of the
   // following paramaters.
   UCHAR requestTypeReservedBits;
   UCHAR request;
   USHORT value;
   USHORT index;
} VENDOR_OR_CLASS_REQUEST_CONTROL, *PVENDOR_OR_CLASS_REQUEST_CONTROL;
//
typedef struct _SET_FEATURE_CONTROL
{
   USHORT FeatureSelector;
   USHORT Index;
} SET_FEATURE_CONTROL, *PSET_FEATURE_CONTROL;
///////////////////////////////////////////////////////////
//
// control structure for Anchor Downloads
//
///////////////////////////////////////////////////////////
typedef struct _ANCHOR_DOWNLOAD_CONTROL
{
   WORD Offset;
} ANCHOR_DOWNLOAD_CONTROL, *PANCHOR_DOWNLOAD_CONTROL;
#define MAX_INTEL_HEX_RECORD_LENGTH 16
//
typedef struct _INTEL_HEX_RECORD
{
   BYTE  Length;
   WORD  Address;
   BYTE  Type;
   BYTE  Data[MAX_INTEL_HEX_RECORD_LENGTH];
} INTEL_HEX_RECORD, *PINTEL_HEX_RECORD;
//
typedef struct _SET_INTERFACE_IN
{
   UCHAR interfaceNum;
   UCHAR alternateSetting;
} SET_INTERFACE_IN, *PSET_INTERFACE_IN;
//
typedef struct _GET_STRING_DESCRIPTOR_IN
{
   UCHAR    Index;
   USHORT   LanguageId;
} GET_STRING_DESCRIPTOR_IN, *PGET_STRING_DESCRIPTOR_IN;
//
typedef struct _EZUSB_DRIVER_VERSION
{
   WORD     MajorVersion;
   WORD     MinorVersion;
   WORD     BuildVersion;
} EZUSB_DRIVER_VERSION, *PEZUSB_DRIVER_VERSION;
//
#ifdef DRIVER
//
typedef struct _EZUSB_FIRMWARE
{
   // tag contains a string to identify the start of the firmware
   // image in the driver binary.  Another utilty can then be used
   // to replace the firmware image inthe driver without requiring
   // a recompile
   UCHAR tag[10];
   ULONG size;
   UCHAR firmware[];
} EZUSB_FIRMWARE, *PEZUSB_FIRMWARE;
#define Ezusb_NAME_MAX  64
#define Ezusb_MAX_PIPES 4
//
// The interface number on this device that this driver expects to use
// This would be in the bInterfaceNumber field of the Interface Descriptor, hence
// this device driver would need to know this value.
//
#define SAMPLE_INTERFACE_NBR 0x00
//
typedef struct _Ezusb_PIPE {
    BOOLEAN Opened;
    UCHAR Pad[3];
    PUSBD_PIPE_INFORMATION PipeInfo;
    WCHAR Name[Ezusb_NAME_MAX];
	LARGE_INTEGER BytesXfered;
	LARGE_INTEGER ElapsedTime;
	LARGE_INTEGER TimerStart;
	LARGE_INTEGER ElapsedCycles;
	LARGE_INTEGER CyclesStart;
	BOOLEAN		  bPerfTimerEnabled;
} EzusbPIPE, *PEzusb_PIPE;
//
typedef struct _Ezusb_RW_CONTEXT {
    PURB Urb;
    PDEVICE_OBJECT DeviceObject;
} Ezusb_RW_CONTEXT, *PEzusb_RW_CONTEXT;
// End of the Ezusb Fx2 specific section.
//
// PnP Device States used by Driver.
typedef enum _PNP_DEVICE_STATE {
    // In STATE_REMOVED, we immediately fail any received I/O requests
    STATE_REMOVED                   = 0x00000000,
    // In STATE_SURPRISE_REMOVED, we immediately fail all incoming requests
    STATE_SURPRISE_REMOVED          = 0x00010000,
    // In STATE_NEVER_STARTED, we immediately fail all incoming requests
    STATE_NEVER_STARTED             = 0x00020000,
    // In STATE_STARTED, requests are processed and removed from the queues normally
    STATE_STARTED                   = 0x01210000,
   // In STATE_STOPPED, we fail incoming requests
    STATE_STOPPED                   = 0x10100000,
    // In STATE_POWER_PROCESSING, we fail incoming requests
    STATE_POWER_PROCESSING          = 0x11000000,
} PNP_DEVICESTATE, *PPNPDEVICE_STATE;
//
// A structure representing the Ezusb Device Instance information
//
typedef struct _DEVICE_EXTENSION
{
   // physical device object
   PDEVICE_OBJECT       PhysicalDeviceObject;        
   // Device object we call when submitting Urbs/Irps to the USB stack
   PDEVICE_OBJECT		StackDeviceObject;		
   // Indicates the device needs to be cleaned up (ie., some configuration
   // has occurred and needs to be torn down).
   BOOLEAN              NeedCleanup;
   BOOLEAN              AcceptingRequests;
   BOOLEAN              PrometheusSequenceLocked;
   // configuration handle for the configuration the
   // device is currently in
   USBD_CONFIGURATION_HANDLE ConfigurationHandle;
   // ptr to the USB device descriptor
   // for this device
   PUSB_DEVICE_DESCRIPTOR DeviceDescriptor;
   // we support up to one interface
   PUSBD_INTERFACE_INFORMATION Interface;
   // the number of device handles currently open to the device object.
   // Gets incremented by Create and decremented by Close
   ULONG                OpenHandles;
   // Name buffer for our named Functional device object link
   WCHAR DeviceLinkNameBuffer[Ezusb_NAME_MAX];
   // This member is used to store the URB status of the
   // most recently failed URB.  If a USB transfer fails, a caller
   // can use IOCTL_EZUSB_GET_LAST_ERROR to retrieve this value.
   // There's only room for one, so you better get it quick (or at
   // least before the next URB failure occurs).
   USBD_STATUS          LastFailedUrbStatus;
   // use counter for the device.  Gets incremented when the driver receives
   // a request and gets decremented when a request s completed.
   LONG                 usage;
   // this ev gets set when it is ok to remove the device
   KEVENT               evRemove;
   // TRUE if we're trying to remove this device
   BOOLEAN              removing;
   EzusbPIPE            PipeList[Ezusb_MAX_PIPES];
   ULONG                PendingIoCount;
   // Added by tpk for the device power management
   DEVICE_CAPABILITIES  DeviceCapabilities;
   BOOLEAN              DeviceCapabilitiesInitialized;
   ULONG                PowerDownLevel;
   DEVICE_POWER_STATE   DevicePowerState;
   SYSTEM_POWER_STATE   SystemPowerState;
   // DEVICE_POWER_STATE[PowerSystemMaximum] array of system
   //  to device power mappings
   DEVICE_POWER_STATE   SystemToDevicePowerMappings[PowerSystemMaximum];
    // PIRP System power IRP temporary storage
   PIRP                 SystemPowerIrp;
    // PnP State Variables
   PNP_DEVICESTATE      DevicePnPState;
   KSPIN_LOCK           IoStateLock;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;
//
#if DBG
#define EzusbKdPrint(_x_)
#define TRAP()
#define Pwr_KdPrint(_x_) DbgPrint _x_ ;
//#define TRAP() DbgBreakPoint()
//#define EzusbKdPrint(_x_) DbgPrint("Ezusb.SYS: "); \
//                             DbgPrint _x_ ;
//#define TRAP() DbgBreakPoint()
#else
//#define Pwr_KdPrint(_x_) 
#define Pwr_KdPrint(_x_) DbgPrint _x_ ;
#define EzusbKdPrint(_x_)
#define TRAP()
#endif
//
NTSTATUS
EzusbFileRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );
//        
NTSTATUS
EzusbDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );
//
VOID
EzusbUnload(
    IN PDRIVER_OBJECT DriverObject
    );
//
NTSTATUS
EzusbStartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
EzusbStopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
EzusbRemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
EzusbCallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    );
//
NTSTATUS
EzusbPnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );
//
NTSTATUS
EzusbCreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT *DeviceObject,
    LONG Instance
    );
//
NTSTATUS
EzusbConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
EzusbCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );
//
NTSTATUS
EzusbClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );
//
NTSTATUS
EzusbRead_Write(
   IN  PDEVICE_OBJECT DeviceObject,
   IN  PIRP Irp
   );
//
NTSTATUS
EzusbProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );
//
NTSTATUS
EzusbSelectInterfaces(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    IN PUSBD_INTERFACE_INFORMATION Interface
    );
//
NTSTATUS
EzusbResetPipe(
   IN PDEVICE_OBJECT DeviceObject,
   IN ULONG PipeNum
   );
//
NTSTATUS
EzusbAbortPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN USBD_PIPE_HANDLE PipeHandle
    );
//
ULONG
EzusbGetCurrentFrameNumber(
    IN PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
EzusbRead_Write_Direct(
    IN  PDEVICE_OBJECT DeviceObject,
    IN  PIRP Irp,
    IN  BOOLEAN Read
    );
//
ULONG
EzusbDownloadTest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PVENDOR_REQUEST_IN pVendorRequest
    );
//
NTSTATUS
EzusbResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    );
//
VOID
EzusbCleanup(
    PDEVICE_OBJECT DeviceObject
    );
//
ULONG
EzusbGetDescriptor(
    IN PDEVICE_OBJECT fdo,
    PVOID             DescriptorBuffer,
    ULONG             BufferLength,
    UCHAR             DescriptorType
    );
//
ULONG
EzusbGetDeviceDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    PVOID             pvOutputBuffer
    );
//
ULONG
EzusbGetConfigDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    PVOID             pvOutputBuffer,
    ULONG             ulngth
    );
//
ULONG
EzusbVendorRequest(
    IN PDEVICE_OBJECT DeviceObject,
    PVENDOR_REQUEST_IN pVendorRequest
    );
//
ULONG
EzusbGetCurrentConfig(
    IN PDEVICE_OBJECT DeviceObject,
    IN PVENDOR_REQUEST_IN pVendorRequest
    );
//
ULONG
EzusbGetCurrentInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PVENDOR_REQUEST_IN pVendorRequest
    );
//
PUSB_CONFIGURATION_DESCRIPTOR
GetConfigDescriptor(
    IN PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS
SetInterface(
   IN PDEVICE_OBJECT DeviceObject,
   IN UCHAR InterfaceNumber,
   IN UCHAR AlternateSetting
   );
//
ULONG
EzusbGetStringDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    UCHAR             Index,
    USHORT            LanguageId,
    PVOID             pvOutputBuffer,
    ULONG             ulLength
    );
//
NTSTATUS
EzusbVendorRequest2(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );
//
NTSTATUS
ForwardAndWait(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );
//
NTSTATUS
EzusbDefaultPnpHandler(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   );
//
NTSTATUS
EzusbDispatchPnp(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP           Irp
   );
//
NTSTATUS
EzusbDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );
//
NTSTATUS
EzusbHandleStartDevice(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );
//
NTSTATUS
EzusbHandleRemoveDevice(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   );
//
NTSTATUS 
OnRequestComplete(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp,
   IN PKEVENT pev
   );
//
NTSTATUS
CompleteRequest(
   IN PIRP Irp,
   IN NTSTATUS status,
   IN ULONG info
   );
//
BOOLEAN LockDevice(
   IN PDEVICE_OBJECT fdo
   );
//
void UnlockDevice(
   PDEVICE_OBJECT fdo
   );
//
NTSTATUS EzusbAnchorDownload(
   PDEVICE_OBJECT fdo,
   WORD offset,
   PUCHAR downloadBuffer,
   ULONG downloadSize
   );
//
NTSTATUS EzusbDownloadIntelHex(
   PDEVICE_OBJECT fdo,
   PINTEL_HEX_RECORD hexRecord
   );
//
NTSTATUS Ezusb8051Reset(
   PDEVICE_OBJECT fdo,
   UCHAR resetBit
   );
//
BOOLEAN IsFx2(
   IN  PDEVICE_OBJECT fdo
   );
//
NTSTATUS EzusbSetFeature(
   IN PDEVICE_OBJECT fdo,
   IN PSET_FEATURE_CONTROL setFeatureControl
   );
//
LONG
DecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );
//
VOID
IncrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );
//
NTSTATUS 
EzusbHandleSystemPowerIrp(
    PDEVICE_OBJECT fdo,
    PIRP Irp
    );
//
NTSTATUS 
EzusbHandleDevicePowerIrp(
    PDEVICE_OBJECT fdo,
    PIRP Irp
    );
//
NTSTATUS 
SystemPowerIoCompletionRoutine(
    PDEVICE_OBJECT fdo,
    PIRP Irp,
    PVOID Context
    );
//
VOID 
DevicePowerRequestCompletionRoutine(
    PDEVICE_OBJECT fdo,
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus
    );
//
CONST PCHAR 
NtStatusToString(
    NTSTATUS Status
    );
//
CONST PCHAR 
EzusbNtStatusToString(
    NTSTATUS Status
    );
//
PCHAR 
EzusbPrintDevicePowerState(
    DEVICE_POWER_STATE State
    );
//
CONST PCHAR 
EzusbPrintState(
    PDEVICE_EXTENSION DevExt
    );
//
VOID 
EzusbUpdateDeviceState(
    PDEVICE_EXTENSION DevExt,
    PNP_DEVICESTATE State
    );
//
NTSTATUS  
DevicePowerIoCompletionRoutine(
    PDEVICE_OBJECT fdo,
    PIRP Irp, PVOID Context
    );
//
CONST PCHAR 
EzusbMinorFunctionToString(
    UCHAR MajorFunction,
    UCHAR MinorFunction);
//
CONST PCHAR 
EzusbMajorFunctionToString(
    UCHAR MajorFunction
    );
//
PCHAR 
EzusbPrintSystemPowerState(
    SYSTEM_POWER_STATE State
    );
#endif      //DRIVER section
///////////////////////////////////////////////////////
//
//              IOCTL Definitions
//
// User mode applications wishing to send IOCTLs to a kernel mode driver
// must use this file to set up the correct type of IOCTL code permissions.
//
// Note: this file depends on the file DEVIOCTL.H which contains the macro
// definition for "CTL_CODE" below.  Include that file before  you include
// this one in your source code. DEVIOCTL.H is a Microsoft header file.
// DEVIOCTL.H is normally obtained by installing the Windows DDK.
//
///////////////////////////////////////////////////////

//
// Set the base of the IOCTL control codes.  This is somewhat of an
// arbitrary base number, so you can change this if you want unique
// IOCTL codes.  You should consult the Windows NT DDK for valid ranges
// of IOCTL index codes before you choose a base index number.
//

#define EzusbIOCTL_INDEX  0x0800


#define IOCTL_Ezusb_GET_PIPE_INFO     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+0,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_GET_DEVICE_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+1,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+2,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_BULK_OR_INTERRUPT_WRITE     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+3,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_BULK_OR_INTERRUPT_READ      CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+4,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


#define IOCTL_Ezusb_VENDOR_REQUEST              CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+5,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_GET_CURRENT_CONFIG          CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+6,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_ANCHOR_DOWNLOAD             CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+7,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_RESET  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+12,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_RESETPIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+13,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_ABORTPIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+15,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_SETINTERFACE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+16,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_Ezusb_GET_STRING_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+17,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


//
// Perform an IN transfer over the specified bulk or interrupt pipe.
//
// lpInBuffer: BULK_TRANSFER_CONTROL stucture specifying the pipe number to read from
// nInBufferSize: sizeof(BULK_TRANSFER_CONTROL)
// lpOutBuffer: Buffer to hold data read from the device.  
// nOutputBufferSize: size of lpOutBuffer.  This parameter determines
//    the size of the USB transfer.
// lpBytesReturned: actual number of bytes read
// 
#define IOCTL_EZUSB_BULK_READ             CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+19,\
                                                   METHOD_OUT_DIRECT,  \
                                                   FILE_ANY_ACCESS)


//
// Perform an OUT transfer over the specified bulk or interrupt pipe.
//
// lpInBuffer: BULK_TRANSFER_CONTROL stucture specifying the pipe number to write to
// nInBufferSize: sizeof(BULK_TRANSFER_CONTROL)
// lpOutBuffer: Buffer of data to write to the device
// nOutputBufferSize: size of lpOutBuffer.  This parameter determines
//    the size of the USB transfer.
// lpBytesReturned: actual number of bytes written
// 
#define IOCTL_EZUSB_BULK_WRITE            CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+20,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)





//
// The following IOCTL's are defined as using METHOD_DIRECT_IN buffering.
// This means that the output buffer is directly mapped into system
// space and probed for read access by the driver.  This means that it is
// brought into memory if it happens to be paged out to disk.  Even though
// the buffer is only probed for read access, it is safe (probably) to
// write to it as well.  This read/write capability is used for the loopback
// IOCTL's
// 

// TODO Insert Loopback IOCTL's

//
// Retrieve the current USB frame number from the Host Controller
//
// lpInBuffer: NULL
// nInBufferSize: 0
// lpOutBuffer: PULONG to hold current frame number
// nOutputBufferSize: sizeof(PULONG)
// 
#define IOCTL_EZUSB_GET_CURRENT_FRAME_NUMBER  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+21,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


//
// Performs a vendor or class specific control transfer to EP0.  The contents of
// the input parameter determine the type of request.  See the USB spec
// for more information on class and vendor control transfers.
//
// lpInBuffer: PVENDOR_OR_CLASS_REQUEST_CONTROL
// nInBufferSize: sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL)
// lpOutBuffer: pointer to a buffer if the request involves a data transfer
// nOutputBufferSize: size of the transfer buffer (corresponds to the wLength
//    field of the USB setup packet)
// 
#define IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+22,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

//
// Retrieves the actual USBD_STATUS code for the most recently failed
// URB.
//
// lpInBuffer: NULL
// nInBufferSize: 0
// lpOutBuffer: PULONG to hold the URB status
// nOutputBufferSize: sizeof(ULONG)
// 

#define IOCTL_EZUSB_GET_LAST_ERROR   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+23,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

//
// Performs and Anchor Download.
//
// lpInBuffer: PANCHOR_DOWNLOAD_CONTROL
// nInBufferSize: sizeof(ANCHOR_DOWNLOAD_CONTROL)
// lpOutBuffer: pointer to a buffer of data to download to the device
// nOutputBufferSize: size of the transfer buffer
// 
#define IOCTL_EZUSB_ANCHOR_DOWNLOAD   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+27,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

//
// Returns driver version information
//
// lpInBuffer: NULL
// nInBufferSize: 0
// lpOutBuffer: PEZUSB_DRIVER_VERSION
// nOutputBufferSize: sizeof(EZUSB_DRIVER_VERSION)
// 
#define IOCTL_EZUSB_GET_DRIVER_VERSION   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+29,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_SET_FEATURE         CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+33,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


#define IOCTL_EZUSB_BULK_READ_LOCK             CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+50,\
                                                   METHOD_OUT_DIRECT,  \
                                                   FILE_ANY_ACCESS)


#define IOCTL_EZUSB_BULK_WRITE_LOCK        CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   EzusbIOCTL_INDEX+51,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)
