/// scr_2dto3dray(X, Y)


var res = array_create(3);

var _x = obj_controller.aspect * dtan(obj_controller.fov/2) * (2 * argument0 / view_wport[0] - 1);
var _y = dtan(obj_controller.fov/2) * (1 - 2 * argument1 / view_hport[0]);

var mat = matrix_get(matrix_view);

res[0] = _x * mat[0] + _y * mat[1] + mat[2];
res[1] = _x * mat[4] + _y * mat[5] + mat[6];
res[2] = _x * mat[8] + _y * mat[9] + mat[10];

// Normalize the vector
res = vector3d_normalize(res);

return res;
