/// vector3d_cross(Vector1, Vector2)

var vec1 = argument0;
var vec2 = argument1;
var res = array_create(3);

res[@0] = vec1[1]*vec2[2] - vec1[2]*vec2[1]
res[@1] = vec1[2]*vec2[0] - vec1[0]*vec2[2]
res[@2] = vec1[0]*vec2[1] - vec1[1]*vec2[0]

return res;
