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

#define VERBOSE  TRUE


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_scheduler(void *arg);
static void scheduler_handledisplay();


/*********************************
             Globals
*********************************/

static OSThread	s_threadstruct_scheduler;

static Scheduler s_scheduler;

static FrameBuffer* s_displayingfb;


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
        
        // Pick what to do based on the message value:
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
            //case MSG_SCHEDULER_PRENMI:
            //    scheduler_handlenmi();
            //    break;
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
        return;
    }
    
    // We have a framebuffer available, pop it from the ready stack
    l_fb = graphics_popframebuffer();
    #if VERBOSE 
        debug_printf("Scheduler Thread: Popped a framebuffer.\n");
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