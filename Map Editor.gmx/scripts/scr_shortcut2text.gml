// scr_shortcut2text(ShortcutArray)

var shortcut = argument0;
var size = array_length_1d(shortcut);
var str = "";

for (i=0; i<size; i++)
{
    switch (shortcut[i])
    {
        case vk_control:
            str += "Ctrl"; break;
        case vk_alt:
            str += "Alt"; break;
        case vk_f1:
            str += "F1"; break;
        case vk_f2:
            str += "F2"; break;
        case vk_f3:
            str += "F3"; break;
        case vk_f4:
            str += "F4"; break;
        case vk_f5:
            str += "F5"; break;
        case vk_f6:
            str += "F6"; break;
        case vk_f7:
            str += "F7"; break;
        case vk_f8:
            str += "F8"; break;
        case vk_f9:
            str += "F9"; break;
        case vk_f10:
            str += "F10"; break;
        case vk_f11:
            str += "F11"; break;
        case vk_f12:
            str += "F12"; break;
        case vk_home:
            str += "Home"; break;
        default:
            str += chr(shortcut[i]); break;
    }
        
    if (i+1 != size)
        str += "+"
}

return str
