#include <ultra64.h>
#include "../../debug.h"
#include "../../types.h"
#include "vector3d.h"


/*==============================
    vector3d_initialize
    Returns an initialized 3D vector
    @param  The initial X value
    @param  The initial Y value
    @param  The initial Z value
    @return An initialized 3D vector
==============================*/

inline Vector3D vector3d_initialize(f32 x, f32 y, f32 z)
{
    Vector3D v = {.x = x, .y = y, .z = z};
    return v;
}