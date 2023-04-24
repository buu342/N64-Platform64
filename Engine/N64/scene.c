#include <ultra64.h>
#include "types.h"
#include "debug.h"
#include "osconfig.h"
#include "scheduler.h"
#include "graphics.h"
#include "rcp.h"
#include "helper.h"
#include "scene.h"

static f32 s_xpos;
static f32 s_xpos_old;
static u8 s_color;

static f32 s_timescale = 1.0f;
static f32 s_subtick   = 0.0f;
static f32 s_frametime = 0.0f;

static u32 s_size = 64;
static f32 s_speed = 200*DELTATIME;


/*==============================
    scene_initialize
    Initializes the scene
==============================*/

void scene_initialize()
{
    s_color = 0;
    s_xpos = 0;
    s_xpos_old = 0;
}


/*==============================
    scene_update
    Updates the scene by DELTATIME ticks
==============================*/

void scene_update()
{
    s_xpos_old = s_xpos;
    s_xpos += s_speed;
    if (s_xpos > graphics_get_screenw()-s_size || s_xpos <= 0)
        s_speed = -s_speed;
}


/*==============================
    scene_render
    Renders the scene
==============================*/

void scene_render()
{
    int l_ypos = 64;
    int l_xrenderpos = s_xpos*s_subtick + s_xpos_old*(1.0f-s_subtick);
    
    // Clear the framebuffer with a horrible blue
    rcp_clearbuffers(0, 0, 255);
    
    // Draw a rectangle
    gDPSetCycleType(g_displistp++, G_CYC_FILL);
    gDPSetFillColor(g_displistp++, GPACK_RGBA5551(255, 0, 0, 1) << 16 | GPACK_RGBA5551(255, 0, 0, 1));
    gDPFillRectangle(g_displistp++, l_xrenderpos, l_ypos, l_xrenderpos+s_size, l_ypos+s_size);
    gDPPipeSync(g_displistp++);
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
    scene_shouldchange
    Checks if the scene should change
    @return Returns FALSE for now
==============================*/

inline bool scene_shouldchange()
{
    return FALSE;
}