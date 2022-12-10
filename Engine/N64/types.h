#ifndef PLATFORM64_TYPES_H
#define PLATFORM64_TYPES_H
    
    
    /*********************************
              Boolean Types
    *********************************/
    
    #define FALSE  0
    #define TRUE   1

    typedef u8 bool;
    
    
    /*********************************
               Vector Types
    *********************************/
    
    typedef struct 
    {
        f32 x;
        f32 y;
    } Vector2D;
    
    typedef union
    {
        Vector2D v;
        f32 a[2];
    } U_Vector2D;
    
    
    /*********************************
                  Shapes
    *********************************/

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
        f32 array[16];
    } OctArray;
    
#endif