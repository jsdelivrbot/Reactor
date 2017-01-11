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
13      UART2_TX/PA0        14/D3           5/D3
19      SPI1_MOSI/PA15      11/D0           2/D0
21      SPI1_MISO           9/Q7            -
23      SPI1_CLK            2/CP            -
25      GND                 8/GND           10/GND
12      PA7                 12/D1           3/D1
16      TWI1_SDA/PA19       6/D7            9/D7
18      TWI1_SCK/PA18       5/D6            8/D6
22      UART2_RTS/PA2       4/D5            7/D5
24      SPI1_CS             1/PL            -
26      PA10                3/D4            6/D4


Input bits->Port->Pin
D0  - PA6   - 2
D1  - PA7   - 3
D2  - PA15  - 4
D3  - PA0   - 5
D4  - PA10  - 6
D5  - PA2   - 7
D6  - PA18  - 8
D7  - PA19  - 9


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



//
//
//
int main()
{
    DebugPrintf("\nReactorOutlet.\n");


	uint32_t 	start;
	uint32_t	end;
	volatile GPIOPort* 	gpio 	= (GPIOPort*)SetupGPIO();
	volatile GPIOPort*	portA	= &gpio[0];
	volatile GPIOPort*	portC	= &gpio[1];
	volatile GPIOPort*	portD	= &gpio[2];
	volatile GPIOPort*	portE	= &gpio[3];
	volatile GPIOPort*	portF	= &gpio[4];
	volatile GPIOPort*	portG	= &gpio[5];
	volatile GPIOPort*	portL	= &gpio[6];


	printf("CFG1=%08x\n",portL->CFG1);
	portA->CFG0 	= 0x11111111;
	portA->CFG1 	= 0x11111111;
	portA->CFG2 	= 0x11111111;
	portA->CFG3 	= 0x11111111;
	portA->DAT  	= 0xffffffff;
	portA->DRV0 	= 0x33333333;
	portA->DRV1 	= 0x33333333;
	portA->PUL0 	= 0x00000000;
	portA->PUL1 	= 0x00000000;
	printf("CFG1=%08x\n",portL->CFG1);

/*
Input bits->Port->Pin
D0  - PG7/PA6- 2
D1  - PA7   - 3
D2  - PG6/PA15- 4
D3  - PA0   - 5
D4  - PA10  - 6
D5  - PA2   - 7
D6  - PA18  - 8
D7  - PA19  - 9
*/
	while(true)
	{
		portA->DAT 	= 0xffffffff;
		sleep(1);

		portA->DAT 	= 0x00000000;
		sleep(1);

		Timestamp 	timestamp 	= GetTimestamp();
	}

}


