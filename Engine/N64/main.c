/***************************************************************
                             main.c
                               
Handles the boot process of the ROM.
***************************************************************/

#include <ultra64.h>
#include "usb.h"
#include "osconfig.h"


/*********************************
        Function Prototypes
*********************************/

static void main(void *arg);


/*********************************
             Globals
*********************************/

static OSThread	mainThread;


/*==============================
    boot
    Initializes the hardware and
    prints Hello World
==============================*/

void boot()
{
    // Initialize the hardware and software
    osInitialize();

    // Start the main thread
    // This is needed, otherwise the VI manager thread will take priority and stall
    osCreateThread(&mainThread, THREADID_MAIN, main, NULL, STACKREALSTART_MAIN, THREADPRI_MAIN);
    osStartThread(&mainThread);
}


/*==============================
    main
    The main thread loop
    @param Argument passed to the thread
==============================*/
static void main(void *arg)
{
    // Initialize the debug system, and print hello world.
    usb_initialize();
    usb_write(DATATYPE_TEXT, "Hello world!\n", 13+1);
    
    // Initialize the TV
    osCreateViManager(OS_PRIORITY_VIMGR);
    #if (TVMODE == TV_NTSC)
        osViSetMode(&osViModeNtscLan1);
    #elif (TVMODE == TV_PAL)
        osViSetMode(&osViModeFpalLan1);
    #else
        osViSetMode(&osViModeMpalLan1);
    #endif
    
    // Paint our framebuffer green
    memset(FRAMEBUFF_ADDR1_SD, 0x0F, FRAMEBUFF_SIZE_SD);
    
    // Display the framebuffer
    osViSwapBuffer(FRAMEBUFF_ADDR1_SD);
    
    // Loop forever, needed or the VI will not display correctly
    while (1)
        ;
}