//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//





#ifndef __NOOPERATION_HPP__
#define __NOOPERATION_HPP__

#include <stdint.h>

class NoOperation
{
public:

    uint32_t GetPeriod()
    {
        return 0xffffffff;
    }

    void ProcessNegativeEdge( uint32_t timestamp )
    {
    }

    void ProcessPositiveEdge( uint32_t timestamp )
    {
    }

    void PeriodicProcessing( uint32_t timestamp, uint8_t inputValue, uint8_t& outputValue )
    {
    }
};



#endif

