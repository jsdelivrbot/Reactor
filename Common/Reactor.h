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
typedef struct 
{
    FastSharedBuffer<uint8_t,uint16_t>    inletToControl;
    FastSharedBuffer<uint8_t,uint16_t>    controlToOutlet;
    FastSharedBuffer<uint8_t,uint16_t>    controlToServer;

} SharedMemoryLayout;






#endif


