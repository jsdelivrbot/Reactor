//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//




#ifndef __PULSEWIDTHMODULATOR_HPP__
#define __PULSEWIDTHMODULATOR_HPP__

#include <stdint.h>


template <uint32_t period, uint32_t ticksPerBit, uint8_t txMask>
class PWM
{
public:

    uint32_t    deltas[256];
    uint32_t    numberOfDeltas  = 0;

    uint32_t GetPeriod()
    {
        return period;
    }

    void ProcessNegativeEdge( uint32_t timestamp )
    {
    }

    void ProcessPositiveEdge( uint32_t timestamp )
    {
    }


    void PeriodicProcessing( uint32_t timestamp, uint8_t inputValue, uint8_t& outputValue )
    {
#if 0
        static uint32_t     previousTimestamp = 0;
        if(timestamp>previousTimestamp)
        {
            deltas[numberOfDeltas]  = timestamp - previousTimestamp;
        }
        else
        {
            deltas[numberOfDeltas]  = timestamp + (0xffffffff-previousTimestamp);
        }
        previousTimestamp   = timestamp;
        numberOfDeltas  = (numberOfDeltas + 1)&0xff;
#endif

        if( (bitNumber&0x01) == 0 )
        {
            SetTxLow( outputValue );
        }
        else
        {
            SetTxHigh( outputValue );
        }
        
        bitNumber++;
    }

    void SetTxHigh( uint8_t& outputValue )
    {
        outputValue     |= txMask;
    }

    void SetTxLow( uint8_t& outputValue )
    {
        outputValue     &= ~txMask;
    }


    uint32_t    nextBitTimestamp    = 0;
    uint32_t    bitNumber           = 0;
};


#endif
