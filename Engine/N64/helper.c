/***************************************************************
                            helper.c
                               
Contains a set of small but useful functions
***************************************************************/

#include <ultra64.h>
#include "types.h"
#include "helper.h"


/*==============================
    clampf
    Clamps a value between two others
    @param  The value to clamp
    @param  The minimum value
    @param  The maximum value
    @return The clamped value
==============================*/

f32 clampf(f32 val, f32 min, f32 max) 
{
    return MIN(MAX(val, min), max);
}


/*==============================
    lerpf
    Returns the linear interpolation of 
    two values given a fraction
    @param  The from value
    @param  The target value
    @param  The fraction
    @return The interpolated result
==============================*/

f32 lerpf(f32 from, f32 target, f32 fraction)
{
    return from + (fraction*(target - from));
}