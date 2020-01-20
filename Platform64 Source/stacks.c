/*******************************************************************
                              stacks.c
                               
Initializes all the stacks that will be used by the ROM.
*******************************************************************/

#include <ultra64.h>
#include "stacks.h"

u64 bootStack[STACKSIZE/sizeof(u64)];