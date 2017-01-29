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
    sharedMemory[0]   = 0;
    sharedMemory[100] = 0;
    while( sharedMemory[0] == 0 )
    {       
    }

    printf("[%s]\n", sharedMemory);
    strcpy( (char*)&sharedMemory[100], "Hello Mars!" );

    //
    // InletToControl = 1000->2000;
    // ControlToOutlet = 2000->3000;
    //
    CircularBuffer*  inletToControl  = (CircularBuffer*)&sharedMemory[1000];
    CircularBufferInitialise( inletToControl, sizeof(uint32_t), (void*)&sharedMemory[1000+sizeof(CircularBuffer)] , (1000-sizeof(CircularBuffer))/sizeof(uint32_t) );

    CircularBuffer*  controlToOutlet  = (CircularBuffer*)&sharedMemory[2000];
    CircularBufferInitialise( controlToOutlet, sizeof(uint32_t), (void*)&sharedMemory[2000+sizeof(CircularBuffer)] , (2000-sizeof(CircularBuffer))/sizeof(uint32_t) );

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {
        //
        //
        //
        uint32_t  inData  = 0;
        CircularBufferGet( inletToControl, &inData );

        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();

        //
        //
        //
        uint32_t  outData   = 0;
        CircularBufferPut( controlToOutlet, &outData );
        outData++;
    }

}


