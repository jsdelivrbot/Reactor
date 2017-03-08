//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//


#ifndef __FASTSHAREDBUFFER_HPP__
#define __FASTSHAREDBUFFER_HPP__


#include "Reactor.h"

template <typename ContainedType, typename IndexType>
class FastSharedBuffer
{
public:

    void InitialiseAsReader()
    {
        numberOfReaders++;
    }

    void InitialiseAsWriter()
    {
        tail    = 0;
        head    = 0;
        numberOfWriters++;
    }

    void Put(ContainedType value)
    {
        IndexType    newHead     = head + 1;

        //
        // Wait until there is space in the buffer.
        //
        //while(newHead == tail);
    
        //
        // Put the data in the buffer.
        //
        data[head]  = value;
        head    = newHead;
    }


    ContainedType Get()
    {
        //
        // Wait until there is data in the buffer.
        //
        //while(head == tail);

        //
        // Get the data out of the buffer.
        //
        ContainedType     value   = data[head];
        tail++;

        return value;
    }

private:
public:

    volatile ContainedType   data[2^sizeof(IndexType)];
    volatile IndexType       head;
    volatile IndexType       tail;
    volatile uint32_t        numberOfReaders;
    volatile uint32_t        numberOfWriters;

};


#endif


