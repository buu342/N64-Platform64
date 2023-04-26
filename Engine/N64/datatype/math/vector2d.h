#ifndef PLATFORM64_VECTOR2D_H
#define PLATFORM64_VECTOR2D_H

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

    inline Vector2D vector2d_initialize(f32 x, f32 y);
    inline Vector2D vector2d_add(Vector2D v, f32 val);
    inline Vector2D vector2d_addvector(Vector2D a, Vector2D b);
    inline Vector2D vector2d_mult(Vector2D v, f32 val);
    inline Vector2D vector2d_multvector(Vector2D a, Vector2D b);

#endif