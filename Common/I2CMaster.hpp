//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, March 2017.
//



#ifndef __I2CMASTER_HPP__
#define __I2CMASTER_HPP__

#include <stdint.h>



template <uint32_t period, uint32_t ticksPerBit, uint8_t sdaMask, uint8_t sclMask>
class I2CMaster
{
public:

    uint32_t GetPeriod()
    {
        return period;
    }

    void ProcessNegativeEdge( uint32_t timestamp )
    {
        currentLevel    = 0;
    }

    void ProcessPositiveEdge( uint32_t timestamp )
    {
        currentLevel    = 1;
    }


    void SetSCLLow(uint8_t& output)
    {
        output  &= ~sclMask;
    }

    void SetSCLHigh(uint8_t& output)
    {
        output  |= sclMask;
    }

    void SetSDALow(uint8_t& output)
    {
        output  &= ~sdaMask;
    }

    void SetSDAHigh(uint8_t& output)
    {
        output  |= sdaMask;
    }


    void PeriodicProcessing( uint32_t timestamp, uint8_t inputValue, uint8_t& outputValue )
    {

        #define SET_SDA_ACCORDING_TO_BIT_NUMBER(bitMask) \
                if( (currentByte&bitMask) != 0)          \
                {                                        \
                    SetSDALow(outputValue);                         \
                }                                        \
                else                                     \
                {                                        \
                    SetSDAHigh(outputValue);                        \
                }

        switch(state)
        {
            case 0: 
                SetSDALow(outputValue);    // SCL is already high from previous. Generate START condition (-ve edge on SDA while SCL high)
                state   = 1;
                break;

            case 1: 
                SetSCLLow(outputValue);
                state   = 2;
                break;

            case 2:             // bit 7
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x80 );
                state   = 3;
                break;

            case 3: 
                SetSCLHigh(outputValue);
                state   = 4;
                break;

            case 4: 
                SetSCLLow(outputValue);
                state   = 5;
                break;

            case 5:             // bit 6
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x40 );
                state   = 6;
                break;

            case 6: 
                SetSCLHigh(outputValue);
                state   = 7;
                break;

            case 7: 
                SetSCLLow(outputValue);
                state   = 8;
                break;

            case 8:             // bit 5
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x20 );
                state   = 9;
                break;

            case 9: 
                SetSCLHigh(outputValue);
                state   = 10;
                break;

            case 10: 
                SetSCLLow(outputValue);
                state   = 11;
                break;

            case 11:             // bit 4
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x10 );
                state   = 12;
                break;

            case 12: 
                SetSCLHigh(outputValue);
                state   = 13;
                break;

            case 13: 
                SetSCLLow(outputValue);
                state   = 14;
                break;

            case 14:             // bit 3
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x08 );
                state   = 15;
                break;

            case 15: 
                SetSCLHigh(outputValue);
                state   = 16;
                break;

            case 16: 
                SetSCLLow(outputValue);
                state   = 17;
                break;

            case 17:             // bit 2
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x04 );
                state   = 18;
                break;

            case 18: 
                SetSCLHigh(outputValue);
                state   = 19;
                break;

            case 19: 
                SetSCLLow(outputValue);
                state   = 20;
                break;

            case 20:             // bit 1
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x02 );
                state   = 21;
                break;

            case 21: 
                SetSCLHigh(outputValue);
                state   = 22;
                break;

            case 22: 
                SetSCLLow(outputValue);
                state   = 23;
                break;

            case 23:             // bit 0
                SET_SDA_ACCORDING_TO_BIT_NUMBER( 0x01 );
                state   = 24;
                break;

            case 24: 
                SetSCLHigh(outputValue);
                state   = 25;
                break;

            case 25: 
                SetSCLLow(outputValue);
                state   = 26;
                break;

            case 26:             // ACK bit
                // stop driving SDA so we can detect ACK state in next state.
                SetSCLHigh(outputValue);
                state   = 27;
                break;

            case 27:             
                ack     = currentLevel;
                // drive SDA again now we have detected ACK.
                state   = 28;
                break;

            case 28:            // STOP condition.   SDA +ve edge while SCL high.
                SetSDAHigh(outputValue);
                state   = 29;
                break;

            case 30:            // Pause, then restart.
                state   = 0;
                break;

        }

    }

    uint8_t     currentByte             = 0;
    uint8_t     currentLevel            = 0;
    uint8_t     state                   = 0;
    bool        ack                     = 0;
};

#endif


