/***************************************************************
                           graphics.c
                               
Handles graphics rendering
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "types.h"
#include "scheduler.h"
#include "graphics.h"

/*********************************
             Structs
*********************************/
    
typedef struct
{
    u8 color;
    bool swapbuffer;
} RenderMessage;


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_graphics(void *arg);
static void graphics_renderscene(FrameBuffer* fb, u8 color);


/*********************************
             Globals
*********************************/

// Framebuffers
static FrameBuffer s_framebuffers_sd[FRAMEBUFF_COUNT] = {{FRAMEBUFF_ADDR1_SD}, {FRAMEBUFF_ADDR2_SD}};
static FrameBuffer s_framebuffers_hd[FRAMEBUFF_COUNT] = {{FRAMEBUFF_ADDR1_HD}, {FRAMEBUFF_ADDR2_HD}};
static FrameBuffer* s_lastrendered;

// Thread
static OSThread s_threadstruct_graphics;

// Message queue
static OSMesgQueue s_msgqueue_graphics;
static OSMesg      s_msgbuffer_graphics[8];
static OSMesgQueue s_msgqueue_rdp;
static OSMesg      s_msgbuffer_rdp;
static OSMesgQueue s_msgqueue_vsync;
static OSMesg      s_msgbuffer_vsync;

static Scheduler* s_scheduler;


/*==============================
    graphics_initialize
    Launches the graphics thread
==============================*/

void graphics_initialize(Scheduler* scheduler)
{
    int i;
    
    // Initialize globals
    s_lastrendered = NULL;
    s_scheduler = scheduler;
    
    // Initialize framebuffers to their default state
    for (i=0; i<FRAMEBUFF_COUNT; i++)
    {
        s_framebuffers_sd[i].displist = g_displists[i];
        s_framebuffers_sd[i].status = FBSTATUS_FREE;
    }
    
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
    bool reuse = FALSE;
    FrameBuffer* l_freebuff = NULL;
    
    // Initialize the thread
    debug_printf("Graphics Thread: Started\n");
    osCreateMesgQueue(&s_msgqueue_graphics, s_msgbuffer_graphics, 8);
    osCreateMesgQueue(&s_msgqueue_rdp, &s_msgbuffer_rdp, 1);
    osCreateMesgQueue(&s_msgqueue_vsync, &s_msgbuffer_vsync, 1);
    osSetEventMesg(OS_EVENT_DP, &s_msgqueue_rdp, NULL);
    
    // Spin this thread forever
    while (1)
    {
        int i;
        RenderMessage l_msg;
        RenderMessage* l_msgp;
        
        // Wait for a graphics message to arrive
        debug_printf("Graphics Thread: Loop start. Waiting for render request.\n");
        osRecvMesg(&s_msgqueue_graphics, (OSMesg*)&l_msgp, OS_MESG_BLOCK);
        
        // Make a copy for safekeeping
        l_msg.color = l_msgp->color;
        l_msg.swapbuffer = l_msgp->swapbuffer;
        
        // We received a message, find an available framebuffer if we don't have one yet
        debug_printf("Graphics Thread: Render request received %d %d.\n", l_msg.color, l_msg.swapbuffer);
        if (!reuse)
        {
            for (i=0; i<FRAMEBUFF_COUNT; i++)
            {
                FrameBuffer* l_fb = &s_framebuffers_sd[i];
                if (l_fb->status == FBSTATUS_FREE || (l_fb->status == FBSTATUS_READY && l_fb != s_lastrendered))
                {
                    l_freebuff = l_fb;
                    break;
                }
            }
        }
        
        // If none was found, drop this render request
        if (l_freebuff == NULL)
        {
            debug_printf("Graphics Thread: No available buffer, dropping request.\n");
            continue;
        }
        
        // If the framebuffer is still in use by the VI (the switch takes time), then wait for it to become available
        while (osViGetCurrentFramebuffer() == l_freebuff || osViGetNextFramebuffer() == l_freebuff)
        {
            debug_printf("Graphics Thread: Framebuffer in use by VI. Waiting for VSync.\n");
            s_scheduler->gfx_notify = &s_msgqueue_vsync;
            osRecvMesg(&s_msgqueue_vsync, NULL, OS_MESG_BLOCK);
        }
        
        // Generate the display list for the scene
        debug_printf("Graphics Thread: Found buffer '%d'.\n", i);
        graphics_renderscene(l_freebuff, l_msg.color);
        l_freebuff->status = FBSTATUS_RENDERING;
        
        // Wait for the render task to finish
        (void)osRecvMesg(&s_msgqueue_rdp, NULL, OS_MESG_BLOCK);
        debug_printf("Graphics Thread: Render task finished.\n");
        s_scheduler->task_gfx = NULL;
        
        // If we're not meant to swap the framebuffer yet, then stop here
        // The next loop should reuse this framebuffer if needed
        if (!l_msg.swapbuffer)
        {
            debug_printf("Graphics Thread: Don't swap buffer yet.\n");
            reuse = TRUE;
            continue;
        }
        l_freebuff->status = FBSTATUS_READY;
        debug_printf("Graphics Thread: Framebuffer ready.\n");
            
        // Mark the buffer as the last rendered and notify the scheduler if needed
        s_lastrendered = l_freebuff;
        l_freebuff = NULL;
        reuse = FALSE;
        if (s_scheduler->notify)
        {
            debug_printf("Graphics Thread: Notified scheduler.\n");
            osSendMesg(&s_scheduler->queue, (OSMesg)MSG_SCHEDULER_READYFBUFFER, OS_MESG_BLOCK);
        }
    }
}

static void graphics_renderscene(FrameBuffer* fb, u8 color)
{
    RenderTask l_task;
    
    // Initialize the render task
    l_task.displistp = fb->displist;
    l_task.framebuffer = fb->address;
    #if (FRAMEBUFF_DEPTH == u16)
        l_task.bufferdepth = G_IM_SIZ_16b;
    #else
        l_task.bufferdepth = G_IM_SIZ_32b;
    #endif
    l_task.color = color;
    
    // Build the display list
    rcp_initialize_sd(&l_task);
    rcp_finish(&l_task);
    
    // Let the scheduler know the RCP is going to be busy
    s_scheduler->task_gfx = l_task.task;
    
    // Send the render task to the RCP
    debug_printf("Graphics Thread: Sending render task.\n");
    osSpTaskStart(l_task.task);
}

void graphics_requestrender(u8 color, bool swapbuffer)
{
    RenderMessage l_msg = {color, swapbuffer};
    osSendMesg(&s_msgqueue_graphics, (OSMesg)&l_msg, OS_MESG_BLOCK);
}

bool graphics_framebufferready()
{
    return s_lastrendered != NULL;
}

FrameBuffer* graphics_popframebuffer()
{
    int i;
    FrameBuffer* l_consumed = s_lastrendered;
    
    // Mark the consumed framebuffer as displaying, and remove the last rendered buffer
    l_consumed->status = FBSTATUS_DISPLAYING;
    s_lastrendered = NULL;
    
    // See if we have another framebuffer marked as ready
    for (i=0; i<FRAMEBUFF_COUNT; i++)
    {
        if (s_framebuffers_sd[i].status == FBSTATUS_READY)
        {
            s_lastrendered = &s_framebuffers_sd[i];
            break;
        }
    }
            
    // Return the consumed framebuffer
    return l_consumed;
}