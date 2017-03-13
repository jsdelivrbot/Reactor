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


typedef uint8_t     uint8x8_t[8];



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

    void PeriodicProcessing( uint32_t timestamp, uint8_t inputValue, uint8_t& outputValue )
    {
        static uint8x8_t    bits;
        static uint8x8_t    previousBits;

#define PROCESS_SCHEDULEE(bitNumber, s)                                         \
        bits[bitNumber]     = (inputValue & (1<<bitNumber)) >> bitNumber;       \
        if( (bits[bitNumber] == 0) && (previousBits[bitNumber == 1]) )          \
        {                                                                       \
            s.ProcessNegativeEdge(timestamp);                                   \
        }                                                                       \
        if( (bits[bitNumber] == 1) && (previousBits[bitNumber == 0]) )          \
        {                                                                       \
            s.ProcessPositiveEdge(timestamp);                                   \
        }                                                                       \
        if( (timestamp%s.GetPeriod()) == 0)                                     \
        {                                                                       \
            s.PeriodicProcessing( timestamp, inputValue, outputValue );         \
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

};

////////////////////////////////////////////////////////////////////////////



#endif






////////////////////////////////////////////////////////////////////////////

#if 0


//
//
//
int main()
{
    typedef UARTTransmitter8N1<10,3, 0x01, 1024>    TxType;
    typedef UARTReceiver8N1<8,3, 0x02, 1024>        RxType;
    typedef PWM<30,30, 0x4>                         PWMType;
    TxType          one;
    RxType          two;
    NoOperation     nop;
    PWMType         pwm;
    Scheduler<  100, 
                RxType, 
                RxType,
                RxType,
                RxType,
                RxType,
                RxType,
                RxType,
                RxType >  scheduler(two,two, two, two, two,two, two, two);


    uint8x8_t   bits            = {0};
    uint8x8_t   previousBits    = {0};

    for(uint32_t i=0; i<250000000; i++)
    {
        static uint8_t 	inputValues[]	= {0x08, 0x40,0x41,0x48,0x04,0x81,0x08,0x14,0x18,0x01,0x81,0x80,0x80,0x18,0x40,0x04,0x18,0x04,0x00,0x40};
        uint32_t 	inputIndex 	= 0;
        uint32_t    timestamp       = 0;
        uint8_t     outputValue     = 0;
        uint8_t     inputValue      = inputValues[inputIndex];

        scheduler.PeriodicProcessing( timestamp, 0xab, outputValue );
        static volatile uint8_t 	r = outputValue;

        timestamp++;
    inputIndex++;
    }
}


#endif


