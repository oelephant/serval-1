//----------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2007. All rights reserved.
//----------------------------------------------------------------------------
#ifndef seUSBBRIDGE_H_INCLUDED
#define seUSBBRIDGE_H_INCLUDED


#define USB_ERR_NONE					0		// No error
#define USB_ERR_ADAPTER_NOT_FOUND		1		// USB adapter not found
#define USB_ERR_DRIVER_NOT_FOUND		2		// USB driver not found
#define USB_ERR_IRQ_FAILURE			    3		// USB IRQ failure
#define USB_ERR_ADAPTER_NOT_MAPPED		4		// USB adapter not mapped
#define USB_ERR_UNKNOWN_ERROR			5		// Unknown error - MUST BE LAST!!!

typedef void (*USB_CALLBACK)(void);

int usbHookInterrupt( USB_CALLBACK pISRCallback );
int usbbGetLastError( void );
#endif // seUSBBRIDGE_H_INCLUDED