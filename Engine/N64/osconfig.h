#ifndef PLATFORM64_OSCONFIG_H
#define PLATFORM64_OSCONFIG_H
    
    /*********************************
                TV Config
    *********************************/

    // TV mode ID's
    #define TV_NTSC  0
    #define TV_PAL   1
    #define TV_MPAL  2
    
    // The selected TV mode
    #define TVMODE  TV_PAL
    
    // Target framerate
    #if TVMODE == TV_PAL
        #define TARGET_FRAMERATE 25
    #else
        #define TARGET_FRAMERATE 30
    #endif
    
    
    /*********************************
                 Segments
    *********************************/
    
    // Code segments
    #define SEGMENTSTART_CODE  0x80000400
    #define SEGMENTSIZE_CODE   0x00100000
    
    // RAM Bank Addresses
    #define RAMBANK_START  0x80000000
    #define RAMBANK_SIZE   0x00100000
    #define RAMBANK_1      (RAMBANK_START + RAMBANK_SIZE*0)
    #define RAMBANK_2      (RAMBANK_START + RAMBANK_SIZE*1)
    #define RAMBANK_3      (RAMBANK_START + RAMBANK_SIZE*2)
    #define RAMBANK_4      (RAMBANK_START + RAMBANK_SIZE*3)
    #define RAMBANK_5      (RAMBANK_START + RAMBANK_SIZE*4)
    #define RAMBANK_6      (RAMBANK_START + RAMBANK_SIZE*5)
    #define RAMBANK_7      (RAMBANK_START + RAMBANK_SIZE*6)
    #define RAMBANK_8      (RAMBANK_START + RAMBANK_SIZE*7)
    
    
    /*********************************
                  Stacks
    *********************************/
    
    // Boot stack
    #define STACKSTART_BOOT  RAMBANK_2
    #define	STACKSIZE_BOOT   0x2000

    // Idle thread stack
    #define STACKSTART_IDLE  (STACKSTART_BOOT + STACKSIZE_BOOT)
    #define	STACKSIZE_IDLE   0x2000

    // Main thread stack
    #define STACKSTART_MAIN  (STACKSTART_IDLE + STACKSIZE_IDLE)
    #define	STACKSIZE_MAIN   0x2000

    // Controller thread stack
    #define STACKSTART_CONTROLLER  (STACKSTART_MAIN + STACKSIZE_MAIN)
    #define	STACKSIZE_CONTROLLER   0x2000

    // Scheduler thread stack
    #define STACKSTART_SCHEDULER  (STACKSTART_CONTROLLER + STACKSIZE_CONTROLLER)
    #define	STACKSIZE_SCHEDULER   0x2000

    // Graphics thread stack
    #define STACKSTART_GRAPHICS  (STACKSTART_SCHEDULER + STACKSIZE_SCHEDULER)
    #define	STACKSIZE_GRAPHICS   0x2000
    
    // Audio thread stack
    #define STACKSTART_AUDIO  (STACKSTART_GRAPHICS + STACKSIZE_GRAPHICS)
    #define	STACKSIZE_AUDIO   0x2000
    
    // RCP DRAM stack
    #define STACKSTART_DRAM  (STACKSTART_AUDIO + STACKSIZE_AUDIO)
    #define	STACKSIZE_DRAM   SP_DRAM_STACK_SIZE8
    
    // RDP FIFO buffer
    #define STACKSTART_RDPFIFO  (STACKSTART_DRAM + STACKSIZE_DRAM)
    #define	STACKSIZE_RDPFIFO   0x1000
    #define	STACKEND_RDPFIFO   (u64*)(STACKSTART_RDPFIFO + STACKSIZE_RDPFIFO)
    
    // Real stack starts (because the stacks grow backwards with the MIPS compiler tools)
    #define	STACKREALSTART_BOOT        (u64*)(STACKSTART_BOOT + STACKSIZE_BOOT/sizeof(u64))
    #define	STACKREALSTART_IDLE        (u64*)(STACKSTART_IDLE + STACKSIZE_IDLE/sizeof(u64))
    #define	STACKREALSTART_MAIN        (u64*)(STACKSTART_MAIN + STACKSIZE_MAIN/sizeof(u64))
    #define	STACKREALSTART_CONTROLLER  (u64*)(STACKSTART_CONTROLLER + STACKSIZE_CONTROLLER/sizeof(u64))
    #define	STACKREALSTART_SCHEDULER   (u64*)(STACKSTART_SCHEDULER + STACKSIZE_SCHEDULER/sizeof(u64))
    #define	STACKREALSTART_GRAPHICS    (u64*)(STACKSTART_GRAPHICS + STACKSIZE_GRAPHICS/sizeof(u64))
    #define	STACKREALSTART_AUDIO       (u64*)(STACKSTART_AUDIO + STACKSIZE_AUDIO/sizeof(u64))
    #define	STACKREALSTART_DRAM        (u64*)STACKSTART_DRAM
    #define	STACKREALSTART_RDPFIFO     (u64*)STACKSTART_RDPFIFO
    
    
    /*********************************
               Framebuffers
    *********************************/
    
    // The framebuffer size, based on the TV mode
    #if (TVMODE == TV_NTSC)
        #define SCREEN_WIDTH_SD   320
        #define SCREEN_HEIGHT_SD  240
        
        #define SCREEN_WIDTH_HD   640
        #define SCREEN_HEIGHT_HD  480
    #else
        #define SCREEN_WIDTH_SD   320
        #define SCREEN_HEIGHT_SD  288
        
        #define SCREEN_WIDTH_HD   640
        #define SCREEN_HEIGHT_HD  576
    #endif
    
    // Framebuffer depth and count
    #define FRAMEBUFF_DEPTH     u16
    #define FRAMEBUFF_MAXCOUNT  3
    
    // Framebuffer size
    #define FRAMEBUFF_SIZE_SD  (SCREEN_WIDTH_SD*SCREEN_WIDTH_SD*sizeof(FRAMEBUFF_DEPTH))
    #define FRAMEBUFF_SIZE_HD  (SCREEN_WIDTH_HD*SCREEN_WIDTH_HD*sizeof(FRAMEBUFF_DEPTH))
    
    // Frambuffer addresses (Low res)
    #define FRAMEBUFF_ADDR1_SD  ((FRAMEBUFF_DEPTH*)(RAMBANK_2 + RAMBANK_SIZE - (SCREEN_WIDTH_SD*SCREEN_HEIGHT_SD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDR2_SD  ((FRAMEBUFF_DEPTH*)(RAMBANK_3 + RAMBANK_SIZE - (SCREEN_WIDTH_SD*SCREEN_HEIGHT_SD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDRZ_SD  ((FRAMEBUFF_DEPTH*)(RAMBANK_4 + RAMBANK_SIZE - (SCREEN_WIDTH_SD*SCREEN_HEIGHT_SD*sizeof(FRAMEBUFF_DEPTH))))
    
    // Frambuffer addresses (High res)
    #define FRAMEBUFF_ADDR1_HD  ((FRAMEBUFF_DEPTH*)(RAMBANK_6 + RAMBANK_SIZE - (SCREEN_WIDTH_HD*SCREEN_HEIGHT_HD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDR2_HD  ((FRAMEBUFF_DEPTH*)(RAMBANK_7 + RAMBANK_SIZE - (SCREEN_WIDTH_HD*SCREEN_HEIGHT_HD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDRZ_HD  ((FRAMEBUFF_DEPTH*)(RAMBANK_8 + RAMBANK_SIZE - (SCREEN_WIDTH_HD*SCREEN_HEIGHT_HD*sizeof(FRAMEBUFF_DEPTH))))
    
    
    /*********************************
                 Threads
    *********************************/
    
    // Thread ID's
    #define THREADID_IDLE        1
    #define THREADID_MAIN        2
    #define THREADID_CONTROLLER  3
    #define THREADID_SCHEDULER   4
    #define THREADID_GRAPHICS    5
    #define THREADID_AUDIO       6
    #define THREADID_FAULT       13 // From debug.h, included here for completion sake
    #define THREADID_USB         14 // From debug.h, included here for completion sake
    
    // Thread Priorities
    #define THREADPRI_IDLE        1
    #define THREADPRI_MAIN        10
    #define THREADPRI_CONTROLLER  15
    #define THREADPRI_SCHEDULER   100
    #define THREADPRI_GRAPHICS    20
    #define THREADPRI_AUDIO       25
    #define THREADPRI_FAULT       125 // From debug.h, included here for completion sake
    #define THREADPRI_USB         126 // From debug.h, included here for completion sake
    
    
    /*********************************
                   Heap
    *********************************/
    
    #define HEAP_START  (FRAMEBUFF_ADDRZ_SD - 1)
    #define HEAP_SIZE   0x080000 // Half a megabyte of heap memory
    
    
    /*********************************
                    PI
    *********************************/
    
    #define NUM_PI_MSGS  32
    
    
    /*********************************
          Preprocessor assertions
    *********************************/
    
    // Ensure a valid TV mode was chosen
    #if (TVMODE != TV_NTSC && TVMODE != TV_PAL && TVMODE != TV_MPAL)
        #error Invalid TV mode chosen
    #endif
    
    // Ensure the stacks are properly aligned in memory
    #if (STACKSTART_BOOT%8 != 0)
        #error The boot stack must be 64-bit aligned
    #endif
    #if (STACKSTART_IDLE%8 != 0)
        #error The idle thread stack must be 64-bit aligned
    #endif
    #if (STACKSTART_MAIN%8 != 0)
        #error The main thread stack must be 64-bit aligned
    #endif
    #if (STACKSTART_CONTROLLER%8 != 0)
        #error The controller thread stack must be 64-bit aligned
    #endif
    #if (STACKSTART_SCHEDULER%8 != 0)
        #error The scheduler thread stack must be 64-bit aligned
    #endif
    #if (STACKSTART_GRAPHICS%8 != 0)
        #error The graphics thread stack must be 64-bit aligned
    #endif
    #if (STACKSTART_AUDIO%8 != 0)
        #error The audio thread stack must be 64-bit aligned
    #endif
    #if (STACKSTART_DRAM%16 != 0)
        #error The DRAM stack must be 16-byte aligned
    #endif
    #if (STACKSTART_RDPFIFO%16 != 0)
        #error The RDP FIFO buffer must be 16-byte aligned
    #endif
    
    // Ensure the stack sizes are valid
    #if (STACKSIZE_BOOT < OS_MIN_STACKSIZE)
        #error The boot stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_IDLE < OS_MIN_STACKSIZE)
        #error The idle thread stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_MAIN < OS_MIN_STACKSIZE)
        #error The main thread stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_CONTROLLER < OS_MIN_STACKSIZE)
        #error The controller thread stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_SCHEDULER < OS_MIN_STACKSIZE)
        #error The scheduler thread stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_GRAPHICS < OS_MIN_STACKSIZE)
        #error The graphics thread stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_AUDIO < OS_MIN_STACKSIZE)
        #error The audio thread stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_DRAM < SP_DRAM_STACK_SIZE8)
        #error The dram stack size must be larger than SP_DRAM_STACK_SIZE8
    #endif
    #if (STACKSIZE_RDPFIFO < 0x100)
        #error The RDP FIFO buffer size must be larger than 256 bytes
    #endif
    
#endif