#include <ultra64.h>
#include "debug.h"
#include "types.h"
#include "osconfig.h"
#include "scheduler.h"
#include "graphics.h"
#include "rcp.h"
#include "helper.h"
#include "scene.h"


/*********************************
             Macros
*********************************/

#define RECTSIZE   64
#define RECTSPEED  200*DELTATIME


/*********************************
             Globals
*********************************/

// Engine stuff
static f32 s_timescale = 1.0f;
static f32 s_subtick   = 0.0f;
static f32 s_frametime = 0.0f;
static u64 s_gametime  = 0;

// Rectangle stuff
static Transform2D s_rect_transform;
static Color       s_rect_color;
static Vector2D    s_rect_speed;


/*==============================
    scene_initialize
    Initializes the scene
==============================*/

void scene_initialize()
{
    s_rect_transform = transform2d_initialize(vector2d_initialize(64, 64), 0, vector2d_initialize(RECTSIZE, RECTSIZE));
    s_rect_color = color_initialize(255, 0, 0, 255);
    s_rect_speed = vector2d_initialize(RECTSPEED, 0);
}


/*==============================
    scene_update
    Updates the scene by DELTATIME ticks
==============================*/

void scene_update()
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
    scene_render
    Renders the scene
==============================*/

void scene_render()
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
    debug_printf("Frametime %fms. Subtick %f. Xpos %f\n", s_frametime*1000.0f, s_subtick, l_rectpos.x);
}


/*==============================
    scene_set_timescale
    Sets the timescale for the scene
    Use for slowmotion
    @param The timescale
==============================*/

inline void scene_set_timescale(f32 timescale)
{
    s_timescale = timescale;
}


/*==============================
    scene_set_subtick
    Sets the subtick for the scene
    Used to interpolate between frames
    @param The subtick
==============================*/

inline void scene_set_subtick(f32 subtick)
{
    s_subtick = subtick;
}


/*==============================
    scene_set_frametime
    Sets the frametime for the scene
    Used for framerate independent animations
    @param The frametime
==============================*/

inline void scene_set_frametime(f32 frametime)
{
    s_frametime = frametime;
}


/*==============================
    scene_increment_gametime
    Increments the game time
==============================*/

inline void scene_increment_gametime()
{
    s_gametime++;
}


/*==============================
    scene_get_timescale
    Gets the timescale of the scene
    @return The timescale
==============================*/

inline f32 scene_get_timescale()
{
    return s_timescale;
}


/*==============================
    scene_get_subtick
    Gets the subtick of the scene
    @return The subtick
==============================*/

inline f32 scene_get_subtick()
{
    return s_subtick;
}


/*==============================
    scene_get_frametime
    Gets the frametime of the scene
    @return The frametime
==============================*/

inline f32 scene_get_frametime()
{
    return s_frametime;
}


/*==============================
    scene_get_gametime
    Gets a counter that increments since
    the game booted
    @return The game time
==============================*/

inline u64 scene_get_gametime()
{
    return s_gametime;
}


/*==============================
    scene_shouldchange
    Checks if the scene should change
    @return Returns FALSE for now
==============================*/

inline bool scene_shouldchange()
{
    return FALSE;
}