#ifndef PLATFORM64_RCP_H
#define PLATFORM64_RCP_H

    /*********************************
               Definitions
    *********************************/

    // Time (in seconds) for an RCP task to take before it's declared as hung
    #define RCP_HUNGIME  3
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void rcp_initialize_sd(RenderTask* task);
    void rcp_initialize_hd(RenderTask* task);
    void rcp_finish(RenderTask* task);

#endif