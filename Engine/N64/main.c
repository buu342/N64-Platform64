/***************************************************************
                             main.c
                               
Handles the boot process of the ROM.
***************************************************************/

#include <ultra64.h>
#include "types.h"
#include "debug.h"
#include "osconfig.h"
#include "controller.h"
#include "scheduler.h"
#include "graphics.h"
#include "audio.h"


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_idle(void *arg);
static void threadfunc_main(void *arg);
static void toggle_lag();


/*********************************
             Globals
*********************************/

// Threads
static OSThread	s_threadstruct_idle;
static OSThread	s_threadstruct_main;

// PI Messages
static OSMesg s_pi_messages[NUM_PI_MSGS];
static OSMesgQueue s_pi_queue;

static bool s_shouldlag = FALSE;



/*==============================
    boot
    Initializes the hardware and
    prints Hello World
==============================*/

void boot()
{
    // Initialize the hardware and software
    osInitialize();
    
    // Start the idle thread
    osCreateThread(&s_threadstruct_idle, THREADID_IDLE, threadfunc_idle, NULL, STACKREALSTART_IDLE, THREADPRI_IDLE);
    osStartThread(&s_threadstruct_idle);
}


/*==============================
    threadfunc_idle
    The idle thread
    @param Argument passed to the thread
==============================*/

static void threadfunc_idle(void *arg)
{
    // Create the PI manager
    osCreatePiManager((OSPri)OS_PRIORITY_PIMGR, &s_pi_queue, s_pi_messages, NUM_PI_MSGS);
    
    // Initialize debug mode
    debug_initialize();
    debug_64drivebutton(toggle_lag, TRUE);
    
    // Start the main thread
    osCreateThread(&s_threadstruct_main, THREADID_MAIN, threadfunc_main, NULL, STACKREALSTART_MAIN, THREADPRI_MAIN);
    osStartThread(&s_threadstruct_main);
    
    // Reduce this thread's priority to the minimum possible, and spin forever
    osSetThreadPri(0, 0);
    while (1)
    {
        debug_printf("Idle thread spinning\n");
    }
}


/*==============================
    threadfunc_main
    The main thread loop
    @param Argument passed to the thread
==============================*/

static void threadfunc_main(void *arg)
{
    u8 l_color = 0;
    debug_printf("Created main thread\n");

    // Initialize the TV
    osCreateViManager(OS_PRIORITY_VIMGR);
    #if (TVMODE == TV_NTSC)
        osViSetMode(&osViModeNtscLan1);
    #elif (TVMODE == TV_PAL)
        osViSetMode(&osViModeFpalLan1);
    #else
        osViSetMode(&osViModeMpalLan1);
    #endif
    
    // Initialize the rest of the game
    controller_initialize();
    scheduler_initialize();
    graphics_initialize();
    audio_initialize();
    
    // Loop forever, needed or the VI will not display correctly
    while (1)
    {
        int i;
        
        // Render the scene
        graphics_renderscene(l_color++);
        
        // Check if the flashcart has incoming debug data
        debug_pollcommands();
        
        // If enabled, crunch some numbers for a while to create "lag"
        if (s_shouldlag)
        {
            for (i=0; i<1000000; i++)
                ;
        }
    }
}

/*==============================
    toggle_lag
    Toggles lag when called
==============================*/

static void toggle_lag()
{
    s_shouldlag = !s_shouldlag;
}