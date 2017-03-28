//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//


#ifndef __FASTSHAREDBUFFER_HPP__
#define __FASTSHAREDBUFFER_HPP__


#include <stdint.h>



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
        tail    = 0;
        head    = 1;
        numberOfReaders++;
        DSB;
    }

    void InitialiseAsWriter()
    {
        tail    = 0;
        head    = 1;
        numberOfWriters++;
        DSB;
    }

    void Put(ContainedType value)
    {
        IndexType    newHead     = head + 1;

        //
        // Wait until there is space in the buffer.
        //
        while(newHead == tail);
    
        //
        // Put the data in the buffer.
        //
        data[head]  = value;
        head    = newHead;
        //DSB;
    }


    ContainedType Get()
    {
        IndexType   newTail     = tail + 1;

        //
        // Wait until there is data in the buffer.
        //
        while(head == tail);

        //
        // Get the data out of the buffer.
        //
        ContainedType     value   = data[tail];
        tail    = newTail;
        //DSB;

        return value;
    }

    ContainedType NonBlockingGet(bool& dataAvailable)
    {
        IndexType   newTail     = tail + 1;

        //
        // Wait until there is data in the buffer.
        //
        if(head == tail)
        {
            dataAvailable   = false;
            return 0;
        }
        else
        {
            //
            // Get the data out of the buffer.
            //
            ContainedType     value   = data[tail];
            tail    = newTail;
            dataAvailable   = true;

            return value;
        }

    }

    bool DataAvailable()
    {
        if(head == tail)
        {
            return false;
        }

        return true;
    }

private:
public:

    volatile ContainedType   data[1<<(sizeof(IndexType)*8)];
    volatile IndexType       head;
    volatile IndexType       tail;
    volatile uint32_t        numberOfReaders;
    volatile uint32_t        numberOfWriters;

};


#endif


