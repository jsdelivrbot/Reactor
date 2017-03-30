//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//




#ifndef __PULSEWIDTHMODULATOR_HPP__
#define __PULSEWIDTHMODULATOR_HPP__

#include <stdint.h>


template <uint32_t period, uint32_t ticksPerBit, uint8_t txMask, typename BufferType>
class PWM
{
public:

    PWM(BufferType& _cmdFIFO) :
        cmdFIFO(_cmdFIFO)
    {
    }

    uint32_t GetPeriod()
    {
        return period;
    }

    void ProcessNegativeEdge()
    {
    }

    void ProcessPositiveEdge()
    {
    }


    void PeriodicProcessing( uint8_t inputValue, uint8_t& outputValue )
    {
        bool    dataAvailable   = false;
        uint8_t currentCmd = cmdFIFO.NonBlockingGet(dataAvailable);

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

    BufferType& cmdFIFO;
};


#endif
