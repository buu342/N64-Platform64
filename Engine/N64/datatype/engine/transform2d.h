#ifndef PLATFORM64_TRANSFORM2D_H
#define PLATFORM64_TRANSFORM2D_H

    #include "bool.h"
    #include "../math/vector2d.h"

    typedef struct
    {
        Vector2D   pos;
        Vector2D   scale;
        f32        rot;
        Vector2D   oldpos;
        Vector2D   oldscale;
        f32        oldrot;
        u64        transformtime;
        bool       interpolate;
    } Transform2D;
    
    inline Transform2D transform2d_initialize(Vector2D pos, f32 rot, Vector2D scale);
    inline void        transform2d_set_pos(Transform2D* t, Vector2D pos);
    inline void        transform2d_set_rot(Transform2D* t, f32 rotation);
    inline void        transform2d_set_scale(Transform2D* t, Vector2D scale);
    inline void        transform2d_force_pos(Transform2D* t, Vector2D pos);
    inline void        transform2d_force_rot(Transform2D* t, f32 rotation);
    inline void        transform2d_force_scale(Transform2D* t, Vector2D scale);
    inline Vector2D    transform2d_get_pos(Transform2D* t);
    inline f32         transform2d_get_rot(Transform2D* t);
    inline Vector2D    transform2d_get_scale(Transform2D* t);
    inline Vector2D    transform2d_get_oldpos(Transform2D* t);
    inline f32         transform2d_get_oldrot(Transform2D* t);
    inline Vector2D    transform2d_get_oldscale(Transform2D* t);
    inline Vector2D    transform2d_get_subpos(Transform2D* t);
    inline f32         transform2d_get_subrot(Transform2D* t);
    inline Vector2D    transform2d_get_subscale(Transform2D* t);
    
# endif