//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//





#include <stdint.h>
#include <stdbool.h>
#include "Alloy.h"
#include "ErrorHandling.h"
#include "BoardSupport.h"
#include "TextFormatter.h"
#include "DebugText.h"




GlobalData* Globals();


//
//
//
bool IsThereMailFromCore(uint32_t fromID)
{
    uint32_t    coreID              = CoreNumber();
    uint32_t    mailboxAddress      = 0x400000c0 + (0x10*coreID);;
    uint32_t    mailboxSource       = *(uint32_t*)mailboxAddress;

    if( (mailboxSource&(1<<fromID)) != 0)
    {
        return true;
    }
    else
    {
        return false;
    }

}


//
//
//
CoreMessage* NextMessage()
{
    if(Globals()->numberOfMessagesAvailable > 0)
    {
        return &Globals()->message;
    }
    else
    {
        return 0;
    }
}

//
//
//
void ReleaseMessage(CoreMessage* msg)
{
    Globals()->numberOfMessagesAvailable--;
}


//
//
//
void ClearMailboxFromCore(uint32_t fromID)
{
    uint32_t    mailboxClearAddress;
    uint32_t    coreID  = CoreNumber();

    mailboxClearAddress     = 0x400000c0 + (0x10*coreID);
    *(uint32_t*)mailboxClearAddress     = 1<<fromID;
}

//
//
//
void EnableMailboxFromCore()
{
    uint32_t    coreID  = CoreNumber();
    uint32_t    mailboxInterruptControlAddress  = 0x40000050+(coreID*4);
    uint32_t    currentSettings     = *(uint32_t*)mailboxInterruptControlAddress;

    currentSettings |= 0x0000000f;
    *(uint32_t*)mailboxInterruptControlAddress   = currentSettings;
}


//
//
//
void ProcessMessage(CoreMessage* msg)
{
    uint32_t    coreID  = CoreNumber();
    bridge->messageCounts[coreID]++;

    //
    //
    //
    if(msg->type == CORE_MESSAGE_RESET)
    {
        //CWRR();
    }

    if(msg->type == CORE_MESSAGE_TEST)
    {
    }
#if 0
    if(coreID == 2)
    {
        TriggerMailboxInterrupt( 3 );        
    }
    if(coreID == 3)
    {
        TriggerMailboxInterrupt( 1 );        
    }
#endif    
}


//
//
//
void  __attribute__ ((interrupt ("IRQ"))) IRQHandler()
{
    uint32_t    coreID  = CoreNumber();

    //
    //
    //
    if(IsThereMailFromCore(0) == true)
    {
        Globals()->message.type    = bridge->coreMessages[coreID][0].type;
        Globals()->message.payload = bridge->coreMessages[coreID][0].payload;
        Globals()->numberOfMessagesAvailable++;

        ClearMailboxFromCore( 0 );
    }

    if(IsThereMailFromCore(1) == true)
    {
        Globals()->message.type    = bridge->coreMessages[coreID][1].type;
        Globals()->message.payload = bridge->coreMessages[coreID][1].payload;
        Globals()->numberOfMessagesAvailable++;
        
        ClearMailboxFromCore( 1 );
    }

    if(IsThereMailFromCore(2) == true)
    {
        Globals()->message.type    = bridge->coreMessages[coreID][2].type;
        Globals()->message.payload = bridge->coreMessages[coreID][2].payload;
        Globals()->numberOfMessagesAvailable++;
        
        ClearMailboxFromCore( 2 );
    }

    if(IsThereMailFromCore(3) == true)
    {
        Globals()->message.type    = bridge->coreMessages[coreID][3].type;
        Globals()->message.payload = bridge->coreMessages[coreID][3].payload;
        Globals()->numberOfMessagesAvailable++;
        
        ClearMailboxFromCore( 3 );
    }

}




//
//
//
void DebugText(char* text)
{
#if 0
    uint32_t    coreID  = CoreNumber();

    SystemCall  systemCall  = 
    {
        .type           = 0x00000001,
        .payload        = (uint32_t)text,
        .processedFlag  = false,
    };

    bridge->coreMessages[0][coreID].type    = 123;
    bridge->coreMessages[0][coreID].payload = (uint32_t)&systemCall;
    FlushCache();
    TriggerMailboxInterrupt(0);            

    while( systemCall.processedFlag == false );    
#else
    DebugTextOutput(text);    
#endif    
}


//
//
//
void Core1Main(uint32_t coreID)
{
    //
    //
    //
    bridge->heartBeats[coreID]   = 0;

    //
    // Enable the malbox interrupt.
    //
    EnableMailboxFromCore();

    EnableInterrupts();

    //
    //
    //
    while(true)    
    {
        bridge->heartBeats[coreID]++;
        FlushCache();
        if( (bridge->heartBeats[coreID] % 0x4ffff) == 0 )
        {
            //
            //
            //
            char    string[64];
            FormatText(string, sizeof(string), "Count on core %d is %d", coreID, bridge->heartBeats[coreID] );
            DebugText( &string[0] );

            TriggerMailboxInterrupt(2);            
        }

        //
        //
        //
        CoreMessage*    msg     = NextMessage();
        if(msg != 0)
        {
            ProcessMessage( msg );
            DebugText("Message Received!");
            ReleaseMessage( msg );
        }
    }        
}

//
//
//
void Core2Main(uint32_t coreID)
{
    //
    //
    //
    bridge->heartBeats[coreID]   = 0;

    //
    // Enable the malbox interrupt.
    //
    EnableMailboxFromCore();

    EnableInterrupts();

    //
    //
    //
    while(true)    
    {
        bridge->heartBeats[coreID]++;
        FlushCache();
        if( (bridge->heartBeats[coreID] % 0x4ffff) == 0 )
        {
            //
            //
            //
            char    string[64];
            FormatText(string, sizeof(string), "Count on core %d is %d", coreID, bridge->heartBeats[coreID] );
            DebugText( &string[0] );
        }

        //
        //
        //
        CoreMessage*    msg     = NextMessage();
        if(msg != 0)
        {
            ProcessMessage( msg );
            DebugText("Message Received!");
            TriggerMailboxInterrupt(3);            
            ReleaseMessage( msg );
        }
    }        
}

//
//
//
void Core3Main(uint32_t coreID)
{
    //
    //
    //
    bridge->heartBeats[coreID]   = 0;

    //
    // Enable the malbox interrupt.
    //
    EnableMailboxFromCore();

    EnableInterrupts();

    //
    //
    //
    while(true)    
    {
        bridge->heartBeats[coreID]++;
        FlushCache();
        if( (bridge->heartBeats[coreID] % 0x4ffff) == 0 )
        {
            //
            //
            //
            char    string[64];
            FormatText(string, sizeof(string), "Count on core %d is %d", coreID, bridge->heartBeats[coreID] );
            DebugText( &string[0] );
        }

        //
        //
        //
        CoreMessage*    msg     = NextMessage();
        if(msg != 0)
        {
            ProcessMessage( msg );
            DebugText("Message Received!");
            ReleaseMessage( msg );
        }
    }        
}


//
//
//
void CoreMain(uint32_t coreID)
{
    //
    //
    //
    BoardSupportInitialise();

    //
    //
    //
    switch(coreID)
    {
        case 1:
            Core1Main(coreID);
            break;
            
        case 2:
            Core2Main(coreID);
            break;
            
        case 3:
            Core3Main(coreID);
            break;
            
        default:
            break;
    }
}




