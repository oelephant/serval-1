// TESTINIT.C - Test Simple API initialization


#include <stdio.h>
#include "seapi.h"


int main( int argc, char *argv[] )
{
	argc, argv;

	if ( seAcquireController() )
	{
		if ( seInitializeController(seINIT_NORMAL) )
		{
			printf( "OK!\n" );
		}
		else
		{
			printf( "seInitializeController() failed.\n" );
		}
	}
	else
	{
		printf( "seAcquireController() failed.\n" );
	}

	return 0;
}
