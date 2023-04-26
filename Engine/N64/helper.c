/***************************************************************
                            helper.c
                               
Contains a set of small but useful functions
***************************************************************/

#include <ultra64.h>
#include "debug.h"
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


/*==============================
    roundf
    Rounds a floating point number
    @param  The value to round
    @return The rounded result
==============================*/

s32 roundf(f32 val)
{
    if (val < 0.0f)
        return (s32)(val - 0.5f);
    else
        return (s32)(val + 0.5f);
}