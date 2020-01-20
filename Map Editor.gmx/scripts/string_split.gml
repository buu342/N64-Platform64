// string_split(String, Delimiter)
// Returns an array of strings split from the delimiter

var del = argument1;
var str = argument0 + del;
var len = string_length(del);
var ind = 0;
var arr = array_create(string_count(del, str)); 
repeat (string_count(del, str)) {
    var pos = string_pos(del, str) - 1;
    arr[@ind] = string_copy(str, 1, pos);
    str = string_delete(str, 1, pos + len);
    ind++;
}
return arr;
