#include <ultra64.h>
#include "../../debug.h"
#include "../../types.h"
#include "vector2d.h"


/*==============================
    vector2d_initialize
    Returns an initialized 2D vector
    @param  The initial X value
    @param  The initial Y value
    @return An initialized 2D vector
==============================*/

inline Vector2D vector2d_initialize(f32 x, f32 y)
{
    Vector2D v = {.x = x, .y = y};
    return v;
}


/*==============================
    vector2d_add
    Adds a value to a 2D vector
    @param The vector to add to
    @param The value to add
    @return The resulting vector
==============================*/

inline Vector2D vector2d_add(Vector2D v, f32 val)
{
    v.x += val;
    v.y += val;
    return v;
}


/*==============================
    vector2d_addvector
    Adds a 2D vector to a 2D vector
    @param  The first vector
    @param  The second vector
    @return The combined vector
==============================*/

inline Vector2D vector2d_addvector(Vector2D a, Vector2D b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}


/*==============================
    vector2d_mult
    Multiplies a value to a 2D vector
    @param The vector to multiply to
    @param The value to multiply
    @return The resulting vector
==============================*/

inline Vector2D vector2d_mult(Vector2D v, f32 val)
{
    v.x *= val;
    v.y *= val;
    return v;
}


/*==============================
    vector2d_multvector
    Multiplies a 2D vector to a 2D vector
    @param  The first vector
    @param  The second vector
    @return The combined vector
==============================*/

inline Vector2D vector2d_multvector(Vector2D a, Vector2D b)
{
    a.x *= b.x;
    a.y *= b.y;
    return a;
}