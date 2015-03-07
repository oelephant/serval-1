//----------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2007. All rights reserved.
//----------------------------------------------------------------------------

#pragma warning(disable:4115)   // Disable nonstandard extension 
#ifndef seUSB_TRANSPORT_H_INCLUDED
#define seUSB_TRANSPORT_H_INCLUDED
#include "datatype.h"
#include <windows.h>
#include "cscl.h"

Boolean		seUsbTransportSetup(void);
Boolean		seUsbTransportGetConfig(void);
UInt32		seUsbTransportPktOut( seCSCL_PACKET_TYPE type, PVOID pBuffer, int length, Boolean lock);
Boolean		seUsbTransportIntPktIn ( PVOID pBuffer, int length );
UInt32		seUsbTransportPktIn ( PVOID pBuffer, int length, Boolean lock);
void		seUsbTransportGetPipeInfo(void);
Boolean		seUsbTransportToggleVerbose(void);
Boolean     seUsbTransportReset(void);
Boolean     seUsbTransportInterruptsSetup(void);
Boolean     seUsbTransportInterruptsReset(void);

/*----USB Mini Driver's specific information.---------*/

#define GET_CONFIG_DESCRIPTOR_LENGTH(pv) \
    ((pUsb_Configuration_Descriptor)pv)->wTotalLength

/*----------------------------------------------------*/
typedef struct __usb_Config_Descriptor__ {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT wTotalLength;
    UCHAR bNumInterfaces;
    UCHAR bConfigurationValue;
    UCHAR iConfiguration;
    UCHAR bmAttributes;
    UCHAR MaxPower;
} Usb_Configuration_Descriptor, *pUsb_Configuration_Descriptor;
/*----------------------------------------------------*/
typedef PVOID USBD_PIPE_HANDLE;
typedef PVOID USBD_CONFIGURATION_HANDLE;
typedef PVOID USBD_INTERFACE_HANDLE;

typedef enum _USBD_PIPE_TYPE {
    UsbdPipeTypeControl,
    UsbdPipeTypeIsochronous,
    UsbdPipeTypeBulk,
    UsbdPipeTypeInterrupt
} USBD_PIPE_TYPE;
/*----------------------------------------------------*/
typedef struct _USBD_PIPE_INFORMATION {
    //
    // OUTPUT
    // These fields are filled in by USBD
    //
    USHORT MaximumPacketSize;  // Maximum packet size for this pipe
    UCHAR EndpointAddress;     // 8 bit USB endpoint address (includes direction)
                               // taken from endpoint descriptor
    UCHAR Interval;            // Polling interval in ms if interrupt pipe 
    
    USBD_PIPE_TYPE PipeType;   // PipeType identifies type of transfer valid for this pipe
    USBD_PIPE_HANDLE PipeHandle;
    
    //
    // INPUT
    // These fields are filled in by the client driver
    //
    ULONG MaximumTransferSize; // Maximum size for a single request
                               // in bytes.
    ULONG PipeFlags;                                   
} USBD_PIPE_INFORMATION, *PUSBD_PIPE_INFORMATION;

/*----------------------------------------------------*/
typedef struct _USBD_INTERFACE_INFORMATION {
    USHORT Length;       // Length of this structure, including
                         // all pipe information structures that
                         // follow.
    //
    // INPUT
    //
    // Interface number and Alternate setting this
    // structure is associated with
    //
    UCHAR InterfaceNumber;
    UCHAR AlternateSetting;
    
    //
    // OUTPUT
    // These fields are filled in by USBD
    //
    UCHAR Class;
    UCHAR SubClass;
    UCHAR Protocol;
    UCHAR Reserved;
    
    USBD_INTERFACE_HANDLE InterfaceHandle;
    ULONG NumberOfPipes; 

    //
    // INPUT/OUPUT
    USBD_PIPE_INFORMATION Pipes[1];

} USBD_INTERFACE_INFORMATION, *PUSBD_INTERFACE_INFORMATION;

/*----------------------------------------------------*/
typedef struct _USB_ENDPOINT_DESCRIPTOR 
{
    UCHAR	bLength;
    UCHAR	bDescriptorType;
    UCHAR	bEndpointAddress;
    UCHAR	bmAttributes;
    USHORT	wMaxPacketSize;
    UCHAR	bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

/*----------------------------------------------------*/
typedef struct _USB_INTERFACE_DESCRIPTOR 
{
    UCHAR	bLength;
    UCHAR	bDescriptorType;
    UCHAR	bInterfaceNumber;
    UCHAR	bAlternateSetting;
    UCHAR	bNumEndpoints;
    UCHAR	bInterfaceClass;
    UCHAR	bInterfaceSubClass;
    UCHAR	bInterfaceProtocol;
    UCHAR	iInterface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

/*----End of USB Driver's specific information.--*/


/*-----------------------------------------------------------------------------
	Boolean		seUsbTransportSetup(void);

	Sets up the Usb Transport Layer by Opening a Driver. 

	Returns False when the driver could not be open
			True  when the driver indicates that Device is plugged in
			Device Handle is kept for better performance when the 
			KEEP_DEVICE_HANDLE flag is set.
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	void seUsbTransportGetPipeInfo(void)
	Prints out Pipe information
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Boolean		seUsbTransportGetConfig(void);
	Prints out the Device Configuration
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	UInt32		seUsbTransportPktOut( seCSCL_PACKET_TYPE type, PVOID pBuffer, int length);
	Sends a packet to the Out endpoint. 
	Compiles a trivial packet if it is nessesary.
	Arguments:
	type		---- Prometheus CSCL packet type
	pBuffer		---- Buffer pointer for the assembled packet
	length      ---- Length in bytes

	Returns number of bytes sent

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------

	Boolean		seUsbTransportIntPktIn ( PVOID pBuffer, int length );
	Receives an Interrupt packet from the In endpoint. 

	Arguments:
	pBuffer		---- pointer to an available Buffer
	length      ---- Buffer Length in bytes

	Returns number of bytes received

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	UInt32		seUsbTransportPktIn ( PVOID pBuffer, int length );
	Receives a Prometheus packet from the In endpoint. 

	Arguments:
	pBuffer		---- pointer to an available Buffer
	length      ---- Buffer Length in bytes

	Returns number of bytes received

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------

	Boolean seUsbTransportToggleVerbose(void);

	Toggles a Verbose mode for Usb Transport Layer

	Returns TRUE	if Verbose mode is set
			FALSE	if VErbose mode is not set
------------------------------------------------------------------------------*/


#endif // seUSB_TRANSPORT_H_INCLUDED