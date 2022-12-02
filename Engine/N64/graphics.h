#ifndef PLATFORM64_GRAPHICS_H
#define PLATFORM64_GRAPHICS_H

    #include "osconfig.h"
    
    
    /*********************************
               Definitions
    *********************************/

    #define DISPLIST_SIZE  1024

    #define FBSTATUS_FREE       0
    #define FBSTATUS_RENDERING  1
    #define FBSTATUS_READY      2
    #define FBSTATUS_DISPLAYING 3
    
    
    /*********************************
                 Structs
    *********************************/

    typedef struct 
    {
        void* address;
        Gfx* displist;
        u32 status;
    } FrameBuffer;
    
    typedef struct 
    {
        Gfx*    displistp;
        void*   framebuffer;
        u32     bufferdepth;
        u8      color;
        OSTask* task;
    } RenderTask;
    
    
    /*********************************
                 Globals
    *********************************/

    extern Gfx  g_displists[FRAMEBUFF_COUNT][DISPLIST_SIZE];
    extern Gfx* g_displistp;
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void graphics_initialize(Scheduler* scheduler);
    void graphics_requestrender(u8 color, bool swapbuffer);
    bool graphics_framebufferready();
    FrameBuffer* graphics_popframebuffer();

#endif