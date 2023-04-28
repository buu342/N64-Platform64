#ifndef PLATFORM64_LEVELS_H
#define PLATFORM64_LEVELS_H

    typedef struct {
        bool usecpu;
        void (*render)();
    } DrawFuncs;

    typedef struct {
        void (*start)();
        void (*update)();
        void (*end)();
        u32 drawcount;
        DrawFuncs drawarray[5];
    } LevelDef;
    
    extern inline LevelDef* levels_get_all();
    
    
    /* =============================== */

    typedef enum {
        LEVEL_BOOT = 0,
    } levelid;
    
    extern void level_boot_start();
    extern void level_boot_update();
    extern void level_boot_render();
    
#endif