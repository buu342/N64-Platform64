#ifndef PLATFORM64_OSCONFIG_H
#define PLATFORM64_OSCONFIG_H

    // TV mode ID's
    #define TV_NTSC  0
    #define TV_PAL   1
    #define TV_MPAL  2
    
    // The selected TV mode
    #define TVMODE  TV_PAL
    
    
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

    // Main stack
    #define STACKSTART_MAIN  (STACKSTART_BOOT + STACKSIZE_BOOT)
    #define	STACKSIZE_MAIN   0x2000
    
    // Real stack starts (because the stacks grow backwards with the MIPS compiler tools)
    #define	STACKREALSTART_BOOT  (u64*)(STACKSTART_BOOT + STACKSIZE_BOOT/sizeof(u64))
    #define	STACKREALSTART_MAIN  (u64*)(STACKSTART_MAIN + STACKSIZE_MAIN/sizeof(u64))
    
    
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
    
    // Framebuffer bit depth
    #define FRAMEBUFF_DEPTH  u16
    
    // Framebuffer size
    #define FRAMEBUFF_SIZE_SD  (SCREEN_WIDTH_SD*SCREEN_WIDTH_SD*sizeof(FRAMEBUFF_DEPTH))
    #define FRAMEBUFF_SIZE_HD  (SCREEN_WIDTH_HD*SCREEN_WIDTH_HD*sizeof(FRAMEBUFF_DEPTH))
    
    // Frambuffer addresses (Low res)
    #define FRAMEBUFF_ADDR1_SD  ((FRAMEBUFF_DEPTH*)(RAMBANK_2 + RAMBANK_SIZE - (SCREEN_WIDTH_SD*SCREEN_HEIGHT_SD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDR2_SD  ((FRAMEBUFF_DEPTH*)(RAMBANK_3 + RAMBANK_SIZE - (SCREEN_WIDTH_SD*SCREEN_HEIGHT_SD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDRZ_SD  ((FRAMEBUFF_DEPTH*)(RAMBANK_4 + RAMBANK_SIZE - (SCREEN_WIDTH_SD*SCREEN_HEIGHT_SD*sizeof(FRAMEBUFF_DEPTH))))
    
    // Frambuffer addresses (High res)
    #define FRAMEBUFF_ADDR1_HD  ((FRAMEBUFF_DEPTH*)(RAMBANK_5 + RAMBANK_SIZE - (SCREEN_WIDTH_HD*SCREEN_HEIGHT_HD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDR2_HD  ((FRAMEBUFF_DEPTH*)(RAMBANK_6 + RAMBANK_SIZE - (SCREEN_WIDTH_HD*SCREEN_HEIGHT_HD*sizeof(FRAMEBUFF_DEPTH))))
    #define FRAMEBUFF_ADDRZ_HD  ((FRAMEBUFF_DEPTH*)(RAMBANK_7 + RAMBANK_SIZE - (SCREEN_WIDTH_HD*SCREEN_HEIGHT_HD*sizeof(FRAMEBUFF_DEPTH))))
    
    
    /*********************************
                 Threads
    *********************************/
    
    // Thread ID's
    #define THREADID_MAIN   1
    #define THREADID_FAULT  13 // From debug.h, included here for completion sake
    #define THREADID_USB    14 // From debug.h, included here for completion sake
    
    // Thread Priorities
    #define THREADPRI_MAIN   10
    #define THREADPRI_FAULT  125 // From debug.h, included here for completion sake
    #define THREADPRI_USB    126 // From debug.h, included here for completion sake
    
    
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
    #if (STACKSTART_MAIN%8 != 0)
        #error The main stack must be 64-bit aligned
    #endif
    
    // Ensure the stack sizes are valid
    #if (STACKSIZE_BOOT < OS_MIN_STACKSIZE)
        #error The boot stack size must be larger than OS_MIN_STACKSIZE
    #endif
    #if (STACKSIZE_MAIN < OS_MIN_STACKSIZE)
        #error The main stack size must be larger than OS_MIN_STACKSIZE
    #endif
    
#endif