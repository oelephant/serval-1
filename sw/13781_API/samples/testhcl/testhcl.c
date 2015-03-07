// TESTHCL.C - Test Basic HCL functions.


#include <stdio.h>
#include <stdlib.h>
#include "seapi.h"
#include "os.h"


#define TEST_REG	REG96_BLT_BGCOLOR_0

static UInt16	*gpBigBuf16;
static const int gnBigBuf16 = 32768;
static UInt8	*gpBigBuf8;
static const int gnBigBuf8 = 65536;

static UInt8	gaFill8[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
static UInt16	gaFill16[8] = { 0x1100, 0x3322, 0x5544, 0x7766, 0x9988, 0xBBAA, 0xDDCC, 0xFFEE };
static UInt32	gaFill32[4] = { 0x33221100, 0x77665544, 0xBBAA9988, 0xFFEEDDCC };

static Int32	gSeed = 1;



static void		ShowRegisters( void );
static void		TestVSyncPolling( void );
static void		TestReadWriteReg( void );
static void		TestMemoryPatterns( void );
static void		TestBurstReadMemory( void );
static void		TestBurstWriteMemory( void );
static void		TestAllMemory( void );
static void		TestPeriodicTimers( void );
static void		TestTimerProc( void* pParam );
static void		TestBitRegApi( void );
static Int32	rand32( void );



int main( int argc, char *argv[] )
{
	Boolean fContinuous = (argc > 1 && argv[1][0]=='-' && argv[1][1]=='c');

	gpBigBuf16 = malloc( gnBigBuf16*2 );
	gpBigBuf8 = (UInt8*)gpBigBuf16;

	if ( gpBigBuf16 )
	{
		if ( seAcquireController() && seInitializeController(seINIT_NORMAL) )
		{
			ShowRegisters();

			do
			{
				TestVSyncPolling();
				TestReadWriteReg();
				TestMemoryPatterns();
				TestBurstReadMemory();
				TestBurstWriteMemory();
				TestAllMemory();
				TestPeriodicTimers();
				TestBitRegApi();
			} while ( fContinuous );
		}
		else
			printf( "Cannot acquire/initialize controller.\n" );

		free( gpBigBuf16 );
	}
	else
		printf( "Cannot allocate large test buffer.\n" );

	return 0;
}



static void ShowRegisters( void )
{
	int i, c;

	printf( "Initialized Registers\n" );

	printf( "%02X=%04X", REG00_REV_CODE,  seReadReg16(REG00_REV_CODE) );
	printf( " " );
	printf( "%02X=%04X", REG02_PROD_CODE, seReadReg16(REG02_PROD_CODE) );

	for ( i = 0, c = 3; segHclInfo.Regs[i].Index != REGFLAG_END_OF_TABLE; i++ )
	{
		if ( segHclInfo.Regs[i].Index < REGFLAG_BASE )
		{
			UInt32 Value = seReadReg16( segHclInfo.Regs[i].Index );
			if ( c >= 10 )
				printf( "\n" ), c = 0;
			if ( c++ )
				printf( " " );
			printf( "%02X%s%04X", segHclInfo.Regs[i].Index, (Value==segHclInfo.Regs[i].Value)?"=":"*", Value );
		}
	}

	printf("\n\nSYSCLK = %dHz\n\n", seClkGetSysClk() );

}



static void TestVSyncPolling( void )
{
	int i;

	seModifyReg16( REG22_DISP_SET, 0x0060, 1<<5 ); // set TE Function such that TE output pin is high during VNDP

	printf( "Test sePollWhileLow/HighReg()...\n" );

	for ( i = 0; i < 79; i++ )
	{
		if ( !sePollWhileHighReg16(REG22_DISP_SET,0x0080,0,500) )
			break;
		if ( !sePollWhileLowReg16(REG22_DISP_SET,0x0080,0,500) )
			break;
		printf( "." );
	}

	printf( "\nVREF polling %s.\n\n", (i<79)?"TIMED OUT":"OK" );
}



static void TestReadWriteReg( void )
{
	Boolean fPassed = TRUE;
	UInt32 OriginalValue = seReadReg16( TEST_REG );

	printf("Test seReadReg16/seWriteReg16\n");
	seWriteReg16( TEST_REG, 0x00AB );

	if (seReadReg16( TEST_REG )!=0x00AB)
		fPassed = FALSE;

	printf("Test seReadReg8/seWriteReg8\n");
	seWriteReg8( TEST_REG, 0x55 );

	if (seReadReg8( TEST_REG )!=0x55)
		fPassed = FALSE;

	seWriteReg8( TEST_REG, 0xAA );

	if (seReadReg8( TEST_REG )!=0xAA)
		fPassed = FALSE;


	printf( "Test %s.\n\n", fPassed?"OK":"FAILED" );

	seWriteReg16( TEST_REG, OriginalValue );
}



static void TestMemoryPatterns( void )
{
	int i, n;

	Boolean fPassed = TRUE;
	printf( "Test seFillMemory(), seReadMemory(), seWriteMemory() using patterns...\n" );

	for ( n = 0; n < 3; n++ )
	{
		seFillMemory16( 0, 0x0000, 8 );
		for ( i = 0; i < 16; i+=2 )
			if ( seReadMemory16(i) != 0x0000 )
			{
				fPassed = FALSE;
				break;
			}
		if ( i < 16 )
		{
			printf( "Prefill and read of 0000h failed, %d-bit pass.\n", 1<<(n+3) );
			break;
		}

		switch ( n )
		{
		  case 0:
			for ( i = 0; i < 16; i++ )
				seWriteMemory8( i, gaFill8[i] );
			break;
		  case 1:
			for ( i = 0; i < 8; i++ )
				seWriteMemory16( i*2, gaFill16[i] );
			break;
		  case 2:
			for ( i = 0; i < 4; i++ )
				seWriteMemory32( i*4, gaFill32[i] );
			break;
		}

		for ( i = 0; i < 16; i++ )
			if ( seReadMemory8(i) != gaFill8[i] )
				break;
		if ( i < 16 )
		{
			printf( "seReadMemory8() failed, %d-bit pass.\n", 1<<(n+3) );
			fPassed = FALSE;
			break;
		}

		for ( i = 0; i < 8; i++ )
			if ( seReadMemory16(i*2) != gaFill16[i] )
				break;
		if ( i < 8 )
		{
			printf( "seReadMemory16() failed, %d-bit pass.\n", 1<<(n+3) );
			fPassed = FALSE;
			break;
		}

		for ( i = 0; i < 4; i++ )
			if ( seReadMemory32(i*4) != gaFill32[i] )
				break;
		if ( i < 4 )
		{
			printf( "seReadMemory32() failed, %d-bit pass.\n", 1<<(n+3) );
			break;
		}
	}

	printf( "Test %s.\n\n", fPassed?"OK":"FAILED" );
}



static void TestBurstReadMemory( void )
{
	int		i, c;
	UInt8	aData8[16];
	UInt16	aData16[8];
	UInt32	aData32[4];

	printf( "Test seBurstReadMemory() using patterns...\n" );

	for ( i = 0; i < 8; i++ )
		seWriteMemory16( i*2, gaFill16[i] );

	seBurstReadMemory8(  0, aData8, 16 );
	seBurstReadMemory16( 0, aData16, 8 );
	seBurstReadMemory32( 0, aData32, 4 );
	c = 0;

	for ( i = 0; i < 16; i++ )
		if ( aData8[i] != gaFill8[i] )
			break;
	if ( i < 16 )
	{
		printf( "seBurstReadMemory() 8-bit failed.\n" );
		c++;
	}

	for ( i = 0; i < 8; i++ )
		if ( aData16[i] != gaFill16[i] )
			break;
	if ( i < 8 )
	{
		printf( "seBurstReadMemory() 16-bit failed.\n" );
		c++;
	}

	for ( i = 0; i < 4; i++ )
		if ( aData32[i] != gaFill32[i] )
			break;
	if ( i < 4 )
	{
		printf( "seBurstReadMemory() 32-bit failed.\n" );
		c++;
	}

	printf( "Test %s.\n\n", (c)?"FAILED":"OK" );
}



static void TestBurstWriteMemory( void )
{
	int i, n;
	Boolean fPassed = TRUE;

	printf( "Test seBurstWriteMemory() using patterns...\n" );

	for ( n = 0; n < 3; n++ )
	{
		switch ( n )
		{
		  case 0:
			seFillMemory8( 0, 0x0000, 16 );
			break;
		  case 1:
			seFillMemory16( 0, 0x0000, 8 );
			break;
		  case 2:
			seFillMemory32( 0, 0x0000, 4 );
			break;
		}

		for ( i = 0; i < 16; i+=2 )
			if ( seReadMemory16(i) != 0x0000 )
				break;
		if ( i < 16 )
		{
			fPassed = FALSE;
			printf( "Prefill and read of 0000h failed, %d-bit pass.\n", 1<<(n+3) );
			break;
		}

		switch ( n )
		{
		  case 0:
			seBurstWriteMemory8(  0, gaFill8, 16 );
			break;
		  case 1:
			seBurstWriteMemory16( 0, gaFill16, 8 );
			break;
		  case 2:
			seBurstWriteMemory32( 0, gaFill32, 4 );
			break;
		}

		for ( i = 0; i < 8; i++ )
			if ( seReadMemory16(i*2) != gaFill16[i] )
				break;
		if ( i < 8 )
		{
			fPassed = FALSE;
			printf( "seBurstWriteMemory() failed, %d-bit pass.\n", 1<<(n+3) );
			break;
		}
	}

	printf( "Test %s.\n\n", fPassed?"OK":"FAILED" );
}



static void TestAllMemory( void )
{
	int i, c, n;

	printf( "Test all memory with pseudo-random sequence..." );

	for ( n = 0, c = 0; n < 2; n++ )
	{
		UInt32 MemSize = seVMEM_SIZE;
		UInt32 SeedOld;
		UInt32 Offset;

		printf("\nCycle #%d Write", n+1 );

		SeedOld = gSeed;
		for ( Offset = 0; Offset < MemSize; Offset += gnBigBuf16*2 )
		{
			for ( i = 0; i < gnBigBuf16; i++ )
				gpBigBuf16[i] = (UInt16)rand32();
			seBurstWriteMemory16( Offset, gpBigBuf16, gnBigBuf16 );
			printf( "." );
		}

		printf("\nCycle #%d Read", n+1 );
		
		gSeed = SeedOld;
		for ( Offset = 0; Offset < MemSize; Offset += gnBigBuf16*2 )
		{
			seBurstReadMemory16( Offset, gpBigBuf16, gnBigBuf16 );
			for ( i = 0; i < gnBigBuf16; i++ )
				if ( gpBigBuf16[i] != (UInt16)rand32() )
					c++;
			printf( "." );
		}
	}			

	printf( "\nTest had %d mismatches - %s.\n\n", c, (c)?"BAD":"GOOD" );
}



static void TestPeriodicTimers( void )
{
	Handle	aTimers[3];
	int		aCounters[3];
	Boolean	fGood;

	printf( "Test periodic timers...\n" );

	aCounters[0] = aCounters[1] = aCounters[2] = 0;
	aTimers[0] = seOsCreatePeriodicTimer( 200, TestTimerProc, &aCounters[0] );
	aTimers[1] = seOsCreatePeriodicTimer( 500, TestTimerProc, &aCounters[1] );
	aTimers[2] = seOsCreatePeriodicTimer( 1000, TestTimerProc, &aCounters[2] );

	seOsSleepMS( 4200 );

	fGood = (aCounters[0]>=20 && aCounters[1]>=8 && aCounters[2]>=4);
	printf("\nCounters = %d, %d, %d - %s\n", aCounters[0], aCounters[1], aCounters[2], (fGood)?"GOOD":"BAD" );

	seOsDestroyPeriodicTimer( aTimers[0] );
	seOsDestroyPeriodicTimer( aTimers[1] );
	seOsDestroyPeriodicTimer( aTimers[2] );
	aCounters[0] = aCounters[1] = aCounters[2] = 0;

	seOsSleepMS( 1200 );
	fGood = (aCounters[0]==0 && aCounters[1]==0 && aCounters[2]==0);

	printf("Counters = %d, %d, %d - %s\n\n", aCounters[0], aCounters[1], aCounters[2], (fGood)?"GOOD":"BAD" );
}




static void TestTimerProc( void* pParam )
{
	(*(int*)pParam)++;
	printf( "." );
}




static void TestBitRegApi( void )
{
	int i, c;
	UInt32 bits;

	printf( "Test seModifyReg16()...\n" );
	for ( i=0,c=0,bits=1; i<16; i++ )
	{
		seWriteReg16( TEST_REG, 0 );
		seModifyReg16( TEST_REG, 0, bits );	// test setting the bits
		if ( bits != seReadReg16(TEST_REG) )
			c++;

		seWriteReg16( TEST_REG, 0xFF );
		seModifyReg16( TEST_REG, bits, 0 );	// test clearing the bits
		if ( seReadReg16(TEST_REG) != (UInt32)(~(1<<i)&0xFF) )
			c++;

		bits <<= 1;
	}
	printf( "seModifyReg16() test had %d mismatches - %s.\n\n", c, (c)?"BAD":"GOOD" );


	printf( "Test seSetBitsReg16()...\n" );
	for ( i=0,c=0,bits=1; i<16; i++ )
	{
		seWriteReg16( TEST_REG, 0 );
		seSetBitsReg16( TEST_REG, bits );	// test setting the bits
		if ( bits != seReadReg16(TEST_REG) )
			c++;

		bits <<= 1;
	}
	printf( "seSetBitsReg16() test had %d mismatches - %s.\n\n", c, (c)?"BAD":"GOOD" );


	printf( "Test seClearBitsReg16()...\n" );
	for ( i=0,c=0,bits=1; i<8; i++ )
	{
		seWriteReg16( TEST_REG, 0xFF );
		seClearBitsReg16( TEST_REG, bits );	// test clearing the bits
		if ( seReadReg16(TEST_REG) != (UInt32)(~(1<<i)&0xFF) )
			c++;

		bits <<= 1;
	}
	printf( "seClearBitsReg16() test had %d mismatches - %s.\n\n", c, (c)?"BAD":"GOOD" );
}




static Int32 rand32( void )
{
	UInt32 lo, hi;

	lo = 16807 * (gSeed & 0xFFFF);
	hi = 16807 * ((UInt32)gSeed >> 16);
	lo += (hi & 0x7FFF) << 16;

	if (lo > 0x7FFFFFFF)
	{
		lo &= 0x7FFFFFFF;
		++lo;
	}

	lo += hi >> 15;

	if (lo > 0x7FFFFFFF)
	{
		lo &= 0x7FFFFFFF;
		++lo;
	}

	gSeed = lo;
	return gSeed;
}
