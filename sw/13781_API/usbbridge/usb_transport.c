//----------------------------------------------------------------------------
//
//  (C)SEIKO EPSON CORPORATION 2005-2007. All rights reserved.
//----------------------------------------------------------------------------
#include "usb_transport.h"
#include "s1d13xxxUSB.h"
#include "os.h"

/* Private Functions */

static	Boolean seUsbTransportOpenDriver(HANDLE * phDeviceHandle);
static	HANDLE	seUsbTransportOpenPipe(UINT pipeNum);
static	UInt32	seUsbTransportOut(int pipeNum, PVOID pbuffer, int length, Boolean lock);
static	UInt32	seUsbTransportIn(int pipeNum, PVOID pbuffer, int length, Boolean lock);
static	void	ParseStringDescriptor(PVOID pvBuffer);
static	void	DebugPktInfo(UInt8 *pbuffer, int length);
static	void	ParseConfigurationDescriptor(PVOID pvBuffer);
static	void	ParseStringDescriptor(PVOID pvBuffer);

/* Transport Layer Private Variables */

static	HANDLE  hDevice          =  NULL;       // Handle for ReadMge/WriteMge 
static 	HANDLE	hIntPipe         =  NULL;       // Handle for Interrupt message
static  HANDLE	hIntEvent        =  NULL;	    // Interrupt event
static	Boolean UsbTransportVerbose = 0;        // Allows to display packets
static  char pDriverName[]	     =	"ezusb-0";  // Driver's base name

/* End of Private Variables */

/* Usb Transport Layer Specific Pipe assignments */
/* 
	Following assignments are specific to the Cypress Device
	and should be replaced in case of other Device in use 
*/

