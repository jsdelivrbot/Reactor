//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//



#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <cstdint>
#include <stdio.h>
#include <stdint.h>

extern "C"
{
 #include "DebugText.h"
 #include "Timestamp.h"
}

typedef uint8_t     uint8x8_t[8];
typedef uint64_t    uint64x8_t[8];



//
//
//
template <  uint32_t iterationDuration, 
            typename Schedulee1Type,
            typename Schedulee2Type,
            typename Schedulee3Type,
            typename Schedulee4Type,
            typename Schedulee5Type,
            typename Schedulee6Type,
            typename Schedulee7Type,
            typename Schedulee8Type
            >
class Scheduler
{

public:

    Scheduler(  Schedulee1Type& _schedulee1,
                Schedulee2Type& _schedulee2,
                Schedulee3Type& _schedulee3,
                Schedulee4Type& _schedulee4,
                Schedulee5Type& _schedulee5,
                Schedulee6Type& _schedulee6,
                Schedulee7Type& _schedulee7,
                Schedulee8Type& _schedulee8
                ) :
            schedulee1(_schedulee1),
            schedulee2(_schedulee2),
            schedulee3(_schedulee3),
            schedulee4(_schedulee4),
            schedulee5(_schedulee5),
            schedulee6(_schedulee6),
            schedulee7(_schedulee7),
            schedulee8(_schedulee8)
    {

    }


    //
    // 8xTimeslots, each 24MHz/8 in duration = 8 channels at 1MHz.
    //
    void PeriodicProcessing( uint8_t inputValue, uint8_t& outputValue )
    {
        static uint32_t     cycleCount  = 0;
        uint32_t            timestamp = cycleCount;
        cycleCount++;

#define PROCESS_SCHEDULEE(bitNumber, s)                                         \
        bits[bitNumber]     = (inputValue & (1<<bitNumber)) >> bitNumber;       \
        if( (bits[bitNumber] == 0) && (previousBits[bitNumber] == 1) )          \
        {                                                                       \
            s.ProcessNegativeEdge(timestamp);                                   \
        }                                                                       \
        if( (bits[bitNumber] == 1) && (previousBits[bitNumber] == 0) )          \
        {                                                                       \
            s.ProcessPositiveEdge(timestamp);                                   \
        }                                                                       \
        if( (timestamp-previousTimestamps[bitNumber]) >= s.GetPeriod() )        \
        {                                                                       \
            s.PeriodicProcessing( timestamp, inputValue, outputValue );         \
            previousTimestamps[bitNumber]   = timestamp;                        \
        }                                                                       \
        previousBits[bitNumber]     = bits[bitNumber];        

        PROCESS_SCHEDULEE(0, schedulee1);
        PROCESS_SCHEDULEE(1, schedulee2);
        PROCESS_SCHEDULEE(2, schedulee3);
        PROCESS_SCHEDULEE(3, schedulee4);
        PROCESS_SCHEDULEE(4, schedulee5);
        PROCESS_SCHEDULEE(5, schedulee6);
        PROCESS_SCHEDULEE(6, schedulee7);
        PROCESS_SCHEDULEE(7, schedulee8);
    }

private:

    Schedulee1Type&     schedulee1;
    Schedulee2Type&     schedulee2;
    Schedulee3Type&     schedulee3;
    Schedulee4Type&     schedulee4;
    Schedulee5Type&     schedulee5;
    Schedulee6Type&     schedulee6;
    Schedulee7Type&     schedulee7;
    Schedulee8Type&     schedulee8;

    volatile uint8x8_t           bits;
    volatile uint8x8_t           previousBits;
    volatile uint64x8_t          previousTimestamps;

};

////////////////////////////////////////////////////////////////////////////



#endif



