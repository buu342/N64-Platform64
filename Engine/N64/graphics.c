/***************************************************************
                          graphics.c
                               
Handles graphics rendering
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "graphics.h"


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_graphics(void *arg);


/*********************************
             Globals
*********************************/

static OSThread	s_threadstruct_graphics;


/*==============================
    graphics_initialize
    Launches the graphics thread
==============================*/

void graphics_initialize()
{
    // Start the graphics thread
    osCreateThread(&s_threadstruct_graphics, THREADID_GRAPHICS, threadfunc_graphics, NULL, STACKREALSTART_GRAPHICS, THREADPRI_GRAPHICS);
    osStartThread(&s_threadstruct_graphics);
}


/*==============================
    thread_graphics
    The graphics thread
    @param Argument passed to the thread
==============================*/

static void threadfunc_graphics(void *arg)
{
    debug_printf("Created graphics thread\n");
    
    // Spin this thread forever
    while (1)
    {
        debug_printf("Graphics thread loop start\n");
        
        debug_printf("Stopping graphics thread\n");
        osStopThread(&s_threadstruct_graphics);
    }
}