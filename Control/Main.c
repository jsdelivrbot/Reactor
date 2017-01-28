//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include "Timestamp.h"
#include "DebugText.h"
#include "SharedMemory.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>


void Test()
{

  const char *name = "shm-example";	// file name
  const int SIZE = 4096;		// file size

  int shm_fd;		// file descriptor, from shm_open()
  char *shm_base;	// base address, from mmap()

  /* open the shared memory segment as if it was a file */
  shm_fd = shm_open(name, O_RDONLY, 0666);
  if (shm_fd == -1) {
    printf("cons: Shared memory failed\n");
    exit(1);
  }

  /* map the shared memory segment to the address space of the process */
  shm_base = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
  if (shm_base == MAP_FAILED) {
    printf("cons: Map failed\n");
    // close and unlink?
    exit(1);
  }

    memset( shm_base, 0, SIZE );
    while(shm_base[0] != 0)
    {
        printf("Producer Waiting for sync.\n");
        sleep(1);
    }

    strcpy(shm_base, "Hello World!\n");

    exit(0);

}

//
//
//
int main()
{
    DebugPrintf("\nReactorControl.\n");

    Test();

    //
    //
    //
    //void*   sharedMemory    = SharedMemoryInitialise(0x00000001);

    //
    //
    //
    uint32_t 	i 	= 0;
    while(true)
    {

        //
        // Get the current timestamp.
        //
        Timestamp    timestamp 	= GetTimestamp();
    }

}


