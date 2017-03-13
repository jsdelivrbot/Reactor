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
#include "Scheduler.hpp"
#include "PulseWidthModulator.hpp"
#include "UARTTransmitter8N1.hpp"
#include "UARTReceiver8N1.hpp"
#include "NoOperation.hpp"
#include "I2CMaster.hpp"

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
    typedef UARTTransmitter8N1<10,3, 0x01, 1024>    TxType;
    typedef UARTReceiver8N1<8,3, 0x02, 1024>        RxType;
    typedef PWM<30,30, 0x4>                         PWMType;
    typedef I2CMaster<5, 30, 0x04,0x08>             I2CMasterType;
    TxType          one;
    RxType          two;
    NoOperation     nop;
    PWMType         pwm;
    I2CMasterType   i2cMaster;
    Scheduler<  100, 
                RxType, 
                RxType,
                RxType,
                I2CMasterType,
                RxType,
                RxType,
                RxType,
                RxType >  scheduler(two, two, two, i2cMaster, two,two, two, two);    


    //
    //
    //
    while(true)
    {
        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();

        //
        // Get the current input values.
        //
        uint8_t value   = sharedMemory->inletToControl.Get();

        //
        // Process the input.
        //
        uint8_t     outputValue;
        scheduler.PeriodicProcessing( timestamp, 0xab, outputValue );

        //
        // Set the outputs.
        //
        sharedMemory->controlToOutlet.Put( outputValue );

#if 1
        //
        //
        //
        static uint8_t     oldState    = 0;
        uint8_t            newState    = value;
        if( (uint8_t)((oldState+1)) != newState )
        {
            //
            // Change of data.
            //
            DebugPrintf("[%08x, %02x %02x] (%d,%d) \n", timestamp, newState, oldState, sharedMemory->inletToControl.head,sharedMemory->inletToControl.tail);
            //DebugPrintf(".");

        }
        oldState    = newState;
#endif
    }

}


