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
      		8192,       	
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
	if(state == true) 							\
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


	portA->CFG0 	= 0x11111111;
	portA->CFG1 	= 0x11111111;
	portA->CFG2 	= 0x11111111;
	portA->CFG3 	= 0x11111111;
	portA->DAT  	= 0xffffffff;
	portA->DRV0 	= 0x22222222;
	portA->DRV1 	= 0x22222222;
	portA->PUL0 	= 0x22222222;
	portA->PUL1 	= 0x22222222;

	while(true)
	{
#if 1		
		SetLineState( I,I,I,I, I,I,I,I );		// 0x00
		sleep(1);
		SetLineState( I,O,I,O, I,O,I,O );		// 0x00
		sleep(1);
		SetLineState( O,I,O,I, O,I,O,I );		// 0x00
		sleep(1);
#endif		
#if 0
		portA->DAT 	= 0xffffffff;
		sleep(5);
		portA->DAT 	= 0x00000000;
		sleep(5);
		portA->DAT 	= 0xaaaaaaaa;
		sleep(5);
		portA->DAT 	= 0x55555555;
		sleep(5);
#endif
#if 1
		//            0 1 2 3  4 5 6 7
		SetLineState( O,O,O,O, O,O,O,O );		// 0x00
		sleep(1);
		SetLineState( I,O,O,O, O,O,O,O ); 				// 0x01
		sleep(1);
		SetLineState( O,I,O,O, O,O,O,O ); 				// 0x02
		sleep(1);
		SetLineState( O,O,I,O, O,O,O,O ); 					// 0x04
		sleep(1);
		SetLineState( O,O,O,I, O,O,O,O ); 			// 0x08
		sleep(1);
		SetLineState( O,O,O,O, I,O,O,O ); 				// 0x10
		sleep(1);
		SetLineState( O,O,O,O, O,I,O,O ); 			// 0x20
		sleep(1);
		SetLineState( O,O,O,O, O,O,I,O ); 			// 0x40
		sleep(1);
		SetLineState( O,O,O,O, O,O,O,I ); 		// 0x80
		sleep(1);

		//printf("off\n");
		//portA->DAT 	= 0x00000000;
		//sleep(5);
		//printf("on\n");
#endif
		Timestamp 	timestamp 	= GetTimestamp();
	}

}


