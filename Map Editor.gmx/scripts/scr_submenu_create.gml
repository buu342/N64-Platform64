// scr_submenu_create(Name, Script, ShortcutArray, IconIndex)

enum submenu_e {
    name = 0,
    script = 1,
    shortcut = 2,
    icon = 3,
}

var r; r[1] = 0; 
r[@submenu_e.name] = argument0;
r[@submenu_e.script] = argument1;
r[@submenu_e.shortcut] = argument2;
r[@submenu_e.icon] = argument3;

return r;
