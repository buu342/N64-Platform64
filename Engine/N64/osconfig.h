#ifndef PLATFORM64_OSCONFIG_H
#define PLATFORM64_OSCONFIG_H

    #define	BOOTSTACKSIZE   0x2000
    extern u64 bootStack[BOOTSTACKSIZE/sizeof(u64)];
    
#endif