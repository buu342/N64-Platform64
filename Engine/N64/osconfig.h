#ifndef PLATFORM64_OSCONFIG_H
#define PLATFORM64_OSCONFIG_H

    // Stack sizes
    #define	BOOTSTACKSIZE 0x2000
    #define	MAINSTACKSIZE 0x2000
    
    // Stack variable declarations
    extern u64 bootStack[BOOTSTACKSIZE/sizeof(u64)];
    extern u64 mainStack[MAINSTACKSIZE/sizeof(u64)];
    
#endif