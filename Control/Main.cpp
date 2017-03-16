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
#include "ErrorHandling.h"
#include "Utilities.h"

}




uint8_t*    counter64Base   = 0;
volatile uint32_t*   counterControl;

void SetupCounter64()
{
    const uint32_t	    BASE 					= 0x01F01C00;	// CPUCFG
    const uint32_t  	PAGE_SIZE 				= 4096;
    const uint32_t		BASEPage 				= BASE & ~(PAGE_SIZE-1);
    uint32_t 			BASEOffsetIntoPage		= BASE - BASEPage;
    int					mem_fd					= 0;
    uint8_t*			regAddrMap 				= NULL;


    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
    {
        perror("can't open /dev/mem");
        exit (1);
    }

      regAddrMap = (uint8_t*)mmap(
                        NULL,          
                        BASEOffsetIntoPage+(PAGE_SIZE*2),       	
                        PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
                        MAP_SHARED,       //Shared with other processes
                        mem_fd,           
                        BASEPage);

        if (regAddrMap == NULL) 
        {
              perror("mmap error");
              close(mem_fd);
              exit (1);
        }

    uint32_t*   pvalue = (uint32_t*)(regAddrMap + BASEOffsetIntoPage);

    counter64Base   = (uint8_t*)pvalue;

    counterControl  = (uint32_t*)(counter64Base+0x0280);

    //uint32_t*   counterControl  = (uint32_t*)(counter64Base+0x0280);
}

uint64_t GetCounter64()
{
    *counterControl     = 2;
    uint64_t    counter           = *(volatile uint64_t*)(counter64Base+0x0284);

    return counter;
}


//
//
//
int main()
{
    DebugPrintf("\nReactorControl.\n");

    SetupCounter64();

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
    while( (sharedMemory->inletToControl.numberOfWriters == 0) )
    {
        //printf("%llx\n",GetCounter64());        
    }
    DebugPrintf("Connected.\n");


    //
    //
    //
    typedef UARTTransmitter8N1<10,3, 0x01, 1024>    TxType;
    typedef UARTReceiver8N1<8,3, 0x02, 1024>        RxType;
    typedef PWM<24000000/200000,0, 0x08>                      PWMType;
    typedef I2CMaster<5, 10, 0x04,0x08>             I2CMasterType;
    //TxType          one;
    //RxType          two;
    NoOperation     nop;
    PWMType         pwm;
    //I2CMasterType   i2cMaster;
    Scheduler<  100, 
                PWMType, 
                NoOperation,
                NoOperation,
                NoOperation,
                NoOperation,
                NoOperation,
                NoOperation,
                NoOperation >  scheduler(pwm, nop, nop, nop, nop,nop, nop, nop);    


    //
    //
    //
    uint32_t    i   = 0;
    while(true)
    {
        //
        // Get the current timestamp.
        //
        //Timestamp    timestamp 	= GetTimestamp();
        uint64_t timestamp   = GetCounter64();

        //
        // Get the current input values.
        //
        uint8_t value   = sharedMemory->inletToControl.Get();

        //
        // Process the input.
        //
        uint8_t     outputValue;
        scheduler.PeriodicProcessing( timestamp, value, outputValue );

        //
        // Set the outputs.
        //
        sharedMemory->controlToOutlet.Put( outputValue );

        i++;
    }

}


