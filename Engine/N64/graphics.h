#ifndef PLATFORM64_GRAPHICS_H
#define PLATFORM64_GRAPHICS_H

    /*********************************
               Definitions
    *********************************/

    // The size of a single display list
    // Increase this value if the assertion in rcp.c fails
    #define DISPLIST_SIZE  1024

    // Framebuffer status variables
    #define FBSTATUS_FREE       0
    #define FBSTATUS_RENDERING  1
    #define FBSTATUS_READY      2
    #define FBSTATUS_DISPLAYING 3
    
    
    /*********************************
                 Structs
    *********************************/

    // Struct that describes a framebuffer
    typedef struct 
    {
        void* address;
        Gfx*  displist;
        u32   status;
    } FrameBuffer;
    
    // Struct that describes a render task for the RCP
    typedef struct 
    {
        Gfx*    displistp;
        void*   framebuffer;
        void*   zbuffer;
        u32     bufferdepth;
        u8      ishd;
        void    (*func)();
        OSTask* task;
    } RenderTask;
    
    
    /*********************************
                 Globals
    *********************************/

    // Display list pointers
    extern Gfx  g_displists[FRAMEBUFF_MAXCOUNT][DISPLIST_SIZE];
    extern Gfx* g_displistp;
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void graphics_initialize(Scheduler* scheduler);
    void graphics_requestrender(void (*func)(), bool usecpu, bool swapbuffer);
    void graphics_register_fbuffer(bool ishd, void* address);
    void graphics_register_zbuffer(bool ishd, void* address);
    void graphics_unregister_fbuffer(bool ishd, void* address);
    void graphics_set_hd(bool enable);
    u32 graphics_get_screenw();
    u32 graphics_get_screenh();
    void graphics_stopthread();

#endif