#define  seCSCL_MGE_DATAOUT_PIPE		(0)
#define  seCSCL_MGE_DATAIN_PIPE			(2)
#define  seCSCL_MGE_INTERRUPT_PIPE		(3)

	
/*-----------------------------------------------------------------------------
	seUsbTransportSetup()
------------------------------------------------------------------------------*/
Boolean seUsbTransportSetup(void)
{
	EZUSB_DRIVER_VERSION DriverVersion = {0, };
	int nBytes = 0;    

	// Cleanup in case there were some handles opened earlier
    seUsbTransportReset();
    seUsbTransportInterruptsReset();

    if ( seUsbTransportOpenDriver (&hDevice) != TRUE)
	{
        //seASSERT( FALSE, ("Usb Transport: Failed to Open Driver.") );
        hDevice = NULL;
		return FALSE;
	}

	if ( DeviceIoControl (hDevice,
					IOCTL_EZUSB_GET_DRIVER_VERSION,
					&DriverVersion,
					sizeof (EZUSB_DRIVER_VERSION),
					&DriverVersion,
					sizeof (EZUSB_DRIVER_VERSION),
					(unsigned long *)&nBytes,
					NULL) )
	{
		if ( DriverVersion.MajorVersion >= 1 && DriverVersion.MinorVersion >=34)
			return TRUE;
		else
		{
		    seASSERT( FALSE, ("Usb Transport: Required upgrade of the S1D13xxxUSB driver to version 1.34 or higher.") );
			return FALSE;
		}
	}
    seASSERT( FALSE, ("Usb Transport: Driver Version is not available.") );

	return FALSE;

}
/*-----------------------------------------------------------------------------
	seUsbTransportReset()
------------------------------------------------------------------------------*/
Boolean seUsbTransportReset(void)
{
    Boolean bResult = FALSE;

    if (hDevice == NULL) 
    {
        return FALSE;
    }else 
    {
#if 0
        int pipenum = seCSCL_MGE_DATAOUT_PIPE;
        int nBytes  = 0;
        bResult = DeviceIoControl (hDevice,
                                IOCTL_Ezusb_RESET,
                                &pipenum,
                                sizeof(int),
                                NULL,
                                0,
                                (unsigned long *)&nBytes,
                                NULL);
        if ( ! bResult )
                return bResult;
        bResult = DeviceIoControl (hDevice,
                                IOCTL_Ezusb_RESETPIPE,
                                &pipenum,
                                sizeof(int),
                                NULL,
                                0,
                                (unsigned long *)&nBytes,
                                NULL);
#else
        bResult = TRUE;
#endif
        CloseHandle (hDevice); 
        hDevice = NULL;
    }
    return bResult;
}
/*-----------------------------------------------------------------------------
	seUsbTransportInterruptsSetup()
------------------------------------------------------------------------------*/
Boolean seUsbTransportInterruptsSetup(void)
{
	seUsbTransportInterruptsReset();
    if ((hIntPipe = seUsbTransportOpenPipe ( seCSCL_MGE_INTERRUPT_PIPE)) == (HANDLE)INVALID_HANDLE_VALUE)
    {        
        seASSERT( FALSE, ("Usb Transport: Failed to Open Interrupt Driver.") );
        hIntPipe = NULL;
		return FALSE;
    }

    // Create the ISR complete/unblock event handle.
	hIntEvent = CreateEvent( NULL, TRUE, FALSE, NULL );		// Manual reset, Initial State = non-signaled, Unnamed object

	if ( !hIntEvent )
    {
        seASSERT( FALSE, ("Usb Transport: Create Event failed.") );
        CloseHandle ( hIntPipe );
        hIntPipe = NULL;
        return FALSE;
    }
 
    return TRUE;

}
/*-----------------------------------------------------------------------------
	seUsbTransportInterruptsReset()
------------------------------------------------------------------------------*/
Boolean seUsbTransportInterruptsReset(void)
{
    Boolean bResult = FALSE;
    if (hIntPipe == NULL) 
    {
        return FALSE;
    }else
    {
#if 0 // HW reset the specific pipe
        int pipenum = seCSCL_MGE_INTERRUPT_PIPE;
        int nBytes  = 0;
    
        bResult = DeviceIoControl (hDevice,
                                IOCTL_Ezusb_RESET,
                                &pipenum,
                                sizeof(int),
                                NULL,
                                0,
                                (unsigned long *)&nBytes,
                                NULL);
        if ( ! bResult )
                return bResult;
        bResult = DeviceIoControl (hDevice,
                                IOCTL_Ezusb_RESETPIPE,
                                &pipenum,
                                sizeof(int),
                                NULL,
                                0,
                                (unsigned long *)&nBytes,
                                    NULL);
        
#else
        bResult = TRUE;
#endif
        CloseHandle (hIntPipe); 
        hIntPipe = NULL;

        if ( hIntEvent )
        {
            ResetEvent( hIntEvent );
            CloseHandle( hIntEvent );
            hIntEvent = NULL;
        }
    }
    return bResult;
}

/*-----------------------------------------------------------------------------
	seUsbTransportGetConfig()
------------------------------------------------------------------------------*/
Boolean seUsbTransportGetConfig(void)
{   
    Boolean bResult                         = FALSE;
    int     nBytes                          = 0;
    PVOID   pvBuffer                        = 0;
    ULONG   ulLength                        = 0;

	if (hDevice == NULL) 
	{
        seASSERT( FALSE, ("Usb Transport: Device is not opened.") );
        return FALSE;
    }

	/* The configuration descriptor is obtained using two separate calls.  The
	 first call is done to determine the size of the entire configuration descriptor,
	 and the second call is done with that total size specified.  */
	pvBuffer = malloc (sizeof (Usb_Configuration_Descriptor) + 128);			
	
    if ( pvBuffer == NULL)
    {    
        seASSERT( FALSE, ("Usb Transport: Memory Allocation failed in GetConfigDescriptor.") );
		return FALSE;
    }

    /* Get the first bytes of the configuration descriptor to determine the size of
	   the entire configuration descriptor*/
	bResult = DeviceIoControl (hDevice,
				IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR,
				pvBuffer,
				sizeof (Usb_Configuration_Descriptor),
				pvBuffer,
				sizeof (Usb_Configuration_Descriptor),
				(unsigned long *)&nBytes,
				NULL);
			
	if (bResult!=TRUE) 
    {
        seASSERT( FALSE, ("Usb Transport: Get Configuration Descriptor Failed.") );
		//CloseHandle (hDevice);
        //hDevice = NULL;
		free (pvBuffer);
		return FALSE;
	}
			
	ulLength = GET_CONFIG_DESCRIPTOR_LENGTH(pvBuffer);
			
	/* Now get the entire configuration descriptor */
	pvBuffer = realloc (pvBuffer, ulLength);
			
	if (pvBuffer) 
	{				
		bResult = DeviceIoControl (hDevice,
					IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR,
					pvBuffer,
					ulLength,
					pvBuffer,
					ulLength,
					(unsigned long *)&nBytes,
					NULL);
				
		if (bResult==TRUE) 
		{
			ParseConfigurationDescriptor(pvBuffer);
		} 
		else 
		{
            seASSERT( FALSE, ("Usb Transport: Usb Transport: GetConfigDescriptor Ioctl failed.") );
    		free (pvBuffer);
    		return FALSE;
		}
	} 
	else 
	{
        seASSERT( FALSE, ("Usb Transport: Memory Allocation failed in GetConfigDescriptor.") );
		//CloseHandle (hDevice); // Close the handle
        //hDevice = NULL;
		return FALSE;
	}

	free (pvBuffer); 
	return TRUE;
}


