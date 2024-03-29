#include <ultra64.h>
#include "../../debug.h"
#include "../../types.h"
#include "../../engine.h"
#include "transform2d.h"


/*==============================
    transform2d_initialize
    Creates an initialized 2D transform struct
    @return an initialized 2D transform struct
==============================*/

inline Transform2D transform2d_initialize(Vector2D pos, f32 rot, Vector2D scale)
{
    Transform2D t = {
        .pos = pos,    .rot = rot,    .scale = scale,
        .oldpos = pos, .oldrot = rot, .oldscale = scale, 
        .transformtime = 0, .interpolate = TRUE
    };
    return t;
}


/*==============================
    transform2d_updateold
    Updates the old values of a transform2d struct
    @param The 2D transform struct to update
==============================*/

static inline void transform2d_updateold(Transform2D* t)
{
    if (engine_get_gametime() != t->transformtime)
    {
        t->transformtime = engine_get_gametime();
        t->interpolate = TRUE;
        t->oldpos = t->pos;
        t->oldrot = t->rot;
        t->oldscale = t->scale;
    }
}


/*==============================
    transform2d_set_pos
    Sets the position of a 2D transform
    @param The 2D transform struct to modify
    @param The position to set
==============================*/

inline void transform2d_set_pos(Transform2D* t, Vector2D pos)
{
    transform2d_updateold(t);
    t->pos = pos;
}


/*==============================
    transform2d_set_rot
    Sets the rotation of a 2D transform
    @param The 2D transform struct to modify
    @param The rotation to set
==============================*/

inline void transform2d_set_rot(Transform2D* t, f32 rotation)
{
    transform2d_updateold(t);
    t->rot = rotation;
}


/*==============================
    transform2d_set_scale
    Sets the scale of a 2D transform
    @param The 2D transform struct to modify
    @param The scale to set
==============================*/

inline void transform2d_set_scale(Transform2D* t, Vector2D scale)
{
    transform2d_updateold(t);
    t->scale = scale;
}


/*==============================
    transform2d_force_pos
    Forces the position of a 2D transform, disabling interpolation
    @param The 2D transform struct to modify
    @param The position to force
==============================*/

inline void transform2d_force_pos(Transform2D* t, Vector2D pos)
{
    transform2d_updateold(t);
    t->pos = pos;
    t->interpolate = FALSE;
}


/*==============================
    transform2d_force_rot
    Forces the rotation of a 2D transform, disabling interpolation
    @param The 2D transform struct to modify
    @param The rotation to force
==============================*/

inline void transform2d_force_rot(Transform2D* t, f32 rotation)
{
    transform2d_updateold(t);
    t->rot = rotation;
    t->interpolate = FALSE;
}


/*==============================
    transform2d_force_scale
    Forces the scale of a 2D transform, disabling interpolation
    @param The 2D transform struct to modify
    @param The scale to force
==============================*/

inline void transform2d_force_scale(Transform2D* t, Vector2D scale)
{
    transform2d_updateold(t);
    t->scale = scale;
    t->interpolate = FALSE;
}


/*==============================
    transform2d_get_pos
    Gets the current position of a 2D transform
    @param  The 2D transform to get the current position of
    @return The current position as a 2D vector
==============================*/

inline Vector2D transform2d_get_pos(Transform2D* t)
{
    return t->pos;
}


/*==============================
    transform2d_get_rot
    Gets the current rotation of a 2D transform
    @param  The 2D transform to get the current rotation of
    @return The current rotation as an angle
==============================*/

inline f32 transform2d_get_rot(Transform2D* t)
{
    return t->rot;
}


/*==============================
    transform2d_get_scale
    Gets the current scale of a 2D transform
    @param  The 2D transform to get the current scale of
    @return The current scale as a 2D vector
==============================*/

inline Vector2D transform2d_get_scale(Transform2D* t)
{
    return t->scale;
}


/*==============================
    transform2d_get_oldpos
    Gets the old position of a 2D transform
    @param  The 2D transform to get the old position of
    @return The old position as a 2D vector
==============================*/

inline Vector2D transform2d_get_oldpos(Transform2D* t)
{
    return t->oldpos;
}


/*==============================
    transform2d_get_oldrot
    Gets the old rotation of a 2D transform
    @param  The 2D transform to get the old rotation of
    @return The old rotation as an angle
==============================*/

inline f32 transform2d_get_oldrot(Transform2D* t)
{
    return t->oldrot;
}


/*==============================
    transform2d_get_oldscale
    Gets the old scale of a 2D transform
    @param  The 2D transform to get the old scale of
    @return The old scale as a 2D vector
==============================*/

inline Vector2D transform2d_get_oldscale(Transform2D* t)
{
    return t->oldscale;
}


/*==============================
    transform2d_get_subpos
    Gets the subtick position of a 2D transform
    @param  The 2D transform to get the subtick position of
    @return The subtick position as a 2D vector
==============================*/

inline Vector2D transform2d_get_subpos(Transform2D* t)
{
    return vector2d_addvector(vector2d_mult(t->pos, engine_get_subtick()), vector2d_mult(t->oldpos, 1.0f-engine_get_subtick()));
}


/*==============================
    transform2d_get_subrot
    Gets the subtick rotation of a 2D transform
    @param  The 2D transform to get the subtick rotation of
    @return The subtick rotation as an angle
==============================*/

inline f32 transform2d_get_subrot(Transform2D* t)
{
    return t->rot*engine_get_subtick() + t->oldrot*(1.0f-engine_get_subtick());
}


/*==============================
    transform2d_get_subscale
    Gets the subtick scale of a 2D transform
    @param  The 2D transform to get the subtick scale of
    @return The subtick scale as a 2D vector
==============================*/

inline Vector2D transform2d_get_subscale(Transform2D* t)
{
    return vector2d_addvector(vector2d_mult(t->scale, engine_get_subtick()), vector2d_mult(t->oldscale, 1.0f-engine_get_subtick()));
}