#ifndef PLATFORM64_VECTOR3D_H
#define PLATFORM64_VECTOR3D_H

    typedef struct
    {
        f32 x;
        f32 y;
        f32 z;
    } Vector3D;
    
    typedef union
    {
        Vector3D v;
        f32 a[3];
    } U_Vector3D;

#endif