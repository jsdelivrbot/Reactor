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
            fflush(stdout);
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
		DebugPrintf("[%d]\n", value );
        fflush(stdout);
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
    //
    //
    SharedMemoryLayout*     layout  = (SharedMemoryLayout*)sharedMemory;
    volatile FastSharedBuffer*       inletToControl  = &layout->inletToControl;
    FastSharedBufferInitialiseAsReader( inletToControl );

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( (inletToControl->numberOfWriters == 0) );
    DebugPrintf("Connected.\n");

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {
        static uint8_t      state   = 0;

        //
        //
        //
        SharedMemoryFlush( sharedMemory );
        uint8_t value   = FastSharedBufferGet( inletToControl );
        SharedMemoryFlush( sharedMemory );

        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();

        //
        //
        //
        static uint8_t     oldState    = 0;
        uint8_t            newState    = value;
        if(oldState != newState )
        {
            //
            // Change of data.
            //
            DebugPrintf("[%08x, %02x %02x]\n", timestamp, newState, oldState);

            oldState    = newState;
        }
        //fprintf(stderr, "[%d]",outData);
    }

}


