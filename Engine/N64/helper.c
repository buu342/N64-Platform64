/***************************************************************
                            helper.c
                               
Contains a set of small but useful functions
***************************************************************/

#include <ultra64.h>
#include "types.h"
#include "helper.h"

float clampf(float val, float min, float max) 
{
    return MIN(MAX(val, min), max);
}