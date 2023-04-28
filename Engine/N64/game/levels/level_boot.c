#include <ultra64.h>
#include "../../debug.h"
#include "../../types.h"
#include "../../osconfig.h"
#include "../../scheduler.h"
#include "../../graphics.h"
#include "../../rcp.h"
#include "../../helper.h"
#include "../../engine.h"
#include "levels.h"


/*********************************
             Macros
*********************************/

#define RECTSIZE   64
#define RECTSPEED  200*engine_get_deltatime()

// Rectangle stuff
static Transform2D s_rect_transform;
static Color       s_rect_color;
static Vector2D    s_rect_speed;


/*==============================
    level_boot_start
    Starts the boot level
==============================*/

void level_boot_start()
{
    /*s_rect_transform = transform2d_initialize(vector2d_initialize(64, 64), 0, vector2d_initialize(RECTSIZE, RECTSIZE));
    s_rect_color = color_initialize(255, 0, 0, 255);
    s_rect_speed = vector2d_initialize(RECTSPEED, 0);*/
}


/*==============================
    level_boot_update
    Updates the boot level by deltatime ticks
==============================*/

void level_boot_update()
{
    Vector2D l_rectpos = transform2d_get_pos(&s_rect_transform);
    Vector2D l_rectsize = transform2d_get_scale(&s_rect_transform);

    // Move the rectangle around
    l_rectpos.x += s_rect_speed.x;
    l_rectpos.y += s_rect_speed.y;

    // Prevent the rectangle from going out of bounds
    if (l_rectpos.x > graphics_get_screenw()-l_rectsize.x || l_rectpos.x <= 0)
    {
        s_rect_speed.x = -s_rect_speed.x;
        l_rectpos.x = clampf(l_rectpos.x, 0, graphics_get_screenw()-l_rectsize.x);
    }
    if (l_rectpos.y > graphics_get_screenh()-l_rectsize.y || l_rectpos.y <= 0)
    {
        s_rect_speed.y = -s_rect_speed.y;
        l_rectpos.y = clampf(l_rectpos.y, 0, graphics_get_screenh()-l_rectsize.y);
    }

    // Apply the transformations
    transform2d_set_pos(&s_rect_transform, l_rectpos);
}


/*==============================
    level_boot_render
    Renders the boot level
==============================*/

void level_boot_render()
{
    Vector2D l_rectpos = transform2d_get_subpos(&s_rect_transform);
    Vector2D l_rectscale = transform2d_get_subscale(&s_rect_transform);
    
    // Clear the framebuffer with a horrible blue
    rcp_clearbuffers(0, 0, 255);
    
    // Draw a rectangle
    gDPSetCycleType(g_displistp++, G_CYC_FILL);
    gDPSetFillColor(g_displistp++, 
        GPACK_RGBA5551(s_rect_color.red, s_rect_color.green, s_rect_color.blue, 1) << 16 | 
        GPACK_RGBA5551(s_rect_color.red, s_rect_color.green, s_rect_color.blue, 1)
    );
    gDPFillRectangle(g_displistp++, l_rectpos.x, l_rectpos.y, l_rectpos.x + l_rectscale.x, l_rectpos.y + l_rectscale.y);
    gDPPipeSync(g_displistp++);
    
    // Print some debug info
    debug_printf("Frametime %fms. Subtick %f. Xpos %f\n", engine_get_frametime()*1000.0f, engine_get_subtick(), l_rectpos.x);
}