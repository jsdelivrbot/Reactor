//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//





#ifndef __UARTTRANSMITTER8N1_HPP__
#define __UARTTRANSMITTER8N1_HPP__

#include <stdint.h>



template <uint32_t period, uint32_t ticksPerBit, uint8_t txMask, uint32_t fifoDepth>
class UARTTransmitter8N1
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
        switch(bitNumber)
        {
            case 0:     // START
                SetTxLow( outputValue );
                break;

            case 1:     // D0
                SetTx( currentByte & 0x01, outputValue );
                break;

            case 2:     // D1
                SetTx( currentByte & 0x02, outputValue );
                break;

            case 3:     // D2
                SetTx( currentByte & 0x04, outputValue );
                break;

            case 4:     // D3
                SetTx( currentByte & 0x08, outputValue );
                break;

            case 5:     // D4
                SetTx( currentByte & 0x10, outputValue );
                break;

            case 6:     // D5
                SetTx( currentByte & 0x20, outputValue );
                break;

            case 7:     // D6
                SetTx( currentByte & 0x40, outputValue );
                break;

            case 8:     // D7
                SetTx( currentByte & 0x80, outputValue );
                break;

            case 9:     // STOP
                SetTxHigh( outputValue );
                currentByte     = fifo[fifoTail];
                fifoTail++;
                byteStartTimestamp  = timestamp;
                break;
        }

        bitNumber           = (bitNumber+1) & 0x7;

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

    void PutIntoFIFO(uint8_t value)
    {
        fifo[fifoHead]  = value;
        fifoHead    = (fifoHead+1)%fifoDepth;
    }

    uint32_t    byteStartTimestamp  = 0;
    uint32_t    nextBitTimestamp    = 0;
    uint32_t    bitNumber           = 0;
    uint8_t     currentByte;

    uint32_t    fifoHead            = 0;
    uint32_t    fifoTail            = 0;
    uint8_t     fifo[fifoDepth];
};


#endif



