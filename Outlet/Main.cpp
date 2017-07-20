//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//


/*


Hdr26   Funtion              ShiftReg       Hdr10
---------------------------------------------------------------
1       3V3                 16/VCC          1/3V3
7       PWM1/PA6            13/D2           4/D2 
11      TWI1_SDA/PA1        6/D7            9/D7 !
13      UART2_TX/PA0        14/D3           5/D3
15      TWI1_SCK/PA3        5/D6            8/D6 !
19      SPI1_MOSI/PA15      11/D0           2/D0
21      SPI1_MISO           9/Q7            -
23      SPI1_CLK            2/CP            -
25      GND                 8/GND           10/GND
12      PA7                 12/D1           3/D1
22      UART2_RTS/PA2       4/D5            7/D5
24      SPI1_CS             1/PL            -
26      PA10                3/D4            6/D4


Input bits->Port->Pin
D0  - PA15  - 2
D1  - PA7   - 3
D2  - PA6   - 4
D3  - PA0   - 5
D4  - PA10  - 6
D5  - PA2   - 7
D6  - PA3   - 8
D7  - PA1   - 9

TWI0-SDA = 3 = PA12 = LED1
TWI0-SCK = 5 = PA11 = LED2
UART1_TX = 8 = PG6 = LED3
UART1_RX = 10 = PG7 = LED4

*/


//
// https://github.com/thoughtpolice/enable_arm_pmu.git
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

#include "Reactor.h"

extern "C"
{
#include "DebugText.h"
#include "Timestamp.h"
#include "SharedMemory.h"
#include "CircularBuffer.h"
#include "ErrorHandling.h"
#include "Utilities.h"
#include "MessageBox.h"
#include <pthread.h>
}


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

    

//
//
//
int main()
{
    DebugPrintf("\nReactorOutlet.\n");

	uint32_t 	start;
	uint32_t	end;
	volatile GPIOPort* 	gpio 	= (GPIOPort*)SetupGPIO();
	portA	= &gpio[0];
	portG 	= &gpio[6];


	portA->CFG0 	= 0x11111111;
	portA->CFG1 	= 0x00000000;
	portA->CFG2 	= 0x00000000;
	portA->CFG3 	= 0x00000000;
	portA->DAT  	= 0xffffffff;
	portA->DRV0 	= 0x22222222;
	portA->DRV1 	= 0x22222222;
	portA->PUL0 	= 0x22222222;
	portA->PUL1 	= 0x22222222;

	portG->DAT  	= 0xffffffff;
	portG->DRV0 	= 0x33333333;
	portG->DRV1 	= 0x33333333;
	portG->PUL0 	= 0x22222222;
	portG->PUL1 	= 0x22222222;

	portG->CFG0 	&= ~0xff000000;
	portG->CFG0 	|=  0x11000000;

    //
    //
    //
    sharedMemory    = (SharedMemoryLayout*)SharedMemorySlaveInitialise(0x00000001);

    //
    //
    //
    MessageBoxInitialise();

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( sharedMemory->controlToOutlet.numberOfWriters == 0 );
    DebugPrintf("Connected.\n");

	//
	//
	//
	uint32_t 	outputStates 	= portA->DAT&0xffffff00;
	volatile uint8_t*   portA_DAT8    = (uint8_t*)&portA->DAT;
	volatile uint32_t*  portA_DAT32   = (uint32_t*)&portA->DAT;

#if 0
    outputStates    &= ~(1<<0);
#endif
	uint8_t 	i = 0;
    while(true)
    {
		uint8_t 	b = sharedMemory->controlToOutlet.Get();
#if 0
        static uint32_t     i=0;
        static uint8_t      v   = 0;
        if( (i%10000000) == 0)
        {
            v   = ~v;
        }
        b   &= ~(1<<0);
        b   |= v&(1<<0);
        i++;
#endif
        //portA->DAT  = outputStates|(uint32_t)(b);
        *portA_DAT8     = 0x00;
        uint32_t inValue    = *portA_DAT32;
        printf("%08x\n",inValue&0x000fff00);

        usleep(1000);
        *portA_DAT8     = 0xf0;

        usleep(1000);

        // 0x01 - 0
        // 0x02 - 1
        // 0x04 - 6
        // 0x08 - 3
        // 0x10 - 
        // 0x20 - 5
        // 0x40 - 1,2 (PIns 7 & 11 are mistakenly joined at connector!) just 2 after trace cut.
        // 0x80 - 7
        //

        inValue    = *portA_DAT32;
        printf("%08x\n",inValue&0x000fff00);

        //
        // 101 10111100 00000000
        //
        // D0 PA10 = 1
        // D1 PA11 = 1
        // D2 PA12 = 1
        // D3 PA13 = 1
        // D4 PA14 = 
        // D5 PA15 = 1
        // D6 PA16 = 1
        // D7 PA18 = 
    }

}


