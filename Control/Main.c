//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "Timestamp.h"
#include "DebugText.h"
#include "SharedMemory.h"
#include "CircularBuffer.h"
#include "Reactor.h"
#include "ErrorHandling.h"
#include "Utilities.h"






uint32_t 		checkValue 	= 0;
bool 			started 	= false;

void ProcessValue( uint32_t value )
{
	//fprintf(stderr, "[%d]",value);

	if(started == true)
	{
		if(value != checkValue)
		{
			DebugPrintf("%d != %d\n", checkValue, value);
			PANIC("mismatch!");
		}
		checkValue++;
	}
	else
	{
		checkValue 	= value+1;
		started 	= true;
	}
	
	if( (checkValue % 10000000) == 0)
	{
		printf("[%d]\n", value );
	}
	
}




//
//
//
int main()
{
    DebugPrintf("\nReactorControl.\n");

    //
    //
    //
    volatile uint8_t*   sharedMemory    = (uint8_t*)SharedMemoryMasterInitialise(0x00000001);

    //
    // InletToControl = 1000->2000;
    // ControlToOutlet = 2000->3000;
    //
    CircularBuffer*  inletToControl  = (CircularBuffer*)&sharedMemory[1000];
    CircularBufferInitialiseAsReader( inletToControl, 
                                      sizeof(DataFromInlet), 
                                      (void*)&sharedMemory[1000+sizeof(CircularBuffer)] , 
                                      (1000-sizeof(CircularBuffer))/sizeof(DataFromInlet) );

    CircularBuffer*  controlToOutlet  = (CircularBuffer*)&sharedMemory[2000];
    CircularBufferInitialiseAsWriter( controlToOutlet, 
                                      sizeof(DataToOutlet), 
                                      (void*)&sharedMemory[2000+sizeof(CircularBuffer)] , 
                                      (1000-sizeof(CircularBuffer))/sizeof(DataToOutlet) );

    //
    // Wait until we are fully connected.
    //
    printf("Waiting for connections.\n");
    while( (inletToControl->numberOfWriters == 0) || (controlToOutlet->numberOfReaders == 0) );
    printf("Connected.\n");

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {
        //
        //
        //
        DataFromInlet  inData;
        CircularBufferGet( inletToControl, &inData );
        SharedMemoryFlush( sharedMemory );

        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();

        //
        //
        //
        DataToOutlet  outData;
        memcpy( &outData, &inData, sizeof(outData) );
        for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(outData.data); i++)
        {
          ProcessValue( outData.data[i] );
        }	
        //CircularBufferShow( controlToOutlet );
        CircularBufferPut( controlToOutlet, &outData );
        SharedMemoryFlush( sharedMemory );

        //fprintf(stderr, "[%d]",outData);
    }

}


