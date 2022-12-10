#ifndef PLATFORM64_HELPER_H
#define PLATFORM64_HELPER_H
    
    
    /*********************************
             Function Macros
    *********************************/
    
    // Time conversion
    #define SEC_TO_USEC(a)  (a*1000000)
    #define USEC_TO_SEC(a)  (((f32)a)*0.000001)
    
    
    /*********************************
            Function Prototypes
    *********************************/
    
    float clampf(float val, float min, float max);

#endif