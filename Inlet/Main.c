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
#include <stddef.h>
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


//
//
//
int main()
{
    uint32_t 			start;
    uint32_t			end;

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

    spi 	= (SPIPort*)SetupSPI();
    spi0    = &spi[0];
    spi1    = &spi[1];

    //
    // CCU:BUS_CLK_GATING_REG0:SPI1     = PASS.
    //
    uint32_t    clkGating     = readl(0x01C20000+0x0060); 
    writel( 0x01C20000+0x0060, clkGating | (1<<21) ); 
    printf("BUS_CLK_GATING_REG0:SPI1 = %08x\n", readl(0x01C20000+0x0060));

    //
    // CCU:SPI1_CLK_REG clock setup
    //
    writel( 0x01C20000+0x00A4, 0x80000000 );    // 2.4MHz, OSC24M
    //writel( 0x01C20000+0x00A4, 0x81000000 );    // 12MHz, PLL_PERIPH0
    //writel( 0x01C20000+0x00A4, 0x82000000 );    // 12MHz, PLL_PERIPH1
    //writel( 0x01C20000+0x00A4, 0x82030003 );    // 2MHz, PLL_PERIPH1
    printf("SPI1_CLK_REG = %08x\n", readl(0x01C20000+0x00A4));
    
    //
    // Pin setup.
    //
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

    portA->CFG1 	&= ~0xfff00000;
    portA->CFG1 	|=  0x22200000;

    portA->CFG2 	&= ~0x0000000f;
    portA->CFG2 	|=  0x00000002;

    //
    // spiX port setup.
    //
    volatile SPIPort* spiX    = spi1;
    spiX->CTL 	= 0x00000083;
    spiX->INTCTL = 0x000001c4;
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
    spiX->BCC 	    = 0x00000001;

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

    uint32_t 	i 	= 0;
    while(true)
    {
        //printf("FSR=%08x\n", spiX->FSR);
        //printf("INT_STA=%08x\n", spiX->INT_STA);

        //portA->DAT      = 0xffffffff;

        //
        // clear down all status flags.
        //
        spiX->INT_STA   = 0xffffffff;

        //
        // reset the FIFOs and write the data into the FIFO.
        //
        spiX->FCR       = 0x80008000;
        //uint32_t*       txFIFO  = (uint32_t*)&spiX->TXD;
        spiX->TXD 	= 0xff;
        //spiX->TXD 	= 0x2;
        //spiX->TXD 	= 0xee;
        //spiX->TXD 	= 0x4;
        //spiX->TXD 	= 0x01234567;

        //
        //
        //
        spiX->BC 	    = 0x00000001;
        spiX->CTL 	    = 0x00000003;

        //
        // Set XCHG and wait for it to complete.
        // also set chip-select polarity to generate a pulse prior to the clks to act as a parallel-load pulse for the shift register.
        //
        spiX->INTCTL = 0x80000000;
        while( (spiX->INT_STA&0x00001000) == 0)
        {
            //printf("  FSR=%08x\n", spiX->FSR);
            //printf("  CTL=%08x\n", spiX->CTL);
            //printf("2  INT_STA=%08x\n", spiX->INT_STA);
            //sleep(1);        
        }
        //portA->DAT      = 0x00000000;

        //
        // Read from the Rx FIFO while it isn't empty.
        //
        
        //while( (spiX->FSR&0x0000000f) >= 0 )

        do
        {
            //printf("3  INT_STA=%08x\n", spiX->INT_STA);
            volatile uint32_t    rxValue     = spiX->RXD;
            //printf("FSR=%08x\n", spiX->FSR);
            //printf("RXD=%08x\n", rxValue);
        } while( (spiX->INT_STA&0x00000002) == 0 );


        //i++;
        //sleep(1);

        //
        // Get the current timestamp.
        //
        //uint32_t    timestamp 	= rdtsc32();
    }

}


