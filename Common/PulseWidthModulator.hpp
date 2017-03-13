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
        if(timestamp > nextBitTimestamp)
        {
            nextBitTimestamp    = timestamp + ticksPerBit;

            switch(bitNumber)
            {
                case 0:     // ON
                    SetTxLow( outputValue );
                    break;

                case 1:     // OFF
                    SetTxHigh( outputValue );
                    break;
            }

            bitNumber           = (bitNumber+1) & 0x1;
        }
    }

    void SetTxHigh( uint8_t& outputValue )
    {
        outputValue     |= txMask;
    }

    void SetTxLow( uint8_t& outputValue )
    {
        outputValue     &= ~txMask;
    }

    void SetTx( uint8_t zeroToClearFlag, uint8_t& outputValue )
    {
        if( zeroToClearFlag == 0 )
        {
            outputValue     &= ~txMask;
        }
        else
        {
            outputValue     |= txMask;            
        }
    }

    uint32_t    nextBitTimestamp    = 0;
    uint32_t    bitNumber           = 0;
};


#endif