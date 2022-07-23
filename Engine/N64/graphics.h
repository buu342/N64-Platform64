#ifndef PLATFORM64_GRAPHICS_H
#define PLATFORM64_GRAPHICS_H

    #include "osconfig.h"
    
    
    /*********************************
               Definitions
    *********************************/

    #define DISPLIST_SIZE  1024
    
    
    /*********************************
                 Structs
    *********************************/
    
    typedef struct 
    {
        Gfx*  displistp;
        void* framebuffer;
        u32   bufferdepth;
        u8    color;
    } RenderTask;
    
    
    /*********************************
                 Globals
    *********************************/

    extern Gfx  g_displists[FRAMEBUFF_COUNT][DISPLIST_SIZE];
    extern Gfx* g_displistp;
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void graphics_initialize();
    void graphics_renderscene(const u8 color);

#endif