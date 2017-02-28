//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//



#ifndef __REACTOR_H__
#define __REACTOR_H__


#include <stdint.h>


//
//
//
#define ISB	__asm__ volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define DSB	__asm__ volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define DMB	__asm__ volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))


//
//
//
typedef struct
{
    uint8_t     data[0xffff];
    uint16_t    head;
    uint16_t    tail;
    uint32_t    numberOfReaders;
    uint32_t    numberOfWriters;

} FastSharedBuffer;


void FastSharedBufferInitialiseAsReader( volatile FastSharedBuffer* buffer );
void FastSharedBufferInitialiseAsWriter( volatile FastSharedBuffer* buffer );
void FastSharedBufferPut( volatile FastSharedBuffer* buffer, uint8_t value );
uint8_t FastSharedBufferGet( volatile FastSharedBuffer* buffer );




//
//
//
typedef struct 
{
    FastSharedBuffer    inletToControl;
    FastSharedBuffer    controlToOutlet;
    FastSharedBuffer    controlToServer;

} SharedMemoryLayout;




// InletToControl = 1000->2000;
// ControlToOutlet = 2000->3000;
// ControlToServer = 3000->13000;


//
//
//
typedef struct
{
    uint32_t    timestamp;
    uint32_t    data[64];

} DataFromInlet;


//
//
//
typedef struct
{
    uint32_t    data[64];

} DataToOutlet;



//
//
//
typedef struct
{
    uint32_t    timestamp;
    uint32_t    data[64*10];

} DataToServer;




#endif


