#ifndef PLATFORM64_HELPER_H
#define PLATFORM64_HELPER_H
    
    
    /*********************************
             Function Macros
    *********************************/
    
    // Time conversion
    #define SEC_TO_USEC(a)   (((f64)a)*1000000.0f)
    #define MSEC_TO_USEC(a)  (((f64)a)*1000.0f)
    #define USEC_TO_MSEC(a)  (((f64)a)/1000.0f)
    #define USEC_TO_SEC(a)   (((f64)a)/1000000.0f)
    
    
    /*********************************
            Function Prototypes
    *********************************/
    
    float clampf(float val, float min, float max);

#endif