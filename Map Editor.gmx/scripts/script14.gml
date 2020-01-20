var view_mat = matrix_get(matrix_view);

var _x = aspect * dtan(fov/2) * ( 2 * mouse_x / port_w - 1);
var _y =          dtan(fov/2) * (1 - 2 * mouse_y / port_h );
                      //xaxis            //yaxis       //zaxis
mouse_vector_x = _x * view_mat[0] + _y * view_mat[1] + view_mat[2];
mouse_vector_y = _x * view_mat[4] + _y * view_mat[5] + view_mat[6];
mouse_vector_z = _x * view_mat[8] + _y * view_mat[9] + view_mat[10];
