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
#include "scene.h"
#include "helper.h"


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_idle(void *arg);
static void threadfunc_main(void *arg);
static void toggle_lag();
static void advance_step();

// Unexposed scene.c functions
inline void scene_set_subtick(f32 subtick);
inline void scene_set_frametime(f32 frametime);


/*********************************
             Globals
*********************************/

// Threads
static OSThread	s_threadstruct_idle;
static OSThread	s_threadstruct_main;

// PI Messages
static OSMesg s_pi_messages[NUM_PI_MSGS];
static OSMesgQueue s_pi_queue;

// Lag switch for demonstrations
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
        //debug_printf("Idle Thread: Spinning\n");
    }
}


/*==============================
    threadfunc_main
    The main thread loop
    @param Argument passed to the thread
==============================*/

static void threadfunc_main(void *arg)
{
    int i;
    Vector2D l_oldstick[MAXCONTROLLERS];
    Scheduler* l_scheduler;
    debug_printf("Main Thread: Started\n");
    
    // Initialize the rest of the game
    l_scheduler = scheduler_initialize();
    graphics_initialize(l_scheduler);
    audio_initialize();
    controller_initialize();
    controller_rumble_init();
    memset(l_oldstick, 0, sizeof(Vector2D)*MAXCONTROLLERS);
    
    // Register framebuffers we are going to use
    graphics_register_fbuffer(FALSE, FRAMEBUFF_ADDR1_SD);
    graphics_register_fbuffer(FALSE, FRAMEBUFF_ADDR2_SD);
    if (osGetMemSize() == 0x00800000) // Only register HD addresses if a memory pak is present
    {
        graphics_register_fbuffer(TRUE, FRAMEBUFF_ADDR1_HD);
        graphics_register_fbuffer(TRUE, FRAMEBUFF_ADDR2_HD);
    }
    
    // Register controller actions
    controller_register_action(PLAYER_1, ACTION_JUMP, A_BUTTON);
    controller_register_action(PLAYER_2, ACTION_JUMP, A_BUTTON);
    
    // And now, perform the game main
    while (1)
    {
        OSTime l_oldtime = 0;
        OSTime l_accumulator = 0;
        const OSTime l_dt = OS_USEC_TO_CYCLES(SEC_TO_USEC(DELTATIME));

        // Initialize the scene
        scene_initialize();
        
        // Handle the scene loop
        while (!scene_shouldchange())
        {
            OSTime l_curtime = osGetTime();
            OSTime l_frametime = l_curtime - l_oldtime;
            
            // In order to prevent problems if the game slows down significantly, we will clamp the maximum timestep the simulation can take
            if (l_frametime > OS_USEC_TO_CYCLES(SEC_TO_USEC(0.25f)))
                l_frametime = OS_USEC_TO_CYCLES(SEC_TO_USEC(0.25f));
            scene_set_frametime(USEC_TO_SEC(OS_CYCLES_TO_USEC(l_frametime)));
            scene_set_subtick(((f64)l_accumulator)/((f64)l_dt));
            l_oldtime = l_curtime;
            
            // Perform the update in discrete steps (ticks)
            l_accumulator += l_frametime;
            while (l_accumulator >= l_dt)
            {
                scene_update();
                l_accumulator -= l_dt;
            }
            
            // Read control stick data
            controller_query_all();
            controller_read_all();
            
            // Render the scene
            scene_set_subtick(((f64)l_accumulator)/((f64)l_dt));
            scene_render();
            
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
}


/*==============================
    toggle_lag
    Toggles lag when called
==============================*/

static void toggle_lag()
{
    s_shouldlag = !s_shouldlag;
}