/*----------------------------------------------------------------------------
	seUsbTransportGetPipeInfo()
-----------------------------------------------------------------------------*/		
void seUsbTransportGetPipeInfo(void)
{
	char PIPE_TYPE_STRINGS[4][4] = {"CTL","ISO","BLK","INT"};
	char PIPE_DIRECTION[2][4] = {"OUT","IN "};
	PUSBD_PIPE_INFORMATION pPipe;
	PUSBD_INTERFACE_INFORMATION pInterface;
	UCHAR InterfaceInfo[1024];
    Boolean bResult                         = FALSE;
    int     nBytes                          = 0;
			
	if (hDevice != NULL) 
	{
		bResult = DeviceIoControl (hDevice,
				IOCTL_Ezusb_GET_PIPE_INFO,
				NULL,
				0,
				InterfaceInfo,
				sizeof(InterfaceInfo),
				(unsigned long *)&nBytes,
				NULL);
	}
				
	if (bResult==TRUE)
	{
		int i;
		pInterface = (PUSBD_INTERFACE_INFORMATION) InterfaceInfo;
		pPipe = pInterface->Pipes;
		if (UsbTransportVerbose)
		{
			for (i = 0; i < (int) pInterface->NumberOfPipes; i++)
			{
				seCSCLPrintf ("\nUsb Transport: \nPipe: %2d   Type: %s Endpoint: %2d %s  MaxPktSize: 0x%x",i,PIPE_TYPE_STRINGS[pPipe[i].PipeType],(pPipe[i].EndpointAddress & 0x0F),
					PIPE_DIRECTION[(pPipe[i].EndpointAddress >> 7)],pPipe[i].MaximumPacketSize);
			}
		}
	}
	else
	{
		seASSERT( FALSE, ("Usb Transport: Get Pipe Info failed."));	
	}

		
} 

/*-----------------------------------------------------------------------------
	seUsbTransportToggleVerbose)
-----------------------------------------------------------------------------*/
Boolean seUsbTransportToggleVerbose(void)
{
	UsbTransportVerbose ^= TRUE;
	return UsbTransportVerbose;
}


/*----------------------------------------------------------------------------
	seUsbTransportPktOut()
------------------------------------------------------------------------------*/
UInt32 seUsbTransportPktOut( seCSCL_PACKET_TYPE type, PVOID pBuffer, int length, Boolean lock )
{
	if ( pBuffer == NULL)
	{
		seCsclHeader sePktHdr = {0,};
		sePktHdr.byte0 = (unsigned char)type;
		/* Send Header Only packet */
		return( seUsbTransportOut( seCSCL_MGE_DATAOUT_PIPE, (UInt8 *)&sePktHdr,2, lock));	
	}
	else
	{
		return(seUsbTransportOut( seCSCL_MGE_DATAOUT_PIPE, pBuffer,length, lock ));
	}
}

