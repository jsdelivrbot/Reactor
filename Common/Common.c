//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//




#include "Reactor.h"




void FastSharedBufferInitialiseAsReader( volatile FastSharedBuffer* buffer )
{
    buffer->numberOfReaders++;
}


void FastSharedBufferInitialiseAsWriter( volatile FastSharedBuffer* buffer )
{
    buffer->tail    = 0;
    buffer->head    = 0;
    buffer->numberOfWriters++;
}


void FastSharedBufferPut( volatile FastSharedBuffer* buffer, uint8_t value )
{
    uint16_t    newHead     = buffer->head + 1;

    while(newHead == buffer->tail);

    buffer->data[buffer->head]  = value;
    buffer->head    = newHead;
}


uint8_t FastSharedBufferGet( volatile FastSharedBuffer* buffer )
{
    //
    //
    //
    while(buffer->head == buffer->tail);

    //
    //
    //
    uint8_t     value   = buffer->data[buffer->head];
    buffer->tail++;

    return value;
}

