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
    
    f32 clampf(f32 val, f32 min, f32 max);
    f32 lerpf(f32 from, f32 target, f32 fraction);
    s32 roundf(f32 val);

#endif