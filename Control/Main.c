//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include "Timestamp.h"
#include "DebugText.h"
#include "SharedMemory.h"




//
//
//
int main()
{
    DebugPrintf("\nReactorControl.\n");

    //
    //
    //
    void*   sharedMemory    = SharedMemoryInitialise(0x00000001);

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {

        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();
    }

}


