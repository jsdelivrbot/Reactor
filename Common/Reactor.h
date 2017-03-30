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

typedef FastSharedBuffer<uint8_t,uint8_t>   ChannelBufferType;


//
//
//
typedef struct 
{
    BufferType    inletToControl;
    BufferType    controlToOutlet;
    BufferType    controlToServer;

    ChannelBufferType   channel0Command;
    ChannelBufferType   channel0In;
    ChannelBufferType   channel0Out;

    ChannelBufferType   channel1Command;
    ChannelBufferType   channel1In;
    ChannelBufferType   channel1Out;

    ChannelBufferType   channel2Command;
    ChannelBufferType   channel2In;
    ChannelBufferType   channel2Out;

    ChannelBufferType   channel3Command;
    ChannelBufferType   channel3In;
    ChannelBufferType   channel3Out;

    ChannelBufferType   channel4Command;
    ChannelBufferType   channel4In;
    ChannelBufferType   channel4Out;

    ChannelBufferType   channel5Command;
    ChannelBufferType   channel5In;
    ChannelBufferType   channel5Out;

    ChannelBufferType   channel6Command;
    ChannelBufferType   channel6In;
    ChannelBufferType   channel6Out;

    ChannelBufferType   channel7Command;
    ChannelBufferType   channel7In;
    ChannelBufferType   channel7Out;

} SharedMemoryLayout;





#endif


