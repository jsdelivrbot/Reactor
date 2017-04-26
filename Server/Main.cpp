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
#include "TCPServer.hpp"
#include "FastSharedBuffer.hpp"

extern "C"
{
#include "Timestamp.h"
#include "DebugText.h"
#include "SharedMemory.h"
#include "CircularBuffer.h"
#include "ErrorHandling.h"
#include "Utilities.h"
#include "FTDIDataSource.h"
}


uint64_t    totalBytes  = 0;


FastSharedBuffer<uint8_t,uint16_t>     lowRateBuffer;


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
static uint8_t      data[1024*1024];


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
        //DebugPrintf("Tick... %lld %02x\n", totalBytes, data[0]);
        totalBytes  = 0;

        uint8_t     sequence[]  =
        {
            0x12,
            0x14,
            0x18,
            0x16,
            0x02,
            0x04,
            0x16,
        };
        static uint8_t     index   = 0;
        uint8_t     value   = sequence[index%sizeof(sequence)];
        index++;

        sharedMemory->channel0In.Put( 0x7e );
        sharedMemory->channel0In.Put( value );

        sharedMemory->channel0Command.Put( 0xfe );
        sharedMemory->channel0Command.Put( 0x02 );

        sharedMemory->channel0Command.Put( 0xff );
        sharedMemory->channel0Command.Put( 0xfd );
#endif


        sharedMemory->channel1In.Put( 0xff );



        sharedMemory->channel2In.Put( 0x83 );
        sharedMemory->channel2Command.Put( 0xfe );
        sharedMemory->channel2Command.Put( 0x01 );
        sharedMemory->channel2Command.Put( 0xff );
        sharedMemory->channel2Command.Put( 0xfd );

        sharedMemory->channel2In.Put( 0x83 );
        sharedMemory->channel2Command.Put( 0xfe );
        sharedMemory->channel2Command.Put( 0x01 );
        sharedMemory->channel2Command.Put( 0xff );
        sharedMemory->channel2Command.Put( 0xfd );



        usleep(1000);
    }
}





//
// Called with data received over USB from teh FT232H.
// This is full rate 40MB/sec data.
//
int Callback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
    if(buffer != NULL)
    {
        //
        // Decimate the high-rate data into low-rate data for streaming.
        //
        for(uint32_t i=0; i<length; i+=10)
        {
            uint8_t     byte    = buffer[i];
            lowRateBuffer.NonBlockingPut(byte);
        }
        data[0] = buffer[0];

        totalBytes  += length;
    }

    return 0;
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
    pthread_create(&threadId, NULL, TCPServer, NULL);

    //
    // Start up the FTDI data source.
    //
    FTDIDevice          dev;
    int                 err;
    int                 c;

    err = FTDIDevice_Open(&dev);
    if (err)
    {
        fprintf(stderr, "USB: Error opening device\n");
        return 1;
    }

    FTDIDevice_SetMode( &dev, FTDI_INTERFACE_A,
                   FTDI_BITMODE_BITBANG, 0x00,
                   4000000);
    FTDIDevice_ReadStream( &dev, FTDI_INTERFACE_A, Callback, &data, 512, 32);

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


