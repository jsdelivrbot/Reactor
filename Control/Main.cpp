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
#include <unistd.h>
#include "Reactor.h"


extern "C"
{

#include "Timestamp.h"
#include "DebugText.h"
#include "SharedMemory.h"
#include "CircularBuffer.h"
#include "ErrorHandling.h"
#include "Utilities.h"

}




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
			PANIC();
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
    SharedMemoryLayout*   sharedMemory    = (SharedMemoryLayout*)SharedMemoryMasterInitialise(0x00000001);


    sharedMemory->inletToControl.InitialiseAsReader();
    sharedMemory->controlToOutlet.InitialiseAsWriter();

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( (sharedMemory->inletToControl.numberOfWriters == 0) );
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
        uint8_t value   = sharedMemory->inletToControl.Get();
        sharedMemory->controlToOutlet.Put( value );
        //SharedMemoryFlush(sharedMemory);
        //DebugPrintf("%d\n",value);
        //usleep(10);



        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();
#if 1
        //
        //
        //
        static uint8_t     oldState    = 0;
        uint8_t            newState    = value;
        if( (oldState+1)%100 != newState )
        {
            //
            // Change of data.
            //
            DebugPrintf("[%08x, %02x %02x] (%d,%d) \n", timestamp, newState, oldState, sharedMemory->inletToControl.head,sharedMemory->inletToControl.tail);
            //DebugPrintf(".");

        }
        oldState    = newState;
        //fprintf(stderr, "[%d]",outData);
#endif
    }

}


