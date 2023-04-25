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

    inline Vector2D vector2d_initialize(f32 x, f32 y);
    inline Vector2D vector2d_add(Vector2D v, f32 val);
    inline Vector2D vector2d_addvector(Vector2D a, Vector2D b);
    inline Vector2D vector2d_mult(Vector2D v, f32 val);
    inline Vector2D vector2d_multvector(Vector2D a, Vector2D b);
    
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
    
    
    /*********************************
                Rotations
    *********************************/
    
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
    
    
    /*********************************
            2D Transformations
    *********************************/

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
    
    
    /*********************************
            3D Transformations
    *********************************/
    
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
    
    
    /*********************************
                  Other
    *********************************/
    
    typedef struct 
    {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    } Color;

    inline Color color_initialize(u8 red, u8 green, u8 blue, u8 alpha);
    
#endif