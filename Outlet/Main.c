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
#include "DebugText.h"
#include "Timestamp.h"
#include "SharedMemory.h"
#include "CircularBuffer.h"
#include "ErrorHandling.h"
#include "Reactor.h"
#include "Utilities.h"

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
  	void*			regAddrMap 		= MAP_FAILED;


	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
	{
		perror("can't open /dev/mem");
		exit (1);
	}

  	regAddrMap = mmap(
      		NULL,          
      		0xffff,       	
			PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
			MAP_SHARED,       //Shared with other processes
      		mem_fd,           
		GPIO_BASEPage);

  	if (regAddrMap == MAP_FAILED) 
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
			PANIC("mismatch!");
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
uint32_t 	ledMask 	= 0;
uint32_t 	ledClearMask	= ~((1<<12)|(1<<11));
void SetLEDState(bool ledA, bool ledB, bool ledC, bool ledD)
{
	if(ledA == true)
	{
		ledMask 	|= 1<<12;
	}
	else
	{
		ledMask 	&= ~(1<<12);
	}

	if(ledB == true)
	{
		ledMask 	|= 1<<11;
	}
	else
	{
		ledMask 	&= ~(1<<11);
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


volatile uint32_t    Abuffer[0xffff];   // aligned on 64KB boundary so 16 bit index wraps.
volatile uint8_t     Cbuffer[0xffff];

#define CS           (1<<13)
#define CLK          (1<<14)
#define CLK_CS       (CLK|CS)
#define MISO         (1<<16)
    

//
//    31       23       15     8 7      0 
// A: 00000000 00000000 10000100 11001111
// B: 00000000 00000001 01100000 00000000
//
void Loop()
{
	uint8_t             C;
    uint16_t    aIndex  = 0;
    uint16_t    cIndex  = 0;
	volatile uint32_t*  portA_DAT   = &portA->DAT;
#if 1
	for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(Abuffer)/128; i+=128)
	{
		for(uint32_t j=0; j<128; j+=8)
		{
			Abuffer[i+j+0] 	= 1<<6;
			Abuffer[i+j+1] 	= 1<<7;
			Abuffer[i+j+2] 	= 1<<15;
			Abuffer[i+j+3] 	= 1<<0;
			Abuffer[i+j+4] 	= 1<<10;
			Abuffer[i+j+5] 	= 1<<2;
			Abuffer[i+j+6] 	= 1<<3;
			Abuffer[i+j+7] 	= 1<<1;
		}
	}
#endif
	memset( (void*)&Abuffer[0], 0x20, sizeof(Abuffer));

	uint32_t 	temp;
	uint32_t 	output;
	uint8_t 	value 	= 0;

    while(true)
    {
		ChangeLEDState();
		SetOutputState(value);
		value 	= ~value;
#if 0
        output      = (value & ledClearMask) | ledMask;
        aIndex++;        
        *portA_DAT  = output;                   // CLR_CS

        output      = (value & ledClearMask) | ledMask;
        aIndex++;        
        *portA_DAT  = output;                   // CLR_CS

		*portA_DAT 	|= CS;
		*portA_DAT 	&= ~CS;
#endif

#if 1
        *portA_DAT  = value;                      // CLR_CS
        aIndex++;
        *portA_DAT  = value | CS;                 // SET_CS
        aIndex++;

        C           = (*portA_DAT & MISO)>>9;   // 7
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>10; // 6
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>11; // 5
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>12; // 4
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>13; // 3
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>14; // 2
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>15; // 1
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        C           |= (*portA_DAT & MISO)>>16; // 0
        *portA_DAT  = value | CLK_CS;             // SET_CLK
        aIndex++;
        *portA_DAT  = value | CS;                 // CLR_CLK
        aIndex++;

        //
        //
        //
        //Cbuffer[cIndex]     = C;
        //cIndex++;
#endif
		DebugPrintf("[%02x]\n",C);
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




	portA->CFG0 	= 0x11111111;
	portA->CFG1 	= 0x11111111;
	portA->CFG2 	= 0x11111110;
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
#if 0
	while(true)
	{
		SetLEDState(true, false, false, false);
		for(volatile uint32_t i=0; i<12000000; i++);
		SetLEDState(false, true, false, false);
		for(volatile uint32_t i=0; i<12000000; i++);
		SetLEDState(false, false, true, false);
		for(volatile uint32_t i=0; i<12000000; i++);
		SetLEDState(false, false, false, true);
		for(volatile uint32_t i=0; i<12000000; i++);
	}
#endif
    //
    //
    //
    volatile uint8_t*   sharedMemory    = (uint8_t*)SharedMemorySlaveInitialise(0x00000001);

    //
    // InletToControl = 1000->2000;
    // ControlToOutlet = 2000->3000;
    //
    CircularBuffer*  controlToOutlet  = (CircularBuffer*)&sharedMemory[2000];
	CircularBufferInitialiseAsReader( 	controlToOutlet, 
										sizeof(DataToOutlet), 
										(void*)&sharedMemory[2000+sizeof(CircularBuffer)] , 
										(2000-sizeof(CircularBuffer))/sizeof(DataToOutlet) );

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( controlToOutlet->numberOfWriters == 0 );
    DebugPrintf("Connected.\n");


	Loop();

	while(true)
	{
		//
		//
		//
		Timestamp 	timestamp 	= GetTimestamp();

        //
        //
        //
        DataToOutlet  outData;
        SharedMemoryFlush( sharedMemory );
        CircularBufferGet( controlToOutlet, &outData );
        SharedMemoryFlush( sharedMemory );

		for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(outData.data); i++)
		{
			//SetOutputState( outData.data[i] );
				portA->DAT 	= 0xffffffff;
				portA->DAT 	= 0x00000000;
		}	

		
	}

}


