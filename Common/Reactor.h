


#ifndef __REACTOR_H__
#define __REACTOR_H__


#include <stdint.h>



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


