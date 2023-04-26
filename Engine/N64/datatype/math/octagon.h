#ifndef PLATFORM64_OCTAGON_H
#define PLATFORM64_OCTAGON_H

    typedef struct 
    {
        Vector2D top;
        Vector2D topright;
        Vector2D right;
        Vector2D bottomright;
        Vector2D bottom;
        Vector2D bottomleft;
        Vector2D left;
        Vector2D topleft;
    } Octagon;
    
    typedef union 
    {
        Octagon octagon;
        f32 array[2*8];
    } OctArray;

#endif