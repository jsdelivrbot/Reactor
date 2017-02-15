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
    // InletToControl = 1000->2000;
    // ControlToOutlet = 2000->3000;
    // ControlToServer = 4000->14000;
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

    CircularBuffer*  controlToServer  = (CircularBuffer*)&sharedMemory[4000];
    CircularBufferInitialiseAsWriter( controlToServer, 
                                      sizeof(DataToServer), 
                                      (void*)&sharedMemory[4000+sizeof(CircularBuffer)] , 
                                      (10000-sizeof(CircularBuffer))/sizeof(DataToServer) );

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( (inletToControl->numberOfWriters == 0) || (controlToOutlet->numberOfReaders == 0) || (controlToServer->numberOfReaders == 0) );
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
        DataFromInlet  inData;
        SharedMemoryFlush( sharedMemory );
        CircularBufferGet( inletToControl, &inData );
        SharedMemoryFlush( sharedMemory );

        //
        //
        //
        for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(inData.data); i++)
        {
            static uint32_t     oldState    = 0;
            uint32_t            newState    = inData.data[i];
            if(oldState != newState )
            {
                //
                // CHange of data.
                //
                DebugPrintf("%08x: [%02x]<%02x>\n",inData.timestamp, inData.data[i], state);

                if( newState != state )
                {
                    DebugPrintf("<Mismatch!>\n");
                    //PANIC();
                }
                oldState    = newState;
            }
        }	



        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();

        //
        //
        //
        static uint32_t     count   = 0;
		count++;

		if( (count%1000) == 0 )
		{
            state++;
		}
        
        DataToOutlet  outData;
        memcpy( &outData, &inData, sizeof(outData) );
        for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(outData.data); i++)
        {
            outData.data[i]   = state;
        }	
        SharedMemoryFlush( sharedMemory );
        CircularBufferPut( controlToOutlet, &outData );
        SharedMemoryFlush( sharedMemory );

        //
        //
        //
        DataToServer  serverData;
        memcpy( &serverData, &inData, sizeof(serverData) );
        SharedMemoryFlush( sharedMemory );
        CircularBufferLossyPut( controlToServer, &serverData );
        SharedMemoryFlush( sharedMemory );

        //fprintf(stderr, "[%d]",outData);
    }

}


