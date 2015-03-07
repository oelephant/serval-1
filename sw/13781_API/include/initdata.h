////////////////////////////////////////////////////////////////////////////////
///
/// \file INITDATA.H
///
/// \par
///		This file contains a the definition of the seHclData structure which is
///		used to configure the API for different hardware and modes of operation.
///		This structure allows CFG.EXE to reconfigure the API in the sample programs
///		after they have been compiled.
///
//	©SEIKO EPSON CORPORATION 2009-2010. All rights reserved. 
////////////////////////////////////////////////////////////////////////////////

#ifndef seINITDATA_H_INCLUDED
#define seINITDATA_H_INCLUDED

#include "datatype.h"							// Grab public definitions and data types.


#define seHCL_REGSIZE		593					///< Estimated maximum number of registers in seHclData.
#define seHCL_LCD1SIZE		768					///< Estimated maximum number of LCD1 entries in seHclData.
#define seHCL_LCD2SIZE		768					///< Estimated maximum number of LCD2 entries in seHclData.
#define seHCL_I2CSIZE		256					///< Estimated maximum number of I2C entries in seHclData.
#define seHCL_I2C2SIZE		256					///< Estimated maximum number of I2C entries in seHclData.

#pragma pack(1)
////////////////////////////////////////////////////////////////////////////////
///
/// \brief 
///		This structure stores register address and value pairs and is used to configure
///		All the registers to a known state after reset.
///
////////////////////////////////////////////////////////////////////////////////
typedef struct									///< Register data structure
{
	UInt16		Index;							///< Register index
	UInt16		Value;							///< Register data
} seHclRegData;


////////////////////////////////////////////////////////////////////////////////
///
/// \brief 
///		This structure is used to store I2C programming sequences for configuration
///		of I2C devices after reset.
///
///	\see I2cInitCamera 
///	\see I2cStandardWrite 
///	\see I2cAgilentProcessorWrite 
////////////////////////////////////////////////////////////////////////////////
typedef struct									///< I2C data structure
{						
	UInt16		Type;							///< I2C command type (see seHCL_I2C_xxx definitions, below)
	UInt16		Index;							///< I2C index (if applicable)
	UInt16		Data;							///< I2C data value
} seHclI2CData;


////////////////////////////////////////////////////////////////////////////////
///
/// \brief 
///		This structure contains the runtime configuration options for the API.
///		The CFG.EXE program can be used to reconfigure the API settings in the
///		sample programs without recompiling.  Compile time options are configured
///		in settings.h file
///
///		#### DEVELOPER WARNING: CHANGES HERE MUST BE REFLECTED IN CHIPDEF ####
///
////////////////////////////////////////////////////////////////////////////////
typedef struct									///< HCL initialization structure
{
	char		szIdString[16];					///< HCL ID string
	char		szConfigString[256];			///< Configuration description
	char		sLCD1StringPool[256];			///< Parallel LCD1 strings pool in {index,"string"} format
	UInt16		Size;							///< Sizeof this structure
	UInt16		HclCRC;							///< CRC of the rest of structure

	char		szChipId[8];					///< S1D chip ID
	UInt16		DetectEndian;					///< Endian detect (LE=0x1234)
	UInt16		Reserved1;						///< Reserved for alignment

	UInt32		ClkI;							///< CLKI frequency (in Hz)

	UInt8		USBConfig[6];					///< USB Configuration Registers

	UInt32		BaseAddress;					///< Physical base memory address
	UInt32		RegisterOffset;					///< Physical register address offset form base address
	UInt32		MemoryOffset;					///< Physical memory address offset form base address

	UInt32		Flags;							///< Hcl information flags

	UInt32		LCD1Regs[seHCL_LCD1SIZE];		///< Parallel LCD register initialization sequence (LCD1)

	seHclRegData Regs[seHCL_REGSIZE];			///< Initialization registers
} seHclData;
#pragma pack()

// Definitions for seHclData.Flags
#define sefUSING_FPGA			0x00000001UL	///< True if using an FPGA, not the real chip.
#define sefNO_RESET				0x00000002UL	///< True if software reset is not needed during chip init.
#define sefDEBUG_REG_WRITES		0x00000004UL	///< True if HCL debugging of register writes is enabled (not supported for API)

#define sefADAPTER_USB			0x00000010UL	///< True if the adapter type is USB.
#define sefADAPTER_GENERIC		0x00000020UL	///< True if the adapter type is generic, can not coexist with sefADAPTER_USB.

#define sefNO_INIT_GPIO			0x00001000UL	///< True if not configuring GPIO pins during chip init.
#define sefNO_INIT_I2C			0x00002000UL	///< True if not supporting I2C bus on GPIO pins or hardware (global enable/disable).
#define sefSOFTWARE_I2C			0x00004000UL	///< True if I2C is done in software using GPIO pins.

#define sefSOFT_BYTESWAP		0x00010000UL	///< True if bus data is byte swapped (not supported for API).
#define sefHOST_AUTO			0x00020000UL	///< True if Host Auto detect mode.

// Definitions for seHclData.Cam1Data.Type options
#define seHCL_I2C_ENDOFTABLE	0				///< End of camera I2C data when an entry EQUALS this value
#define seHCL_I2C_DEVICE_ID		1				///< This entry sets the I2C slave device ID (data field)
#define seHCL_I2C_DELAY			2				///< This entry is a millisecond delay (data field)
#define seHCL_I2C_STDWRITE		3				///< This entry is a standard I2C (index,data) write command
#define seHCL_I2C_AGPWRITE		4				///< This entry is an Agilent Processor I2C (index,data) write command
#define seHCL_I2C_AGSWRITE		5				///< This entry is an Agilent Sensor I2C (index,data) write command

// Definitions for seHclData.LCD1Regs and seHclData.LCD2Regs and seHclData.LCD3Regs
#define seHCL_LCD_ENDOFTABLE	0x00000000UL	///< End of parallel LCD list when an entry EQUALS this value
#define seHCL_LCD_16BIT 		0x04000000UL	///< True if this entry is a serial 16 BIT LCD command
#define seHCL_LCD_24BIT 		0x08000000UL	///< True if this entry is a serial 24 BIT LCD command
#define seHCL_LCD_COMMAND		0x10000000UL	///< True if this entry is a parallel LCD command
#define seHCL_LCD_PARAM			0x20000000UL	///< True if this entry is a parallel LCD parameter (data)
#define seHCL_LCD_DELAY			0x40000000UL	///< True if this entry is a microsecond delay value
#define seHCL_LCD_STARTOFTBL	0x80000000UL	///< Start of parallel LCD list when an entry EQUALS this value
#define seHCL_LCD_FLAGMASK		0xFF000000UL	///< Parallel LCD flag mask
#define seHCL_LCD_DATAMASK		0x00FFFFFFUL	///< Parallel LCD data mask

// Definitions for seHclData.DetectEndian (DEPRECATED FOR API)
#define seHCL_LITTLE_ENDIAN		0x1234
#define seHCL_BIG_ENDIAN		0x3412


#ifdef __cplusplus
   extern "C" {
#endif

extern seHclData segHclInfo;					// Export segHclInfo for application usage

#ifdef __cplusplus
   }
#endif


#endif // seINITDATA_H_INCLUDED
