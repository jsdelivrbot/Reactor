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
//#include "GeneratedScheduler.hpp"
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
//#include "MessageBox.h"

}




uint8_t*    counter64Base   = 0;
volatile uint8_t*   counterControl;
volatile uint32_t*   counterValue;

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
    //counterValue    = (volatile uint64_t*)(counter64Base+0x0284);
    counterValue    = (volatile uint32_t*)(counter64Base+0x0284);

    counterControl  = (uint8_t*)(counter64Base+0x0280);

    //uint32_t*   counterControl  = (uint32_t*)(counter64Base+0x0280);
}

uint64_t GetCounter64()
{
    *counterControl     = 2;
    uint32_t    value   = *counterValue;
    return (uint64_t)value;
}



SharedMemoryLayout*   sharedMemory;


void* entryPoint(void*)
{
    while(true)
    {
#if 0
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
    DebugPrintf("\nReactorControl.\n");

    SetupCounter64();

    //
    //
    //
    sharedMemory    = (SharedMemoryLayout*)SharedMemoryMasterInitialise(0x00000001);

    sharedMemory->inletToControl.InitialiseAsReader();
    sharedMemory->controlToOutlet.InitialiseAsWriter();

    //
    //
    //
    //MessageBoxInitialise();

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
    typedef PWM<1, 0, 0x02, ChannelBufferType>                      PWMType1;
    typedef PWM<1, 0, 0x10, ChannelBufferType>                      PWMType2;
    typedef I2CMaster<10, 0x01,0x10, ChannelBufferType>             I2CMasterType2;
    typedef I2CMaster<100, 0x04,0x08, ChannelBufferType>             I2CMasterType;
    //TxType          one;
    //RxType          two;
    NoOperation     nop;
    PWMType1         pwm(sharedMemory->channel1In);
    PWMType2         pwm2(sharedMemory->channel2In);
    I2CMasterType   i2cMaster(sharedMemory->channel0In, sharedMemory->channel0Out, sharedMemory->channel0Command );
    I2CMasterType2  i2cMaster2(sharedMemory->channel2In, sharedMemory->channel2Out, sharedMemory->channel2Command );

    Scheduler<  100, 
                PWMType1, 
                I2CMasterType,
                I2CMasterType2,
                I2CMasterType2,
                I2CMasterType2,
                I2CMasterType2,
                PWMType2,
                PWMType2 >  scheduler(pwm, i2cMaster, i2cMaster2, i2cMaster2, i2cMaster2,i2cMaster2, pwm2, pwm2);    


    //
    //
    //
    //pthread_t   threadId;
    //pthread_create(&threadId, NULL, entryPoint, NULL);

    //
    //
    //
    while(true)
    {
        static uint8_t     outputValue;

        //
        // Get the current input values.
        //
        uint8_t value   = sharedMemory->inletToControl.Get();
        //uint32_t value32 = 0;
        //MessageBoxRead(1, &value32);
        //uint8_t value   = value32&0xff;
        //DebugPrintf("rx %02x\n",value);

        //
        // Process the input.
        //
        scheduler.PeriodicProcessing( value, outputValue );

        //
        // Set the outputs.
        //
        sharedMemory->controlToOutlet.Put( outputValue );
    }

}


