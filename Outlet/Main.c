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

#include "armpmu_lib.h"


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


	while(true)
	{
		//portA->DAT 	|= 1<<17;
		portA->DAT 	= 0xffffffff;
		//printf("%08x\n", portA->DAT);
		//sleep(1);

		//portA->DAT 	&= ~(1<<17);
		portA->DAT 	= 0;
		//printf("%08x\n", portA->DAT);
		//sleep(1);

#if 0
		start 	= rdtsc32();
		for(uint32_t i=0; i<1000; i++)
		{
			static volatile 	uint32_t i 	= 0;
			i++;
		}
		end 	= rdtsc32();
		
		printf("diff = %d\n", end-start);
#endif
	}

}


