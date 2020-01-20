#ifndef P64_OSSTACK_H
#define P64_OSSTACK_H

    #define	STACKSIZE	0x2000

    extern u64 bootStack[STACKSIZE/sizeof(u64)];
    
#endif