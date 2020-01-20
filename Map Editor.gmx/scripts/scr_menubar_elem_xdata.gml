// scr_menubar_elem_xpos(menubar, element)

var data;
var menubar = argument0
var elem = argument1
var size = array_length_1d(menubar);
var lastx = MENUBAR_SPACING
for (i=0; i<=elem; i+=1)
{
    var menu = menubar[i];
    var str = menu[menu_e.name];
    var strlen = string_width(str);

    data[@0] = lastx-MENUBAR_SPACING
    lastx = lastx + strlen + MENUBAR_SPACING*2    
    data[@1] =  lastx
}

return data
