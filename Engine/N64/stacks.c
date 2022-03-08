/*******************************************************************
                              stacks.c
                               
Initializes all the stacks that will be used by the ROM.
*******************************************************************/

#include <ultra64.h>
#include "osconfig.h"

u64 bootStack[BOOTSTACKSIZE/sizeof(u64)];