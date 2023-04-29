#include <ultra64.h>
#include "debug.h"
#include "types.h"
#include "osconfig.h"
#include "scheduler.h"
#include "graphics.h"
#include "engine.h"
#include "game/levels/levels.h"


/*********************************
             Globals
*********************************/

// Engine stuff
static f32 s_timescale = 1.0f;
static f32 s_deltatime_scaled = DELTATIME;
static f32 s_subtick   = 0.0f;
static f32 s_frametime = 0.0f;
static u64 s_gametime  = 0;

// Level stuff
static levelid s_levelid = LEVEL_BOOT;
static levelid s_levelid_old = LEVEL_BOOT;
static bool s_changelevel = FALSE;



/*==============================
    engine_level_start
    Starts a level
==============================*/

void engine_level_start()
{
    s_changelevel = FALSE;
    if (levels_get_all()[s_levelid].start != NULL)
        levels_get_all()[s_levelid].start();
}


/*==============================
    engine_level_update
    Updates the level by deltatime ticks
==============================*/

void engine_level_update()
{
    if (levels_get_all()[s_levelid].update != NULL)
        levels_get_all()[s_levelid].update();
}


/*==============================
    engine_level_render
    Renders the level
==============================*/

void engine_level_render()
{
    int i;    
    for (i=0; i<levels_get_all()[s_levelid].drawcount; i++)
    {
        DrawFuncs* task = &levels_get_all()[s_levelid].drawarray[i];
        graphics_requestrender(task->render, task->usecpu, (i == levels_get_all()[s_levelid].drawcount-1));
    }
}


/*==============================
    engine_level_end
    Ends the level
==============================*/

void engine_level_end()
{
    if (levels_get_all()[s_levelid_old].end != NULL)
        levels_get_all()[s_levelid_old].end();
}


/*==============================
    engine_level_change
    Handles level changing
    @param The levelid to switch to
==============================*/

void engine_level_change(u32 levelid)
{
    s_levelid_old = s_levelid;
    s_levelid = levelid;
    s_changelevel = TRUE;
}


/*==============================
    engine_level_shouldchange
    Checks if the level should change
    @return Whether to change the level or not
==============================*/

bool engine_level_shouldchange(levelid id)
{
    return s_changelevel;
}


/*==============================
    engine_set_timescale
    Sets the timescale for the engine
    Use for slowmotion
    @param The timescale
==============================*/

inline void engine_set_timescale(f32 timescale)
{
    s_timescale = timescale;
    s_deltatime_scaled = DELTATIME*timescale;
}


/*==============================
    engine_set_subtick
    Sets the subtick for the engine
    Used to interpolate between frames
    @param The subtick
==============================*/

inline void engine_set_subtick(f32 subtick)
{
    s_subtick = subtick;
}


/*==============================
    engine_set_frametime
    Sets the frametime for the engine
    Used for framerate independent animations
    @param The frametime
==============================*/

inline void engine_set_frametime(f32 frametime)
{
    s_frametime = frametime;
}


/*==============================
    engine_increment_gametime
    Increments the game time
==============================*/

inline void engine_increment_gametime()
{
    s_gametime++;
}


/*==============================
    engine_get_deltatime
    Gets the deltatime
    @return The deltatime
==============================*/

inline f32 engine_get_deltatime()
{
    return s_deltatime_scaled;
}


/*==============================
    engine_get_deltatime_unscaled
    Gets the unscaled deltatime
    @return The unscaled deltatime
==============================*/

inline f32 engine_get_deltatime_unscaled()
{
    return DELTATIME;
}


/*==============================
    engine_get_timescale
    Gets the timescale of the engine
    @return The timescale
==============================*/

inline f32 engine_get_timescale()
{
    return s_timescale;
}


/*==============================
    engine_get_subtick
    Gets the subtick of the engine
    @return The subtick
==============================*/

inline f32 engine_get_subtick()
{
    return s_subtick;
}


/*==============================
    engine_get_frametime
    Gets the frametime of the engine
    @return The frametime
==============================*/

inline f32 engine_get_frametime()
{
    return s_frametime;
}


/*==============================
    engine_get_gametime
    Gets a counter that increments since
    the game booted
    @return The game time
==============================*/

inline u64 engine_get_gametime()
{
    return s_gametime;
}


/*==============================
    engine_shouldchange
    Checks if the engine should change
    @return Returns FALSE for now
==============================*/

inline bool engine_shouldchange()
{
    return FALSE;
}