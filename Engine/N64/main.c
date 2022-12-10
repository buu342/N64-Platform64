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
static void advance_step();


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
        debug_printf("Idle Thread: Spinning\n");
    }
}


/*==============================
    threadfunc_main
    The main thread loop
    @param Argument passed to the thread
==============================*/

static void threadfunc_main(void *arg)
{
    Scheduler* l_scheduler;
    u8 l_color = 0;
    debug_printf("Main Thread: Started\n");
    
    // Initialize the rest of the game
    l_scheduler = scheduler_initialize();
    graphics_initialize(l_scheduler);
    audio_initialize();
    controller_initialize();
    
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
    
    // And now, perform the game main
    while (1)
    {
        int i;
        
        // Print controller data to show it's working
        if (controller_action_pressed(PLAYER_1, ACTION_JUMP))
            debug_printf("Jump button pressed\n");
        if (controller_action_down(PLAYER_1, ACTION_JUMP))
            debug_printf("Jump button down\n");
        debug_printf("Player 1 stick: {%0.4f, %0.4f}\n", controller_get_x(PLAYER_1), controller_get_y(PLAYER_1));
        
        // Render the scene
        graphics_requestrender(l_color++, TRUE);
        
        // Read control stick data
        controller_read_all();
        
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