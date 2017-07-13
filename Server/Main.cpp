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
#include "I2CMaster.hpp"

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

ChannelBufferType   ioCmd;
ChannelBufferType   ioIn;
ChannelBufferType   ioOut;

uint64_t    totalBytes  = 0;


//
//
//
typedef struct
{
	volatile uint32_t	CFG0;
	volatile uint32_t	CFG1;
	volatile uint32_t	CFG2;
	volatile uint32_t	CFG3;

	volatile uint32_t	DAT;
	volatile uint32_t	DRV0;
	volatile uint32_t	DRV1;

	volatile uint32_t	PUL0;
	volatile uint32_t	PUL1;

} GPIOPort;


volatile GPIOPort*	portA 	= (GPIOPort*)0;
volatile GPIOPort*	portG 	= (GPIOPort*)0;


//
//
//
uint32_t* SetupGPIO()
{
	const unsigned long	GPIO_BASE 		= 0x01C20800;
	const unsigned long 	PAGE_SIZE 		= 4096;
	const unsigned long	GPIO_BASEPage 		= GPIO_BASE & ~(PAGE_SIZE-1);
	uint32_t 		GPIO_BASEOffsetIntoPage	= GPIO_BASE - GPIO_BASEPage;
  	int			mem_fd			= 0;
  	uint8_t*			regAddrMap 		= NULL;


	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
	{
		perror("can't open /dev/mem");
		exit (1);
	}

  	regAddrMap = (uint8_t*)mmap(
      		NULL,          
      		0xffff,       	
			PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
			MAP_SHARED,       //Shared with other processes
      		mem_fd,           
		GPIO_BASEPage);

  	if (regAddrMap == NULL) 
	{
          	perror("mmap error");
          	close(mem_fd);
          	exit (1);
  	}

	printf("gpio mapped to %p = %08x\n", regAddrMap, (uint32_t)GPIO_BASEPage);

	return (uint32_t*)(regAddrMap + GPIO_BASEOffsetIntoPage);
}

    


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


void* I2CThread(void*)
{
    I2CMaster<100, 1<<6, 1<<7, ChannelBufferType>  i2cMaster(ioIn,ioOut,ioCmd);
	volatile GPIOPort* 	gpio 	= (GPIOPort*)SetupGPIO();
	portA	= &gpio[0];
	portG 	= &gpio[6];

	portG->DAT  	= 0xffffffff;
	portG->DRV0 	= 0x33333333;
	portG->DRV1 	= 0x33333333;
	portG->PUL0 	= 0x11111111;
	portG->PUL1 	= 0x11111111;

	portG->CFG0 	&= ~0xff000000;
	portG->CFG0 	|=  0x11000000;


    uint32_t    originalValue   = portG->DAT & ~((1<<6) | (1<<7));
    while(true)
    {
        static uint8_t inputValue  = 0;
        static uint8_t outputValue = 0;
        i2cMaster.PeriodicProcessing( inputValue, outputValue );

        uint32_t    newValue    = originalValue | outputValue;
        portG->DAT  = newValue;

        usleep(100);
    }
}



#define OFF         (3)
#define PINK        (0)
#define BLUE        (2)
#define ORANGE      (1)

#define LED0        (0)
#define LED1        (2)
#define LED2        (6)
#define LED3        (4)


#define LED0_OFF        (OFF<<LED0)
#define LED0_BLUE       (BLUE<<LED0)
#define LED0_ORANGE     (ORANGE<<LED0)
#define LED0_PINK       (PINK<<LED0)

#define LED1_OFF        (OFF<<LED1)
#define LED1_BLUE       (1<<LED1)
#define LED1_ORANGE     (2<<LED1)
#define LED1_PINK       (PINK<<LED1)

#define LED2_OFF        (OFF<<LED2)
#define LED2_BLUE       (BLUE<<LED2)
#define LED2_ORANGE     (ORANGE<<LED2)
#define LED2_PINK       (PINK<<LED2)

#define LED3_OFF        (OFF<<LED3)
#define LED3_BLUE       (BLUE<<LED3)
#define LED3_ORANGE     (ORANGE<<LED3)
#define LED3_PINK       (PINK<<LED3)


void* entryPoint(void*)
{
    //
    //
    //
    while(true)
    {
        //DebugPrintf("Tick... %lld %02x\n", totalBytes, data[0]);
        totalBytes  = 0;

#if 0
        uint8_t     sequence[]  =
        {
            LED0_OFF    | LED1_OFF      | LED2_OFF      | LED3_OFF,
            LED0_ORANGE | LED1_ORANGE   | LED2_ORANGE   | LED3_ORANGE,
            LED0_PINK   | LED1_PINK     | LED2_PINK     | LED3_PINK  ,
            LED0_BLUE   | LED1_BLUE     | LED2_BLUE     | LED3_BLUE  ,
        };
#endif
#if 1
        uint8_t     sequence[]  =
        {
            LED0_OFF    | LED1_OFF      | LED2_OFF      | LED3_BLUE,
            LED0_OFF    | LED1_OFF      | LED2_BLUE     | LED3_BLUE,
            LED0_OFF    | LED1_BLUE     | LED2_BLUE     | LED3_BLUE  ,
            LED0_BLUE   | LED1_BLUE     | LED2_BLUE     | LED3_BLUE  ,

            LED0_BLUE   | LED1_BLUE     | LED2_BLUE     | LED3_PINK,
            LED0_BLUE   | LED1_BLUE     | LED2_PINK     | LED3_PINK,
            LED0_BLUE   | LED1_PINK     | LED2_PINK     | LED3_PINK  ,
            LED0_PINK   | LED1_PINK     | LED2_PINK     | LED3_PINK  ,

            LED0_PINK   | LED1_PINK     | LED2_PINK     | LED3_ORANGE  ,
            LED0_PINK   | LED1_PINK     | LED2_ORANGE   | LED3_ORANGE  ,
            LED0_PINK   | LED1_ORANGE   | LED2_ORANGE   | LED3_ORANGE  ,
            LED0_ORANGE | LED1_ORANGE   | LED2_ORANGE   | LED3_ORANGE  ,

            LED0_ORANGE | LED1_ORANGE   | LED2_ORANGE   | LED3_OFF  ,
            LED0_ORANGE | LED1_ORANGE   | LED2_OFF      | LED3_OFF  ,
            LED0_ORANGE | LED1_OFF      | LED2_OFF      | LED3_OFF  ,
            LED0_OFF    | LED1_OFF      | LED2_OFF      | LED3_OFF  ,
        };
#endif
        static uint8_t     index   = 0;
        uint8_t     value   = sequence[index%sizeof(sequence)];
        index++;

        ioIn.Put( 0x70 );
        ioIn.Put( value );

        ioCmd.Put( 0xfe );
        ioCmd.Put( 0x02 );

        ioCmd.Put( 0xff );
        ioCmd.Put( 0xfd );



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



        usleep(100000);
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
        for(uint32_t i=0; i<length; i+=5)
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
    //
    //
    pthread_create(&threadId, NULL, I2CThread, NULL);
#if 0
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
#endif
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


