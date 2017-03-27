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
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>


#include "Reactor.h"
#include "PCF8574.hpp"

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



SharedMemoryLayout*   sharedMemory;


void* entryPoint(void*)
{
    PCF8574<ChannelBufferType, ChannelBufferType, 0x40>     pcf8574(sharedMemory->channel0In, sharedMemory->channel0Out);

    //
    //
    //
    while(true)
    {
        //pcf8574.SetOutputs(0x00);
        //pcf8574.SetOutputs(0x01);
#if 1
        DebugPrintf("Tick...\n");

        sharedMemory->channel0In.Put( 0x01 );
        sharedMemory->channel0In.Put( 0x02 );
        sharedMemory->channel0In.Put( 0x04 );
        sharedMemory->channel0In.Put( 0x08 );
        sharedMemory->channel0In.Put( 0x10 );
        sharedMemory->channel0In.Put( 0x20 );
        sharedMemory->channel0In.Put( 0x40 );
        sharedMemory->channel0In.Put( 0x83 );

        sharedMemory->channel0Command.Put( 0xfe );
        sharedMemory->channel0Command.Put( 0x08 );

        sharedMemory->channel0Command.Put( 0xff );
        sharedMemory->channel0Command.Put( 0xfd );
#endif

        usleep(100000);
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
    sharedMemory    = (SharedMemoryLayout*)SharedMemorySlaveInitialise(0x00000001);

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connection.\n");
    //while( sharedMemory->controlToServer.numberOfWriters == 0 );
    DebugPrintf("Connected.\n");

    //
    //
    //
    pthread_t   threadId;
    pthread_create(&threadId, NULL, entryPoint, NULL);

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {
        //
        //
        //

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
            fflush(stdout);
        }
    }

}


