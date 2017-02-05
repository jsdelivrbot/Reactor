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
void ProcessValue( CircularBuffer* circularBuffer, uint32_t value )
{
	//fprintf(stderr, "[%d]",value);

	if(started == true)
	{
		if(value != checkValue)
		{
			DebugPrintf("%d != %d\n", checkValue, value);
			CircularBufferShow( circularBuffer );
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
    DebugPrintf("\nReactorServer.\n");

    //
    //
    //
    volatile uint8_t*   sharedMemory    = (uint8_t*)SharedMemorySlaveInitialise(0x00000001);

    //
    // InletToControl = 1000->2000;
    // ControlToOutlet = 2000->3000;
    // ControlToServer = 4000->14000;
    //
    CircularBuffer*  controlToServer  = (CircularBuffer*)&sharedMemory[4000];
    CircularBufferInitialiseAsReader( controlToServer, 
                                      sizeof(DataToServer), 
                                      (void*)&sharedMemory[4000+sizeof(CircularBuffer)] , 
                                      (10000-sizeof(CircularBuffer))/sizeof(DataToServer) );

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connection.\n");
    while( controlToServer->numberOfWriters == 0 )
    DebugPrintf("Connected.\n");

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {
        //
        //
        //
        DataToServer  inData;
        SharedMemoryFlush( sharedMemory );
        CircularBufferGet( controlToServer, &inData );
        SharedMemoryFlush( sharedMemory );

        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();

        //
        //
        //
        //for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(inData.data); i++)
        //{
        //  ProcessValue( controlToServer, inData.data[i] );
        //}	

        static uint32_t count   = 0;
        count++;
        if((count%10000) == 0)
        {
            DebugPrintf("[%d] ",inData.data[0]);
            fflush(stdout);
        }
    }

}


