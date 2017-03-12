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



typedef FastSharedBuffer<uint8_t,uint8_t>   BufferType;


//
//
//
typedef struct 
{
    BufferType    inletToControl;
    BufferType    controlToOutlet;
    BufferType    controlToServer;

} SharedMemoryLayout;





#endif


