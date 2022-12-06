/***************************************************************
                          scheduler.c
                               
Handles the scheduler, which coordinates between graphics and
audio tasks, as well as handles PreNMI interrupts (Reset Button)
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "types.h"
#include "scheduler.h"
#include "graphics.h"


/*********************************
           Definitions
*********************************/

// Enable this to see how the scheduler is behaving with prints
#define VERBOSE  FALSE


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_scheduler(void *arg);
static void scheduler_handledisplay();
static void scheduler_handlenmi();
static void scheduler_doresetwipe();

// Unexposed graphics.c functions
extern bool graphics_framebufferready();
extern FrameBuffer* graphics_popframebuffer();


/*********************************
             Globals
*********************************/

static OSThread	s_threadstruct_scheduler; // The thread stucture
static Scheduler s_scheduler; // The scheduler itself
static FrameBuffer* s_displayingfb; // The current displaying framebuffer


/*==============================
    scheduler_initialize
    Launches the scheduler thread
==============================*/

Scheduler* scheduler_initialize()
{
    // Initialize globals
    s_displayingfb = NULL;
    
    // Initialize the scheduler
    s_scheduler.tvblack = TRUE;
    s_scheduler.reset = FALSE;
    s_scheduler.task_gfx = NULL;
    s_scheduler.task_audio = NULL;
    s_scheduler.gfx_notify = NULL;
    osCreateMesgQueue(&s_scheduler.queue, s_scheduler.messages, SIZE_MSGQUEUE_SCHEDULER);
    
    // Initialize the TV
    osCreateViManager(OS_PRIORITY_VIMGR);
    #if (TVMODE == TV_NTSC)
        osViSetMode(&osViModeNtscLan1);
    #elif (TVMODE == TV_PAL)
        osViSetMode(&osViModeFpalLan1);
    #else
        osViSetMode(&osViModeMpalLan1);
    #endif
    osViBlack(TRUE);
    
    // Set the target framerate and register the event callbacks
    #if TVMODE == TV_PAL
        osViSetEvent(&s_scheduler.queue, (OSMesg)MSG_SCHEDULER_VSYNC, 50/TARGET_FRAMERATE);
    #else
        osViSetEvent(&s_scheduler.queue, (OSMesg)MSG_SCHEDULER_VSYNC, 60/TARGET_FRAMERATE);
    #endif
    osSetEventMesg(OS_EVENT_PRENMI, &s_scheduler.queue, (OSMesg)MSG_SCHEDULER_PRENMI);

    // Start the scheduler thread
    osCreateThread(&s_threadstruct_scheduler, THREADID_SCHEDULER, threadfunc_scheduler, NULL, STACKREALSTART_SCHEDULER, THREADPRI_SCHEDULER);
    osStartThread(&s_threadstruct_scheduler);
    
    // Return the scheduler object
    return &s_scheduler;
}


/*==============================
    thread_scheduler
    The scheduler thread
    @param Argument passed to the thread
==============================*/

static void threadfunc_scheduler(void *arg)
{
    #if VERBOSE 
        debug_printf("Scheduler Thread: Started\n");
    #endif
    
    // Spin this thread forever
    while (1)
    {
        OSMesg l_msg;
        #if VERBOSE 
            debug_printf("Scheduler Thread: Loop start, waiting for message\n");
        #endif
        osRecvMesg(&s_scheduler.queue, (OSMesg *)&l_msg, OS_MESG_BLOCK);
        
        // At this point, a message has been received from another thread
        #if VERBOSE 
            debug_printf("Scheduler Thread: Message '%d' received\n", (s32)l_msg);
        #endif
        
        // Pick what to do based on the message value
        switch ((s32)l_msg)
        {
            case MSG_SCHEDULER_VSYNC:
                if (s_scheduler.gfx_notify != NULL)
                {
                    osSendMesg(s_scheduler.gfx_notify, NULL, OS_MESG_BLOCK);
                    s_scheduler.gfx_notify = NULL;
                }
                scheduler_handledisplay();
                break;
            case MSG_SCHEDULER_PRENMI:
                scheduler_handlenmi();
                break;
            case MSG_SCHEDULER_RCPHANG:
                debug_printf("RCP is HUNG UP!!\noh! MY GOD!!\n");
                debug_assert(FALSE);
                break;
            default:
                #if VERBOSE 
                    debug_printf("Scheduler Thread: Bad message '0x%2x' received\n", (s32)l_msg);
                #endif
                break;
        }
    }
}


