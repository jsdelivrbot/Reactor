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
#include "DebugText.h"
#include "Timestamp.h"
#include "SharedMemory.h"
#include <time.h>
#include "CircularBuffer.h"
#include "Reactor.h"
#include "Utilities.h"
#include <pthread.h>










volatile uint32_t        inputCount  = 0;


void* doSomeThing(void *arg)
{
    while(true)
    {
        DebugPrintf("%d\n", inputCount/10);
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



void writel(uint32_t offset, uint32_t value)
{
    const uint32_t	    BASE 					= offset;
    const uint32_t  	PAGE_SIZE 				= 4096;
    const uint32_t		BASEPage 				= BASE & ~(PAGE_SIZE-1);
    uint32_t 			BASEOffsetIntoPage		= BASE - BASEPage;
    int					mem_fd					= 0;
    void*				regAddrMap 				= MAP_FAILED;


    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
    {
        perror("can't open /dev/mem");
        exit (1);
    }

      regAddrMap = mmap(
                        NULL,          
                        BASEOffsetIntoPage+(PAGE_SIZE*1),       	
                        PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
                        MAP_SHARED,       //Shared with other processes
                        mem_fd,           
                        BASEPage);

        if (regAddrMap == MAP_FAILED) 
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
    void*				regAddrMap 				= MAP_FAILED;


    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
    {
        perror("can't open /dev/mem");
        exit (1);
    }

      regAddrMap = mmap(
                        NULL,          
                        BASEOffsetIntoPage+(PAGE_SIZE*1),       	
                        PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
                        MAP_SHARED,       //Shared with other processes
                        mem_fd,           
                        BASEPage);

        if (regAddrMap == MAP_FAILED) 
        {
              perror("mmap error");
              close(mem_fd);
              exit (1);
        }

    uint32_t*   pvalue = (uint32_t*)(regAddrMap + BASEOffsetIntoPage);
    printf("(readl) value @%08x = %08x\n", offset, *pvalue );

    return *pvalue;
}







//
//
//
typedef struct
{
    volatile uint32_t	CH_CTL;
    volatile uint32_t	CH0_PERIOD;
    volatile uint32_t	CH1_PERIOD;


} PWMPort;


//
//
//
PWMPort* SetupPWM()
{
    const uint32_t	    BASE 					= 0x01C21400;	// SPI0
    const uint32_t  	PAGE_SIZE 				= 4096;
    const uint32_t		BASEPage 				= BASE & ~(PAGE_SIZE-1);
    uint32_t 			BASEOffsetIntoPage		= BASE - BASEPage;
    int					mem_fd					= 0;
    void*				regAddrMap 				= MAP_FAILED;


    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
    {
        perror("can't open /dev/mem");
        exit (1);
    }

      regAddrMap = mmap(
                        NULL,          
                        BASEOffsetIntoPage+(PAGE_SIZE*2),       	
                        PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
                        MAP_SHARED,       //Shared with other processes
                        mem_fd,           
                        BASEPage);

        if (regAddrMap == MAP_FAILED) 
        {
              perror("mmap error");
              close(mem_fd);
              exit (1);
        }

    uint32_t*   pvalue = (uint32_t*)(regAddrMap + BASEOffsetIntoPage);

    return (PWMPort*)pvalue;
}





//
//
//
typedef struct
{
    volatile uint32_t	reserved0;

    volatile uint32_t	CTL;			// 0x04 Global Control
    volatile uint32_t	INTCTL;			// 0x08 Transfer Conrol

    volatile uint32_t	reserved1;		// 0x0c

    volatile uint32_t	IER;			// 0x10 Interrupt control
    volatile uint32_t	INT_STA;		// 0x14 Interrupt status
    volatile uint32_t	FCR;			// 0x18 FIFO control
    volatile uint32_t	FSR;			// 0x1c FIFO status
    volatile uint32_t	WAIT;			// 0x20 wait clock counter
    volatile uint32_t	CCTL;			// 0x24 Clock rate control

    volatile uint32_t	reserved2;		// 0x28
    volatile uint32_t	reserved3;		// 0x2c

    volatile uint32_t	BC;				// 0x30 burst counter
    volatile uint32_t	TC;				// 0x34 transmit counter
    volatile uint32_t	BCC;			// 0x38 Burst control

    volatile uint8_t	reserved4[0x1c4];// 0x40

    volatile uint32_t	TXD;			// 0x200

    volatile uint8_t	reserved5[0xfc];// 0x204

    volatile uint32_t	RXD;			// 0x300

    volatile uint8_t	reserved6[0xcfc];// 0x204

} SPIPort;


//
//
//
SPIPort* SetupSPI()
{
    const uint32_t	    BASE 					= 0x01C68000;	// SPI0
    const uint32_t  	PAGE_SIZE 				= 4096;
    const uint32_t		BASEPage 				= BASE & ~(PAGE_SIZE-1);
    uint32_t 			BASEOffsetIntoPage		= BASE - BASEPage;
    int					mem_fd					= 0;
    void*				regAddrMap 				= MAP_FAILED;


    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
    {
        perror("can't open /dev/mem");
        exit (1);
    }

      regAddrMap = mmap(
                        NULL,          
                        BASEOffsetIntoPage+(PAGE_SIZE*2),       	
                        PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
                        MAP_SHARED,       //Shared with other processes
                        mem_fd,           
                        BASEPage);

        if (regAddrMap == MAP_FAILED) 
        {
              perror("mmap error");
              close(mem_fd);
              exit (1);
        }

    uint32_t*   pvalue = (uint32_t*)(regAddrMap + BASEOffsetIntoPage);

    return (SPIPort*)pvalue;
}


//
//
//
void spi_set_clk(SPIPort* spi, uint32_t spi_clk, uint32_t ahb_clk)
{
/* SPI Clock Control Register Bit Fields & Masks,default:0x0000_0002 */
#define SPI_CLK_CTL_CDR2		(0xFF <<  0)	/* Clock Divide Rate 2,master mode only : SPI_CLK = AHB_CLK/(2*(n+1)) */
#define SPI_CLK_CTL_CDR1		(0xF  <<  8)	/* Clock Divide Rate 1,master mode only : SPI_CLK = AHB_CLK/2^n */
#define SPI_CLK_CTL_DRS			(0x1  << 12)	/* Divide rate select,default,0:rate 1;1:rate 2 */
#define SPI_CLK_SCOPE			(SPI_CLK_CTL_CDR2+1)


    uint32_t    reg_val = 0;
    uint32_t    div_clk = ahb_clk / (spi_clk << 1);

    printf("set spi clock %d, mclk %d\n", spi_clk, ahb_clk);
    reg_val = spi->CCTL; // readl(base_addr + SPI_CLK_CTL_REG);

    /* CDR2 */
    if(div_clk <= SPI_CLK_SCOPE) {
        if (div_clk != 0) {
            div_clk--;
        }
        reg_val &= ~SPI_CLK_CTL_CDR2;
        reg_val |= (div_clk | SPI_CLK_CTL_DRS);
        printf("CDR2 - n = %d \n", div_clk);
    }/* CDR1 */
    else {
		div_clk = 0;
		while(ahb_clk > spi_clk){
			div_clk++;
			ahb_clk >>= 1;
		}
        reg_val &= ~(SPI_CLK_CTL_CDR1 | SPI_CLK_CTL_DRS);
        reg_val |= (div_clk << 8);
        printf("CDR1 - n = %d \n", div_clk);
    }
    spi->CCTL   = reg_val;  //writel(reg_val, base_addr + SPI_CLK_CTL_REG);
}







volatile GPIOPort* 	gpio 	= 0;
volatile SPIPort* 	spi 	= 0;
volatile SPIPort*   spi0    = 0;
volatile SPIPort*   spi1    = 0;
volatile GPIOPort*	portA	= 0;
volatile GPIOPort*	portC	= 0;
volatile GPIOPort*	portD	= 0;
volatile GPIOPort*	portE	= 0;
volatile GPIOPort*	portF	= 0;
volatile GPIOPort*	portG	= 0;
volatile GPIOPort*	portL	= 0;



#define SET_CLK()       {portA->DAT |= (1<<14);}
#define CLR_CLK()       {portA->DAT &= ~(1<<14);}

#define SET_CS()        {portA->DAT |= (1<<13);}
#define CLR_CS()        {portA->DAT &= ~(1<<13);}

//#define GET_MISO()      ((portA->DAT & (1<<16))>>8)      // To avoid an unnecessary shift, this returns 0x80 when high and 0x00 when low.
#define GET_MISO()      (( portA->DAT & (1<<16) )?0x01:0x00)      // To avoid an unnecessary shift, this returns 0x80 when high and 0x00 when low.


//
//
//
void SetupSPIController( volatile SPIPort* spiX )
{
    //
    // CCU:AHB1_APB1_CFG_REG     = PASS.
    //
    //writel( 0x01C20000+0x0054, 0x00003080 ); // 2.4MHz


    //
    // CCU:BUS_CLK_GATING_REG0:SPI1     = PASS.
    //
    uint32_t    clkGating     = readl(0x01C20000+0x0060); 
    writel( 0x01C20000+0x0060, clkGating | (1<<21) ); 
    printf("BUS_CLK_GATING_REG0:SPI1 = %08x\n", readl(0x01C20000+0x0060));

    //
    // CCU:SPI1_CLK_REG clock setup
    //
    //writel( 0x01C20000+0x00A4, 0x81020000 );    // 12/4MHz, PLL_PERIPH0  <---
    printf("SPI1_CLK_REG = %08x\n", readl(0x01C20000+0x00A4));
    
    //
    // spiX port setup.
    //
    spiX->CTL 	= 0x00000001;       // slave mode.
    spiX->INTCTL = 0x00000184;      // s/w control of SS.
    spiX->IER 	= 0x00000000;
    spiX->INT_STA= 0x00000642;
    spiX->FCR 	= 0x00100010;
    spiX->FSR 	= 0x00400000;
    spiX->WAIT 	= 0x00000000;
    spiX->CCTL 	= 0x00001004;
    spiX->BC 	= 0x00000004;
    spiX->TC 	= 0x00000000;
    spiX->BCC 	= 0x00000000;


    //
    // Reset and wait for reset to complete.
    //
    portA->DAT 	= 0xffffffff;
    spiX->CTL 	    = 0x80000000;
    while( (spiX->CTL&0x80000000) != 0);
    portA->DAT 	= 0x00000000;

    //
    // Reset the TX fifo.
    //
    spiX->FCR       = spiX->FCR | 0x80000000;

    //
    // Clear all conditions.
    //
    spiX->INT_STA   = 0xffffffff;

    //
    // Setup burst mode.
    //
    spiX->BC 	    = 0x00000001;
    spiX->BCC 	    = 0x00000000;

    printf("\n\n");
    printf("CTL=%08x\n", spiX->CTL);
    printf("INTCTL=%08x\n", spiX->INTCTL);
    printf("IER=%08x\n", spiX->IER);
    printf("INT_STA=%08x\n", spiX->INT_STA);
    printf("FCR=%08x\n", spiX->FCR);
    printf("FSR=%08x\n", spiX->FSR);
    printf("WAIT=%08x\n", spiX->WAIT);
    printf("CCTL=%08x\n", spiX->CCTL);
    printf("BC=%08x\n", spiX->BC);
    printf("TC=%08x\n", spiX->TC);
    printf("BCC=%08x\n", spiX->BCC);
    printf("\n\n");

    printf("\n\n");
    printf("CTL=%08x\n",    offsetof(SPIPort, CTL));
    printf("INTCTL=%08x\n", offsetof(SPIPort, INTCTL));
    printf("IER=%08x\n",    offsetof(SPIPort, IER));
    printf("INT_STA=%08x\n",offsetof(SPIPort, INT_STA));
    printf("FCR=%08x\n",    offsetof(SPIPort, FCR));
    printf("FSR=%08x\n",    offsetof(SPIPort, FSR));
    printf("WAIT=%08x\n",   offsetof(SPIPort, WAIT));
    printf("CCTL=%08x\n",   offsetof(SPIPort, CCTL));
    printf("BC=%08x\n",     offsetof(SPIPort, BC));
    printf("TC=%08x\n",     offsetof(SPIPort, TC));
    printf("BCC=%08x\n",    offsetof(SPIPort, BCC));
    printf("TXD=%08x\n",    offsetof(SPIPort, TXD));
    printf("RXD=%08x\n",    offsetof(SPIPort, RXD));
    printf("\n\n");

        spiX->TC 	    = 0x00000000;
        spiX->BCC 	    = 0x00000000;
        spiX->WAIT 	    = 0x00000000;
        spiX->FCR       = 0x80008000;

    //
    // CLK = 6
    // MISO = 4
    // MOSI = 2
    // = 0
    //

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

	SET_OR_CLEAR_BIT( portValue, 16,  d0 ); 	// d0 15
	SET_OR_CLEAR_BIT( portValue, 7,  d1 ); 	// d1 7
	SET_OR_CLEAR_BIT( portValue, 6, d2 ); 	// d2 6
	SET_OR_CLEAR_BIT( portValue, 0,  d3 ); 	// d3 0
	SET_OR_CLEAR_BIT( portValue, 10, d4 ); 	// d4 10
	SET_OR_CLEAR_BIT( portValue, 2,  d5 ); 	// d5 2
	SET_OR_CLEAR_BIT( portValue, 3,  d6 ); 	// d6 18
	SET_OR_CLEAR_BIT( portValue, 1,  d7 ); 	// d7 19
	
	portA->DAT 	= portValue;

}



//assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("\n");
}

//
//
//
void GetByteFromShiftRegister( volatile FastSharedBuffer* buffer, volatile SPIPort* spiX, PWMPort* pwmPort )
{
    volatile uint32_t*   pINTCTL     = &spiX->INTCTL;
    volatile uint32_t*   pINT_STA    = &spiX->INT_STA;
    volatile uint32_t*   pFCR        = &spiX->FCR;
    volatile uint32_t*   pTXD        = &spiX->TXD;
    volatile uint32_t*   pBC         = &spiX->BC;
    volatile uint32_t*   pCTL        = &spiX->CTL;
    volatile uint32_t*   pRXD        = &spiX->RXD;
    volatile uint32_t*   pIER        = &spiX->IER;
    volatile uint32_t*   pTC         = &spiX->TC;
    volatile uint32_t*   pFSR        = &spiX->FSR;
    uint32_t            temp;
    static uint32_t     wordCount   = 0;
	volatile uint32_t*  portA_DAT32   = (uint32_t*)&portA->DAT;
	volatile uint16_t*  portA_DAT16   = (uint16_t*)&portA->DAT;
	volatile uint8_t*   portA_DAT8    = (uint8_t*)&portA->DAT;

    *pCTL 	    = 0x00000001;
    *pIER       = 0;
    
    uint8_t x = 0;



    //portA->DAT  = 0;            


    pwmPort->CH0_PERIOD = (2<<16)|1;

    uint32_t    value = 0;
    
    //pwmPort->CH_CTL     = (1<<9)|(1<<6)|(1<<4) | 0xf;
    pwmPort->CH_CTL     = (1<<6)|(1<<4) | 0xf;

    spiX->CTL 	= 0x00000001;       // slave mode.
    *pINTCTL = 0x00000003;      // CS polarity bit.
    volatile uint8_t     rxValue;
    uint16_t*   portA_DAT   = (uint16_t*)&portA->DAT;
    while(true)
    {
#if 0        
        //uint32_t    currentValue    = portA->DAT;
        //SetOutputState(x);
        //x++;
        value   = portA->DAT;

        portA->DAT  = value & ~(1<<4);
        portA->DAT  = value | (1<<4);
        //pwmPort->CH_CTL     = (1<<9)|(1<<6)|(1<<4) | 0xf;
        //pwmPort->CH_CTL     = (1<<6)|(1<<4) | 0xf;

        while( ((*pFSR)&0xff) == 0 );
        //pwmPort->CH_CTL     = 0;
        //while( ((*pFSR)&0xff) > 0 )
        {
            rxValue     = *((uint8_t*)pRXD);
            //DebugPrintf("[%02x]\n",rxValue);
            //printBits(sizeof(rxValue), (void const * const )&rxValue);
            //FastSharedBufferPut( buffer, rxValue );
        }
        volatile uint8_t rr = rxValue+1;
#else
        volatile uint16_t   inputValue  = *portA_DAT16;
        inputCount++;
#endif
    }
}


//
//
//
int main()
{
    uint32_t 			start;
    uint32_t			end;

    DebugPrintf("\nReactorInlet.\n");

    spi 	= (SPIPort*)SetupSPI();
    spi0    = &spi[0];
    spi1    = &spi[1];
    volatile SPIPort* spiX    = spi1;


    //
    //
    //
    // CCU:BUS_CLK_GATING_REG0:SPI1     = PASS.
    //    
    DebugPrintf("BUS_CLK_GATING_REG0 : %08x\n", readl(0x01C20000+0x0060) );
    DebugPrintf("AHB1_APB1_CFG_REG : %08x\n", readl(0x01C20000+0x0054) );
    DebugPrintf("SPI1_CLK_REG : %08x\n", readl(0x01C20000+0x00A4) );
    DebugPrintf("CTL=%08x\n", spiX->CTL);
    DebugPrintf("INTCTL=%08x\n", spiX->INTCTL);
    DebugPrintf("IER=%08x\n", spiX->IER);
    DebugPrintf("INT_STA=%08x\n", spiX->INT_STA);
    DebugPrintf("FCR=%08x\n", spiX->FCR);
    DebugPrintf("FSR=%08x\n", spiX->FSR);
    DebugPrintf("WAIT=%08x\n", spiX->WAIT);
    DebugPrintf("CCTL=%08x\n", spiX->CCTL);
    DebugPrintf("BC=%08x\n", spiX->BC);
    DebugPrintf("TC=%08x\n", spiX->TC);
    DebugPrintf("BCC=%08x\n", spiX->BCC);


    //
    //
    //
    volatile uint8_t*   sharedMemory    = (uint8_t*)SharedMemorySlaveInitialise(0x00000001);
    strcpy( (char*)sharedMemory, "Hello World." );

    //
    //
    //
    SharedMemoryLayout*     layout  = (SharedMemoryLayout*)sharedMemory;
    volatile FastSharedBuffer*       inletToControl  = &layout->inletToControl;
    FastSharedBufferInitialiseAsWriter( inletToControl );

    //
    // Wait until we are fully connected.
    //
    DebugPrintf("Waiting for connections.\n");
    while( inletToControl->numberOfReaders == 0 );
    DebugPrintf("Connected.\n");


    //
    //
    //
    gpio 	= (GPIOPort*)SetupGPIO();
    portA	= &gpio[0];
    portC	= &gpio[1];
    portD	= &gpio[2];
    portE	= &gpio[3];
    portF	= &gpio[4];
    portG	= &gpio[5];
    portL	= &gpio[6];



	portA->CFG0 	= 0x11311111;
	portA->CFG1 	= 0x22211111;
	portA->CFG2 	= 0x11111111;
	portA->CFG3 	= 0x11111111;
	portA->DAT  	= 0xffffffff;
	portA->DRV0 	= 0x22222222;
	portA->DRV1 	= 0x22222222;
	portA->PUL0 	= 0x22222222;
	portA->PUL1 	= 0x22222222;

    //
    //
    //
    SetupSPIController( spiX );

    //
    // CCU:BUS_CLK_GATING_REG0:SPI1     = PASS.
    //
    uint32_t    clkGating     = readl(0x01C20000+0x0060); 
    writel( 0x01C20000+0x0060, clkGating | (1<<21) ); 

    //
    // CCU:SPI1_CLK_REG clock setup
    //
    //writel( 0x01C20000+0x00A4, 0x80000000 );    // 2.4MHz, OSC24M
    writel( 0x01C20000+0x00A4, 0x81000000 );        // CLK_DIV_RATIO_M=5, CLK_DIV_RATIO_N=/1, CLK_SRC_SEL=PLL_PERIPH0, SCLK_GATING:ON Clock Source/Divider N/Divider M
    spiX->CCTL 	= 0x0000100b;       // 25 MHz SPI clock.
    //spiX->CCTL 	= 0x00001010;           // 16.7 MHz SPI clk.
    spiX->IER   = 0;    

    //
    //
    //
    PWMPort* pwmPort    = SetupPWM();
    pwmPort->CH_CTL     = 0;
    pwmPort->CH0_PERIOD = (4096<<16)|4096;
    //sleep(1);
    //pwmPort->CH_CTL     = (1<<9)|(1<<6)|(1<<4)|(1<<20)|(1<<23) | 0xf;
    pwmPort->CH_CTL     = (1<<6)|(1<<4) | 0x6;
    pwmPort->CH0_PERIOD = (0xffff<<16)|0xffff;

    static pthread_t    threadId;
    pthread_create( &threadId, NULL, &doSomeThing, NULL);
    DebugPrintf(" Started display thread\n ");


    while(true)
    {
        GetByteFromShiftRegister( inletToControl, spiX, pwmPort);
    }
}


