#ifndef PLATFORM64_GRAPHICS_H
#define PLATFORM64_GRAPHICS_H

    // TV mode ID's
    #define TV_NTSC 0
    #define TV_PAL  1
    #define TV_MPAL 2
    
    // The selected TV mode
    #define TVMODE TV_PAL
    
    // The framebuffer size, based on the TV mode
    #if (TVMODE == TV_NTSC)
        #define SCREEN_WIDTH_SD  320
        #define SCREEN_HEIGHT_SD 240
        
        #define SCREEN_WIDTH_HD  640
        #define SCREEN_HEIGHT_HD 480
    #else
        #define SCREEN_WIDTH_SD  320
        #define SCREEN_HEIGHT_SD 288
        
        #define SCREEN_WIDTH_HD  640
        #define SCREEN_HEIGHT_HD 576
    #endif
    
#endif