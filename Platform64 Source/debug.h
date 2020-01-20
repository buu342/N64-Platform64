// Enable/disable debug mode
#define DEBUG_MODE 1

#if DEBUG_MODE
    #define debug_print(a) _debug_print(a)
#else
    #define debug_print(a)
#endif