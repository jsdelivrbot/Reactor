//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//


#ifndef __FASTSHAREDBUFFER_HPP__
#define __FASTSHAREDBUFFER_HPP__


#include "Reactor.h"



//
//
//
#define ISB	__asm__ volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define DSB	__asm__ volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define DMB	__asm__ volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))



template <typename ContainedType, typename IndexType>
class FastSharedBuffer
{
public:

    void InitialiseAsReader()
    {
        numberOfReaders++;
        DSB;
    }

    void InitialiseAsWriter()
    {
        tail    = 0;
        head    = 0;
        numberOfWriters++;
        DSB;
    }

    void Put(ContainedType value)
    {
        DSB;
        IndexType    newHead     = head + 1;

        //
        // Wait until there is space in the buffer.
        //
        DSB;
        while(newHead == tail);
    
        //
        // Put the data in the buffer.
        //
        data[head]  = value;
        head    = newHead;
        DSB;
    }


    ContainedType Get()
    {
        //
        // Wait until there is data in the buffer.
        //
        DSB;
        while(head == tail);

        //
        // Get the data out of the buffer.
        //
        ContainedType     value   = data[tail];
        tail++;
        DSB;

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


