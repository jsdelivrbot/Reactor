



#include <stdint.h>
#include <stdbool.h>
#include "UARTTransmitter.h"
#include "MemoryPool.h"



MEMORYPOOL(Small,  16,   256);
MEMORYPOOL(Medium, 64,   64);
MEMORYPOOL(Large,  256,  16);


//
//
//
void main()
{
    void*   a   = AllocateFromPool_Small();    
    void*   b   = AllocateFromPool_Medium();    
    void*   c   = AllocateFromPool_Large();

    ReleaseToPool_Small( a );    
    ReleaseToPool_Medium( b );    
    ReleaseToPool_Large( c );    
}


