//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//



#ifndef __REACTOR_H__
#define __REACTOR_H__


#include <stdint.h>
#include "FastSharedBuffer.hpp"


//
//
//
#define ISB	__asm__ volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define DSB	__asm__ volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define DMB	__asm__ volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))





//
//
//
typedef struct 
{
    FastSharedBuffer<uint8_t,uint16_t>    inletToControl;
    FastSharedBuffer<uint8_t,uint16_t>    controlToOutlet;
    FastSharedBuffer<uint8_t,uint16_t>    controlToServer;

} SharedMemoryLayout;






#endif


