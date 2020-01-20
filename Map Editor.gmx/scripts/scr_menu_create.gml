// scr_menu_create(Name, SubmenuArray, ShortcutOrd)

enum menu_e {
    name = 0,
    submenu = 1,
    shortcut = 2,
}

var r; r[1] = 0; 
r[@menu_e.name] = argument0;
r[@menu_e.submenu] = argument1;
r[@menu_e.shortcut] = argument2;

return r;
