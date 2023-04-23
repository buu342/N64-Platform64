#include <ultra64.h>
#include "types.h"
#include "debug.h"
#include "osconfig.h"
#include "scheduler.h"
#include "graphics.h"
#include "scene.h"

static f32 s_xpos;
static u8 s_color = 0;

static f32 s_scene_timescale = 1.0f;
static f32 s_scene_subtick   = 0.0f;
static f32 s_scene_frametime = 0.0f;

void scene_initialize()
{
    s_xpos = 0;
}

void scene_update()
{
    s_xpos += 5;
}

void scene_render()
{
    graphics_requestrender(s_color++, TRUE);
}

inline void scene_set_timescale(f32 timescale)
{
    s_scene_timescale = timescale;
}

inline void scene_set_subtick(f32 subtick)
{
    s_scene_subtick = subtick;
}

inline void scene_set_frametime(f32 frametime)
{
    s_scene_frametime = frametime;
}

inline f32 scene_get_timescale()
{
    return s_scene_timescale;
}

inline f32 scene_get_subtick()
{
    return s_scene_subtick;
}

inline f32 scene_get_frametime()
{
    return s_scene_frametime;
}

inline bool scene_shouldchange()
{
    return FALSE;
}