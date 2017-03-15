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

#include "Reactor.h"

extern "C"
{ 
#include "DebugText.h"
#include "Timestamp.h"
#include "SharedMemory.h"
#include <time.h>
#include "CircularBuffer.h"
#include "Utilities.h"
#include <pthread.h>
}

#include "FastSharedBuffer.hpp"







SharedMemoryLayout*   sharedMemory;


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
              8192,       	
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



void writel(uint32_t offset, uint32_t value)
{
    const uint32_t	    BASE 					= offset;
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
                        BASEOffsetIntoPage+(PAGE_SIZE*1),       	
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
    printf("(writel 1) value @%08x = %08x\n", offset, *pvalue );
    *pvalue  = value;
    msync( pvalue, 4, MS_SYNC|MS_INVALIDATE ); 
    printf("(writel 2) value @%08x = %08x\n", offset, *pvalue );
}


uint32_t readl(uint32_t offset)
{
    const uint32_t	    BASE 					= offset;
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
                        BASEOffsetIntoPage+(PAGE_SIZE*1),       	
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
    printf("(readl) value @%08x = %08x\n", offset, *pvalue );

    return *pvalue;
}












volatile GPIOPort* 	gpio 	= 0;
volatile GPIOPort*	portA	= 0;






//
//
//
int main()
{
    DebugPrintf("\nReactorInlet.\n");

    //
    //
    //
    sharedMemory    = (SharedMemoryLayout*)SharedMemorySlaveInitialise(0x00000001);

    //
    //
    //
    sharedMemory->inletToControl.InitialiseAsWriter();

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( sharedMemory->inletToControl.numberOfReaders == 0 );
    DebugPrintf("Connected.\n");


    //
    //
    //
    gpio 	= (GPIOPort*)SetupGPIO();
    portA	= &gpio[0];



    portA->CFG0 	= 0x11311111;
    portA->CFG1 	= 0x22211111;
    portA->CFG2 	= 0x11111111;
    portA->CFG3 	= 0x11111111;
    portA->DAT  	= 0xffffffff;
    portA->DRV0 	= 0x22222222;
    portA->DRV1 	= 0x22222222;
    portA->PUL0 	= 0x22222222;
    portA->PUL1 	= 0x22222222;

    uint16_t*   portA_DAT16   = (uint16_t*)&portA->DAT;

    while(true)
    {
        uint16_t   inputValue  = *portA_DAT16;
        sharedMemory->inletToControl.Put( (uint8_t)inputValue );
    }
}


