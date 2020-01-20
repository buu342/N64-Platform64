// ray_tri_intersect(RayPos, RayVector, Triangle(s))
// Möller-Tumbore intersection algorithm implementation

var EPSILON = 0.0000001;
var RayPos = argument0;
var RayVec = argument1;
var tris = argument2;
var numtris = array_height_2d(tris)

for (var i=0; i<numtris; i++)
{
    var vert0 = tris[i, 0];
    var vert1 = tris[i, 1];
    var vert2 = tris[i, 2];
    
    var edge1 = vector3d_subtract(vert1, vert0);
    var edge2 = vector3d_subtract(vert2, vert0);
    
    var h = vector3d_cross(RayVec, edge2);
    var a = vector3d_dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON)
        continue;
    
    var f = 1.0/a;
    var s = vector3d_subtract(RayPos, vert0);
    var u = f*vector3d_dot(s, h);
    
    if (u < 0.0 || u > 1.0)
        continue;
        
    var q = vector3d_cross(s, edge1);
    var v = f*vector3d_dot(RayVec, q);
    
    if (v < 0.0 || u + v > 1.0)
        continue;
        
    var t = f*vector3d_dot(edge2, q);
    if (t > EPSILON && t < 1/EPSILON) // ray intersection
    {
        //var IntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    
    // This means that there is a line intersection but not a ray intersection.
    continue;
}

return false;
