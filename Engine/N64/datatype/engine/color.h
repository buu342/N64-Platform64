#ifndef PLATFORM64_COLOR_H
#define PLATFORM64_COLOR_H

    typedef struct 
    {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    } Color;
    
    inline Color color_initialize(u8 red, u8 green, u8 blue, u8 alpha);
    
# endif