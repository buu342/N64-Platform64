#include <ultra64.h>
#include "../../debug.h"
#include "../../types.h"
#include "color.h"


/*==============================
    color_initialize
    Initializes a color struct
    @param  The red value
    @param  The green value
    @param  The blue value
    @param  The alpha value
    @return The initialized color struct
==============================*/

inline Color color_initialize(u8 red, u8 green, u8 blue, u8 alpha)
{
    Color c = {.red = red, .green = green, .blue = blue, .alpha = alpha};
    return c;
}