/*==============================
    scheduler_handledisplay
    Handles switching framebuffers
==============================*/

static void scheduler_handledisplay()
{
    FrameBuffer* l_fb;
    
    // Check if a framebuffer that is ready exists, if not then we have to wait for the next retrace
    if (!graphics_framebufferready())
    {
        #if VERBOSE 
            debug_printf("Scheduler Thread: No framebuffer available.\n");
        #endif
        
        // If the reset button was pressed recently, we can do some silly screen wipe
        if (s_scheduler.reset)
            scheduler_doresetwipe();
        return;
    }
    
    // We have a framebuffer available, pop it from the ready stack
    l_fb = graphics_popframebuffer();
    #if VERBOSE 
        debug_printf("Scheduler Thread: Popped a framebuffer at %p.\n", l_fb->address);
    #endif
    
    // Mark the old framebuffer as free, and swap with the new one
    if (s_displayingfb != NULL)
        s_displayingfb->status = FBSTATUS_FREE;
    s_displayingfb = l_fb;
    osViSwapBuffer(l_fb->address);
    #if VERBOSE 
        debug_printf("Scheduler Thread: Swapped framebuffer.\n");
    #endif
    
    // Turn the TV on if it isn't
    if (s_scheduler.tvblack)
    {
        #if VERBOSE 
            debug_printf("Scheduler Thread: TV Turned on.\n");
        #endif
        s_scheduler.tvblack = FALSE;
        osViBlack(FALSE);
    }
}


/*==============================
    scheduler_handlenmi
    Handles Pre-NMI (reset button).
    Reset process is described in Developer News 1.5
==============================*/

static void scheduler_handlenmi()
{
    osViSetYScale(1.0);
    audio_stopthread();
    graphics_stopthread();
    osSpTaskYield();
    osAfterPreNMI();
    s_scheduler.reset = TRUE;
}


/*==============================
    scheduler_doresetwipe
    Do some fancy effect on the CPU to the framebuffer
    while the reset button is down
==============================*/

#define COLUMNWIDTH  2
#define MELTSPEED    8

static void scheduler_doresetwipe()
{
    int i = 0;
    FRAMEBUFF_DEPTH* l_frame = s_displayingfb->address;
    static bool l_initialized = FALSE;
    static s32 l_columnstate[SCREEN_WIDTH_HD/COLUMNWIDTH];
    const u32 l_screenw = graphics_get_screenw();
    const u32 l_screenh = graphics_get_screenh();
    #if VERBOSE
        debug_printf("Scheduler Thread: Doing fizzle\n");
    #endif
    
    // Initialize the melt effect
    // This is done by initializing each column based on the previous value
    // If I were to use pure random, it'd look like garbage unfortunately
    if (!l_initialized)
    {
        l_columnstate[0] = MELTSPEED*(guRandom()%16);
        for (i=1; i<l_screenw/COLUMNWIDTH; i++)
        {
            u32 l_val = MELTSPEED*((guRandom()%3) - 1);
            l_columnstate[i] = l_columnstate[i-1] + l_val;
            if (l_columnstate[i] < 0) 
                l_columnstate[i] = 0;
            else if (l_columnstate[i] == 16*MELTSPEED)
                l_columnstate[i] = 15*MELTSPEED;
        }
        l_initialized = TRUE;
    }
    
    // Perform the melt effect
    // The column state tells us how far down a specific column of pixels should be
    for (i=0; i<l_screenw/COLUMNWIDTH; i++)
    {
        if (l_columnstate[i] < 0)
        {
            int j, k;
            for (j=l_screenh; j>0; j--)
            {
                for (k=0; k<COLUMNWIDTH; k++)
                {
                    int l_ipos = i*COLUMNWIDTH+k;
                    if (j > -l_columnstate[i] && (j-MELTSPEED) > 0) // Copy pixels above the melt effect
                        l_frame[l_ipos + l_screenw*j] = l_frame[l_ipos + l_screenw*(j-MELTSPEED)];
                    else if (j < -l_columnstate[i]-MELTSPEED) // Blacken pixels which haven't been yet (no need to re-blacken)
                        l_frame[l_ipos + l_screenw*j] = 0;
                }
            }
        }
        l_columnstate[i] -= MELTSPEED;
    }
}