#ifndef PLATFORM64_QUATERNION_H
#define PLATFORM64_QUATERNION_H

    typedef struct
    {
        f32 w;
        f32 x;
        f32 y;
        f32 z;
    } Quaternion;
    
    typedef union
    {
        Quaternion q;
        f32 a[4];
    } U_Quaternion;

#endif