/*----------------------------------------------------------------------------
	seUsbTransportPktIn ()
------------------------------------------------------------------------------*/
UInt32 seUsbTransportPktIn ( UInt8 *pBuffer, int length, Boolean lock )
{

	if ( pBuffer == NULL)
	{
		seASSERT( FALSE, ("Usb Transport: No buffer available for seUsbTransportIn transaction."));
		return 0;
	}
	else
	{
		return( seUsbTransportIn( seCSCL_MGE_DATAIN_PIPE, pBuffer,length, lock));
	}
}
#include "os.h"

/*----------------------------------------------------------------------------
	seUsbTransportIntPktIn ( ) 
------------------------------------------------------------------------------*/
static volatile int gIRQOK;

VOID CALLBACK FileIOCompletionRoutine(
DWORD dwErrorCode,                // completion code
DWORD dwNumberOfBytesTransfered,  // number of bytes transferred
LPOVERLAPPED lpOverlapped)         // I/O information buffer
{
    dwErrorCode;
    lpOverlapped;

    if (dwNumberOfBytesTransfered != 0)
        gIRQOK = 1;
    else
        gIRQOK = 2;
}

Boolean seUsbTransportIntPktIn ( UInt8 *pBuffer, int length ) 
{
    OVERLAPPED	OverLapped = {0, };
    DWORD status;

    if ( (pBuffer == NULL) || (length == 0))
    {
        seASSERT( FALSE, ("Usb Transport: No buffer available for Interrupt seUsbTransportIn transaction"));
        return FALSE;
    }

    memset(pBuffer, 0, length);

    gIRQOK = 0;
    status = ReadFileEx(hIntPipe, pBuffer, length, &OverLapped, FileIOCompletionRoutine);

    /* The SleepEx() function returns when either the time-out period has elapsed or 
    when an I/O completion callback function occurs*/ 
    SleepEx(1000, TRUE); 

    if (status != 0 && gIRQOK == 1)
        return TRUE;
    else
        return FALSE;
}

