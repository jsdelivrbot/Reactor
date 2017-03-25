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
    while(true)
    {
#if 1
        DebugPrintf("Tick...\n");
        sharedMemory->channel0In.Put( 0x01 );
        sharedMemory->channel0In.Put( 0x02 );
        sharedMemory->channel0In.Put( 0x04 );
        sharedMemory->channel0In.Put( 0x08 );
        sharedMemory->channel0In.Put( 0x10 );
        sharedMemory->channel0In.Put( 0x20 );
        sharedMemory->channel0In.Put( 0x40 );
        sharedMemory->channel0In.Put( 0x80 );
#endif
#if 0
        i2cMaster.InsertIntoTxFIFO( 0x01 );
        i2cMaster.InsertIntoTxFIFO( 0x02 );
        i2cMaster.InsertIntoTxFIFO( 0x04 );
        i2cMaster.InsertIntoTxFIFO( 0xff );
        i2cMaster.InsertIntoTxFIFO( 0xff );
        i2cMaster.InsertIntoTxFIFO( 0x20 );
        i2cMaster.InsertIntoTxFIFO( 0x40 );
        i2cMaster.InsertIntoTxFIFO( 0x80 );
        i2cMaster.Transmit();
#endif
#if 0        
        static uint64_t     previousValue   = 0;
        uint64_t            thisValue       = GetTimestamp();
        uint64_t            delta;
        if(thisValue > previousValue)
        {
            delta           = thisValue - previousValue;
        }
        else
        {
            delta           = thisValue + (0xffffffff-previousValue);
        }

        //DebugPrintf("%lld.\n",delta);
        previousValue   = thisValue;

#endif
#if 0

        //
        //
        //
        for(uint32_t i=0; i<256; i++)
        {
            DebugPrintf("%d\n",pwm.deltas[i]);
        }
#endif
#if 0
        {
            static uint32_t     period;
            static int32_t      delta   = 1;
            if(period < 1)
            {
                delta   = 1;
            }
            if(period >= 500)
            {
                delta   = -1;
            }
            period  += delta;

            //pwm.SetPeriod(period);
        }
#endif

        usleep(100000);
        //sleep(1);
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


