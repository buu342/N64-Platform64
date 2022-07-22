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
} t_gfxtask;


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_graphics(void *arg);


/*********************************
             Globals
*********************************/

static OSThread	s_threadstruct_graphics;

static OSMesgQueue s_msgqueue_graphics;
static OSMesg      s_msgbuffer_graphics[SIZE_MSGQUEUE_GRAPHICS];


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
    t_gfxtask* task;
    
    // Initialize the thread
    debug_printf("Created graphics thread\n");
    osCreateMesgQueue(&s_msgqueue_graphics, s_msgbuffer_graphics, SIZE_MSGQUEUE_GRAPHICS);
    
    // Spin this thread forever
    while (1)
    {
        // Wait for a graphics message to arrive
        osRecvMesg(&s_msgqueue_graphics, (OSMesg *)&task, OS_MESG_BLOCK);
        debug_printf("Graphics thread loop start\n");
        
        // Paint our framebuffer green
        memset(FRAMEBUFF_ADDR1_SD, task->color, FRAMEBUFF_SIZE_SD);
        
        // Display the framebuffer
        debug_printf("Framebuffer colored with %2x\n", task->color);
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
    t_gfxtask task;
    debug_printf("Requesting scene render with color %2x\n", a_color);
    
    // Specify the task for the graphics thread to execute
    task.color = a_color;
    
    // Send the task to the graphics thread
    osSendMesg(&s_msgqueue_graphics, (OSMesg *)&task, OS_MESG_BLOCK);
}