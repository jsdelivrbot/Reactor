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
#include <pthread.h>
}


SharedMemoryLayout*   sharedMemory;
volatile uint32_t        inputCount  = 0;


void* doSomeThing(void *arg)
{
    while(true)
    {
        DebugPrintf("%d (%d,%d)\n", inputCount/10, sharedMemory->controlToOutlet.head, sharedMemory->controlToOutlet.tail);
        inputCount  = 0;
        sleep(10);
    }
}

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



#define SET_OR_CLEAR_BIT(value,bitNumber, state)\
	if(state != false) 							\
	{											\
		value 	= value | (1<<bitNumber);		\
	}											\
	else										\
	{											\
		value 	= value & ~(1<<bitNumber);		\
	}

/*
Input bits->Port->Pin
D0  - PA6   - 2
D1  - PA7   - 3
D2  - PA15  - 4
D3  - PA0   - 5
D4  - PA10  - 6
D5  - PA2   - 7
D6  - PA3   - 8
D7  - PA1   - 9
*/
void SetLineState(bool d0, bool d1, bool d2, bool d3, bool d4, bool d5, bool d6, bool d7)
{
	uint32_t 	portValue 	= portA->DAT;

	SET_OR_CLEAR_BIT( portValue, 6,  d0 ); 	// d0 15
	SET_OR_CLEAR_BIT( portValue, 7,  d1 ); 	// d1 7
	SET_OR_CLEAR_BIT( portValue, 15, d2 ); 	// d2 6
	SET_OR_CLEAR_BIT( portValue, 0,  d3 ); 	// d3 0
	SET_OR_CLEAR_BIT( portValue, 10, d4 ); 	// d4 10
	SET_OR_CLEAR_BIT( portValue, 2,  d5 ); 	// d5 2
	SET_OR_CLEAR_BIT( portValue, 3,  d6 ); 	// d6 18
	SET_OR_CLEAR_BIT( portValue, 1,  d7 ); 	// d7 19
	
	portA->DAT 	= portValue;
}

#define O 		false
#define I 		true




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
	
	if( (checkValue % 1000000) == 0)
	{
		DebugPrintf("[%d]\n", value );
        fflush(stdout);
	}
	
}


//
//
//
void SetOutputState( uint8_t state )
{
	uint32_t 	portValue 	= portA->DAT;
	bool 		d0 			= state & 0x01;
	bool 		d1 			= state & 0x02;
	bool 		d2 			= state & 0x04;
	bool 		d3 			= state & 0x08;
	bool 		d4 			= state & 0x10;
	bool 		d5 			= state & 0x20;
	bool 		d6 			= state & 0x40;
	bool 		d7 			= state & 0x80;

	SET_OR_CLEAR_BIT( portValue, 6,  d0 ); 	// d0 15
	SET_OR_CLEAR_BIT( portValue, 7,  d1 ); 	// d1 7
	SET_OR_CLEAR_BIT( portValue, 15, d2 ); 	// d2 6
	SET_OR_CLEAR_BIT( portValue, 0,  d3 ); 	// d3 0
	SET_OR_CLEAR_BIT( portValue, 10, d4 ); 	// d4 10
	SET_OR_CLEAR_BIT( portValue, 2,  d5 ); 	// d5 2
	SET_OR_CLEAR_BIT( portValue, 3,  d6 ); 	// d6 18
	SET_OR_CLEAR_BIT( portValue, 1,  d7 ); 	// d7 19
	
	portA->DAT 	= portValue;

}



/*
TWI0-SDA = 3 = PA12 = LED1
TWI0-SCK = 5 = PA11 = LED2
UART1_TX = 8 = PG6 = LED3
UART1_RX = 10 = PG7 = LED4
*/
uint32_t 	ledStatus 		= 0;
uint32_t 	ledMask			= ((1<<12)|(1<<11));
void SetLEDState(bool ledA, bool ledB, bool ledC, bool ledD)
{
	if(ledA == true)
	{
		ledStatus 	|= 1<<12	;
	}
	else
	{
		ledStatus 	&= ~(1<<12);	
	}

	if(ledB == true)
	{
		ledStatus 	|= 1<<11	;
	}
	else
	{
		ledStatus 	&= ~(1<<11);	
	}

	if(ledC == true)
	{
		portG->DAT 	|= 1<<6;
	}
	else
	{
		portG->DAT 	&= ~(1<<6);
	}

	if(ledD == true)
	{
		portG->DAT 	|= 1<<7;
	}
	else
	{
		portG->DAT 	&= ~(1<<7);
	}

}



