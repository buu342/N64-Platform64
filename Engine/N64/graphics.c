/***************************************************************
                           graphics.c
                               
Handles graphics rendering
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "graphics.h"

/*********************************
           Definitions
*********************************/

#define SIZE_MSGQUEUE_GRAPHICS  8
    
    
/*********************************
             Structs
*********************************/

typedef struct 
{
    u8 color;
} ThreadMsg_Graphics;


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_graphics(void *arg);


/*********************************
             Globals
*********************************/

// Thread
static OSThread    s_threadstruct_graphics;

// Message queue
static OSMesgQueue s_msgqueue_graphics;
static OSMesg      s_msgbuffer_graphics[SIZE_MSGQUEUE_GRAPHICS];
static OSMesgQueue s_msgqueue_rdp;
static OSMesg      s_msgbuffer_rdp;
static OSMesg      s_msgbuffer_dummy;

// RCP task
static OSTask s_rcptask_render =
{
    M_GFXTASK,              // Task type
    0,                      // Task flags
    NULL,                   // RCP boot microcode pointer (fill in later)
    0,                      // RCP boot microcode size (fill in later)
    NULL,                   // Task microcode pointer (fill in later)
    SP_UCODE_SIZE,          // Task microcode size
    NULL,                   // Task microcode data pointer (fill in later)
    SP_UCODE_DATA_SIZE,     // Task microcode data size
    STACKREALSTART_DRAM,    // Task DRAM stack pointer
    STACKSIZE_DRAM,         // Task DRAM stack size
    STACKREALSTART_RDPFIFO, // Task FIFO buffer start pointer
    STACKEND_RDPFIFO,       // Task FIFO buffer end pointer
    NULL,                   // Task data pointer (fill in later)
    0,                      // Task data size (fill in later)
    NULL,                   // Task yield buffer pointer (unused)
    0                       // Task yield buffer size (unused)
};


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
    ThreadMsg_Graphics* task;
    
    // Initialize the thread
    debug_printf("Created graphics thread\n");
    osCreateMesgQueue(&s_msgqueue_graphics, s_msgbuffer_graphics, SIZE_MSGQUEUE_GRAPHICS);
    osCreateMesgQueue(&s_msgqueue_rdp, &s_msgbuffer_rdp, 1);
    osSetEventMesg(OS_EVENT_DP, &s_msgqueue_rdp, s_msgbuffer_dummy);
    
    // Spin this thread forever
    while (1)
    {
        RenderTask render;
        
        // Wait for a graphics message to arrive
        osRecvMesg(&s_msgqueue_graphics, (OSMesg *)&task, OS_MESG_BLOCK);
        debug_printf("Graphics thread loop start\n");
        
        // Initialize our render task
        render.displistp = g_displists[0];
        render.framebuffer = FRAMEBUFF_ADDR1_SD;
        #if (FRAMEBUFF_DEPTH == u16)
            render.bufferdepth = G_IM_SIZ_16b;
        #else
            render.bufferdepth = G_IM_SIZ_32b;
        #endif
        render.color = task->color;
    
        // Use the RCP to render to the framebuffer
        rcp_initialize_sd(&render);
        rcp_finish(&render);
        
        // Setup the RCP task and send it
        s_rcptask_render.t.ucode_boot      = (u64*)rspbootTextStart;
        s_rcptask_render.t.ucode_boot_size = (u32)rspbootTextEnd-(u32)rspbootTextStart;
        s_rcptask_render.t.ucode           = (u64*)gspF3DEX2_fifoTextStart;
        s_rcptask_render.t.ucode_data      = (u64*)gspF3DEX2_fifoDataStart;
        s_rcptask_render.t.data_ptr        = (u64*)g_displists[0];
        s_rcptask_render.t.data_size       = (u32)((g_displistp - render.displistp)*sizeof(Gfx));
        osSpTaskStart(&s_rcptask_render);
        
        // Wait for the RCP to finish
        debug_printf("\tSending render task\n");
        (void)osRecvMesg(&s_msgqueue_rdp, NULL, OS_MESG_BLOCK);
        debug_printf("\tRDP finished task\n");
        
        // Display the framebuffer
        debug_printf("\tFramebuffer colored with %2x\n", task->color);
        osViSwapBuffer(FRAMEBUFF_ADDR1_SD);
    }
}


/*==============================
    graphics_renderscene
    Queues a scene render
    @param The color to set the framebuffer
==============================*/

void graphics_renderscene(const u8 a_color)
{
    ThreadMsg_Graphics task;
    debug_printf("Requesting scene render with color %2x\n", a_color);
    
    // Specify the task for the graphics thread to execute
    task.color = a_color;
    
    // Send the task to the graphics thread
    osSendMesg(&s_msgqueue_graphics, (OSMesg *)&task, OS_MESG_BLOCK);
}