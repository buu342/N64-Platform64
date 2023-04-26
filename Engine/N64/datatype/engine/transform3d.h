#ifndef PLATFORM64_TRANSFORM2D_H
#define PLATFORM64_TRANSFORM2D_H

    #include "bool.h"
    #include "../math/vector3d.h"
    #include "../math/quaternion.h"

    typedef struct 
    {
        Vector3D   pos;
        Vector3D   scale;
        Quaternion rot;
        Vector3D   oldpos;
        Vector3D   oldscale;
        Quaternion oldrot;
        u64        transformtime;
        bool       interpolate;
    } Transform3D;
    
    inline Transform3D transform3d_initialize(Vector3D pos, Quaternion rot, Vector3D scale);
    inline void        transform3d_set_pos(Transform3D* t, Vector3D pos);
    inline void        transform3d_set_rot(Transform3D* t, Quaternion rotation);
    inline void        transform3d_set_scale(Transform3D* t, Vector3D scale);
    inline void        transform3d_force_pos(Transform3D* t, Vector3D pos);
    inline void        transform3d_force_rot(Transform3D* t, Quaternion rotation);
    inline void        transform3d_force_scale(Transform3D* t, Vector3D scale);
    inline Vector3D    transform3d_get_pos(Transform3D* t);
    inline Quaternion  transform3d_get_rot(Transform3D* t);
    inline Vector3D    transform3d_get_scale(Transform3D* t);
    inline Vector3D    transform3d_get_oldpos(Transform3D* t);
    inline Quaternion  transform3d_get_oldrot(Transform3D* t);
    inline Vector3D    transform3d_get_oldscale(Transform3D* t);
    inline Vector3D    transform3d_get_subpos(Transform3D* t);
    inline Quaternion  transform3d_get_subrot(Transform3D* t);
    inline Vector3D    transform3d_get_subscale(Transform3D* t);
    
# endif