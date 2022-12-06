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
#include "rcp.h"


/*********************************
           Definitions
*********************************/

// Enable this to see how the graphics thread is behaving with prints
#define VERBOSE  FALSE


/*********************************
             Structs
*********************************/

// A struct which describes the render task from the main thread
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
static FrameBuffer s_framebuffers_sd[FRAMEBUFF_MAXCOUNT] = {{NULL}, {NULL}, {NULL}};
static FrameBuffer s_framebuffers_hd[FRAMEBUFF_MAXCOUNT] = {{NULL}, {NULL}, {NULL}};
static FrameBuffer* s_lastrendered;
static u8 s_framebuffcount_sd;
static u8 s_framebuffcount_hd;
static u32* s_zbuffer_sd;
static u32* s_zbuffer_hd;
static bool s_ishd;

// Active framebuffer trackers
static FrameBuffer* s_framebuffers_active;
static u8 s_framebuffers_activecount;

// Thread
static OSThread s_threadstruct_graphics;

// Message queues for thread communication
static OSMesgQueue s_msgqueue_graphics;
static OSMesg      s_msgbuffer_graphics[FRAMEBUFF_MAXCOUNT];
static OSMesgQueue s_msgqueue_rdp;
static OSMesg      s_msgbuffer_rdp;
static OSMesgQueue s_msgqueue_vsync;
static OSMesg      s_msgbuffer_vsync;

// The scheduler
static Scheduler* s_scheduler;

// RCP hang detection timer
#if DEBUG_MODE
    OSTimer s_rcptime;
#endif


/*==============================
    graphics_initialize
    Launches the graphics thread
==============================*/