/*-----------------------------------------------------------------------------
	Static Functions 
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	DebugPktInfo()
------------------------------------------------------------------------------*/
static void DebugPktInfo( UInt8 * pbuffer, int length)
{
	int i = 0;

	if (!UsbTransportVerbose) 
		return;
		
	seCSCLPrintf ( "\nUsb Transport: PKT << 0x%02x 0x%02x", pbuffer[0],pbuffer[1]);

	seCSCLPrintf ("\n len %d", length);
	length = (length > 16) ? 16 : length;
	for ( i = 2; i < length; i++)
	{
		if ( (i-2)%8 == 0) seCSCLPrintf ("\n");
			seCSCLPrintf ( "0x%02x ", pbuffer[i]);
	}

	seCSCLPrintf( ">>\n");
}
/*-----------------------------------------------------------------------------
	seUsbTransportOpenPipe()
-----------------------------------------------------------------------------*/
static HANDLE seUsbTransportOpenPipe( UINT pipeNum)
{
	HANDLE hPipe;
    char PipeName[64] = "";
    char Buffer[32];

    strcat (PipeName, "\\\\.\\");
    strcat (PipeName, pDriverName);
	sprintf(Buffer, "\\PIPE0%d",  pipeNum);
    strcat (PipeName,Buffer);

   	if ( GetEnvironmentVariable("S1DBOARD",Buffer,sizeof(Buffer)) )
	{
		if ( Buffer[0] >= '0' && Buffer[0] < '8' )
		{
			PipeName[10] = Buffer[0]; 
		}
	}

	if (UsbTransportVerbose)
	    seCSCLPrintf( "\nUsb Transport: PipeName %s", PipeName );

	hPipe = CreateFile(PipeName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

    if ( hPipe == INVALID_HANDLE_VALUE )
        seASSERT( FALSE, ("Open Pipe failed %x", GetLastError()));

	return hPipe;
}

/*-----------------------------------------------------------------------------

  seUsbTransportOpenDriver() 
  
Opens the device driver using symbolic name provided                             
                                                 
Arguments                                         
phDeviceHandle:                           
Pointer to Device Driver handle where the file handle is placed.            
devname:                                 
Null terminated string containing the device name                           
                                                 
 Return Value:                                 
Boolean that indicates if the driver was  successfully opened or not.               

-----------------------------------------------------------------------------*/
static Boolean seUsbTransportOpenDriver (HANDLE * phDeviceHandle)
{    
    char completeDeviceName[64] = "";
    char Buffer[32];
    
    // Check the environment for: S1DBOARD=n
	// This is for rare situations where there are multiple USB boards and a command prompt window
	// can be assigned to a particular PCI board using the environment variable.

	if ( GetEnvironmentVariable("S1DBOARD",Buffer,sizeof(Buffer)) )
	{
		if ( Buffer[0] >= '0' && Buffer[0] < '8' )
		{
			pDriverName[6] = Buffer[0]; 
		}

        if (UsbTransportVerbose)
	    {
		    seCSCLPrintf( "env var: %s ", Buffer);
        }
	}

    strcat (completeDeviceName,
            "\\\\.\\"
            );

    strcat (completeDeviceName,
		    pDriverName
		    );

    *phDeviceHandle = CreateFile(   completeDeviceName,
		                            GENERIC_WRITE,
		                            FILE_SHARE_WRITE,
		                            NULL,
		                            OPEN_EXISTING,
		                            0,
		                            NULL);

    if (*phDeviceHandle == INVALID_HANDLE_VALUE) {
        seCSCLPrintf( "Device %s failed to open\n", completeDeviceName);
        return (FALSE);
    } else {
        return (TRUE);
    } 


}
/*-----------------------------------------------------------------------------
  ParseConfigurationDescriptor()                                                           
-----------------------------------------------------------------------------*/
static void ParseConfigurationDescriptor(PVOID pvBuffer)
{
    pUsb_Configuration_Descriptor pCD = (pUsb_Configuration_Descriptor) pvBuffer;
    PUSB_INTERFACE_DESCRIPTOR     pID = NULL; 
    PUSB_ENDPOINT_DESCRIPTOR      pED = NULL;
    
    int                    i         = 0;
  
	if (UsbTransportVerbose)
	{
		seCSCLPrintf( "\nConfig Descriptor:  ");
		seCSCLPrintf( "\nbLength: 0x%x", pCD->bLength);
		seCSCLPrintf( "\nbDescriptorType:  %d", pCD->bDescriptorType); 
		seCSCLPrintf( "\nwTotalLength:  %d (0x%x)", pCD->wTotalLength, pCD->wTotalLength); 
		seCSCLPrintf( "\nbNumInterfaces:  %d", pCD->bNumInterfaces);  
		seCSCLPrintf( "\nbConfigurationValue:  %d", pCD->bConfigurationValue);  
		seCSCLPrintf( "\niConfiguration:  %d", pCD->iConfiguration);
		seCSCLPrintf( "\nbmAttributes: 0x%x", pCD->bmAttributes);
		seCSCLPrintf( "\nMaxPower:  %d", pCD->MaxPower);
		seCSCLPrintf( "\n\n");
  

		for( pID = (PUSB_INTERFACE_DESCRIPTOR) (((char*)pCD) + pCD->bLength); 
			pID < (PUSB_INTERFACE_DESCRIPTOR) (((char*)pCD) + pCD->wTotalLength);
		pID->bLength) // Note: Quits if pointer is not growing - i.e. pID->bLength == 0
		{ 

			seCSCLPrintf( "\nInterface Descriptor:  ");  
			seCSCLPrintf( "\n--------------------------------");     
			seCSCLPrintf( "\nbLength:  0x%x", pID->bLength); 
			seCSCLPrintf( "\nbDescriptorType:  %d", pID->bDescriptorType);
			seCSCLPrintf( "\nbInterfaceNumber:  %d", pID->bInterfaceNumber);  
			seCSCLPrintf( "\nbAlternateSetting:  %d", pID->bAlternateSetting); 
			seCSCLPrintf( "\nbNumEndpoints:  %d", pID->bNumEndpoints);
			seCSCLPrintf( "\nbInterfaceClass:  %d (0x%x)", pID->bInterfaceClass, pID->bInterfaceClass);  
			seCSCLPrintf( "\nbInterfaceSubClass:  %d (0x%x)", pID->bInterfaceSubClass, pID->bInterfaceSubClass);
			seCSCLPrintf( "\nbInterfaceProtocol:  %d (0x%x)", pID->bInterfaceProtocol, pID->bInterfaceProtocol);  
			seCSCLPrintf( "\niInterface:  %d", pID->iInterface);
			seCSCLPrintf("\n\n");

			pED = (PUSB_ENDPOINT_DESCRIPTOR) (((char*)pID) + pID->bLength);

			for (i=0;i<pID->bNumEndpoints;i++) {
				seCSCLPrintf( "\nEndpoint Descriptor %d  ", i);
				seCSCLPrintf("\n--------------------------------");
				seCSCLPrintf("\nbLength:  0x%x", pED->bLength); 
				seCSCLPrintf( "\nbDescriptorType:  %d", pED->bDescriptorType);
				seCSCLPrintf("\nbEndpointAddress:  0x%x", pED->bEndpointAddress);   
				seCSCLPrintf( "\nbmAttributes:  0x%x", pED->bmAttributes);
				seCSCLPrintf("\nwMaxPacketSize:  %d", pED->wMaxPacketSize);
				seCSCLPrintf( "\nbInterval:  %d", pED->bInterval);   
				seCSCLPrintf( "\n\n");
   
				pED = (PUSB_ENDPOINT_DESCRIPTOR) (((char*)pED) + (pED->bLength));

			}
			pID = (PUSB_INTERFACE_DESCRIPTOR) (char*)pED; // pointer should grow
		} 
	}
}

/*----------------------------------------------------------------------------
	UInt32 seUsbTransportIn()
------------------------------------------------------------------------------*/
static UInt32 seUsbTransportIn(int pipeNum, UInt8 *pbuffer, int length, Boolean lock )
{
	BULK_TRANSFER_CONTROL bulkControl;
    Boolean bResult = FALSE;
    int nBytes  = 0;
    
	bulkControl.pipeNum = pipeNum;
			
    if (hDevice != NULL)
    {
		DWORD ioctl_val = ( lock ) ? IOCTL_EZUSB_BULK_READ_LOCK : IOCTL_EZUSB_BULK_READ;
		bResult = DeviceIoControl (hDevice,
						ioctl_val, 
						&bulkControl,
						sizeof (BULK_TRANSFER_CONTROL),
						pbuffer,
						length,
						(unsigned long *)&nBytes,
						NULL);
					
		if( (bResult == TRUE) ) 
		{
			DebugPktInfo( pbuffer, nBytes);
		}
		else 
		{
			seASSERT( FALSE, ("Usb Transport: USB T: Read IOCTL failed: NACK %d", GetLastError()));
		}
		//pTh->length = nBytes;
	}

	return nBytes;
}
/*----------------------------------------------------------------------------
	seUsbTransportOut()
------------------------------------------------------------------------------*/
static UInt32 seUsbTransportOut(int pipeNum, UInt8 *pbuffer, int length, Boolean lock){
	BULK_TRANSFER_CONTROL bulkControl;
    Boolean bResult = FALSE;
    int     nBytes  = 0;

    bulkControl.pipeNum = pipeNum;
			
    if (hDevice != NULL)
    {
		DWORD ioctl_val = ( lock ) ? IOCTL_EZUSB_BULK_WRITE_LOCK : IOCTL_EZUSB_BULK_WRITE;
		bResult = (unsigned char)DeviceIoControl (hDevice,
						ioctl_val, 
						&bulkControl,
						sizeof (BULK_TRANSFER_CONTROL),
						pbuffer,
						length,
						(unsigned long *)&nBytes,
						NULL);
					
		if( (bResult == TRUE) ) 
		{
			DebugPktInfo( pbuffer, length);
		} 
		else 
		{	
			seASSERT( FALSE, ("Usb Transport: Write IOCTL failed %d", GetLastError()));
		}
		
	}

	return nBytes;
}