void ChangeLEDState()
{
	static uint32_t 	i=0;

	i++;
	if(i>=200000)
	{
		static uint32_t 	j=0;
		uint32_t 			k 	= j % 4;

		switch(k)
		{
			case 0: SetLEDState(true, false, false, false); break;
			case 1: SetLEDState(false, true, false, false); break;
			case 2: SetLEDState(false, false, true, false); break;
			case 3: SetLEDState(false, false, false, true); break;
			default: break;
		}

		j++;
		i 	= 0;
	}
}


volatile uint32_t    Abuffer[0xffff];   // index is 16 bit in size to give nice wrapping behaviour.

#define PL           (1<<4)
    

//
//    31       23       15     8 7      0 
// A: 00000000 00000000 10000100 11001111
// B: 00000000 00000001 01100000 00000000
//
void Loop( FastSharedBuffer<uint8_t,uint8_t>& buffer )
{
    uint16_t    aIndex  = 0;
	volatile uint32_t*  portA_DAT32   = (uint32_t*)&portA->DAT;
	volatile uint16_t*  portA_DAT16   = (uint16_t*)&portA->DAT;
	volatile uint8_t*   portA_DAT8    = (uint8_t*)&portA->DAT;

	memset( (void*)&Abuffer[0], 0xff, sizeof(Abuffer));

	//
	// values used for output should have bits set as appropriate for the pins and 
	// should have PL and LED bits cleared.
	// This avoids performing repeated processing in the inner loop.
	//
	for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(Abuffer); i++)
	{
		Abuffer[i] 	= 0xffffffff & (~PL) & (~ledMask);
	}

	uint32_t 	temp;
	uint32_t 	output;
	uint8_t 	value 	= *portA_DAT32;

    while(true)
    {
		//DebugPrintf("tick\n");
		//ChangeLEDState();
        //SharedMemoryFlush(sharedMemory);
		uint8_t 	b = buffer.Get();
		//DebugPrintf("%d\n",b);
        *portA_DAT8  = b;
        //volatile uint8_t   inputValue  = *portA_DAT8;
        //volatile uint32_t   inputValue  = *portA_DAT;
        //value++;
        inputCount++;
		for(volatile uint32_t i=0; i<10; i++);
#if 0
		//
		//
		//
		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus;
        *portA_DAT  = output;                   // CLR_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL

		value 	= Abuffer[aIndex];
        aIndex++;        
        output      = value | ledStatus	 | PL;
        *portA_DAT  = output;                   // SET_PL
#endif
    }

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





	portA->CFG0 	= 0x11311111;
	portA->CFG1 	= 0x22211111;
	portA->CFG2 	= 0x11111111;
	portA->CFG3 	= 0x11111111;
	portA->DAT  	= 0xffffffff;
	portA->DRV0 	= 0x22222222;
	portA->DRV1 	= 0x22222222;
	portA->PUL0 	= 0x22222222;
	portA->PUL1 	= 0x22222222;




	portG->DAT  	= 0xffffffff;
	portG->DRV0 	= 0x33333333;
	portG->DRV1 	= 0x33333333;
	portG->PUL0 	= 0x00000000;
	portG->PUL1 	= 0x00000000;

	portG->CFG0 	&= ~0xff000000;
	portG->CFG0 	|=  0x11000000;

    //
    //
    //
    sharedMemory    = (SharedMemoryLayout*)SharedMemorySlaveInitialise(0x00000001);


    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( sharedMemory->controlToOutlet.numberOfWriters == 0 );
    DebugPrintf("Connected.\n");

    static pthread_t    threadId;
    pthread_create( &threadId, NULL, &doSomeThing, NULL);
    DebugPrintf(" Started display thread\n ");

	Loop( sharedMemory->controlToOutlet );

}