void graphics_initialize(Scheduler* scheduler)
{
    int i;
    
    // Initialize globals
    s_ishd = FALSE;
    s_lastrendered = NULL;
    s_scheduler = scheduler;
    s_framebuffcount_sd = 0;
    s_framebuffcount_hd = 0;
    s_framebuffers_active = s_framebuffers_sd;
    s_framebuffers_activecount = s_framebuffcount_sd;
    s_zbuffer_sd = NULL;
    s_zbuffer_hd = NULL;
    
    // Initialize framebuffers to their default state
    for (i=0; i<s_framebuffers_activecount; i++)
    {
        s_framebuffers_active[i].displist = g_displists[i];
        s_framebuffers_active[i].status = FBSTATUS_FREE;
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
    FrameBuffer* l_freebuff = NULL;
    
    // Initialize the thread
    debug_printf("Graphics Thread: Started\n");
    osCreateMesgQueue(&s_msgqueue_graphics, s_msgbuffer_graphics, FRAMEBUFF_MAXCOUNT);
    osCreateMesgQueue(&s_msgqueue_rdp, &s_msgbuffer_rdp, 1);
    osCreateMesgQueue(&s_msgqueue_vsync, &s_msgbuffer_vsync, 1);
    osSetEventMesg(OS_EVENT_DP, &s_msgqueue_rdp, NULL);
    
    // Spin this thread forever
    while (1)
    {
        int i;
        int l_buffcount;
        RenderMessage l_msg;
        RenderMessage* l_msgp;
        
        // Wait for a graphics message to arrive
        #if VERBOSE 
            debug_printf("Graphics Thread: Loop start. Waiting for render request.\n"); 
        #endif
        osRecvMesg(&s_msgqueue_graphics, (OSMesg*)&l_msgp, OS_MESG_BLOCK);
        
        // Make a copy for safekeeping
        memcpy(&l_msg, l_msgp, sizeof(RenderMessage));
        
        // We received a message, find an available framebuffer if we don't have one yet
        #if VERBOSE 
            debug_printf("Graphics Thread: Render request received %d %d.\n", l_msg.color, l_msg.swapbuffer);
        #endif
        if (l_freebuff == NULL)
        {
            for (i=0; i<s_framebuffers_activecount; i++)
            {
                FrameBuffer* l_fb = &s_framebuffers_active[i];
                if (l_fb->status == FBSTATUS_FREE || (l_fb->status == FBSTATUS_READY && l_fb != s_lastrendered))
                {
                    if (s_framebuffers_activecount > 2 && (osViGetCurrentFramebuffer() == l_fb->address || osViGetNextFramebuffer() == l_freebuff->address)) // If we have triple buffering, no need to use a framebuffer that's busy
                        continue;
                    l_freebuff = l_fb;
                    break;
                }
            }
            
            // If none was found, drop this render request
            if (l_freebuff == NULL)
            {
                #if VERBOSE 
                    debug_printf("Graphics Thread: No available buffer, dropping request.\n");
                #endif
                continue;
            }
        }
        
        // If the framebuffer is still in use by the VI (the switch takes time), then wait for it to become available
        while (osViGetCurrentFramebuffer() == l_freebuff->address || osViGetNextFramebuffer() == l_freebuff->address)
        {
            #if VERBOSE 
                debug_printf("Graphics Thread: Framebuffer in use by VI. Waiting for VSync.\n");
            #endif
            s_scheduler->gfx_notify = &s_msgqueue_vsync;
            osRecvMesg(&s_msgqueue_vsync, NULL, OS_MESG_BLOCK);
        }
        
        // Generate the display list for the scene
        #if VERBOSE 
            debug_printf("Graphics Thread: Found buffer '%d' at %p.\n", i, l_freebuff->address);
        #endif
        graphics_renderscene(l_freebuff, l_msg.color);
        l_freebuff->status = FBSTATUS_RENDERING;
        
        // Wait for the render task to finish
        (void)osRecvMesg(&s_msgqueue_rdp, NULL, OS_MESG_BLOCK);
        #if VERBOSE 
            debug_printf("Graphics Thread: Render task finished.\n");
        #endif
        s_scheduler->task_gfx = NULL;
        
        // If we got here, the RCP did not hang, hooray!
        #if DEBUG_MODE
            osStopTimer(&s_rcptime);
        #endif
        
        // If we're not meant to swap the framebuffer yet, then stop here
        // The next loop should reuse this framebuffer if needed
        if (!l_msg.swapbuffer)
        {
            #if VERBOSE 
                debug_printf("Graphics Thread: Don't swap buffer yet.\n");
            #endif
            continue;
        }
        l_freebuff->status = FBSTATUS_READY;
        #if VERBOSE 
            debug_printf("Graphics Thread: Framebuffer %d ready.\n", i);
        #endif
            
        // Mark the buffer as the last rendered
        s_lastrendered = l_freebuff;
        l_freebuff = NULL;
    }
}


/*==============================
    graphics_renderscene
    Creates a display list and render task for the RCP
    @param The framebuffer to use
    @param The color to wipe the screen with
==============================*/

static void graphics_renderscene(FrameBuffer* fb, u8 color)
{
    RenderTask l_task;
    
    // Initialize the render task
    l_task.displistp = fb->displist;
    l_task.framebuffer = fb->address;
    if (s_ishd)
        l_task.zbuffer = s_zbuffer_hd;
    else
        l_task.zbuffer = s_zbuffer_sd;
    #if (FRAMEBUFF_DEPTH == u16)
        l_task.bufferdepth = G_IM_SIZ_16b;
    #else
        l_task.bufferdepth = G_IM_SIZ_32b;
    #endif
    l_task.color = color;
    
    // Build the display list
    if (s_ishd)
        rcp_initialize_hd(&l_task);
    else
        rcp_initialize_sd(&l_task);
    rcp_finish(&l_task);
    
    // Let the scheduler know the RCP is going to be busy
    s_scheduler->task_gfx = l_task.task;
    
    // Start a timer to see how long it took to render. If it took too long, the RCP is hung
    #if DEBUG_MODE
        osSetTimer(&s_rcptime, OS_USEC_TO_CYCLES(SEC_TO_USEC(RCP_HUNGIME)), 0, &s_scheduler->queue, (OSMesg)MSG_SCHEDULER_RCPHANG);
    #endif
    
    // Send the render task to the RCP
    #if VERBOSE 
        debug_printf("Graphics Thread: Sending render task.\n");
    #endif
    osSpTaskStart(l_task.task);
}


/*==============================
    graphics_requestrender
    Requests a scene render from the main thread
    @param The color to wipe the screen with
    @param Whether the framebuffer should be swapped 
           when the render is finished
==============================*/

void graphics_requestrender(u8 color, bool swapbuffer)
{
    RenderMessage l_msg = {color, swapbuffer};
    osSendMesg(&s_msgqueue_graphics, (OSMesg)&l_msg, OS_MESG_BLOCK);
}


/*==============================
    graphics_framebufferready
    Checks if a framebuffer is ready to be consumed
    by the VI
    @returns Whether a framebuffer is available
==============================*/

bool graphics_framebufferready()
{
    return s_lastrendered != NULL;
}


/*==============================
    graphics_popframebuffer
    Pops the last rendered framebuffer from the queue
    of rendered buffers
    @returns The popped framebuffer
==============================*/

FrameBuffer* graphics_popframebuffer()
{
    int i;
    FrameBuffer* l_consumed = s_lastrendered;
    
    // Mark the consumed framebuffer as displaying, and remove the last rendered buffer
    l_consumed->status = FBSTATUS_DISPLAYING;
    s_lastrendered = NULL;
    
    // See if we have another framebuffer marked as ready
    for (i=0; i<s_framebuffers_activecount; i++)
    {
        if (s_framebuffers_active[i].status == FBSTATUS_READY)
        {
            s_lastrendered = &s_framebuffers_active[i];
            break;
        }
    }
            
    // Return the consumed framebuffer
    return l_consumed;
}


/*==============================
    graphics_register_fbuffer
    Registers a new framebuffer that the graphics thread can use
    @param Whether this is an HD framebuffer or not
    @param The address to register the new framebuffer to
==============================*/

void graphics_register_fbuffer(bool ishd, void* address)
{
    #if DEBUG_MODE
        int i;
    #endif
    FrameBuffer* l_targetbuffers;
    u8 l_targetcount;
    u8* l_targetcount_pointer;

    // Initialize our target variables to point to the correct globals
    if (ishd)
    {
        l_targetbuffers = s_framebuffers_hd;
        l_targetcount = s_framebuffcount_hd;
        l_targetcount_pointer = &s_framebuffcount_hd;
    }
    else
    {
        l_targetbuffers = s_framebuffers_sd;
        l_targetcount = s_framebuffcount_sd;
        l_targetcount_pointer = &s_framebuffcount_sd;
    }
    
    #if DEBUG_MODE
        // Prevent registering more framebuffers than the max
        if (l_targetcount == FRAMEBUFF_MAXCOUNT)
        {
            debug_printf("Error: attempted to register more %s framebuffers than the max!\n", (ishd ? "HD" : "SD"));
            debug_assert(FALSE);
            return;
        }
        
        // Prevent duplicate framebuffers
        for (i=0; i<l_targetcount; i++)
        {
            if (l_targetbuffers[i].address == address)
            {
                debug_printf("Error: duplicate %s framebuffer %p registered!\n", (ishd ? "HD" : "SD"), address);
                debug_assert(FALSE);
                return;
            }
        }
    #endif
    
    // Register the new framebuffer and initialize it
    (*l_targetcount_pointer)++;
    l_targetcount = (*l_targetcount_pointer) - 1;
    l_targetbuffers[l_targetcount].address = address;
    l_targetbuffers[l_targetcount].displist = g_displists[l_targetcount];
    l_targetbuffers[l_targetcount].status = FBSTATUS_FREE;
    
    // Decrease the number of active framebuffers if applicable
    if (s_framebuffers_active == l_targetbuffers)
        s_framebuffers_activecount++;
    debug_printf("Successfully registered a new %s framebuffer at address %p\n", (ishd ? "HD" : "SD"), address);
}


/*==============================
    graphics_register_zbuffer
    Registers the Z-buffer address
    @param Whether this is an HD zbuffer or not
    @param The address to register the new Z-buffer to.
           Use NULL to disable the Z-buffer
==============================*/

void graphics_register_zbuffer(bool ishd, void* address)
{
    ishd ? (s_zbuffer_hd = address) : (s_zbuffer_sd = address);
    debug_printf("Successfully registered %s Z-buffer to address %p\n", (ishd ? "HD" : "SD"), address);
}


/*==============================
    graphics_unregister_fbuffer
    Unregisters a framebuffer that the graphics thread can use
    Note: Calling this in the graphics thread may cause a stall due to
    the while loop.
    @param Whether this is an HD framebuffer or not
    @param The address to register the new framebuffer to
==============================*/

void graphics_unregister_fbuffer(bool ishd, void* address)
{
    int i;
    #if DEBUG_MODE
        bool l_found = FALSE;
    #endif
    FrameBuffer* l_targetbuffers;
    u8 l_targetcount;
    u8* l_targetcount_pointer;
    
    // Initialize our target variables to point to the correct globals
    if (ishd)
    {
        l_targetbuffers = s_framebuffers_hd;
        l_targetcount = s_framebuffcount_hd;
        l_targetcount_pointer = &s_framebuffcount_hd;
    }
    else
    {
        l_targetbuffers = s_framebuffers_sd;
        l_targetcount = s_framebuffcount_sd;
        l_targetcount_pointer = &s_framebuffcount_sd;
    }
    
    // Find the requested framebuffer
    for (i=0; i<l_targetcount; i++)
    {
        if (l_targetbuffers[i].address == address)
        {                
            // If the RDP is currently drawing to this buffer, stall until it's finished
            #if DEBUG_MODE
                if (l_targetbuffers[i].status == FBSTATUS_RENDERING)
                    debug_printf("Warning: Framebuffer to unregister in use by RCP. Stalling...\n");
            #endif
            while (l_targetbuffers[i].status == FBSTATUS_RENDERING)
                ;
                
            // If this was the last rendered framebuffer, pop the framebuffer stack
            if (s_lastrendered == &l_targetbuffers[i])
                graphics_popframebuffer();
                
            // Swap this framebuffer's data with the last one
            memcpy(&l_targetbuffers[i], &l_targetbuffers[l_targetcount-1], sizeof(FrameBuffer));
            
            // Unregister
            l_targetbuffers[l_targetcount-1].address = NULL;    
            (*l_targetcount_pointer)--;
            #if DEBUG_MODE
                l_found = TRUE;
            #endif
            break;
        }
    }
    
    // Complain if the address wasn't found
    #if DEBUG_MODE
        if (!l_found)
        {
            debug_printf("Attempted to unregister nonexistent %s framebuffer %p\n", (ishd ? "HD" : "SD"), address); 
            return;
        }
    #endif
    
    // Decrease the number of active framebuffers if applicable
    if (s_framebuffers_active == l_targetbuffers)
    {
        s_framebuffers_activecount--;
        #if DEBUG_MODE
            if (s_framebuffers_activecount < 2)
                debug_printf("Warning: Only %d %s framebuffers are registered.\n\tI hope you know what you're doing.\n", s_framebuffers_activecount, (ishd ? "HD" : "SD"));
        #endif
    }
    debug_printf("Successfully unregistered %s framebuffer at address %p.\n", (ishd ? "HD" : "SD"), address);
}


/*==============================
    graphics_set_hd
    Enables or disables the use of HD
    mode.
    @param Whether or not to enable HD mode
==============================*/

void graphics_set_hd(bool enable)
{
    int i;
    
    // Set the VI mode to 480p or 240p
    if (enable)
    {
        // If no HD framebuffers are registered, stop
        #if DEBUG_MODE
            if (s_framebuffcount_hd == 0)
            {
                debug_printf("Error: Attempted to set HD mode without any HD buffers registered\n");
                debug_assert(FALSE);
                return;
            }
        #endif
        
        // Set the VI to HD mode and initialize active globals
        #if (TVMODE == TV_NTSC)
            osViSetMode(&osViModeNtscHan1);
        #elif (TVMODE == TV_PAL)
            osViSetMode(&osViModeFpalHan1);
        #else
            osViSetMode(&osViModeMpalHan1);
        #endif
        s_framebuffers_active = s_framebuffers_hd;
        s_framebuffers_activecount = s_framebuffcount_hd;
    }
    else
    {
        // Set the VI to SD mode and initialize active globals
        #if (TVMODE == TV_NTSC)
            osViSetMode(&osViModeNtscLan1);
        #elif (TVMODE == TV_PAL)
            osViSetMode(&osViModeFpalLan1);
        #else
            osViSetMode(&osViModeMpalLan1);
        #endif
        s_framebuffers_active = s_framebuffers_sd;
        s_framebuffers_activecount = s_framebuffcount_sd;
    }
    s_ishd = enable;
    
    // Blacken the TV
    osViBlack(TRUE);
    s_scheduler->tvblack = TRUE;
    
    // Re-Initialize the framebuffers
    for (i=0; i<s_framebuffers_activecount; i++)
    {
        s_framebuffers_active[i].displist = g_displists[i];
        s_framebuffers_active[i].status = FBSTATUS_FREE;
    }
}


/*==============================
    graphics_get_screenw
    Returns the width of the screen
    @returns The screen width
==============================*/

u32 graphics_get_screenw()
{
    return s_ishd ? SCREEN_WIDTH_HD : SCREEN_WIDTH_SD;
}


/*==============================
    graphics_get_screenh
    Returns the height of the screen
    @returns The screen height
==============================*/

u32 graphics_get_screenh()
{
    return s_ishd ? SCREEN_HEIGHT_HD : SCREEN_HEIGHT_SD;
}


/*==============================
    graphics_stopthread
    Stops the graphics thread
==============================*/

void graphics_stopthread()
{
    #if DEBUG_MODE
        osStopTimer(&s_rcptime);
    #endif
    osStopThread(&s_threadstruct_graphics);
}