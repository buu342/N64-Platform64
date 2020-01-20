/// vector3d_normalize(Vector)

var vec = argument0;
var res = array_create(3);

var l = sqrt(sqr(vec[0])+sqr(vec[1])+sqr(vec[2]));
for (var i=0; i<3; i++){ 
    res[@i] = vec[i]/l;
}

return res;
