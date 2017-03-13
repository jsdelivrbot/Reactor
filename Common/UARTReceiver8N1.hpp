//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//



#ifndef __UARTRECEIVER8N1_HPP__
#define __UARTRECEIVER8N1_HPP__

#include <stdint.h>



template <uint32_t period, uint32_t ticksPerBit, uint8_t rxMask, uint32_t fifoDepth>
class UARTReceiver8N1
{
public:

    uint32_t GetPeriod()
    {
        return period;
    }

    void ProcessNegativeEdge( uint32_t timestamp )
    {
        if(startDetected == false)
        {
            //
            // Start of byte.
            //
            startDetected       = true;
            timestampOfStartBit = timestamp;
            bitNumber 	        = 0;
            currentByte         = 0;
        }
        else
        {
            //
            // Within byte.
            //
        }

        currentLevel    = 0;
    }

    void ProcessPositiveEdge( uint32_t timestamp )
    {
        currentLevel    = 1;
    }


    void PeriodicProcessing( uint32_t timestamp, uint8_t inputValue, uint8_t& outputValue )
    {
        currentByte <<= 1;
        currentByte |= currentLevel;

        bitNumber++;
        if(bitNumber >= 10)
        {
            startDetected 	= false;
        }
    }

    uint32_t    timestampOfStartBit     = 0;
    bool        startDetected           = false;
    uint8_t     currentByte             = 0;
    uint8_t     currentLevel            = 0;
    uint8_t     bitNumber               = 0;
};

#endif


