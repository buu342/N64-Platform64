// scr_menubar_create(MenuArray)

enum menubar_enums {
    name = 0,
    submenu = 1,
    shortcut = 2,
}

var r; r[1] = 0; 
r[@name] = argument0;
r[@submenu] = argument1;
r[@shortcut] = argument2;

return r;
